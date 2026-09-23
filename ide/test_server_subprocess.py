"""One-shot tau commands must not block the IDE's event loop (issue #133).

Run from the repository root:

    python -m unittest -v ide.test_server_subprocess

A fake tau (a small python script) stands in for the binary, so no build is
needed; it records its pid so the tests can check that no child is orphaned.
"""
from __future__ import annotations

import asyncio
import os
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path
from unittest.mock import patch

import httpx

from ide import server


def _alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    return True


class SubprocessTests(unittest.IsolatedAsyncioTestCase):

    async def asyncSetUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="tau-ide-test-"))
        self.pid_file = self.dir / "pid"
        self.fake = self.dir / "tau"
        self.fake.write_text(
            f"#!{sys.executable}\n"
            "import os, sys, time\n"
            f"open({str(self.pid_file)!r}, 'w').write(str(os.getpid()))\n"
            "time.sleep(float(os.environ.get('FAKE_TAU_SLEEP', '0.5')))\n"
            "print('%1: T')\n"
            "print('args:', ' '.join(sys.argv[1:]), file=sys.stderr)\n")
        self.fake.chmod(0o755)
        binary = patch.object(server, "get_tau_binary", return_value=str(self.fake))
        binary.start()
        self.addCleanup(binary.stop)
        self.client = httpx.AsyncClient(
            transport=httpx.ASGITransport(app=server.app), base_url="http://ide")
        self.addAsyncCleanup(self.client.aclose)

    async def _wait_for_child(self) -> int:
        deadline = time.monotonic() + 5
        while not self.pid_file.exists():
            if time.monotonic() > deadline:
                self.fail("the fake tau never started")
            await asyncio.sleep(0.01)
        await asyncio.sleep(0.05)  # let the pid write finish
        return int(self.pid_file.read_text())

    async def test_status_is_served_while_an_evaluation_runs(self):
        done = []

        async def evaluate():
            r = await self.client.post("/api/eval", json={"command": "sat T"})
            done.append("eval")
            return r

        async def status():
            await self._wait_for_child()
            r = await self.client.get("/api/status")
            done.append("status")
            return r

        ev, st = await asyncio.gather(evaluate(), status())
        self.assertEqual(done, ["status", "eval"])
        self.assertEqual(st.status_code, 200)
        self.assertEqual(ev.status_code, 200)
        self.assertEqual(ev.json()["output"], "%1: T\n")
        self.assertEqual(ev.json()["exit_code"], 0)

    async def test_every_one_shot_route_answers(self):
        cases = [
            ("eval", {"command": "sat T"}),
            ("run", {"source": "T."}),
            ("parse", {"formula": "T"}),
            ("normalize", {"formula": "T", "form": "dnf"}),
            ("check", {"formula": "T", "type": "valid"}),
            ("solve", {"formula": "x = 0"}),
            ("substitute", {"formula": "x", "target": "x", "replacement": "y"}),
        ]
        with patch.dict(os.environ, {"FAKE_TAU_SLEEP": "0"}):
            for route, body in cases:
                with self.subTest(route=route):
                    r = await self.client.post(f"/api/{route}", json=body)
                    self.assertEqual(r.status_code, 200)
                    self.assertEqual(r.json()["output"], "%1: T\n")

    async def test_run_removes_its_source_file(self):
        with patch.dict(os.environ, {"FAKE_TAU_SLEEP": "0"}):
            r = await self.client.post("/api/run", json={"source": "T."})
        self.assertEqual(r.status_code, 200)
        source = r.json()["error"].split()[1]
        self.assertTrue(source.endswith(".tau"))
        self.assertFalse(Path(source).exists())

    async def test_timeout_kills_the_child(self):
        with patch.dict(os.environ, {"FAKE_TAU_SLEEP": "30"}):
            with self.assertRaises(subprocess.TimeoutExpired):
                await server._run_tau([str(self.fake)], timeout=0.5)
        self.assertFalse(_alive(int(self.pid_file.read_text())))

    async def test_cancelled_request_kills_the_child(self):
        with patch.dict(os.environ, {"FAKE_TAU_SLEEP": "30"}):
            task = asyncio.create_task(server._run_tau([str(self.fake)], timeout=60))
            pid = await self._wait_for_child()
            task.cancel()
            with self.assertRaises(asyncio.CancelledError):
                await task
        self.assertFalse(_alive(pid))

    async def test_children_are_limited_to_the_configured_number(self):
        running = maximum = 0
        real_exec = asyncio.create_subprocess_exec

        async def counting_exec(*args, **kwargs):
            nonlocal running, maximum
            running += 1
            maximum = max(maximum, running)
            proc = await real_exec(*args, **kwargs)
            real_wait = proc.communicate

            async def communicate(*a, **k):
                nonlocal running
                try:
                    return await real_wait(*a, **k)
                finally:
                    running -= 1
            proc.communicate = communicate
            return proc

        with patch.dict(os.environ, {"FAKE_TAU_SLEEP": "0.1"}), \
                patch.object(server.asyncio, "create_subprocess_exec", counting_exec):
            results = await asyncio.gather(*(
                server._run_tau([str(self.fake)], timeout=10) for _ in range(4)))
        self.assertTrue(all(r.returncode == 0 for r in results))
        self.assertEqual(maximum, server._TAU_MAX_CHILDREN)


if __name__ == "__main__":
    unittest.main()
