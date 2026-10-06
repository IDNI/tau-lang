"""The IDE's LLM provider layer: one normalized reply from three APIs.

Run from the repository root:

    python -m unittest -v ide.test_llm

No request leaves the process: a fake transport stands in for the chat
completions endpoints and a fake module for the anthropic SDK.
"""
from __future__ import annotations

import json
import sys
import types
import unittest
from unittest.mock import patch

import httpx

from ide import llm


def _chat_transport(seen: list, status: int = 200, reply: dict | None = None):
    """An httpx transport that records each request and answers @p reply."""
    body = reply if reply is not None else {
        "model": "served-model",
        "choices": [{"message": {"content": "the answer",
                                 "reasoning_content": "the reasoning"}}],
        "usage": {"prompt_tokens": 3, "completion_tokens": 4,
                  "total_tokens": 7},
    }

    def handler(request: httpx.Request) -> httpx.Response:
        seen.append(request)
        return httpx.Response(status, json=body)

    return httpx.MockTransport(handler)


class _Block:
    def __init__(self, **kw):
        self.__dict__.update(kw)


class _Usage:
    input_tokens = 10
    output_tokens = 5
    cache_read_input_tokens = 100
    cache_creation_input_tokens = 20


def _fake_anthropic(calls: list, *, stop_reason: str = "end_turn",
                    raises: str | None = None):
    """A module with the names ide.llm takes from the anthropic SDK."""
    mod = types.ModuleType("anthropic")

    class APIError(Exception):
        pass

    class APIStatusError(APIError):
        def __init__(self, message, status_code):
            super().__init__(message)
            self.message = message
            self.status_code = status_code

    class APIConnectionError(APIError):
        pass

    class _Messages:
        async def create(self, **kwargs):
            calls.append(kwargs)
            if raises == "status":
                raise APIStatusError("model: not found", 404)
            if raises == "connection":
                raise APIConnectionError("no route")
            return _Block(
                model="claude-served",
                stop_reason=stop_reason,
                content=[
                    _Block(type="thinking", thinking="the reasoning"),
                    _Block(type="text", text="the "),
                    _Block(type="text", text="answer"),
                ],
                usage=_Usage(),
            )

    class AsyncAnthropic:
        def __init__(self, api_key=None, **kw):
            calls.append({"api_key": api_key})
            self.messages = _Messages()

        async def __aenter__(self):
            return self

        async def __aexit__(self, *exc):
            return False

    mod.APIError = APIError
    mod.APIStatusError = APIStatusError
    mod.APIConnectionError = APIConnectionError
    mod.AsyncAnthropic = AsyncAnthropic
    return mod


class ChatCompletionsTests(unittest.IsolatedAsyncioTestCase):

    async def _call(self, provider, model, **kw):
        seen: list = []
        with patch.object(llm, "_chat_transport",
                          lambda: _chat_transport(seen, **kw)):
            result = await llm.call_llm(provider, "sk-key", "SYSTEM", "USER",
                                        model)
        return result, seen

    async def test_deepseek_request_and_normalized_reply(self):
        result, seen = await self._call("deepseek", "deepseek-chat")
        self.assertEqual(len(seen), 1)
        req = seen[0]
        self.assertEqual(str(req.url),
                         "https://api.deepseek.com/v1/chat/completions")
        self.assertEqual(req.headers["authorization"], "Bearer sk-key")
        payload = json.loads(req.content)
        self.assertEqual(payload["model"], "deepseek-chat")
        self.assertEqual(payload["messages"], [
            {"role": "system", "content": "SYSTEM"},
            {"role": "user", "content": "USER"},
        ])
        self.assertIn("temperature", payload)
        self.assertEqual(result, {
            "content": "the answer",
            "reasoning": "the reasoning",
            "model": "served-model",
            "usage": {"prompt_tokens": 3, "completion_tokens": 4,
                      "total_tokens": 7},
        })

    async def test_deepseek_reasoner_gets_no_temperature(self):
        _, seen = await self._call("deepseek", "deepseek-reasoner")
        self.assertNotIn("temperature", json.loads(seen[0].content))

    async def test_openai_uses_its_own_base_url(self):
        _, seen = await self._call("openai", "some-model")
        self.assertEqual(str(seen[0].url),
                         "https://api.openai.com/v1/chat/completions")

    async def test_http_error_names_the_provider(self):
        result, _ = await self._call("openai", "some-model", status=401,
                                     reply={"error": "bad key"})
        self.assertEqual(result["status"], 401)
        self.assertIn("OpenAI API error 401", result["error"])

    async def test_a_reply_without_choices_is_an_empty_answer(self):
        result, _ = await self._call("deepseek", "deepseek-chat", reply={})
        self.assertEqual(result["content"], "")
        self.assertEqual(result["usage"]["total_tokens"], 0)


