"""
DeepSeek LLM integration for the Tau IDE.

Provides production-grade natural-language ↔ Tau translation with
comprehensive context: full grammar, language semantics, worked examples,
operator reference, common patterns, and error avoidance.
"""
from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

import httpx

REPO_ROOT = Path(__file__).resolve().parent.parent

# ---------------------------------------------------------------------------
# Context assembly — everything the LLM needs to write correct Tau code
# ---------------------------------------------------------------------------

def _read_file(path: Path, max_chars: int = 0) -> str:
    try:
        text = path.read_text()
        if max_chars and len(text) > max_chars:
            text = text[:max_chars] + "\n... [truncated]"
        return text
    except Exception:
        return ""


def build_system_prompt() -> str:
    """Assemble a comprehensive system prompt for Tau code generation."""
    grammar = _read_file(REPO_ROOT / "parser" / "tau.tgf")
    readme = _read_file(REPO_ROOT / "README.md", max_chars=40000)

    return f"""You are an expert Tau language programmer and formal methods specialist.
The Tau language is an expressive, decidable, and executable formal software
specification language based on Linear Temporal Logic over Atomless Boolean
Algebras (LTL/ABA).

═══════════════════════════════════════════════════════════════════════════════
COMPLETE TAU GRAMMAR (from parser/tau.tgf — this is the CANONICAL reference)
═══════════════════════════════════════════════════════════════════════════════

{grammar}

═══════════════════════════════════════════════════════════════════════════════
LANGUAGE SEMANTICS & RULES (CRITICAL — read every point)
═══════════════════════════════════════════════════════════════════════════════

1. SPECIFICATIONS (specs)
   - A spec has optional definitions followed by a main wff, ending with "."
   - Definitions: recurrence relations, input_def, output_def, each ending with "."
   - Input:  io_var_name := in stream.   (e.g., "i1 := in console.")
   - Output: io_var_name := out stream.  (e.g., "o1 := out console.")
   - Streams: "console" or file("filename")
   - The main formula is a wff that must hold at every time step

2. WELL-FORMED FORMULAS (wff) — evaluate to T or F
   - Boolean connectives: && (and), || (or), ! (not), -> (implies),
     <- (reverse implies), <-> (iff), ^^ (xor)
   - Quantifiers: "all x,y wff" (universal), "ex x,y wff" (existential)
   - Temporal (LTL): G/always/[] (globally), F/sometimes/<> (eventually),
     U (until), R (release), W (weak until), S (since), T (triggered)
   - CTL* (requires `fragment ctl_star.`): A (for-all-paths), E (exists-path),
     - (semantic negation = no winning strategy)
   - Constants: T (true), F (false)
   - Conditional: wff ? wff : wff

3. BOOLEAN FUNCTIONS (bf) — expressions over BA elements
   - Arithmetic: +, -, *, /, %
   - Bitwise: & (and), | (or), ^ (xor), ' (complement/negation),
     !& (nand), !| (nor), !^ (xnor)
   - Shift: <<, >>
   - Constants: 1 (top/true), 0 (bottom/false) — optionally typed: 1:bv, 0:sbf
   - BA constants: {{source}} (e.g., {{a}}, {{0}}, {{x + y}})
   - Variables: identifier (e.g., x, y, myvar)
   - I/O variables: name[offset] (e.g., o1[t], i1[t-1])
   - Captures: $name
   - Uninterpreted constants: <name:sort>
   - Function application: func_name(arg1, arg2, ...)

4. RELATIONAL OPERATORS (bf comparisons → wff)
   - =, !=, <, <=, >, >=, !<, !<=, !>, !>=
   - Interval: bf <= bf <= bf

5. TYPE SYSTEM
   - Types: bv (bitvector), sbf (symbolic boolean function),
     qlt (rational interval), qint (integer), nlang (natural language), tau
   - Type annotations: expr:type or expr:type[subtype]
   - I/O streams are typed: "o1:bv := out console."

6. STATEMENT TERMINATOR
   - Every statement/command MUST end with "." (period)
   - Multiple commands: "cmd1. cmd2. cmd3."

7. REPL COMMANDS
   - sat <expr>. — check satisfiability
   - valid <expr>. — check validity (tautology)
   - unsat <expr>. — check unsatisfiability
   - solve [--min|--max] [--type] <wff>. — find satisfying assignment
   - normalize <expr>. — normalize a formula
   - dnf/cnf/anf/nnf/pnf/mnf <expr>. — normal form conversions
   - onf <var> <expr>. — order normal form w.r.t. variable
   - qelim <wff>. — quantifier elimination
   - subst <expr> [<from> / <to>]. — substitution
   - inst <expr> [<var> / <val>]. — instantiation
   - whatis <expr>. — type inspection
   - run <spec>. — execute a specification
   - lgrs <wff>. — least general residual solution
   - fragment ltl | ctl_star. — switch grammar fragment

8. RECURRENCE RELATIONS
   - Define named relations: name(args) := expr.
   - Reference with offsets: name[t], name[t-1], name[0]
   - Fallback: name(args) fallback first|last|expr

9. COMMENTS
   - Line comments start with #

10. CONSTANT-TIME CONSTRAINTS
    - [var = num], [var != num], [var < num], [var <= num], etc.
    - Used for time-bounded reasoning

═══════════════════════════════════════════════════════════════════════════════
IDIOMATIC TAU EXAMPLES (study these carefully for style and patterns)
═══════════════════════════════════════════════════════════════════════════════

Example 1: Mirror input — output always equals input
  i1 := in console.
  o1 := out console.
  always o1[t] = i1[t].

Example 2: Safety — output is never zero
  i1 := in console.
  o1 := out console.
  always o1[t] != 0.

Example 3: Liveness — output eventually becomes 1
  i1 := in console.
  o1 := out console.
  sometimes o1[t] = 1.

Example 4: Recurrence relation — output follows a pattern
  i1 := in console.
  o1 := out console.
  o1[0] = 1 && o1[t] = o1[t-1].

Example 5: Multi-output with temporal constraint
  i1 := in console.
  o1 := out console.
  o2 := out console.
  G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 0).

Example 6: Satisfiability check (REPL command)
  sat ex x x = {{0}}.

Example 7: Boolean formula with quantifier elimination
  qelim ex x (x = {{0}} && x != {{1}}).

Example 8: LTL with delay/lookback
  i1 := in console.
  o1 := out console.
  G (o1[t]:tau = i1[t-1]:tau).

Example 9: CTL* fragment (requires: fragment ctl_star.)
  fragment ctl_star.
  A G (o1[t] = 1).

Example 10: Substitution
  subst (x && y) [x / {{1}}].

Example 11: Normal form conversion
  dnf (x || y) && z.

Example 12: Type-annotated specification
  i1:bv := in console.
  o1:bv := out console.
  always o1[t]:bv = i1[t]:bv.

Example 13: Solve for a variable
  solve ex x (x & y = {{0}}).

Example 14: Interval constraint
  ex x ({{0}} <= x <= {{1}}).

Example 15: Heterogeneous function declaration and use
  func dynamic f : bv, bv -> bv.
  always o1[t] = f(i1[t], i2[t]).

═══════════════════════════════════════════════════════════════════════════════
COMMON MISTAKES TO AVOID
═══════════════════════════════════════════════════════════════════════════════

- ALWAYS end statements with "." — forgetting the period is the #1 error
- BA constants use curly braces: {{0}}, {{1}}, {{a}} — not bare 0/1 for constants
  (bare 0/1 are bf_f/bf_t, which is the Boolean bottom/top element)
- I/O variables need brackets: o1[t], not o1 or o1(t)
- Use ":=" for definitions, not "="
- "=" is equality comparison (relational), ":=" is assignment/definition
- Single-letter temporal ops (G, F, U, R, W, S, T) are case-sensitive
- "T" alone means True (wff_t), "F" alone means False (wff_f) — don't confuse
  with the temporal operator "F" (which requires an argument: "F wff")
- Complement uses postfix ': x' means NOT x (bf_neg), not x'
- Use "!" for wff negation, "'" for bf complement
- Type annotations use ":" not "::" — e.g., "x:bv" not "x::bv"
- "always" and "G" are synonyms, "sometimes" and "F" are synonyms,
  "[]" and "G"/"always" are synonyms, "<>" and "F"/"sometimes" are synonyms

═══════════════════════════════════════════════════════════════════════════════
FULL README (for deeper semantic understanding)
═══════════════════════════════════════════════════════════════════════════════

{readme}
"""


