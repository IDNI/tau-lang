// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef SPLITTER_H
#define SPLITTER_H

#include "satisfiability.h"
#include "nso_ba.h"

namespace idni::tau_lang {
enum class split_sym {
	conjunction, disjunction
};

template<typename... BAs>
tau<BAs...> split(const tau<BAs...>& fm, const size_t fm_type, bool is_cnf,
		   const splitter_type st, std::vector<tau<BAs...> >& mem,
		   size_t i, bool check_temps) {
	// First get clauses if not already in mem
	const bool is_wff = fm_type == tau_parser::wff;
	if (mem.empty()) {
		if (is_wff) {
			mem = is_cnf
				      ? get_cnf_wff_clauses(fm)
				      : get_dnf_wff_clauses(fm);
		} else {
			assert(fm_type == tau_parser::bf);
			mem = is_cnf
				      ? get_cnf_bf_clauses(fm)
				      : get_dnf_bf_clauses(fm);
		}
	}
	auto sym = is_wff
		? (is_cnf ? _T<BAs...> : _F<BAs...>)
		: (is_cnf ? _1<BAs...> : _0<BAs...>);
	switch (st) {
	case splitter_type::upper: {
		// Remove exactly one clause
		if (mem.size() == 1)
			return fm;
		if (i < mem.size()) {
			// We cannot delete a clause holding a temporary io var
			if (check_temps && has_temporary_io_var(mem[i]))
				return fm;
			std::map<tau<BAs...>, tau<BAs...> > changes = {
				{mem[i], sym}
			};
			return replace(fm, changes);
		}
		break;
	}
	case splitter_type::middle: {
		// Remove half of the clause
		size_t window = mem.size() / 2;
		std::map<tau<BAs...>, tau<BAs...> > changes;
		for (size_t j = 0; j < window; ++j) {
			size_t idx = (i + j) % mem.size();
			if (check_temps && has_temporary_io_var(mem[idx]))
				continue;
			changes.emplace(mem[idx], sym);
		}
		if (!changes.empty()) return replace(fm, changes);
		break;
	}
	case splitter_type::lower: {
		// Remove all but one clause
		if (i < mem.size())
			if (check_temps && !has_temporary_io_var(mem[i]))
				return mem[i];
		break;
	}
	case splitter_type::bad: {
		// This case must not happen
		assert(false);
	}
	}
	return fm;
}

// If checking a temporal Tau formula F, we split a single DNF clause.
// In order to check if the split clause yields a splitter for F, we have that
// "fm" holds the clause, "splitter" holds the splitter of the clause and
// "spec" holds the original temporal Tau formula
// If we check a non-temporal Tau formula, it suffices to place it in "fm" and
// the proposed splitter in "splitter".
template<typename... BAs>
bool is_splitter(const tau<BAs...>& fm, const tau<BAs...>& splitter, const tau<BAs...>& spec = nullptr) {
	if (spec) {
		// We are dealing with a temporal formula
		if (!are_tau_equivalent(splitter, fm)) {
			assert(is_tau_impl(splitter, fm));
			std::map<tau<BAs...>, tau<BAs...>> c = {{fm, splitter}};
			auto new_spec = normalize_with_temp_simp(replace(spec, c));
			if (is_tau_formula_sat(new_spec))
				return true;
		}
	} else {
		// We are dealing with a non-temporal formula
		if (!are_nso_equivalent(splitter, fm) && normalizer_step(splitter)
		    != _F<BAs...>) {
			assert(is_nso_impl(splitter, fm));
			return true;
		}
	}
	return false;
}

// Find a Boolean function which implies f
template<typename... BAs>
tau<BAs...> good_splitter_using_function(const tau<BAs...>& f, splitter_type st,
					 const tau<BAs...>& original_fm) {
	assert(is_non_terminal(tau_parser::bf, f));
	// First check if we have more then one disjunct
	std::vector<tau<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(f, tau_parser::bf, false, st, m, i, true);
		if (s != f) {
			std::map<tau<BAs...>, tau<BAs...> > changes = {{f, s}};
			auto new_fm = replace(original_fm, changes);
			if(!are_nso_equivalent(original_fm, new_fm))
				return new_fm;
		}
	} while (++i < m.size());

	// Find possible coefficient in each disjunct of f
	std::vector<tau<BAs...> > clauses = get_leaves(f, tau_parser::bf_or, tau_parser::bf);
	// In case f is just a single clause
	if (clauses.empty()) clauses.push_back(f);
	for (const auto &clause: clauses) {
		auto coeff = find_top(clause, is_non_terminal<tau_parser::bf_constant, BAs...>);
		if (!coeff) continue;
		assert(is_non_terminal(tau_parser::bf_constant, coeff.value()));
		auto s = splitter(coeff.value());
		if (s != coeff.value()) {
			std::map<tau<BAs...>, tau<BAs...> > changes = {{coeff.value(), trim(s)}};
			auto new_fm = replace(original_fm, changes);
			if(!are_nso_equivalent(original_fm, new_fm))
				return new_fm;
		}
	}
	return original_fm;
}

