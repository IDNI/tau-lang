#!/usr/bin/env python3
"""Sweep every LTL formula of a given operator count against Spot.

Each formula over o1, o2 and i1 goes through realizable, sat, valid and the
normal forms. ltlsynt decides the first two, ltlfilt the third and the
equivalence of each printed normal form. Past operators reach Spot as testers.
With --run, a sample of the realizable formulas also runs on periodic inputs,
and the formula is evaluated on the lasso of the printed trace.

Exit status 0 means no defect. A defect prints one line; --out keeps a record
per formula, and a rerun with the same --out skips the formulas it holds.

    ltl_sweep.py --tau build/release/tau --mode n2 --jobs 8
"""
import argparse, collections, json, os, random, re, resource, shutil
import subprocess, sys
from concurrent.futures import ThreadPoolExecutor, as_completed

TAU = None
TIMEOUT_SEC = 240
ADDRESS_SPACE = 6 << 30
UN = ['G', 'F', 'O', 'H']
BT = ['U', 'R', 'W', 'S', 'T']
BB = ['&&', '||', '->', '<->', '^^']
ATOMS = ['a', 'b', 'c']
ATOM_TAU = {'a': 'o1[t]:bv[1] = 1', 'b': 'o2[t]:bv[1] = 1', 'c': 'i1[t]:bv[1] = 1'}
ATOM_SPOT = {'a': 'o1', 'b': 'o2', 'c': 'i1'}
TEMPORAL = set(UN) | set(BT)
PAST = {'O', 'H', 'S', 'T'}
FUTURE = {'G', 'F', 'U', 'R', 'W'}


def nops(f):
    return 0 if isinstance(f, str) else 1 + sum(nops(x) for x in f[1:])


def tau_str(f):
    if isinstance(f, str):
        return {'t': 'T', 'f': 'F'}.get(f, ATOM_TAU.get(f, f))
    if len(f) == 2:
        return f'{f[0]} ({tau_str(f[1])})'
    return f'({tau_str(f[1])}) {f[0]} ({tau_str(f[2])})'


def has(f, ops):
    return False if isinstance(f, str) else f[0] in ops or any(has(x, ops) for x in f[1:])


def scoped(f):
    """True when every atom sits under a temporal operator."""
    if isinstance(f, str):
        return f in ('t', 'f')
    if f[0] in TEMPORAL:
        return True
    return all(scoped(x) for x in f[1:])


def conjuncts(f):
    return conjuncts(f[1]) + conjuncts(f[2]) if not isinstance(f, str) and f[0] == '&&' else [f]


def invariant_reading(f):
    """f as tau reads it: without a future operator, or beside another
    conjunct, a clause with no future operator is an `always` (README
    "Lookback initialization")."""
    cs = conjuncts(f)
    if not has(f, FUTURE) or len(cs) > 1:
        cs = [c if has(c, FUTURE) else ['G', c] for c in cs]
    r = cs[0]
    for c in cs[1:]:
        r = ['&&', r, c]
    return r


