"""Tau-lang IDE — local webserver.

Usage:
    python -m ide.server [--port PORT] [--tau-binary PATH]

Serves a full-featured IDE for composing, navigating, debugging,
and running Tau programs.
"""
from __future__ import annotations

import argparse
import asyncio
import json
import os
import signal
import subprocess
import sys
import tempfile
import uuid
from pathlib import Path
from typing import Any

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles

from ide.grammar import get_grammar, grammar_to_autocomplete, grammar_to_help_sections
from ide.llm import nl_to_tau, tau_to_nl, tau_assist

IDE_DIR = Path(__file__).resolve().parent
REPO_ROOT = IDE_DIR.parent
STATIC_DIR = IDE_DIR / "static"

app = FastAPI(title="Tau-lang IDE")

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

_tau_binary: str | None = None


def find_tau_binary() -> str | None:
    """Locate the tau binary from build directory or PATH."""
    candidates = [
        REPO_ROOT / "build-Release" / "tau",
        REPO_ROOT / "build-Debug" / "tau",
        REPO_ROOT / "build-RelWithDebInfo" / "tau",
        REPO_ROOT / "build" / "tau",
        REPO_ROOT / "build" / "Release" / "tau",
        REPO_ROOT / "build" / "Debug" / "tau",
    ]
    for c in candidates:
        if c.is_file() and os.access(c, os.X_OK):
            return str(c)
    # try PATH
    import shutil
    found = shutil.which("tau")
    return found


def get_tau_binary() -> str | None:
    global _tau_binary
    if _tau_binary is None:
        _tau_binary = find_tau_binary()
    return _tau_binary


# ---------------------------------------------------------------------------
# REPL session manager
# ---------------------------------------------------------------------------

