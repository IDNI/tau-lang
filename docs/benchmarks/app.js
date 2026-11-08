// The benchmarks dashboard: reads the records of the benchmarks-data branch
// (scripts/ci/benchmark-store.py writes them) and draws them with Chart.js.
// Every list and chart follows the order of the commits (the commit date of
// each record), not the order the benchmarks ran in.
// ?data=<url> points it at another data folder, such as a local one.
(() => {
"use strict";

const params = new URLSearchParams(location.search);
const DATA = (params.get("data") ||
  "https://raw.githubusercontent.com/IDNI/tau-lang/benchmarks-data/data").replace(/\/$/, "");
const COMMIT = "https://github.com/IDNI/tau-lang/commit/";
const BASELINE_RECORDS = 5;
const WARN = { time: 0.25, memory: 0.20 };
const FALLBACK_SERIES = ["linux-x86_64-release", "testnet"];
const GROUP_ORDER = ["unit", "integration", "repl", "ltl", "api", "codegen", "python", "sdk", "js", "testnet"];

// the rows of a record: key, time, unit of the time, memory
const ROWS = {
  ctest: { key: r => r[0], time: r => r[1], unit: "s", mem: null, status: r => r[2] },
  process: { key: r => r[0], time: r => r[2] + r[3], unit: "s", mem: r => r[4], status: null },
  case: { key: r => `${r[0]} / ${r[1]} / ${r[2]}`, time: r => r[3], unit: "ms", mem: r => r[4], status: null },
  testnet: { key: r => r[0], time: r => r[1], unit: "s", mem: null, status: r => r[2] },
};

const $ = id => document.getElementById(id);
const css = name => getComputedStyle(document.documentElement).getPropertyValue(name).trim();
const seriesColor = i => css(`--series-${(i % 8) + 1}`);

const state = { series: "", index: null, history: null, records: new Map(), view: "overview", charts: {} };

// ---- formatting ---------------------------------------------------------
const fmt = {
  s: v => `${v.toFixed(2)} s`,
  ms: v => `${v.toFixed(1)} ms`,
  kb: v => `${(v / 1024).toFixed(1)} MB`,
  pct: (cur, base) => base ? `${(cur - base) / base * 100 >= 0 ? "+" : ""}${((cur - base) / base * 100).toFixed(0)} %` : "—",
  date: d => (d || "").replace("T", " ").replace(/:\d\dZ$/, "Z"),
  when: e => fmt.date(when(e)),
  sha: s => (s || "").slice(0, 8),
  n: v => v.toLocaleString(),
};
function deltaCell(cur, base) {
  if (!base) return `<td class="num">—</td>`;
  const d = (cur - base) / base;
  const cls = d > 0.05 ? "up" : d < -0.05 ? "down" : "";
  return `<td class="num ${cls}">${fmt.pct(cur, base)}</td>`;
}
// the date a record is ordered and labelled by: the commit's, else the run's
const when = e => e.commit_date || e.date || "";
const byCommit = (a, b) => when(a).localeCompare(when(b)) || (a.date || "").localeCompare(b.date || "");
const esc = s => String(s).replace(/[&<>"]/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
const median = a => { const s = [...a].sort((x, y) => x - y); const m = s.length >> 1; return s.length % 2 ? s[m] : (s[m - 1] + s[m]) / 2; };

// ---- loading ------------------------------------------------------------
async function getJSON(url, optional) {
  const r = await fetch(url, { cache: "no-cache" });
  if (!r.ok) { if (optional) return null; throw new Error(`${url}: ${r.status}`); }
  return r.json();
}
function status(text) { $("status").textContent = text; }

async function loadSeriesList() {
  const list = await getJSON(`${DATA}/series.json`, true);
  const names = list && list.series.length ? list.series.map(s => s.name) : FALLBACK_SERIES;
  const sel = $("series");
  sel.innerHTML = names.map(n => `<option>${esc(n)}</option>`).join("");
  const wanted = hashParams().get("series");
  sel.value = names.includes(wanted) ? wanted : names[0];
  state.series = sel.value;
}
async function loadIndex() {
  status(`loading ${state.series}…`);
  state.records.clear();
  state.index = await getJSON(`${DATA}/${state.series}/index.json`, true) || { records: [] };
  state.history = await getJSON(`${DATA}/${state.series}/history.json`, true) || { records: [] };
  state.index.records.sort((a, b) => byCommit(b, a));
  state.history.records.sort(byCommit);
  status(state.index.records.length ? "" : `no record of ${state.series} yet`);
}
async function loadRecord(entry) {
  if (!state.records.has(entry.file))
    state.records.set(entry.file, getJSON(`${DATA}/${state.series}/${entry.file}`));
  return state.records.get(entry.file);
}
// index entries oldest first, history first
const timeline = () => [...state.history.records, ...[...state.index.records].reverse()];
const kind = () => state.series.startsWith("testnet") ? "pytest" : "ctest";
const totalTimeKey = () => kind() === "pytest" ? "pytest_s" : "ctest_wall_s";
const totalMemKey = () => kind() === "pytest" ? "run_rss_max_kb" : "process_rss_max_kb";

// ---- hash routing -------------------------------------------------------
function hashParams() { return new URLSearchParams(location.hash.slice(1)); }
function setHash(changes) {
  const h = hashParams();
  for (const [k, v] of Object.entries(changes)) v ? h.set(k, v) : h.delete(k);
  history.replaceState(null, "", "#" + h.toString());
}

// ---- charts -------------------------------------------------------------
function chart(id, config) {
  if (state.charts[id]) state.charts[id].destroy();
  const ink2 = css("--ink-2"), grid = css("--grid"), axis = css("--axis");
  Chart.defaults.color = ink2;
  Chart.defaults.font.family = getComputedStyle(document.body).fontFamily;
  config.options = Object.assign({
    responsive: true, maintainAspectRatio: false, animation: false,
    interaction: { mode: "index", intersect: false },
    plugins: Object.assign({ legend: { display: false } }, config.options?.plugins || {}),
    scales: {
      x: Object.assign({ grid: { display: false }, border: { color: axis }, ticks: { maxRotation: 0, autoSkip: true, maxTicksLimit: 12 } }, config.options?.scales?.x || {}),
      y: Object.assign({ beginAtZero: true, grid: { color: grid }, border: { display: false } }, config.options?.scales?.y || {}),
    },
  }, config.options || {}, { plugins: Object.assign({ legend: { display: false } }, config.options?.plugins || {}) });
  state.charts[id] = new Chart($(id), config);
  return state.charts[id];
}
function lineDataset(label, data, color, extra) {
  return Object.assign({ label, data, borderColor: color, backgroundColor: color, borderWidth: 2,
    pointRadius: 4, pointHoverRadius: 6, pointBorderColor: css("--surface"), pointBorderWidth: 1, tension: 0 }, extra || {});
}
function openRun(entries) {
  return (evt, items) => { const e = items[0] && entries[items[0].index]; if (e && e.run_url) window.open(e.run_url, "_blank"); };
}
const entryTitle = entries => items => { const e = entries[items[0].dataIndex]; return `${fmt.sha(e.sha)} · ${fmt.when(e)}${e.branch ? " · " + e.branch : ""}`; };
const entryFooter = entries => items => { const e = entries[items[0].dataIndex]; return e.host ? `${e.host.cpu || ""} · -j ${e.host.jobs || "?"}` : ""; };

// ---- overview -----------------------------------------------------------
function renderOverview() {
  const entries = timeline();
  const latest = state.index.records[0];
  const tiles = $("tiles");
  if (!latest) { tiles.innerHTML = ""; $("table-records").innerHTML = ""; return; }
  const t = latest.totals || {};
  tiles.innerHTML = [
    ["latest commit", `<a href="${COMMIT}${esc(latest.sha)}">${fmt.sha(latest.sha)}</a>`, fmt.when(latest)],
    ["tests", fmt.n(t.tests || 0), `${t.failed || 0} failed`],
    ["total time", fmt.s(t[totalTimeKey()] || 0), kind() === "pytest" ? "sum of the pytest tests" : "sum of the ctest entries"],
    ["peak memory", fmt.kb(t[totalMemKey()] || 0), "largest process of the run"],
    ["level", `<span class="level ${esc(latest.level || "none")}">${esc(latest.level || "none")}</span>`, `${state.index.records.length} records in the window, ${state.history.records.length} in the history`],
  ].map(([k, v, s]) => `<div class="tile"><div class="k">${k}</div><div class="v">${v}</div><div class="s">${s}</div></div>`).join("");

  const labels = entries.map(e => fmt.sha(e.sha));
  $("overview-time-unit").textContent = kind() === "pytest" ? "(s, sum of the pytest tests)" : "(s, sum of the ctest entries)";
  chart("chart-total-time", { type: "line",
    data: { labels, datasets: [lineDataset("total time", entries.map(e => (e.totals || {})[totalTimeKey()] || null), seriesColor(0))] },
    options: { onClick: openRun(entries), plugins: { tooltip: { callbacks: { title: entryTitle(entries), label: c => ` ${fmt.s(c.parsed.y)}`, footer: entryFooter(entries) } } } } });
  chart("chart-total-memory", { type: "line",
    data: { labels, datasets: [lineDataset("peak memory", entries.map(e => ((e.totals || {})[totalMemKey()] || 0) / 1024), seriesColor(1))] },
    options: { onClick: openRun(entries), plugins: { tooltip: { callbacks: { title: entryTitle(entries), label: c => ` ${c.parsed.y.toFixed(1)} MB`, footer: entryFooter(entries) } } } } });

  const groups = GROUP_ORDER.filter(g => entries.some(e => e.groups && e.groups[g]));
  $("fig-groups").classList.toggle("hidden", groups.length < 2);
  if (groups.length >= 2) {
    chart("chart-groups", { type: "line",
      data: { labels, datasets: groups.map((g, i) => lineDataset(g, entries.map(e => e.groups && e.groups[g] ? e.groups[g].wall_s : null), seriesColor(i))) },
      options: { onClick: openRun(entries), plugins: { tooltip: { callbacks: { title: entryTitle(entries), label: c => ` ${c.dataset.label}: ${fmt.s(c.parsed.y)}` } } } } });
    $("legend-groups").innerHTML = groups.map((g, i) => `<span style="--c:${seriesColor(i)}">${esc(g)}</span>`).join("");
  }

  const rows = state.index.records.slice(0, 15).map(e => `<tr>
    <td>${fmt.when(e)}</td><td><a href="${COMMIT}${esc(e.sha)}">${fmt.sha(e.sha)}</a></td><td>${esc(e.branch || "")}</td>
    <td class="num">${fmt.n((e.totals || {}).tests || 0)}</td><td class="num">${(e.totals || {}).failed || 0}</td>
    <td class="num">${fmt.s((e.totals || {})[totalTimeKey()] || 0)}</td><td class="num">${fmt.kb((e.totals || {})[totalMemKey()] || 0)}</td>
    <td><span class="level ${esc(e.level || "none")}">${esc(e.level || "none")}</span></td>
    <td>${e.run_url ? `<a href="${esc(e.run_url)}">run</a>` : ""}</td></tr>`);
  $("table-records").innerHTML = `<tr><th>commit date</th><th>commit</th><th>branch</th><th class="num">tests</th><th class="num">failed</th><th class="num">total time</th><th class="num">peak memory</th><th>level</th><th>run</th></tr>${rows.join("")}`;
}

// ---- test ---------------------------------------------------------------
async function fillTestNames() {
  const latest = state.index.records[0];
  if (!latest) { $("test-names").innerHTML = ""; return; }
  const rec = await loadRecord(latest);
  const names = [];
  for (const [group, spec] of Object.entries(ROWS))
    for (const r of rec[group] || []) names.push(`${group}: ${spec.key(r)}`);
  $("test-names").innerHTML = names.slice(0, 20000).map(n => `<option value="${esc(n)}">`).join("");
  $("test-hint").textContent = `${fmt.n(names.length)} tests in the latest record; pick one, as "group: name".`;
}
function parseTestName(text) {
  const m = /^(ctest|process|case|testnet): (.+)$/.exec(text || "");
  return m ? { group: m[1], name: m[2] } : null;
}
async function renderTest() {
  const sel = parseTestName($("test-search").value);
  const figs = [$("fig-test-time"), $("fig-test-memory")];
  if (!sel) { figs.forEach(f => f.classList.add("hidden")); $("table-test").innerHTML = ""; return; }
  const depth = +$("test-depth").value;
  const entries = state.index.records.slice(0, depth).reverse();
  status(`loading ${entries.length} records…`);
  const recs = await Promise.all(entries.map(loadRecord));
  status("");
  const spec = ROWS[sel.group];
  const points = recs.map(rec => {
    const row = (rec[sel.group] || []).find(r => spec.key(r) === sel.name);
    if (!row || (spec.status && spec.status(row) !== "pass")) return { time: null, mem: null };
    return { time: spec.time(row), mem: spec.mem ? spec.mem(row) : null };
  });
  const labels = entries.map(e => fmt.sha(e.sha));
  const band = (values, warn) => values.map((v, i) => {
    const prev = values.slice(Math.max(0, i - BASELINE_RECORDS), i).filter(x => x !== null);
    if (!prev.length) return [null, null];
    const m = median(prev); return [m, m * (1 + warn)];
  });
  const draw = (id, values, unit, warn, fig, cap, caption) => {
    const has = values.some(v => v !== null);
    fig.classList.toggle("hidden", !has);
    if (!has) return;
    $(cap).innerHTML = `${caption} <span class="unit">(${unit === "kb" ? "MB" : unit}; the band is the median of the previous ${BASELINE_RECORDS} records up to the warning threshold)</span>`;
    const scaled = v => v === null ? null : unit === "kb" ? v / 1024 : v;
    const b = band(values, warn);
    chart(id, { type: "line",
      data: { labels, datasets: [
        lineDataset("baseline", b.map(x => scaled(x[0])), css("--muted"), { pointRadius: 0, borderWidth: 1, borderDash: [4, 4] }),
        lineDataset("warning threshold", b.map(x => scaled(x[1])), css("--muted"), { pointRadius: 0, borderWidth: 0, fill: "-1", backgroundColor: css("--band") }),
        lineDataset(sel.name, values.map(scaled), seriesColor(unit === "kb" ? 1 : 0)),
      ] },
      options: { onClick: openRun(entries), plugins: { tooltip: { filter: i => i.datasetIndex === 2, callbacks: { title: entryTitle(entries), label: c => ` ${unit === "kb" ? c.parsed.y.toFixed(1) + " MB" : fmt[unit](c.parsed.y)}`, footer: entryFooter(entries) } } } } });
  };
  draw("chart-test-time", points.map(p => p.time), spec.unit, WARN.time, figs[0], "cap-test-time", `Time of ${esc(sel.name)}`);
  draw("chart-test-memory", points.map(p => p.mem), "kb", WARN.memory, figs[1], "cap-test-memory", `Peak memory of ${esc(sel.name)}`);
  const last = entries.map((e, i) => ({ e, p: points[i] })).slice(-6).reverse();
  const hasMem = points.some(p => p.mem !== null);
  $("table-test").innerHTML = `<tr><th>commit date</th><th>commit</th><th class="num">time</th>${hasMem ? '<th class="num">peak memory</th>' : ""}<th>run</th></tr>` +
    last.map(({ e, p }) => `<tr><td>${fmt.when(e)}</td><td><a href="${COMMIT}${esc(e.sha)}">${fmt.sha(e.sha)}</a></td>
      <td class="num">${p.time === null ? "—" : fmt[spec.unit](p.time)}</td>${hasMem ? `<td class="num">${p.mem === null ? "—" : fmt.kb(p.mem)}</td>` : ""}
      <td>${e.run_url ? `<a href="${esc(e.run_url)}">run</a>` : ""}</td></tr>`).join("");
  setHash({ test: `${sel.group}: ${sel.name}` });
}

// ---- latest run ---------------------------------------------------------
async function renderLatest() {
  const latest = state.index.records[0];
  if (!latest) { ["latest-head", "table-flags", "table-groups", "table-slowest", "table-heaviest"].forEach(id => $(id).innerHTML = ""); return; }
  const rec = await loadRecord(latest);
  const t = rec.totals || {};
  $("latest-head").innerHTML = `<p><a href="${COMMIT}${esc(rec.sha)}">${fmt.sha(rec.sha)}</a> on ${esc(rec.branch || "")}, committed ${fmt.when(rec)}, measured ${fmt.date(rec.date)} ·
    ${fmt.n(t.tests || 0)} tests, ${t.failed || 0} failed · ${esc((rec.host || {}).cpu || "")} · -j ${(rec.host || {}).jobs || "?"} ·
    ${rec.run_url ? `<a href="${esc(rec.run_url)}">workflow run</a>` : ""}</p>`;

  const flags = Object.entries(rec.flags || {}).sort((a, b) => (a[1].level !== "fail") - (b[1].level !== "fail") || (b[1].current / b[1].baseline) - (a[1].current / a[1].baseline));
  $("table-flags").innerHTML = flags.length ? `<tr><th>level</th><th>test</th><th>metric</th><th class="num">baseline</th><th class="num">current</th><th class="num">Δ</th><th class="num">records over</th></tr>` +
    flags.map(([key, f]) => { const [group, , name] = key.split("|"); const u = f.unit;
      return `<tr class="clickable" data-test="${esc(group)}: ${esc(name)}"><td><span class="level ${esc(f.level)}">${esc(f.level)}${f.candidate === "fail" && f.level === "warn" ? " (1st over fail)" : ""}</span></td>
      <td class="name">${esc(name)} <span class="unit">${esc(group)}</span></td><td>${esc(f.metric)}</td><td class="num">${fmt[u](f.baseline)}</td><td class="num">${fmt[u](f.current)}</td>${deltaCell(f.current, f.baseline)}<td class="num">${f.streak}</td></tr>`; }).join("")
    : `<tr><td>${state.index.records.length > 1 ? "No test over the thresholds." : "No baseline yet."}</td></tr>`;

  const groups = latest.groups || {};
  const order = GROUP_ORDER.filter(g => groups[g]);
  $("table-groups").innerHTML = order.length ? `<tr><th>group</th><th class="num">tests</th><th class="num">failed</th><th class="num">wall time</th></tr>` +
    order.map(g => `<tr><td>${esc(g)}</td><td class="num">${fmt.n(groups[g].tests)}</td><td class="num">${groups[g].failed}</td><td class="num">${fmt.s(groups[g].wall_s)}</td></tr>`).join("") : "";

  const slowGroup = kind() === "pytest" ? "testnet" : "ctest";
  const spec = ROWS[slowGroup];
  const slow = (rec[slowGroup] || []).filter(r => !spec.status || spec.status(r) === "pass").sort((a, b) => spec.time(b) - spec.time(a)).slice(0, 20);
  $("h-slowest").textContent = `Slowest ${slowGroup === "testnet" ? "tau-testnet tests" : "ctest entries"} (wall)`;
  $("table-slowest").innerHTML = `<tr><th>test</th><th class="num">time</th></tr>` +
    slow.map(r => `<tr class="clickable" data-test="${esc(slowGroup)}: ${esc(spec.key(r))}"><td class="name">${esc(spec.key(r))}</td><td class="num">${fmt.s(spec.time(r))}</td></tr>`).join("");

  const heavy = (rec.process || []).slice().sort((a, b) => b[4] - a[4]).slice(0, 20);
  $("h-heaviest").classList.toggle("hidden", !heavy.length);
  $("table-heaviest").innerHTML = heavy.length ? `<tr><th>process</th><th class="num">peak RSS</th><th class="num">user+sys</th></tr>` +
    heavy.map(r => `<tr class="clickable" data-test="process: ${esc(r[0])}"><td class="name">${esc(r[0])}</td><td class="num">${fmt.kb(r[4])}</td><td class="num">${fmt.s(r[2] + r[3])}</td></tr>`).join("") : "";
}

// ---- compare ------------------------------------------------------------
function fillCompareControls() {
  const opts = state.index.records.map((e, i) => `<option value="${i}">${fmt.sha(e.sha)} · ${fmt.when(e)}</option>`).join("");
  $("cmp-a").innerHTML = opts; $("cmp-b").innerHTML = opts;
  $("cmp-a").value = Math.min(1, state.index.records.length - 1); $("cmp-b").value = 0;
  const groups = kind() === "pytest" ? ["testnet"] : ["ctest", "process", "case"];
  $("cmp-group").innerHTML = groups.map(g => `<option>${g}</option>`).join("");
}
async function renderCompare() {
  const a = state.index.records[+$("cmp-a").value], b = state.index.records[+$("cmp-b").value];
  if (!a || !b) { $("cmp-summary").innerHTML = ""; $("table-compare").innerHTML = ""; return; }
  const group = $("cmp-group").value, metric = $("cmp-metric").value, min = (+$("cmp-min").value || 0) / 100;
  const spec = ROWS[group];
  // the absolute floor, in the unit of the rows, with the thresholds' floors as defaults
  const floorUnit = metric === "memory" ? "MB" : spec.unit;
  if ($("cmp-floor-unit").textContent !== floorUnit) {
    $("cmp-floor-unit").textContent = floorUnit;
    $("cmp-floor").value = floorUnit === "MB" ? 32 : floorUnit === "ms" ? 50 : 0.25;
  }
  const floor = (+$("cmp-floor").value || 0) * (floorUnit === "MB" ? 1024 : 1);
  if (metric === "memory" && !spec.mem) { $("cmp-summary").innerHTML = `<div class="tile"><div class="k">memory</div><div class="v">—</div><div class="s">${group} rows carry no memory</div></div>`; $("table-compare").innerHTML = ""; return; }
  status("loading the two records…");
  const [ra, rb] = await Promise.all([loadRecord(a), loadRecord(b)]);
  status("");
  const value = metric === "time" ? spec.time : spec.mem;
  const unit = metric === "time" ? spec.unit : "kb";
  const map = rec => { const m = new Map(); for (const r of rec[group] || []) if (!spec.status || spec.status(r) === "pass") m.set(spec.key(r), value(r)); return m; };
  const ma = map(ra), mb = map(rb);
  let slower = 0, faster = 0, same = 0, sumA = 0, sumB = 0;
  const rows = [];
  for (const [k, vb] of mb) {
    if (!ma.has(k)) { rows.push({ k, va: null, vb }); continue; }
    const va = ma.get(k); sumA += va; sumB += vb;
    const d = va ? (vb - va) / va : 0;
    const over = Math.abs(d) >= min && Math.abs(vb - va) >= floor;
    if (over && d > 0) slower++; else if (over && d < 0) faster++; else same++;
    if (over) rows.push({ k, va, vb, d });
  }
  const gone = [...ma.keys()].filter(k => !mb.has(k)).length;
  rows.sort((x, y) => (y.d ?? Infinity) - (x.d ?? Infinity));
  $("cmp-summary").innerHTML = [
    ["compared", fmt.n(slower + faster + same), `${rows.filter(r => r.va === null).length} new, ${gone} gone`],
    [`slower`, fmt.n(slower), `over ${min * 100} % and the floor`], [`faster`, fmt.n(faster), `over ${min * 100} % and the floor`],
    ["sum", `${fmt[unit](sumA)} → ${fmt[unit](sumB)}`, fmt.pct(sumB, sumA)],
  ].map(([k, v, s]) => `<div class="tile"><div class="k">${k}</div><div class="v">${v}</div><div class="s">${s}</div></div>`).join("");
  $("table-compare").innerHTML = `<tr><th>test</th><th class="num">${fmt.sha(a.sha)}</th><th class="num">${fmt.sha(b.sha)}</th><th class="num">Δ</th></tr>` +
    rows.slice(0, 500).map(r => `<tr class="clickable" data-test="${esc(group)}: ${esc(r.k)}"><td class="name">${esc(r.k)}</td><td class="num">${r.va === null ? "new" : fmt[unit](r.va)}</td><td class="num">${fmt[unit](r.vb)}</td>${deltaCell(r.vb, r.va)}</tr>`).join("") +
    (rows.length > 500 ? `<tr><td colspan="4">${fmt.n(rows.length - 500)} more rows over the filter</td></tr>` : "");
}

// ---- wiring -------------------------------------------------------------
async function showView(view) {
  state.view = view;
  document.querySelectorAll("#tabs button").forEach(b => b.classList.toggle("active", b.dataset.view === view));
  document.querySelectorAll(".view").forEach(v => v.classList.toggle("hidden", v.id !== `view-${view}`));
  setHash({ view });
  try {
    if (view === "overview") renderOverview();
    else if (view === "test") { await fillTestNames(); await renderTest(); }
    else if (view === "latest") await renderLatest();
    else if (view === "compare") { fillCompareControls(); await renderCompare(); }
  } catch (e) { status(`error: ${e.message}`); }
}
async function selectSeries(name) {
  state.series = name;
  $("series").value = name;
  setHash({ series: name });
  await loadIndex();
  await showView(state.view);
}
document.addEventListener("click", e => {
  const tr = e.target.closest("tr.clickable");
  if (tr && tr.dataset.test) { $("test-search").value = tr.dataset.test; showView("test"); }
});
$("series").addEventListener("change", e => selectSeries(e.target.value));
document.querySelectorAll("#tabs button").forEach(b => b.addEventListener("click", () => showView(b.dataset.view)));
$("test-search").addEventListener("change", renderTest);
$("test-depth").addEventListener("change", renderTest);
["cmp-a", "cmp-b", "cmp-group", "cmp-metric", "cmp-min", "cmp-floor"].forEach(id => $(id).addEventListener("change", renderCompare));
window.matchMedia("(prefers-color-scheme: dark)").addEventListener("change", () => showView(state.view));

(async () => {
  $("data-source").textContent = DATA;
  try {
    await loadSeriesList();
    const h = hashParams();
    if (h.get("test")) $("test-search").value = h.get("test");
    await loadIndex();
    await showView(h.get("view") || (h.get("test") ? "test" : "overview"));
  } catch (e) { status(`error: ${e.message}`); }
})();
})();
