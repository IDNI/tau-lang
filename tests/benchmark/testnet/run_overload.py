#!/usr/bin/env python3
"""Run the tau-overload benchmarks and write their rows to one results folder.

The four scripts of this folder load tau the way tau-testnet does, with
rules that grow until the engine strains:

  dispatch-blowup  tau_dispatch_blowup.py: the load time of one keyed rule (a
                   ?: chain of N branches, or one flat || of K terms) through
                   the CLI, as a boot spec or as a revision.
  clause-compile   bench_clause_compile.py: the time to build an interpreter
                   for a guarded-comparison clause through the Python binding
                   (the build rows), and the time of its first step (the step
                   rows).
  load-test-bv     load_test_bitvectors.py: a running CLI that receives one
                   seeded bitvector rule per step; one row per step, and a
                   run row with the rest of the time and the peak memory.
  account-rules-repro
                   tau_account_rules_repro.py: one rule per account through
                   the Python binding, its `sweep --quick`. N rules as the
                   parts of one specification (the open and step rows) or
                   added by an update (the add rows); one rule per
                   interpreter (the build row, and the memory of 200 live
                   ones); 1000 interpreters built, stepped and dropped in one
                   process (the build and step of the first and last 100).

Every point runs in its own process, under a time cap, and a row keeps the
time, the peak resident memory and the status of its point:
[name, seconds, rss_kb, status], the name being "<group> / <point>". The
status is pass, cap (the point was killed at its time or memory cap: its time
is a lower bound, and the comparisons still read it) or fail (a wrong answer
or a crash). The results folder receives overload.json with the rows, and the
logs of each script under logs/. The exit status is 1 when a row failed.

Usage:
  run_overload.py --tau build/release/tau \
      --module-dir build/release/bindings/python/nanobind --out results
"""

import argparse
import csv
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent

# (path, kind, size, width, live only) of each dispatch-blowup point: both rule
# shapes, both load paths, the 384-bit keys, and a rule whose live branch
# alone stays flat, as the control
DISPATCH_POINTS = (
    ("spec", "chain", 8, 8, False),
    ("spec", "chain", 32, 8, False),
    ("revision", "chain", 24, 8, False),
    ("spec", "terms", 8, 8, False),
    ("spec", "terms", 16, 8, False),
    ("revision", "terms", 14, 8, False),
    ("spec", "chain", 32, 384, False),
    ("spec", "chain", 64, 8, True),
)
# a sweep of bench_clause_compile.py ends at its first point over the budget
CLAUSE_ARGS = ("--guards", "8,16,32,64", "--shape", "both", "--authors", "1,4",
               "--width", "8", "--step")
LOAD_WIDTH = 16
LOAD_SEED = 1
LOAD_RULES = 10
# per account-rules-repro point a cap of --cap seconds; the sweep has 15 points
ACCOUNT_POINTS = 15


def log(text: str) -> None:
    print(f"run_overload: {text}", flush=True)


def tree_rss_kb(root: int) -> int:
    """The resident memory of a process and all its descendants, in kB."""
    parent, rss = {}, {}
    page_kb = os.sysconf("SC_PAGE_SIZE") // 1024
    for entry in os.listdir("/proc"):
        if not entry.isdigit():
            continue
        try:
            with open(f"/proc/{entry}/stat") as f:
                fields = f.read().rsplit(")", 1)[1].split()
            parent[int(entry)] = int(fields[1])
            rss[int(entry)] = int(fields[21]) * page_kb
        except (OSError, ValueError, IndexError):
            continue
    total = 0
    for pid in rss:
        p = pid
        while p in parent and p != root and p > 1:
            p = parent[p]
        if p == root:
            total += rss[pid]
    return total


def run(cmd: list, log_file: Path, cap: float, env=None) -> tuple:
    """(exit status, wall seconds, peak memory of the process tree in kB) of a
    command; a run over `cap` seconds is killed and its status is None."""
    log_file.parent.mkdir(parents=True, exist_ok=True)
    start = time.monotonic()
    peak = 0
    with open(log_file, "w") as out:
        proc = subprocess.Popen(cmd, stdout=out, stderr=subprocess.STDOUT,
                                env=env, start_new_session=True)
        while proc.poll() is None:
            peak = max(peak, tree_rss_kb(proc.pid))
            if time.monotonic() - start > cap:
                os.killpg(proc.pid, 9)
                proc.wait()
                return None, round(time.monotonic() - start, 3), peak
            time.sleep(0.25)
    return proc.returncode, round(time.monotonic() - start, 3), peak


