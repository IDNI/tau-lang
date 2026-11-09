#!/usr/bin/env python3
"""Compare a benchmark record with the last records of its series.

The baseline of a test is the median of its value in the `baseline_records`
records of the series before the commit (scripts/ci/benchmark-thresholds.json):
the records are ordered by commit date, so a commit measured after its
successors is still compared with what came before it, and an earlier record
of the same commit is not part of its baseline.

The runners differ in CPU, and a slow model shifts every time of a record by
the same share. The times are therefore normalized by CPU model: each model
gets a factor, the median over its records of the record's CPU time divided
by the CPU time of the neighbouring records on the reference model (the most
frequent one), learnt from the records of the series with at least
`normalize_by_cpu.min_records` of the model; a record's times are divided by
its model's factor before the comparison. Memory is not scaled.
A test is flagged at warning level when its value is over the baseline by
the `warn` share and by the floor of its unit, and as a failure candidate
when over by the `fail` share; a candidate becomes a failure when the
record before it flagged the same test as a candidate too (`fail_streak`
consecutive records). The totals of the series follow the `totals`
thresholds. A failing test, in the record or in a baseline, is not compared;
an overload point killed at its cap is, with the cap as its time.

Outputs: a markdown summary (--summary), the flagged part of it for a
commit comment (--comment, written only when something is flagged), the
flags with their streaks (--flags, stored with the record so the next
comparison sees them) and GitHub annotations on stdout (--annotations).
The exit status is 2 at failure level and 0 otherwise.

Usage:
  benchmark-compare.py --current record.json --data data/<series> \
      --summary summary.md --comment comment.md --flags flags.json --annotations
"""

import argparse
import json
import statistics
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent

# group -> (key of a row, time of a row, unit of the time, memory of a row)
GROUPS = {
    "ctest": (lambda r: r[0], lambda r: r[1], "s", None),
    "process": (lambda r: r[0], lambda r: r[2] + r[3], "s", lambda r: r[4]),
    "case": (lambda r: f"{r[0]} / {r[1]} / {r[2]}", lambda r: r[3], "ms",
             lambda r: r[4]),
    "testnet": (lambda r: r[0], lambda r: r[1], "s", None),
    # a step row of an overload point carries no memory of its own (0)
    "overload": (lambda r: r[0], lambda r: r[1], "s", lambda r: r[2] or None),
}
# groups whose rows carry a status, at this index
STATUS_INDEX = {"ctest": 2, "testnet": 2, "overload": 3}
# the statuses whose rows are compared: an overload point killed at its cap
# keeps the cap as its time, a lower bound that still shows a regression
COMPARED_STATUS = ("pass", "cap")
# totals compared, with their unit
TOTAL_UNITS = {
    "ctest_wall_s": "s", "case_ms": "ms", "process_cpu_s": "s",
    "process_rss_max_kb": "kb", "pytest_s": "s", "run_wall_s": "s",
    "run_cpu_s": "s", "run_rss_max_kb": "kb", "overload_s": "s",
}
ANNOTATION_LIMIT = 10
TOP = 20


def load_thresholds(path):
    return json.loads(Path(path).read_text())


def order_key(record: dict) -> tuple:
    """The order of a record or index entry: by commit, then by run."""
    return (record.get("commit_date") or record.get("date") or "",
            record.get("date") or "")


def load_index(data_dir: Path) -> list:
    """The index entries of the series, newest commit first."""
    index = data_dir / "index.json"
    if not index.is_file():
        return []
    entries = json.loads(index.read_text()).get("records", [])
    entries.sort(key=order_key, reverse=True)
    return entries


def cpu_of(record_or_entry: dict) -> str:
    return (record_or_entry.get("host") or {}).get("cpu") or "unknown"


def short_cpu(name: str) -> str:
    """'AMD EPYC 9V45 96-Core Processor' -> 'AMD EPYC 9V45 96-Core'."""
    for noise in (" Processor", "(R)", "(TM)", " CPU"):
        name = name.replace(noise, "")
    return " ".join(name.split())


