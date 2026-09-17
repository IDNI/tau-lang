// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

namespace bv_case_split_detail {

// Base-2 digits of the bitvector constant in `c`, left-padded to `width`.
template <NodeType node>
std::optional<std::string> bits_of(tref c, size_t width) {
	auto cte = std::get<bv>(tree<node>::get(c).get_ba_constant());
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
	auto step = [&](tref n) -> tref {
		if (!is_child_quantifier<node>(n)) return n;
		const tau& t = tau::get(n);
		const tref var = t[0].first();
		const tref scope = t[0].second();
		const size_t vtype = tau::get(var).get_ba_type();
		if (vtype == 0 || !is_bv_type_family<node>(vtype)) return n;
		const size_t width = get_bv_width<node>(vtype);
		if (width == 0) return n;
		// Scan: every occurrence of `var` must be one side of a comparison
		// whose other side is a constant of the same type. BA constants are
		// opaque and not descended into.
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
					if (!tau::get(other).is_ba_constant()
						|| tau::get(other).get_ba_type() != vtype) { ok = false; break; }
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
		auto instance = [&](tref body, tref w) -> tref {
			subtree_map<node, tref> changes;
			for (tref o : occ) changes[o] = w;
			return rewriter::replace<node>(body, changes);
		};
		auto cases = [&](tref body) -> tref {
			tref res = nullptr;
			for (tref w : witnesses) {
				tref inst = instance(body, w);
				res = !res ? inst : is_ex ? tau::build_wff_or(res, inst)
					: tau::build_wff_and(res, inst);
			}
			return res;
		};
		auto eliminate = [&](auto& self, tref body) -> tref {
			if (!contains<node>(body, var)) return body;
			const tau& tb = tau::get(body);
			if (tb.is(tau::wff) && (tb.child_is(tau::wff_and) || tb.child_is(tau::wff_or))) {
				const bool is_and = tb.child_is(tau::wff_and);
				std::vector<tref> dep, ind;
				std::function<void(tref)> flat = [&](tref x) {
					const tau& tx = tau::get(x);
					if (tx.is(tau::wff) && tx.child_is(is_and ? tau::wff_and : tau::wff_or)) {
						flat(tx[0].first()); flat(tx[0].second()); return;
					}
					(contains<node>(x, var) ? dep : ind).push_back(x);
				};
				flat(body);
				if (!ind.empty()) {
					tref d = dep.size() == 1 ? dep[0]
						: (is_and ? tau::build_wff_and(dep) : tau::build_wff_or(dep));
					std::vector<tref> parts{self(self, d)};
					parts.insert(parts.end(), ind.begin(), ind.end());
					return is_and ? tau::build_wff_and(parts) : tau::build_wff_or(parts);
				}
			}
			if (is_child_quantifier<node>(body)
				&& tb.child_is(is_ex ? tau::wff_ex : tau::wff_all)
				&& !(tau::get(tb[0].first()) == tau::get(var))) {
				tref y = tb[0].first(), sc = tb[0].second();
				tref inner = self(self, sc);
				return is_ex ? tau::build_wff_ex(y, inner, false)
					: tau::build_wff_all(y, inner, false);
			}
			return cases(body);
		};
		return eliminate(eliminate, scope);
	};
	auto visit = [](tref x) { return while_is_formula<node>(x); };
	return post_order<node>(formula).apply_unique(step, visit);
}

} // namespace idni::tau_lang