def dispatch_rows(args, logs: Path) -> list:
    rows = []
    for path, kind, size, width, live in DISPATCH_POINTS:
        point = f"{kind} {'N' if kind == 'chain' else 'K'}={size} {path} W={width}" \
            + (" live" if live else "")
        stem = f"dispatch-{kind}-{size}-{path}-{width}" + ("-live" if live else "")
        csv_file = logs / f"{stem}.csv"
        cmd = [args.python, "-I", str(HERE / "tau_dispatch_blowup.py"), kind, str(size),
               "--path", path, "--width", str(width), "--tau", args.tau,
               "--cap", str(args.cap), "--max-rss-mb", str(args.max_rss_mb),
               "--out", str(csv_file)] + (["--live-only"] if live else [])
        _, wall, _ = run(cmd, logs / f"{stem}.log", args.cap + 120)
        row = None
        if csv_file.is_file():
            with open(csv_file) as f:
                row = next(csv.DictReader(f), None)
        name = f"dispatch-blowup / {point}"
        if row is None:
            rows.append([name, wall, 0, "fail"])
        else:
            # the script's own guards: cap (time) and rss (memory)
            status = "cap" if row["status"] in ("cap", "rss") else \
                "pass" if row["status"] == "ok" and row["verdicts_ok"] == row["verdicts_total"] \
                else "fail"
            seconds = float(row["time_s"] or row["wall_s"] or wall)
            rows.append([name, round(seconds, 3), int(float(row["peak_rss_mb"] or 0) * 1024),
                         status])
        log(f"{rows[-1]}")
    return rows


def clause_rows(args, logs: Path) -> list:
    json_file = logs / "clause-compile.json"
    env = dict(os.environ, PYTHONPATH=args.module_dir)
    cmd = [args.python, str(HERE / "bench_clause_compile.py"), *CLAUSE_ARGS,
           "--budget", str(args.clause_budget), "--json", str(json_file)]
    rc, wall, _ = run(cmd, logs / "clause-compile.log", args.clause_budget * 40, env)
    if not json_file.is_file():
        return [["clause-compile / run", wall, 0, "fail"]]
    rows = []
    for r in json.loads(json_file.read_text()).get("results", []):
        point = f"{r['shape']} a={r['authors']} W={r['width']} g={r['guards']}"
        status = {"ok": "pass", "timeout": "cap"}.get(r.get("status"), "fail")
        build = r.get("build_seconds", r.get("wall_seconds", 0.0))
        rows.append([f"clause-compile / {point} build", float(build),
                     int(float(r.get("peak_rss_mb", 0)) * 1024), status])
        if status == "pass":
            rows.append([f"clause-compile / {point} step", float(r.get("step_seconds", 0.0)),
                         0, "pass" if "step_seconds" in r else "fail"])
    log(f"clause-compile: {len(rows)} rows in {wall} s")
    return rows


STEP_RE = re.compile(r"\[INCOMING\] Result for o1\[t\]: .*\(([0-9.]+)s\)")


def load_rows(args, logs: Path) -> list:
    log_file = logs / "load-test-bv.log"
    cmd = [args.python, "-I", str(HERE / "load_test_bitvectors.py"), "--tau-binary", args.tau,
           "--iterations", str(LOAD_RULES), "--delay", "0", "--width", str(LOAD_WIDTH),
           "--seed", str(LOAD_SEED), "--no-stderr"]
    rc, wall, peak = run(cmd, log_file, args.cap * 3)
    text = re.sub(r"\x1b\[[0-9;]*m", "", log_file.read_text(errors="replace"))
    steps = [float(m.group(1)) for m in STEP_RE.finditer(text)]
    rows = [[f"load-test-bv / bv[{LOAD_WIDTH}] rule {i + 1:02d}", s, 0, "pass"]
            for i, s in enumerate(steps)]
    if rc is None:
        status = "cap"
    else:
        status = "pass" if rc == 0 and len(steps) == LOAD_RULES else "fail"
    # the rest of the run: the start of tau and, on a cap, the step it killed;
    # the group then sums to the wall time of the run
    rows.append([f"load-test-bv / bv[{LOAD_WIDTH}] run", round(max(0.0, wall - sum(steps)), 3),
                 peak, status])
    log(f"load-test-bv: {len(steps)} of {LOAD_RULES} rules in {wall} s, peak {peak} kB")
    return rows


