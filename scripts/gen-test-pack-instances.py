#!/usr/bin/env python3
# gen-test-pack-instances.py <build-dir> [--min-tus N] [--shards K]
#
# Writes src/test_pack_instances.def from the test objects of a release build
# of the default pack on x86_64 Linux, built with an empty .def: the list
# removes the very weak symbols it is read from. Every function template instantiation over a node type
# that at least N test objects compile on their own (a weak symbol) becomes one
# TAU_TEST_PACK_INST(shard, signature) line. extern_template_test_pack.h
# declares each line extern in every test TU, and the K translation units
# src/instantiate_test_pack_<shard>.cpp instantiate them once, in the
# TAU_test_pack library the tests link. K must match the shards
# src/instantiate_test_pack.h and tests/CMakeLists.txt know (8).
#
# The configured pack's node is written as test_node_t, so the list fits the
# reduced packs too; a line that names an algebra, or a function its plugin
# declares, is guarded on that algebra.
import argparse, collections, glob, os, re, subprocess, sys

ap = argparse.ArgumentParser()
ap.add_argument("build")
ap.add_argument("--min-tus", type=int, default=8)
ap.add_argument("--shards", type=int, default=8)
a = ap.parse_args()
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

pack_h = glob.glob(f"{a.build}/generated/include/tau_pack.h")
if not pack_h: sys.exit(f"{a.build}: no generated/include/tau_pack.h")
m = re.search(r"#define TAU_PACK_FULL_BAS (.*)", open(pack_h[0]).read())
PACK_NODE = f"node<{m.group(1).strip()}>"

size = collections.Counter(); tus = collections.Counter()
objs = glob.glob(f"{a.build}/tests/**/*.o", recursive=True)
if not objs: sys.exit(f"{a.build}: no test objects")
for o in objs:
	out = subprocess.run(["nm", "-S", "--defined-only", o],
		capture_output=True, text=True).stdout
	for l in out.splitlines():
		p = l.split()
		if len(p) != 4 or p[2] not in "Ww": continue
		try: size[p[3]] += int(p[1], 16)
		except ValueError: continue
		tus[p[3]] += 1
names = [n for n in size if tus[n] >= a.min_tus]
dem = subprocess.run(["c++filt"], input="\n".join(names),
	capture_output=True, text=True).stdout.splitlines()

SBF = ("std::shared_ptr<idni::tau_lang::bdd_handle<idni::tau_lang::Bool, "
	"idni::tau_lang::bdd_options{(unsigned char)18, (unsigned char)12}> >")
REPL = [(SBF, "sbf_ba"), ("cvc5::Term", "bv"), ("[abi:cxx11]", ""),
	("std::__cxx11::basic_string<char, std::char_traits<char>, "
	 "std::allocator<char> >", "std::string"),
	("idni::tau_lang::", "")]
GUARD = {"sbf_ba": "SBF", "bv": "BV", "qint": "QINT", "qlt": "QLT",
	"nlang_ba": "NLANG", "hsb": "HSB"}
# a local entity, an inline member or a name the source cannot spell
LOCAL = ("{lambda", "(anonymous namespace)", "{unnamed", "::{", "_GLOBAL",
	"guard variable", "typeinfo", "vtable", "clone ", "operator", "TLS ")

# declared in cpp_codegen.h, which extern_template_test_pack.h does not include:
# only the codegen suites read it, and every other suite would parse it too
UNREACHABLE = {"build_program_desc", "playable_table_solution"}

def top_split(s, sep):
	out, d, cur = [], 0, ""
	for ch in s:
		if ch in "<(": d += 1
		if ch in ">)": d -= 1
		if ch == sep and d == 0: out.append(cur); cur = ""
		else: cur += ch
	out.append(cur)
	return out

