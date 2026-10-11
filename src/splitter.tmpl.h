// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "splitter.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "splitter"

namespace idni::tau_lang {

// Shared clause-removal engine behind the splitter searches: drops one
// DNF clause of fm at a time (replacing it by 0 for terms, F for
// formulas; clauses with temporary io streams are kept when check_temps)
// and offers each reduced formula to callback. Returns the first reduced
// formula callback accepts, fm unchanged when it accepts none, or nullptr
// when fm has no DNF clause.
template<typename ... BAs> requires BAsPack<BAs...>
tref split_path(tref fm, bool check_temps, const auto& callback) {
	using node = node<BAs...>;
	using tau = tree<node>;
	auto remove_clause = [&](tref clause) {
		// Do not delete temporary streams
		if (check_temps && has_temporary_io_var<node>(clause))
			return clause;
		return tau::get(fm).is_term()
			       ? tau::_0(find_ba_type<node>(fm))
			       : tau::_F();
	};
	return expression_paths<node>(fm).apply_only_if(remove_clause, callback);
}

// Splits the BA constant held by t: delegates to the element's own BA
// splitter and rewraps the result as a bf ba_constant tree of t's type.
// t must be a ba_constant node; an error of the BA's splitter is returned.
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> tau_splitter(const tree<node<BAs...>>& t,
	splitter_type st = splitter_type::upper)
{
	using node = node<BAs...>;
	result<tref> r;
	DBG(assert(t.is_ba_constant());)
	TAU_TRY(auto v, node::ba::splitter(t.get_ba_constant(), st));
	return r.with_value(
		tree<node>::build_bf_ba_constant(v, t.get_ba_type()));
}

// If checking a temporal Tau formula F, we split a single DNF clause.
// In order to check if the split clause yields a splitter for F, we have that
// "fm" holds the clause, "splitter" holds the splitter of the clause and
// "spec" holds the original temporal Tau formula
// If we check a non-temporal Tau formula, it suffices to place it in "fm" and
// the proposed splitter in "splitter".
// Returns true when the candidate is satisfiable and not equivalent to what
// it replaces; false otherwise, also when the rewritten clause does not
// normalize. A solver error travels in the report.
template <typename... BAs>
requires BAsPack<BAs...>
result<bool> is_splitter(tref fm, tref splitter, tref spec_clause = nullptr) {
	using node = tau_lang::node<BAs...>;
	result<bool> r;
	if (spec_clause) {
		// We are dealing with a temporal formula
		// NOTE: temporal upper splitters do not necessarily imply fm; the
		// implication check below (sat + non-equivalence) is the correct gate.
		auto new_spec_clause_r = normalize_with_temp_simp<node>(
			rewriter::replace<node>(spec_clause, fm, splitter));
		// No value when the definitions in the clause do not settle;
		// no normalized clause means no splitter to report.
		if (!new_spec_clause_r.has_value()) return r.with_value(false);
		tref new_spec_clause = new_spec_clause_r.value();
		auto sat = r.merge_take(is_tau_formula_sat<node>(new_spec_clause));
		if (sat.has_value() && sat.value()) {
			auto eq = r.merge_take(
				are_tau_equivalent<node>(new_spec_clause, spec_clause));
			if (!(eq.has_value() && eq.value()))
				return r.with_value(true);
		}
	} else {
		// We are dealing with a non-temporal formula
		auto sat = r.merge_take(is_non_temp_nso_satisfiable<node>(splitter));
		if (sat.has_value() && sat.value()
			&& !are_nso_equivalent<node>(splitter, fm)) {
			DBG(auto impl = is_nso_impl<node>(splitter, fm);
				assert(impl.has_value() && impl.value());)
			return r.with_value(true);
		}
	}
	return r.with_value(false);
}

// Given an inequality literal f (g != 0) of clause, searches for a
// strictly smaller function implying g — by dropping disjuncts of g, or
// by splitting a coefficient inside one — such that swapping it into
// clause yields a splitter of original_fm. Returns the rewritten clause,
// or clause unchanged when no candidate passes is_splitter; a solver error
// stops the search and is returned.
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> good_splitter_using_function(tref f, tref clause,
	tref fm_without_clause, tref original_fm, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	result<tref> r;
	tref func = norm_equation<node>(f);
	if (tau::get(func).equals_T() || tau::get(func).equals_F())
		return r.with_value(clause);
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
		auto splits = r.merge_take(is_splitter<BAs...>(
			original_fm, splitter_candidate, spec_clause));
		return splits.has_value() && splits.value();
	};
	tref s = split_path<BAs...>(func, true, check_splitter);
	if (tau::get(s) != tau::get(func)) return r.with_value(new_clause);
	// Find possible coefficient in each disjunct of f
	tref curr_path = nullptr;
	auto remove_path = [&](tref path) {
		curr_path = path;
		return tau::_0(find_ba_type<node>(path));
	};
	auto split_coeff = [&](tref curr_f) {
		if (r.has_error()) return false;
		tref coeff = tau::get(curr_path)
			.find_top(is<node, tau::ba_constant>);
		if (!coeff) return false;
		DBG(assert(is<node>(coeff, tau::ba_constant));)
		auto scoeff_opt = r.merge_take(
			tau_splitter<BAs...>(tau::get(coeff)));
		if (!scoeff_opt.has_value()) return false;
		const tau& scoeff_t = tau::get(*scoeff_opt);
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
			auto splits = r.merge_take(is_splitter<BAs...>(
				original_fm, splitter_candidate, spec_clause));
			if (splits.has_value() && splits.value())
				return true;
		}
		return false;
	};
	tref nfunc = expression_paths<node>(func).apply_only_if(remove_path, split_coeff);
	if (r.has_error()) return r;
	if (tau::get(nfunc) != tau::get(func))
		return r.with_value(new_clause);
	return r.with_value(clause);
}

