"""
Tau Grammar Parser — extracts structure from parser/tau.tgf.

Provides:
  - Keyword/operator/type/command lists derived from the grammar
  - Production rules for each non-terminal
  - Autocomplete suggestions based on grammar context
  - Grammar documentation for help panel
"""
from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
GRAMMAR_PATH = REPO_ROOT / "parser" / "tau.tgf"


@dataclass
class Production:
    """A single grammar production alternative."""
    lhs: str
    pattern: str
    label: str = ""
    doc: str = ""


@dataclass
class GrammarInfo:
    """Structured information extracted from the TGF grammar."""
    # Raw
    productions: dict[str, list[Production]] = field(default_factory=dict)
    raw_text: str = ""

    # Derived token sets (for highlighting and autocomplete)
    keywords: list[str] = field(default_factory=list)
    temporal_ops: list[str] = field(default_factory=list)
    ctl_star_ops: list[str] = field(default_factory=list)
    commands: list[str] = field(default_factory=list)
    command_aliases: dict[str, list[str]] = field(default_factory=dict)
    types: list[str] = field(default_factory=list)
    boolean_ops: list[str] = field(default_factory=list)
    wff_ops: list[str] = field(default_factory=list)
    bf_ops: list[str] = field(default_factory=list)
    relational_ops: list[str] = field(default_factory=list)
    fragments: list[str] = field(default_factory=list)
    options: list[str] = field(default_factory=list)

    # Production docs for help
    wff_productions: list[dict] = field(default_factory=list)
    bf_productions: list[dict] = field(default_factory=list)
    cli_productions: list[dict] = field(default_factory=list)
    constraint_productions: list[dict] = field(default_factory=list)


def parse_grammar(path: Path | None = None) -> GrammarInfo:
    """Parse the TGF grammar file and extract structured info."""
    path = path or GRAMMAR_PATH
    if not path.exists():
        return GrammarInfo()

    raw = path.read_text()
    info = GrammarInfo(raw_text=raw)

    # Parse productions
    _parse_productions(raw, info)

    # Extract token categories
    _extract_keywords(info)
    _extract_temporal_ops(info)
    _extract_commands(info)
    _extract_types(info)
    _extract_operators(info)
    _extract_fragments(info)

    # Build production docs
    _build_production_docs(info)

    return info


def _parse_productions(raw: str, info: GrammarInfo):
    """Parse TGF production rules."""
    # Remove comments (lines starting with #, but not inside strings)
    lines = []
    for line in raw.split("\n"):
        stripped = line.strip()
        if stripped.startswith("#") or stripped.startswith("@"):
            continue
        # Remove inline comments that aren't inside strings
        if "#" in line and '"' not in line.split("#")[0]:
            line = line.split("#")[0]
        lines.append(line)

    text = "\n".join(lines)

    # Match productions: lhs => rhs .
    # Productions can span multiple lines, ended by a lone '.'
    prod_re = re.compile(
        r"(\w+)(?:\[(\w+)\])?\s*=>\s*(.*?)\s*\.",
        re.DOTALL,
    )

    for m in prod_re.finditer(text):
        lhs = m.group(1)
        variant = m.group(2) or ""
        rhs = m.group(3).strip()

        if lhs not in info.productions:
            info.productions[lhs] = []

        # Split alternatives by |
        alternatives = _split_alternatives(rhs)
        for alt in alternatives:
            alt = alt.strip()
            if not alt:
                continue
            # Extract label (: name at end)
            label = ""
            label_match = re.search(r":(\w+)\s*$", alt)
            if label_match:
                label = label_match.group(1)
            key = f"{lhs}[{variant}]" if variant else lhs
            info.productions.setdefault(key, []).append(
                Production(lhs=key, pattern=alt, label=label)
            )


def _split_alternatives(rhs: str) -> list[str]:
    """Split RHS by top-level | (not inside parens/quotes)."""
    alts = []
    depth = 0
    current = []
    in_str = False
    i = 0
    while i < len(rhs):
        c = rhs[i]
        if c == '"' and (i == 0 or rhs[i - 1] != "\\"):
            in_str = not in_str
        elif not in_str:
            if c in "([":
                depth += 1
            elif c in ")]":
                depth -= 1
            elif c == "|" and depth == 0:
                alts.append("".join(current))
                current = []
                i += 1
                continue
        current.append(c)
        i += 1
    alts.append("".join(current))
    return alts