class Spot:
    """Past-free Spot string of a formula, with one tester per past operator."""

    def __init__(self):
        self.defs_c, self.defs_p = [], []
        self.aux_c, self.aux_p = [], []
        self.memo = {}

    def go(self, f):
        if isinstance(f, str):
            return {'t': '1', 'f': '0'}.get(f, ATOM_SPOT.get(f, f))
        k = json.dumps(f)
        if k in self.memo:
            return self.memo[k]
        r = self._go(f)
        self.memo[k] = r
        return r

    def _go(self, f):
        o = f[0]
        if len(f) == 2:
            x = self.go(f[1])
            if o == '!':
                return f'(!({x}))'
            if o in 'GF':
                return f'({o}({x}))'
            if o == 'O':
                return self.go(['S', 't', f[1]])
            if o == 'H':
                return self.go(['!', ['S', 't', ['!', f[1]]]])
        l, r = self.go(f[1]), self.go(f[2])
        if o in ('&&', '||', '->', '<->'):
            return f'(({l}) {dict(zip(BB, ["&", "|", "->", "<->"]))[o]} ({r}))'
        if o == '^^':
            return f'(({l}) xor ({r}))'
        if o in ('U', 'R', 'W'):
            return f'(({l}) {o} ({r}))'
        if o == 'T':
            return self.go(['!', ['S', ['!', f[1]], ['!', f[2]]]])
        assert o == 'S', o
        causal = not (has(f[1], FUTURE) or has(f[2], FUTURE))
        n = len(self.aux_c) + len(self.aux_p)
        p = f'px{n}'
        d = f'(({p} <-> ({r})) & G((X {p}) <-> (X({r}) | (X({l}) & {p}))))'
        (self.aux_c if causal else self.aux_p).append(p)
        (self.defs_c if causal else self.defs_p).append(d)
        return p


def lim():
    # macOS refuses RLIMIT_AS; a refused limit must not stop the child
    try:
        resource.setrlimit(resource.RLIMIT_AS, (ADDRESS_SPACE, ADDRESS_SPACE))
    except (ValueError, OSError):
        pass


def sh(cmd, inp=None, tmo=60):
    try:
        r = subprocess.run(cmd, input=inp, capture_output=True, text=True, timeout=tmo, preexec_fn=lim)
        return r.returncode, r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        return -99, 'TIMEOUT'


def ltlsynt(ins, outs, phi):
    rc, out = sh(['ltlsynt', '--realizability', f'--ins={",".join(ins)}', f'--outs={",".join(outs)}', '-f', phi], tmo=60)
    if rc == 0:
        return True
    if rc == 1:
        return False
    return None


def spot_realizable(f):
    """(lower, upper) bound of realizability. Equal when no tester reads the future."""
    s = Spot()
    phi = s.go(f)
    defs = ' & '.join(s.defs_c + [phi])
    low_all = ' & '.join(s.defs_c + s.defs_p + [phi])
    outs = ['o1', 'o2'] + s.aux_c
    lo = ltlsynt(['i1'], outs + s.aux_p, low_all)
    if not s.aux_p:
        return lo, lo
    hi = ltlsynt(['i1'] + s.aux_p, outs, f"({' & '.join(s.defs_p)}) -> ({defs})")
    return lo, hi


def spot_sat(phi):
    rc, out = sh(['ltlfilt', '--satisfiable', '-f', phi, '-c'], tmo=60)
    return out.strip().splitlines()[0] != '0' if rc in (0, 1) and out.strip() else None


def spot_valid(f):
    s = Spot()
    n = s.go(['!', f])
    return None if (r := spot_sat(' & '.join(s.defs_c + s.defs_p + [n]))) is None else not r


def spot_equiv(f, g):
    s = Spot()
    n = s.go(['!', ['<->', f, g]])
    return None if (r := spot_sat(' & '.join(s.defs_c + s.defs_p + [n]))) is None else not r


TOK = re.compile(r"\s*(<->|->|&&|\|\||\^\^|!|\(|\)|always\b|sometimes\b|[oi]\d\[t\](?::bv\[1\])?'?\s*(?:!=|=)\s*[01]|[oi]\d\[t\](?::bv\[1\])?'?\s*(?:!=|=)\s*[oi]\d\[t\](?::bv\[1\])?'?|[GFOHURWSTA-Z]\b)")


def lex(s):
    out, i = [], 0
    while i < len(s):
        if s[i:].strip() == '':
            break
        m = TOK.match(s, i)
        if not m:
            raise ValueError('lex ' + s[i:])
        out.append(m.group(1))
        i = m.end()
    return out


