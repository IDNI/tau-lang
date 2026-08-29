// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
#pragma once

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	auto normalized = normalizer<node>(substitution);
	if (!normalized.has_value()) return false;
	tref check = normalized.value();
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	// A solved value may still carry free uconsts (bare witnesses);
	// equals_T() alone can't see past them, so existentially close what's
	// left before deciding, exactly as is_non_temp_nso_satisfiable does.
	if (tau::get(check).equals_T()) return true;
	auto sat = is_non_temp_nso_satisfiable<node>(check);
	return sat.has_value() && sat.value();
}

inline bool test_find_solution(const char* src) {
	tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
	equation = norm_all_equations<node_t>(equation);
	equation = apply_all_xor_def<node_t>(equation);
	auto solution = find_solution<node_t>(equation);
	return check_solution<node_t>(equation, solution.value());
}

// Builds the inequality_system the same way solve_general_system does:
// normalized to bf_neq_0 form, a clause reduced to T dropped, one reduced
// to F fails the system outright (nullopt).
inline std::optional<inequality_system<node_t>> build_inequality_system(
		const std::vector<std::string>& inequalities) {
	inequality_system<node_t> system;
	for (const auto& ineq : inequalities) {
		tref eq = get_nso_rr<node_t>(tau::get(ineq)).value().main->get();
		eq = norm_all_equations<node_t>(eq);
		eq = apply_all_xor_def<node_t>(eq);
		if (tau::get(eq).equals_F()) return {};
		if (tau::get(eq).equals_T()) continue;
		system.insert(eq);
	}
	return system;
}

inline bool test_solve_inequality_system(
		const std::vector<std::string>& inequalities, tref splitter_one) {
	auto built = build_inequality_system(inequalities);
	if (!built.has_value()) return false;
	const inequality_system<node_t>& system = built.value();
	solver_options options = {
		.splitter_one = splitter_one,
		.mode = solver_mode::general,
		.type_id = system.empty() ? 0 : find_ba_type<node_t>(*system.begin())
	};
	auto solution = solve_inequality_system<node_t>(system, options);
	if (!solution.has_value()) return false;
	bool check = true;
	for (tref equation : system)
		check = check && check_solution<node_t>(equation, solution.value());
	return check;
}

inline bool test_solve_system(
		const std::string& equality,
		const std::vector<std::string>& inequalities,
		tref splitter_one) {
	equation_system<node_t> system;
	if (!equality.empty())
		system.first = get_nso_rr<node_t>(
			tau::get(equality)).value().main->get();
	for (const auto& ineq : inequalities)
		system.second.insert(get_nso_rr<node_t>(
			tau::get(ineq)).value().main->get());
	solver_options options = {
		.splitter_one = splitter_one,
		.mode = solver_mode::general
	};
	auto solution = solve_system<node_t>(system, options);
	auto copy = solution.value();
	bool check = system.first
		? check_solution<node_t>(system.first.value(), copy) : false;
	for (const auto& equation : system.second) {
		auto copy2 = solution.value();
		check = check && check_solution<node_t>(equation, copy2);
	}
	return check;
}

inline bool test_solve(const std::string& system, const solver_options& options) {
	tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
	auto solution = solve<node_t>(form, options);
	return solution.has_value()
		? check_solution<node_t>(form, solution.value()) : false;
}

inline bool test_solve(const std::string& system, tref type) {
	solver_options options = {
		.splitter_one = node_t::ba::splitter_one(type),
		.mode = solver_mode::general
	};
	return test_solve(system, options);
}

inline bool test_solve_min(const std::string& system, tref type) {
	solver_options options = {
		.splitter_one = node_t::ba::splitter_one(type),
		.mode = solver_mode::minimum
	};
	return test_solve(system, options);
}

inline bool test_solve_max(const std::string& system, tref type) {
	solver_options options = {
		.splitter_one = node_t::ba::splitter_one(type),
		.mode = solver_mode::maximum
	};
	return test_solve(system, options);
}