// Dual of good_splitter_using_function for an equality literal f
// (g = 0): searches for a function implied by g — weakening one CNF
// literal of a path to 1, or reverse-splitting a negated coefficient —
// such that swapping it into clause yields a splitter of original_fm.
// Returns the rewritten clause, or clause unchanged on failure; a solver
// error stops the search and is returned.
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> good_reverse_splitter_using_function(tref f, splitter_type st,
	tref clause, tref fm_without_clause, tref original_fm, tref spec_clause) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	result<tref> r;
	tref func = norm_equation<node>(f);
	if (tau::get(func).equals_T() || tau::get(func).equals_F())
		return r.with_value(clause);
	func = tau::trim2(func);
	DBG(assert(is<node>(func, tau::bf));)

	tref curr_path = nullptr;
	tref new_clause = nullptr;
	auto remove_path = [&](tref path) {
		curr_path = path;
		return tau::_0(find_ba_type<node>(path));
	};
	auto reverse_split_coeff = [&](tref coeff) {
		// We need to split the negated constant
		coeff = tau::build_bf_neg(coeff);
		DBG(assert(is<node>(tau::trim(coeff), tau::ba_constant));)
		auto s_opt = r.merge_take(
			tau_splitter<BAs...>(tau::get(coeff)[0], st));
		if (!s_opt.has_value()) return coeff;
		// Negate again to get the reversed splitter
		return tau::build_bf_neg(*s_opt);
	};
	auto remove_literal = [&](tref curr_f) {
		if (r.has_error()) return false;
		// Remove single CNF clause form curr_path
		// Then check if we have a splitter
		trefs lits = get_cnf_bf_clauses<node>(curr_path);
		for (tref& lit : lits) {
			tref tmp = lit;
			lit = is_child<node>(lit, tau::ba_constant)
				      ? reverse_split_coeff(lit)
				      : tau::_1(find_ba_type<node>(lit));
			if (r.has_error()) return false;
			tref new_path = tau::build_bf_and(lits, find_ba_type<node>(lit));
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
			auto splits = r.merge_take(is_splitter<BAs...>(
				original_fm, splitter_candidate, spec_clause));
			if (splits.has_value() && splits.value())
				return true;
			lit = tmp;
		}
		return false;
	};
	tref nfunc = expression_paths<node>(func).apply_only_if(
		remove_path, remove_literal);
	if (r.has_error()) return r;
	if (tau::get(nfunc) != tau::get(func))
		return r.with_value(new_clause);
	return r.with_value(clause);
}

/** @internal @copydoc tau_bad_splitter @endinternal */
// Return a bad splitter for the provided formula: conjuncts a fresh
// uninterpreted "split" constant != 0 into the left disjunct of the
// first wff_or met in post-order (or into fm as a whole when it has no
// disjunction),
// so the result strictly implies fm but is only trivially smaller.
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
			added = true;  // Mark that we've added the constraint
			return tau::trim(tau::build_wff_or(
				tau::build_wff_and(tau::get(n).first(), new_uniter_const),
				tau::get(n).second()));
		}
		return n;
	};
	auto visit = [&added](tref n) {
		if (added) return false;
		else return is_formula<node>(n);
	};
	tref split_fm = post_order<node>(fm).apply_unique(f, visit);
	if (tau::subtree_equals(split_fm, fm))
		return tau::build_wff_and(fm, new_uniter_const);
	else return split_fm;
}

