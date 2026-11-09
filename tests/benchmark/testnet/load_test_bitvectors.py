#!/usr/bin/env python3
"""
Standalone script for load testing Tau core by dynamically sending rule proposals.

This single-file utility embeds the entire Tau specification and can spawn Tau either
via a local binary (preferred) or via native Python bindings. It iteratively submits ever-more-
complex bitvector-based rules and measures the Tau core's latency.
"""

import subprocess
import threading
import time
import os
import sys
import select
import queue
import re
import tempfile
import atexit
import random
import shutil

try:
    import tau_native
except ImportError:
    tau_native = None

# -------- Embedded Tau Specification --------
TAU_SPEC = r"""
always u[t] = i0[t]
"""

# -------- Configuration --------
TAU_READY_SIGNAL = 'Execution step: 0'
PROCESS_TIMEOUT = 120    # Tau startup timeout
COMM_TIMEOUT = 99999     # Tau response timeout

# -------- Base Patterns for Bitvector Expressions --------
DEFAULT_BV_WIDTH = 64
RANDOM_BV_WIDTHS = [1, 2, 4, 8, 16, 32, 64, 128, 256]

OPS_BV = ['+', '-', '!&', '!|', '!^']
COMPARISONS_BV = ['<', '!<', '<=', '!<=', '>', '!>', '>=', '!>=', '=', '!=']


def bv_const(value: int, width: int) -> str:
    return f"{{ {value} }}:bv[{width}]"


def input_ref(index: int, width: int, time_ref: str = "t") -> str:
    return f"i{index}[{time_ref}]:bv[{width}]"


def build_base_patterns_bv(width: int) -> list[str]:
    inputs = [input_ref(i, width) for i in range(1, 17)]
    max_value = (1 << width) - 1
    one = bv_const(min(1, max_value), width)
    two = bv_const(min(2, max_value), width)
    seven = bv_const(min(7, max_value), width)
    forty_two = bv_const(min(42, max_value), width)
    sixty_six = bv_const(min(66, max_value), width)
    one_seventy = bv_const(min(170, max_value), width)

    return [
        f"({inputs[0]} + {inputs[1]})",
        f"({inputs[2]} - {inputs[3]})",
        f"({inputs[4]} + {forty_two})",
        f"({inputs[6]} - {seven})",
        f"({inputs[8]} + ({inputs[9]} << {one}))",
        f"({inputs[0]} !& {inputs[1]})",
        f"({inputs[2]} !| {inputs[3]})",
        f"({inputs[4]} !^ {inputs[5]})",
        f"({inputs[6]} !& {inputs[6]})",
        f"(0 - {inputs[12]})",
        f"({inputs[13]} << {two})",
        f"({inputs[14]} >> {one})",
        f"(({inputs[0]} + {inputs[1]}) !& ({inputs[2]} - {one_seventy}))",
        f"((0 - ({inputs[3]} + {sixty_six})) !| ({inputs[4]} !^ {inputs[5]}))",
        f"(({inputs[6]} << {two}) + ({inputs[7]} >> {one}) - {inputs[8]})",
    ]


