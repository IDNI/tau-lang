#!/usr/bin/env python3
"""Tests of scripts/ci/benchmark-collect.py over synthetic results.

Run: python3 -I scripts/ci/tests/test_benchmark_collect.py
"""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

COLLECT = Path(__file__).resolve().parents[1] / "benchmark-collect.py"

CTEST_JUNIT = """<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="(empty)" tests="3" failures="1" disabled="0" skipped="1">
  <testcase name="test_bool" classname="(empty)" time="0.41" status="run"/>
  <testcase name="test_repl-pack_gate" classname="(empty)" time="2.5" status="fail">
    <failure message="Failed"/>
  </testcase>
  <testcase name="test_repl-hostfs_case" classname="(empty)" time="0" status="disabled">
    <skipped message="Disabled"/>
  </testcase>
</testsuite>
"""

PYTEST_JUNIT = """<?xml version="1.0" encoding="utf-8"?>
<testsuites><testsuite name="pytest" tests="2" failures="0" skipped="1">
  <testcase classname="tests.test_consensus" name="test_consensus_time" time="12.345"/>
  <testcase classname="tests.test_node" name="test_sync[2]" time="0.001"><skipped/></testcase>
</testsuite></testsuites>
"""

TIME_V = """\tCommand being timed: "python -m pytest"
\tUser time (seconds): 100.50
\tSystem time (seconds): 4.25
\tElapsed (wall clock) time (h:mm:ss or m:ss): 1:02:03
\tMaximum resident set size (kbytes): 654321
"""


def run_collect(*args):
    return subprocess.run([sys.executable, "-I", str(COLLECT), *args],
                          capture_output=True, text=True, check=True)


class CollectCtest(unittest.TestCase):
    def test_merges_the_three_sources(self):
        with tempfile.TemporaryDirectory() as tmp:
            r = Path(tmp) / "results"
            (r / "benchmarks").mkdir(parents=True)
            (r / "measure").mkdir()
            (r / "junit.xml").write_text(CTEST_JUNIT)
            (r / "ctest-status").write_text("8\n")
            (r / "benchmarks" / "test_bool.json").write_text(json.dumps({
                "tests": [
                    {"suite": "bool_ba", "name": "and", "ms": 0.5, "rss_kb": 100, "passed": True},
                    {"suite": "bool_ba", "name": "or", "ms": 1.25, "rss_kb": 2048, "passed": True},
                ], "rss_kb_per_case": True, "total_tests": 2, "passed": 2, "failed": 0}))
            (r / "measure" / "test_bool.jsonl").write_text(
                '{"wall": 0.40, "user": 0.30, "sys": 0.05, "rss_kb": 50000, "status": 0}\n')
            # a case that ran tau twice, the second run killed by a signal
            (r / "measure" / "test_repl-pack_gate.jsonl").write_text(
                '{"wall": 1.0, "user": 0.9, "sys": 0.1, "rss_kb": 70000, "status": 0}\n'
                'Command terminated by signal 9\n'
                '{"wall": 1.5, "user": 1.2, "sys": 0.2, "rss_kb": 90000, "status": 0}\n')
            out = Path(tmp) / "out" / "record.json"
            run_collect("--kind", "ctest", "--series", "linux-x86_64-release",
                        "--results", str(r), "--out", str(out), "--sha", "abcdef12",
                        "--branch", "devel", "--run-id", "7", "--jobs", "4",
                        "--date", "2026-11-07T10:00:00Z")
            rec = json.loads(out.read_text())
        self.assertEqual(rec["series"], "linux-x86_64-release")
        self.assertEqual(rec["sha"], "abcdef12")
        self.assertEqual(rec["date"], "2026-11-07T10:00:00Z")
        self.assertEqual(rec["host"]["jobs"], 4)
        self.assertEqual(rec["ctest"], [
            ["test_bool", 0.41, "pass"],
            ["test_repl-pack_gate", 2.5, "fail"],
            ["test_repl-hostfs_case", 0.0, "skip"]])
        self.assertEqual(rec["case"], [
            ["test_bool", "bool_ba", "and", 0.5, 100],
            ["test_bool", "bool_ba", "or", 1.25, 2048]])
        self.assertEqual(rec["process"], [
            ["test_bool", 0.4, 0.3, 0.05, 50000],
            ["test_repl-pack_gate", 2.5, 2.1, 0.3, 90000]])
        t = rec["totals"]
        self.assertEqual(t["tests"], 3)
        self.assertEqual(t["failed"], 1)
        self.assertEqual(t["status"], 8)
        self.assertAlmostEqual(t["ctest_wall_s"], 2.91)
        self.assertAlmostEqual(t["case_ms"], 1.75)
        self.assertAlmostEqual(t["process_cpu_s"], 2.75)
        self.assertEqual(t["process_rss_max_kb"], 90000)

    def test_missing_inputs_give_an_empty_record(self):
        with tempfile.TemporaryDirectory() as tmp:
            r = Path(tmp) / "results"
            r.mkdir()
            out = Path(tmp) / "record.json"
            run_collect("--kind", "ctest", "--series", "s", "--results", str(r),
                        "--out", str(out))
            rec = json.loads(out.read_text())
        self.assertEqual(rec["ctest"], [])
        self.assertEqual(rec["case"], [])
        self.assertEqual(rec["process"], [])
        self.assertIsNone(rec["totals"]["status"])
        self.assertEqual(rec["totals"]["process_rss_max_kb"], 0)


class CollectPytest(unittest.TestCase):
    def test_reads_junit_and_time(self):
        with tempfile.TemporaryDirectory() as tmp:
            r = Path(tmp) / "results"
            r.mkdir()
            (r / "junit.xml").write_text(PYTEST_JUNIT)
            (r / "time.txt").write_text(TIME_V)
            (r / "status").write_text("0\n")
            out = Path(tmp) / "record.json"
            run_collect("--kind", "pytest", "--series", "testnet",
                        "--results", str(r), "--out", str(out))
            rec = json.loads(out.read_text())
        self.assertEqual(rec["testnet"], [
            ["tests.test_consensus::test_consensus_time", 12.345, "pass"],
            ["tests.test_node::test_sync[2]", 0.001, "skip"]])
        t = rec["totals"]
        self.assertEqual(t["tests"], 2)
        self.assertEqual(t["failed"], 0)
        self.assertEqual(t["status"], 0)
        self.assertAlmostEqual(t["pytest_s"], 12.346)
        self.assertAlmostEqual(t["run_wall_s"], 3723.0)
        self.assertAlmostEqual(t["run_cpu_s"], 104.75)
        self.assertEqual(t["run_rss_max_kb"], 654321)


if __name__ == "__main__":
    unittest.main()
