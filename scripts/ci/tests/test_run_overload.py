#!/usr/bin/env python3
"""Tests of the account-rules-repro rows of tests/benchmark/testnet/run_overload.py
and of the group order of the dashboard (docs/benchmarks/app.js).

Run: python3 -I scripts/ci/tests/test_run_overload.py
"""

import importlib.util
import json
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[3]
SPEC = importlib.util.spec_from_file_location(
    "run_overload", ROOT / "tests/benchmark/testnet/run_overload.py")
ro = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(ro)

G = "account-rules-repro"


def point(kind, status="ok", **fields):
    params = fields.pop("params", {})
    return {"kind": kind, "status": status, "params": params, "wall_s": 9.0,
            "peak_rss_mb": 2.0, **fields}


def block(txs, build, step, shared=None):
    return {"txs": txs, "build_ms": build, "step_ms": step, "shared_ms": shared,
            "rss_mb": 50.0, "tref": 10}


class PointRows(unittest.TestCase):
    def test_parts(self):
        rows = ro.account_point_rows(point("parts", params={"n": 16}, open_s=0.5,
                                           step_mean_ms=20.0))
        self.assertEqual(rows, [[f"{G} / parts N=16 open", 0.5, 2048, "pass"],
                                [f"{G} / parts N=16 step", 0.02, 0, "pass"]])

    def test_parts_killed_before_open(self):
        rows = ro.account_point_rows(point("parts", "cap", params={"n": 1024}))
        self.assertEqual(rows, [[f"{G} / parts N=1024 open", 9.0, 2048, "cap"]])

    def test_parts_killed_after_open(self):
        rows = ro.account_point_rows(point("parts", "rss", params={"n": 1024}, open_s=200.0))
        self.assertEqual(rows, [[f"{G} / parts N=1024 open", 200.0, 2048, "cap"]])

    def test_parts_wrong_verdict_fails(self):
        rows = ro.account_point_rows(point("parts", params={"n": 16}, open_s=0.5,
                                           step_mean_ms=20.0, verdicts_wrong=1))
        self.assertEqual({r[3] for r in rows}, {"fail"})

    def test_add_one(self):
        self.assertEqual(ro.account_point_rows(point("add-one", params={"n": 64}, feed_s=0.2)),
                         [[f"{G} / add-one N=64 update", 0.2, 2048, "pass"]])
        self.assertEqual(ro.account_point_rows(point("add-one", "cap", params={"n": 256})),
                         [[f"{G} / add-one N=256 update", 9.0, 2048, "cap"]])

    def test_live(self):
        rows = ro.account_point_rows(point("live", params={"count": 200}, rss_before_mb=50.0,
                                           rss_after_mb=80.0))
        self.assertEqual(rows, [[f"{G} / live 200 interpreters", 9.0, 30720, "pass"]])

    def test_live_without_memory_takes_the_peak(self):
        rows = ro.account_point_rows(point("live", "error", params={"count": 200}))
        self.assertEqual(rows, [[f"{G} / live 200 interpreters", 9.0, 2048, "fail"]])

    def test_churn(self):
        blocks = [block(100, 30.0, 1.5)] + [block(t, 31.0, 2.0) for t in range(200, 1001, 100)]
        rows = ro.account_point_rows(point("churn", params={"k": 1000, "shared": False},
                                           blocks=blocks))
        self.assertEqual(rows, [
            [f"{G} / churn 1000 build first 100", 3.0, 0, "pass"],
            [f"{G} / churn 1000 step first 100", 0.15, 0, "pass"],
            [f"{G} / churn 1000 build last 100", 3.1, 0, "pass"],
            [f"{G} / churn 1000 step last 100", 0.2, 0, "pass"],
            [f"{G} / churn 1000 run", 9.0, 2048, "pass"]])

    def test_churn_shared_one_block(self):
        rows = ro.account_point_rows(point("churn", params={"k": 50, "shared": True},
                                           blocks=[block(50, 30.0, 2.0, 1.0)]))
        names = [r[0] for r in rows]
        self.assertIn(f"{G} / churn 50 with-shared shared step first 50", names)
        self.assertIn(f"{G} / churn 50 with-shared build last 50", names)
        self.assertEqual(rows[-1], [f"{G} / churn 50 with-shared run", 9.0, 2048, "pass"])

    def test_churn_without_blocks(self):
        # a run that ends with no block measured nothing: it fails, unless it was capped
        ok = ro.account_point_rows(point("churn", params={"k": 1000}))
        self.assertEqual(ok, [[f"{G} / churn 1000 run", 9.0, 2048, "fail"]])
        cap = ro.account_point_rows(point("churn", "cap", params={"k": 1000}))
        self.assertEqual(cap, [[f"{G} / churn 1000 run", 9.0, 2048, "cap"]])

    def test_single_is_folded(self):
        self.assertEqual(ro.account_point_rows(point("single", build_ms=30.0)), [])