_system_prompt_cache: str | None = None


def get_system_prompt() -> str:
    global _system_prompt_cache
    if _system_prompt_cache is None:
        _system_prompt_cache = build_system_prompt()
    return _system_prompt_cache


# ---------------------------------------------------------------------------
# DeepSeek API calls
# ---------------------------------------------------------------------------

DEEPSEEK_API_URL = "https://api.deepseek.com/v1/chat/completions"


async def call_deepseek(
    api_key: str,
    messages: list[dict[str, str]],
    model: str = "deepseek-reasoner",
    temperature: float = 0.3,
    max_tokens: int = 4096,
) -> dict[str, Any]:
    """Call the DeepSeek API with comprehensive error handling."""
    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json",
    }

    # Build payload - deepseek-reasoner doesn't support temperature
    payload: dict[str, Any] = {
        "model": model,
        "messages": messages,
        "max_tokens": max_tokens,
    }
    if model != "deepseek-reasoner":
        payload["temperature"] = temperature

    async with httpx.AsyncClient(timeout=120.0) as client:
        resp = await client.post(DEEPSEEK_API_URL, headers=headers, json=payload)
        if resp.status_code != 200:
            return {
                "error": f"DeepSeek API error {resp.status_code}: {resp.text}",
                "status": resp.status_code,
            }
        data = resp.json()
        choice = data.get("choices", [{}])[0]
        message = choice.get("message", {})
        return {
            "content": message.get("content", ""),
            "reasoning": message.get("reasoning_content", ""),
            "model": data.get("model", model),
            "usage": data.get("usage", {}),
        }


