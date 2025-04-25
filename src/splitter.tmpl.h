// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "splitter.h"

namespace idni::tau_lang {

template <BAsPack... BAs>
tref split(tref fm, const size_t fm_type, bool is_cnf, const splitter_type st,
	trefs& mem, size_t i, bool check_temps)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	// First get clauses if not already in mem
	const bool is_wff = fm_type == tau::wff;
	if (mem.empty()) {
		if (is_wff) {
			mem = is_cnf
				? tau::get_cnf_wff_clauses(fm)
				: tau::get_dnf_wff_clauses(fm);
		} else {
			DBG(assert(fm_type == tau::bf);)
			mem = is_cnf
				? tau::get_cnf_bf_clauses(fm)
				: tau::get_dnf_bf_clauses(fm);
		}
	}
	tref sym = is_wff
		? (is_cnf ? tau::_T() : tau::_F())
		: (is_cnf ? tau::_1() : tau::_0());
	switch (st) {
	case splitter_type::upper: {
		// Remove exactly one clause
		if (mem.size() == 1)
			return fm;
		if (i < mem.size()) {
			// We cannot delete a clause holding a temporary io var
			if (check_temps && has_temporary_io_var(mem[i]))
				return fm;
			return rewriter::replace<node>(fm, mem[i], sym);
		}
		break;
	}
	case splitter_type::middle: {
		// Remove half of the clause
		size_t window = mem.size() / 2;
		typename tau::subtree_map changes;
		for (size_t j = 0; j < window; ++j) {
			size_t idx = (i + j) % mem.size();
			if (check_temps && has_temporary_io_var(mem[idx]))
				continue;
			changes.emplace(mem[idx], sym);
		}
		if (!changes.empty())
			return rewriter::replace<node>(fm, changes);
		break;
	}
	case splitter_type::lower: {
		// Remove all but one clause
		if (i < mem.size())
			if (check_temps && !has_temporary_io_var(mem[i]))
				return mem[i];
		break;
	}
	case splitter_type::bad:
		// This case must not happen
		assert(false);
	}
	return fm;
}

// If checking a temporal Tau formula F, we split a single DNF clause.
// In order to check if the split clause yields a splitter for F, we have that
// "fm" holds the clause, "splitter" holds the splitter of the clause and
// "spec" holds the original temporal Tau formula
// If we check a non-temporal Tau formula, it suffices to place it in "fm" and
// the proposed splitter in "splitter".
template <BAsPack... BAs>
bool is_splitter(tref fm, tref splitter, tref spec_clause) {
	if (spec_clause) {
		// We are dealing with a temporal formula
		DBG(assert(is_tau_impl(splitter, fm));)
		tref new_spec_clause = normalize_with_temp_simp(
			rewriter::replace<node<BAs...>>(spec_clause, fm, splitter));
		if (is_tau_formula_sat<BAs...>(new_spec_clause)) {
			if (!are_tau_equivalent<BAs...>(new_spec_clause, spec_clause))
				return true;
		}
	} else {
		// We are dealing with a non-temporal formula
		if (is_non_temp_nso_satisfiable<BAs...>(splitter)
			&& !are_nso_equivalent<BAs...>(splitter, fm))
		{
			DBG(assert(is_nso_impl<BAs...>(splitter, fm));)
			return true;
		}
	}
	return false;
}

// Find a Boolean function which implies f
template <BAsPack... BAs>
tref good_splitter_using_function(tref f, splitter_type st, tref original_fm) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	DBG(assert(is<node>(f, tau::bf));)
	// First check if we have more then one disjunct
	trefs m;
	size_t i = 0;
	do {
		tref s = split(f, tau::bf, false, st, m, i, true);
		if (!tau::subtree_equals(s, f)) {
			tref new_fm = rewriter::replace<node>(original_fm, f, s);
			if(!are_nso_equivalent<BAs...>(original_fm, new_fm))
				return new_fm;
		}
	} while (++i < m.size());

	// Find possible coefficient in each disjunct of f
	trefs clauses = tau::get_leaves(f, tau::bf_or);
	// In case f is just a single clause
	if (clauses.empty()) clauses.push_back(f);
	for (tref clause : clauses) {
		tref coeff = tau::get(clause)
			.find_top(is<node, tau::bf_constant>);
		if (!coeff) continue;
		DBG(assert(is<node>(coeff, tau::bf_constant));)
		tref s = splitter<BAs...>(coeff);
		if (!tau::subtree_equals(s, coeff)) {
			tref new_fm = rewriter::replace<node>(
					original_fm, coeff, tau::trim(s));
			if(!are_nso_equivalent<BAs...>(original_fm, new_fm))
				return new_fm;
		}
	}
	return original_fm;
}