def choose_exprs_for_iteration(idx: int, base_patterns: list[str]) -> list[str]:
    count = len(base_patterns)
    if idx <= count:
        return [base_patterns[idx - 1]]
    if idx <= 2 * count:
        return base_patterns[:min(idx - count + 1, count)]
    rand_len = random.randint(max(3, count // 3), count)
    return random.sample(base_patterns, rand_len)


def build_folded_expr(exprs: list[str]) -> str:
    acc = exprs[0]
    for expr in exprs[1:]:
        acc = f"({acc} {random.choice(OPS_BV)} {expr})"
    return acc


def apply_lookback(rule: str, max_lookback: int) -> str:
    if max_lookback <= 0:
        return rule

    def lb_sub(match):
        k = random.randint(0, max_lookback)
        suffix = match.group(2) if match.group(2) else ""
        return f"i{match.group(1)}[t-{k}]{suffix}" if k > 0 else match.group(0)

    return re.sub(r'i(\d+)\[t\](:bv\[\d+\])?', lb_sub, rule)


def build_template_rule(exprs: list[str], idx: int, width: int, base_patterns: list[str]) -> str:
    while len(exprs) < 4:
        exprs.append(random.choice(base_patterns))

    out_a = 1
    out_b = 2 + (idx % 4)
    out_c = 6 + (idx % 5)
    max_value = (1 << width) - 1

    def sample_constant(candidates: list[int]) -> str:
        bounded = sorted({min(candidate, max_value) for candidate in candidates})
        return bv_const(random.choice(bounded), width)

    shift_a = bv_const(random.randint(1, min(4, max(1, width - 1))), width)
    shift_b = bv_const(random.randint(1, min(5, max(1, width - 1))), width)
    shift_c = bv_const(min(2, max_value), width)
    threshold_a = sample_constant([1, 2, 4, 8, 16, 32, 64, 96, 128, 160, 192])
    threshold_b = sample_constant([1, 2, 3, 4, 8, 16, 24, 48, 72, 96, 120])
    low_limit = sample_constant([1, 2, 4, 8, 16, 32, 64, 96, 128, 160])
    high_limit = sample_constant([2, 4, 8, 16, 32, 64, 128, 256, 384, 512, 768])
    time_cutoff = sample_constant([4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192])
    auth_token = sample_constant([1, 3, 7, 11, 42, 99, 255])

    folded = build_folded_expr(exprs)
    alt_folded = build_folded_expr(list(reversed(exprs[:max(2, min(6, len(exprs)))])))
    primary = exprs[0]
    secondary = exprs[1] if len(exprs) > 1 else primary
    tertiary = exprs[2] if len(exprs) > 2 else secondary
    quaternary = exprs[3] if len(exprs) > 3 else tertiary
    comparator = random.choice(COMPARISONS_BV)

    templates = [
        (
            "threshold gate",
            f"always ( (({primary} + {secondary}) >= {threshold_a} && o{out_a}[t] = {bv_const(1, width)})"
            f" || (({primary} + {secondary}) < {threshold_a} && o{out_a}[t] = {bv_const(0, width)}) )."
        ),
        (
            "feature extractor",
            f"always ( (o{out_a}[t]:bv[{width}] = ({primary} >> {shift_a}) + ({secondary} << {shift_b}))"
            f" && (o{out_b}[t]:bv[{width}] = ({tertiary} - ({primary} >> {bv_const(1, width)}))) )."
        ),
        (
            "temporal classifier",
            f"always ( (({primary} > {primary.replace('[t]', '[t-1]')} && o{out_a}[t] = {bv_const(1, width)})"
            f" || ({primary} <= {primary.replace('[t]', '[t-1]')} && o{out_a}[t] = {bv_const(0, width)}))"
            f" && (o{out_b}[t]:bv[{width}] = ({secondary} + ({tertiary} >> {bv_const(1, width)}))) )."
        ),
        (
            "multi output",
            f"always ( (o{out_a}[t]:bv[{width}] = {folded})"
            f" && (o{out_b}[t]:bv[{width}] = ({secondary} + {tertiary} - {quaternary}))"
            f" && (o{out_c}[t]:bv[{width}] = ({primary} !^ {secondary})) )."
        ),
        (
            "network style locals",
            f"always ( ex s1 ex s2 ex h1 ex h2 ("
            f" (s1 = (({primary} << {bv_const(1, width)}) + {primary} + ({secondary} << {bv_const(1, width)})))"
            f" && ((s1 >= {threshold_a} && h1 = {bv_const(1, width)}) || (s1 < {threshold_a} && h1 = {bv_const(0, width)}))"
            f" && (s2 = (({tertiary} << {shift_c}) + {tertiary} + ({bv_const(0, width)} - {quaternary})))"
            f" && ((s2 >= {threshold_b} && h2 = {bv_const(1, width)}) || (s2 < {threshold_b} && h2 = {bv_const(0, width)}))"
            f" && (((h2 = {bv_const(1, width)}) && o{out_a}[t] = {bv_const(min(200, max_value), width)})"
            f" || ((h2 = {bv_const(0, width)}) && ((h1 = {bv_const(1, width)} && o{out_a}[t] = {bv_const(min(50, max_value), width)})"
            f" || (h1 = {bv_const(0, width)} && o{out_a}[t] = {bv_const(0, width)}))))"
            f" && (o{out_b}[t]:bv[{width}] = s1 + s2)"
            f" ) )."
        ),
        (
            "temporary spending limit",
            f"always ( ({secondary} {comparator} {alt_folded})"
            f" -> ((i5[t]:bv[{width}] < {time_cutoff} && {primary} > {high_limit}) ? o{out_a}[t] = {bv_const(0, width)} :"
            f" ((!(i5[t]:bv[{width}] < {time_cutoff}) && {primary} > {low_limit}) ? o{out_a}[t] = {bv_const(0, width)} :"
            f" o{out_a}[t] = {bv_const(1, width)})) )."
        ),
        (
            "2fa gate",
            f"always ( (({folded} > {high_limit} && !(i6[t]:bv[{width}] = {auth_token}))"
            f" ? o{out_a}[t] = {bv_const(0, width)} : o{out_a}[t] = {bv_const(1, width)})"
            f" && (o{out_b}[t]:bv[{width}] = ({secondary} + ({tertiary} << {bv_const(1, width)}))) )."
        ),
    ]
    return templates[(idx - 1) % len(templates)][1]


def pick_rule_width(randomize: bool) -> int:
    return random.choice(RANDOM_BV_WIDTHS) if randomize else DEFAULT_BV_WIDTH


def build_rule_for_iteration(idx: int, max_lookback: int, width: int) -> str:
    base_patterns = build_base_patterns_bv(width)
    exprs = choose_exprs_for_iteration(idx, base_patterns)
    return apply_lookback(build_template_rule(exprs, idx, width, base_patterns), max_lookback)

# -------- Global State --------
PRINT_STDERR = True
last_exec_ts = None          # timestamp of the most‑recent "Execution step" seen
tau_process = None
tau_process_lock = threading.Lock()
tau_ready = threading.Event()
server_should_stop = threading.Event()
tau_stderr_lines = queue.Queue(maxsize=100)
current_rule_width = DEFAULT_BV_WIDTH
tau_interface = None
execution_mode = 'repl'

def read_stderr():
    """Reads Tau stderr, prints live and stores recent lines."""
    global tau_process, tau_stderr_lines, server_should_stop
    with tau_process_lock:
        proc = tau_process
    if not proc or not proc.stderr:
        return
    for line in proc.stderr:
        text = line.strip()
        if not text or server_should_stop.is_set():
            break
        if PRINT_STDERR:  # only print if the user asked for it
            print(f"\033[91m[TAU_STDERR] {text}\033[0m", file=sys.stderr)
        try:
            tau_stderr_lines.put_nowait(text)
        except queue.Full:
            tau_stderr_lines.get_nowait(); tau_stderr_lines.put_nowait(text)

def _wait_for_tau_ready(proc):
    """Common helper: wait for Tau ready signal on stdout."""
    global tau_ready
    start_time = time.monotonic()
    stdout_stream = proc.stdout
    while time.monotonic() - start_time < PROCESS_TIMEOUT:
        if proc.poll() is not None:
            raise RuntimeError("Tau process exited unexpectedly while waiting for ready signal.")
        buf = bytearray()
        fd = stdout_stream.fileno()
        while True:
            rlist, _, _ = select.select([fd], [], [], 0.1)
            if not rlist:
                if time.monotonic() - start_time > PROCESS_TIMEOUT:
                    break
                continue
            chunk = os.read(fd, 1)
            if not chunk:
                break
            buf += chunk
            if chunk == b'\n':
                break
        line = buf.decode('utf-8', errors='replace')
        if not line and proc.poll() is not None:
            break
        if line.strip():
            print(f"[TAU_STDOUT] {line.strip()}")
        if TAU_READY_SIGNAL in line:
            print(f"[TAU_STDOUT] got ready signal")
            tau_ready.set()
            return
    raise RuntimeError("Tau did not become ready in time")

def start_and_manage_tau_process_binary(spec_dir, spec_filename, tau_binary_path):
    """Starts Tau via local binary with the embedded spec and waits for ready."""
    global tau_process
    spec_path = os.path.join(spec_dir, spec_filename)
    if not os.path.isfile(tau_binary_path):
        raise FileNotFoundError(f"Tau binary not found: {tau_binary_path}")
    if not os.access(tau_binary_path, os.X_OK):
        raise PermissionError(f"Tau binary is not executable: {tau_binary_path}")
    proc = subprocess.Popen(
        [tau_binary_path, spec_path],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True, bufsize=1
    )
    with tau_process_lock:
        tau_process = proc
    threading.Thread(target=read_stderr, daemon=True).start()
    _wait_for_tau_ready(proc)

def start_and_manage_tau_process_mock(*_args):
    """Mock Tau REPL for local testing: prints ready, then for each step,
    reads all inputs and produces a result for o1[t] and a new exec step."""
    global tau_process, tau_ready
    code = (
        "import sys, time\n"
        "print('Execution step: 0'); sys.stdout.flush()\n"
        "step = 0\n"
        "while True:\n"
        "    step += 1\n"
        "    sys.stdout.write('i0[t] := '); sys.stdout.flush()\n"
        "    rule = sys.stdin.readline()\n"
        "    if not rule: break\n"
        "    sys.stdout.write(f'o1[t] := #b11110000\\n'); sys.stdout.flush()\n"
        "    sys.stdout.write(f'Execution step: {step}\\n'); sys.stdout.flush()\n"
        "    time.sleep(0.01)\n"
    )
    print("[DEBUG mock] launching mock Tau REPL")
    proc = subprocess.Popen(
        [sys.executable, '-u', '-c', code],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True, bufsize=1
    )
    with tau_process_lock:
        tau_process = proc
    threading.Thread(target=read_stderr, daemon=True).start()
    print("[DEBUG mock] mock Tau ready")
    tau_ready.set()

def communicate_with_tau_repl(input_rule: str) -> str:
    """Interact with Tau: respond to iN[t] prompts (i0[t] with the rule, others with random values),
    then return the o1[t] value after the next execution step."""
    global tau_process, tau_ready, last_exec_ts, current_rule_width
    if not tau_ready.is_set():
        raise RuntimeError("Tau not ready")
    with tau_process_lock:
        proc = tau_process
    if not proc or proc.poll() is not None:
        tau_ready.clear()
        raise RuntimeError("Tau process not running")

    current_stdin = proc.stdin
    current_stdout = proc.stdout
    start = time.monotonic()
    error_count = 0
    max_errors = 3
    output_val = None

    while time.monotonic() - start < COMM_TIMEOUT:
        if proc.poll() is not None:
            tau_ready.clear()
            raise RuntimeError(f"Tau process exited unexpectedly with code {proc.returncode}")

        buf = bytearray()
        fd = current_stdout.fileno()
        while True:
            if proc.poll() is not None:
                tau_ready.clear()
                raise RuntimeError(f"Tau process exited unexpectedly with code {proc.returncode}")
            rlist, _, _ = select.select([fd], [], [], 0.1)
            if not rlist:
                continue
            chunk = os.read(fd, 1)
            if not chunk:  # EOF
                break
            if chunk == b'\n':
                break
            buf += chunk
            if b':=' in buf:  # prompt may arrive without newline
                break
        line = buf.decode('utf-8', errors='replace')
        line_strip = line.strip()
        if line_strip:
            print(f"[TAU_STDOUT] {line_strip}")

        in_match = re.search(r'i(\d+)\[[^]]+\](?::bv\[(\d+)\])?\s*(:=|=)\s*$', line_strip)
        if in_match:
            idx2 = int(in_match.group(1))
            if idx2 == 0:
                payload = input_rule
            else:
                width = int(in_match.group(2) or current_rule_width)
                val = random.getrandbits(width)
                payload = f'{val}'

            print(f"[DEBUG] Responding to i{idx2}[t] with '\033[93m{payload}\033[0m'")
            current_stdin.write(payload + '\n')
            current_stdin.flush()
            buf.clear()            # reset buffer for next line
            continue

        if re.match(r".*(:=|=)\s*$", line_strip):
            continue

        out_match = re.search(r'o1\[[^]]+\]\s*(:=|=)\s*(.*)', line_strip)
        if out_match:
            output_val = out_match.group(2).strip()
            continue

        if line_strip.startswith("Execution step:"):
            now = time.monotonic()
            if last_exec_ts is not None:
                delta = now - last_exec_ts
                print(f"\033[92m[METRIC] Execution‑step interval: {delta:.6f}s\033[0m")
            last_exec_ts = now
            return output_val

        if "(Error)" in line_strip:
            error_count += 1
            print(f"[ERROR] communicate_with_tau_repl: Tau reported an error ({error_count}/{max_errors}): {line_strip}")
            if "Syntax Error" in line_strip:
                raise Exception(f"Tau syntax error: {line_strip}")
            if error_count >= max_errors:
                raise Exception(f"Too many errors from Tau ({error_count}). Last error: {line_strip}")
            continue

    raise TimeoutError("Did not receive full Tau response in time.")

def communicate_with_tau_native(input_rule: str) -> str:
    """Interact with Tau natively: send the rule and random inputs for i1..i16."""
    global tau_interface, last_exec_ts, current_rule_width

    if tau_interface is None:
        raise RuntimeError("Tau native interface not initialized")

    print(f"\033[96m[OUTGOING] {input_rule}\033[0m")

    input_values = {}
    for i in range(1, 17):
        val = random.getrandbits(current_rule_width)
        input_values[i] = str(val)

    output_val = tau_interface.communicate(
        rule_text=input_rule,
        target_output_stream_index=1,
        input_stream_values=input_values,
        apply_rules_update=True
    )

    now = time.monotonic()
    if last_exec_ts is not None:
        delta = now - last_exec_ts
        print(f"\033[92m[METRIC] Execution‑step interval: {delta:.6f}s\033[0m")
    last_exec_ts = now

    return str(output_val)

def communicate_with_tau(input_rule: str) -> str:
    global execution_mode
    if execution_mode == 'native':
        return communicate_with_tau_native(input_rule)
    else:
        return communicate_with_tau_repl(input_rule)

def main():
    global PROCESS_TIMEOUT, current_rule_width, tau_interface, execution_mode, PRINT_STDERR
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--iterations',      type=int,   default=50,
                        help='Number of rule steps to send (0 = infinite)')
    parser.add_argument('--delay',           type=float, default=1,
                        help='Delay between iterations (seconds)')
    parser.add_argument('--startup-timeout', type=float, default=PROCESS_TIMEOUT,
                        help='Seconds to wait for Tau ready signal')
    parser.add_argument('--mock',           action='store_true',
                        help='Use a mock Tau process for local testing')
    parser.add_argument('--no-stderr', action='store_false', dest='show_stderr',
                        help='Suppress Tau stderr output (default shows stderr)')
    parser.add_argument('--print-only', action='store_true',
                        help='Print test formulas without running Tau instance')

    # Execution mode selection
    parser.add_argument('--native', action='store_true',
                        help='Use native python bindings (tau_native)')
    parser.add_argument('--tau-binary', type=str, default=os.environ.get('TAU_BINARY', '../tau-lang/build-Release/tau'),
                        help='Path to tau binary to execute (preferred mode, defaults to ../tau-lang/build-Release/tau)')
    parser.add_argument('--max-lookback', type=int, default=0,
                    help='Maximum lookback steps for inputs (default 0). If > 0, rules will randomly use [t-k].')
    parser.add_argument('--width', type=int, default=None,
                        help='Set bitvector width to a specific value (overrides --random-bv-width)')
    parser.add_argument('--random-bv-width', action='store_true',
                        help='Pick one random bitvector width for this run from 1,2,4,8,16,32,64,128,256 instead of fixed bv[64].')
    parser.add_argument('--seed', type=int, default=None,
                        help='Seed of the rules and the input values, so two runs send the same load')
    args = parser.parse_args()
    if args.seed is not None:
        random.seed(args.seed)
    print(f"[DEBUG] CLI args: {args}")
    if args.width is not None:
        rule_width = args.width
    else:
        rule_width = pick_rule_width(args.random_bv_width)
    current_rule_width = rule_width
    print(f"[DEBUG] Using rule bitvector width: bv[{rule_width}]")

    # Handle print-only mode
    if args.print_only:
        print("# Generated test formulas:")
        iterations = args.iterations if args.iterations > 0 else 50  # Default to 50 if infinite
        for idx in range(1, iterations + 1):
            print(build_rule_for_iteration(idx, args.max_lookback, rule_width))
        return

    PRINT_STDERR = args.show_stderr
    PROCESS_TIMEOUT = args.startup_timeout

    # Prepare embedded spec for real Tau runs
    spec_dir = tempfile.mkdtemp(prefix='tau_spec_')
    spec_file = 'embedded_spec.tau'
    spec_path = os.path.join(spec_dir, spec_file)
    with open(spec_path, 'w') as f:
        f.write(TAU_SPEC)
    atexit.register(lambda: shutil.rmtree(spec_dir, ignore_errors=True))

    if args.native:
        if tau_native is None:
            print("Error: --native requested but tau_native could not be imported.", file=sys.stderr)
            sys.exit(1)
        execution_mode = 'native'
        print("[DEBUG] Initializing Tau Native Interface...")
        tau_interface = tau_native.TauInterface(spec_path)
        print("[DEBUG] main: Tau native is ready, entering iteration loop")
    else:
        execution_mode = 'repl'
        start_fn = None
        start_args = ()
        if args.mock:
            start_fn = start_and_manage_tau_process_mock
            start_args = ()
        elif args.tau_binary:
            start_fn = start_and_manage_tau_process_binary
            start_args = (spec_dir, spec_file, args.tau_binary)
        else:
            parser.print_help(sys.stdout)
            return

        # Launch Tau manager thread
        tau_ready.clear(); server_should_stop.clear()
        mgr = threading.Thread(target=start_fn,
                               args=start_args,
                               daemon=True)
        mgr.start()

        if not tau_ready.wait(timeout=PROCESS_TIMEOUT):
            print("Failed to start Tau", file=sys.stderr)
            sys.exit(1)
        print("[DEBUG] main: Tau REPL is ready, entering iteration loop")

    timings = []
    idx = 0
    try:
        while not server_should_stop.is_set():
            print(f"[DEBUG] main: start loop idx={idx+1}")
            idx += 1
            if args.iterations and idx > args.iterations:
                break
            rule = build_rule_for_iteration(idx, args.max_lookback, rule_width)
            start = time.time()
            try:
                out = communicate_with_tau(rule)
            except Exception as e:
                print(f"Iteration {idx} error: {e}")
                break
            dt = time.time() - start
            timings.append(dt)
            print(f"\033[92m[INCOMING] Result for o1[t]: {out}  ({dt:.4f}s)\033[0m")
            if args.delay:
                print(f"[DEBUG] main: sleep for {args.delay} seconds")
                time.sleep(args.delay)
    finally:
        if timings:
            total = sum(timings); avg = total/len(timings)
            print(f"Sent {len(timings)} in {total:.4f}s, avg {avg:.4f}s per call")
        server_should_stop.set()
        if execution_mode == 'repl':
            mgr.join(timeout=5)

if __name__ == '__main__':
    main()