async def nl_to_tau(api_key: str, prompt: str, context: str = "", model: str = "deepseek-reasoner") -> dict[str, Any]:
    """Convert natural language description to Tau code."""
    system = get_system_prompt()
    user_msg = f"""Convert the following natural language description into valid Tau language code.

REQUIREMENTS:
- Output ONLY the Tau code, no explanations before or after
- Every statement must end with "."
- Use proper I/O definitions if the spec involves inputs/outputs
- Use the correct operators from the grammar
- Add comments (#) explaining each part of the code
- If the description is ambiguous, choose the most reasonable interpretation

"""
    if context:
        user_msg += f"CURRENT EDITOR CONTEXT (for reference):\n```\n{context}\n```\n\n"

    user_msg += f"DESCRIPTION:\n{prompt}"

    result = await call_deepseek(api_key, [
        {"role": "system", "content": system},
        {"role": "user", "content": user_msg},
    ], model=model)

    if "error" in result:
        return result

    # Extract just the code from the response
    content = result.get("content", "")
    code = _extract_code_block(content)
    return {
        "code": code,
        "raw_response": content,
        "reasoning": result.get("reasoning", ""),
        "model": result.get("model", ""),
        "usage": result.get("usage", {}),
    }


async def tau_to_nl(api_key: str, code: str, model: str = "deepseek-reasoner") -> dict[str, Any]:
    """Convert Tau code to a natural language explanation."""
    system = get_system_prompt()
    user_msg = f"""Explain the following Tau language code in clear, structured natural language.

REQUIREMENTS:
- Start with a one-sentence summary of what the code does
- Then provide a line-by-line or section-by-section breakdown
- Explain each temporal operator, quantifier, and type annotation
- Describe the I/O behavior if applicable
- Mention which grammar fragment is needed (ltl vs ctl_star) if relevant
- Use precise language — explain what each operator MEANS semantically
- If this is a REPL command, explain what it computes

TAU CODE:
```
{code}
```"""

    result = await call_deepseek(api_key, [
        {"role": "system", "content": system},
        {"role": "user", "content": user_msg},
    ], model=model)

    if "error" in result:
        return result

    return {
        "explanation": result.get("content", ""),
        "reasoning": result.get("reasoning", ""),
        "model": result.get("model", ""),
        "usage": result.get("usage", {}),
    }


