#!/usr/bin/env python3
"""
Cost of one Tau rule per account, measured through the `tau` Python binding only.

An account rule is one `always` clause: three amount thresholds on i1 (1000, 10000, 100000),
each cleared by an approver id on i18, i19 or i20, writing one bv[24] output (1 = allowed).

  parts N ...    N rules as the parts of ONE specification, each on its own output:
                 open time, step time, peak RSS
  add-one N ...  boot a router specification, feed a base rule, feed parts 1..N-1 as one
                 unit, then time the feed of part N alone
  single         one rule alone in its own interpreter: build, step, memory per live interpreter
  churn [K]      K times in ONE process: build an interpreter for another account's rule,
                 step it once, drop it; per block of 100 report build and step means, RSS and
                 tref_count. --with-shared adds one long-lived interpreter stepped every
                 iteration, --gc calls gc.collect() after each drop
  sweep          everything above with defaults, then the two tables (--quick skips the
                 points expected to reach the cap)

Every measurement point runs in a fresh child process, because the engine types streams
process-wide and may crash on teardown. Every verdict is checked; a wrong one is printed
loudly and the exit code becomes 1.

  python tau_account_rules_repro.py sweep --tau-dir DIR_WITH_TAU_SO --json results.json
  python tau_account_rules_repro.py parts 16 64 --tau-dir DIR_WITH_TAU_SO
  python tau_account_rules_repro.py churn 1000 --with-shared --gc
"""

import argparse
import gc
import json
import os
import platform
import random
import re
import resource
import signal
import subprocess
import sys
import tempfile
import time
from datetime import datetime

LIMITS = ((100000, 20), (10000, 19), (1000, 18))  # (amount threshold, approver stream)
ROUTER = "((!(i0[t] = 0)) ? ( u[t] = i0[t] && o0[t] = 0 ) : o0[t] = 1)."
BASE = "o1[t]:bv[24] = i1[t]:bv[24]"
PART_OUT = 1000  # part k of a shared specification writes o(PART_OUT + k)
SINGLE_OUT = 5
CHECK_AMOUNT = 1001
BANDS = ((1, 1000), (1001, 10000), (10001, 100000), (100001, 1000000))
VOTE_PRESENT = 0.6
MAX_ACCOUNTS = 21000  # approver ids 3k+1..3k+3 must fit in bv[16]
SEED = 20261109
BLOCK = 100
ANSI = re.compile(r"\x1b\[[0-9;]*m")

tau = None


