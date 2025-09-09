// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "splitter.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "splitter"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
tref split(tref fm, typename node<BAs...>::type fm_type, bool is_cnf,
	const splitter_type st, trefs& mem, size_t i, bool check_temps)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	// First get clauses if not already in mem
	const bool is_wff = fm_type == tau::wff;
	if (mem.empty()) {
		if (is_wff) {
			mem = is_cnf
				? get_cnf_wff_clauses<node>(fm)
				: get_dnf_wff_clauses<node>(fm);
		} else {
			DBG(assert(fm_type == tau::bf);)
			mem = is_cnf
				? get_cnf_bf_clauses<node>(fm)
				: get_dnf_bf_clauses<node>(fm);
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
			if (check_temps && has_temporary_io_var<node>(mem[i]))
				return fm;
			return rewriter::replace<node>(fm, mem[i], sym);
		}
		break;
	}
	case splitter_type::middle: {
		// Remove half of the clause
		size_t window = mem.size() / 2;
		subtree_map<node, tref> changes;
		for (size_t j = 0; j < window; ++j) {
			size_t idx = (i + j) % mem.size();
			if (check_temps && has_temporary_io_var<node>(mem[idx]))
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
			if (check_temps && !has_temporary_io_var<node>(mem[i]))
				return mem[i];
		break;
	}
	case splitter_type::bad:
		// This case must not happen
		assert(false);
	}
	return fm;
}