class AnthropicTests(unittest.IsolatedAsyncioTestCase):

    async def _call(self, **fake):
        calls: list = []
        with patch.dict(sys.modules, {"anthropic": _fake_anthropic(calls,
                                                                   **fake)}):
            result = await llm.call_llm("anthropic", "sk-ant", "SYSTEM",
                                        "USER", "claude-opus-5-5")
        return result, calls

    async def test_request(self):
        _, calls = await self._call()
        self.assertEqual(calls[0], {"api_key": "sk-ant"})
        kw = calls[1]
        self.assertEqual(kw["model"], "claude-opus-5-5")
        self.assertEqual(kw["max_tokens"], 16000)
        self.assertEqual(kw["system"], [{
            "type": "text", "text": "SYSTEM",
            "cache_control": {"type": "ephemeral"}}])
        self.assertEqual(kw["thinking"],
                         {"type": "adaptive", "display": "summarized"})
        self.assertEqual(kw["messages"],
                         [{"role": "user", "content": "USER"}])
        # the current models reject it
        self.assertNotIn("temperature", kw)

    async def test_normalized_reply(self):
        result, _ = await self._call()
        self.assertEqual(result, {
            "content": "the answer",
            "reasoning": "the reasoning",
            "model": "claude-served",
            "usage": {"prompt_tokens": 130, "completion_tokens": 5,
                      "total_tokens": 135},
        })

    async def test_refusal_is_an_error(self):
        result, _ = await self._call(stop_reason="refusal")
        self.assertIn("error", result)
        self.assertNotIn("content", result)

    async def test_status_error_is_mapped(self):
        result, _ = await self._call(raises="status")
        self.assertEqual(result["status"], 404)
        self.assertIn("Anthropic API error 404", result["error"])
        self.assertIn("model: not found", result["error"])

    async def test_connection_error_is_mapped(self):
        result, _ = await self._call(raises="connection")
        self.assertIn("Anthropic API", result["error"])
        self.assertNotIn("content", result)

    async def test_missing_sdk_is_an_error_not_a_crash(self):
        with patch.dict(sys.modules, {"anthropic": None}):
            result = await llm.call_llm("anthropic", "k", "S", "U",
                                        "claude-opus-5-5")
        self.assertIn("pip install anthropic", result["error"])


class ProviderTests(unittest.IsolatedAsyncioTestCase):

    async def test_unknown_provider_is_an_error(self):
        result = await llm.call_llm("nobody", "k", "S", "U", "m")
        self.assertIn("nobody", result["error"])

    async def test_the_three_entry_points_pass_the_provider(self):
        seen: list = []

        async def fake(provider, api_key, system, user_msg, model, **kw):
            seen.append((provider, api_key, model))
            self.assertTrue(system)
            self.assertTrue(user_msg)
            return {"content": "```\nalways o1[t] = 1.\n```",
                    "reasoning": "", "model": model, "usage": {}}

        with patch.object(llm, "call_llm", fake):
            gen = await llm.nl_to_tau("k", "p", provider="anthropic",
                                      model="m1")
            exp = await llm.tau_to_nl("k", "c", provider="openai", model="m2")
            ast = await llm.tau_assist("k", "q", "e", provider="deepseek",
                                       model="m3")
        self.assertEqual(seen, [("anthropic", "k", "m1"),
                                ("openai", "k", "m2"),
                                ("deepseek", "k", "m3")])
        self.assertEqual(gen["code"], "always o1[t] = 1.")
        self.assertIn("always", exp["explanation"])
        self.assertEqual(ast["code_snippets"], ["always o1[t] = 1."])


