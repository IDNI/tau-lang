#!/usr/bin/env python3
"""Benchmark tau-lang interpreter construction for guarded-comparison clauses.

WHY THIS EXISTS
    tau-testnet's per-user policy rules are one `always()` unit holding a chain
    of guarded comparisons -- "over this amount, that public key must match".
    Building an interpreter for one is the dominant cost in the whole product:
    at three guards it is 5.8s on an M-series laptop and 312s on an aarch64
    server, and the server figure is past the watchdog limit that SIGKILLs the
    node and quarantines its database. So this is not a micro-benchmark; it is
    the thing that decides how expressive a user policy is allowed to be.

BASELINE (Apple M-series, python 3.13, flat shape, one author, bv[8])

        guards      1     2     3     4     6      8     10     12
        build (s) 0.3   0.9   1.7   2.8   6.1   11.1   18.6   27.7
        peak (MB)  43    55    70    93   147    187    272    357

    Time is QUADRATIC and memory is LINEAR: cost ~ guards^1.79 over that range.
    The default sweep runs to 12 on purpose -- the interesting question for an
    engine change is where the curve bends, not what it costs at n=3.

WHY THE DEFAULT WIDTH IS 8 AND NOT 384

    The clauses tau-testnet deploys name 384-bit public keys, but the engine
    never sees them at that width. `tau_shrink` interns any long value used
    exclusively in `=`/`!=` contexts to a small integer and rewrites the formula
    text and the stream values immediately before the interpreter runs, so a
    live composite reaches the engine looking like

        !(i20[t]:bv[8] = { 11 }:bv[8])

    Its width is chosen per process from the node's intern count: <=254 ids give
    bv[8], <=65534 bv[16], then bv[24]. So bv[8]..bv[24] is the whole range a
    real node can occupy, and across ALL of it the cost is flat -- at 8 guards,
    11.3s / 12.1s / 12.5s, about 10%. Width is therefore NOT a tau-testnet
    optimisation target, and benchmarking at 384 measures a spec nobody deploys.

    `--width 384` is still worth running, but as a DIAGNOSTIC: it quantifies what
    the shrink layer is buying (3x at one guard rising to 6x at twelve, and a
    worse exponent, 2.15 against 1.79). Read it as the cost of ever letting a
    wide value reach the engine, not as available headroom.

THE ONE FINDING THAT IS AN OPTIMISATION TARGET

    Shape is worth ~1.4x: at three guards the flat form -- one conditional over a
    disjunction -- beats the semantically identical nested cascade, 5.9s against
    8.2s at bv[384]. Free, if the caller emits the flat form.

    Everything else is the quadratic in guard count, at small width. That is what
    tau-testnet actually pays, and it is what an engine change has to move.

DEPENDENCIES
    The nanobind module only. No tau-testnet imports -- copy this file into
    tau-lang if that is easier.

        PYTHONPATH=<build>/bindings/python/nanobind python3 bench_clause_compile.py

USAGE
    The PYTHONPATH must point at the directory holding a tau.cpython-*.so whose
    ABI tag matches the python you run, and the module is NOT auto-discovered --
    a stale build is the classic way to measure the wrong engine, so the script
    lists candidates with their dates and lets you choose. Get it wrong and it
    says so before running anything.

    # the default sweep, crossing the 55s point and continuing
    bench_clause_compile.py

    # record a baseline, change the engine, compare
    bench_clause_compile.py --json before.json
    bench_clause_compile.py --json after.json --baseline before.json

    # one point, in detail, including the cost of the first step
    bench_clause_compile.py --guards 8 --step --repeat 3

    # the other axes
    bench_clause_compile.py --shape cascade      # nested vs flat disjunction
    bench_clause_compile.py --authors 1,2,4      # composite nesting
    bench_clause_compile.py --width 24,64,384    # does bv width matter?

FOUR THINGS THAT WILL WASTE YOUR AFTERNOON
    1. A specification is ONE `always ( ... ).` sentence. Two always() sentences
       joined by a newline make get_interpreter return None -- including for
       tau-testnet's own shipped rules. Every spec here is a single unit.
    2. Per-stream bitvector typing is process-global and STICKY. One spec typing
       i18 at bv[384] and the next at bv[24] leaves get_interpreter returning
       None for the rest of the process. Every data point therefore runs in its
       own child; that is not paranoia, it is the only way two points in one
       sweep are comparable.
    3. `get_interpreter` returning None is a FAILURE, not a fast result. An
       "optimisation" that breaks construction otherwise looks like a 100x win.
       Every timing here is discarded unless a real interpreter came back.
    4. Engine diagnostics go to fd 2 and are ANSI-coloured. Do not grep stdout
       for them; the child's stderr is captured and surfaced on failure.
"""
from __future__ import annotations

