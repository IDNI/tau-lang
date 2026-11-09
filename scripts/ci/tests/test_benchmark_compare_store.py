#!/usr/bin/env python3
"""Tests of benchmark-compare.py and benchmark-store.py over synthetic records.

Run: python3 -I scripts/ci/tests/test_benchmark_compare_store.py
"""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

CI = Path(__file__).resolve().parents[1]
COMPARE, STORE = CI / "benchmark-compare.py", CI / "benchmark-store.py"


def record(sha, day, slow=1.0, quick=0.10, mem=100000, case_ms=10.0,
           status="pass", total=None, run_day=None, cpu="c"):
    """A record of a commit of `day`, measured on `run_day` (default: the same day)."""
    return {
        "series": "s", "sha": sha, "branch": "devel", "run_id": "1",
        "run_url": "u", "date": f"2026-11-{run_day or day:02d}T10:00:00Z",
        "commit_date": f"2026-11-{day:02d}T10:00:00Z",
        "host": {"os": "o", "cpu": cpu, "cpus": 4, "jobs": 4},
        "totals": {"tests": 3, "failed": 0,
                   "ctest_wall_s": total if total is not None else slow + quick + 1.0,
                   "case_ms": case_ms, "process_cpu_s": slow,
                   "process_rss_max_kb": mem},
        "ctest": [["test_slow", slow, status], ["test_quick", quick, "pass"],
                  ["test_repl-x", 1.0, "pass"]],
        "case": [["test_slow", "suite", "case", case_ms, mem]],
        "process": [["test_slow", slow, slow - 0.01, 0.01, mem]],
    }


def run(script, *args, check=True):
    return subprocess.run([sys.executable, "-I", str(script), *args],
                          capture_output=True, text=True, check=check)


def store(data, rec, flags=None, keep=60):
    f = data.parent / f"rec-{rec['sha']}.json"
    f.write_text(json.dumps(rec))
    args = ["--data", str(data), "--record", str(f), "--keep", str(keep)]
    if flags:
        args += ["--flags", str(flags)]
    return run(STORE, *args)


def compare(data, rec, tmp):
    f = tmp / f"cur-{rec['sha']}.json"
    f.write_text(json.dumps(rec))
    out = {k: tmp / f"{k}-{rec['sha']}" for k in ("summary", "comment", "flags")}
    p = run(COMPARE, "--current", str(f), "--data", str(data / "s"),
            "--summary", str(out["summary"]), "--comment", str(out["comment"]),
            "--flags", str(out["flags"]), "--annotations",
            "--dashboard-url", "https://example/dash", check=False)
    flags = json.loads(out["flags"].read_text())
    return p, flags, out