def proxy_of(entry: dict):
    """The CPU time of a record, the speed proxy of its runner."""
    t = entry.get("totals") or {}
    for key in ("process_cpu_s", "run_cpu_s"):
        if t.get(key):
            return float(t[key])
    return None


def cpu_factors(entries: list, min_records: int, neighbours: int) -> dict:
    """{cpu: factor} over the entries, 1 for the reference model and for a
    model with fewer than `min_records` records, plus the reference under
    the key ""; `entries` are in commit order, newest first.

    The factor of a model is the median over its records of the record's
    proxy divided by the median proxy of the nearest `neighbours` reference
    records on each side, so a drift of the code over the window cancels.
    """
    rows = [(i, cpu_of(e), proxy_of(e)) for i, e in enumerate(entries)]
    rows = [r for r in rows if r[2]]
    if not rows:
        return {"": "unknown"}
    counts = {}
    for _, cpu, _ in rows:
        counts[cpu] = counts.get(cpu, 0) + 1
    reference = max(sorted(counts), key=counts.get)
    ref_rows = [r for r in rows if r[1] == reference]
    factors = {"": reference, reference: 1.0}
    for cpu, n in counts.items():
        if cpu == reference or n < min_records:
            continue
        ratios = []
        for i, _, proxy in (r for r in rows if r[1] == cpu):
            near = sorted(ref_rows, key=lambda r: abs(r[0] - i))[:2 * neighbours]
            ratios.append(proxy / statistics.median(r[2] for r in near))
        factors[cpu] = round(statistics.median(ratios), 4)
    return factors


def load_baselines(entries: list, data_dir: Path, count: int, current: dict) -> list:
    """The `count` records of the series before `current`, newest first."""
    entries = [e for e in entries
               if e.get("sha") != current.get("sha")
               and order_key(e) < order_key(current)]
    entries = entries[:count]
    records = []
    for e in entries:
        file = data_dir / e["file"]
        if file.is_file():
            records.append(json.loads(file.read_text()))
    return records


def rows_of(record: dict, group: str, scale: float = 1.0) -> dict:
    """key -> (time, memory or None) of the passing rows of a group, the
    time multiplied by `scale`."""
    key_of, time_of, _unit, mem_of = GROUPS[group]
    status = STATUS_INDEX.get(group)
    out = {}
    for r in record.get(group, []):
        if status is not None and r[status] not in COMPARED_STATUS:
            continue
        mem = mem_of(r) if mem_of else None
        out[key_of(r)] = (float(time_of(r)) * scale, int(mem) if mem is not None else None)
    return out


def fmt(value, unit) -> str:
    if value is None:
        return "—"
    if unit == "kb":
        return f"{value / 1024:.1f} MB"
    if unit == "ms":
        return f"{value:.1f} ms"
    return f"{value:.2f} s"


def pct(cur, base) -> str:
    if not base:
        return "—"
    d = (cur - base) / base * 100
    return f"{d:+.0f} %"