import argparse
import json
import os
import statistics
import subprocess
import sys
import tempfile
import time

# Literals carry letters on purpose: an all-digit hex constant has been observed
# to silently change the meaning of an identity guard in this stack, so a
# benchmark built from all-digit keys can measure a spec nobody would deploy.
_KEY_ALPHABET = "abcdef98"


def _key(i: int, width_bits: int) -> str:
    """A distinct hex literal of the right length for `width_bits`."""
    digits = max(2, width_bits // 4)
    stem = (_KEY_ALPHABET[i % len(_KEY_ALPHABET)] + "%02x" % (i + 3)) * digits
    return stem[:digits]


def build_spec(guards: int, *, shape: str, authors: int, width: int,
               value_width: int = 24) -> str:
    """One `always()` unit: `authors` clauses, each with `guards` comparisons.

    Mirrors what tau-testnet actually deploys -- an identity guard per author
    over a policy body, which is itself a chain of "amount over X requires key
    K on stream S" terms.
    """
    def bv(v: int, w: int) -> str:
        return "{ #x%0*x }:bv[%d]" % (max(2, w // 4), v, w)

    def lit(h: str, w: int) -> str:
        return "{ #x" + h + " }:bv[%d]" % w

    def out(v: int) -> str:
        return "o5[t]:bv[%d] = %s" % (value_width, bv(v, value_width))

    def body(author: int) -> str:
        terms = []
        for i in range(guards):
            stream = 18 + i
            threshold = 1000 * (10 ** min(i, 4)) + i
            terms.append(
                "(i1[t]:bv[%d] > %s && !(i%d[t]:bv[%d] = %s))"
                % (value_width, bv(threshold, value_width), stream, width,
                   lit(_key(author * guards + i, width), width))
            )
        if shape == "flat":
            # One conditional over a disjunction: the shipped shape.
            return "( (" + " || ".join(terms) + ") ? (" + out(0) + ") : (" + out(1) + ") )"
        # Nested: semantically identical, measurably different.
        expr = "(" + out(1) + ")"
        for term in reversed(terms):
            expr = "(" + term + " ? (" + out(0) + ") : " + expr + ")"
        return expr

    if authors == 1:
        return "always ( " + body(0) + " )."

    # Composite: each author's body behind an identity guard on i12, exactly as
    # the clause registry composes them.
    expr = "(" + out(1) + ")"
    for a in reversed(range(authors)):
        guard = "i12[t]:bv[%d] = %s" % (width, lit(_key(900 + a, width), width))
        expr = "(" + guard + " ? ( " + body(a) + " ) : " + expr + ")"
    return "always ( " + expr + " )."


CHILD = r'''
import json, sys, time
spec = sys.stdin.read()
want_step = sys.argv[1] == "step"
import tau

def unwrap(res):
    """(value or None, error text or None) of a call. The binding returns a
    tau.result whose report says whether the call failed; an older binding
    returns the value itself, None on failure."""
    if not hasattr(res, "report"):
        return res, None
    report = res.report
    if report.has_error:
        return None, str(report).strip()[-400:] or "error report"
    return (res.value if res else None), None

t0 = time.perf_counter()
iface, build_error = unwrap(tau.get_interpreter(spec))
build = time.perf_counter() - t0

result = {"built": iface is not None, "build_seconds": build}
if build_error:
    sys.stderr.write(build_error + "\n")

if iface is not None and want_step:
    # Stepping is a SEPARATE cost from construction and has its own cliff --
    # specs have been seen to build in ~40s and then step for >200s. An engine
    # change can easily move one and not the other. A step asks for its inputs
    # until it yields its outputs.
    try:
        t1 = time.perf_counter()
        outputs = None
        for _ in range(100):
            needed, err = unwrap(tau.get_inputs_for_step(iface))
            if err or needed is None:
                raise RuntimeError(err or "no input prompt")
            outputs, err = unwrap(tau.step(iface, {s: "0" for s in needed}))
            if err:
                raise RuntimeError(err)
            if outputs is not None:
                break
        if outputs is None:
            raise RuntimeError("no output after 100 prompts")
        result["step_seconds"] = time.perf_counter() - t1
    except Exception as exc:
        result["step_error"] = "%s: %s" % (type(exc).__name__, exc)

try:
    import resource
    # ru_maxrss is KILOBYTES on Linux and BYTES on macOS. Getting this wrong
    # reports a 64MB build as 64GB, which is the kind of number people act on.
    divisor = 1024.0 * 1024.0 if sys.platform == "darwin" else 1024.0
    result["peak_rss_mb"] = round(
        resource.getrusage(resource.RUSAGE_SELF).ru_maxrss / divisor, 1)
except Exception:
    pass

print("BENCH_JSON " + json.dumps(result))
'''


def run_point(spec: str, *, timeout: float, step: bool) -> dict:
    """One data point, in its own process. See trap 2."""
    src = tempfile.NamedTemporaryFile("w", suffix="_bench_child.py", delete=False)
    src.write(CHILD)
    src.close()
    started = time.perf_counter()
    try:
        proc = subprocess.run(
            [sys.executable, src.name, "step" if step else "nostep"],
            input=spec, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return {"status": "timeout", "wall_seconds": timeout}
    finally:
        try:
            os.unlink(src.name)
        except OSError:
            pass

    wall = time.perf_counter() - started
    payload = None
    for line in (proc.stdout or "").splitlines():
        if line.startswith("BENCH_JSON "):
            payload = json.loads(line[len("BENCH_JSON "):])
    if payload is None:
        return {"status": "crashed", "wall_seconds": round(wall, 2),
                "rc": proc.returncode,
                "stderr": (proc.stderr or "").strip()[-400:]}
    if not payload.get("built"):
        # Trap 3: this is a failure, and timing it would be a lie.
        return {"status": "not_built", "wall_seconds": round(wall, 2),
                "stderr": (proc.stderr or "").strip()[-400:]}
    payload["status"] = "ok"
    payload["wall_seconds"] = round(wall, 2)
    payload["build_seconds"] = round(payload["build_seconds"], 2)
    if "step_seconds" in payload:
        payload["step_seconds"] = round(payload["step_seconds"], 2)
    return payload


def measure(spec: str, *, repeat: int, timeout: float, step: bool) -> dict:
    runs = [run_point(spec, timeout=timeout, step=step) for _ in range(repeat)]
    good = [r for r in runs if r.get("status") == "ok"]
    if not good:
        return dict(runs[-1], repeats=repeat)
    builds = [r["build_seconds"] for r in good]
    out = {
        "status": "ok",
        "repeats": repeat,
        "ok_runs": len(good),
        "build_seconds": round(statistics.median(builds), 2),
        "build_min": round(min(builds), 2),
        "build_max": round(max(builds), 2),
        "spec_chars": len(spec),
    }
    rss = [r["peak_rss_mb"] for r in good if "peak_rss_mb" in r]
    if rss:
        out["peak_rss_mb"] = max(rss)
    steps = [r["step_seconds"] for r in good if "step_seconds" in r]
    if steps:
        out["step_seconds"] = round(statistics.median(steps), 2)
    errs = [r["step_error"] for r in good if "step_error" in r]
    if errs:
        out["step_error"] = errs[0]
    return out


def growth_exponent(points: list[tuple[int, float]]) -> float | None:
    """Least-squares k for cost ~ n**k. The number an engine change should move."""
    usable = [(n, s) for n, s in points if n > 0 and s > 0]
    if len(usable) < 3:
        return None
    import math
    xs = [math.log(n) for n, _ in usable]
    ys = [math.log(s) for _, s in usable]
    mx, my = sum(xs) / len(xs), sum(ys) / len(ys)
    denom = sum((x - mx) ** 2 for x in xs)
    if denom == 0:
        return None
    return round(sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / denom, 2)


def _abi_tag() -> str:
    """The ABI tag this interpreter's extension modules must carry."""
    return "cpython-%d%d" % (sys.version_info[0], sys.version_info[1])


def find_candidate_modules() -> list[tuple[str, str, float]]:
    """Every tau.cpython-*.so worth mentioning, as (path, abi_tag, mtime).

    Searched in the order a developer would expect: an explicit TAU_LANG_ROOT,
    the tree this script sits in, the working directory, then the usual clone
    location. Stale builds are the classic way to measure the wrong engine, so
    candidates are reported WITH their mtimes and never selected automatically.
    """
    import glob

    roots = []
    if os.environ.get("TAU_LANG_ROOT"):
        roots.append(os.environ["TAU_LANG_ROOT"])
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    roots += [here, os.getcwd(), os.path.expanduser("~/idni/tau-lang")]

    seen, found = set(), []
    for root in roots:
        pattern = os.path.join(root, "build*", "bindings", "python", "nanobind",
                               "tau.cpython-*.so")
        for path in sorted(glob.glob(pattern)):
            real = os.path.realpath(path)
            if real in seen:
                continue
            seen.add(real)
            base = os.path.basename(path)
            tag = base.split(".")[1].rsplit("-", 1)[0] if "." in base else "?"
            try:
                found.append((path, tag, os.path.getmtime(path)))
            except OSError:
                pass
    return found


def preflight() -> None:
    """Prove the engine is importable before running a sweep.

    Without this, a wrong PYTHONPATH produces one identical ModuleNotFoundError
    per data point and no explanation -- which is exactly what it did to the
    first person who ran it.
    """
    probe = subprocess.run(
        [sys.executable, "-c", "import tau; print(tau.__file__)"],
        capture_output=True, text=True)
    if probe.returncode == 0:
        print("  engine      %s" % probe.stdout.strip())
        return

    reason = (probe.stderr or "").strip().splitlines()
    print("CANNOT IMPORT THE ENGINE")
    print("  %s" % (reason[-1] if reason else "unknown import failure"))
    print("  running     python %s (needs a %s module)"
          % (sys.version.split()[0], _abi_tag()))
    print("  PYTHONPATH  %s" % (os.environ.get("PYTHONPATH") or "(unset)"))
    print()

    candidates = find_candidate_modules()
    if not candidates:
        print("  No tau.cpython-*.so found. Build the nanobind bindings, or point")
        print("  TAU_LANG_ROOT at your tau-lang checkout and try again.")
        raise SystemExit(2)

    want = _abi_tag()
    print("  Modules found (newest first) -- check the date before trusting one:")
    for path, tag, mtime in sorted(candidates, key=lambda c: -c[2]):
        when = time.strftime("%Y-%m-%d %H:%M", time.localtime(mtime))
        mark = "  <- matches this python" if tag == want else "     (%s, not %s)" % (tag, want)
        print("    %s  %s%s" % (when, os.path.dirname(path), mark))
    print()

    usable = [c for c in candidates if c[1] == want]
    if usable:
        newest = sorted(usable, key=lambda c: -c[2])[0]
        print("  Try:")
        print("    PYTHONPATH=%s \\" % os.path.dirname(newest[0]))
        print("      %s %s" % (sys.executable, " ".join(sys.argv)))
    else:
        print("  None of those match %s. Either run the python whose tag appears"
              % want)
        print("  above, or rebuild the bindings against this one.")
    raise SystemExit(2)


def parse_ints(text: str) -> list[int]:
    return [int(p) for p in str(text).replace(",", " ").split()]


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--guards", default="1,2,3,4,6,8,10,12",
                    help="comparison terms per clause (default sweeps past the "
                         "55s point at 8)")
    ap.add_argument("--shape", default="flat", choices=("flat", "cascade", "both"))
    ap.add_argument("--authors", default="1",
                    help="clauses composed under identity guards")
    ap.add_argument("--width", default="8",
                    help="bv width of the compared literal. 8 is what tau_shrink "
                         "hands the engine in production; 384 is a diagnostic "
                         "for what shrink saves, not a deployed shape")
    ap.add_argument("--repeat", type=int, default=1, help="runs per point; median wins")
    ap.add_argument("--step", action="store_true",
                    help="also time the first step (a separate cliff)")
    ap.add_argument("--budget", type=float, default=600.0,
                    help="seconds; a point over this ends that sweep (0 = no cap)")
    ap.add_argument("--json", help="write results here for later --baseline use")
    ap.add_argument("--baseline", help="compare against a previous --json file")
    ap.add_argument("--quick", action="store_true",
                    help="1,2,3,4 with a 60s budget, for CI")
    args = ap.parse_args(argv)

    if args.quick:
        args.guards, args.budget = "1,2,3,4", 60.0

    shapes = ("flat", "cascade") if args.shape == "both" else (args.shape,)
    guards_list = parse_ints(args.guards)
    authors_list = parse_ints(args.authors)
    widths = parse_ints(args.width)
    timeout = args.budget if args.budget > 0 else 86400.0

    print("tau clause-compile benchmark")
    print("  python      %s" % sys.version.split()[0])
    print("  PYTHONPATH  %s" % (os.environ.get("PYTHONPATH") or "(unset)"))
    print("  budget      %s" % ("none" if args.budget <= 0 else "%.0fs/point" % args.budget))
    preflight()

    # Checked here, not after the sweep: comparing is the whole point of passing
    # it, and finding out it was missing costs a rerun.
    if args.baseline and not os.path.exists(args.baseline):
        print()
        print("  --baseline %s does not exist." % args.baseline)
        print("  Record one first:  %s --json %s"
              % (os.path.basename(sys.argv[0]), args.baseline))
        raise SystemExit(2)
    print()

    results = []
    for shape in shapes:
        for authors in authors_list:
            for width in widths:
                label = "shape=%s authors=%d width=%d" % (shape, authors, width)
                print(label)
                header = "  guards   build      "
                if args.step:
                    header += "step       "
                header += "rss        spec"
                print(header)
                curve = []
                for guards in guards_list:
                    spec = build_spec(guards, shape=shape, authors=authors,
                                      width=width)
                    row = measure(spec, repeat=args.repeat, timeout=timeout,
                                  step=args.step)
                    row.update(guards=guards, shape=shape, authors=authors,
                               width=width)
                    results.append(row)

                    if row["status"] != "ok":
                        detail = row.get("stderr", "")
                        print("  %6d   %-9s  %s" % (
                            guards, row["status"].upper(),
                            (detail.splitlines() or [""])[-1][:70]))
                        if row["status"] == "timeout":
                            print("           (over budget; ending this sweep)")
                            break
                        continue

                    line = "  %6d   %7.1fs " % (guards, row["build_seconds"])
                    if args.step:
                        line += "  %7s " % (
                            ("%.1fs" % row["step_seconds"]) if "step_seconds" in row
                            else "-")
                    line += "  %7s   %d chars" % (
                        ("%.0fMB" % row["peak_rss_mb"]) if "peak_rss_mb" in row else "-",
                        row["spec_chars"])
                    print(line)
                    curve.append((guards, row["build_seconds"]))
                    if args.budget > 0 and row["build_seconds"] > args.budget:
                        print("           (over budget; ending this sweep)")
                        break

                k = growth_exponent(curve)
                if k is not None:
                    print("  cost ~ guards^%.2f over the measured range" % k)
                print()

    if args.baseline:
        compare(results, args.baseline)

    if args.json:
        with open(args.json, "w") as fh:
            json.dump({"generated_at": time.time(),
                       "python": sys.version.split()[0],
                       "results": results}, fh, indent=2)
        print("wrote %s" % args.json)

    return 0 if any(r["status"] == "ok" for r in results) else 1


def compare(results: list[dict], baseline_path: str) -> None:
    try:
        with open(baseline_path) as fh:
            base = json.load(fh).get("results") or []
    except (OSError, ValueError) as exc:
        print("could not read baseline %s: %s" % (baseline_path, exc))
        return

    def key(r):
        return (r.get("shape"), r.get("authors"), r.get("width"), r.get("guards"))

    old = {key(r): r for r in base if r.get("status") == "ok"}
    print("against %s" % baseline_path)
    print("  guards   before      after       change")
    any_row = False
    for row in results:
        prior = old.get(key(row))
        if not prior or row.get("status") != "ok":
            continue
        any_row = True
        was, now = prior["build_seconds"], row["build_seconds"]
        delta = (now - was) / was * 100 if was else 0.0
        verdict = "faster" if delta < -5 else ("SLOWER" if delta > 5 else "same")
        print("  %6d   %7.1fs   %7.1fs   %+6.1f%%  %s"
              % (row["guards"], was, now, delta, verdict))
    if not any_row:
        print("  (no comparable points)")
    print()


if __name__ == "__main__":
    raise SystemExit(main())
