#!/usr/bin/env python3
"""Measure how the load time of one rule grows with its size in the tau binary.

Two rule shapes, both made of keyed branches that compare an amount (i1)
against thresholds and require a key on i18, i19, ...:

  chain N   N nested branches, (i12 = id) ? clause : rest, each clause a flat
            3-term condition (3N terms in all).
  terms K   a single branch whose clause is a flat disjunction of K terms.

Each rule is loaded through one of three paths, in a fresh tau process:

  spec      the rule is the boot file.  Time = wall time from launch to the
            "Execution step: 0" prompt.
  revision  the binary boots a small router; the base rule is typed at i0 on
            step 0 and the rule on step 1.  Time = wall time between the
            "Execution step: 1" and "Execution step: 2" prompts.
  reparse   the "Updated specification" text that the revision printed is
            booted as a spec.  Time = as for spec.

After loading, the rule is run on a few inputs and o5 is compared with the
expected answer (0 without the key, 1 with it); a wrong answer is reported
loudly.  Needs Python 3.8+ (standard library only), ps, and the tau binary.

Examples:
  python3 tau_dispatch_blowup.py --tau /path/to/tau chain 34
  python3 tau_dispatch_blowup.py terms 17 --path revision
  python3 tau_dispatch_blowup.py sweep --out results.csv
"""

import argparse
import csv
import os
import platform
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass, field
from typing import Callable, Dict, List, Optional, Tuple

AMOUNT, ID_STREAM, KEY_STREAM = 1, 12, 18
CHAIN_THRESHOLDS = (1000, 10000, 100000)
BASE_RULE = "always ( o1[t]:bv[24] = i1[t]:bv[24] ).\n"
ROUTER = "((!(i0[t] = 0)) ? ( u[t] = i0[t] && o0[t] = 0 ) : o0[t] = 1).\n"
FIRST_PROMPT = "Execution step: 0"
POLL_SECONDS = 0.5

Const = Tuple[int, int, int]  # (small id, 384-bit tag byte, 384-bit counter)


# ---------------------------------------------------------------- rule text

def num24(v: int) -> str:
    return "{ #x%06x }:bv[24]" % v


class Ids:
    """Renders ids and keys as small integers (W=8) or 384-bit constants (W=384)."""

    def __init__(self, width: int, largest: int) -> None:
        self.width = width
        # bv[8] holds ids up to 254; a larger id space needs bv[16].
        self.bits = 384 if width == 384 else (8 if largest <= 254 else 16)

    def lit(self, c: Const) -> str:
        small, tag, count = c
        if self.width == 384:
            return "#x" + ("%02x%04x" % (tag, count)) * 16
        return str(small)

    def text(self, c: Const) -> str:
        return "{ %s }:bv[%d]" % (self.lit(c), self.bits)


@dataclass
class Branch:
    ident: Const
    keys: List[Const]       # keys[j] and thresholds[j] make up the term on stream KEY_STREAM + j
    thresholds: List[int]


def chain_branch(a: int) -> Branch:
    base = 4 * (a - 1) + 1
    return Branch((base, 0xAB, a),
                  [(base + 1 + j, 0xCA + j, a) for j in range(3)],
                  list(CHAIN_THRESHOLDS))


def terms_branch(k: int) -> Branch:
    return Branch((1, 0xAB, 1),
                  [(j + 2, 0xCD, j + 1) for j in range(k)],
                  [1000 * (j + 1) for j in range(k)])


def clause(ids: Ids, br: Branch) -> str:
    terms = ["(i1[t]:bv[24] > %s && !(i%d[t]:bv[%d] = %s))"
             % (num24(br.thresholds[j]), KEY_STREAM + j, ids.bits, ids.text(br.keys[j]))
             for j in reversed(range(len(br.keys)))]
    return "( ( %s ) ? (o5[t]:bv[24] = %s) : (o5[t]:bv[24] = %s) )" % (
        " || ".join(terms), num24(0), num24(1))


def nested(ids: Ids, branches: List[Branch]) -> str:
    rest = "(o5[t]:bv[24] = %s)" % num24(1)
    for br in reversed(branches):
        rest = "((i12[t]:bv[%d] = %s) ? ( %s ) : %s)" % (
            ids.bits, ids.text(br.ident), clause(ids, br), rest)
    return rest


