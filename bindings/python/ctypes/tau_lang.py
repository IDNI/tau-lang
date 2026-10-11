"""Python bindings for tau-lang LTL(ABA) synthesis and execution.

Thin ctypes wrapper around libtau_ctypes.so / .dylib / .dll.
Provides realizability checking, Mealy machine synthesis, and
step-by-step execution for the Tau-Neuro Phase 1 pipeline.

Example — realizability check
-----------------------------

    from tau_lang import decide, Verdict

    v = decide("G ((o1[t]:qlt > {1/4}:qlt) && (o1[t]:qlt < {3/4}:qlt)).")
    if v == Verdict.REALIZABLE:
        print("realizable")

Example — Mealy machine synthesis and stepping
-----------------------------------------------

    from tau_lang import synthesize, mealy_step, mealy_free

    handle = synthesize("G (o1[t]:qlt = {1/2}:qlt).")
    outputs = mealy_step(handle, {})
    print(outputs)  # {"o1": "{1/2}:qlt", "state": 1}
    mealy_free(handle)
"""

# TODO (MEDIUM): decide fate of ctypes binding. Throw away ctypes binding in favor of nanobind?

from __future__ import annotations

import ctypes
import enum
import json
import os
import platform
from ctypes import c_char_p, c_double, c_int, c_int64, c_uint64
from typing import Optional


class Verdict(enum.IntEnum):
    """Return value of :func:`decide`."""

    REALIZABLE = 0
    UNREALIZABLE = 1
    PARSE_ERROR = 2
    INTERNAL_ERR = 3


# Error codes for synthesize (negative = error).
SYNTH_UNREALIZABLE = -1
SYNTH_PARSE_ERROR = -2
SYNTH_INTERNAL_ERR = -3


class SynthesisError(RuntimeError):
    """Raised when Mealy machine synthesis fails."""

    def __init__(self, code: int, detail: str = ""):
        self.code = code
        msg = {
            SYNTH_UNREALIZABLE: "formula is unrealizable",
            SYNTH_PARSE_ERROR: "parse error",
            SYNTH_INTERNAL_ERR: "internal error",
        }.get(code, f"unknown error ({code})")
        if detail:
            msg += f": {detail}"
        super().__init__(msg)


class StepError(RuntimeError):
    """Raised when a Mealy machine step fails."""

    pass


def _lib_name() -> str:
    system = platform.system()
    if system == "Darwin":
        return "libtau_ctypes.dylib"
    if system == "Windows":
        return "tau_ctypes.dll"
    return "libtau_ctypes.so"


def _candidate_paths() -> list[str]:
    env = os.environ.get("TAU_LTL_LIB")
    paths = []
    if env:
        paths.append(env)
    here = os.path.dirname(os.path.abspath(__file__))
    name = _lib_name()
    paths.extend(
        [
            os.path.join(here, name),
            os.path.join(here, "..", "..", "..", "build", name),
            os.path.join(here, "..", "..", "..", "build", "bindings", "python", "ctypes", name),
            os.path.join(here, "..", "..", "..", "build", "bindings", "python", name),
            os.path.join("/usr/local/lib", name),
            os.path.join("/usr/lib", name),
        ]
    )
    return paths


_LIB: Optional[ctypes.CDLL] = None


