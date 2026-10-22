# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

# Test stub standing in for the LLM endpoint of the nlang oracle. It serves
# one mode on a free loopback port, points tau at it through the TAU_NLANG_*
# variables, runs the command after `--` and exits with its status.
#
#   python3 -I server.py <mode> -- <tau> <args>...
#
# Modes:
#   openai, anthropic  answer each oracle question from a fixed world (below)
#                      in the reply format of that provider
#   status:<code>      answer every request with HTTP <code>
#   badjson            answer 200 with a body that is not JSON
#   nochoices          answer 200 with an openai reply that has no choices
#   typeerror          answer 200 with {"type":"error"} and no message
#   refusal            answer 200 with an anthropic refusal
#   slow               hold every request 5 seconds and close it unanswered
#
# The world: a statement holding "square circle" is a contradiction, one
# holding "always" a tautology; "it pours" and "it rains hard" are
# equivalent; a stronger statement adds "heavily", except for one holding
# "dots", which gets only dots back; a statement holding " and " decomposes
# into its two sides, one holding " or " into a disjunction whose right side
# is negated, one holding "braces" gets braces around text that is not JSON,
# and any other is atomic. Each question is logged to stderr as
# `stub: <kind> <statement>`.

import http.server
import json
import os
import re
import subprocess
import sys
import threading
import time


def ask(prompt):
	m = re.search(r"statement '(.*)' a logical contradiction", prompt)
	if m:
		s = m.group(1)
		log("empty", s)
		if "square circle" in s:
			return "Answer: YES."
		if "unsure" in s:
			return "I cannot tell"
		return "no"
	m = re.search(r"statement '(.*)' a tautology", prompt)
	if m:
		s = m.group(1)
		log("universal", s)
		return "yes" if "always" in s else "NO, it is not"
	m = re.search(r"statements '(.*)' and '(.*)' logically equivalent",
		prompt)
	if m:
		a, b = m.group(1), m.group(2)
		log("equivalent", a + " | " + b)
		return "Yes." if {a, b} == {"it pours", "it rains hard"} else "No."
	m = re.search(r"strictly implies '(.*)' but", prompt)
	if m:
		s = m.group(1)
		log("stronger", s)
		return " ... " if "dots" in s else " " + s + " heavily.\n"
	m = re.search(r'Statement: "(.*)"', prompt)
	if m:
		s = m.group(1)
		log("decompose", s)
		atom = lambda t: {"op": "atom", "text": t}
		if "braces" in s:
			return "{op: atom}"
		if " and " in s:
			l, r = s.split(" and ", 1)
			f = {"op": "and", "left": atom(l), "right": atom(r)}
		elif " or " in s:
			l, r = s.split(" or ", 1)
			f = {"op": "or", "left": atom(l),
				"right": {"op": "not", "inner": atom(r)}}
		else:
			return "It is atomic."
		return "Here it is:\n```json\n" + json.dumps(f) + "\n```"
	log("other", prompt[:40])
	return "no"


def log(kind, text):
	sys.stderr.write("stub: " + kind + " " + text + "\n")
	sys.stderr.flush()


def reply_of(provider, text):
	if provider == "anthropic":
		return {"type": "message", "stop_reason": "end_turn",
			"content": [{"type": "text", "text": text}]}
	return {"choices": [{"message": {"role": "assistant",
		"content": text}}]}


class handler(http.server.BaseHTTPRequestHandler):
	mode = "openai"

	def log_message(self, *args):
		pass

	def send(self, code, body):
		data = body.encode()
		self.send_response(code)
		self.send_header("Content-Type", "application/json")
		self.send_header("Content-Length", str(len(data)))
		self.end_headers()
		self.wfile.write(data)

	def do_POST(self):
		n = int(self.headers.get("Content-Length", "0"))
		request = json.loads(self.rfile.read(n) or b"{}")
		mode = handler.mode
		if mode.startswith("status:"):
			return self.send(int(mode[7:]), '{"error":"stub"}')
		if mode == "badjson":
			return self.send(200, "this is not json")
		if mode == "nochoices":
			return self.send(200, '{"choices":[]}')
		if mode == "typeerror":
			return self.send(200, '{"type":"error"}')
		if mode == "refusal":
			return self.send(200, json.dumps({"type": "message",
				"stop_reason": "refusal", "content": []}))
		if mode == "slow":
			time.sleep(5)
			return
		prompt = request["messages"][0]["content"]
		provider = "anthropic" if self.path.endswith("/messages") \
			else "openai"
		self.send(200, json.dumps(reply_of(provider, ask(prompt))))


# A client that gave up on a slow answer is not an error of the stub.
class quiet_server(http.server.ThreadingHTTPServer):
	def handle_error(self, request, client_address):
		pass


def main():
	if len(sys.argv) < 4 or sys.argv[2] != "--":
		sys.exit("usage: server.py <mode> -- <command>...")
	handler.mode = sys.argv[1]
	server = quiet_server(("127.0.0.1", 0), handler)
	server.daemon_threads = True
	threading.Thread(target=server.serve_forever, daemon=True).start()
	env = dict(os.environ)
	for var in ("TAU_NLANG_MODEL", "TAU_NLANG_EFFORT", "OPENAI_API_KEY",
			"ANTHROPIC_API_KEY", "http_proxy", "HTTP_PROXY",
			"all_proxy", "ALL_PROXY"):
		env.pop(var, None)
	env["TAU_NLANG_ENDPOINT"] = \
		"http://127.0.0.1:%d/v1" % server.server_address[1]
	env["TAU_NLANG_API_KEY"] = "stub-key"
	env["TAU_NLANG_PROVIDER"] = \
		"anthropic" if handler.mode in ("anthropic", "refusal") \
		else "openai"
	env["NO_PROXY"] = env["no_proxy"] = "*"
	status = subprocess.call(sys.argv[3:], env=env)
	server.shutdown()
	sys.exit(status)


main()
