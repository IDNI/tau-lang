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
           status="pass", total=None):
    return {
        "series": "s", "sha": sha, "branch": "devel", "run_id": "1",
        "run_url": "u", "date": f"2026-11-{day:02d}T10:00:00Z",
        "host": {"os": "o", "cpu": "c", "cpus": 4, "jobs": 4},
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
        self.assertIn("median of the last 5 records", s)
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
                         {"unit": {"tests": 2, "failed": 0, "wall_s": 1.1},
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


if __name__ == "__main__":
    unittest.main()