// Find a Boolean function which is implied by f
template <BAsPack... BAs>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
						tref original_fm)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	DBG(assert(is<node>(f, tau::bf));)
	// Convert Boolean function to CNF
	tref f_cnf = to_cnf<node, false>(f);

	// Try to remove a conjunt to produce splitter
	trefs m;
	size_t i = 0;
	do {
		tref s = split<BAs...>(f_cnf, tau::bf, true, st, m, i, true);
		if (!tau::subtree_equals(s, f_cnf)) {
			tref new_fm = rewriter::replace<node>(original_fm, f, s);
			if(!are_nso_equivalent<BAs...>(original_fm, new_fm))
				return new_fm;
		}
	} while (++i < m.size());

	// Try to split coefficient in each conjunct of f
	trefs clauses = tau::get_leaves(f_cnf, tau::bf_and);
	for (tref clause : clauses) {
		tref coeff = tau::get(clause).find_top(
			is_child<node, tau::bf_constant>);
		if (!coeff) continue;
		// TODO: Optimization: Instead of negating the formula think about reverse splitter call
		// If coefficient exists try to split the negation
		tref neg_coeff = push_negation_in<node, false>(
			tau::build_bf_neg(coeff));
		DBG(assert(is<node>(
				tau::trim(neg_coeff), tau::bf_constant));)
		tref s = splitter<BAs...>(tau::trim(neg_coeff), st);
		// Negating s results in a reversed splitter for s
		s = push_negation_in<node, false>(tau::build_bf_neg(s));
		if (!tau::subtree_equals(s, coeff)) {
			tref new_fm = rewriter::replace<node>(
					original_fm, coeff, s);
			if (!are_nso_equivalent<BAs...>(original_fm, new_fm))
				return new_fm;
		}
	}
	return original_fm;
}

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <BAsPack... BAs>
tref tau_bad_splitter(tref fm ) {
	using tau = tree<node<BAs...>>;
	tref new_uniter_const = tau::build_wff_neq(
		get_new_uninterpreted_constant<node<BAs...>>(fm, "split"));
	trefs clauses = tau::get_dnf_wff_clauses(fm);
	// Add bad splitter only to a single disjunct if possible
	if (!clauses.empty()) {
		clauses[0] = tau::build_wff_and(clauses[0], new_uniter_const);
		return tau::build_wff_or(clauses);
	} else return tau::build_wff_and(fm, new_uniter_const);
}

// Return a splitter for the provided non-temporal formula
// We assume the formula is fully normalized by normalizer
template <BAsPack... BAs>
std::pair<tref, splitter_type> nso_tau_splitter(tref fm,
				splitter_type st, tref spec_clause)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	if (st == splitter_type::bad)
		return { tau_bad_splitter(fm), splitter_type::bad };

	// Collect coefficients to produce splitters
	trefs constants = tau::get(fm)
		.select_top(is_child<node, tau::bf_constant>);

	//fm = snf_wff(fm);
	// Collect all occurances of "||" while assuming that fm is in DNF
	trefs clauses = tau::get_dnf_wff_clauses(fm);
	for (tref clause : clauses) {
		// check for equality parts
		trefs eqs = tau::get(clause)
			.select_top(is<node, tau::bf_eq>);
		for (tref eq : eqs) {
			DBG(assert(tau::get(eq)[1][0].is(tau::bf_f));)
			const auto& f = tau::get(eq)[0];
			size_t type_f = f.get_ba_type();
			typename tau::subtree_set free_vars = tau::get_free_vars_from_nso(fm);
			for (tref c : constants) {
				// First check that types match
				size_t type_c = tau::get(c).get_ba_type();
				if (type_f && type_f != type_c) continue;

				// Try to convert f(x,...) = 0 to f(x,...) = 0 && x < c' for some variable x in f
				trefs vars_f = f.select_top(
					is_child<node, tau::variable>);
				for (tref v : vars_f) {
					if (!free_vars.contains(tau::trim(v))) continue;
					tref new_fm = rewriter::replace<node>(
						fm, clause, tau::build_wff_and(clause,
							tau::build_bf_less_equal(v, c)));
					if (is_splitter<BAs...>(fm, new_fm, spec_clause))
						return { new_fm, st };
				}
			}
			if (tref s = good_reverse_splitter_using_function<BAs...>(f, st, clause);
				!tau::subtree_equals(s, clause))
			{
				//TODO: this equiv check should happen in good_reverse_splitter_using_function
				tref new_fm = rewriter::replace<node>(fm, clause, s);
				if (is_splitter<BAs...>(fm, new_fm, spec_clause))
					return { new_fm, st };
			}
		}
		// check for inequality parts
		trefs neqs = tau::get(clause)
				.select_top(is<node, tau::bf_neq>);
		for (tref neq : neqs) {
			DBG(assert(is<node>(tau::trim(neq)[1], tau::bf_f));)
			const auto& f = tau::get(neq)[0];
			size_t type_f = f.get_ba_type();
			for (tref c : constants) {
				// Try to convert f != 0 to f >= c
				// First check that types match
				size_t type_c = tau::get(c).get_ba_type();
				if (type_f && type_f != type_c) continue;
				tref r = tau::build_bf_greater_equal(f, c);
				tref new_fm = rewriter::replace<node>(
					fm, clause, tau::build_wff_and(clause, r));
				new_fm = rewriter::replace<node>(new_fm, neq, tau::_T());
				if (is_splitter<BAs...>(fm, new_fm, spec_clause))
					return { new_fm, st };
			}
			if (tref s = good_splitter_using_function<BAs...>(f, st, clause);
				!tau::subtree_equals(s, clause))
			{
				//TODO: this equiv check should happen in good_splitter_using_function
				tref new_fm = rewriter::replace<node>(fm, clause, s);
				if (is_splitter<BAs...>(fm, new_fm, spec_clause))
					return { new_fm, st };
			}
		}
	}

	// Split disjunction if possible
	trefs m;
	size_t i = 0;
	do {
		tref s = split<BAs...>(fm, tau::wff, false, st, m, i, true);
		if (is_splitter<BAs...>(fm, s, spec_clause)) return { s, st };
	} while (++i < m.size());

	// return bad splitter by conjuncting new uninterpreted constant
	return { tau_bad_splitter<BAs...>(fm), splitter_type::bad };
}

