// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "normalizer.h"

namespace idni::tau_lang {

inline static bool use_debug_output_in_sat = false;
inline void print_fixpoint_info(const std::string& message,
	const std::string& result, const bool output)
{
	if (!output) return;
	if (!use_debug_output_in_sat) BOOST_LOG_TRIVIAL(info) << "\n"
					<< message << "\n" << result << "\n\n";
	else std::cerr << message << "\n" << result << "\n";
}

// Check if a formula has a temporary output stream
// which are used for flag handling
template <BAsPack... BAs>
bool has_temporary_io_var(tref fm) {
	using tau = tree<node<BAs...>>;
	auto io_vars = tau::get(fm)
		.select_top(is_child<node<BAs...>, tau::io_var>);
	for (tref var : io_vars) // Check if the name of var starts with "_"
		if (tau::get_io_name(var)[0] == '_') return true;
	return false;
}

template <BAsPack... BAs>
tref transform_to_execution(tref fm,
	const int_t start_time = 0,
	const bool output = false)
{
	using tau = tree<node<BAs...>>;
	assert(tau::get_dnf_wff_clauses(fm).size() == 1);
	std::cout << "transform_to_execution: " << TAU_TO_STR(fm) << " " << start_time << " " << output << "\n";
	return fm; // TODO (HIGH)
}

template <BAsPack... BAs>
bool is_tau_formula_sat(tref fm, const int_t start_time = 0,
	const bool output = false)
{
	using tau = tree<node<BAs...>>;
	BOOST_LOG_TRIVIAL(debug) << "(I) Start is_tau_formula_sat";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << TAU_TO_STR(fm);
	tref normalized_fm = normalize_with_temp_simp<node<BAs...>>(fm);
	trefs clauses = tau::get_leaves(normalized_fm, tau::wff_or);
	// Convert each disjunct to unbounded continuation
	for (tref clause : clauses) {
		if (transform_to_execution<BAs...>(clause, start_time, output) != tau::_F()) {
			BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
			return true;
		}
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
	return false;
}

// Check for temporal formulas if f1 implies f2
template <BAsPack... BAs>
bool is_tau_impl(tref f1, tref f2) {
	using tau = tree<node<BAs...>>;
	auto f1_norm = normalizer_step<node<BAs...>>(f1);
	auto f2_norm = normalizer_step<node<BAs...>>(f2);
	auto imp_check = normalize_with_temp_simp<node<BAs...>>(
		tau::build_wff_neg(tau::build_wff_imply(f1_norm, f2_norm)));
	auto clauses = tau::get_dnf_wff_clauses(imp_check);
	// Now check that each disjunct is not satisfiable
	for (tref c : clauses) {
		auto ctn = transform_to_execution<BAs...>(c);
		if (!tau::subtree_equals(ctn, tau::_F())) return false;
	}
	return true;
}

template <BAsPack... BAs>
tref simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false)
{
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	BOOST_LOG_TRIVIAL(debug) << "(I) Start simp_tau_unsat_valid";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;
	// Check if formula is valid
	if (is_tau_impl<BAs...>(tau::_T(), fm)) return tau::_T();
	tref normalized_fm = normalize_with_temp_simp<node>(fm);
	trefs clauses = tau::get_leaves(normalized_fm, tau::wff_or);

	// Check satisfiability of each clause
	for (tref clause: clauses) if (tau::subtree_equals(tau::_F(),
		transform_to_execution<BAs...>(clause, start_time, output)))
			clause = tau::_F();

	BOOST_LOG_TRIVIAL(debug) << "(I) End simp_tau_unsat_valid";
	return tau::build_wff_or(clauses);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SATISFIABILITY_H__