#!/usr/bin/env python3
"""Merge the measurements of one test run into one benchmark record.

A ctest run (--kind ctest) leaves, in --results:
  junit.xml           ctest --output-junit: one testcase per ctest entry
  benchmarks/*.json   the doctest listener of each suite (tests/benchmark_listener.h)
  measure/*.jsonl     scripts/ci/measure-test.sh: one line per measured process
  ctest-status        the exit status of ctest
A pytest run (--kind pytest) leaves:
  junit.xml           pytest --junitxml
  time.txt            /usr/bin/time -v of the whole run
  status              the exit status of pytest

The record is the JSON documented in the benchmarks plan: a header with the
series, the commit and the host, the totals, and one compact row per test.

Usage:
  benchmark-collect.py --kind ctest --series linux-x86_64-release \
      --results build/test-results --sha <sha> --branch devel \
      --run-id <id> --run-url <url> --jobs 4 --out record.json
"""

import argparse
import datetime as dt
import json
import os
import platform
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def read_junit(path: Path) -> list:
    """Rows [name, seconds, status] of a junit file; status is pass, fail or skip."""
    rows = []
    if not path.is_file():
        return rows
    root = ET.parse(path).getroot()
    for case in root.iter("testcase"):
        name = case.get("name", "")
        classname = case.get("classname", "")
        # pytest names a test by its module and its function; ctest leaves
        # the classname empty or equal to the project.
        if classname and classname != name and "." in classname \
                and not classname.endswith(name):
            name = f"{classname}::{name}"
        seconds = float(case.get("time") or 0.0)
        status = "pass"
        if case.find("failure") is not None or case.find("error") is not None:
            status = "fail"
        elif case.find("skipped") is not None \
                or case.get("status") in ("notrun", "disabled"):
            status = "skip"
        elif case.get("status") == "fail":
            status = "fail"
        rows.append([name, round(seconds, 3), status])
    return rows


def read_listener_files(folder: Path) -> list:
    """Rows [suite binary, test suite, case, ms, rss_kb] of every listener file."""
    rows = []
    if not folder.is_dir():
        return rows
    for file in sorted(folder.glob("*.json")):
        try:
            data = json.loads(file.read_text())
        except (OSError, json.JSONDecodeError) as e:
            print(f"benchmark-collect: {file}: {e}", file=sys.stderr)
            continue
        for t in data.get("tests", []):
            rows.append([file.stem, t.get("suite", ""), t.get("name", ""),
                         round(float(t.get("ms", 0.0)), 3),
                         int(t.get("rss_kb", 0) or 0)])
    return rows


def read_measure_files(folder: Path) -> list:
    """Rows [test, wall, user, sys, rss_kb] of every measured test.

    A test that ran several processes sums their times and keeps the
    largest resident set. A line GNU time wrote for a signal is skipped.
    """
    rows = []
    if not folder.is_dir():
        return rows
    for file in sorted(folder.glob("*.jsonl")):
        wall = user = sys_s = 0.0
        rss = 0
        seen = False
        for line in file.read_text().splitlines():
            line = line.strip()
            if not line.startswith("{"):
                continue
            try:
                m = json.loads(line)
            except json.JSONDecodeError:
                continue
            seen = True
            wall += float(m.get("wall", 0.0))
            user += float(m.get("user", 0.0))
            sys_s += float(m.get("sys", 0.0))
            rss = max(rss, int(m.get("rss_kb", 0)))
        if seen:
            rows.append([file.stem, round(wall, 3), round(user, 3),
                         round(sys_s, 3), rss])
    return rows


def read_time_v(path: Path) -> dict:
    """The fields of a /usr/bin/time -v output that the record keeps."""
    out = {}
    if not path.is_file():
        return out
    for line in path.read_text().splitlines():
        if "Maximum resident set size" in line:
            out["rss_kb"] = int(line.rsplit(":", 1)[1])
        elif "User time (seconds)" in line:
            out["user"] = float(line.rsplit(":", 1)[1])
        elif "System time (seconds)" in line:
            out["sys"] = float(line.rsplit(":", 1)[1])
        elif "Elapsed (wall clock) time" in line:
            parts = line.rsplit("): ", 1)[1].split(":")
            secs = 0.0
            for p in parts:
                secs = secs * 60 + float(p)
            out["wall"] = round(secs, 3)
    return out


def read_status(path: Path):
    try:
        return int(path.read_text().strip())
    except (OSError, ValueError):
        return None


def host_info(jobs: int) -> dict:
    cpu = ""
    try:
        for line in Path("/proc/cpuinfo").read_text().splitlines():
            if line.startswith("model name"):
                cpu = line.split(":", 1)[1].strip()
                break
    except OSError:
        pass
    os_name = platform.platform()
    try:
        for line in Path("/etc/os-release").read_text().splitlines():
            if line.startswith("PRETTY_NAME="):
                os_name = line.split("=", 1)[1].strip().strip('"')
                break
    except OSError:
        pass
    return {"os": os_name, "cpu": cpu, "cpus": os.cpu_count() or 0,
            "jobs": jobs}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--kind", choices=("ctest", "pytest"), required=True)
    ap.add_argument("--series", required=True)
    ap.add_argument("--results", required=True, type=Path)
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--sha", default="")
    ap.add_argument("--branch", default="")
    ap.add_argument("--run-id", default="")
    ap.add_argument("--run-url", default="")
    ap.add_argument("--jobs", type=int, default=0)
    ap.add_argument("--date", default="",
                    help="ISO date of the run (default: now, UTC)")
    args = ap.parse_args()

    r = args.results
    record = {
        "series": args.series,
        "sha": args.sha,
        "branch": args.branch,
        "run_id": args.run_id,
        "run_url": args.run_url,
        "date": args.date or dt.datetime.now(dt.timezone.utc)
            .strftime("%Y-%m-%dT%H:%M:%SZ"),
        "host": host_info(args.jobs),
    }
    junit = read_junit(r / "junit.xml")
    failed = sum(1 for row in junit if row[2] == "fail")
    if args.kind == "ctest":
        cases = read_listener_files(r / "benchmarks")
        procs = read_measure_files(r / "measure")
        record["totals"] = {
            "tests": len(junit),
            "failed": failed,
            "ctest_wall_s": round(sum(row[1] for row in junit), 3),
            "case_ms": round(sum(row[3] for row in cases), 3),
            "process_cpu_s": round(sum(row[2] + row[3] for row in procs), 3),
            "process_rss_max_kb": max((row[4] for row in procs), default=0),
            "status": read_status(r / "ctest-status"),
        }
        record["ctest"] = junit
        record["case"] = cases
        record["process"] = procs
    else:
        run = read_time_v(r / "time.txt")
        record["totals"] = {
            "tests": len(junit),
            "failed": failed,
            "pytest_s": round(sum(row[1] for row in junit), 3),
            "run_wall_s": run.get("wall", 0.0),
            "run_cpu_s": round(run.get("user", 0.0) + run.get("sys", 0.0), 3),
            "run_rss_max_kb": run.get("rss_kb", 0),
            "status": read_status(r / "status"),
        }
        record["testnet"] = junit

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(record, separators=(",", ":")) + "\n")
    t = record["totals"]
    print(f"benchmark-collect: {args.series} {args.sha[:8]}: "
          f"{t['tests']} tests, {t['failed']} failed -> {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
