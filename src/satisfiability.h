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
	tref normalized_fm = normalize_with_temp_simp(fm);
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
template <typename... BAs>
bool is_tau_impl(tref f1, tref f2) {
	using tau = tree<node<BAs...>>;
	auto f1_norm = normalizer_step(f1);
	auto f2_norm = normalizer_step(f2);
	auto imp_check = normalize_with_temp_simp((tau::build_wff_neg(
				tau::build_wff_imply(f1_norm, f2_norm))));
	auto clauses = tau::get_dnf_wff_clauses(imp_check);
	// Now check that each disjunct is not satisfiable
	for (tref c : clauses) {
		auto ctn = transform_to_execution<BAs...>(c);
		if (!tau::subtree_equals(ctn, tau::_F())) return false;
	}
	return true;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SATISFIABILITY_H__