def atom_of(t):
    m = re.match(r"([oi]\d)\[t\](?::bv\[1\])?('?)\s*(!=|=)\s*(.*)$", t)
    v = {'o1': 'a', 'o2': 'b', 'i1': 'c'}[m.group(1)]
    x = ['!', v] if m.group(2) else v
    rhs = m.group(4)
    if rhs in '01':
        pos = (rhs == '1') == (m.group(3) == '=')
    else:
        m2 = re.match(r"([oi]\d)\[t\](?::bv\[1\])?('?)", rhs)
        w = {'o1': 'a', 'o2': 'b', 'i1': 'c'}[m2.group(1)]
        y = ['!', w] if m2.group(2) else w
        e = ['<->', x, y]
        return e if m.group(3) == '=' else ['!', e]
    return x if pos else ['!', x]


class P:
    def __init__(self, s):
        self.t, self.i = lex(s), 0

    def peek(self):
        return self.t[self.i] if self.i < len(self.t) else None

    def eat(self, x=None):
        v = self.peek()
        if x is not None and v != x:
            raise ValueError(f'want {x} got {v}')
        self.i += 1
        return v

    def parse(self):
        r = self.rimp()
        if self.peek() is not None:
            raise ValueError('trailing ' + str(self.peek()))
        return r

    def rimp(self):
        l = self.iff()
        if self.peek() == '->':
            self.eat()
            return ['->', l, self.rimp()]
        return l

    def iff(self):
        l = self.or_()
        if self.peek() == '<->':
            self.eat()
            return ['<->', l, self.iff()]
        return l

    def or_(self):
        l = self.xor()
        while self.peek() == '||':
            self.eat()
            l = ['||', l, self.xor()]
        return l

    def xor(self):
        l = self.and_()
        while self.peek() == '^^':
            self.eat()
            l = ['^^', l, self.and_()]
        return l

    def and_(self):
        l = self.bin()
        while self.peek() == '&&':
            self.eat()
            l = ['&&', l, self.bin()]
        return l

    def bin(self):
        l = self.pre()
        if self.peek() in ('U', 'R', 'W', 'S', 'T'):
            o = self.eat()
            return [o, l, self.bin()]
        return l

    def pre(self):
        v = self.peek()
        if v == '!':
            self.eat()
            return ['!', self.pre()]
        if v == 'always' or v == 'G':
            self.eat()
            return ['G', self.pre()]
        if v == 'sometimes':
            self.eat()
            return ['F', self.pre()]
        if v in ('O', 'H'):
            self.eat()
            return [v, self.pre()]
        if v == 'F':
            self.eat()
            if self.peek() in (None, ')', '&&', '||', '->', '<->', '^^', 'U', 'R', 'W', 'S'):
                return 'f'
            if self.peek() == 'T' and (self.i + 1 < len(self.t)) and self.t[self.i + 1] not in (
                    ')', '&&', '||', '->', '<->', '^^', 'U', 'R', 'W', 'S', 'T'):
                return 'f'
            return ['F', self.pre()]
        if v == 'T':
            self.eat()
            return 't'
        if v == '(':
            self.eat()
            r = self.rimp()
            self.eat(')')
            return r
        if v and re.match(r'[oi]\d', v):
            self.eat()
            return atom_of(v)
        raise ValueError(f'prim {v}')


CMDS = ['realizable', 'sat', 'valid', 'n', 'nnf', 'dnf', 'cnf', 'mnf']