class AccountRows(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.args = SimpleNamespace(python="python3", module_dir="m", cap=10.0, max_rss_mb=1024)
        self.saved_run = ro.run

    def tearDown(self):
        ro.run = self.saved_run
        shutil.rmtree(self.tmp)

    def sweep(self, points, rc=0, write=True):
        def fake_run(cmd, log_file, cap, env=None):
            self.cmd, self.cap = cmd, cap
            if write:
                Path(cmd[cmd.index("--json") + 1]).write_text(json.dumps({"points": points}))
            return rc, 42.0, 0
        ro.run = fake_run
        return ro.account_rows(self.args, self.tmp)

    def test_command(self):
        self.sweep([])
        self.assertEqual(self.cmd[3:5], ["sweep", "--quick"])
        self.assertEqual(self.cmd[self.cmd.index("--cap") + 1], "10.0")
        self.assertEqual(self.cmd[self.cmd.index("--rss-cap-gb") + 1], "1.0")
        self.assertEqual(self.cap, 10.0 * ro.ACCOUNT_POINTS + 300)

    def test_no_results(self):
        self.assertEqual(self.sweep([], rc=1, write=False),
                         [[f"{G} / run", 42.0, 0, "fail"]])

    def test_single_mean(self):
        rows = self.sweep([point("single", build_ms=20.0, peak_rss_mb=1.0),
                           point("single", build_ms=40.0, peak_rss_mb=3.0)])
        self.assertEqual(rows, [[f"{G} / single build", 0.03, 3072, "pass"]])

    def test_single_statuses(self):
        capped = self.sweep([point("single", build_ms=20.0), point("single", "cap")])
        self.assertEqual(capped[0][1:], [0.02, 2048, "cap"])
        died = self.sweep([point("single", build_ms=20.0), point("single", "died")])
        self.assertEqual(died[0][3], "fail")
        wrong = self.sweep([point("single", build_ms=20.0, verdicts_wrong=1)])
        self.assertEqual(wrong[0][3], "fail")
        none = self.sweep([point("single", "cap")])
        self.assertEqual(none[0][1:], [0.0, 2048, "cap"])

    def test_run_killed(self):
        rows = self.sweep([point("add-one", params={"n": 16}, feed_s=0.1)], rc=None)
        self.assertEqual(rows[-1], [f"{G} / run", 42.0, 0, "cap"])

    def test_run_failed_without_a_failed_point(self):
        rows = self.sweep([point("add-one", params={"n": 16}, feed_s=0.1)], rc=1)
        self.assertEqual(rows[-1], [f"{G} / run", 42.0, 0, "fail"])

    def test_run_failed_with_a_failed_point(self):
        rows = self.sweep([point("add-one", "error", params={"n": 16})], rc=1)
        self.assertEqual(rows, [[f"{G} / add-one N=16 update", 9.0, 2048, "fail"]])


@unittest.skipUnless(shutil.which("node"), "node is not installed")
class DashboardGroups(unittest.TestCase):
    def groups_of(self, maps):
        source = (ROOT / "docs/benchmarks/app.js").read_text()
        code = re.search(r"^const GROUP_ORDER.*?^};$", source, re.S | re.M).group(0)
        out = subprocess.run(["node", "-e", code + f"\nconsole.log(JSON.stringify(groupsOf({json.dumps(maps)})))"],
                             capture_output=True, text=True, check=True).stdout
        return json.loads(out)

    def test_listed_first_then_by_name(self):
        self.assertEqual(self.groups_of([{"load-test-bv": 1, "zeta": 1},
                                         {G: 1, "alpha": 1, "dispatch-blowup": 1}, None]),
                         ["dispatch-blowup", "load-test-bv", G, "alpha", "zeta"])

    def test_empty(self):
        self.assertEqual(self.groups_of([None, {}]), [])


if __name__ == "__main__":
    unittest.main()