class ReplSession:
    """Manages a persistent tau REPL subprocess."""

    def __init__(self, session_id: str, binary: str):
        self.session_id = session_id
        self.binary = binary
        self.process: subprocess.Popen | None = None
        self._lock = asyncio.Lock()

    async def start(self) -> bool:
        if self.process and self.process.poll() is None:
            return True
        try:
            self.process = subprocess.Popen(
                [self.binary, "-c", "false", "-B", "false"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=0,
            )
            return True
        except Exception:
            return False

    async def send_command(self, cmd: str, timeout: float = 30.0) -> str:
        """Send a command to the REPL and collect output."""
        if not self.process or self.process.poll() is not None:
            if not await self.start():
                return "ERROR: Could not start tau REPL"

        async with self._lock:
            sentinel = f"__IDE_SENTINEL_{uuid.uuid4().hex[:8]}__"
            # Send the command followed by a sentinel echo trick
            # We send the actual command, then a help command to generate
            # known output we can detect
            full_cmd = f"{cmd}\n"
            try:
                self.process.stdin.write(full_cmd)
                self.process.stdin.flush()
            except (BrokenPipeError, OSError):
                return "ERROR: REPL process died"

            # Read output with timeout
            output_lines: list[str] = []
            loop = asyncio.get_event_loop()

            def _read_until_prompt():
                lines = []
                import select
                while True:
                    ready, _, _ = select.select(
                        [self.process.stdout], [], [], 0.3
                    )
                    if ready:
                        line = self.process.stdout.readline()
                        if not line:
                            break
                        lines.append(line)
                        # Detect prompt
                        if "tau>" in line:
                            break
                    else:
                        # Check if we've been waiting long enough
                        if lines:
                            break
                return "".join(lines)

            try:
                result = await asyncio.wait_for(
                    loop.run_in_executor(None, _read_until_prompt),
                    timeout=timeout,
                )
                return result
            except asyncio.TimeoutError:
                return "".join(output_lines) + "\n[Timeout after {:.0f}s]".format(timeout)

    async def stop(self):
        if self.process and self.process.poll() is None:
            try:
                self.process.stdin.write("q.\n")
                self.process.stdin.flush()
                self.process.wait(timeout=3)
            except Exception:
                self.process.kill()


_sessions: dict[str, ReplSession] = {}


async def get_or_create_session(session_id: str | None = None) -> ReplSession:
    binary = get_tau_binary()
    if not binary:
        raise RuntimeError("tau binary not found")
    if session_id is None:
        session_id = uuid.uuid4().hex[:12]
    if session_id not in _sessions:
        _sessions[session_id] = ReplSession(session_id, binary)
    return _sessions[session_id]


# ---------------------------------------------------------------------------
# REST API endpoints
# ---------------------------------------------------------------------------

@app.get("/", response_class=HTMLResponse)
async def index():
    index_path = STATIC_DIR / "index.html"
    return HTMLResponse(content=index_path.read_text())


@app.get("/api/status")
async def api_status():
    binary = get_tau_binary()
    return JSONResponse({
        "tau_binary": binary,
        "tau_available": binary is not None,
        "sessions": list(_sessions.keys()),
    })


@app.get("/api/grammar")
async def api_grammar():
    """Return the tau grammar file and parsed structure."""
    gi = get_grammar()
    return JSONResponse({
        "raw": gi.raw_text,
        "wff_rules": gi.wff_productions,
        "bf_rules": gi.bf_productions,
        "cli_rules": gi.cli_productions,
        "constraint_rules": gi.constraint_productions,
        "keywords": gi.keywords,
        "temporal_ops": gi.temporal_ops,
        "ctl_star_ops": gi.ctl_star_ops,
        "types": gi.types,
        "wff_ops": [(op, desc, label) for op, desc, label in gi.wff_ops],
        "bf_ops": [(op, desc, label) for op, desc, label in gi.bf_ops],
        "relational_ops": [(op, desc, label) for op, desc, label in gi.relational_ops],
        "fragments": gi.fragments,
    })


@app.get("/api/grammar/autocomplete")
async def api_grammar_autocomplete():
    """Return autocomplete entries derived from grammar."""
    gi = get_grammar()
    return JSONResponse(grammar_to_autocomplete(gi))


@app.get("/api/grammar/tokens")
async def api_grammar_tokens():
    """Return token categories for syntax highlighting (grammar-derived)."""
    gi = get_grammar()
    return JSONResponse({
        "keywords": gi.keywords,
        "temporal_ops": gi.temporal_ops,
        "ctl_star_ops": gi.ctl_star_ops,
        "commands": gi.commands,
        "types": gi.types,
        "fragments": gi.fragments,
        "wff_ops": [op for op, _, _ in gi.wff_ops],
        "bf_ops": [op for op, _, _ in gi.bf_ops],
        "relational_ops": [op for op, _, _ in gi.relational_ops],
    })


@app.get("/api/examples")
async def api_examples():
    """List available example programs."""
    examples: list[dict[str, str]] = []
    # Check tests for .tau files or spec examples
    test_dirs = [
        REPO_ROOT / "tests",
        REPO_ROOT / "tests" / "integration",
    ]
    for d in test_dirs:
        if d.is_dir():
            for f in sorted(d.iterdir()):
                if f.suffix in (".tau", ".cpp") and "test_" in f.name:
                    examples.append({
                        "name": f.name,
                        "path": str(f.relative_to(REPO_ROOT)),
                        "type": "test",
                    })
    return JSONResponse({"examples": examples})


@app.get("/api/help")
async def api_help():
    """Return comprehensive help content for the IDE."""
    content = get_help_content()
    # Attach grammar-derived reference data
    content["grammar_ref"] = grammar_to_help_sections(get_grammar())
    return JSONResponse(content)


@app.get("/api/help/{topic}")
async def api_help_topic(topic: str):
    """Return help for a specific topic."""
    content = get_help_content()
    topics = content.get("topics", {})
    if topic in topics:
        return JSONResponse({"topic": topic, "content": topics[topic]})
    return JSONResponse({"error": f"Unknown topic: {topic}"}, status_code=404)


@app.post("/api/eval")
async def api_eval(body: dict[str, Any]):
    """Evaluate a single REPL command."""
    cmd = body.get("command", "").strip()
    session_id = body.get("session_id")
    if not cmd:
        return JSONResponse({"error": "No command provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    # For one-shot evaluation, use -e flag
    try:
        result = subprocess.run(
            [binary, "-e", cmd, "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=30,
        )
        return JSONResponse({
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Command timed out (30s)"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


@app.post("/api/run")
async def api_run(body: dict[str, Any]):
    """Run a Tau specification."""
    source = body.get("source", "").strip()
    if not source:
        return JSONResponse({"error": "No source provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    # Write source to temp file and run
    with tempfile.NamedTemporaryFile(
        mode="w", suffix=".tau", delete=False, dir="/tmp"
    ) as f:
        f.write(source)
        tmp_path = f.name

    try:
        result = subprocess.run(
            [binary, tmp_path, "-q", "-c", "false", "-B", "false"],
            capture_output=True,
            text=True,
            timeout=30,
        )
        return JSONResponse({
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Execution timed out (30s)"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)
    finally:
        os.unlink(tmp_path)


@app.post("/api/parse")
async def api_parse(body: dict[str, Any]):
    """Parse a formula and return structure info."""
    formula = body.get("formula", "").strip()
    if not formula:
        return JSONResponse({"error": "No formula provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    # Use whatis command to get type info
    try:
        result = subprocess.run(
            [binary, "-e", f"whatis {formula}.", "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=10,
        )
        return JSONResponse({
            "formula": formula,
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Parse timed out"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


@app.post("/api/normalize")
async def api_normalize(body: dict[str, Any]):
    """Normalize a formula."""
    formula = body.get("formula", "").strip()
    form = body.get("form", "normalize")  # dnf, cnf, nnf, mnf, pnf, anf
    if not formula:
        return JSONResponse({"error": "No formula provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    cmd = f"{form} {formula}."
    try:
        result = subprocess.run(
            [binary, "-e", cmd, "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=30,
        )
        return JSONResponse({
            "formula": formula,
            "form": form,
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Normalization timed out"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


@app.post("/api/check")
async def api_check(body: dict[str, Any]):
    """Check satisfiability or validity of a formula."""
    formula = body.get("formula", "").strip()
    check_type = body.get("type", "sat")  # sat, unsat, valid
    if not formula:
        return JSONResponse({"error": "No formula provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    cmd = f"{check_type} {formula}."
    try:
        result = subprocess.run(
            [binary, "-e", cmd, "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=30,
        )
        return JSONResponse({
            "formula": formula,
            "check": check_type,
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Check timed out"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


@app.post("/api/solve")
async def api_solve(body: dict[str, Any]):
    """Solve a formula (find satisfying assignment)."""
    formula = body.get("formula", "").strip()
    if not formula:
        return JSONResponse({"error": "No formula provided"}, status_code=400)

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    cmd = f"solve {formula}."
    try:
        result = subprocess.run(
            [binary, "-e", cmd, "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=60,
        )
        return JSONResponse({
            "formula": formula,
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Solve timed out (60s)"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


@app.post("/api/substitute")
async def api_substitute(body: dict[str, Any]):
    """Substitute in a formula."""
    formula = body.get("formula", "").strip()
    target = body.get("target", "").strip()
    replacement = body.get("replacement", "").strip()
    if not formula or not target or not replacement:
        return JSONResponse(
            {"error": "Need formula, target, and replacement"}, status_code=400
        )

    binary = get_tau_binary()
    if not binary:
        return JSONResponse({"error": "tau binary not found"}, status_code=503)

    cmd = f"subst {formula} [{target} / {replacement}]."
    try:
        result = subprocess.run(
            [binary, "-e", cmd, "-c", "false", "-B", "false", "-q"],
            capture_output=True,
            text=True,
            timeout=30,
        )
        return JSONResponse({
            "formula": formula,
            "target": target,
            "replacement": replacement,
            "output": result.stdout,
            "error": result.stderr,
            "exit_code": result.returncode,
        })
    except subprocess.TimeoutExpired:
        return JSONResponse({"error": "Substitution timed out"}, status_code=408)
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)


# ---------------------------------------------------------------------------
# DeepSeek LLM integration
# ---------------------------------------------------------------------------

@app.post("/api/llm/generate")
async def api_llm_generate(body: dict[str, Any]):
    """Natural language → Tau code generation."""
    api_key = body.get("api_key", "").strip()
    prompt = body.get("prompt", "").strip()
    context = body.get("context", "")
    model = body.get("model", "deepseek-reasoner")
    if not api_key:
        return JSONResponse({"error": "DeepSeek API key required"}, status_code=400)
    if not prompt:
        return JSONResponse({"error": "Prompt required"}, status_code=400)
    result = await nl_to_tau(api_key, prompt, context=context, model=model)
    if "error" in result:
        return JSONResponse(result, status_code=502)
    return JSONResponse(result)


@app.post("/api/llm/explain")
async def api_llm_explain(body: dict[str, Any]):
    """Tau code → natural language explanation."""
    api_key = body.get("api_key", "").strip()
    code = body.get("code", "").strip()
    model = body.get("model", "deepseek-reasoner")
    if not api_key:
        return JSONResponse({"error": "DeepSeek API key required"}, status_code=400)
    if not code:
        return JSONResponse({"error": "Code required"}, status_code=400)
    result = await tau_to_nl(api_key, code, model=model)
    if "error" in result:
        return JSONResponse(result, status_code=502)
    return JSONResponse(result)


@app.post("/api/llm/assist")
async def api_llm_assist(body: dict[str, Any]):
    """Context-aware code assistance — answers questions about the current program."""
    api_key = body.get("api_key", "").strip()
    question = body.get("question", "").strip()
    editor_content = body.get("editor_content", "")
    cursor_line = body.get("cursor_line", 0)
    model = body.get("model", "deepseek-reasoner")
    if not api_key:
        return JSONResponse({"error": "DeepSeek API key required"}, status_code=400)
    if not question:
        return JSONResponse({"error": "Question required"}, status_code=400)
    result = await tau_assist(api_key, question, editor_content,
                              cursor_line=cursor_line, model=model)
    if "error" in result:
        return JSONResponse(result, status_code=502)
    return JSONResponse(result)


# ---------------------------------------------------------------------------
# WebSocket for interactive REPL
# ---------------------------------------------------------------------------

@app.websocket("/ws/repl")
async def ws_repl(ws: WebSocket):
    """Interactive REPL over WebSocket."""
    await ws.accept()
    session_id = uuid.uuid4().hex[:12]

    binary = get_tau_binary()
    if not binary:
        await ws.send_json({"type": "error", "data": "tau binary not found"})
        await ws.close()
        return

    # Start the REPL process
    process = await asyncio.create_subprocess_exec(
        binary, "-c", "false", "-B", "false",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT,
    )

    # Start output reader
    reader_task = asyncio.create_task(read_output_loop(process, ws))

    try:
        while True:
            msg = await ws.receive_json()
            if msg.get("type") == "command":
                cmd = msg.get("data", "")
                if process.stdin:
                    process.stdin.write((cmd + "\n").encode())
                    await process.stdin.drain()
            elif msg.get("type") == "interrupt":
                process.send_signal(signal.SIGINT)
            elif msg.get("type") == "close":
                break
    except WebSocketDisconnect:
        pass
    finally:
        reader_task.cancel()
        if process.returncode is None:
            process.terminate()
            try:
                await asyncio.wait_for(process.wait(), timeout=3)
            except asyncio.TimeoutError:
                process.kill()


async def read_output_loop(process, ws: WebSocket):
    """Continuously read process output and send to websocket."""
    try:
        while True:
            try:
                data = await asyncio.wait_for(
                    process.stdout.read(4096), timeout=0.3
                )
                if not data:
                    await ws.send_json({"type": "exit", "data": ""})
                    break
                text = data.decode("utf-8", errors="replace")
                await ws.send_json({"type": "output", "data": text})
            except asyncio.TimeoutError:
                continue
    except (WebSocketDisconnect, Exception):
        pass


# ---------------------------------------------------------------------------
# Help system
# ---------------------------------------------------------------------------

def get_help_content() -> dict[str, Any]:
    """Build comprehensive help content from grammar and docs."""
    return {
        "overview": {
            "title": "Tau Language IDE",
            "description": (
                "Tau is a reactive synthesis engine and interpreter for "
                "Linear Temporal Logic (LTL) specifications over Atomless "
                "Boolean Algebras (ABA). It converts temporal logic specs "
                "into executable Mealy machine strategies."
            ),
        },
        "topics": {
            "getting-started": {
                "title": "Getting Started",
                "content": HELP_GETTING_STARTED,
            },
            "grammar": {
                "title": "Grammar Reference",
                "content": HELP_GRAMMAR,
            },
            "operators": {
                "title": "Operators",
                "content": HELP_OPERATORS,
            },
            "commands": {
                "title": "REPL Commands",
                "content": HELP_COMMANDS,
            },
            "types": {
                "title": "Type System",
                "content": HELP_TYPES,
            },
            "examples": {
                "title": "Examples",
                "content": HELP_EXAMPLES,
            },
            "fragments": {
                "title": "Logic Fragments",
                "content": HELP_FRAGMENTS,
            },
            "normal-forms": {
                "title": "Normal Forms",
                "content": HELP_NORMAL_FORMS,
            },
            "synthesis": {
                "title": "Synthesis & Realizability",
                "content": HELP_SYNTHESIS,
            },
            "keyboard": {
                "title": "Keyboard Shortcuts",
                "content": HELP_KEYBOARD,
            },
        },
    }


HELP_GETTING_STARTED = """
## Getting Started with Tau

### Quick Start
1. Type a formula in the editor and press **Ctrl+Enter** to evaluate
2. Use the REPL panel for interactive exploration
3. Click **Run** to execute a specification file

### Your First Formula
```
# Check if a formula is satisfiable
sat ex x x = {0}.
```

### Your First Specification
```
# Define inputs and outputs
i1 := in console.
o1 := out console.

# Always: output equals input
always o1[t] = i1[t].
```

### Key Concepts
- **wff** (well-formed formula): Boolean/temporal formulas that evaluate to T/F
- **bf** (Boolean function): Expressions over Boolean algebra elements
- **spec**: A complete Tau specification with I/O definitions and temporal constraints
"""

HELP_GRAMMAR = """
## Tau Grammar Reference

### Specifications
```
spec := [definitions] wff.
definitions := (rec_relation | input_def | output_def '.')+
```

### I/O Definitions
```
input_def  := io_var_name [: type] := in stream.
output_def := io_var_name [: type] := out stream.
stream     := file("filename") | console
```

### Well-Formed Formulas (wff)
```
wff := (wff)              # parenthesized
     | sometimes wff       # <> / sometimes
     | always wff          # [] / always / G
     | F wff               # eventually (LTL F)
     | A wff               # forall-path (CTL*)
     | E wff               # exists-path (CTL*)
     | - wff               # semantic negation
     | wff ? wff : wff     # conditional
     | all vars wff        # universal quantification
     | ex vars wff         # existential quantification
     | wff -> wff          # implication
     | wff <- wff          # reverse implication
     | wff <-> wff         # equivalence
     | wff || wff          # disjunction
     | wff ^^ wff          # exclusive or
     | wff && wff          # conjunction
     | ! wff               # negation
     | T                   # true
     | F                   # false
     | bf = bf             # equality
     | bf != bf            # inequality
     | bf < bf             # less than
     | bf <= bf            # less or equal
     | bf > bf             # greater than
     | bf >= bf            # greater or equal
     | wff U wff           # until
     | wff R wff           # release
     | wff W wff           # weak until
     | wff S wff           # since
     | wff T wff           # triggered (past)
```

### Boolean Functions (bf)
```
bf := (bf)                 # parenthesized
    | {source}             # BA constant
    | variable             # variable reference
    | fall vars bf         # boolean forall
    | fex vars bf          # boolean exists
    | ref(args)            # function reference
    | func(args)           # function application
    | bf >> bf             # shift right
    | bf << bf             # shift left
    | bf + bf              # addition
    | bf - bf              # subtraction
    | bf % bf              # modulo
    | bf * bf              # multiplication
    | bf / bf              # division
    | bf !| bf             # NOR
    | bf !^ bf             # XNOR
    | bf !& bf             # NAND
    | bf | bf              # OR
    | bf ^ bf              # XOR
    | bf & bf              # AND (explicit)
    | bf bf                # AND (implicit, no sep)
    | 1 [: type]           # true / top
    | 0 [: type]           # false / bottom
    | bf'                  # complement / negation
```

### Variables
```
variable := var_name [: type]
io_var   := io_var_name[offset]
var_name := (alpha \\ {T,F}) (alnum | '_')*   # charvar mode
```
"""

HELP_OPERATORS = """
## Operators Reference

### Temporal Operators (LTL)
| Operator | Syntax | Meaning |
|----------|--------|---------|
| Always | `always phi` / `[] phi` / `G phi` | phi holds at every step |
| Sometimes | `sometimes phi` / `<> phi` | phi holds at some step |
| Eventually | `F phi` | phi will hold eventually |
| Until | `phi U psi` | phi holds until psi becomes true |
| Release | `phi R psi` | psi holds until (and including when) phi becomes true |
| Weak Until | `phi W psi` | like Until but phi may hold forever |
| Since | `phi S psi` | (past) phi has held since psi was true |
| Triggered | `phi T psi` | (past) triggered operator |

### CTL* Operators (requires `fragment ctl_star`)
| Operator | Syntax | Meaning |
|----------|--------|---------|
| For-all paths | `A phi` | phi holds on all computation paths |
| Exists path | `E phi` | phi holds on some computation path |
| Semantic neg | `- phi` | no winning strategy satisfying phi |

### Boolean Operators (wff level)
| Operator | Syntax | Meaning |
|----------|--------|---------|
| And | `phi && psi` | conjunction |
| Or | `phi \\|\\| psi` | disjunction |
| Not | `! phi` | negation |
| Implies | `phi -> psi` | implication |
| Equiv | `phi <-> psi` | biconditional |
| Xor | `phi ^^ psi` | exclusive or |

### Boolean Function Operators (bf level)
| Operator | Syntax | Meaning |
|----------|--------|---------|
| And | `a & b` or `a b` | conjunction |
| Or | `a \\| b` | disjunction |
| Xor | `a ^ b` | exclusive or |
| Complement | `a'` | negation |
| NAND | `a !& b` | not-and |
| NOR | `a !\\| b` | not-or |
| XNOR | `a !^ b` | not-xor |

### Arithmetic (bf level)
| Operator | Syntax |
|----------|--------|
| Add | `a + b` |
| Sub | `a - b` |
| Mul | `a * b` |
| Div | `a / b` |
| Mod | `a % b` |
| Shift left | `a << b` |
| Shift right | `a >> b` |

### Relational (produce wff)
| Operator | Syntax |
|----------|--------|
| Equal | `a = b` |
| Not equal | `a != b` |
| Less than | `a < b` |
| Less or eq | `a <= b` |
| Greater | `a > b` |
| Greater or eq | `a >= b` |
| Interval | `a <= b <= c` |
"""

HELP_COMMANDS = """
## REPL Commands

### Evaluation & Checking
| Command | Description |
|---------|-------------|
| `sat phi.` | Check if formula phi is satisfiable |
| `unsat phi.` | Check if formula phi is unsatisfiable |
| `valid phi.` | Check if formula phi is valid (tautology) |
| `solve phi.` | Find a satisfying assignment for phi |
| `lgrs phi.` | Compute the LGRS (Least General Residual Solution) |

### Normalization
| Command | Description |
|---------|-------------|
| `normalize phi.` / `n phi.` | Normalize a specification |
| `dnf phi.` | Convert to Disjunctive Normal Form |
| `cnf phi.` | Convert to Conjunctive Normal Form |
| `nnf phi.` | Convert to Negation Normal Form |
| `pnf phi.` | Convert to Prenex Normal Form |
| `mnf phi.` | Convert to Minterm Normal Form |
| `anf phi.` | Convert to Algebraic Normal Form |
| `onf var phi.` | Convert to Order Normal Form w.r.t. variable |
| `qelim phi.` | Quantifier elimination |

### Execution
| Command | Description |
|---------|-------------|
| `run phi.` / `r phi.` | Run a specification (execute) |
| `ltl phi.` | Show LTL skeleton |

### Manipulation
| Command | Description |
|---------|-------------|
| `subst phi [a / b].` / `s phi [a/b].` | Substitute b for a in phi |
| `inst phi [a / b].` / `i phi [a/b].` | Instantiate variable a with b |
| `whatis phi.` | Type inspection — show what type phi is |

### Definitions
| Command | Description |
|---------|-------------|
| `f(x) := body.` | Define a recurrence relation |
| `name := in console.` | Define an input stream |
| `name := out console.` | Define an output stream |
| `defs.` | List all definitions |

### Session
| Command | Description |
|---------|-------------|
| `help` / `h` | Show help |
| `help command` | Help for a specific command |
| `version` / `v` | Show version |
| `quit` / `q` | Quit the REPL |
| `history` / `hist` | Show command history |
| `%N` | Reference history entry N |
| `%-N` | Reference Nth-from-last entry |
| `reset.` | Reset the session |
| `fragment ltl.` | Switch to LTL fragment (default) |
| `fragment ctl_star.` | Switch to CTL* fragment |

### Options
| Command | Description |
|---------|-------------|
| `get` | Show all options |
| `get option` | Show specific option value |
| `set option value` | Set option value |
| `enable option` | Enable a boolean option |
| `disable option` | Disable a boolean option |
| `toggle option` | Toggle a boolean option |
"""

HELP_TYPES = """
## Type System

Tau operates over several Boolean Algebra types:

### Built-in Types
| Type | Description |
|------|-------------|
| `bv` | Bitvectors — finite Boolean algebra |
| `sbf` | Symbolic Boolean Functions |
| `qlt` | Dense linear orders over rationals (Q, <) |
| `qint` | Rational intervals |
| `nlang` | Natural language Boolean Algebra (LLM oracle) |
| `:tau` | Self-referential Tau type |

### Type Annotations
Variables and constants can be annotated with types:
```
x : bv          # x is a bitvector variable
{01} : bv       # bitvector constant 01
1 : qlt         # top element in qlt
0 : bv          # bottom element in bv
```

### I/O Stream Types
```
i1 : bv  := in console.    # input stream of bitvectors
o1 : qlt := out console.   # output stream of rationals
```
"""

HELP_EXAMPLES = """
## Examples

### Basic Boolean Formula
```
# Check satisfiability of a boolean formula
sat ex x (x = {01}).
```

### Simple Specification
```
# Echo spec: output always equals input
i1 := in console.
o1 := out console.
always o1[t] = i1[t].
```

### Safety Property
```
# Output is never all-zeros
i1 := in console.
o1 := out console.
always o1[t] != 0.
```

### Liveness Property
```
# Every request is eventually followed by a response
i1 := in console.
o1 := out console.
always (i1[t] = 1 -> sometimes o1[t] = 1).
```

### Recurrence Relations
```
# Define a function
f(x) := x | x'.
# Use it
sat f({01}) = 1.
```

### Normal Form Conversions
```
# Convert to DNF
dnf ex x (x = {01} && x != 0).

# Convert to NNF
nnf !(a && b -> c).
```

### CTL* (requires fragment switch)
```
fragment ctl_star.
# For all paths, eventually safe
A F (o1[t] = 1).
# There exists a path where we reach goal
E F (goal[t] = 1).
```

### Quantifier Elimination
```
qelim ex x (x & y = x).
```

### Substitution
```
# Substitute variable
subst (a && b) [a / (c || d)].
```
"""

HELP_FRAGMENTS = """
## Logic Fragments

Tau supports multiple logic fragments that determine which operators are available.

### LTL Fragment (default)
The default fragment. Supports standard LTL temporal operators:
- `always` / `[]` / `G` — globally
- `sometimes` / `<>` — eventually
- `F` — future/eventually
- `U` — until
- `R` — release
- `W` — weak until
- `S` — since (past)
- `T` — triggered (past)

Switch to LTL:
```
fragment ltl.
```

### CTL* Fragment
Extends LTL with branching-time operators:
- `A phi` — for all computation paths, phi holds
- `E phi` — there exists a computation path where phi holds
- `- phi` — semantic negation (no winning strategy satisfying phi)

Switch to CTL*:
```
fragment ctl_star.
```

### Semantic Negation (`-`)
Different from syntactic negation (`!`):
- `! phi` — syntactic complement (flip T/F)
- `- phi` — strategy-level: "there is no winning strategy satisfying phi"

The semantic negation is reduced to LTL via the Bloem/Schewe/Khalimov
witness-output encoding.
"""

HELP_NORMAL_FORMS = """
## Normal Forms

### DNF (Disjunctive Normal Form)
A disjunction of conjunctions: `(a && b) || (c && d)`
```
dnf formula.
```

### CNF (Conjunctive Normal Form)
A conjunction of disjunctions: `(a || b) && (c || d)`
```
cnf formula.
```

### NNF (Negation Normal Form)
Negations pushed to atoms only.
```
nnf formula.
```

### PNF (Prenex Normal Form)
All quantifiers moved to the front.
```
pnf formula.
```

### MNF (Minterm Normal Form)
Expansion over all minterms.
```
mnf formula.
```

### ANF (Algebraic Normal Form)
XOR of AND terms (Zhegalkin polynomial).
```
anf formula.
```

### ONF (Order Normal Form)
With respect to a specific variable:
```
onf x formula.
```
"""

HELP_SYNTHESIS = """
## Synthesis & Realizability

### Realizability
A specification is **realizable** if there exists a system strategy
(Mealy machine) that satisfies it against all possible environment inputs.

### Running a Specification
```
run spec.
```
This will:
1. Parse the specification
2. Extract the LTL skeleton
3. Call the synthesis backend (ltlsynt/Spot)
4. If realizable, execute the resulting strategy

### LTL Skeleton
```
ltl spec.
```
Shows the propositional LTL skeleton extracted from the specification,
with data atoms replaced by Boolean propositions.

### Solving
```
solve formula.
```
Finds a satisfying assignment for the formula's variables.

### The LGRS
```
lgrs formula.
```
Computes the Least General Residual Solution — the most general
substitution that makes the formula true.
"""

HELP_KEYBOARD = """
## Keyboard Shortcuts

### Editor
| Shortcut | Action |
|----------|--------|
| `Ctrl+Enter` | Evaluate current line/selection |
| `Shift+Enter` | Run entire program |
| `Ctrl+S` | Save file |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
| `Ctrl+/` | Toggle comment |
| `Ctrl+D` | Duplicate line |
| `Ctrl+Space` | Trigger autocomplete |
| `Tab` | Indent |
| `Shift+Tab` | Unindent |
| `Ctrl+F` | Find |
| `Ctrl+H` | Find and replace |

### REPL
| Shortcut | Action |
|----------|--------|
| `Enter` | Submit command |
| `Up/Down` | Navigate command history |
| `Ctrl+C` | Interrupt current command |
| `Ctrl+L` | Clear REPL output |

### Navigation
| Shortcut | Action |
|----------|--------|
| `Ctrl+P` | Quick file open |
| `Ctrl+G` | Go to line |
| `Ctrl+Shift+P` | Command palette |
| `F1` | Open help panel |
| `Escape` | Close panels/dialogs |
"""


# ---------------------------------------------------------------------------
# Static files
# ---------------------------------------------------------------------------

app.mount("/static", StaticFiles(directory=str(STATIC_DIR)), name="static")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="Tau-lang IDE server")
    parser.add_argument("--port", type=int, default=8080)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--tau-binary", default=None, help="Path to tau binary")
    args = parser.parse_args()

    global _tau_binary
    if args.tau_binary:
        _tau_binary = args.tau_binary

    import uvicorn
    uvicorn.run(app, host=args.host, port=args.port)


if __name__ == "__main__":
    main()