def _load() -> ctypes.CDLL:
    global _LIB
    if _LIB is not None:
        return _LIB
    for p in _candidate_paths():
        if os.path.exists(p):
            _LIB = ctypes.CDLL(p)
            break
    else:
        raise RuntimeError(
            "libtau_ctypes not found in any of: "
            + ", ".join(_candidate_paths())
            + "; set TAU_LTL_LIB to its full path."
        )
    # Realizability API
    _LIB.tau_lang_decide.argtypes = [c_char_p]
    _LIB.tau_lang_decide.restype = c_int
    _LIB.tau_lang_last_error.argtypes = []
    _LIB.tau_lang_last_error.restype = c_char_p
    # Mealy machine API
    _LIB.tau_lang_synthesize.argtypes = [c_char_p]
    _LIB.tau_lang_synthesize.restype = c_int64
    _LIB.tau_lang_mealy_step.argtypes = [c_int64, c_char_p]
    _LIB.tau_lang_mealy_step.restype = c_char_p
    _LIB.tau_lang_mealy_output_vars.argtypes = [c_int64]
    _LIB.tau_lang_mealy_output_vars.restype = c_char_p
    _LIB.tau_lang_mealy_input_vars.argtypes = [c_int64]
    _LIB.tau_lang_mealy_input_vars.restype = c_char_p
    _LIB.tau_lang_mealy_state.argtypes = [c_int64]
    _LIB.tau_lang_mealy_state.restype = c_int64
    _LIB.tau_lang_mealy_free.argtypes = [c_int64]
    _LIB.tau_lang_mealy_free.restype = None
    _LIB.tau_lang_reset.argtypes = []
    _LIB.tau_lang_reset.restype = c_int64
    # Runtime limits
    _LIB.tau_lang_set_limit.argtypes = [c_char_p, c_uint64]
    _LIB.tau_lang_set_limit.restype = c_int
    _LIB.tau_lang_get_limit.argtypes = [c_char_p, ctypes.POINTER(c_uint64)]
    _LIB.tau_lang_get_limit.restype = c_int
    _LIB.tau_lang_limit_names.argtypes = []
    _LIB.tau_lang_limit_names.restype = c_char_p
    _LIB.tau_lang_set_gc_growth_factor.argtypes = [c_double]
    _LIB.tau_lang_set_gc_growth_factor.restype = None
    _LIB.tau_lang_get_gc_growth_factor.argtypes = []
    _LIB.tau_lang_get_gc_growth_factor.restype = c_double
    _LIB.tau_lang_set_ltl_timeout_sec.argtypes = [c_int64]
    _LIB.tau_lang_set_ltl_timeout_sec.restype = None
    _LIB.tau_lang_get_ltl_timeout_sec.argtypes = []
    _LIB.tau_lang_get_ltl_timeout_sec.restype = c_int64
    _LIB.tau_lang_set_ltl_algorithm.argtypes = [c_char_p]
    _LIB.tau_lang_set_ltl_algorithm.restype = None
    _LIB.tau_lang_get_ltl_algorithm.argtypes = []
    _LIB.tau_lang_get_ltl_algorithm.restype = c_char_p
    return _LIB


# ---------------------------------------------------------------------------
# Realizability
# ---------------------------------------------------------------------------


def decide(formula: str) -> Verdict:
    """Decide LTL(ABA) realizability. The formula must end with '.'."""
    lib = _load()
    r = lib.tau_lang_decide(formula.encode("utf-8"))
    return Verdict(r)


def last_error() -> str:
    """Short message describing the most recent non-realizability result."""
    lib = _load()
    p = lib.tau_lang_last_error()
    return p.decode("utf-8") if p else ""


# ---------------------------------------------------------------------------
# Mealy machine synthesis and execution
# ---------------------------------------------------------------------------


def synthesize(formula: str) -> int:
    """Synthesize a Mealy machine from an LTL(ABA) formula.

    Returns an opaque handle (>0) for use with :func:`mealy_step`,
    :func:`mealy_output_vars`, :func:`mealy_state`, and
    :func:`mealy_free`.

    Raises :class:`SynthesisError` on failure.
    """
    lib = _load()
    handle = lib.tau_lang_synthesize(formula.encode("utf-8"))
    if handle <= 0:
        raise SynthesisError(handle, last_error())
    return handle


def mealy_step(handle: int, inputs: dict) -> dict:
    """Advance the Mealy machine one step.

    Parameters
    ----------
    handle : int
        Handle returned by :func:`synthesize`.
    inputs : dict
        Mapping of input variable names to values,
        e.g. ``{"i1": "{1/2}:qlt"}``.  Pass ``{}`` for specs
        with no input variables.

    Returns
    -------
    dict
        Output variable assignments and ``"state"`` key,
        e.g. ``{"o1": "{3/4}:qlt", "state": 1}``.

    Raises
    ------
    StepError
        If the step fails.
    """
    lib = _load()
    input_json = json.dumps(inputs).encode("utf-8")
    result = lib.tau_lang_mealy_step(handle, input_json)
    if not result:
        raise StepError(last_error())
    return json.loads(result.decode("utf-8"))