// Find a Boolean function which is implied by f
template<typename... BAs>
tau<BAs...> good_reverse_splitter_using_function(const tau<BAs...> &f, splitter_type st,
						 const tau<BAs...> &original_fm) {
	assert(is_non_terminal(tau_parser::bf, f));
	// Convert Boolean function to CNF
	auto f_cnf = to_cnf2(f, false);

	// Try to remove a conjunt to produce splitter
	std::vector<tau<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(f_cnf, tau_parser::bf, true, st, m, i, true);
		if (s != f_cnf) {
			std::map<tau<BAs...>, tau<BAs...> > changes = {{f, s}};
			auto new_fm = replace(original_fm, changes);
			if(!are_nso_equivalent(original_fm, new_fm))
				return new_fm;
		}
	} while (++i < m.size());

	// Try to split coefficient in each conjunct of f
	std::vector<tau<BAs...> > clauses = get_leaves(f_cnf, tau_parser::bf_and, tau_parser::bf);
	for (const auto &clause: clauses) {
		auto coeff = find_top(clause, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
		if (!coeff) continue;
		// TODO: Optimization: Instead of negating the formula think about reverse splitter call
		// If coefficient exists try to split the negation
		auto neg_coeff = push_negation_in(build_bf_neg(coeff.value()), false);
		assert(is_non_terminal(tau_parser::bf_constant, trim(neg_coeff)));
		auto s = splitter(trim(neg_coeff), st);
		// Negating s results in a reversed splitter for s
		s = push_negation_in(build_bf_neg(s), false);
		if (s != coeff.value()) {
			std::map<tau<BAs...>, tau<BAs...> > changes = {{coeff.value(), s}};
			auto new_fm = replace(original_fm, changes);
			if(!are_nso_equivalent(original_fm, new_fm))
				return new_fm;
		}
	}
	return original_fm;
}

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template<typename... BAs>
tau<BAs...> tau_bad_splitter(tau<BAs...> fm = _T<BAs...>) {
	auto new_uniter_const = build_wff_neq(get_new_uniter_const(fm, "split"));
	auto clauses = get_dnf_wff_clauses(fm);
	// Add bad splitter only to a single disjunct if possible
	if (!clauses.empty()) {
		clauses[0] = build_wff_and(clauses[0], new_uniter_const);
		return build_wff_or<BAs...>(clauses);
	} else return build_wff_and(fm, new_uniter_const);
}