class Comparison:
    def __init__(self, current, baselines, thresholds, previous_flags, factors=None):
        self.current = current
        self.baselines = baselines  # newest first
        self.th = thresholds
        self.previous = previous_flags  # key -> flag of the record before
        self.factors = factors or {"": cpu_of(current)}  # cpu -> factor
        self.flags = {}
        self.rows = []  # every compared row, for the top lists

    def scale(self, record) -> float:
        """What the times of a record are multiplied by: 1 / its CPU's factor."""
        return 1.0 / self.factors.get(cpu_of(record), 1.0)

    def floor(self, metric, unit):
        if metric == "memory":
            return self.th["memory"]["floor_kb"]
        return self.th["time"]["floor_ms" if unit == "ms" else "floor_s"]

    def judge(self, key, metric, unit, cur, history, warn, fail, floor):
        """Flags one value against the median of its history."""
        base = statistics.median(history)
        self.rows.append((key, metric, unit, cur, base, history))
        over = cur - base
        if over <= floor:
            return
        candidate = None
        if cur > base * (1 + fail):
            candidate = "fail"
        elif cur > base * (1 + warn):
            candidate = "warn"
        if candidate is None:
            return
        # streak: consecutive records with the test flagged; fails: those at
        # failure candidate level, which the record before must have
        # reached too for the failure to count
        prev = self.previous.get(key, {})
        streak = prev.get("streak", 0) + 1
        fails = prev.get("fails", 0) + 1 if candidate == "fail" else 0
        level = "fail" if fails >= self.th["fail_streak"] else "warn"
        self.flags[key] = {
            "level": level, "candidate": candidate, "streak": streak,
            "fails": fails, "metric": metric, "unit": unit, "current": cur,
            "baseline": base, "history": history,
        }

    def run(self):
        t_warn, t_fail = self.th["time"]["warn"], self.th["time"]["fail"]
        m_warn, m_fail = self.th["memory"]["warn"], self.th["memory"]["fail"]
        for group in GROUPS:
            if group not in self.current:
                continue
            cur_rows = rows_of(self.current, group, self.scale(self.current))
            base_rows = [rows_of(b, group, self.scale(b)) for b in self.baselines]
            unit = GROUPS[group][2]
            for key, (time, mem) in cur_rows.items():
                times = [b[key][0] for b in base_rows if key in b]
                if times:
                    self.judge(f"{group}|time|{key}", "time", unit, time,
                               times, t_warn, t_fail, self.floor("time", unit))
                if mem is not None:
                    mems = [b[key][1] for b in base_rows
                            if key in b and b[key][1] is not None]
                    if mems:
                        self.judge(f"{group}|memory|{key}", "memory", "kb",
                                   mem, mems, m_warn, m_fail,
                                   self.floor("memory", "kb"))
        totals = self.current.get("totals", {})
        for name, unit in TOTAL_UNITS.items():
            if name not in totals:
                continue
            # a time total follows the CPU factor, a memory total does not
            scaled = (lambda r, v: v * self.scale(r)) if unit != "kb" else (lambda r, v: v)
            history = [scaled(b, b["totals"][name]) for b in self.baselines
                       if b.get("totals", {}).get(name) is not None]
            if not history or not totals[name]:
                continue
            self.judge(f"totals|{unit}|{name}", "totals", unit,
                       scaled(self.current, totals[name]), history,
                       self.th["totals"]["warn"], self.th["totals"]["fail"], 0)

    def level(self) -> str:
        levels = {f["level"] for f in self.flags.values()}
        if "fail" in levels:
            return "fail"
        if "warn" in levels:
            return "warn"
        return "none"


def history_columns(baselines) -> list:
    """Column headers of the last values, oldest first."""
    return [b.get("sha", "")[:8] or "?" for b in reversed(baselines)]


def history_cells(history, baselines, unit) -> list:
    """The values of a key in the baseline records, oldest first.

    `history` holds the values of the records that have the key, newest
    first; a record without it (a new test) shows a dash at the left.
    """
    cells = [fmt(v, unit) for v in reversed(history)]
    return ["—"] * (len(baselines) - len(cells)) + cells


def flagged_table(cmp: Comparison, metric_filter) -> str:
    rows = [(k, f) for k, f in cmp.flags.items() if metric_filter(f)]
    if not rows:
        return ""
    rows.sort(key=lambda kf: (kf[1]["level"] != "fail",
                              -(kf[1]["current"] / kf[1]["baseline"])))
    cols = history_columns(cmp.baselines)
    out = ["| level | test | " + " | ".join(cols) + " | current | Δ |",
           "|---|---|" + "---:|" * len(cols) + "---:|---:|"]
    for key, f in rows:
        group, _metric, name = key.split("|", 2)
        label = name if group == "totals" else f"`{name}` ({group})"
        level = f["level"]
        if f["candidate"] == "fail" and level == "warn":
            level = "warn (1st over fail)"
        cells = history_cells(f["history"], cmp.baselines, f["unit"])
        out.append(f"| {level} | {label} | " + " | ".join(cells)
                   + f" | **{fmt(f['current'], f['unit'])}** | "
                   f"{pct(f['current'], f['baseline'])} |")
    return "\n".join(out) + "\n"


