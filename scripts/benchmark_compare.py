#!/usr/bin/env python3
"""Compare benchmark timing against a baseline.

Performance regressions are treated as bugs — this script exits non-zero
if any test's timing exceeds the baseline by more than the allowed threshold.

Usage:
  benchmark_compare.py --baseline baseline.json --current current.json [--threshold 1.20]

  --threshold: multiplicative factor (default 1.20 = 20% regression allowed).
               Set to 1.0 to flag any slowdown at all.

Output:
  - Prints a table of regressions (test name, baseline ms, current ms, ratio).
  - Exits 0 if no regressions, 1 if regressions found.
  - Writes regressions to benchmark_regressions.json for CI artifact upload.
"""

import argparse
import json
import sys
from pathlib import Path


def load_timing(path: str) -> dict[str, dict]:
    """Load benchmark JSON and return {suite/name -> {ms, passed}}."""
    with open(path) as f:
        data = json.load(f)
    results = {}
    for t in data.get("tests", []):
        key = f"{t['suite']} / {t['name']}"
        results[key] = {"ms": t["ms"], "passed": t["passed"]}
    return results


def compare(baseline: dict, current: dict, threshold: float) -> list[dict]:
    """Find tests that regressed beyond the threshold."""
    regressions = []
    for key, cur in current.items():
        if key not in baseline:
            continue  # new test, no baseline to compare against
        base = baseline[key]
        if base["ms"] < 0.1:
            continue  # skip sub-0.1ms tests (noise-dominated)
        ratio = cur["ms"] / base["ms"] if base["ms"] > 0 else float("inf")
        if ratio > threshold:
            regressions.append({
                "test": key,
                "baseline_ms": round(base["ms"], 3),
                "current_ms": round(cur["ms"], 3),
                "ratio": round(ratio, 3),
                "delta_ms": round(cur["ms"] - base["ms"], 3),
            })
    regressions.sort(key=lambda r: r["ratio"], reverse=True)
    return regressions


def main():
    parser = argparse.ArgumentParser(description="Benchmark regression detector")
    parser.add_argument("--baseline", required=True, help="Baseline timing JSON")
    parser.add_argument("--current", required=True, help="Current timing JSON")
    parser.add_argument("--threshold", type=float, default=1.20,
                        help="Max allowed ratio (default: 1.20 = 20%% regression)")
    parser.add_argument("--output", default="benchmark_regressions.json",
                        help="Output file for regression details")
    args = parser.parse_args()

    if not Path(args.baseline).exists():
        print(f"[benchmark] No baseline found at {args.baseline} — "
              "storing current run as baseline.")
        # Copy current to baseline location
        import shutil
        shutil.copy2(args.current, args.baseline)
        sys.exit(0)

    baseline = load_timing(args.baseline)
    current = load_timing(args.current)
    regressions = compare(baseline, current, args.threshold)

    # Also detect improvements (for informational printing)
    improvements = []
    for key, cur in current.items():
        if key not in baseline:
            continue
        base = baseline[key]
        if base["ms"] < 0.1:
            continue
        ratio = cur["ms"] / base["ms"] if base["ms"] > 0 else 0
        if ratio < (1.0 / args.threshold):
            improvements.append({
                "test": key,
                "baseline_ms": round(base["ms"], 3),
                "current_ms": round(cur["ms"], 3),
                "ratio": round(ratio, 3),
            })

    # Print summary
    print(f"\n{'='*72}")
    print(f"  BENCHMARK COMPARISON (threshold: {args.threshold:.0%} of baseline)")
    print(f"{'='*72}")
    print(f"  Tests in baseline: {len(baseline)}")
    print(f"  Tests in current:  {len(current)}")
    print(f"  New tests:         {len(current) - len(set(current) & set(baseline))}")

    if improvements:
        print(f"\n  IMPROVEMENTS ({len(improvements)}):")
        for imp in sorted(improvements, key=lambda x: x["ratio"]):
            print(f"    {imp['ratio']:.2f}x  {imp['baseline_ms']:>8.1f} -> "
                  f"{imp['current_ms']:>8.1f} ms  {imp['test']}")

    if regressions:
        print(f"\n  REGRESSIONS ({len(regressions)}) — TREATED AS BUGS:")
        print(f"  {'Ratio':>6}  {'Baseline':>10}  {'Current':>10}  "
              f"{'Delta':>10}  Test")
        print(f"  {'-'*6}  {'-'*10}  {'-'*10}  {'-'*10}  {'-'*40}")
        for reg in regressions:
            print(f"  {reg['ratio']:>5.2f}x  {reg['baseline_ms']:>9.1f}  "
                  f"{reg['current_ms']:>9.1f}  +{reg['delta_ms']:>8.1f}  "
                  f"{reg['test']}")
        print(f"\n  FAIL: {len(regressions)} test(s) regressed beyond "
              f"{args.threshold:.0%} threshold.")
    else:
        print(f"\n  PASS: No performance regressions detected.")

    print(f"{'='*72}\n")

    # Write regression details
    with open(args.output, "w") as f:
        json.dump({
            "threshold": args.threshold,
            "regressions": regressions,
            "improvements": [dict(r) for r in improvements],
            "baseline_tests": len(baseline),
            "current_tests": len(current),
        }, f, indent=2)

    if regressions:
        sys.exit(1)


if __name__ == "__main__":
    main()