def plugin_of(fn):
	"""The algebra whose plugin alone declares fn, or None for core."""
	# a namespace-scope declaration starts in column 0; a call is indented
	hits = subprocess.run(["grep", "-rlE", rf"^[A-Za-z].*[ *&]{fn} *\(",
		f"{root}/src", "--include=*.h"], capture_output=True, text=True).stdout.split()
	dirs = {re.sub(r".*/src/boolean_algebras/([^/]+)/.*", r"\1", h)
		if "/boolean_algebras/" in h and "/_template/" not in h else "core"
		for h in hits if not h.endswith(("/tau.h", "/undefs.h"))}
	dirs.discard("_template")
	if len(dirs) == 1 and "core" not in dirs:
		d = dirs.pop()
		return {"sbf": "SBF", "bv": "BV", "qint": "QINT", "qlt": "QLT",
			"nlang": "NLANG", "hsb": "HSB"}.get(d)
	return None

rows = {}
plugin = {}
for n, d in zip(names, dem):
	if "idni::tau_lang::node<" not in d or any(x in d for x in LOCAL): continue
	s = d
	for x, y in REPL: s = s.replace(x, y)
	s = s.replace(PACK_NODE, "test_node_t")
	if "{" in s or "long long" in s: continue
	# the LP64 spellings of the tree's aliases, so a line reads as its source does
	s = re.sub(r"\bunsigned long\b", "size_t", s)
	s = s.replace("long const*", "tref")
	s = re.sub(r"\blong\b", "int64_t", s)
	d0, p = 0, -1
	for i, ch in enumerate(s):
		if ch == "<": d0 += 1
		elif ch == ">": d0 -= 1
		elif ch == "(" and d0 == 0: p = i; break
	toks = top_split(s[:p], " ")
	if len(toks) < 2: continue  # constructor, destructor
	qn = toks[-1]
	if re.match(r"(std|__gnu_cxx|boost|cvc5|doctest)::", qn): continue
	if "detail::" in qn: continue  # implementation helpers stay with their caller
	if re.sub(r"<.*", "", qn).split("::")[-1] in UNREACHABLE: continue
	# a template outside idni::tau_lang cannot be instantiated from inside it
	if qn.startswith(("idni::", "::")): continue
	fn = re.sub(r"<.*", "", qn).split("::")[-1]
	if fn not in plugin: plugin[fn] = plugin_of(fn)
	g = {GUARD[w] for w in re.findall(r"\b(sbf_ba|bv|qint|qlt|nlang_ba|hsb)\b", s)}
	if plugin[fn]: g.add(plugin[fn])
	k = (tuple(sorted(g)), s)
	sz, t = rows.get(k, (0, 0))
	rows[k] = (sz + size[n], max(t, tus[n]))

# balance the shards by the code each line produces
load = [0] * a.shards
shard = {}
for k, (sz, t) in sorted(rows.items(), key=lambda x: -x[1][0]):
	i = load.index(min(load)); load[i] += sz; shard[k] = i

by_guard = collections.defaultdict(list)
for k in sorted(rows, key=lambda k: (k[0], k[1])): by_guard[k[0]].append(k)
out = [
	"// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md",
	"",
	"// Generated by scripts/gen-test-pack-instances.py: the function template",
	"// instantiations the test TUs would otherwise each compile. Do not edit;",
	"// regenerate from a release build of the default pack. Each line is",
	"// TAU_TEST_PACK_INST(shard, signature...), expanded by",
	"// extern_template_test_pack.h and src/instantiate_test_pack_<shard>.cpp.",
	"//",
	"// The signatures come from demangled LP64 Linux symbols, where long is",
	"// int64_t and unsigned long is size_t; elsewhere a test TU instantiates",
	"// its own templates, as without this list.",
	"",
	"#if defined(__linux__) && defined(__LP64__)",
	""]
for g in sorted(by_guard):
	if g: out.append("#if " + " && ".join(f"defined(TAU_PACK_HAS_BA_{x})" for x in g))
	for k in by_guard[g]:
		out.append(f"TAU_TEST_PACK_INST({shard[k]}, {k[1]})")
	if g: out.append("#endif")
	out.append("")
out.append("#endif // __linux__ && __LP64__")
out.append("")
open(f"{root}/src/test_pack_instances.def", "w").write("\n".join(out))
print(f"{len(rows)} instantiations, {sum(v[0] for v in rows.values()) >> 10} KB "
	f"of test object code, shards {[x >> 10 for x in load]} KB", file=sys.stderr)