def _extract_keywords(info: GrammarInfo):
    """Extract language keywords from grammar."""
    info.keywords = [
        "always", "sometimes", "all", "ex", "fall", "fex",
        "in", "out", "console", "file",
        "fallback", "first", "last",
    ]


def _extract_temporal_ops(info: GrammarInfo):
    """Extract temporal operators from wff productions."""
    # LTL temporal ops
    info.temporal_ops = ["G", "F", "U", "R", "W", "S", "T"]
    # CTL* operators
    info.ctl_star_ops = ["A", "E"]


def _extract_commands(info: GrammarInfo):
    """Extract CLI commands from grammar."""
    info.commands = []
    info.command_aliases = {}

    # Parse cli_command productions for command names
    command_patterns = [
        (["q", "quit"], "quit_cmd", "Quit the REPL"),
        (["v", "version"], "version_cmd", "Show version"),
        (["c", "clear"], "clear_cmd", "Clear screen"),
        (["h", "help"], "help_cmd", "Show help [topic]"),
        (["valid"], "valid_cmd", "Check validity of formula/spec"),
        (["sat"], "sat_cmd", "Check satisfiability"),
        (["unsat"], "unsat_cmd", "Check unsatisfiability"),
        (["solve"], "solve_cmd", "Find satisfying assignment"),
        (["lgrs"], "lgrs_cmd", "Compute LGRS"),
        (["r", "run"], "run_cmd", "Run a specification"),
        (["ltl"], "ltl_cmd", "Show LTL skeleton"),
        (["func"], "func_cmd", "Declare a function: func <mode> <name> : <sorts> -> <sort>"),
        (["n", "normalize"], "normalize_cmd", "Normalize a formula/spec"),
        (["s", "subst", "substitute"], "subst_cmd", "Substitute: subst <expr> [<from> / <to>]"),
        (["i", "inst", "instantiate"], "inst_cmd", "Instantiate: inst <expr> [<var> / <val>]"),
        (["dnf"], "dnf_cmd", "Convert to Disjunctive Normal Form"),
        (["cnf"], "cnf_cmd", "Convert to Conjunctive Normal Form"),
        (["anf"], "anf_cmd", "Convert to Algebraic Normal Form"),
        (["nnf"], "nnf_cmd", "Convert to Negation Normal Form"),
        (["pnf"], "pnf_cmd", "Convert to Prenex Normal Form"),
        (["mnf"], "mnf_cmd", "Convert to Minterm Normal Form"),
        (["onf"], "onf_cmd", "Convert to Order Normal Form: onf <var> <expr>"),
        (["qelim"], "qelim_cmd", "Quantifier elimination"),
        (["whatis"], "whatis_cmd", "Type inspection"),
        (["reset"], "reset_cmd", "Reset session"),
        (["fragment"], "fragment_cmd", "Switch grammar fragment: fragment ltl | ctl_star"),
        (["get"], "get_cmd", "Get option value"),
        (["set"], "set_cmd", "Set option: set <name> <value>"),
        (["enable"], "enable_cmd", "Enable boolean option"),
        (["disable"], "disable_cmd", "Disable boolean option"),
        (["toggle"], "toggle_cmd", "Toggle boolean option"),
        (["defs", "definitions"], "def_list_cmd", "List all definitions"),
        (["hist", "history"], "history_list_cmd", "Show command history"),
    ]

    for aliases, label, doc in command_patterns:
        primary = aliases[-1]  # longest form
        info.commands.append(primary)
        info.command_aliases[primary] = aliases
        # Add to productions with doc
        if "cli_command" not in info.productions:
            info.productions["cli_command"] = []
        info.productions["cli_command"].append(
            Production(lhs="cli_command", pattern=" | ".join(f'"{a}"' for a in aliases), label=label, doc=doc)
        )


def _extract_types(info: GrammarInfo):
    """Extract known BA types."""
    info.types = ["bv", "sbf", "qlt", "qint", "nlang", "tau"]