def account_point_rows(p: dict) -> list:
    """The rows of one point of tau_account_rules_repro.py; [] for a point
    that is folded into another row."""
    status = "cap" if p["status"] in ("cap", "rss") else \
        "pass" if p["status"] == "ok" and not p.get("verdicts_wrong") else "fail"
    peak = int(float(p.get("peak_rss_mb") or 0) * 1024)
    kind, params = p["kind"], p.get("params", {})
    group = "account-rules-repro"
    if kind == "parts":
        n = params["n"]
        if "open_s" not in p:
            return [[f"{group} / parts N={n} open", round(p["wall_s"], 3), peak, status]]
        rows = [[f"{group} / parts N={n} open", round(p["open_s"], 3), peak, status]]
        if "step_mean_ms" in p:
            rows.append([f"{group} / parts N={n} step", round(p["step_mean_ms"] / 1000, 3), 0,
                         status])
        return rows
    if kind == "add-one":
        n = params["n"]
        seconds = p["feed_s"] if "feed_s" in p else p["wall_s"]
        return [[f"{group} / add-one N={n} update", round(seconds, 3), peak, status]]
    if kind == "live":
        count = params["count"]
        # the memory the live interpreters hold, beyond the first one
        held = p.get("rss_after_mb") and p.get("rss_before_mb") is not None
        kb = int((p["rss_after_mb"] - p["rss_before_mb"]) * 1024) if held else peak
        return [[f"{group} / live {count} interpreters", round(p["wall_s"], 3), kb, status]]
    if kind == "churn":
        label = "churn %d%s" % (params["k"], " with-shared" if params.get("shared") else "")
        blocks = p.get("blocks") or []
        rows = []
        if blocks:
            # a block is 100 interpreters: the sum of their builds and of their
            # steps, so a growth over the run reads as first against last
            first, last = blocks[0], blocks[-1]
            for which, b in (("first", first), ("last", last)):
                n = b["txs"] - (blocks[-2]["txs"] if which == "last" and len(blocks) > 1 else 0)
                rows.append([f"{group} / {label} build {which} {n}",
                             round(b["build_ms"] * n / 1000, 3), 0, status])
                rows.append([f"{group} / {label} step {which} {n}",
                             round(b["step_ms"] * n / 1000, 3), 0, status])
                if b.get("shared_ms") is not None:
                    rows.append([f"{group} / {label} shared step {which} {n}",
                                 round(b["shared_ms"] * n / 1000, 3), 0, status])
        rows.append([f"{group} / {label} run", round(p["wall_s"], 3), peak,
                     status if blocks else "fail" if status == "pass" else status])
        return rows
    return []


def account_rows(args, logs: Path) -> list:
    json_file = logs / "account-rules-repro.json"
    cmd = [args.python, "-I", str(HERE / "tau_account_rules_repro.py"), "sweep", "--quick",
           "--tau-dir", args.module_dir, "--cap", str(args.cap),
           "--rss-cap-gb", str(args.max_rss_mb / 1024), "--json", str(json_file),
           "--md", str(logs / "account-rules-repro.md")]
    rc, wall, _ = run(cmd, logs / "account-rules-repro.log", args.cap * ACCOUNT_POINTS + 300)
    if not json_file.is_file():
        return [["account-rules-repro / run", wall, 0, "fail"]]
    points = json.loads(json_file.read_text()).get("points", [])
    singles = [p for p in points if p["kind"] == "single"]
    rows = []
    if singles:
        # the fresh processes of one rule alone: their mean build
        ok = [p for p in singles if p["status"] == "ok" and "build_ms" in p]
        status = "fail" if any(p.get("verdicts_wrong") for p in singles) else \
            "pass" if len(ok) == len(singles) else \
            "cap" if all(p["status"] in ("ok", "cap", "rss") for p in singles) else "fail"
        build = sum(p["build_ms"] for p in ok) / len(ok) / 1000 if ok else 0.0
        rows.append(["account-rules-repro / single build", round(build, 3),
                     int(max(float(p.get("peak_rss_mb") or 0) for p in singles) * 1024), status])
    for p in points:
        rows += account_point_rows(p)
    if rc is None:
        rows.append(["account-rules-repro / run", wall, 0, "cap"])
    elif rc != 0 and not any(r[3] == "fail" for r in rows):
        # the sweep failed without a failed point: the module or the run itself
        rows.append(["account-rules-repro / run", wall, 0, "fail"])
    log(f"account-rules-repro: {len(points)} points, {len(rows)} rows in {wall} s")
    return rows


GROUPS = {"dispatch-blowup": dispatch_rows, "clause-compile": clause_rows,
          "load-test-bv": load_rows, "account-rules-repro": account_rows}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--tau", required=True, help="the tau CLI")
    ap.add_argument("--module-dir", required=True,
                    help="the folder of the nanobind module (tau.cpython-*.so)")
    ap.add_argument("--python", default=sys.executable,
                    help="the interpreter the module was built for (default: this one)")
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--cap", type=float, default=300.0,
                    help="seconds per dispatch-blowup and account-rules-repro point; the load test "
                         "gets three (default 300)")
    ap.add_argument("--clause-budget", type=float, default=120.0,
                    help="seconds per clause-compile point (default 120)")
    ap.add_argument("--max-rss-mb", type=int, default=8192)
    ap.add_argument("--only", choices=tuple(GROUPS), action="append",
                    help="run this group only (repeatable)")
    args = ap.parse_args()
    args.tau = os.path.abspath(args.tau)
    args.module_dir = os.path.abspath(args.module_dir)

    logs = args.out / "logs"
    logs.mkdir(parents=True, exist_ok=True)
    rows = []
    for name, fn in GROUPS.items():
        if args.only and name not in args.only:
            continue
        log(f"{name}: start")
        rows += fn(args, logs)
    failed = sum(1 for r in rows if r[3] == "fail")
    capped = sum(1 for r in rows if r[3] == "cap")
    (args.out / "overload.json").write_text(json.dumps({"rows": rows}, indent=1) + "\n")
    log(f"{len(rows)} rows, {capped} capped, {failed} failed -> {args.out / 'overload.json'}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
