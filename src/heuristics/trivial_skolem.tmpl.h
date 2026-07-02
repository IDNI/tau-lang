// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "trivial_skolem"

namespace idni::tau_lang {

// Attempts to invert `side = other` for `var`: recursively peels invertible
// term operators off `side` until `var` is isolated, accumulating the
// witness expression in `result`. Returns false if `var` can't be isolated
// this way. (Only the base case is implemented so far; operator peeling is
// added in later tasks.)
template <NodeType node>
bool trivial_skolem_invert_term(tref side, tref other, tref var, tref& result) {
	using tau = tree<node>;
	// side/other/result are "bf"-wrapped terms; var is a raw variable node,
	// so it must be compared against side's unwrapped content.
	if (tau::get(side)[0].get() == var) { result = other; return true; }
	const tau& t = tau::get(side)[0];
	if (t.is(tau::bf_neg)) {
		tref inner = t.first();
		return trivial_skolem_invert_term<node>(inner,
			build_bf_neg<node>(other), var, result);
	}
	if (t.is(tau::bf_xor)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_xor<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_xor<node>(other, l), var, result);
	}
	if (t.is(tau::bf_add)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_sub<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_sub<node>(other, l), var, result);
	}
	if (t.is(tau::bf_sub)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_add<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_sub<node>(l, other), var, result);
	}
	return false;
}

// Checks whether `clause` (a wff-wrapped formula) is a bf_eq that isolates
// `var` via invertible operators, with a witness free of every variable in
// `vars`. On success, `witness` holds the witness expression.
template <NodeType node>
bool trivial_skolem_invertible_clause(tref clause, tref var,
	const trefs& vars, tref& witness)
{
	using tau = tree<node>;
	const tau& c0 = tau::get(clause)[0];
	if (!c0.is(tau::bf_eq)) return false;
	tref l = c0.first(), r = c0.second();
	bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
	if (l_has == r_has) return false;
	tref candidate;
	bool ok = l_has
		? trivial_skolem_invert_term<node>(l, r, var, candidate)
		: trivial_skolem_invert_term<node>(r, l, var, candidate);
	if (!ok) return false;
	for (tref v : vars) if (contains<node>(candidate, v)) return false;
	witness = candidate;
	return true;
}

template <NodeType node>
tref trivial_skolem_ex(const trefs& vars, tref phi) {
	using tau = tree<node>;
	trefs clauses = get_cnf_wff_clauses<node>(phi);
	std::vector<bool> dropped(clauses.size(), false);
	trefs kept_vars;
	for (tref v : vars) {
		size_t occurrence = clauses.size();
		bool multiple = false;
		for (size_t i = 0; i < clauses.size(); ++i) {
			if (!contains<node>(clauses[i], v)) continue;
			if (occurrence != clauses.size()) { multiple = true; break; }
			occurrence = i;
		}
		[[maybe_unused]] tref witness = nullptr;
		if (!multiple && occurrence != clauses.size()
				&& trivial_skolem_invertible_clause<node>(
					clauses[occurrence], v, vars, witness))
			dropped[occurrence] = true;
		else kept_vars.push_back(v);
	}
	trefs remaining;
	for (size_t i = 0; i < clauses.size(); ++i)
		if (!dropped[i]) remaining.push_back(clauses[i]);
	tref matrix;
	if (remaining.empty()) matrix = tau::_T();
	else {
		matrix = remaining[0];
		for (size_t i = 1; i < remaining.size(); ++i)
			matrix = tau::build_wff_and(matrix, remaining[i]);
	}
	if (kept_vars.empty()) return matrix;
	return tau::build_wff_ex_many(kept_vars, matrix);
}

} // namespace idni::tau_lang
