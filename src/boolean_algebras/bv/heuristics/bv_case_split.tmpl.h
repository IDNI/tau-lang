// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

namespace bv_case_split_detail {

// Base-2 digits of the bitvector value `c` (a constant, or the typed `0` or
// `1`, all bits clear or set), left-padded to `width`; nullopt when it is not
// a concrete value or is wider than `width`. A constant must hold a `bv`.
template <NodeType node>
std::optional<std::string> bits_of(tref c, size_t width) {
	const auto& t = tree<node>::get(c);
	if (t.is(tree<node>::bf_f)) return std::string(width, '0');
	if (t.is(tree<node>::bf_t)) return std::string(width, '1');
	auto cte = std::get<bv>(t.get_ba_constant());
	if (!cte.isBitVectorValue()) return std::nullopt;
	std::string s = cte.getBitVectorValue();
	if (s.size() > width) return std::nullopt;
	return std::string(width - s.size(), '0') + s;
}

// Successor of a padded base-2 string; nullopt on overflow (all ones).
inline std::optional<std::string> succ(std::string s) {
	for (size_t i = s.size(); i-- > 0;) {
		if (s[i] == '0') { s[i] = '1'; return s; }
		s[i] = '0';
	}
	return std::nullopt;
}

// Distinct nodes reachable from `n`; the count stops once it passes `limit`.
template <NodeType node>
size_t dag_size(tref n, size_t limit) {
	std::vector<tref> st{n};
	std::unordered_set<tref> seen;
	while (!st.empty() && seen.size() <= limit) {
		tref x = st.back(); st.pop_back();
		if (!x || !seen.insert(x).second) continue;
		for (tref c : tree<node>::get(x).children()) st.push_back(c);
	}
	return seen.size();
}

inline size_t saturating_mul(size_t a, size_t b) {
	return b && a > std::numeric_limits<size_t>::max() / b
		? std::numeric_limits<size_t>::max() : a * b;
}

} // namespace bv_case_split_detail