def run_tau(f, cmds=CMDS, env_extra=None, opts=()):
    text = tau_str(f)
    inp = ''.join(f'{c} {text}\n' for c in cmds) + 'quit\n'
    env = dict(os.environ)
    if env_extra:
        env.update(env_extra)
    try:
        r = subprocess.run([TAU, '--color', 'false', '--benchmarks', 'false', '--ltl-timeout', '30', *opts],
                           input=inp, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                           timeout=TIMEOUT_SEC, preexec_fn=lim, env=env)
    except subprocess.TimeoutExpired:
        return {'_crash': 'TIMEOUT'}
    out = re.sub(r'\x1b\[[0-9;]*m', '', r.stdout)
    segs = re.split(r'(?:\[ %\d+ \] )?tau> ', out)[1:]
    res = {}
    for c, seg in zip(cmds, segs):
        m = re.findall(r'^%\d+: (.*)$', seg, re.M)
        res[c] = m[-1].strip() if m else 'ERR: ' + ' | '.join(seg.strip().splitlines()[:4])[:240]
    if r.returncode != 0 or len(segs) <= len(cmds) - 1:
        res['_crash'] = f'rc={r.returncode} segs={len(segs)} tail={out[-300:]}'
    return res


def verdict(s):
    return True if s == 'T' else False if s == 'F' else None


def check(f):
    rec = {'f': f, 'txt': tau_str(f), 'scoped': scoped(f), 'n': nops(f), 'bad': []}
    res = run_tau(f)
    rec['tau'] = res
    if '_crash' in res:
        rec['bad'].append(['crash', res['_crash']])
        return rec
    # README: a bare formula joined to a temporal one by more than && is refused
    if not rec['scoped'] and all('scoped by a temporal quantifier' in res[c] for c in ('realizable', 'sat')):
        return rec
    read = invariant_reading(f)
    lo, hi = spot_realizable(read)
    rec['spot_real'] = [lo, hi]
    rec['spot_valid'] = spot_valid(read)
    for c in ('realizable', 'sat'):
        v = verdict(res[c])
        if v is None:
            # the two bounds of a tester over a future operand differ
            if lo is hi or 'past over future' not in res[c]:
                rec['bad'].append([c + '-noverdict', res[c]])
        elif lo is not None and hi is not None and lo == hi and v != lo and rec['scoped']:
            rec['bad'].append([c, v, lo])
    v = verdict(res['valid'])
    if v is None:
        rec['bad'].append(['valid-noverdict', res['valid']])
    elif rec['spot_valid'] is not None and v != rec['spot_valid'] and rec['scoped']:
        rec['bad'].append(['valid', v, rec['spot_valid']])
    if res['realizable'] != res['sat']:
        rec['bad'].append(['real-vs-sat', res['realizable'], res['sat']])
    if verdict(res['valid']) and verdict(res['sat']) is False:
        rec['bad'].append(['valid-not-sat'])
    for c in ('n', 'nnf', 'dnf', 'cnf', 'mnf'):
        s = res[c]
        if s.startswith('ERR'):
            rec['bad'].append([c + '-err', s])
            continue
        try:
            g = P(s).parse()
        except Exception as e:
            rec['bad'].append([c + '-unparsed', str(e), s])
            continue
        if rec['scoped']:
            eq = spot_equiv(read, invariant_reading(g))
            if eq is False:
                rec['bad'].append([c + '-notequiv', s])
            elif eq is None:
                rec['bad'].append([c + '-eq-unknown', s])
    return rec


def enum(n, atoms, unary, binary):
    """Every formula with exactly n operators, its leaves taken from atoms."""
    if n == 0:
        yield from atoms
        return
    for k in range(n):
        for x in enum(k, atoms, unary, binary):
            if k == n - 1:
                for u in unary:
                    yield [u, x]
            for b in binary:
                for y in enum(n - 1 - k, atoms, unary, binary):
                    yield [b, x, y]


def canonical(f):
    """Keep one of each o1/o2 renaming: o1 is the first output to appear."""
    s = json.dumps(f)
    a, b = s.find('"a"'), s.find('"b"')
    return not (b >= 0 and (a < 0 or b < a))


RUN_STEPS = 40
PATTERNS = {'zero': [0], 'one': [1], 'alt': [0, 1], 'p3': [1, 0, 0], 'stem': None}