// Return a splitter for the provided non-temporal formula, paired with its
// type: st when a real splitter is found (from an equality, then an
// inequality, then by dropping a disjunct), splitter_type::bad for the
// fallback. spec_clause is the temporal clause fm comes from, or nullptr.
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
result<std::pair<tref, splitter_type>> nso_tau_splitter(tref fm,
				splitter_type st, tref spec_clause = nullptr)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	result<std::pair<tref, splitter_type>> r;
	if (st == splitter_type::bad)
		return r.with_value(std::make_pair(
			tau_bad_splitter<BAs...>(fm), splitter_type::bad));

	// Collect coefficients to produce splitters
	trefs constants = tau::get(fm)
		.select_top(is_child<node, tau::ba_constant>);

	tref curr_clause = nullptr;
	auto remove_clause = [&curr_clause](tref clause) {
		curr_clause = clause;
		return tau::_F();
	};
	auto split_equality = [&](tref curr_fm) {
		if (r.has_error()) return false;
		// check for equality parts
		trefs eqs = tau::get(curr_clause).select_top(
			is_child<node, tau::bf_eq>);
		for (tref eq : eqs) {
			if (r.has_error()) return false;
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
						auto splits = r.merge_take(
							is_splitter<BAs...>(
								fm, new_fm,
								spec_clause));
						if (r.has_error()) return false;
						if (splits.has_value()
							&& splits.value()) {
							curr_clause = new_clause;
							return true;
						}
					}
				}
			}
			auto s = r.merge_take(
				good_reverse_splitter_using_function<BAs...>(
					eq, st, curr_clause, curr_fm, fm,
					spec_clause));
			if (r.has_error()) return false;
			if (s.has_value()
				&& tau::get(*s) != tau::get(curr_clause)) {
				curr_clause = *s;
				return true;
			}
		}
		return false;
	};
	tref splitter = expression_paths<node>(fm).apply_only_if(
		remove_clause, split_equality);
	if (r.has_error()) return r;
	// Splitter found (guard against null when fm has no expression paths)
	if (splitter && tau::get(fm) != tau::get(splitter))
		return r.with_value(std::make_pair(
			tau::build_wff_or(splitter, curr_clause), st));

	auto split_inequality = [&](tref curr_fm) {
		if (r.has_error()) return false;
		// check for inequality parts
		trefs neqs = tau::get(curr_clause).select_top(
			is_child<node, tau::bf_neq>);
		for (tref neq : neqs) {
			if (r.has_error()) return false;

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
					tref gteq = tau::build_bf_gteq(tau::trim2(norm_neq), c);
					tref new_clause = tau::build_wff_and(
						rewriter::replace<node>(curr_clause,
							neq, tau::_T()), gteq);
					if (tau::get(new_clause).equals_F())
						continue;
					tref new_fm = tau::build_wff_or(
							curr_fm, new_clause);
					auto splits = r.merge_take(
						is_splitter<BAs...>(fm,
							new_fm, spec_clause));
					if (r.has_error()) return false;
					if (splits.has_value()
						&& splits.value()) {
						curr_clause = new_clause;
						return true;
					}
				}
			}
			auto s = r.merge_take(
				good_splitter_using_function<BAs...>(
					neq, curr_clause, curr_fm, fm,
					spec_clause));
			if (r.has_error()) return false;
			if (s.has_value()
				&& tau::get(*s) != tau::get(curr_clause)) {
				curr_clause = *s;
				return true;
			}
		}
		return false;
	};
	curr_clause = nullptr;
	splitter = expression_paths<node>(fm).apply_only_if(
		remove_clause, split_inequality);
	if (r.has_error()) return r;
	// Splitter found (guard against null when fm has no expression paths)
	if (splitter && tau::get(fm) != tau::get(splitter))
		return r.with_value(std::make_pair(
			tau::build_wff_or(splitter, curr_clause), st));

	// Split disjunction if possible
	auto check_splitter = [&](tref s) {
		if (r.has_error()) return false;
		if (tau::get(fm) == tau::get(s)) return false;
		auto splits = r.merge_take(
			is_splitter<BAs...>(fm, s, spec_clause));
		if (r.has_error()) return false;
		return splits.has_value() && splits.value();
	};
	splitter = split_path<BAs...>(fm, true, check_splitter);
	if (r.has_error()) return r;
	// Guard against null: split_path returns nullptr when fm has no paths
	// (e.g. F, which has an empty DNF). Falls through to bad splitter.
	if (splitter && tau::get(fm) != tau::get(splitter))
		return r.with_value(std::make_pair(splitter, st));
	// return bad splitter by conjuncting new uninterpreted constant
	return r.with_value(std::make_pair(
		tau_bad_splitter<BAs...>(fm), splitter_type::bad));
}