@dataclass
class Check:
    label: str
    expect: int
    values: Dict[int, str]  # stdin value per stream; unlisted streams read 0


@dataclass
class Workload:
    bits: int
    streams: List[int]      # input streams in prompt order
    spec: str               # boot file of the spec path
    rule: str               # rule typed at i0 on the revision path
    checks: List[Check]


def key_checks(ids: Ids, br: Branch, name: str) -> List[Check]:
    bare = {AMOUNT: "1001", ID_STREAM: ids.lit(br.ident)}
    keyed = dict(bare)
    keyed[KEY_STREAM] = ids.lit(br.keys[0])
    return [Check("%s, no key" % name, 0, bare), Check("%s, key on i18" % name, 1, keyed)]


def chain_workload(n: int, width: int, live_only: bool) -> Workload:
    ids = Ids(width, 4 * n)
    shown = [n] if live_only else list(range(1, n + 1))
    body = nested(ids, [chain_branch(a) for a in shown])
    picked = shown if (live_only or n <= 8) else sorted({1, (n + 1) // 2, n})
    checks: List[Check] = []
    for a in picked:
        checks += key_checks(ids, chain_branch(a), "branch %d" % a)
    return Workload(
        bits=ids.bits,
        streams=[AMOUNT, ID_STREAM, 18, 19, 20],
        spec="always ( (o1[t]:bv[24] = i1[t]:bv[24]) && (%s) ).\n" % body,
        rule="always ( %s ).\n" % body,
        checks=checks)


def terms_workload(k: int, width: int) -> Workload:
    ids = Ids(width, k + 1)
    br = terms_branch(k)
    body = nested(ids, [br])
    base = {AMOUNT: str(1000 * k + 1), ID_STREAM: ids.lit(br.ident)}
    every = dict(base)
    every.update({KEY_STREAM + j: ids.lit(br.keys[j]) for j in range(k)})
    short = dict(every)
    del short[KEY_STREAM + k - 1]
    return Workload(
        bits=ids.bits,
        streams=[AMOUNT, ID_STREAM] + [KEY_STREAM + j for j in range(k)],
        spec="always ( (o1[t]:bv[24] = i1[t]:bv[24]) && (%s) ).\n" % body,
        rule="always ( %s ).\n" % body,
        checks=[Check("no keys", 0, base),
                Check("all %d keys" % k, 1, every),
                Check("all but the last key", 0, short)])


def check_lines(wl: Workload, with_i0: bool) -> List[str]:
    lines: List[str] = []
    for c in wl.checks:
        if with_i0:
            lines.append("F")
        lines += [c.values.get(s, "0") for s in wl.streams]
    return lines


def stdin_spec(wl: Workload) -> str:
    return "\n".join(check_lines(wl, False)) + "\n"


def stdin_router(wl: Workload) -> str:
    head = [BASE_RULE.rstrip("\n"), wl.rule.rstrip("\n"), "0"]
    return "\n".join(head + check_lines(wl, True)) + "\n"


def stdin_reparse(wl: Workload) -> str:
    return "\n".join(check_lines(wl, True)) + "\n"


# ------------------------------------------------------------------- points

@dataclass(frozen=True)
class Point:
    kind: str            # "chain" or "terms"
    size: int            # N for chain, k for terms
    path: str            # "spec", "revision" or "reparse"
    width: int
    live_only: bool = False
    kills: bool = False  # expected to end in a kill; skipped by --quick


def workload(pt: Point) -> Workload:
    if pt.kind == "chain":
        return chain_workload(pt.size, pt.width, pt.live_only)
    return terms_workload(pt.size, pt.width)


def sym(kind: str) -> str:
    return "N" if kind == "chain" else "k"


def describe(pt: Point) -> str:
    return "%s %s=%d %s W=%d%s" % (pt.kind, sym(pt.kind), pt.size, pt.path, pt.width,
                                   " live-only" if pt.live_only else "")


def slug(pt: Point) -> str:
    return "%s-%s%d-%s-w%d%s" % (pt.kind, sym(pt.kind).lower(), pt.size, pt.path, pt.width,
                                 "-live" if pt.live_only else "")


# ------------------------------------------------------------- running tau

@dataclass
class Ctx:
    tau: str
    cap: float
    rss_cap: int
    keep: Optional[str]


@dataclass
class Run:
    status: str = "error"   # ok, cap, rss or error
    wall: float = 0.0
    rss_mb: float = 0.0
    out: str = ""
    err: str = ""
    marker_s: Optional[float] = None
    marks: Dict[int, float] = field(default_factory=dict)
    note: str = ""


ANSI = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
TIMING = re.compile(r"^\s*([A-Za-z_][A-Za-z_ ]*):\s*([0-9.]+(?:[eE][+-]?[0-9]+)?)\s*ms\s*$", re.M)
O5_LINE = re.compile(r"\bo5\[(\d+)\]\s*:=\s*(\S+)")
STEP_MARK = re.compile(r"Execution step: (\d+)")
UPDATED = re.compile(r"^Updated specification(?: \(\d+ chars\))?: (.*)$", re.M)


def read_text(path: str) -> str:
    try:
        with open(path, "rb") as f:
            return ANSI.sub("", f.read().decode("utf-8", "replace"))
    except OSError:
        return ""


def rss_kb(pid: int) -> int:
    try:
        res = subprocess.run(["ps", "-o", "rss=", "-p", str(pid)], stdout=subprocess.PIPE,
                             stderr=subprocess.DEVNULL, universal_newlines=True, timeout=10)
        return int(res.stdout.split()[0])
    except (OSError, subprocess.SubprocessError, ValueError, IndexError):
        return 0


def exit_code(status: int) -> int:
    return -os.WTERMSIG(status) if os.WIFSIGNALED(status) else os.WEXITSTATUS(status)


def execute(ctx: Ctx, boot: str, stdin: str, out: str, err: str, cwd: str) -> Run:
    run = Run()
    try:
        with open(stdin, "rb") as fi, open(out, "wb") as fo, open(err, "wb") as fe:
            t0 = time.monotonic()
            # The Popen object stays referenced: its cleanup would otherwise reap the child
            # before the wait4 below, which is where the exit status and peak memory come from.
            proc = subprocess.Popen([ctx.tau, boot], stdin=fi, stdout=fo, stderr=fe, cwd=cwd)
    except OSError as exc:
        run.note = "cannot start tau: %s" % exc.strerror
        return run
    pid = proc.pid
    reaped = False
    killed = None
    peak_kb = 0
    out_size = -1
    next_poll = t0 + POLL_SECONDS
    status, usage = 0, None
    try:
        while True:
            done, status, usage = os.wait4(pid, os.WNOHANG)
            if done:
                reaped = True
                run.wall = time.monotonic() - t0
                break
            now = time.monotonic()
            # Step markers are the only timing both binary generations print, so stamp each on arrival.
            size = os.path.getsize(out)
            if size != out_size:
                out_size = size
                for m in STEP_MARK.finditer(read_text(out)):
                    run.marks.setdefault(int(m.group(1)), now - t0)
                if 0 in run.marks:
                    run.marker_s = run.marks[0]
            if now >= next_poll:
                kb = rss_kb(pid)
                peak_kb = max(peak_kb, kb)
                next_poll = now + POLL_SECONDS
                if kb > ctx.rss_cap * 1024:
                    killed = "rss"
            if killed is None and now - t0 > ctx.cap:
                killed = "cap"
            if killed:
                os.kill(pid, signal.SIGKILL)
                _, status, usage = os.wait4(pid, 0)
                reaped = True
                run.wall = time.monotonic() - t0
                break
            time.sleep(0.003)
    finally:
        if not reaped:
            try:
                os.kill(pid, signal.SIGKILL)
                os.wait4(pid, 0)
            except OSError:
                pass
        proc.returncode = exit_code(status)
    # ru_maxrss is in bytes on macOS and in kilobytes elsewhere.
    unit = 1024.0 * 1024.0 if sys.platform == "darwin" else 1024.0
    run.rss_mb = max(peak_kb / 1024.0, usage.ru_maxrss / unit)
    run.out, run.err = read_text(out), read_text(err)
    if killed:
        run.status = killed
        run.note = "killed at the %s" % ("time cap" if killed == "cap" else "memory guard")
    elif os.WIFSIGNALED(status):
        run.note = "terminated by signal %d" % os.WTERMSIG(status)
    elif exit_code(status) != 0:
        first = next((ln.strip() for ln in run.out.splitlines() if ln.strip()), "")
        run.note = "exit status %d%s" % (exit_code(status), ": " + first[:120] if first else "")
    else:
        run.status = "ok"
    return run


def timings(run: Run) -> Dict[str, List[float]]:
    """Seconds per label from the binary's timing lines, if it prints any."""
    found: Dict[str, List[float]] = {}
    for text in (run.err, run.out):
        for m in TIMING.finditer(text):
            found.setdefault(m.group(1).strip(), []).append(float(m.group(2)) / 1000.0)
        if "run" in found:
            break
    return found


def o5_values(out: str) -> Dict[int, Optional[int]]:
    seen: Dict[int, Optional[int]] = {}
    for m in O5_LINE.finditer(out):
        tok = m.group(2)
        try:
            val: Optional[int] = int(tok[2:], 16) if tok.startswith("#x") else int(tok)
        except ValueError:
            val = None
        seen.setdefault(int(m.group(1)), val)
    return seen


# ----------------------------------------------------------------- results

@dataclass
class Result:
    point: Point
    bits: int
    status: str = "error"             # ok, cap, rss or error
    time_s: Optional[float] = None    # build time, revision time or rebuild time
    revision_s: Optional[float] = None
    rss_mb: float = 0.0
    ok: Optional[int] = None
    total: Optional[int] = None
    problems: List[str] = field(default_factory=list)
    wall_s: float = 0.0
    killed_after: float = 0.0
    fallback: bool = False            # time_s is the wall time to the first prompt
    note: str = ""
    load1: float = 0.0


class Workdir:
    def __init__(self, ctx: Ctx, name: str) -> None:
        self.temp = ctx.keep is None
        if ctx.keep is None:
            self.path = tempfile.mkdtemp(prefix="tau_blowup_")
        else:
            self.path = os.path.join(ctx.keep, name)
            os.makedirs(self.path, exist_ok=True)

    def __enter__(self) -> "Workdir":
        return self

    def __exit__(self, *exc: object) -> None:
        if self.temp:
            shutil.rmtree(self.path, ignore_errors=True)

    def file(self, name: str) -> str:
        return os.path.join(self.path, name)

    def launch(self, ctx: Ctx, boot: str, stdin: str, stem: str = "") -> Run:
        for name, text in (("boot.tau", boot), ("stdin.txt", stdin)):
            with open(self.file(stem + name), "w") as f:
                f.write(text)
        return execute(ctx, self.file(stem + "boot.tau"), self.file(stem + "stdin.txt"),
                       self.file(stem + "stdout.txt"), self.file(stem + "stderr.txt"), self.path)


def adopt(res: Result, run: Run) -> bool:
    """Fold a process into the result; False when it did not finish normally."""
    res.wall_s += run.wall
    res.rss_mb = max(res.rss_mb, run.rss_mb)
    if run.status != "ok":
        res.status, res.killed_after, res.note = run.status, run.wall, run.note
    return run.status == "ok"


def grade(res: Result, wl: Workload, out: str, first_step: int, tag: str) -> None:
    seen = o5_values(out)
    passed = 0
    for i, c in enumerate(wl.checks):
        got = seen.get(first_step + i)
        if got == c.expect:
            passed += 1
        else:
            res.problems.append("%scheck %d (%s): expected o5 = %d, got %s" % (
                tag, i + 1, c.label, c.expect,
                got if first_step + i in seen else "no o5 output"))
    res.ok = (res.ok or 0) + passed
    res.total = (res.total or 0) + len(wl.checks)


def measure_spec(res: Result, wl: Workload, wd: Workdir, ctx: Ctx) -> None:
    run = wd.launch(ctx, wl.spec, stdin_spec(wl))
    if not adopt(res, run):
        return
    build = run.marks.get(0)
    res.fallback = True
    if build is None:
        res.note = "no Execution step marker seen"
        return
    res.time_s = build
    grade(res, wl, run.out, 0, "")
    res.status = "ok"


def revision_run(res: Result, wl: Workload, wd: Workdir, ctx: Ctx, tag: str) -> Optional[Run]:
    run = wd.launch(ctx, ROUTER, stdin_router(wl))
    if not adopt(res, run):
        return None
    if 1 not in run.marks or 2 not in run.marks:
        res.note = "step 1/2 markers not seen"
        return None
    res.revision_s = run.marks[2] - run.marks[1]
    grade(res, wl, run.out, 2, tag)
    return run


def measure_revision(res: Result, wl: Workload, wd: Workdir, ctx: Ctx) -> None:
    if revision_run(res, wl, wd, ctx, "") is None:
        return
    res.time_s, res.revision_s = res.revision_s, None
    res.status = "ok"


def measure_reparse(res: Result, wl: Workload, wd: Workdir, ctx: Ctx) -> None:
    run = revision_run(res, wl, wd, ctx, "revision: ")
    if run is None:
        return
    printed = UPDATED.findall(run.out)
    if not printed:
        res.note = "the binary printed no Updated specification text"
        return
    again = wd.launch(ctx, printed[-1] + ".\n", stdin_reparse(wl), "reparsed.")
    if not adopt(res, again):
        return
    build = again.marks.get(0)
    if build is None:
        res.note = "no Execution step marker for the reparsed text"
        return
    res.time_s = build
    grade(res, wl, again.out, 0, "reparsed: ")
    res.status = "ok"


def measure(pt: Point, ctx: Ctx) -> Result:
    wl = workload(pt)
    res = Result(point=pt, bits=wl.bits, load1=os.getloadavg()[0])
    handler = {"spec": measure_spec, "revision": measure_revision,
               "reparse": measure_reparse}[pt.path]
    with Workdir(ctx, slug(pt)) as wd:
        handler(res, wl, wd, ctx)
    return res


# --------------------------------------------------------------- reporting

def fmt_s(x: float) -> str:
    if x < 1:
        return "%.3f" % x
    return ("%.2f" if x < 10 else "%.1f") % x


def secs(x: Optional[float]) -> str:
    return "-" if x is None else "%s s" % fmt_s(x)


def kill_cell(r: Result, ctx: Ctx) -> str:
    if r.status == "cap":
        return "> %g s" % ctx.cap
    if r.status == "rss":
        return "> %s s (killed)" % fmt_s(r.killed_after)
    return "error"


def time_cells(r: Result, ctx: Ctx, reparse: bool) -> List[str]:
    if r.status == "ok":
        if reparse:
            return [secs(r.revision_s), secs(r.time_s)]
        return [secs(r.time_s)]
    if not reparse:
        return [kill_cell(r, ctx)]
    if r.revision_s is not None:
        return [secs(r.revision_s), kill_cell(r, ctx)]
    return [kill_cell(r, ctx), "-"]


def rss_cell(r: Result, ctx: Ctx) -> str:
    return "> %d MB" % ctx.rss_cap if r.status == "rss" else "%d MB" % round(r.rss_mb)


def verdict_text(r: Result) -> str:
    if r.ok is None or r.total is None:
        return "-"
    if r.problems:
        return "WRONG %d/%d" % (r.ok, r.total)
    return "%d/%d ok" % (r.ok, r.total)


def verdict_cell(r: Result) -> str:
    return "**%s**" % verdict_text(r) if r.problems else verdict_text(r)


def size_cell(r: Result) -> str:
    pt = r.point
    bits = " (bv[%d])" % r.bits if pt.width == 8 and r.bits != 8 else ""
    return "%d%s" % (pt.size, bits)


@dataclass
class Group:
    title: str
    heads: List[str]
    lead: Callable[[Result], List[str]]
    time_head: str
    points: List[Point]


def render(group: Group, results: List[Result], ctx: Ctx) -> str:
    reparse = any(r.point.path == "reparse" for r in results)
    times = ["revision time", "rebuild time"] if reparse else [group.time_head]
    heads = group.heads + times + ["peak RSS", "verdicts"]
    align = [":---"] * len(group.heads) + ["---:"] * (len(heads) - len(group.heads))
    lines = ["### " + group.title, "", "| " + " | ".join(heads) + " |", "| " + " | ".join(align) + " |"]
    for r in results:
        row = group.lead(r) + time_cells(r, ctx, reparse) + [rss_cell(r, ctx), verdict_cell(r)]
        lines.append("| " + " | ".join(row) + " |")
    return "\n".join(lines) + "\n"


def one_group(title: str, kind: str, path: str, pts: List[Point]) -> Group:
    head = {"spec": "build time", "revision": "revision time", "reparse": "rebuild time"}[path]
    return Group(title, [sym(kind)], lambda r: [size_cell(r)], head, pts)


def sweep_groups() -> List[Group]:
    def many(kind: str, path: str, values: Tuple[int, ...], width: int = 8,
             kills: Tuple[int, ...] = ()) -> List[Point]:
        return [Point(kind, v, path, width, kills=v in kills) for v in values]

    live = [Point("chain", 34, "spec", 8, True), Point("chain", 64, "spec", 8, True),
            Point("chain", 32, "revision", 8, True), Point("chain", 64, "revision", 8, True)]
    wide = many("chain", "spec", (1, 8, 32), 384) + many("terms", "spec", (1, 8, 16), 384)
    return [
        one_group("chain, spec path, W=8", "chain", "spec",
                  many("chain", "spec", (1, 2, 4, 8, 16, 32, 33, 34, 36, 40, 48), kills=(48,))),
        one_group("chain, revision path, W=8", "chain", "revision",
                  many("chain", "revision", (1, 2, 4, 8, 16, 24, 29, 30, 32))),
        Group("chain, live branch only, W=8", ["path", "N"],
              lambda r: [r.point.path, size_cell(r)], "time", live),
        one_group("terms, spec path, W=8", "terms", "spec",
                  many("terms", "spec", (1, 2, 4, 8, 12, 14, 15, 16, 17, 18, 20), kills=(20,))),
        one_group("terms, revision path, W=8", "terms", "revision",
                  many("terms", "revision", (1, 4, 8, 12, 14, 15, 16))),
        Group("W=384, spec path", ["rule"],
              lambda r: ["%s %s=%d" % (r.point.kind, sym(r.point.kind), r.point.size)],
              "build time", wide),
    ]


def plan_text() -> str:
    lines = ["points (* = ends in a kill, skipped by --quick):"]
    for g in sweep_groups():
        cells = []
        for p in g.points:
            label = ("%s %d" % (p.path, p.size)) if "path" in g.heads else (
                "%s %d" % (p.kind, p.size) if g.heads == ["rule"] else str(p.size))
            cells.append(label + ("*" if p.kills else ""))
        lines.append("  %s: %s" % (g.title, ", ".join(cells)))
    return "\n".join(lines)


def sysctl(name: str) -> str:
    try:
        return subprocess.run(["sysctl", "-n", name], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                              universal_newlines=True, timeout=10).stdout.strip()
    except (OSError, subprocess.SubprocessError):
        return ""


def describe_os() -> str:
    if sys.platform == "darwin":
        return "macOS %s (Darwin %s, %s)" % (platform.mac_ver()[0], platform.release(), platform.machine())
    pretty = ""
    try:
        with open("/etc/os-release") as f:
            for line in f:
                if line.startswith("PRETTY_NAME="):
                    pretty = line.split("=", 1)[1].strip().strip('"')
    except OSError:
        pass
    return "%s (%s %s, %s)" % (pretty or platform.system(), platform.system(), platform.release(),
                               platform.machine())


def describe_cpu() -> str:
    name = ""
    if sys.platform == "darwin":
        name = sysctl("machdep.cpu.brand_string")
    else:
        try:
            with open("/proc/cpuinfo") as f:
                for line in f:
                    if line.startswith("model name"):
                        name = line.split(":", 1)[1].strip()
                        break
        except OSError:
            pass
    return "%s, %d cores" % (name or platform.processor() or platform.machine(), os.cpu_count() or 0)


def describe_memory() -> str:
    nbytes = 0
    try:
        if sys.platform == "darwin":
            nbytes = int(sysctl("hw.memsize") or 0)
        else:
            with open("/proc/meminfo") as f:
                for line in f:
                    if line.startswith("MemTotal:"):
                        nbytes = int(line.split()[1]) * 1024
                        break
    except (OSError, ValueError):
        pass
    return "%d GB" % round(nbytes / float(1 << 30)) if nbytes else "unknown"


def tau_version(tau: str) -> str:
    try:
        res = subprocess.run([tau, "--version"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                             universal_newlines=True, timeout=60)
        return next((ln.strip() for ln in res.stdout.splitlines() if ln.strip()), "unknown")
    except (OSError, subprocess.SubprocessError):
        return "unknown"


def env_stamp(tau: str) -> str:
    return "tau: %s | OS: %s | CPU: %s | memory: %s | Python: %s" % (
        tau_version(tau), describe_os(), describe_cpu(), describe_memory(), platform.python_version())


CSV_COLUMNS = ["kind", "size", "path", "width", "bv_bits", "live_only", "status", "time_s",
               "revision_s", "peak_rss_mb", "verdicts_ok", "verdicts_total", "wall_s", "load1", "note"]


class CsvSink:
    def __init__(self, path: Optional[str]) -> None:
        self.file = None
        self.writer = None
        if path:
            os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
            self.file = open(path, "w", newline="")
            self.writer = csv.writer(self.file)
            self.writer.writerow(CSV_COLUMNS)

    def add(self, r: Result) -> None:
        if self.writer is None:
            return

        def num(v: Optional[float], digits: int) -> str:
            return "" if v is None else str(round(v, digits))

        pt = r.point
        self.writer.writerow([
            pt.kind, pt.size, pt.path, pt.width, r.bits, int(pt.live_only), r.status,
            num(r.time_s, 4), num(r.revision_s, 4), num(r.rss_mb, 1),
            "" if r.ok is None else r.ok, "" if r.total is None else r.total,
            num(r.wall_s, 2), num(r.load1, 2), r.note])
        self.file.flush()

    def close(self) -> None:
        if self.file:
            self.file.close()


def progress(text: str) -> None:
    sys.stderr.write(text)
    sys.stderr.flush()


def summary(r: Result, ctx: Ctx) -> str:
    if r.status == "ok":
        if r.point.path == "reparse":
            times = "revision %s, rebuild %s" % (secs(r.revision_s), secs(r.time_s))
        else:
            times = secs(r.time_s)
        return "%s, %d MB, verdicts %s" % (times, round(r.rss_mb), verdict_text(r))
    if r.status == "cap":
        return "%s (cap), %d MB" % (kill_cell(r, ctx), round(r.rss_mb))
    if r.status == "rss":
        return "%s (killed after %s)" % (rss_cell(r, ctx), secs(r.killed_after))
    return "ERROR: %s" % r.note


def run_points(groups: List[Group], ctx: Ctx, quick: bool, sink: CsvSink
               ) -> Tuple[List[Tuple[Group, List[Result]]], bool]:
    plan = [[p for p in g.points if not (quick and p.kills)] for g in groups]
    total = sum(len(p) for p in plan)
    done: List[Tuple[Group, List[Result]]] = []
    index = 0
    try:
        for g, pts in zip(groups, plan):
            results: List[Result] = []
            done.append((g, results))
            for pt in pts:
                index += 1
                progress("[%2d/%d] %s ... " % (index, total, describe(pt)))
                try:
                    r = measure(pt, ctx)
                except Exception as exc:
                    r = Result(point=pt, bits=0, note="%s: %s" % (type(exc).__name__, exc))
                results.append(r)
                sink.add(r)
                progress(summary(r, ctx) + "\n")
                for line in r.problems:
                    progress("!!! WRONG VERDICT (%s): %s\n" % (describe(pt), line))
    except KeyboardInterrupt:
        progress("\ninterrupted; reporting the points finished so far\n")
        return done, True
    return done, False


FOOTNOTE = (
    "Time is wall time: launch to the first step prompt for the spec and reparse paths, and between the "
    "step 1 and step 2 prompts for the revision path.  "
    "`> T s` = killed at the time cap, `> M MB` = killed at the memory guard.  Peak RSS is the "
    "process's peak resident set.  verdicts = o5 answers that matched the expected ones.")


def report(done: List[Tuple[Group, List[Result]]], ctx: Ctx, stamp: str) -> bool:
    print(stamp)
    print("Limits: %g s time cap and %d MB memory guard per run; one tau process at a time.\n"
          % (ctx.cap, ctx.rss_cap))
    for g, results in done:
        if results:
            print(render(g, results, ctx))
    everyone = [r for _, rs in done for r in rs]
    print(FOOTNOTE)
    wrong = [(r, p) for r in everyone for p in r.problems]
    errors = [r for r in everyone if r.status == "error"]
    if wrong:
        print("\n**WRONG VERDICTS**\n")
        for r, p in wrong:
            print("- %s: %s" % (describe(r.point), p))
    if errors:
        print("\n**ERRORS**\n")
        for r in errors:
            print("- %s: %s" % (describe(r.point), r.note))
    sys.stdout.flush()
    return not wrong and not errors


# --------------------------------------------------------------------- cli

def positive(text: str) -> int:
    value = int(text)
    if value < 1:
        raise argparse.ArgumentTypeError("must be at least 1")
    return value


def add_common(p: argparse.ArgumentParser, nested_parser: bool) -> None:
    def d(value: object) -> object:
        # A sub-command must not overwrite what the main parser already stored.
        return argparse.SUPPRESS if nested_parser else value

    p.add_argument("--tau", metavar="PATH", default=d(None),
                   help="the tau binary (default: $TAU, then tau on PATH)")
    p.add_argument("--cap", type=float, metavar="SECONDS", default=d(300.0),
                   help="kill a run after this many seconds (default 300)")
    p.add_argument("--max-rss-mb", type=int, metavar="MB", default=d(8192),
                   help="kill a run whose resident memory passes this (default 8192)")
    p.add_argument("--out", metavar="FILE", default=d(None), help="write a CSV of all points")
    p.add_argument("--keep", metavar="DIR", default=d(None),
                   help="keep each point's inputs and outputs in DIR/<point>/ "
                        "(run by hand: tau boot.tau < stdin.txt)")


def add_point(p: argparse.ArgumentParser) -> None:
    p.add_argument("--path", choices=("spec", "revision", "reparse"), default="spec",
                   help="how the rule is loaded (default spec)")
    p.add_argument("--width", type=int, choices=(8, 384), default=8,
                   help="W: 8 = small integer ids, 384 = 384-bit constants (default 8)")
    p.add_argument("--live-only", action="store_true",
                   help="chain only: keep just the branch the checks select (one guard plus "
                        "fallback), with the ids and bit width of the full chain")


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="tau_dispatch_blowup.py", description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="With no command, runs sweep.  Exit status 1 if a verdict was wrong or a run failed.")
    add_common(p, False)
    p.add_argument("--quick", action="store_true", help="sweep: skip the points that end in a kill")
    sub = p.add_subparsers(dest="cmd", metavar="{chain,terms,sweep}")
    c = sub.add_parser("chain", help="one chain point")
    c.add_argument("size", type=positive, metavar="N", help="number of branches")
    add_point(c)
    add_common(c, True)
    t = sub.add_parser("terms", help="one terms point")
    t.add_argument("size", type=positive, metavar="K", help="number of terms")
    add_point(t)
    add_common(t, True)
    s = sub.add_parser("sweep", help="the default set of points, as markdown tables (default command)",
                       description="Run the default set of points one at a time and print markdown tables.",
                       epilog=plan_text(), formatter_class=argparse.RawDescriptionHelpFormatter)
    s.add_argument("--quick", action="store_true", default=argparse.SUPPRESS,
                   help="skip the points that end in a kill")
    add_common(s, True)
    return p


def die(message: str) -> None:
    sys.stderr.write(message + "\n")
    sys.exit(2)


def find_tau(arg: Optional[str]) -> str:
    found = shutil.which(arg or os.environ.get("TAU") or "tau")
    if not found:
        die("tau binary not found: pass --tau PATH, set $TAU, or put tau on PATH")
    return os.path.abspath(found)


def main(argv: Optional[List[str]] = None) -> int:
    if not hasattr(os, "wait4"):
        die("this script needs Linux or macOS")
    args = build_parser().parse_args(argv)
    if rss_kb(os.getpid()) <= 0:
        die("the memory guard needs `ps -o rss= -p PID`, which does not work here")
    ctx = Ctx(find_tau(args.tau), args.cap, args.max_rss_mb, args.keep and os.path.abspath(args.keep))
    if args.cmd in ("chain", "terms"):
        live = args.live_only and args.cmd == "chain"
        if args.live_only and not live:
            progress("note: terms has a single branch, so --live-only changes nothing\n")
        pt = Point(args.cmd, args.size, args.path, args.width, live)
        title = "%s %s=%d, %s path, W=%d%s" % (args.cmd, sym(args.cmd), args.size, args.path,
                                               args.width, ", live branch only" if live else "")
        groups = [one_group(title, args.cmd, args.path, [pt])]
    else:
        groups = sweep_groups()
    stamp = env_stamp(ctx.tau)
    sink = CsvSink(args.out)
    try:
        done, interrupted = run_points(groups, ctx, args.quick, sink)
    finally:
        sink.close()
    clean = report(done, ctx, stamp)
    if interrupted:
        return 130
    return 0 if clean else 1


if __name__ == "__main__":
    sys.exit(main())