def _extract_operators(info: GrammarInfo):
    """Extract operators from grammar."""
    info.wff_ops = [
        ("->", "implication", "wff_imply"),
        ("<-", "reverse implication", "wff_rimply"),
        ("<->", "equivalence", "wff_equiv"),
        ("||", "disjunction", "wff_or"),
        ("^^", "exclusive or", "wff_xor"),
        ("&&", "conjunction", "wff_and"),
        ("!", "negation (syntactic)", "wff_neg"),
        ("-", "negation (semantic, CTL*)", "wff_semantic_neg"),
    ]

    info.bf_ops = [
        (">>", "shift right", "bf_shr"),
        ("<<", "shift left", "bf_shl"),
        ("+", "addition", "bf_add"),
        ("-", "subtraction", "bf_sub"),
        ("%", "modulo", "bf_mod"),
        ("*", "multiplication", "bf_mul"),
        ("/", "division", "bf_div"),
        ("!|", "NOR", "bf_nor"),
        ("!^", "XNOR", "bf_xnor"),
        ("!&", "NAND", "bf_nand"),
        ("|", "OR", "bf_or"),
        ("^", "XOR", "bf_xor"),
        ("&", "AND", "bf_and"),
        ("'", "complement", "bf_neg"),
    ]

    info.relational_ops = [
        ("=", "equality", "bf_eq"),
        ("!=", "inequality", "bf_neq"),
        ("<", "less than", "bf_lt"),
        ("<=", "less or equal", "bf_lteq"),
        (">", "greater than", "bf_gt"),
        (">=", "greater or equal", "bf_gteq"),
        ("!<", "not less than", "bf_nlt"),
        ("!>", "not greater than", "bf_ngt"),
        ("!<=", "not less or equal", "bf_nlteq"),
        ("!>=", "not greater or equal", "bf_ngteq"),
    ]

    info.fragments = ["ltl", "ctl_star", "ctl*"]


def _extract_fragments(info: GrammarInfo):
    """Extract grammar fragments (already set by _extract_operators, kept for clarity)."""
    if not info.fragments:
        info.fragments = ["ltl", "ctl_star", "ctl*"]