template <NodeType node>
tref bv_case_split_quantifiers(tref formula) {
	using tau = tree<node>;
	using namespace bv_case_split_detail;
	auto is_comparison = [](const tau& atom) {
		switch (atom.value.nt) {
			case tau::bf_eq: case tau::bf_neq:
			case tau::bf_lt: case tau::bf_lteq:
			case tau::bf_gt: case tau::bf_gteq:
			case tau::bf_nlt: case tau::bf_nlteq:
			case tau::bf_ngt: case tau::bf_ngteq:
				return true;
			default: return false;
		}
	};
	auto is_order = [](const tau& atom) {
		return atom.value.nt != tau::bf_eq && atom.value.nt != tau::bf_neq;
	};
	// The instances of every split binder together may hold at most
	// bv_case_split_max_growth times the nodes of `formula`. Binders whose
	// variables share one scope multiply each other's instances, so the
	// budget is shared by all of them; a binder past it is left to the
	// pipeline (GitHub #203).
	const size_t budget = bv_case_split_max_growth
		== std::numeric_limits<size_t>::max()
		? std::numeric_limits<size_t>::max()
		: saturating_mul(dag_size<node>(formula,
			std::numeric_limits<size_t>::max()), bv_case_split_max_growth);
	size_t spent = 0;
	auto step = [&](tref n) -> tref {
		if (!is_child_quantifier<node>(n)) return n;
		const tau& t = tau::get(n);
		const tref var = t[0].first();
		const tref scope = t[0].second();
		const size_t vtype = tau::get(var).get_ba_type();
		if (vtype == 0 || !is_bv_type_family<node>(vtype)) return n;
		auto width_r = get_bv_width<node>(vtype);
		// TODO (HIGH) dropped error: get_bv_width's report -- post_order's step callback returns a fixed tref, so the quantifier stays untouched.
		if (!width_r.has_value()) return n;
		const size_t width = width_r.value();
		// Scan: every occurrence of `var` must be one side of a comparison
		// whose other side is a value of the same type: a constant, or the
		// typed `0` or `1` normalization leaves in its place. BA constants
		// are opaque and not descended into.
		std::vector<tref> occ;
		std::vector<tref> tests;   // the tested constants (term nodes), deduplicated
		bool ok = true, any_order = false;
		// The scope is walked as a DAG: substitution passes upstream share
		// subterms, and revisiting a shared subtree once per parent is
		// exponential in the sharing (GitHub #122). The verdict depends on
		// the set of nodes seen, not on the order.
		std::vector<tref> st{scope};
		std::unordered_set<tref> seen;
		while (!st.empty() && ok) {
			tref x = st.back(); st.pop_back();
			if (!x || !seen.insert(x).second) continue;
			const tau& tx = tau::get(x);
			if (tx.is_ba_constant()) continue;
			// A nested binder of the same variable: its occurrences are
			// structurally identical to the free ones and would be
			// substituted with them. Decline rather than capture.
			if (is_child_quantifier<node>(x)
				&& tau::get(tx[0].first()) == tau::get(var)) { ok = false; break; }
			if (tx.is(tau::wff) && is_comparison(tx[0])) {
				const tau& atom = tx[0];
				tref l = atom[0][0].get(), r = atom[1][0].get();
				const bool lv = tau::get(l) == tau::get(var);
				const bool rv = tau::get(r) == tau::get(var);
				if (lv || rv) {
					if (lv && rv) { ok = false; break; }
					tref other = lv ? r : l;
					const tau& to = tau::get(other);
					if (!(to.is_ba_constant() || to.is(tau::bf_f)
							|| to.is(tau::bf_t))
						|| to.get_ba_type() != vtype) { ok = false; break; }
					occ.push_back(lv ? l : r);
					if (is_order(atom)) any_order = true;
					bool dup = false;
					for (tref c : tests)
						if (tau::get(c) == tau::get(other)) { dup = true; break; }
					if (!dup) tests.push_back(other);
					continue;
				}
			}
			if (tau::get(x) == tau::get(var)) { ok = false; break; }
			for (tref c : tx.children()) st.push_back(c);
		}
		if (!ok || occ.empty()
			|| tests.size() > bv_case_split_max_tests) return n;
		auto constant_of = [&](const std::string& bits) -> tref {
			typename node::constant cte = {make_bitvector_value(width, bits)};
			return tau::get_ba_constant(cte, vtype);
		};
		const std::string zero(width, '0');
		std::vector<tref> witnesses;
		// Known cells: sort the constants, then each of them and one value per
		// non-empty interval (a single complement value when no order atom
		// occurs -- all intervals agree then).
		std::vector<std::pair<std::string, tref>> tested;
		for (tref c : tests) {
			auto bits = bits_of<node>(c, width);
			if (!bits) return n;
			// `0` and a constant of all clear bits are one cell
			if (std::ranges::find(tested, *bits,
				&std::pair<std::string, tref>::first) == tested.end())
				tested.emplace_back(*bits, c);
		}
		std::sort(tested.begin(), tested.end(),
			[](const auto& a, const auto& b) { return a.first < b.first; });
		for (const auto& [_, c] : tested) witnesses.push_back(c);
		std::vector<std::string> gaps;
		if (tested.front().first != zero) gaps.push_back(zero);
		for (size_t i = 0; i + 1 < tested.size(); ++i) {
			auto s = succ(tested[i].first);
			if (s && *s < tested[i + 1].first) gaps.push_back(*s);
		}
		if (auto s = succ(tested.back().first); s) gaps.push_back(*s);
		if (!any_order && !gaps.empty()) gaps.resize(1);
		for (const auto& g : gaps) witnesses.push_back(constant_of(g));
		// Miniscoping before instantiating: only the part of the scope that
		// mentions `var` is instantiated per cell. For either quantifier,
		// Q v (A(v) && B) == (Q v A) && B and Q v (A(v) || B) == (Q v A) || B
		// when B does not contain v, so along an `&&`/`||` chain every
		// conjunct (disjunct) free of v is attached once, outside the case
		// analysis, instead of being copied into every cell. A binder of
		// another variable of the same kind is passed through: same-kind
		// quantifiers commute and the witnesses are constants, so nothing is
		// captured. Without the descent a foreign binder between v and its
		// dependent conjunct made `cases` copy the whole scope -- inner
		// binders included -- once per cell, and nested blocks multiplied
		// (GitHub #122).
		const bool is_ex = t.child_is(tau::wff_ex);
		size_t cost = 0;
		bool over = false;
		auto instance = [&](tref body, tref w) -> tref {
			subtree_map<node, tref> changes;
			for (tref o : occ) changes[o] = w;
			return rewriter::replace<node>(body, changes);
		};
		auto cases = [&](tref body) -> tref {
			if (budget != std::numeric_limits<size_t>::max()) {
				const size_t left = budget - spent - cost;
				const size_t size = dag_size<node>(body,
					left / witnesses.size());
				if (saturating_mul(size, witnesses.size()) > left) {
					over = true;
					return body;
				}
				cost += size * witnesses.size();
			}
			tref res = nullptr;
			for (tref w : witnesses) {
				tref inst = instance(body, w);
				res = !res ? inst : is_ex ? tau::build_wff_or(res, inst)
					: tau::build_wff_and(res, inst);
			}
			return res;
		};
		// A std::function, not a lambda handed to itself: cl overflows its
		// stack on that generic lambda inside this template.
		std::function<tref(tref)> eliminate = [&](tref body) -> tref {
			if (!contains<node>(body, var)) return body;
			const tau& tb = tau::get(body);
			if (tb.is(tau::wff) && (tb.child_is(tau::wff_and) || tb.child_is(tau::wff_or))) {
				const bool is_and = tb.child_is(tau::wff_and);
				std::vector<tref> dep, ind;
				// Flattened with an explicit stack: a desugared `?:` chain is
				// a connective chain as long as the formula, and one frame per
				// link overflows the thread stack (GitHub #205). A shared
				// operand is kept once, as the connective is idempotent.
				std::vector<tref> st{body};
				std::unordered_set<tref> seen;
				while (!st.empty()) {
					tref x = st.back(); st.pop_back();
					if (!seen.insert(x).second) continue;
					const tau& tx = tau::get(x);
					if (tx.is(tau::wff) && tx.child_is(is_and ? tau::wff_and : tau::wff_or)) {
						st.push_back(tx[0].second()); st.push_back(tx[0].first());
						continue;
					}
					(contains<node>(x, var) ? dep : ind).push_back(x);
				}
				if (!ind.empty()) {
					tref d = dep.size() == 1 ? dep[0]
						: (is_and ? tau::build_wff_and(dep) : tau::build_wff_or(dep));
					std::vector<tref> parts{eliminate(d)};
					parts.insert(parts.end(), ind.begin(), ind.end());
					return is_and ? tau::build_wff_and(parts) : tau::build_wff_or(parts);
				}
			}
			if (is_child_quantifier<node>(body)
				&& tb.child_is(is_ex ? tau::wff_ex : tau::wff_all)
				&& !(tau::get(tb[0].first()) == tau::get(var))) {
				tref y = tb[0].first(), sc = tb[0].second();
				tref inner = eliminate(sc);
				return is_ex ? tau::build_wff_ex(y, inner, false)
					: tau::build_wff_all(y, inner, false);
			}
			return cases(body);
		};
		tref res = eliminate(scope);
		if (over) return n;
		spent += cost;
		return res;
	};
	auto visit = [](tref x) { return while_is_formula<node>(x); };
	return post_order<node>(formula).apply_unique(step, visit);
}

} // namespace idni::tau_lang