async def tau_assist(
    api_key: str,
    question: str,
    editor_content: str,
    cursor_line: int = 0,
    model: str = "deepseek-reasoner",
) -> dict[str, Any]:
    """Context-aware code assistance — answers questions about the current program.

    Handles prompts like "how do I add a liveness property to this?" where
    the editor_content is the user's current partial Tau program.
    """
    system = get_system_prompt()
    user_msg = f"""The user is writing a Tau program in the IDE editor and has a question.
Answer their question with concrete Tau code that integrates with their
existing program. Provide code snippets they can insert.

REQUIREMENTS:
- Look at their existing code to understand the context (what I/O vars exist,
  what the spec does so far, what types are used)
- Provide Tau code snippets that FIT into their existing program
- If they need new I/O definitions, include them
- If they need to modify existing lines, show the before/after
- Add comments (#) explaining what each new part does
- Every statement must end with "."
- Be precise — use the exact syntax from the grammar
- If the question is about REPL commands, show the command syntax
- Format your response as:
  1. Brief explanation of what to do
  2. Code snippet(s) to add/modify
  3. Where to place them (e.g., "add after the I/O definitions")

CURRENT EDITOR CONTENT (line {cursor_line} is where the cursor is):
```
{editor_content}
```

USER QUESTION:
{question}"""

    result = await call_deepseek(api_key, [
        {"role": "system", "content": system},
        {"role": "user", "content": user_msg},
    ], model=model)

    if "error" in result:
        return result

    content = result.get("content", "")
    # Extract any code blocks from the response
    code_blocks = re.findall(r"```(?:\w*)\n(.*?)```", content, re.DOTALL)

    return {
        "answer": content,
        "code_snippets": [block.strip() for block in code_blocks],
        "reasoning": result.get("reasoning", ""),
        "model": result.get("model", ""),
        "usage": result.get("usage", {}),
    }


def _extract_code_block(text: str) -> str:
    """Extract code from markdown code blocks, or return raw text."""
    # Try to find fenced code block
    m = re.search(r"```(?:\w*)\n(.*?)```", text, re.DOTALL)
    if m:
        return m.group(1).strip()
    # Try to find indented code block
    lines = text.strip().split("\n")
    code_lines = []
    in_code = False
    for line in lines:
        stripped = line.strip()
        # Skip lines that are clearly prose
        if stripped and not stripped.startswith("#") and (
            stripped.endswith(".") and not stripped.endswith("..") or
            stripped.startswith("i") and ":=" in stripped or
            stripped.startswith("o") and ":=" in stripped or
            any(stripped.startswith(kw) for kw in [
                "always", "sometimes", "G ", "F ", "A ", "E ",
                "sat ", "valid ", "unsat ", "solve ", "normalize ",
                "dnf ", "cnf ", "run ", "fragment ", "func ",
                "all ", "ex ",
            ])
        ):
            in_code = True
            code_lines.append(stripped)
        elif in_code and stripped.startswith("#"):
            code_lines.append(stripped)
        elif in_code and not stripped:
            code_lines.append("")
        elif in_code:
            # Might be continuation
            if "." in stripped or ":=" in stripped:
                code_lines.append(stripped)
            else:
                break
    if code_lines:
        return "\n".join(code_lines).strip()
    return text.strip()