// We assume fm to be normalized in DNF
template <BAsPack... BAs>
tref tau_splitter(tref fm, splitter_type st) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	BOOST_LOG_TRIVIAL(debug) << "(I) Start of tau_splitter";
	// First we decide if we deal with a temporal formula
	if (!tau::has_temp_var(fm)) return nso_tau_splitter(fm, st).first;

	auto splitter_of_clause = [&](tref clause) {
		trefs specs = tau::get_cnf_wff_clauses(clause);
		bool good_splitter = false;
		for (tref spec : specs) {
			bool is_aw = is_child<node>(spec, tau::wff_always);
			auto [splitter, type] = nso_tau_splitter<BAs...>(
					tau::get(spec)[0].first(), st, clause);
			if (type != splitter_type::bad) {
				BOOST_LOG_TRIVIAL(trace) << "Splitter of spec: " << splitter;
				good_splitter = true;
				if (is_aw) splitter = tau::build_wff_always(splitter);
				else splitter = tau::build_wff_sometimes(splitter);
				spec = splitter;
				break;
			}
		}
		if (good_splitter)
			return std::make_pair(tau::build_wff_and(specs), st);
		else return std::make_pair(tau::_F(), splitter_type::bad);
	};

	// Fm is temporal
	trefs clauses = tau::get_dnf_wff_clauses(fm);
	for (int_t i = 0; i < (int_t) clauses.size(); ++i) {
		// First check redundancy between current clause and rest
		bool is_redundant = false;
		for (size_t j = 0; j < clauses.size(); ++j) {
			if ((size_t) i == j) continue;
			if (is_tau_impl<BAs...>(clauses[j], clauses[i])) {
				clauses.erase(clauses.begin() + i);
				--i, is_redundant = true;
				break;
			}
		}
		if (is_redundant) continue;
		auto [splitter, type] = splitter_of_clause(clauses[i]);
		if (type != splitter_type::bad) {
			clauses[i] = splitter;
			return tau::build_wff_or(clauses);
		}
	}
	if (clauses.size() == 1) {
		// Conjunct always part with bad splitter
		// If there is no always part, create one
		tref aw = tau::get(fm).find_top(is_child<node, tau::wff_always>);
		if (aw != nullptr) {
			tref aw_bad_splitter = tau_bad_splitter<BAs...>(
						tau::get(aw)[0].first());
			return rewriter::replace<node>(fm, aw,
				tau::build_wff_always(aw_bad_splitter));
		} else return tau::build_wff_and(
			tau::build_wff_always(tau_bad_splitter<BAs...>()), fm);
	} else {
	// By assumption there is more than one clause and all not redundant
	// Split disjunction taking splitter type into account
	return split<BAs...>(fm, tau::wff, false, st, clauses, 0, false);
	}
}

} // idni::tau_lang namespace
