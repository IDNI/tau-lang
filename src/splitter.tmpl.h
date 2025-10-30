// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "splitter.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "splitter"

namespace idni::tau_lang {

// template <typename... BAs>
// requires BAsPack<BAs...>
// tref split(tref fm, typename node<BAs...>::type fm_type, bool is_cnf,
// 	const splitter_type st, trefs& mem, size_t i, bool check_temps)
// {
// 	using node = tau_lang::node<BAs...>;
// 	using tau = tree<node>;
// 	// First get clauses if not already in mem
// 	const bool is_wff = fm_type == tau::wff;
// 	if (mem.empty()) {
// 		if (is_wff) {
// 			mem = is_cnf
// 				? get_cnf_wff_clauses<node>(fm)
// 				: get_dnf_wff_clauses<node>(fm);
// 		} else {
// 			DBG(assert(fm_type == tau::bf);)
// 			mem = is_cnf
// 				? get_cnf_bf_clauses<node>(fm)
// 				: get_dnf_bf_clauses<node>(fm);
// 		}
// 	}
// 	tref sym = is_wff
// 		? (is_cnf ? tau::_T() : tau::_F())
// 		: (is_cnf ? tau::_1() : tau::_0());
// 	switch (st) {
// 	case splitter_type::upper: {
// 		// Remove exactly one clause
// 		if (mem.size() == 1)
// 			return fm;
// 		if (i < mem.size()) {
// 			// We cannot delete a clause holding a temporary io var
// 			if (check_temps && has_temporary_io_var<node>(mem[i]))
// 				return fm;
// 			return rewriter::replace<node>(fm, mem[i], sym);
// 		}
// 		break;
// 	}
// 	case splitter_type::middle: {
// 		// Remove half of the clause
// 		size_t window = mem.size() / 2;
// 		subtree_map<node, tref> changes;
// 		for (size_t j = 0; j < window; ++j) {
// 			size_t idx = (i + j) % mem.size();
// 			if (check_temps && has_temporary_io_var<node>(mem[idx]))
// 				continue;
// 			changes.emplace(mem[idx], sym);
// 		}
// 		if (!changes.empty())
// 			return rewriter::replace<node>(fm, changes);
// 		break;
// 	}
// 	case splitter_type::lower: {
// 		// Remove all but one clause
// 		if (i < mem.size())
// 			if (check_temps && !has_temporary_io_var<node>(mem[i]))
// 				return mem[i];
// 		break;
// 	}
// 	case splitter_type::bad:
// 		// This case must not happen
// 		assert(false);
// 	}
// 	return fm;
// }

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
				return tau::get(fm).is_term()
					       ? tau::_0()
					       : tau::_F();
			};
			auto cb = [&callback](tref curr_fm) {
				return callback(curr_fm);
			};
			return expression_paths<node>(fm).apply_only_if(
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
			&& !are_nso_equivalent<node>(splitter, fm)) {
			DBG(assert(is_nso_impl<node>(splitter, fm));)
			return true;
		}
	}
	return false;
}

// Find a Boolean function which implies f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_splitter_using_function(tref f, splitter_type st, tref clause,
	tref fm_without_clause, tref original_fm, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	tref func = norm_equation<node>(f);
	if (tau::get(func).equals_T() || tau::get(func).equals_F())
		return clause;
	DBG(assert(is<node>(tau::trim2(func), tau::bf));)
	func = tau::trim2(func);
	// First check if we have more than one disjunct
	tref new_clause = nullptr;
	auto check_splitter = [&](tref s) {
		if (tau::get(s).equals_0()) return false;
		new_clause = rewriter::replace<node>(clause, f, tau::build_bf_neq_0(s));
		if (tau::get(new_clause).equals_F()) return false;
		tref splitter_candidate = tau::build_wff_or(
			fm_without_clause, new_clause);
		if (is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
			return true;
		else return false;
	};
	tref s = split_path<BAs...>(func, st, true, check_splitter);
	if (tau::get(s) != tau::get(func)) return new_clause;
	// Find possible coefficient in each disjunct of f
	tref curr_path = nullptr;
	auto remove_path = [&](tref path) {
		curr_path = path;
		return tau::_0();
	};
	auto split_coeff = [&](tref curr_f) {
		tref coeff = tau::get(curr_path)
			.find_top(is<node, tau::ba_constant>);
		if (!coeff) return false;
		DBG(assert(is<node>(coeff, tau::ba_constant));)
		const tau& scoeff_t = splitter<BAs...>(tau::get(coeff));
		if (scoeff_t.equals_0()) return false;
		tref scoeff = scoeff_t.first();
		if (tau::get(scoeff) != tau::get(coeff)) {
			tref path = rewriter::replace<node>(curr_path, coeff, scoeff);
			curr_f = tau::build_bf_or(curr_f, path);
			new_clause = rewriter::replace<node>(clause, f,
				tau::build_bf_neq_0(curr_f));
			if (tau::get(new_clause).equals_F()) return false;
			tref splitter_candidate =
				tau::build_wff_or(fm_without_clause, new_clause);
			if(is_splitter<BAs...>(original_fm, splitter_candidate, spec_clause))
				return true;
		}
		return false;
	};
	tref nfunc = expression_paths<node>(func).apply_only_if(remove_path, split_coeff);
	if (tau::get(nfunc) != tau::get(func))
		return new_clause;
	return clause;
}