def top_table(cmp: Comparison, group, metric, unit, title) -> str:
    rows = [r for r in cmp.rows if r[0].startswith(f"{group}|{metric}|")]
    if not rows:
        return ""
    rows.sort(key=lambda r: -r[3])
    cols = history_columns(cmp.baselines)
    out = [f"### {title}", "",
           "| test | " + " | ".join(cols) + " | current | Δ |",
           "|---|" + "---:|" * len(cols) + "---:|---:|"]
    for key, _m, _u, cur, base, history in rows[:TOP]:
        name = key.split("|", 2)[2]
        cells = history_cells(history, cmp.baselines, unit)
        out.append(f"| `{name}` | " + " | ".join(cells)
                   + f" | {fmt(cur, unit)} | {pct(cur, base)} |")
    return "\n".join(out) + "\n\n"


def totals_table(cmp: Comparison) -> str:
    rows = [r for r in cmp.rows if r[0].startswith("totals|")]
    if not rows:
        return ""
    out = ["| total | baseline (median) | current | Δ |", "|---|---:|---:|---:|"]
    for key, _m, unit, cur, base, _h in rows:
        name = key.split("|", 2)[2]
        flag = cmp.flags.get(key)
        mark = f" **{flag['level']}**" if flag else ""
        out.append(f"| {name} | {fmt(base, unit)} | {fmt(cur, unit)} | "
                   f"{pct(cur, base)}{mark} |")
    return "\n".join(out) + "\n\n"


def header(cmp: Comparison, dashboard_url) -> str:
    c = cmp.current
    host = c.get("host", {})
    link = f" · [dashboard]({dashboard_url})" if dashboard_url else ""
    lines = [f"## Benchmarks · {c.get('series')} · `{c.get('sha', '')[:8]}` "
             f"({c.get('branch', '')}){link}", ""]
    t = c.get("totals", {})
    lines.append(f"{t.get('tests', 0)} tests, {t.get('failed', 0)} failed · "
                 f"{host.get('cpu', '?')} · {host.get('os', '?')} · "
                 f"-j {host.get('jobs', '?')}")
    reference = cmp.factors.get("", "")
    factor = cmp.factors.get(cpu_of(c), 1.0)
    models = {k: v for k, v in cmp.factors.items() if k}
    if len(models) > 1:
        others = ", ".join(f"{short_cpu(k)} ×{v:.3f}" for k, v in sorted(models.items()) if k != reference)
        lines.append(f"Times normalized by CPU: this record ×1/{factor:.3f}; "
                     f"reference {short_cpu(reference)}, factors {others}.")
    if cmp.baselines:
        shas = ", ".join(f"`{b.get('sha', '')[:8]}`" for b in cmp.baselines)
        lines.append(f"Baseline: median of the {len(cmp.baselines)} "
                     f"records before this commit ({shas}).")
    else:
        lines.append("No baseline yet: no record of the series before this commit.")
    return "\n".join(lines) + "\n\n"


def summary_markdown(cmp: Comparison, dashboard_url) -> str:
    out = header(cmp, dashboard_url)
    out += totals_table(cmp)
    flagged = flagged_table(cmp, lambda f: f["metric"] != "totals")
    if flagged:
        out += f"### Over the thresholds ({cmp.level()})\n\n" + flagged + "\n"
    elif cmp.baselines:
        out += "No test over the thresholds.\n\n"
    for group, metric, unit, title in (
            ("ctest", "time", "s", "Slowest ctest entries (wall)"),
            ("process", "time", "s", "Slowest processes (user+sys)"),
            ("case", "time", "ms", "Slowest doctest cases"),
            ("testnet", "time", "s", "Slowest tau-testnet tests"),
            ("overload", "time", "s", "Slowest tau-overload points"),
            ("overload", "memory", "kb", "Heaviest tau-overload points (peak RSS)"),
            ("process", "memory", "kb", "Heaviest processes (peak RSS)"),
            ("case", "memory", "kb", "Heaviest doctest cases (peak RSS)")):
        out += top_table(cmp, group, metric, unit, title)
    return out