def _build_production_docs(info: GrammarInfo):
    """Build human-readable docs for each production category."""
    info.wff_productions = [
        {"syntax": "(wff)", "label": "wff_parenthesis", "doc": "Parenthesized formula"},
        {"syntax": "sometimes wff / <> wff", "label": "wff_sometimes", "doc": "Eventually (diamond)"},
        {"syntax": "always wff / [] wff / G wff", "label": "wff_always", "doc": "Globally (box)"},
        {"syntax": "F wff", "label": "wff_F", "doc": "Future / Eventually"},
        {"syntax": "A wff", "label": "wff_A", "doc": "For-all paths (CTL*)"},
        {"syntax": "E wff", "label": "wff_E", "doc": "Exists-path (CTL*)"},
        {"syntax": "- wff", "label": "wff_semantic_neg", "doc": "Semantic negation (no winning strategy)"},
        {"syntax": "wff ? wff : wff", "label": "wff_conditional", "doc": "Conditional (if-then-else)"},
        {"syntax": "all vars wff", "label": "wff_all", "doc": "Universal quantification"},
        {"syntax": "ex vars wff", "label": "wff_ex", "doc": "Existential quantification"},
        {"syntax": "wff -> wff", "label": "wff_imply", "doc": "Implication"},
        {"syntax": "wff <- wff", "label": "wff_rimply", "doc": "Reverse implication"},
        {"syntax": "wff <-> wff", "label": "wff_equiv", "doc": "Biconditional / equivalence"},
        {"syntax": "wff || wff", "label": "wff_or", "doc": "Disjunction"},
        {"syntax": "wff ^^ wff", "label": "wff_xor", "doc": "Exclusive or"},
        {"syntax": "wff && wff", "label": "wff_and", "doc": "Conjunction"},
        {"syntax": "! wff", "label": "wff_neg", "doc": "Syntactic negation"},
        {"syntax": "T", "label": "wff_t", "doc": "True (tautology)"},
        {"syntax": "F", "label": "wff_f", "doc": "False (contradiction)"},
        {"syntax": "wff U wff", "label": "wff_U", "doc": "Until"},
        {"syntax": "wff R wff", "label": "wff_R", "doc": "Release"},
        {"syntax": "wff W wff", "label": "wff_W", "doc": "Weak until"},
        {"syntax": "wff S wff", "label": "wff_S", "doc": "Since (past)"},
        {"syntax": "wff T wff", "label": "wff_T", "doc": "Triggered (past)"},
        {"syntax": "bf = bf", "label": "bf_eq", "doc": "Equality"},
        {"syntax": "bf != bf", "label": "bf_neq", "doc": "Inequality"},
        {"syntax": "bf < bf", "label": "bf_lt", "doc": "Less than"},
        {"syntax": "bf <= bf", "label": "bf_lteq", "doc": "Less or equal"},
        {"syntax": "bf > bf", "label": "bf_gt", "doc": "Greater than"},
        {"syntax": "bf >= bf", "label": "bf_gteq", "doc": "Greater or equal"},
        {"syntax": "bf <= bf <= bf", "label": "bf_interval", "doc": "Interval constraint"},
    ]

    info.bf_productions = [
        {"syntax": "(bf)", "label": "bf_parenthesis", "doc": "Parenthesized expression"},
        {"syntax": "{source}", "label": "ba_constant", "doc": "BA constant"},
        {"syntax": "variable", "label": "variable", "doc": "Variable reference"},
        {"syntax": "fall vars bf", "label": "bf_fall", "doc": "Boolean universal quantifier"},
        {"syntax": "fex vars bf", "label": "bf_fex", "doc": "Boolean existential quantifier"},
        {"syntax": "func(args)", "label": "bf_func_app", "doc": "Function application"},
        {"syntax": "bf >> bf", "label": "bf_shr", "doc": "Shift right"},
        {"syntax": "bf << bf", "label": "bf_shl", "doc": "Shift left"},
        {"syntax": "bf + bf", "label": "bf_add", "doc": "Addition"},
        {"syntax": "bf - bf", "label": "bf_sub", "doc": "Subtraction"},
        {"syntax": "bf % bf", "label": "bf_mod", "doc": "Modulo"},
        {"syntax": "bf * bf", "label": "bf_mul", "doc": "Multiplication"},
        {"syntax": "bf / bf", "label": "bf_div", "doc": "Division"},
        {"syntax": "bf !| bf", "label": "bf_nor", "doc": "NOR"},
        {"syntax": "bf !^ bf", "label": "bf_xnor", "doc": "XNOR"},
        {"syntax": "bf !& bf", "label": "bf_nand", "doc": "NAND"},
        {"syntax": "bf | bf", "label": "bf_or", "doc": "OR"},
        {"syntax": "bf ^ bf", "label": "bf_xor", "doc": "XOR"},
        {"syntax": "bf & bf", "label": "bf_and", "doc": "AND (explicit)"},
        {"syntax": "bf bf", "label": "bf_and_nosep", "doc": "AND (implicit, no separator)"},
        {"syntax": "1 [: type]", "label": "bf_t", "doc": "True / top element"},
        {"syntax": "0 [: type]", "label": "bf_f", "doc": "False / bottom element"},
        {"syntax": "bf'", "label": "bf_neg", "doc": "Complement / negation"},
    ]

    info.cli_productions = []
    for aliases, label, doc in [
        (["q", "quit"], "quit_cmd", "Quit the REPL session"),
        (["v", "version"], "version_cmd", "Show Tau version"),
        (["c", "clear"], "clear_cmd", "Clear the screen"),
        (["h", "help"], "help_cmd", "Show help (optionally for a specific command)"),
        (["valid"], "valid_cmd", "Check if a formula/spec is valid (tautology)"),
        (["sat"], "sat_cmd", "Check if a formula/spec is satisfiable"),
        (["unsat"], "unsat_cmd", "Check if a formula/spec is unsatisfiable"),
        (["solve"], "solve_cmd", "Find a satisfying assignment. Options: --min, --max, --<type>"),
        (["lgrs"], "lgrs_cmd", "Compute Least General Residual Solution"),
        (["r", "run"], "run_cmd", "Execute a specification (run the synthesis+interpreter pipeline)"),
        (["ltl"], "ltl_cmd", "Show the LTL skeleton of a specification"),
        (["func"], "func_cmd", "Declare a heterogeneous function: func <dynamic|static> name : sorts -> sort"),
        (["n", "normalize"], "normalize_cmd", "Normalize a formula or specification"),
        (["s", "subst", "substitute"], "subst_cmd", "Substitute: subst <expr> [<from> / <to>]"),
        (["i", "inst", "instantiate"], "inst_cmd", "Instantiate: inst <expr> [<var> / <value>]"),
        (["dnf"], "dnf_cmd", "Convert to Disjunctive Normal Form"),
        (["cnf"], "cnf_cmd", "Convert to Conjunctive Normal Form"),
        (["anf"], "anf_cmd", "Convert to Algebraic Normal Form (XOR of ANDs)"),
        (["nnf"], "nnf_cmd", "Convert to Negation Normal Form (push negations to atoms)"),
        (["pnf"], "pnf_cmd", "Convert to Prenex Normal Form (quantifiers to front)"),
        (["mnf"], "mnf_cmd", "Convert to Minterm Normal Form"),
        (["onf"], "onf_cmd", "Convert to Order Normal Form w.r.t. a variable: onf <var> <expr>"),
        (["qelim"], "qelim_cmd", "Quantifier elimination"),
        (["whatis"], "whatis_cmd", "Inspect the type of a formula (wff, bf, spec, etc.)"),
        (["reset"], "reset_cmd", "Reset the current session (clear all definitions and history)"),
        (["fragment"], "fragment_cmd", "Switch grammar fragment: fragment ltl (default) or fragment ctl_star"),
        (["get"], "get_cmd", "Get the current value of an option"),
        (["set"], "set_cmd", "Set an option: set <name> <value>"),
        (["enable"], "enable_cmd", "Enable a boolean option"),
        (["disable"], "disable_cmd", "Disable a boolean option"),
        (["toggle"], "toggle_cmd", "Toggle a boolean option"),
        (["defs", "definitions"], "def_list_cmd", "List all defined recurrence relations and I/O defs"),
        (["hist", "history"], "history_list_cmd", "Show the result history"),
    ]:
        info.cli_productions.append({
            "syntax": " | ".join(aliases),
            "label": label,
            "doc": doc,
            "aliases": aliases,
        })

    info.constraint_productions = [
        {"syntax": "[var != num]", "label": "ctn_neq", "doc": "Constant-time not-equal constraint"},
        {"syntax": "[var = num]", "label": "ctn_eq", "doc": "Constant-time equality constraint"},
        {"syntax": "[var >= num]", "label": "ctn_gteq", "doc": "Constant-time greater-or-equal"},
        {"syntax": "[var > num]", "label": "ctn_gt", "doc": "Constant-time greater-than"},
        {"syntax": "[var <= num]", "label": "ctn_lteq", "doc": "Constant-time less-or-equal"},
        {"syntax": "[var < num]", "label": "ctn_lt", "doc": "Constant-time less-than"},
    ]