class SettingsTests(unittest.TestCase):

    def test_request_wins_over_environment(self):
        env = {"TAU_LLM_PROVIDER": "openai", "TAU_LLM_MODEL": "env-model",
               "TAU_LLM_API_KEY": "sk-env"}
        s = llm.resolve_settings({"provider": "anthropic", "model": "m",
                                  "api_key": " sk-req "}, env)
        self.assertEqual(s, {"provider": "anthropic", "model": "m",
                             "api_key": "sk-req"})

    def test_environment_is_the_fallback(self):
        env = {"TAU_LLM_PROVIDER": "anthropic", "TAU_LLM_MODEL": "env-model",
               "TAU_LLM_API_KEY": "sk-env"}
        s = llm.resolve_settings({}, env)
        self.assertEqual(s, {"provider": "anthropic", "model": "env-model",
                             "api_key": "sk-env"})

    def test_the_shared_key_goes_to_the_environment_s_provider_only(self):
        # TAU_LLM_API_KEY is a key of the provider TAU_LLM_PROVIDER names;
        # no other provider, and no provider when none is named, gets it
        env = {"TAU_LLM_API_KEY": "sk-env"}
        for provider in (None, "deepseek", "openai", "anthropic"):
            body = {"provider": provider} if provider else {}
            self.assertEqual(llm.resolve_settings(body, env)["api_key"], "")
        env["TAU_LLM_PROVIDER"] = "openai"
        self.assertEqual(llm.resolve_settings({}, env)["api_key"], "sk-env")
        self.assertEqual(
            llm.resolve_settings({"provider": "openai"}, env)["api_key"],
            "sk-env")
        for provider in ("deepseek", "anthropic"):
            self.assertEqual(
                llm.resolve_settings({"provider": provider}, env)["api_key"],
                "")

    def test_defaults(self):
        self.assertEqual(llm.resolve_settings({}, {}), {
            "provider": "deepseek", "model": "deepseek-reasoner",
            "api_key": ""})
        self.assertEqual(
            llm.resolve_settings({"provider": "anthropic"}, {})["model"],
            "claude-opus-5-5")
        self.assertEqual(
            llm.resolve_settings({"provider": "openai"}, {})["model"], "")

    def test_the_provider_s_own_key_variable(self):
        env = {"ANTHROPIC_API_KEY": "sk-ant", "OPENAI_API_KEY": "sk-oai",
               "DEEPSEEK_API_KEY": "sk-ds"}
        for provider, key in (("anthropic", "sk-ant"), ("openai", "sk-oai"),
                              ("deepseek", "sk-ds")):
            self.assertEqual(llm.resolve_settings({"provider": provider},
                                                  env)["api_key"], key)

    def test_a_model_of_another_provider_is_not_carried_over(self):
        # the UI names the provider of the model it sends; an environment
        # model belongs to the environment's provider only
        env = {"TAU_LLM_PROVIDER": "anthropic",
               "TAU_LLM_MODEL": "claude-opus-5-5"}
        s = llm.resolve_settings({"provider": "deepseek"}, env)
        self.assertEqual(s["model"], "deepseek-reasoner")

    def test_provider_labels(self):
        self.assertEqual(llm.provider_label("deepseek"), "DeepSeek")
        self.assertEqual(llm.provider_label("openai"), "OpenAI")
        self.assertEqual(llm.provider_label("anthropic"), "Anthropic")
        self.assertEqual(llm.provider_label("nobody"), "nobody")


try:
    from ide import server
except ImportError:  # fastapi is not installed
    server = None

_LLM_ENV = ("TAU_LLM_PROVIDER", "TAU_LLM_MODEL", "TAU_LLM_API_KEY",
            "DEEPSEEK_API_KEY", "OPENAI_API_KEY", "ANTHROPIC_API_KEY")