def comment_markdown(cmp: Comparison, dashboard_url) -> str:
    if not cmp.flags:
        return ""
    out = header(cmp, dashboard_url)
    out += totals_table(cmp)
    flagged = flagged_table(cmp, lambda f: f["metric"] != "totals")
    if flagged:
        out += f"### Over the thresholds ({cmp.level()})\n\n" + flagged
    return out


def annotations(cmp: Comparison) -> list:
    lines = []
    series = cmp.current.get("series", "")
    for level in ("fail", "warn"):
        rows = sorted((kf for kf in cmp.flags.items() if kf[1]["level"] == level),
                      key=lambda kf: -(kf[1]["current"] / kf[1]["baseline"]))
        kind = "error" if level == "fail" else "warning"
        for key, f in rows[:ANNOTATION_LIMIT]:
            group, metric, name = key.split("|", 2)
            lines.append(
                f"::{kind} title=Benchmark {series}::{name} ({group} {metric}) "
                f"{fmt(f['baseline'], f['unit'])} -> {fmt(f['current'], f['unit'])} "
                f"({pct(f['current'], f['baseline'])}, record {f['streak']} over)")
    return lines


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--current", required=True, type=Path)
    ap.add_argument("--data", required=True, type=Path,
                    help="the series folder of the benchmarks-data branch")
    ap.add_argument("--thresholds", type=Path,
                    default=HERE / "benchmark-thresholds.json")
    ap.add_argument("--summary", type=Path)
    ap.add_argument("--comment", type=Path)
    ap.add_argument("--flags", type=Path)
    ap.add_argument("--annotations", action="store_true")
    ap.add_argument("--dashboard-url", default="")
    args = ap.parse_args()

    th = load_thresholds(args.thresholds)
    current = json.loads(args.current.read_text())
    entries = load_index(args.data)
    baselines = load_baselines(entries, args.data, th["baseline_records"], current)
    previous = baselines[0].get("flags", {}) if baselines else {}
    norm = th.get("normalize_by_cpu") or {}
    # the factors come from every record of the series but this commit's own
    factors = cpu_factors([e for e in entries if e.get("sha") != current.get("sha")],
                          norm.get("min_records", 2), norm.get("neighbours", 3)) \
        if norm else {"": cpu_of(current)}
    cmp = Comparison(current, baselines, th, previous, factors)
    cmp.run()

    if args.summary:
        args.summary.write_text(summary_markdown(cmp, args.dashboard_url))
    if args.comment:
        text = comment_markdown(cmp, args.dashboard_url)
        if text:
            args.comment.write_text(text)
        elif args.comment.exists():
            args.comment.unlink()
    if args.flags:
        args.flags.write_text(json.dumps({
            "series": current.get("series"), "sha": current.get("sha"),
            "level": cmp.level(),
            "baselines": [b.get("sha") for b in baselines],
            "cpu": {"name": cpu_of(current), "factor": factors.get(cpu_of(current), 1.0),
                    "reference": factors.get("", ""),
                    "factors": {k: v for k, v in factors.items() if k}},
            "flags": cmp.flags}, indent=1) + "\n")
    if args.annotations:
        for line in annotations(cmp):
            print(line)
    n_fail = sum(1 for f in cmp.flags.values() if f["level"] == "fail")
    n_warn = sum(1 for f in cmp.flags.values() if f["level"] == "warn")
    print(f"benchmark-compare: {current.get('series')} {current.get('sha', '')[:8]}: "
          f"{len(baselines)} baseline records, {n_warn} warnings, {n_fail} failures")
    return 2 if cmp.level() == "fail" else 0


if __name__ == "__main__":
    sys.exit(main())