class Compare(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.data = self.tmp / "data"
        for i in range(5):
            # one noisy baseline: the median ignores it
            store(self.data, record(f"base{i}", day=i + 1,
                                    slow=3.0 if i == 2 else 1.0))

    def test_no_baseline_is_not_an_error(self):
        p, flags, out = compare(self.tmp / "nodata" / "data", record("new", 9), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertEqual(flags["level"], "none")
        self.assertIn("No baseline yet", out["summary"].read_text())
        self.assertFalse(out["comment"].exists())

    def test_within_thresholds(self):
        p, flags, out = compare(self.data, record("ok", 9, slow=1.08), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertEqual(flags["flags"], {})
        self.assertEqual(flags["baselines"], ["base4", "base3", "base2", "base1", "base0"])
        s = out["summary"].read_text()
        self.assertIn("No test over the thresholds.", s)
        self.assertIn("median of the 5 records before this commit", s)
        self.assertIn("Slowest ctest entries", s)
        self.assertFalse(out["comment"].exists())

    def test_floor_hides_a_small_absolute_change(self):
        # 0.10 s -> 0.20 s is +100 % but under the 0.25 s floor
        p, flags, _ = compare(self.data, record("floor", 9, quick=0.20), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertNotIn("ctest|time|test_quick", flags["flags"])

    def test_warning(self):
        p, flags, out = compare(self.data, record("warn", 9, slow=1.4), self.tmp)
        self.assertEqual(p.returncode, 0)
        f = flags["flags"]["ctest|time|test_slow"]
        self.assertEqual((f["level"], f["candidate"], f["streak"], f["fails"]),
                         ("warn", "warn", 1, 0))
        self.assertEqual(f["baseline"], 1.0)
        self.assertIn("::warning title=Benchmark s::test_slow (ctest time) 1.00 s -> 1.40 s (+40 %",
                      p.stdout)
        self.assertIn("| warn | `test_slow` (ctest) | 1.00 s | 1.00 s | 3.00 s | 1.00 s | 1.00 s | **1.40 s** | +40 % |",
                      out["comment"].read_text())
        self.assertIn("[dashboard](https://example/dash)", out["comment"].read_text())

    def test_failure_needs_two_records(self):
        p, flags, _ = compare(self.data, record("f1", 9, slow=2.5, mem=250000), self.tmp)
        self.assertEqual(p.returncode, 0, p.stdout)
        f = flags["flags"]["ctest|time|test_slow"]
        self.assertEqual((f["level"], f["candidate"], f["fails"]), ("warn", "fail", 1))
        m = flags["flags"]["process|memory|test_slow"]
        self.assertEqual((m["level"], m["candidate"], m["unit"]), ("warn", "fail", "kb"))
        # the record stored with its flags makes the next one a failure
        store(self.data, record("f1", 9, slow=2.5, mem=250000),
              flags=self.tmp / "flags-f1")
        p, flags, out = compare(self.data, record("f2", 10, slow=2.6, mem=260000), self.tmp)
        self.assertEqual(p.returncode, 2)
        f = flags["flags"]["ctest|time|test_slow"]
        self.assertEqual((f["level"], f["streak"], f["fails"]), ("fail", 2, 2))
        self.assertEqual(flags["level"], "fail")
        self.assertIn("::error title=Benchmark s::test_slow", p.stdout)
        self.assertIn("Over the thresholds (fail)", out["summary"].read_text())
        # a totals row is flagged too
        self.assertIn("totals|s|ctest_wall_s", flags["flags"])

    def test_a_failing_test_is_not_compared(self):
        p, flags, _ = compare(self.data, record("red", 9, slow=9.0, status="fail"), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertNotIn("ctest|time|test_slow", flags["flags"])

    def test_an_older_commit_is_compared_with_the_records_before_it(self):
        # a commit of day 3 measured on day 9: base3 and base4 are after it,
        # and base2, of the same day but measured earlier, is before it
        p, flags, out = compare(self.data, record("old", 3, run_day=9, slow=1.4), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertEqual(flags["baselines"], ["base2", "base1", "base0"])
        self.assertIn("median of the 3 records before this commit", out["summary"].read_text())
        # a commit before every record has no baseline, whenever it runs
        first = record("first", 1, run_day=9)
        first["commit_date"] = "2026-10-31T10:00:00Z"
        p, flags, out = compare(self.data, first, self.tmp)
        self.assertEqual(flags["baselines"], [])
        self.assertIn("No baseline yet", out["summary"].read_text())

    def test_times_are_normalized_by_cpu(self):
        # a second runner model, 30 % slower on every record it measured
        tmp = Path(tempfile.mkdtemp())
        data = tmp / "data"
        for i in range(6):
            slow_cpu = i % 2 == 1
            store(data, record(f"r{i}", day=i + 1, slow=1.3 if slow_cpu else 1.0,
                               quick=0.13 if slow_cpu else 0.10, cpu="B" if slow_cpu else "A"))
        # a record on the slow model at its usual pace is not a regression
        p, flags, out = compare(data, record("cur", 9, slow=1.3, quick=0.13, cpu="B"), tmp)
        self.assertEqual(p.returncode, 0)
        self.assertNotIn("ctest|time|test_slow", flags["flags"])
        self.assertEqual(flags["cpu"]["reference"], "A")
        self.assertAlmostEqual(flags["cpu"]["factor"], 1.3, places=2)
        self.assertIn("Times normalized by CPU: this record ×1/1.300; reference A, factors B ×1.300.",
                      out["summary"].read_text())
        # the store keeps the normalization with the record
        store(data, record("cur", 9, slow=1.3, quick=0.13, cpu="B"), flags=tmp / "flags-cur")
        index = json.loads((data / "s" / "index.json").read_text())
        self.assertAlmostEqual(index["records"][0]["cpu_factor"], 1.3, places=2)
        # the same values on the fast model are 30 % over its baseline
        p, flags, _ = compare(data, record("cur", 9, slow=1.3, quick=0.13, cpu="A"), tmp)
        self.assertEqual(flags["flags"]["ctest|time|test_slow"]["level"], "warn")
        # a model with a single record gets no factor
        p, flags, _ = compare(data, record("cur", 9, slow=1.3, cpu="C"), tmp)
        self.assertEqual(flags["cpu"]["factor"], 1.0)
        self.assertIn("ctest|time|test_slow", flags["flags"])

    def test_cpu_factors_are_learnt_over_every_series(self):
        # series s: model B measured only an old commit, 30 times slower before a
        # fix, and one at the usual pace; series t: B is 1.1 times slower than A
        tmp = Path(tempfile.mkdtemp())
        data = tmp / "data"
        for i in range(6):
            cpu = "B" if i in (0, 4) else "A"
            store(data, record(f"s{i}", day=i + 1, slow=30.0 if i == 0 else 1.0, cpu=cpu))
        for i in range(6):
            cpu = "B" if i % 3 == 0 else "A"
            rec = record(f"t{i}", day=i + 1, slow=1.1 if cpu == "B" else 1.0, cpu=cpu)
            rec["series"] = "t"
            store(data, rec)
        # learnt from s alone the factor of B would be the median of 30 and 1
        p, flags, _ = compare(data, record("cur", 9, slow=1.1, cpu="B"), tmp)
        self.assertEqual(flags["cpu"]["reference"], "A")
        self.assertAlmostEqual(flags["cpu"]["factor"], 1.1, places=2)
        self.assertEqual(flags["cpu"]["factors"], {"A": 1.0, "B": flags["cpu"]["factor"]})
        self.assertNotIn("ctest|time|test_slow", flags["flags"])

    def test_a_record_of_the_same_commit_is_not_its_own_baseline(self):
        p, flags, _ = compare(self.data, record("base4", 5, run_day=9, slow=1.4), self.tmp)
        self.assertEqual(p.returncode, 0)
        self.assertEqual(flags["baselines"], ["base3", "base2", "base1", "base0"])


def overload_record(sha, day, load=1.0, mem=90000, status="pass"):
    return {
        "series": "s", "sha": sha, "branch": "devel", "run_id": "1",
        "run_url": "u", "date": f"2026-11-{day:02d}T10:00:00Z",
        "commit_date": f"2026-11-{day:02d}T10:00:00Z",
        "host": {"os": "o", "cpu": "c", "cpus": 4, "jobs": 1},
        "totals": {"tests": 3, "failed": 0, "overload_s": load + 0.6,
                   "run_cpu_s": load + 0.6, "run_rss_max_kb": mem},
        "overload": [["dispatch-blowup / chain N=32 spec W=8", load, mem, status],
                     ["clause-compile / flat g=8 build", 0.5, 40000, "pass"],
                     ["clause-compile / flat g=8 step", 0.1, 0, "pass"]],
    }


class Overload(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.data = self.tmp / "data"
        for i in range(5):
            store(self.data, overload_record(f"base{i}", day=i + 1))

    def test_groups_of_the_index(self):
        index = json.loads((self.data / "s" / "index.json").read_text())
        self.assertEqual(index["records"][0]["groups"], {
            "dispatch-blowup": {"tests": 1, "failed": 0, "capped": 0, "wall_s": 1.0,
                                "rss_max_kb": 90000},
            "clause-compile": {"tests": 2, "failed": 0, "capped": 0, "wall_s": 0.6,
                               "rss_max_kb": 40000}})

    def test_a_capped_point_is_counted_apart_from_the_failures(self):
        store(self.data, overload_record("capped", 9, status="cap"))
        store(self.data, overload_record("failed", 10, status="fail"))
        index = json.loads((self.data / "s" / "index.json").read_text())
        groups = {e["sha"]: e["groups"]["dispatch-blowup"] for e in index["records"]}
        self.assertEqual((groups["capped"]["failed"], groups["capped"]["capped"]), (0, 1))
        self.assertEqual((groups["failed"]["failed"], groups["failed"]["capped"]), (1, 0))

    def test_time_and_memory_of_a_point_are_compared(self):
        p, flags, out = compare(self.data, overload_record("slow", 9, load=40.0, mem=900000),
                                self.tmp)
        self.assertEqual(p.returncode, 0, p.stdout)
        key = "dispatch-blowup / chain N=32 spec W=8"
        self.assertEqual(flags["flags"][f"overload|time|{key}"]["candidate"], "fail")
        self.assertEqual(flags["flags"][f"overload|memory|{key}"]["candidate"], "fail")
        # a step row has no memory of its own, so it is never compared on memory
        self.assertNotIn("overload|memory|clause-compile / flat g=8 step", flags["flags"])
        self.assertIn("Slowest tau-overload points", out["summary"].read_text())

    def test_a_point_killed_at_its_cap_is_compared(self):
        _, flags, _ = compare(self.data, overload_record("cap", 9, load=300.0, status="cap"),
                              self.tmp)
        self.assertIn("overload|time|dispatch-blowup / chain N=32 spec W=8", flags["flags"])
        _, flags, _ = compare(self.data, overload_record("bad", 9, load=300.0, status="fail"),
                              self.tmp)
        self.assertNotIn("overload|time|dispatch-blowup / chain N=32 spec W=8", flags["flags"])


class Store(unittest.TestCase):
    def test_index_groups_and_retention(self):
        tmp = Path(tempfile.mkdtemp())
        data = tmp / "data"
        for i in range(3):
            store(data, record(f"r{i}", day=i + 1), keep=2)
        series = data / "s"
        index = json.loads((series / "index.json").read_text())
        self.assertEqual([e["sha"] for e in index["records"]], ["r2", "r1"])
        self.assertEqual(index["records"][0]["file"], "2026-11-03T1000Z-r2.json")
        self.assertEqual(index["records"][0]["groups"],
                         {"unit": {"tests": 2, "failed": 0, "wall_s": 1.1, "rss_max_kb": 100000},
                          "repl": {"tests": 1, "failed": 0, "wall_s": 1.0}})
        self.assertEqual(index["records"][0]["level"], "none")
        history = json.loads((series / "history.json").read_text())
        self.assertEqual([e["sha"] for e in history["records"]], ["r0"])
        self.assertNotIn("file", history["records"][0])
        self.assertEqual(sorted(p.name for p in series.glob("*.json")),
                         ["2026-11-02T1000Z-r1.json", "2026-11-03T1000Z-r2.json",
                          "history.json", "index.json"])
        listed = json.loads((data / "series.json").read_text())["series"]
        self.assertEqual(listed, [{"name": "s", "latest": "2026-11-03T10:00:00Z", "records": 2}])

    def test_restoring_the_same_record_replaces_it(self):
        tmp = Path(tempfile.mkdtemp())
        data = tmp / "data"
        store(data, record("r0", 1))
        flags = tmp / "flags.json"
        flags.write_text(json.dumps({"level": "warn", "flags": {"k": {"level": "warn"}}}))
        store(data, record("r0", 1), flags=flags)
        index = json.loads((data / "s" / "index.json").read_text())
        self.assertEqual(len(index["records"]), 1)
        self.assertEqual(index["records"][0]["level"], "warn")
        stored = json.loads((data / "s" / index["records"][0]["file"]).read_text())
        self.assertEqual(stored["flags"], {"k": {"level": "warn"}})

    def test_records_follow_the_commits_not_the_runs(self):
        tmp = Path(tempfile.mkdtemp())
        data = tmp / "data"
        store(data, record("r1", 2))
        store(data, record("r3", 4))
        # a commit of day 3 measured on day 9 takes its place between them
        store(data, record("r2", 3, run_day=9))
        series = data / "s"
        index = json.loads((series / "index.json").read_text())
        self.assertEqual([e["sha"] for e in index["records"]], ["r3", "r2", "r1"])
        self.assertEqual(index["records"][1]["file"], "2026-11-03T1000Z-r2.json")
        self.assertEqual(index["records"][1]["commit_date"], "2026-11-03T10:00:00Z")
        self.assertEqual(index["records"][1]["date"], "2026-11-09T10:00:00Z")
        listed = json.loads((data / "series.json").read_text())["series"]
        self.assertEqual(listed[0]["latest"], "2026-11-04T10:00:00Z")
        # a second measurement of a commit replaces the first, whatever its file
        store(data, record("r2", 3, run_day=10, slow=2.0))
        index = json.loads((series / "index.json").read_text())
        self.assertEqual([e["sha"] for e in index["records"]], ["r3", "r2", "r1"])
        self.assertEqual(index["records"][1]["date"], "2026-11-10T10:00:00Z")
        self.assertEqual(sorted(p.name for p in series.glob("*-r2.json")),
                         ["2026-11-03T1000Z-r2.json"])
        # the window and the history follow the same order
        store(data, record("r0", 1, run_day=11), keep=2)
        index = json.loads((series / "index.json").read_text())
        self.assertEqual([e["sha"] for e in index["records"]], ["r3", "r2"])
        history = json.loads((series / "history.json").read_text())
        self.assertEqual([e["sha"] for e in history["records"]], ["r0", "r1"])


if __name__ == "__main__":
    unittest.main()