@unittest.skipIf(server is None, "the server needs fastapi")
class ServerTests(unittest.IsolatedAsyncioTestCase):

    def setUp(self):
        import os
        blank = {k: "" for k in _LLM_ENV}
        patcher = patch.dict(os.environ, blank)
        patcher.start()
        self.addCleanup(patcher.stop)
        self.env = os.environ

    async def _request(self, method, path, body=None):
        transport = httpx.ASGITransport(app=server.app)
        async with httpx.AsyncClient(transport=transport,
                                     base_url="http://ide") as client:
            return await client.request(method, path, json=body)

    async def test_a_request_without_a_key_names_its_provider(self):
        for provider, label in (("anthropic", "Anthropic"),
                                ("openai", "OpenAI"), (None, "DeepSeek")):
            body = {"prompt": "p"}
            if provider:
                body["provider"] = provider
            resp = await self._request("POST", "/api/llm/generate", body)
            self.assertEqual(resp.status_code, 400)
            self.assertEqual(resp.json()["error"], f"{label} API key required")

    async def test_an_unknown_provider_is_refused(self):
        for path in ("/api/llm/generate", "/api/llm/explain",
                     "/api/llm/assist"):
            resp = await self._request("POST", path, {
                "provider": "nobody", "api_key": "k", "prompt": "p",
                "code": "c", "question": "q"})
            self.assertEqual(resp.status_code, 400)
            self.assertIn("nobody", resp.json()["error"])

    async def test_each_handler_passes_provider_model_and_key(self):
        seen: list = []

        async def fake(api_key, *args, model="", provider="", **kw):
            seen.append((provider, model, api_key))
            return {"ok": True}

        body = {"provider": "anthropic", "api_key": "sk-req", "prompt": "p",
                "code": "c", "question": "q"}
        with patch.object(server, "nl_to_tau", fake), \
                patch.object(server, "tau_to_nl", fake), \
                patch.object(server, "tau_assist", fake):
            for path in ("/api/llm/generate", "/api/llm/explain",
                         "/api/llm/assist"):
                resp = await self._request("POST", path, body)
                self.assertEqual(resp.status_code, 200)
        self.assertEqual(seen, [("anthropic", "claude-opus-5-5", "sk-req")] * 3)

    async def test_the_environment_supplies_what_a_request_leaves_out(self):
        seen: list = []

        async def fake(api_key, *args, model="", provider="", **kw):
            seen.append((provider, model, api_key))
            return {"ok": True}

        self.env["TAU_LLM_PROVIDER"] = "anthropic"
        self.env["ANTHROPIC_API_KEY"] = "sk-server"
        with patch.object(server, "tau_to_nl", fake):
            resp = await self._request("POST", "/api/llm/explain",
                                       {"code": "c"})
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(seen, [("anthropic", "claude-opus-5-5", "sk-server")])

    async def test_an_llm_error_is_a_502(self):
        async def fake(*args, **kw):
            return {"error": "Anthropic API error 404: nope", "status": 404}

        with patch.object(server, "tau_to_nl", fake):
            resp = await self._request("POST", "/api/llm/explain", {
                "provider": "anthropic", "api_key": "k", "code": "c"})
        self.assertEqual(resp.status_code, 502)
        self.assertIn("404", resp.json()["error"])

    async def test_config_says_whether_the_server_has_a_key_never_the_key(self):
        self.env["OPENAI_API_KEY"] = "sk-server-secret"
        resp = await self._request("GET", "/api/llm/config")
        self.assertEqual(resp.status_code, 200)
        self.assertNotIn("sk-server-secret", resp.text)
        data = resp.json()
        self.assertEqual(data["provider"], "deepseek")
        self.assertEqual(data["model"], "deepseek-reasoner")
        self.assertTrue(data["providers"]["openai"]["server_key"])
        self.assertFalse(data["providers"]["anthropic"]["server_key"])
        self.assertEqual(data["providers"]["anthropic"]["default_model"],
                         "claude-opus-5-5")


if __name__ == "__main__":
    unittest.main()