// Return a splitter for the provided non-temporal formula
// We assume the formula is fully normalized by normalizer
template<typename... BAs>
std::pair<tau<BAs...>, splitter_type> nso_tau_splitter(
	tau<BAs...> fm, splitter_type st, const tau<BAs...>& spec = nullptr) {
	if (st == splitter_type::bad)
		return {tau_bad_splitter(fm), splitter_type::bad};

	// Collect coefficients to produce splitters
	auto bf_constants = select_top(fm, is_child_non_terminal<tau_parser::bf_constant, BAs...>);

	//fm = snf_wff(fm);
	// Collect all occurances of "||" while assuming that fm is in DNF
	auto clauses = get_dnf_wff_clauses(fm);
	for (const auto& clause: clauses) {
		// check for equality parts
		auto eqs = select_top(clause, is_non_terminal<tau_parser::bf_eq, BAs...>);
		for (const auto &eq: eqs) {
			assert(is_non_terminal(tau_parser::bf_f, trim(eq->child[1])));
			auto f = eq->child[0];
			std::set<tau<BAs...>> free_vars = get_free_vars_from_nso(fm);
			for (const auto& c : bf_constants) {
				// Try to convert f(x,...) = 0 to f(x,...) = 0 && x < c' for some variable x in f
				auto vars_f = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>);
				for (const auto& v : vars_f) {
					if (!free_vars.contains(trim(v))) continue;
					std::map<tau<BAs...>, tau<BAs...> >
							changes = {{clause, build_wff_and(clause,
									build_bf_less_equal(v, c))}};
					auto new_fm = replace(fm, changes);
					if (is_splitter(fm, new_fm, spec))
						return {new_fm, st};
				}
			}
			if (auto s = good_reverse_splitter_using_function(f, st, clause); s != clause) {
				//TODO: this equiv check should happen in good_reverse_splitter_using_function
				std::map<tau<BAs...>, tau<BAs...> > c = {{clause, s}};
				auto new_fm = replace(fm, c);
				if (is_splitter(fm, new_fm, spec))
					return {new_fm, st};
			}
		}
		// check for inequality parts
		auto neqs = select_top(clause, is_non_terminal<tau_parser::bf_neq, BAs...>);
		for (const auto &neq: neqs) {
			assert(is_non_terminal(tau_parser::bf_f, trim(neq->child[1])));
			auto f = neq->child[0];
			for (const auto& c : bf_constants) {
				// Try to convert f != 0 to f >= c
				// First check that types match
				auto type_f = find_top(f, is_non_terminal<tau_parser::type, BAs...>);
				auto type_c = c | tau_parser::bf_constant | tau_parser::type;
				if (type_f.has_value() && type_f != type_c)
					continue;
				auto r = build_bf_greater_equal(f, c);
				std::map<tau<BAs...>, tau<BAs...> > changes = {{clause, build_wff_and(clause, r)}};
				auto new_fm = replace(fm, changes);
				changes = {{neq, _T_trimmed<BAs...>}};
				new_fm = replace(new_fm, changes);
				if (is_splitter(fm, new_fm, spec))
					return {new_fm, st};
			}
			if (auto s = good_splitter_using_function(f, st, clause); s != clause) {
				//TODO: this equiv check should happen in good_splitter_using_function
				std::map<tau<BAs...>, tau<BAs...> > c = {{clause, s}};
				auto new_fm = replace(fm, c);
				if (is_splitter(fm, new_fm, spec))
					return {new_fm, st};
			}
		}
	}

	// Split disjunction if possible
	std::vector<tau<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(fm, tau_parser::wff, false, st, m, i, true);
		if (is_splitter(fm, s, spec))
			return {s, st};
	} while (++i < m.size());

	// return bad splitter by conjuncting new uninterpreted constant
	return {tau_bad_splitter(fm), splitter_type::bad};
}

// We assume fm to be normalized in DNF
template<typename... BAs>
tau<BAs...> tau_splitter (const tau<BAs...>& fm, splitter_type st) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start of tau_splitter";
	using p = tau_parser;
	// First we decide if we deal with a temporal formula
	if (!has_temp_var(fm)) return nso_tau_splitter(fm, st).first;

	auto splitter_of_clause = [&](const tau<BAs...>& clause) {
		auto specs = get_cnf_wff_clauses(clause);
		bool good_splitter = false;
		for (auto& spec : specs) {
			bool is_aw = is_child_non_terminal(p::wff_always, spec);
			auto [splitter, type] = nso_tau_splitter(trim2(spec), st, fm);
			if (type != splitter_type::bad) {
				BOOST_LOG_TRIVIAL(trace) << "Splitter of spec: " << splitter;
				good_splitter = true;
				if (is_aw) splitter = build_wff_always(splitter);
				else splitter = build_wff_sometimes(splitter);
				spec = splitter;
				break;
			}
		}
		if (good_splitter) return make_pair(build_wff_and<BAs...>(specs), st);
		else return make_pair(_F<BAs...>, splitter_type::bad);
	};

	// Fm is temporal
	auto clauses = get_dnf_wff_clauses(fm);
	for (int_t i = 0; i < (int_t)clauses.size(); ++i) {
		// First check redundancy between current clause and rest
		bool is_redundant = false;
		for (size_t j = 0; j < clauses.size(); ++j) {
			if ((size_t)i == j) continue;
			if (is_tau_impl(clauses[j], clauses[i])) {
				clauses.erase(clauses.begin() + i);
				--i;
				is_redundant = true;
				break;
			}
		}
		if (is_redundant) continue;
		auto [splitter, type] = splitter_of_clause(clauses[i]);
		if (type != splitter_type::bad) {
			clauses[i] = splitter;
			return build_wff_or<BAs...>(clauses);
		}
	}
	if (clauses.size() == 1) {
		// Conjunct always part with bad splitter
		// If there is no always part, create one
		auto aw = find_top(fm, is_child_non_terminal<p::wff_always, BAs...>);
		if (aw.has_value()) {
			auto aw_bad_splitter = tau_bad_splitter(trim2(aw.value()));
			std::map<tau<BAs...>, tau<BAs...> > c = {
				{aw.value(), build_wff_always(aw_bad_splitter)}
			};
			return replace(fm, c);
		} else return build_wff_and(build_wff_always(tau_bad_splitter<BAs...>()), fm);
	} else {
        // By assumption there is more than one clause and all not redundant
        // Split disjunction taking splitter type into account
        return split(fm, tau_parser::wff, false, st, clauses, 0, false);
	}
}
}
#endif //SPLITTER_H