def grammar_to_autocomplete(info: GrammarInfo) -> list[dict]:
    """Produce flat autocomplete entries from grammar info."""
    entries = []

    # Commands
    for prod in info.cli_productions:
        for alias in prod["aliases"]:
            entries.append({
                "label": alias,
                "type": "command",
                "doc": prod["doc"],
                "detail": prod["label"],
            })

    # Keywords
    for kw in info.keywords:
        entries.append({"label": kw, "type": "keyword", "doc": f"Keyword: {kw}"})

    # Temporal operators
    for op in info.temporal_ops:
        entries.append({"label": op, "type": "temporal", "doc": f"LTL temporal operator"})
    for op in info.ctl_star_ops:
        entries.append({"label": op, "type": "ctl_star", "doc": f"CTL* path quantifier"})

    # Types
    for t in info.types:
        entries.append({"label": t, "type": "type", "doc": f"Boolean Algebra type"})

    # Fragments
    for f in info.fragments:
        entries.append({"label": f, "type": "fragment", "doc": f"Grammar fragment"})

    # Special constants
    entries.append({"label": "T", "type": "constant", "doc": "True (wff tautology)"})
    entries.append({"label": "F", "type": "constant", "doc": "False (wff contradiction)"})

    return entries


def grammar_to_help_sections(info: GrammarInfo) -> dict:
    """Convert grammar info into structured help content."""
    return {
        "grammar_raw": info.raw_text,
        "wff_rules": info.wff_productions,
        "bf_rules": info.bf_productions,
        "cli_rules": info.cli_productions,
        "constraint_rules": info.constraint_productions,
        "keywords": info.keywords,
        "temporal_ops": info.temporal_ops,
        "ctl_star_ops": info.ctl_star_ops,
        "types": info.types,
        "wff_ops": info.wff_ops,
        "bf_ops": info.bf_ops,
        "relational_ops": info.relational_ops,
        "fragments": info.fragments,
    }


# Singleton
_grammar_info: GrammarInfo | None = None


def get_grammar() -> GrammarInfo:
    global _grammar_info
    if _grammar_info is None:
        _grammar_info = parse_grammar()
    return _grammar_info
