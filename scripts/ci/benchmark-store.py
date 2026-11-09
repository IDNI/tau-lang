#!/usr/bin/env python3
"""Store a benchmark record in the data folder of the benchmarks-data branch.

The record goes to data/<series>/<commit date>-<sha>.json with the flags of
its comparison inside, data/<series>/index.json lists the records newest
first with their totals and per-group totals, a record that leaves the window
of the last --keep records is folded into data/<series>/history.json, which
keeps the totals of every record forever, and data/series.json lists the
series for the dashboard.

The order of a series is the order of its commits: every list sorts by the
commit date of the record (`commit_date`, else the date of the run), so a
commit measured by a manual run after its successors takes its place among
them. A series holds one record per commit: a new record of a commit
replaces the one it has.

Usage:
  benchmark-store.py --data benchmarks-data/data --record record.json \
      [--flags flags.json] [--keep 60]
"""

import argparse
import datetime as dt
import json
import sys
from pathlib import Path

# the group of a ctest entry, by the first matching prefix
GROUP_PREFIXES = (
    ("test_repl-", "repl"), ("test_integration-", "integration"),
    ("test_api", "api"), ("test_ltl", "ltl"), ("test_codegen", "codegen"),
    ("test_cpp_codegen", "codegen"), ("binding-python", "python"),
    ("test_sdk", "sdk"), ("js_", "js"),
)


def group_of(name: str) -> str:
    for prefix, group in GROUP_PREFIXES:
        if name.startswith(prefix):
            return group
    return "unit"


def group_totals(record: dict) -> dict:
    """{group: {tests, failed, wall_s, rss_max_kb}} over the ctest, testnet or
    overload rows; an overload group also counts its points killed at their
    cap (capped), which are not failures.

    The peak memory of a group is the largest peak of its processes; the
    testnet suite is one process, so its group takes the peak of the run.
    """
    groups = {}
    for row in record.get("ctest", []):
        g = groups.setdefault(group_of(row[0]), {"tests": 0, "failed": 0, "wall_s": 0.0})
        g["tests"] += 1
        g["failed"] += row[2] == "fail"
        g["wall_s"] = round(g["wall_s"] + row[1], 3)
    for row in record.get("process", []):
        g = groups.setdefault(group_of(row[0]), {"tests": 0, "failed": 0, "wall_s": 0.0})
        g["rss_max_kb"] = max(g.get("rss_max_kb", 0), row[4])
    for row in record.get("testnet", []):
        g = groups.setdefault("testnet", {"tests": 0, "failed": 0, "wall_s": 0.0})
        g["tests"] += 1
        g["failed"] += row[2] == "fail"
        g["wall_s"] = round(g["wall_s"] + row[1], 3)
    if "testnet" in groups:
        groups["testnet"]["rss_max_kb"] = record.get("totals", {}).get("run_rss_max_kb", 0)
    # an overload row is "<group> / <point>"; each point is its own process
    for row in record.get("overload", []):
        g = groups.setdefault(row[0].split(" / ", 1)[0],
                              {"tests": 0, "failed": 0, "capped": 0, "wall_s": 0.0, "rss_max_kb": 0})
        g["tests"] += 1
        g["failed"] += row[3] == "fail"
        g["capped"] += row[3] == "cap"
        g["wall_s"] = round(g["wall_s"] + row[1], 3)
        g["rss_max_kb"] = max(g["rss_max_kb"], row[2])
    return groups


def commit_date(record: dict) -> str:
    return record.get("commit_date") or record.get("date") or ""


def order_key(record: dict) -> tuple:
    """The sort key of a record or index entry: by commit, then by run."""
    return (commit_date(record), record.get("date") or "")


def record_file_name(record: dict) -> str:
    date = dt.datetime.fromisoformat(commit_date(record).replace("Z", "+00:00"))
    return f"{date:%Y-%m-%dT%H%MZ}-{record.get('sha', '')[:8]}.json"


def index_entry(record: dict, file: str) -> dict:
    return {
        "file": file,
        "date": record.get("date"),
        "commit_date": commit_date(record),
        "sha": record.get("sha"),
        "branch": record.get("branch"),
        "run_id": record.get("run_id"),
        "run_url": record.get("run_url"),
        "level": record.get("level", "none"),
        "cpu_factor": (record.get("cpu_norm") or {}).get("factor", 1.0),
        "totals": record.get("totals", {}),
        "groups": group_totals(record),
        "host": record.get("host", {}),
    }


def load_json(path: Path, default):
    return json.loads(path.read_text()) if path.is_file() else default


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--data", required=True, type=Path)
    ap.add_argument("--record", required=True, type=Path)
    ap.add_argument("--flags", type=Path)
    ap.add_argument("--keep", type=int, default=60)
    args = ap.parse_args()

    record = json.loads(args.record.read_text())
    if args.flags and args.flags.is_file():
        flags = json.loads(args.flags.read_text())
        record["flags"] = flags.get("flags", {})
        record["level"] = flags.get("level", "none")
        if flags.get("cpu"):
            record["cpu_norm"] = flags["cpu"]
    series_dir = args.data / record["series"]
    series_dir.mkdir(parents=True, exist_ok=True)

    file = record_file_name(record)
    (series_dir / file).write_text(json.dumps(record, separators=(",", ":")) + "\n")

    index = load_json(series_dir / "index.json", {"series": record["series"], "records": []})
    # the earlier record of the same commit leaves, whatever its file name
    for e in index["records"]:
        if e.get("sha") == record.get("sha") and e["file"] != file:
            (series_dir / e["file"]).unlink(missing_ok=True)
    index["records"] = [e for e in index["records"]
                        if e.get("sha") != record.get("sha") and e["file"] != file]
    index["records"].insert(0, index_entry(record, file))
    index["records"].sort(key=order_key, reverse=True)

    history = load_json(series_dir / "history.json", {"series": record["series"], "records": []})
    folded = index["records"][args.keep:]
    index["records"] = index["records"][:args.keep]
    for e in folded:
        (series_dir / e["file"]).unlink(missing_ok=True)
        entry = {k: v for k, v in e.items() if k != "file"}
        history["records"] = [h for h in history["records"] if h.get("sha") != e.get("sha")]
        history["records"].append(entry)
    history["records"].sort(key=order_key)

    index["updated"] = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    (series_dir / "index.json").write_text(json.dumps(index, indent=1) + "\n")
    if folded or (series_dir / "history.json").is_file():
        (series_dir / "history.json").write_text(json.dumps(history, indent=1) + "\n")

    # the list of series, for the dashboard
    series = load_json(args.data / "series.json", {"series": []})
    series["series"] = [e for e in series["series"] if e["name"] != record["series"]]
    series["series"].append({"name": record["series"],
                             "latest": commit_date(index["records"][0]),
                             "records": len(index["records"])})
    series["series"].sort(key=lambda e: e["name"])
    (args.data / "series.json").write_text(json.dumps(series, indent=1) + "\n")

    print(f"benchmark-store: {record['series']}/{file} stored, "
          f"{len(index['records'])} records in the window, {len(folded)} folded into history")
    return 0


if __name__ == "__main__":
    sys.exit(main())
