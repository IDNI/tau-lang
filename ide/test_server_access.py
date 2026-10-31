"""Only the IDE's own Host and Origin reach the server.

Run from the repository root:

    python -m unittest -v ide.test_server_access
"""
from __future__ import annotations

import unittest
from unittest.mock import patch

from starlette.testclient import TestClient
from starlette.websockets import WebSocketDisconnect

from ide import server


WS_URL = "ws://127.0.0.1:8080/ws/repl"


class RequestAllowedTests(unittest.TestCase):

    def test_decisions(self):
        for host, origin, allowed in [
            ("127.0.0.1:8080", None, True),
            ("localhost:8080", "http://localhost:8080", True),
            ("LOCALHOST:8080", "http://localhost:8080", True),
            ("[::1]:8080", "http://[::1]:8080", True),
            ("localhost:8080", "http://localhost:3000", False),
            ("localhost:8080", "http://localhost", False),
            ("localhost:8080", "https://evil.example", False),
            ("localhost:8080", "null", False),
            ("localhost:8080", "http://[bad", False),
            ("evil.example:8080", "http://evil.example:8080", False),
            ("evil.example:8080", None, False),
            ("127.0.0.1.evil.example", None, False),
            ("[bad", None, False),
            ("", None, False),
        ]:
            with self.subTest(host=host, origin=origin):
                self.assertEqual(server._request_allowed(host, origin), allowed)

    def test_named_host(self):
        with patch.object(server, "_allowed_hosts", {"ide.example.org"}):
            self.assertTrue(server._request_allowed(
                "ide.example.org:8080", "http://ide.example.org:8080"))
            self.assertFalse(server._request_allowed("localhost:8080", None))


class MiddlewareTests(unittest.TestCase):

    def setUp(self):
        self.client = TestClient(server.app, base_url="http://127.0.0.1:8080")

    def _foreign_page_is_refused(self, method, path):
        resp = self.client.request(
            method, path, headers={"Origin": "https://evil.example"}, json={})
        self.assertEqual(resp.status_code, 403, path)

    def test_every_post_endpoint_refuses_a_foreign_page(self):
        posts = [r.path for r in server.app.routes
                 if "POST" in getattr(r, "methods", ())]
        self.assertIn("/api/run", posts)
        for path in posts:
            self._foreign_page_is_refused("POST", path)

    def test_foreign_host_cannot_read(self):
        resp = self.client.get("/api/llm/config", headers={"Host": "evil.example"})
        self.assertEqual(resp.status_code, 403)

    def test_own_page_and_tools_get_through(self):
        own = {"Origin": "http://127.0.0.1:8080"}
        self.assertEqual(self.client.get("/api/status").status_code, 200)
        self.assertEqual(self.client.get("/static/ide.js").status_code, 200)
        self.assertEqual(
            self.client.post("/api/eval", headers=own, json={}).status_code, 400)
        self.assertEqual(self.client.post("/api/eval", json={}).status_code, 400)

    def test_websocket_refuses_a_foreign_page(self):
        with self.assertRaises(WebSocketDisconnect) as refused:
            with self.client.websocket_connect(
                    WS_URL, headers={"Origin": "https://evil.example"}):
                pass
        self.assertEqual(refused.exception.code, 1008)

    def test_websocket_accepts_its_own_page(self):
        with patch.object(server, "get_tau_binary", return_value=None):
            with self.client.websocket_connect(
                    WS_URL, headers={"Origin": "http://127.0.0.1:8080"}) as ws:
                self.assertEqual(ws.receive_json()["type"], "error")


if __name__ == "__main__":
    unittest.main()