template<typename ... BAs> requires BAsPack<BAs...>
tref split_path(tref fm, const splitter_type st, bool check_temps, const auto& callback) {
	using node = node<BAs...>;
	using tau = tree<node>;
	switch (st) {
		case splitter_type::upper: {
			// Remove exactly one clause
			auto remove_clause = [&](tref clause) {
				// Do not delete temporary streams
				if (check_temps && has_temporary_io_var<node>(clause))
					return clause;
				return tau::_F();
			};
			auto cb = [&callback](tref curr_fm) {
				return callback(curr_fm);
			};
			return expression_paths<node>(fm).apply_with_undo(
				remove_clause, cb);
		}
		case splitter_type::middle:
		case splitter_type::lower:
		case splitter_type::bad:
			// TODO: bring back middle and lower splitter? Maybe just upper splitter is enough...
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
template <typename... BAs>
requires BAsPack<BAs...>
bool is_splitter(tref fm, tref splitter, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	if (spec_clause) {
		// We are dealing with a temporal formula
		DBG(assert(is_tau_impl<node>(splitter, fm));)
		tref new_spec_clause = normalize_with_temp_simp<node>(
			rewriter::replace<node>(spec_clause, fm, splitter));
		if (is_tau_formula_sat<node>(new_spec_clause)) {
			if (!are_tau_equivalent<node>(new_spec_clause, spec_clause))
				return true;
		}
	} else {
		// We are dealing with a non-temporal formula
		if (is_non_temp_nso_satisfiable<node>(splitter)
			&& !are_nso_equivalent<node>(splitter, fm))
		{
			DBG(assert(is_nso_impl<node>(splitter, fm));)
			return true;
		}
	}
	return false;
}

// TODO: split_path and remove get_leave
// Find a Boolean function which implies f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_splitter_using_function(tref f, splitter_type st, tref original_clause,
	tref original_fm, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	DBG(assert(is<node>(f, tau::bf));)
	// First check if we have more then one disjunct
	trefs m;
	size_t i = 0;
	do {
		tref s = split<BAs...>(f, tau::bf, false, st, m, i, true);
		if (tau::get(s) != tau::get(f)) {
			tref new_clause = rewriter::replace<node>(original_clause, f, s);
			tref splitter_candidate =
				rewriter::replace<node>(original_fm, original_clause, new_clause);
			if(is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
				return new_clause;
		}
	} while (++i < m.size());

	// Find possible coefficient in each disjunct of f
	trefs clauses = get_leaves<node>(f, tau::bf_or);
	// In case f is just a single clause
	if (clauses.empty()) clauses.push_back(f);
	for (tref clause : clauses) {
		tref coeff = tau::get(clause)
			.find_top(is<node, tau::bf_constant>);
		if (!coeff) continue;
		DBG(assert(is<node>(coeff, tau::bf_constant));)
		tref s = splitter<BAs...>(tau::get(coeff)).get();
		if (tau::get(s) != tau::get(coeff)) {
			tref new_clause = rewriter::replace<node>(
					original_clause, coeff, tau::trim(s));
			tref splitter_candidate =
				rewriter::replace<node>(original_fm, original_clause, new_clause);
			if(is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
				return new_clause;
		}
	}
	return original_clause;
}

// TODO: remove direct cnf conversion, remove get_leaves
// Find a Boolean function which is implied by f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
	tref original_clause, tref original_fm, tref spec_clause)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	DBG(assert(is<node>(f, tau::bf));)
	// Convert Boolean function to CNF
	tref f_cnf = to_cnf<node, false>(f);

	// Try to remove a conjunct to produce splitter
	trefs m;
	size_t i = 0;
	do {
		tref s = split<BAs...>(f_cnf, tau::bf, true, st, m, i, true);
		if (tau::get(s) != tau::get(f_cnf)) {
			tref new_clause = rewriter::replace<node>(original_fm, f, s);
			tref splitter_candidate =
				rewriter::replace<node>(original_fm, original_clause, new_clause);
			if(is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
				return new_clause;
		}
	} while (++i < m.size());

	// Try to split coefficient in each conjunct of f
	trefs clauses = get_leaves<node>(f_cnf, tau::bf_and);
	for (tref clause : clauses) {
		tref coeff = tau::get(clause).find_top(
			is_child<node, tau::bf_constant>);
		if (!coeff) continue;
		// If coefficient exists try to split the negation
		tref neg_coeff = push_negation_in<node, false>(
			tau::build_bf_neg(coeff));
		DBG(assert(is<node>(
				tau::trim(neg_coeff), tau::bf_constant));)
		const auto& s = splitter<BAs...>(tau::get(tau::trim(neg_coeff)), st);
		// Negating s results in a reversed splitter for s
		tref r = push_negation_in<node, false>(tau::build_bf_neg(s.get()));
		if (tau::get(r) != tau::get(coeff)) {
			tref new_clause = rewriter::replace<node>(
					original_fm, coeff, r);
			tref splitter_candidate =
				rewriter::replace<node>(original_fm, original_clause, new_clause);
			if(is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
				return new_clause;
		}
	}
	return original_clause;
}

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	tref new_uniter_const = tau::build_bf_neq_0(
		get_new_uninterpreted_constant<node>(fm, "split", get_ba_type_id<node>("tau")));
	// Find bottom wff_or and conjunct with left child
	bool added = false;
	auto f = [&added, &new_uniter_const](tref n) {
		if (!added && tau::get(n).is(tau::wff_or)) {
			return tau::trim(tau::build_wff_or(
				tau::build_wff_and(tau::get(n).first(), new_uniter_const),
				tau::get(n).second()));
		}
		return n;
	};
	auto visit = [&added](tref n) {
		if (added) return false;
		else return visit_wff<node>(n);
	};
	tref split_fm = post_order<node>(fm).apply_unique(f, visit);
	if (tau::subtree_equals(split_fm, fm))
		return tau::build_wff_and(fm, new_uniter_const);
	else return split_fm;
}

// Return a splitter for the provided non-temporal formula
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
std::pair<tref, splitter_type> nso_tau_splitter(tref fm,
				splitter_type st, tref spec_clause)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	if (st == splitter_type::bad)
		return { tau_bad_splitter<BAs...>(fm), splitter_type::bad };

	// Collect coefficients to produce splitters
	trefs constants = tau::get(fm)
		.select_top(is_child<node, tau::bf_constant>);

	tref curr_clause = nullptr;
	auto remove_clause = [&curr_clause](tref clause) {
		curr_clause = clause;
		return tau::_F();
	};
	auto split_equality = [&](tref curr_fm) {
		// check for equality parts
		trefs eqs = tau::get(curr_clause).select_top(
			is<node, tau::bf_eq>);
		for (tref eq : eqs) {
			eq = norm_trimmed_equation<node>(eq);
			DBG(assert(tau::get(eq)[1][0].is(tau::bf_f));)
			size_t type_f = find_ba_type<node>(eq);
			// Check that term is typed
			if (type_f > 0) {
				const trefs& vars_f = get_free_vars<node>(eq);
				for (tref c : constants) {
					// First check that types match
					size_t type_c = find_ba_type<node>(c);
					if (type_f != type_c) continue;
					// Try to convert f(x,...) = 0 to
					// f(x,...) = 0 && x < c'
					// for some free variable x in f
					for (tref v : vars_f) {
						tref new_clause =
							tau::build_wff_and(curr_clause,
								tau::build_bf_lteq(v, c));
						tref new_fm = tau::build_wff_or(
							curr_fm, new_clause);
						if (is_splitter<BAs...>(
							fm, new_fm, spec_clause)) {
							curr_clause = new_clause;
							return true;
						}
					}
				}
			}
			if (tref s = good_reverse_splitter_using_function<BAs...>(
				tau::trim(eq), st, curr_clause, fm, spec_clause);
					tau::get(s) != tau::get(curr_clause)) {
				curr_clause = s;
				return true;
			}
		}
		return false;
	};
	tref splitter = expression_paths<node>(fm).apply_with_undo(
		remove_clause, split_equality);
	// Splitter found
	if (tau::get(fm) != tau::get(splitter))
		return {tau::build_wff_or(splitter, curr_clause), st};

	auto split_inequality = [&](tref curr_fm) {
		// check for inequality parts
		trefs neqs = tau::get(curr_clause).select_top(
			is<node, tau::bf_neq>);
		for (tref neq : neqs) {
			neq = norm_trimmed_equation<node>(neq);
			DBG(assert(tau::get(neq)[1].child_is(tau::bf_f));)
			size_t type_f = find_ba_type<node>(neq);
			// Check that term is typed
			if (type_f > 0) {
				for (tref c : constants) {
					// Try to convert f != 0 to f >= c
					// First check that types match
					size_t type_c = find_ba_type<node>(c);
					if (type_f != type_c) continue;
					tref r = tau::build_bf_gteq(tau::trim(neq), c);
					tref new_clause = tau::build_wff_and(
						rewriter::replace<node>(curr_clause,
							neq, tau::_T_trimmed()), r);
					tref new_fm = tau::build_wff_or(
							curr_fm, new_clause);
					if (is_splitter<BAs...>(fm, new_fm,
						spec_clause)) {
						curr_clause = new_clause;
						return true;
					}
				}
			}
			if (tref s = good_splitter_using_function<BAs...>(
				tau::trim(neq), st, curr_clause, fm, spec_clause);
				tau::get(s) != tau::get(curr_clause)) {
				curr_clause = s;
				return true;
			}
		}
		return false;
	};
	curr_clause = nullptr;
	splitter = expression_paths<node>(fm).apply_with_undo(
		remove_clause, split_inequality);
	// Splitter found
	if (tau::get(fm) != tau::get(splitter))
		return {tau::build_wff_or(splitter, curr_clause), st};

	// Split disjunction if possible
	auto check_splitter = [&](tref s) {
		if (is_splitter<BAs...>(fm, s, spec_clause)) return true;
		else return false;
	};
	splitter = split_path<BAs...>(fm, st, true, check_splitter);
	if (tau::get(fm) != tau::get(splitter)) return {splitter, st};
	// return bad splitter by conjuncting new uninterpreted constant
	return { tau_bad_splitter<BAs...>(fm), splitter_type::bad };
}

// We assume fm to be normalized
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_splitter(tref fm, splitter_type st) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	LOG_DEBUG << "-- Start of tau_splitter for " << LOG_FM(fm);
	// First we decide if we deal with a temporal formula
	if (!has_temp_var<node>(fm))
		return nso_tau_splitter<BAs...>(fm, st).first;

	auto splitter_of_clause = [&](tref clause) {
		trefs specs = get_cnf_wff_clauses<node>(clause);
		bool good_splitter = false;
		for (tref& spec : specs) {
			bool is_aw = is_child<node>(spec, tau::wff_always);
			auto [splitter, type] = nso_tau_splitter<BAs...>(
					tau::get(spec)[0].first(), st, clause);
			if (type != splitter_type::bad) {
				LOG_TRACE << "Splitter of spec: "
							<< LOG_FM(splitter);
				good_splitter = true;
				splitter = is_aw
					? tau::build_wff_always(splitter)
					: tau::build_wff_sometimes(splitter);
				// Replace the current spec with the good splitter
				spec = splitter;
				break;
			}
		}
		if (good_splitter)
			return std::make_pair(tau::build_wff_and(specs), st);
		else return std::make_pair(tau::_F(), splitter_type::bad);
	};

	// Fm is temporal
	trefs clauses = get_dnf_wff_clauses<node>(fm);
	for (int_t i = 0; i < (int_t) clauses.size(); ++i) {
		// First check redundancy between current clause and rest
		bool is_redundant = false;
		for (size_t j = 0; j < clauses.size(); ++j) {
			if ((size_t) i == j) continue;
			if (is_tau_impl<node>(clauses[j], clauses[i])) {
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

} // namespace idni::tau_lang