# ---------------------------------------------------------------- rule texts
def bv(n, bits):
    return "{ #x%0*x }:bv[%d]" % (bits // 4, n, bits)


def approvers(k):
    return (3 * k + 1, 3 * k + 2, 3 * k + 3)  # for i18, i19, i20


def rule(k, out):
    ids = dict(zip((18, 19, 20), approvers(k)))
    blocked = " || ".join(
        "(i1[t]:bv[24] > %s && !(i%d[t]:bv[16] = %s))" % (bv(limit, 24), slot, bv(ids[slot], 16))
        for limit, slot in LIMITS)
    return "( %s ) ? (o%d[t]:bv[24] = %s) : (o%d[t]:bv[24] = %s)" % (
        blocked, out, bv(0, 24), out, bv(1, 24))


def conjunction(ks):
    return " && ".join("(%s)" % rule(k, PART_OUT + k) for k in ks)


def parts_spec(n):
    return "always ( (%s) && %s )." % (BASE, conjunction(range(1, n + 1)))


def part_unit(ks):
    return "always ( %s )." % conjunction(ks)


def single_spec(k):
    return "always ( %s )." % rule(k, SINGLE_OUT)


def expected(amount, slots):
    return '0' if any(amount > limit and slot not in slots for limit, slot in LIMITS) else '1'


def tx_values(k, amount, slots):
    values = {'i1': str(amount)}
    values.update({'i%d' % s: str(approvers(k)[s - 18]) for s in slots})
    return values


# ------------------------------------------------------------- engine access
class Refused(Exception):
    pass


def load_tau(tau_dir):
    global tau
    if tau_dir:
        sys.path.insert(0, os.path.abspath(tau_dir))
    import tau as module
    report = module.init_report()
    if report.has_error:
        raise Refused("tau failed to initialise: " + errors_of(report))
    tau = module


def errors_of(report):
    return "; ".join(s for s in (ANSI.sub('', str(e)).strip() for e in report.errors) if s)


def unwrap(res):
    return (res.value if res else None), res.report


def build(text):
    interp, report = unwrap(tau.get_interpreter(text))
    if interp is None or report.has_error:
        raise Refused("get_interpreter: " + (errors_of(report) or "no interpreter"))
    return interp


def run_step(interp, values, rule_text=None):
    """One step: feed every prompted input (a rule goes in on i0), return {output name: text}."""
    pending, rule_left = dict(values), rule_text
    for _ in range(100):
        required, report = unwrap(tau.get_inputs_for_step(interp))
        if required is None or report.has_error:
            raise Refused("get_inputs_for_step: " + (errors_of(report) or "no value"))
        assignments, used, offered = {}, [], False
        for stream in required:
            name = stream.name
            if name == 'i0' and rule_left is not None:
                assignments[stream], offered = rule_left, True
            elif name in pending:
                assignments[stream] = pending[name]
                used.append(name)
            else:
                assignments[stream] = 'F' if name == 'i0' else '0'
        before = interp.time_point
        outputs, report = unwrap(tau.step(interp, assignments))
        moved = interp.time_point != before
        if outputs is not None or moved:
            for name in used:
                del pending[name]
            if offered:
                rule_left = None
        if outputs is not None:
            if report.has_error:
                raise Refused("step: " + errors_of(report))
            return {o.name: str(v) for o, v in outputs.items()}
        if not (report.awaiting_input and not moved):
            raise Refused("step: " + (errors_of(report) or "no outputs"))
    raise Refused("step: no outputs after 100 prompts")


def mean(xs):
    return sum(xs) / len(xs)


def timed(fn, *args):
    started = time.perf_counter()
    value = fn(*args)
    return value, (time.perf_counter() - started) * 1000


def feed(interp, text):
    outs = run_step(interp, {}, rule_text=text)
    if outs.get('o0') != 'F':
        raise Refused("rule not accepted by the router (o0=%s)" % outs.get('o0'))


class Verdicts:
    def __init__(self):
        self.total, self.wrong = 0, []

    def add(self, who, label, want, got):
        self.total += 1
        if want != got:
            self.wrong.append({'who': who, 'label': label, 'want': want, 'got': got})

    def summary(self):
        return {'verdicts_total': self.total, 'verdicts_wrong': len(self.wrong),
                'wrong_examples': self.wrong[:5]}


def verify_accounts(interp, ks, out_of):
    """First/middle/last account: no approver blocks, the right i18 approver allows."""
    verdicts, steps = Verdicts(), []
    for k in ks:
        for label, slots in (('no approver', ()), ('i18 approver', (18,))):
            outs, ms = timed(run_step, interp, tx_values(k, CHECK_AMOUNT, slots))
            steps.append(ms)
            verdicts.add('account %d' % k, label, expected(CHECK_AMOUNT, slots),
                         outs.get('o%d' % out_of(k), 'missing'))
    return {'steps_ms': steps, 'step_mean_ms': sum(steps) / len(steps), **verdicts.summary()}


def picks(n):
    return sorted({1, (n + 1) // 2, n})


# ----------------------------------------------------------- child processes
def own_peak_mb():
    peak = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    return peak / (1024 * 1024 if sys.platform == 'darwin' else 1024)


def rss_mb(pid):
    try:
        out = subprocess.run(['ps', '-o', 'rss=', '-p', str(pid)], capture_output=True,
                             text=True, timeout=10).stdout.strip()
        return int(out) / 1024 if out else None
    except (OSError, ValueError, subprocess.SubprocessError):
        return None


def work_parts(job, progress):
    n = job['n']
    text = parts_spec(n)
    interp, ms = timed(build, text)
    progress({'open_s': ms / 1000, 'spec_chars': len(text)})
    return verify_accounts(interp, picks(n), lambda k: PART_OUT + k)


def work_add_one(job, progress):
    n = job['n']
    router = build(ROUTER)
    units = ["always ( %s )." % BASE] + ([part_unit(range(1, n))] if n > 1 else [])
    for i, unit in enumerate(units, 1):
        progress({'stage': 'setup feed %d/%d' % (i, len(units))})
        feed(router, unit)
    progress({'stage': 'timed feed'})
    _, ms = timed(feed, router, part_unit([n]))
    progress({'stage': 'checks', 'feed_s': ms / 1000})
    result = verify_accounts(router, picks(n), lambda k: PART_OUT + k)
    result['spec_print_s'] = timed(router.current_spec)[1] / 1000
    return result


def work_single(job, progress):
    interp, build_ms = timed(build, single_spec(0))
    verdicts, steps = Verdicts(), []
    for amount, slots in ((500, ()), (1001, ()), (1001, (18,)), (10001, (18,)), (10001, (18, 19)),
                          (100001, (18, 19)), (100001, (18, 19, 20))):
        outs, ms = timed(run_step, interp, tx_values(0, amount, slots))
        steps.append(ms)
        verdicts.add('account 0', '%d with %s' % (amount, slots), expected(amount, slots),
                     outs.get('o%d' % SINGLE_OUT, 'missing'))
    return {'build_ms': build_ms, 'first_step_ms': steps[0],
            'later_step_mean_ms': sum(steps[1:]) / len(steps[1:]), **verdicts.summary()}


def work_live(job, progress):
    held = []

    def hold(k):
        interp = build(single_spec(k))
        run_step(interp, tx_values(k, CHECK_AMOUNT, ()))
        held.append(interp)

    hold(0)  # first build pays one-time allocations, so it is not counted
    before = rss_mb(os.getpid())
    for k in range(1, job['count'] + 1):
        hold(k)
    after = rss_mb(os.getpid())
    return {'count': job['count'], 'rss_before_mb': before, 'rss_after_mb': after,
            'mb_per_interpreter': (after - before) / job['count']}


def work_churn(job, progress):
    rng = random.Random(SEED)
    shared = build("always ( %s )." % BASE) if job['shared'] else None
    tref_count = getattr(tau, 'tref_count', None)
    verdicts = Verdicts()
    builds, steps, shareds = [], [], []
    for i in range(job['k']):
        account = i % MAX_ACCOUNTS
        lo, hi = BANDS[rng.randrange(len(BANDS))]
        amount = rng.randint(lo, hi)
        slots = tuple(s for s in (18, 19, 20) if rng.random() < VOTE_PRESENT)
        interp, ms = timed(build, single_spec(account))
        builds.append(ms)
        outs, ms = timed(run_step, interp, tx_values(account, amount, slots))
        steps.append(ms)
        verdicts.add('account %d' % account, '%d with %s' % (amount, slots),
                     expected(amount, slots), outs.get('o%d' % SINGLE_OUT, 'missing'))
        if shared is not None:
            shareds.append(timed(run_step, shared, {'i1': str(amount)})[1])
        del interp, outs
        if job['gc']:
            gc.collect()
        if (i + 1) % BLOCK == 0 or i + 1 == job['k']:
            progress({'block': {'txs': i + 1, 'build_ms': mean(builds), 'step_ms': mean(steps),
                                'shared_ms': mean(shareds) if shareds else None,
                                'rss_mb': rss_mb(os.getpid()),
                                'tref': tref_count() if tref_count else None}})
            builds, steps, shareds = [], [], []
    return verdicts.summary()


WORKERS = {'parts': work_parts, 'add-one': work_add_one, 'single': work_single,
           'live': work_live, 'churn': work_churn}


def worker(job_path):
    with open(job_path) as f:
        job = json.load(f)

    def progress(obj):
        with open(job['events'], 'a') as f:
            f.write(json.dumps(obj) + "\n")

    signal.alarm(int(job['cap']) + 30)
    try:
        load_tau(job['tau_dir'])
        result = WORKERS[job['kind']](job, progress)
    except BaseException as e:
        result = {'error': "%s: %s" % (type(e).__name__, e)}
    result['final'] = True
    result['peak_rss_mb'] = own_peak_mb()
    progress(result)
    os._exit(0)  # the engine can crash on teardown after the result is out


# ------------------------------------------------------------------ parent
def git_head(path):
    folder = os.path.dirname(os.path.abspath(path))
    while folder != os.path.dirname(folder):
        if os.path.exists(os.path.join(folder, '.git')):
            try:
                return subprocess.run(['git', '-C', folder, 'rev-parse', 'HEAD'], capture_output=True,
                                      text=True, timeout=60).stdout.strip() or None
            except (OSError, subprocess.SubprocessError):
                return None
        folder = os.path.dirname(folder)
    return None


def engine_stamp():
    path = tau.__file__
    return {'module': path, 'mtime': datetime.fromtimestamp(os.path.getmtime(path)).isoformat(
        timespec='seconds'), 'git_head': git_head(path)}


class Run:
    def __init__(self, args):
        self.args, self.points, self.bad = args, [], 0
        self.engine = engine_stamp()
        self.engine_end = None
        self.load_start = os.getloadavg()
        self.load_end = None
        self.started = datetime.now().isoformat(timespec='seconds')

    def measure(self, kind, label, **params):
        args = self.args
        point = {'kind': kind, 'label': label, 'params': params, 'load1': os.getloadavg()[0]}
        print("[%s] start (cap %g s, load %.1f)" % (label, args.cap, point['load1']))
        with tempfile.TemporaryDirectory(prefix='tau_repro_') as tmp:
            job = {'kind': kind, 'cap': args.cap, 'tau_dir': args.tau_dir,
                   'events': os.path.join(tmp, 'events.jsonl'), **params}
            job_path = os.path.join(tmp, 'job.json')
            with open(job_path, 'w') as f:
                json.dump(job, f)
            killed, peak = self.supervise(job_path, label, point)
            point.update(self.collect(job['events'], killed, peak, point))
        self.finish_point(point)
        return point

    def supervise(self, job_path, label, point):
        args = self.args
        proc = subprocess.Popen([sys.executable, os.path.abspath(__file__), '--worker', job_path],
                                stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                                stderr=subprocess.DEVNULL)
        started = last_beat = time.monotonic()
        next_poll, peak, killed = 0.0, 0.0, None
        try:
            while proc.poll() is None:
                time.sleep(0.2)
                now = time.monotonic()
                if now - started > args.cap:
                    killed = 'cap'
                elif now >= next_poll:
                    next_poll = now + 1.0
                    mb = rss_mb(proc.pid) or 0.0
                    peak = max(peak, mb)
                    if mb > args.rss_cap_gb * 1024:
                        killed = 'rss'
                    if now - last_beat >= 60:
                        last_beat = now
                        print("[%s] running %.0f s, RSS %.0f MB" % (label, now - started, mb))
                if killed:
                    proc.kill()
                    break
            proc.wait()
        finally:
            if proc.poll() is None:
                proc.kill()
                proc.wait()
        point['wall_s'] = time.monotonic() - started
        if killed:
            point['killed_after_s'] = point['wall_s']
        return killed, peak

    @staticmethod
    def collect(events_path, killed, peak, point):
        merged, blocks = {}, []
        if os.path.exists(events_path):
            with open(events_path) as f:
                for line in f:
                    event = json.loads(line)
                    if 'block' in event:
                        blocks.append(event['block'])
                    else:
                        merged.update(event)
        if blocks:
            merged['blocks'] = blocks
        merged['peak_rss_mb'] = max(merged.get('peak_rss_mb') or 0.0, peak)
        if killed:
            merged['status'] = killed
        elif merged.get('error'):
            merged['status'] = 'error'
        elif merged.get('final'):
            merged['status'] = 'ok'
        else:
            merged['status'] = 'died'
        return merged

    def finish_point(self, point):
        label, status = point['label'], point['status']
        wrong = point.get('verdicts_wrong', 0)
        if status in ('cap', 'rss'):
            print("[%s] killed by the %s guard after %.0f s, stage: %s" % (
                label, status, point['wall_s'], point.get('stage') or 'build'))
        elif status != 'ok':
            print("[%s] FAILED (%s): %s" % (label, status, point.get('error') or 'child died without a result'))
            self.bad += 1
        else:
            print("[%s] done in %.1f s, peak RSS %.0f MB, verdicts %d/%d ok" % (
                label, point['wall_s'], point['peak_rss_mb'],
                point.get('verdicts_total', 0) - wrong, point.get('verdicts_total', 0)))
        if wrong:
            self.bad += 1
            print("!!! WRONG VERDICT in %s: %s" % (label, json.dumps(point['wrong_examples'])))
        self.points.append(point)
        self.save()

    # ---- commands
    def parts(self, ns):
        for n in ns:
            self.measure('parts', 'parts %d' % n, n=n)

    def add_one(self, ns):
        for n in ns:
            self.measure('add-one', 'add-one %d' % n, n=n)

    def single(self):
        for i in range(self.args.repeats):
            self.measure('single', 'single %d/%d' % (i + 1, self.args.repeats))
        self.measure('live', 'live %d' % self.args.live, count=self.args.live)

    def churn(self, k, shared, collect):
        label = 'churn %d%s%s' % (k, ' with-shared' if shared else '', ' gc' if collect else '')
        self.measure('churn', label, k=k, shared=shared, gc=collect)

    def sweep(self):
        quick = self.args.quick
        self.single()
        self.churn(1000, False, False)
        self.churn(1000, True, False)
        self.parts((16, 64, 256, 1024) if quick else (16, 64, 256, 1024, 2048))
        self.add_one((16, 64, 256) if quick else (16, 64, 256, 1024))

    # ---- output
    def save(self):
        if self.args.json:
            with open(self.args.json, 'w') as f:
                json.dump(self.document(), f, indent=1)

    def document(self):
        return {'argv': sys.argv, 'started': self.started, 'python': sys.version.split()[0],
                'platform': platform.platform(), 'cores': os.cpu_count(),
                'engine_start': self.engine, 'engine_end': self.engine_end,
                'load1_start': self.load_start[0], 'load1_end': self.load_end[0] if self.load_end else None,
                'cap_s': self.args.cap, 'rss_cap_gb': self.args.rss_cap_gb, 'points': self.points}

    def finish(self):
        self.engine_end = engine_stamp()
        self.load_end = os.getloadavg()
        if self.engine_end != self.engine:
            print("!!! the tau module or its source revision changed during the run")
            self.bad += 1
        self.save()


# ------------------------------------------------------------------- tables
def fmt_s(x):
    return "%.3f" % x if x < 0.1 else "%.2f" % x if x < 1 else "%.1f" % x if x < 100 else "%.0f" % x


def fmt_ms(x):
    return "%.2f ms" % x if x < 1 else "%.1f ms" % x if x < 10 else "{:,.0f} ms".format(x)


def fmt_mb(x):
    return "%.0f MB" % x if x < 1000 else "%.1f GB" % (x / 1000)


def flag(p, text):
    return text + (" (WRONG VERDICT)" if p.get('verdicts_wrong') else "")


def killed_text(p, cap):
    return "killed at %g s" % cap if p['status'] == 'cap' else "killed over the RSS cap"


def markdown(headers, rows):
    return ["| " + " | ".join(headers) + " |", "|" + "|".join("---" for _ in headers) + "|"] + [
        "| " + " | ".join(r) + " |" for r in rows]


def table_a(points, cap):
    parts = {p['params']['n']: p for p in points if p['kind'] == 'parts'}
    adds = {p['params']['n']: p for p in points if p['kind'] == 'add-one'}
    rows = []
    for n in sorted(set(parts) | set(adds)):
        p, a = parts.get(n), adds.get(n)
        open_c = step_c = rss_c = add_c = ''
        if p:
            open_c = flag(p, fmt_s(p['open_s']) + " s") if 'open_s' in p else (
                killed_text(p, cap) if p['status'] in ('cap', 'rss') else "error")
            step_c = fmt_ms(p['step_mean_ms']) if 'step_mean_ms' in p else ''
            rss_c = fmt_mb(p['peak_rss_mb']) if 'step_mean_ms' in p else ''
        if a:
            if 'feed_s' in a:
                add_c = flag(a, fmt_s(a['feed_s']) + " s")
            elif a['status'] in ('cap', 'rss'):
                add_c = ("not reached in %g s" % cap) if (a.get('stage') or '').startswith('setup') \
                    else killed_text(a, cap)
            else:
                add_c = "error"
        rows.append(["{:,}".format(n), open_c, step_c, rss_c, add_c])
    return markdown(['parts', 'open', 'one step', 'peak RSS', 'add one part by update'], rows)


def table_b(points):
    singles = [p for p in points if p['kind'] == 'single' and p['status'] == 'ok']
    live = next((p for p in points if p['kind'] == 'live' and p['status'] == 'ok'), None)
    rows = [['build from text (`get_interpreter`)', fmt_ms(mean([p['build_ms'] for p in singles])) if singles else ''],
            ['one step (first step of a fresh interpreter)',
             fmt_ms(mean([p['first_step_ms'] for p in singles])) if singles else ''],
            ['one step of the chain\'s shared rules', 'not measured here (it needs the chain\'s own rules)'],
            ['memory per live interpreter', "about %.2f MB" % live['mb_per_interpreter'] if live else '']]
    lines = markdown(['', ''], rows)
    if singles:
        lines += ["", "Means over %d fresh processes; later steps of the same interpreter average %s." % (
            len(singles), fmt_ms(mean([p['later_step_mean_ms'] for p in singles])))]
    if live:
        lines += ["Memory: %d interpreters held live, RSS %.0f -> %.0f MB." % (
            live['count'], live['rss_before_mb'], live['rss_after_mb'])]
    return lines


def churn_lines(points):
    lines = []
    for p in (q for q in points if q['kind'] == 'churn' and q.get('blocks')):
        blocks = p['blocks']
        last_i = min(8, len(blocks) - 1)  # transactions 801-900 when the run is long enough
        first, last = blocks[0], blocks[last_i]
        last_from = blocks[last_i - 1]['txs'] + 1 if last_i else 1
        lines += ["", "%s: step of the new interpreter %s over transactions 1-%d, %s over %d-%d "
                  "(x%.1f); build %s then %s." % (
                      p['label'], fmt_ms(first['step_ms']), first['txs'], fmt_ms(last['step_ms']),
                      last_from, last['txs'], last['step_ms'] / first['step_ms'],
                      fmt_ms(first['build_ms']), fmt_ms(last['build_ms'])), ""]
        shared = any(b['shared_ms'] is not None for b in blocks)
        headers = ['txs up to', 'build', 'step'] + (['shared step'] if shared else []) + ['RSS', 'tref_count']
        rows = [[str(b['txs']), fmt_ms(b['build_ms']), fmt_ms(b['step_ms'])] +
                ([fmt_ms(b['shared_ms'])] if shared else []) +
                [fmt_mb(b['rss_mb']) if b['rss_mb'] else '-', '-' if b['tref'] is None else "{:,}".format(b['tref'])]
                for b in blocks]
        lines += markdown(headers, rows)
    return lines


def render(run):
    pts = run.points
    out = []
    if any(p['kind'] in ('parts', 'add-one') for p in pts):
        out += ["## One specification with N parts", ""] + table_a(pts, run.args.cap) + [""]
    if any(p['kind'] in ('single', 'live') for p in pts):
        out += ["## One interpreter per rule", ""] + table_b(pts) + [""]
    growth = churn_lines(pts)
    if growth:
        out += ["## Build, step, drop in one process"] + growth + [""]
    out += ["engine: %s (tau-lang %s, built %s); python %s; load1 %.1f -> %.1f; single runs" % (
        run.engine['module'].rsplit(os.sep, 1)[-1], (run.engine['git_head'] or 'unknown')[:7],
        run.engine['mtime'], sys.version.split()[0], run.load_start[0], run.load_end[0])]
    return "\n".join(out)


# --------------------------------------------------------------------- main
def parse():
    common = argparse.ArgumentParser(add_help=False, argument_default=argparse.SUPPRESS)
    common.add_argument('--tau-dir', help="directory holding the tau module (default: PYTHONPATH)")
    common.add_argument('--cap', type=float, help="seconds per point before the child is killed (300)")
    common.add_argument('--rss-cap-gb', type=float, help="child RSS in GB before it is killed (8)")
    common.add_argument('--json', help="write all points to this file")
    common.add_argument('--md', help="write the tables to this file")
    parser = argparse.ArgumentParser(description=__doc__, parents=[common],
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.set_defaults(tau_dir=None, cap=300.0, rss_cap_gb=8.0, json=None, md=None)
    parser.add_argument('--worker', help=argparse.SUPPRESS)
    sub = parser.add_subparsers(dest='cmd')
    for name in ('parts', 'add-one'):
        sub.add_parser(name, parents=[common]).add_argument('n', type=int, nargs='+')
    single = sub.add_parser('single', parents=[common])
    single.add_argument('--repeats', type=int, default=5, help="fresh processes for build and step")
    single.add_argument('--live', type=int, default=200, help="interpreters held for the memory figure")
    churn = sub.add_parser('churn', parents=[common])
    churn.add_argument('k', type=int, nargs='?', default=1000)
    churn.add_argument('--with-shared', action='store_true')
    churn.add_argument('--gc', action='store_true')
    sweep = sub.add_parser('sweep', parents=[common])
    sweep.add_argument('--quick', action='store_true')
    sweep.set_defaults(repeats=5, live=200)
    args = parser.parse_args()
    if not args.worker and not args.cmd:
        parser.error("choose a command")
    return args


def main():
    args = parse()
    if args.worker:
        worker(args.worker)
    sys.stdout.reconfigure(line_buffering=True)
    signal.signal(signal.SIGTERM, lambda signum, frame: sys.exit(128 + signum))
    load_tau(args.tau_dir)
    print("tau module: %s" % tau.__file__)
    run = Run(args)
    print("engine: %s" % json.dumps(run.engine))
    try:
        if args.cmd == 'parts':
            run.parts(args.n)
        elif args.cmd == 'add-one':
            run.add_one(args.n)
        elif args.cmd == 'single':
            run.single()
        elif args.cmd == 'churn':
            run.churn(args.k, args.with_shared, args.gc)
        else:
            run.sweep()
    finally:
        run.finish()
        text = render(run)
        print("\n" + text)
        if args.md:
            with open(args.md, 'w') as f:
                f.write(text + "\n")
    sys.exit(1 if run.bad else 0)


if __name__ == '__main__':
    main()