def inputs(name):
    if name == 'stem':
        return [1, 1, 0] + [0] * (RUN_STEPS + 7)
    p = PATTERNS[name]
    return [p[i % len(p)] for i in range(RUN_STEPS + 10)]


def lasso(tr):
    """Smallest (stem, period) whose tail of the trace repeats over at least 3 periods."""
    n = len(tr)
    for q in range(1, 13):
        if n < 3 * q:
            break
        s = n - q
        while s > 0 and tr[s - 1] == tr[s - 1 + q]:
            s -= 1
        if n - s >= 3 * q and s + 2 * q <= n:
            return s, q
    return None


def ev(f, w, q):
    n = len(w)
    succ = lambda i: i + 1 if i + 1 < n else n - q
    if isinstance(f, str):
        if f in 'tf':
            return [f == 't'] * n
        return [x[f] for x in w]
    o = f[0]
    if o == '!':
        return [not x for x in ev(f[1], w, q)]
    if o in ('O', 'H'):
        return ev(['S', 't', f[1]] if o == 'O' else ['!', ['S', 't', ['!', f[1]]]], w, q)
    if o == 'G':
        return ev(['R', 'f', f[1]], w, q)
    if o == 'F':
        return ev(['U', 't', f[1]], w, q)
    a = ev(f[1], w, q)
    b = ev(f[2], w, q)
    if o == '&&':
        return [x and y for x, y in zip(a, b)]
    if o == '||':
        return [x or y for x, y in zip(a, b)]
    if o == '->':
        return [(not x) or y for x, y in zip(a, b)]
    if o == '<->':
        return [x == y for x, y in zip(a, b)]
    if o == '^^':
        return [x != y for x, y in zip(a, b)]
    if o == 'S':
        r, prev = [], False
        for i in range(n):
            prev = b[i] or (a[i] and prev)
            r.append(prev)
        return r
    if o == 'T':
        r, prev = [], True
        for i in range(n):
            prev = b[i] and (a[i] or prev)
            r.append(prev)
        return r
    if o in ('U', 'W'):
        v = [o == 'W'] * n
    else:
        v = [True] * n
    for _ in range(2 * n + 2):
        c = False
        for i in range(n - 1, -1, -1):
            nx = v[succ(i)]
            x = (b[i] or (a[i] and nx)) if o in ('U', 'W') else (b[i] and (a[i] or nx))
            c |= x != v[i]
            v[i] = x
        if not c:
            break
    return v


def run_one(f, pat):
    """A strategy that leaves i1 unread at a step does not depend on it there,
    so the formula must hold on the trace for either value of that read."""
    t = tau_str(f)
    rec = {'f': f, 'txt': t, 'pat': pat, 'bad': []}
    script = f'run {RUN_STEPS} steps {t}\n' + ''.join(f'{x}\n' for x in inputs(pat)) + 'quit\n'
    try:
        r = subprocess.run([TAU, '-X', '--color', 'false', '--benchmarks', 'false', '--ltl-timeout', '30'],
                           input=script, capture_output=True, text=True, timeout=TIMEOUT_SEC,
                           preexec_fn=lim)
    except subprocess.TimeoutExpired:
        rec['bad'].append(['timeout'])
        return rec
    o = re.sub(r'\x1b\[[0-9;]*m', '', r.stdout + r.stderr)
    reads = {int(m[1]): m[2] for m in re.finditer(r'i1\[(\d+)\] : bv\[1\] := (\S+)', o)}
    tr = {}
    for m in re.finditer(r'(o[12])\[(\d+)\] := (\S+)', o):
        tr.setdefault(int(m[2]), {})[m[1]] = m[3]
    uses = {x for x in ('a', 'b') if json.dumps(x) in json.dumps(f)}
    if (max(tr) if tr else -1) < RUN_STEPS - 1:
        if uses:
            rec['bad'].append(['short', o[-260:]])
        return rec
    for fill in (False, True):
        w = [{'a': tr.get(i, {}).get('o1') == '1', 'b': tr.get(i, {}).get('o2') == '1',
              'c': reads[i] == '1' if i in reads else fill} for i in range(RUN_STEPS)]
        tup = [(x['a'], x['b'], x['c']) for x in w]
        ls = lasso(tup)
        if not ls:
            rec['bad'].append(['no-lasso', ''.join(f'{int(a)}{int(b)}' for a, b, _ in tup)])
            return rec
        s, q = ls
        if not ev(invariant_reading(f), w[:s + q] + w[s:s + q] * 6, q)[0]:
            rec['bad'].append(['violated', s, q, ''.join(f'{int(a)}{int(b)}{int(c)}' for a, b, c in tup[:16])])
            return rec
    return rec