def mealy_output_vars(handle: int) -> list:
    """Query output variable names for the synthesized interpreter.

    This returns static metadata (same for all states), not
    state-dependent admissible values.  Use :func:`mealy_step` to test
    whether a particular input produces a valid output.

    Returns a list of output variable name strings, e.g. ``["o1", "o2"]``.
    """
    lib = _load()
    result = lib.tau_lang_mealy_output_vars(handle)
    if not result:
        raise StepError(last_error())
    return json.loads(result.decode("utf-8"))


def mealy_input_vars(handle: int) -> list:
    """Query input variable names needed for the next step.

    Returns a list of input variable name strings, e.g. ``["i1"]``.
    """
    lib = _load()
    result = lib.tau_lang_mealy_input_vars(handle)
    if not result:
        raise StepError(last_error())
    return json.loads(result.decode("utf-8"))


def mealy_state(handle: int) -> int:
    """Query the current time step index.

    Note: this is the step counter, not the internal automaton state node.
    """
    lib = _load()
    state = lib.tau_lang_mealy_state(handle)
    if state < 0:
        raise StepError(last_error())
    return state


def mealy_free(handle: int) -> None:
    """Release resources for a synthesized machine."""
    lib = _load()
    lib.tau_lang_mealy_free(handle)


def reset() -> int:
    """Return the engine to a fresh state.

    Releases every synthesized machine (their handles become invalid), drops
    the definitions, empties the caches and frees every tree node nothing
    holds. Returns the number of nodes freed.
    """
    lib = _load()
    return lib.tau_lang_reset()


# ---------------------------------------------------------------------------
# Runtime limits
# ---------------------------------------------------------------------------
#
# Every numeric limit of the C++ api is reachable by name, and as
# set_<name>(n) / get_<name>() like the nanobind module: set_max_fixpoint_steps,
# get_ltl_hoa_max_states, ... A limit reads back the value in force: the one
# set, else its TAU_* environment variable, else its default.


def limit_names() -> list:
    """Names of the numeric runtime limits, as set_<name> / get_<name>."""
    return json.loads(_load().tau_lang_limit_names().decode("utf-8"))


def set_limit(name: str, value: int) -> None:
    """Set the numeric runtime limit ``name``; 0 = unlimited for a cap."""
    if value < 0:
        raise ValueError(f"{name}: a limit is a non-negative number")
    if _load().tau_lang_set_limit(name.encode("utf-8"), value) != 0:
        raise KeyError(last_error())


def get_limit(name: str) -> int:
    """The value in force of the numeric runtime limit ``name``."""
    v = c_uint64()
    if _load().tau_lang_get_limit(name.encode("utf-8"), ctypes.byref(v)) != 0:
        raise KeyError(last_error())
    return v.value


def set_gc_growth_factor(factor: float) -> None:
    """Growth factor of the gc trigger; <= 0 disables gc."""
    _load().tau_lang_set_gc_growth_factor(factor)


def get_gc_growth_factor() -> float:
    return _load().tau_lang_get_gc_growth_factor()


def set_ltl_timeout_sec(seconds: int) -> None:
    """ltlsynt watchdog; 0 = off, negative = TAU_LTL_TIMEOUT_SEC or 60."""
    _load().tau_lang_set_ltl_timeout_sec(seconds)


def get_ltl_timeout_sec() -> int:
    return _load().tau_lang_get_ltl_timeout_sec()


def set_ltl_algorithm(algorithm: str) -> None:
    """A, B, D or auto; "" falls back to TAU_LTL_ALG."""
    _load().tau_lang_set_ltl_algorithm(algorithm.encode("utf-8"))


def get_ltl_algorithm() -> str:
    return _load().tau_lang_get_ltl_algorithm().decode("utf-8")


def __getattr__(attr: str):
    # set_<limit> / get_<limit> for every name limit_names() lists
    for prefix in ("set_", "get_"):
        if attr.startswith(prefix) and attr[4:] in limit_names():
            name = attr[4:]
            if prefix == "set_":
                return lambda n: set_limit(name, n)
            return lambda: get_limit(name)
    raise AttributeError(f"module {__name__!r} has no attribute {attr!r}")
