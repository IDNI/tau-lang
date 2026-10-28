// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The solver cases every atomless algebra with a splitter passes. A suite
// describes its algebra with a struct of
//   static tref type();          the type tree
//   static constexpr const char
//     *name,                     the type name
//     *a, *b,                    two constants, typed, whose meet is zero
//     *x, *y;                    two variable names
// and runs the cases with TEST_CASE_TEMPLATE_INVOKE(<id>, <struct>) for each
// id below. A case spells its formulas with $A, $B, $x, $y and $T for these.

#include "test_integration-solver_helper.h"

#include <string>
#include <vector>

template <typename BA>
std::string solver_case(std::string s) {
	const std::pair<const char*, const char*> subst[] = {
		{ "$A", BA::a }, { "$B", BA::b }, { "$x", BA::x }, { "$y", BA::y },
		{ "$T", BA::name } };
	for (const auto& [from, to] : subst)
		for (size_t p = s.find(from); p != std::string::npos;
			p = s.find(from, p + std::string(to).size()))
			s.replace(p, std::string(from).size(), to);
	return s;
}

template <typename BA>
std::vector<std::string> solver_cases(std::initializer_list<const char*> l) {
	std::vector<std::string> r;
	for (const char* s : l) r.push_back(solver_case<BA>(s));
	return r;
}

template <typename BA>
bool solver_find(const char* s) {
	const std::string src = solver_case<BA>(s);
	CAPTURE(src);
	return test_find_solution(src.c_str());
}

// @p l joined by " && ", for a capture.
inline std::string solver_joined(const std::vector<std::string>& l) {
	std::string r;
	for (const auto& s : l) r += (r.empty() ? "" : " && ") + s;
	return r;
}

template <typename BA>
bool solver_inequalities(std::initializer_list<const char*> l) {
	const auto system = solver_cases<BA>(l);
	const std::string inequalities = solver_joined(system);
	CAPTURE(inequalities);
	return test_solve_inequality_system(system,
		node_t::ba::splitter_one(BA::type()));
}

template <typename BA>
bool solver_system(const char* eq, std::initializer_list<const char*> l) {
	const std::string equality = solver_case<BA>(eq);
	const auto inequalities = solver_cases<BA>(l);
	const std::string joined = solver_joined(inequalities);
	CAPTURE(equality);
	CAPTURE(joined);
	return test_solve_system(equality, inequalities,
		node_t::ba::splitter_one(BA::type()));
}

template <typename BA>
std::string solver_spec(const char* s) {
	return solver_case<BA>(s);
}

TEST_CASE_TEMPLATE_DEFINE("find_solution: one var", BA, solver_find_one_var) {
	CHECK( solver_find<BA>("$A $x = 0.") );
}

TEST_CASE_TEMPLATE_DEFINE("find_solution: two vars", BA, solver_find_two_vars) {
	CHECK( solver_find<BA>("$A $x | $B $y = 0.") );
}

TEST_CASE_TEMPLATE_DEFINE("find_solution: complement", BA, solver_find_complement) {
	CHECK( solver_find<BA>("$A $x | $A' $y = 0.") );
}

TEST_CASE_TEMPLATE_DEFINE("solve_inequality_system: a constant and its complement",
	BA, solver_ineq_complement)
{
	CHECK( solver_inequalities<BA>({ "$A $x != 0.", "$A' $x != 0." }) );
}

TEST_CASE_TEMPLATE_DEFINE("solve_inequality_system: two vars", BA, solver_ineq_two_vars) {
	CHECK( solver_inequalities<BA>({ "$A $x != 0.", "$B $y != 0." }) );
}

TEST_CASE_TEMPLATE_DEFINE("solve_inequality_system: a var and its complement",
	BA, solver_ineq_var_complement)
{
	CHECK( solver_inequalities<BA>({ "$x : $T != 0.", "($x : $T)' != 0." }) );
}

TEST_CASE_TEMPLATE_DEFINE("solve_system: an equation and the complement",
	BA, solver_system_complement)
{
	CHECK( solver_system<BA>("$A $x = 0.", { "$A' $x != 0." }) );
}

// Both vars share the same constant, so the inequality over the disjoint
// constant is not contradicted.
TEST_CASE_TEMPLATE_DEFINE("solve_system: two vars", BA, solver_system_two_vars) {
	CHECK( solver_system<BA>("$A $x | $A $y = 0.", { "$B $y != 0." }) );
}

TEST_CASE_TEMPLATE_DEFINE("solve_system: x < y", BA, solver_system_lt) {
	CHECK( solver_system<BA>("$x : $T & ($y : $T)' = 0.",
		{ "$x : $T & ($y : $T)' | ($x : $T)' & $y : $T != 0." }) );
}

TEST_CASE_TEMPLATE_DEFINE("solve: x != 0", BA, solver_solve_nonzero) {
	const std::string system = solver_spec<BA>("$x : $T != 0.");
	CAPTURE(system);
	CHECK( test_solve(system, BA::type()) );
	CHECK( test_no_solution_min(system, BA::type()) );
	CHECK( test_solve_max(system, BA::type()) );
}

TEST_CASE_TEMPLATE_DEFINE("solve: x != 0 && x != 1", BA, solver_solve_neither) {
	const std::string system = solver_spec<BA>("$x : $T != 0 && $x : $T != 1.");
	CAPTURE(system);
	CHECK( test_solve(system, BA::type()) );
	CHECK( test_no_solution_min(system, BA::type()) );
	CHECK( test_no_solution_max(system, BA::type()) );
}

TEST_CASE_TEMPLATE_DEFINE("solve: two vars", BA, solver_solve_two_vars) {
	const std::string system = solver_spec<BA>("$A $x != 0 && $B $y != 0.");
	CAPTURE(system);
	CHECK( test_solve(system, BA::type()) );
}

TEST_CASE_TEMPLATE_DEFINE("solve: unsatisfiable", BA, solver_solve_unsat) {
	const std::string system = solver_spec<BA>("$x : $T = 0 && $x : $T != 0.");
	CAPTURE(system);
	CHECK( test_no_solution(system, BA::type()) );
}

// Runs every case above for the algebra @p BA.
#define TAU_SOLVER_BA_SUITE(BA) \
	TEST_CASE_TEMPLATE_INVOKE(solver_find_one_var, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_find_two_vars, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_find_complement, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_ineq_complement, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_ineq_two_vars, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_ineq_var_complement, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_system_complement, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_system_two_vars, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_system_lt, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_solve_nonzero, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_solve_neither, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_solve_two_vars, BA); \
	TEST_CASE_TEMPLATE_INVOKE(solver_solve_unsat, BA)
