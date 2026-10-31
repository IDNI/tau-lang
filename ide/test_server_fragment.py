"""The fragment a client selects reaches every one-shot command.

Run from the repository root:

    python -m unittest -v ide.test_server_fragment
"""
from __future__ import annotations

import subprocess
import unittest
from unittest.mock import AsyncMock, patch

import httpx

from ide import server

ENDPOINTS = [
    ("/api/eval", {"command": "sat T."}),
    ("/api/parse", {"formula": "T"}),
    ("/api/normalize", {"formula": "T"}),
    ("/api/check", {"formula": "T"}),
    ("/api/solve", {"formula": "T = T"}),
    ("/api/substitute", {"formula": "x", "target": "x", "replacement": "y"}),
]


class FragmentTests(unittest.IsolatedAsyncioTestCase):

    async def asyncSetUp(self):
        binary = patch.object(server, "get_tau_binary", return_value="tau")
        binary.start()
        self.addCleanup(binary.stop)
        run = patch.object(server, "_run_tau", new=AsyncMock(
            side_effect=lambda args, timeout: subprocess.CompletedProcess(
                args, 0, "fragment: ctl_star\n\n%1: T\n", "")))
        self.run_tau = run.start()
        self.addCleanup(run.stop)
        self.client = httpx.AsyncClient(
            transport=httpx.ASGITransport(app=server.app), base_url="http://127.0.0.1")
        self.addAsyncCleanup(self.client.aclose)

    async def test_ctl_star_prefixes_every_endpoint(self):
        for path, body in ENDPOINTS:
            with self.subTest(path=path):
                resp = await self.client.post(path, json={**body, "fragment": "ctl_star"})
                self.assertEqual(resp.status_code, 200)
                self.assertEqual(resp.json()["output"], "%1: T\n")
                args = self.run_tau.await_args.args[0]
                self.assertTrue(args[2].startswith("fragment ctl_star. "), args[2])

    async def test_ltl_and_no_fragment_send_the_command_alone(self):
        for extra in ({}, {"fragment": "ltl"}):
            await self.client.post("/api/eval", json={"command": "sat T.", **extra})
            self.assertEqual(self.run_tau.await_args.args[0][2], "sat T.")

    async def test_unknown_fragment_runs_nothing(self):
        resp = await self.client.post(
            "/api/eval", json={"command": "sat T.", "fragment": "pnf"})
        self.assertEqual(resp.status_code, 500)
        self.assertIn("Unknown fragment", resp.json()["error"])
        self.run_tau.assert_not_awaited()


if __name__ == "__main__":
    unittest.main()