// Find a Boolean function which is implied by f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
	tref clause, tref fm_without_clause, tref original_fm, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	tref func = norm_equation<node>(f);
	if (tau::get(func).equals_T() || tau::get(func).equals_F())
		return clause;
	func = tau::trim2(func);
	DBG(assert(is<node>(func, tau::bf));)

	tref curr_path = nullptr;
	tref new_clause = nullptr;
	auto remove_path = [&](tref path) {
		curr_path = path;
		return tau::_0();
	};
	auto reverse_split_coeff = [&st](tref coeff) {
		// We need to split the negated constant
		coeff = tau::build_bf_neg(coeff);
		DBG(assert(is<node>(tau::trim(coeff), tau::ba_constant));)
		tref s = splitter<BAs...>(tau::get(coeff)[0], st).get();
		// Negate again to get the reversed splitter
		return tau::build_bf_neg(s);
	};
	auto remove_literal = [&](tref curr_f) {
		// Remove single CNF clause form curr_path
		// Then check if we have a splitter
		trefs lits = get_cnf_bf_clauses<node>(curr_path);
		for (tref& lit : lits) {
			tref tmp = lit;
			lit = is_child<node>(lit, tau::ba_constant)
				      ? reverse_split_coeff(lit)
				      : tau::_1();
			tref new_path = tau::build_bf_and(lits);
			curr_f = tau::build_bf_or(curr_f, new_path);
			if (tau::get(curr_f).equals_1()) return false;
			new_clause = rewriter::replace<node>(
				clause, f, tau::build_bf_eq_0(curr_f));
			// Check if splitting resulted in false
			if (tau::get(new_clause).equals_F()) {
				lit = tmp;
				continue;
			}
			tref splitter_candidate =
				tau::build_wff_or(fm_without_clause, new_clause);
			if(is_splitter<BAs...>(
				original_fm, splitter_candidate, spec_clause))
				return true;
			lit = tmp;
		}
		return false;
	};
	tref nfunc = expression_paths<node>(func).apply_only_if(
		remove_path, remove_literal);
	if (tau::get(nfunc) != tau::get(func))
		return new_clause;
	return clause;
}

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	tref new_uniter_const = tau::build_bf_neq_0(
		get_new_uninterpreted_constant<node>(fm, "split", get_ba_type_id<node>(tau_type<node>())));
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
		.select_top(is_child<node, tau::ba_constant>);

	tref curr_clause = nullptr;
	auto remove_clause = [&curr_clause](tref clause) {
		curr_clause = clause;
		return tau::_F();
	};
	auto split_equality = [&](tref curr_fm) {
		// check for equality parts
		trefs eqs = tau::get(curr_clause).select_top(
			is_child<node, tau::bf_eq>);
		for (tref eq : eqs) {
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
								tau::build_bf_lteq(
									tau::get(tau::bf, v), c));
						if (tau::get(new_clause).equals_F())
							continue;
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
				eq, st, curr_clause, curr_fm, fm, spec_clause);
					tau::get(s) != tau::get(curr_clause)) {
				curr_clause = s;
				return true;
			}
		}
		return false;
	};
	tref splitter = expression_paths<node>(fm).apply_only_if(
		remove_clause, split_equality);
	// Splitter found
	if (tau::get(fm) != tau::get(splitter))
		return {tau::build_wff_or(splitter, curr_clause), st};

	auto split_inequality = [&](tref curr_fm) {
		// check for inequality parts
		trefs neqs = tau::get(curr_clause).select_top(
			is_child<node, tau::bf_neq>);
		for (tref neq : neqs) {

			size_t type_f = find_ba_type<node>(neq);
			// Check that term is typed
			if (type_f > 0) {
				for (tref c : constants) {
					// Try to convert f != 0 to f >= c
					// First check that types match
					size_t type_c = find_ba_type<node>(c);
					if (type_f != type_c) continue;
					tref norm_neq = norm_equation<node>(neq);
					if (tau::get(norm_neq).equals_T() ||
						tau::get(norm_neq).equals_F())
						continue;
					DBG(assert(tau::get(norm_neq)[0][1].child_is(tau::bf_f));)
					tref r = tau::build_bf_gteq(tau::trim2(norm_neq), c);
					tref new_clause = tau::build_wff_and(
						rewriter::replace<node>(curr_clause,
							neq, tau::_T()), r);
					if (tau::get(new_clause).equals_F())
						continue;
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
				neq, st, curr_clause, curr_fm, fm, spec_clause);
				tau::get(s) != tau::get(curr_clause)) {
				curr_clause = s;
				return true;
			}
		}
		return false;
	};
	curr_clause = nullptr;
	splitter = expression_paths<node>(fm).apply_only_if(
		remove_clause, split_inequality);
	// Splitter found
	if (tau::get(fm) != tau::get(splitter))
		return {tau::build_wff_or(splitter, curr_clause), st};

	// Split disjunction if possible
	auto check_splitter = [&](tref s) {
		if (tau::get(fm) != tau::get(s) &&
			is_splitter<BAs...>(fm, s, spec_clause)) return true;
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

	// Fm is temporal, therefore the temporal layer is in DNF
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
		auto accept = [](tref){return true;};
		return split_path<BAs...>(fm, st, false, accept);
	}
}

// Splitter function for a nso tau::ba_constant node holding a BA constant
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& splitter(const tree<node<BAs...>>& t,
	splitter_type st)
{
	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& t) -> std::variant<BAs...> {
		return splitter(t, st);
	};
	DBG(assert(t.is_ba_constant());)
	return tree<node<BAs...>>::get(
		tree<node<BAs...>>::build_bf_ba_constant(
			std::visit(_splitter, t.get_ba_constant()),
			t.get_ba_type()));
}

} // namespace idni::tau_lang