/** @internal @copydoc tau_splitter(tref, splitter_type) @endinternal */
// Entry point: returns a formula strictly implying fm that is still
// satisfiable and not equivalent to it. Non-temporal formulas go to
// nso_tau_splitter; temporal ones are split per DNF clause, falling back
// to a bad splitter on the always part when no clause splits.
// We assume fm to be normalized
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> tau_splitter(tref fm, splitter_type st) {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	result<tref> r;
	LOG_DEBUG << "-- Start of tau_splitter for " << LOG_FM(fm);
	// First we decide if we deal with a temporal formula
	if (!has_temp_var<node>(fm)) {
		TAU_TRY(auto nso, nso_tau_splitter<BAs...>(fm, st));
		return r.with_value(nso.first);
	}

	auto splitter_of_clause = [&](tref clause)
		-> result<std::pair<tref, splitter_type>> {
		result<std::pair<tref, splitter_type>> cr;
		trefs specs = get_cnf_wff_clauses<node>(clause);
		bool good_splitter = false;
		for (tref& spec : specs) {
			bool is_aw = is_child<node>(spec, tau::wff_always);
			// Only always/sometimes conjuncts carry an inner
			// wff at [0].first(); a bare atomic conjunct (e.g.
			// `x = 0` in a clause that has_temp_var through another
			// conjunct) would have its left BF operand spliced back
			// as a formula. Skip anything not temporal-wrapped.
			if (!is_aw && !is_child<node>(spec, tau::wff_sometimes))
				continue;
			auto nso = cr.merge_take(nso_tau_splitter<BAs...>(
					tau::get(spec)[0].first(), st, clause));
			if (!nso.has_value()) return cr;
			auto [splitter, type] = *nso;
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
			return cr.with_value(
				std::make_pair(tau::build_wff_and(specs), st));
		else return cr.with_value(
			std::make_pair(tau::_F(), splitter_type::bad));
	};

	// Fm is temporal, therefore the temporal layer is in DNF
	trefs clauses = get_dnf_wff_clauses<node>(fm);
	for (int_t i = 0; i < (int_t) clauses.size(); ++i) {
		// i stays signed for the erase offset below, so the clause
		// subscripts are converted once here.
		const size_t i_pos = static_cast<size_t>(i);
		// First check redundancy between current clause and rest
		bool is_redundant = false;
		for (size_t j = 0; j < clauses.size(); ++j) {
			if ((size_t) i == j) continue;
			auto impl = is_tau_impl<node>(clauses[j], clauses[i_pos]);
			if (!impl.has_value()) {
				// An undecided implication keeps the clause.
				auto sc = r.open("rejected candidate");
				r.info("whether this clause implies another one is "
					"undecided",
					{{label::value, truncate_for_message(
						TAU_TO_STR(clauses[i_pos]))}});
				report cand = std::move(impl).report();
				cand.demote_errors_to_warnings();
				r.append(std::move(cand));
				continue;
			}
			const bool implied = impl.value();
			r.merge(std::move(impl));
			if (implied) {
				clauses.erase(clauses.begin() + i);
				--i, is_redundant = true;
				break;
			}
		}
		if (is_redundant) continue;
		auto clause = r.merge_take(splitter_of_clause(clauses[i_pos]));
		if (!clause.has_value()) return r;
		auto [splitter, type] = *clause;
		if (type != splitter_type::bad) {
			clauses[i_pos] = splitter;
			return r.with_value(tau::build_wff_or(clauses));
		}
	}
	if (clauses.size() == 1) {
		// Conjunct always part with bad splitter
		// If there is no always part, create one
		tref aw = tau::get(fm).find_top(is_child<node, tau::wff_always>);
		if (aw != nullptr) {
			tref aw_bad_splitter = tau_bad_splitter<BAs...>(
						tau::get(aw)[0].first());
			return r.with_value(rewriter::replace<node>(fm, aw,
				tau::build_wff_always(aw_bad_splitter)));
		} else return r.with_value(tau::build_wff_and(
			tau::build_wff_always(tau_bad_splitter<BAs...>()), fm));
	}
	// No clause left implies another single one, but the others together
	// may still imply it; dropping a clause they do not imply is the
	// splitter for every type, bad included.
	for (size_t k = clauses.size(); k-- > 0;) {
		trefs others = clauses;
		others.erase(others.begin() + static_cast<int_t>(k));
		tref rest = tau::build_wff_or(others);
		auto implied = is_tau_impl<node>(clauses[k], rest);
		if (implied.has_value()) {
			const bool keep = implied.value();
			r.merge(std::move(implied));
			if (!keep) return r.with_value(rest);
			continue;
		}
		// An undecided implication rejects the candidate.
		auto sc = r.open("rejected candidate");
		r.info("whether the other clauses imply this one is undecided",
			{{label::value,
				truncate_for_message(TAU_TO_STR(clauses[k]))}});
		report cand = std::move(implied).report();
		cand.demote_errors_to_warnings();
		r.append(std::move(cand));
	}
	// No clause can be dropped: conjunct a bad splitter.
	return r.with_value(tau::build_wff_and(
		tau::build_wff_always(tau_bad_splitter<BAs...>()), fm));
}

} // namespace idni::tau_lang