def runnable(rec):
    """A valid formula binds no output, so its run stops at the first step."""
    return rec['scoped'] and rec['tau'].get('realizable') == 'T' and not rec.get('spot_valid')


def formulas(mode):
    if mode == 'n1':
        return list(enum(1, ATOMS, UN + ['!'], BT + BB))
    if mode == 'n2':
        return [f for f in enum(2, ATOMS, UN + ['!'], BT + BB) if canonical(f)]
    assert mode == 'n3t'
    return [f for f in enum(3, ATOMS, UN, BT) if canonical(f)]


def main():
    global TAU
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--tau', required=True)
    ap.add_argument('--mode', nargs='+', default=['n1', 'n2'], choices=['n1', 'n2', 'n3t'])
    ap.add_argument('--jobs', type=int, default=4)
    ap.add_argument('--out', help='keep one JSON record per formula, and resume from it')
    ap.add_argument('--run', type=int, default=0, metavar='N',
                    help='also run N sampled realizable formulas on every input pattern')
    ap.add_argument('--spot-bin', default=os.environ.get('TAU_SPOT_BIN'))
    a = ap.parse_args()
    TAU = os.path.abspath(a.tau)
    if a.spot_bin:
        os.environ['PATH'] = a.spot_bin + os.pathsep + os.environ['PATH']
    if not shutil.which('ltlsynt'):
        sys.exit('ltl_sweep: no ltlsynt on PATH')
    recs = {}
    if a.out and os.path.exists(a.out):
        with open(a.out) as r:
            for line in r:
                x = json.loads(line)
                recs[json.dumps(x['f'])] = x
    fs = [f for m in a.mode for f in formulas(m)]
    todo = [f for f in fs if json.dumps(f) not in recs]
    print(f'{len(fs)} formulas, {len(todo)} to check', flush=True)
    sink = open(a.out, 'a') if a.out else None
    with ThreadPoolExecutor(a.jobs) as ex:
        for i, fu in enumerate(as_completed([ex.submit(check, f) for f in todo]), 1):
            x = fu.result()
            recs[json.dumps(x['f'])] = x
            if sink:
                sink.write(json.dumps(x) + '\n')
                sink.flush()
            if i % 1000 == 0:
                print(i, flush=True)
        runs = []
        if a.run:
            good = sorted(json.dumps(f) for f in fs
                          if runnable(recs[json.dumps(f)]))
            random.Random(7).shuffle(good)
            futs = [ex.submit(run_one, json.loads(k), p) for k in good[:a.run] for p in PATTERNS]
            runs = [fu.result() for fu in as_completed(futs)]
    if sink:
        sink.close()
    defects = [(x['txt'], b) for x in recs.values() for b in x['bad']]
    defects += [(x['txt'] + ' on ' + x['pat'], b) for x in runs for b in x['bad']]
    kinds = collections.Counter(b[0] for _, b in defects)
    print(f'{len(defects)} defects', dict(kinds))
    for t, b in sorted(defects, key=lambda d: len(d[0]))[:30]:
        print(t, str(b)[:300])
    return 1 if defects else 0


if __name__ == '__main__':
    sys.exit(main())
