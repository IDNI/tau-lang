# Solver Integration Tests: qint, hsb, qlt, mixed

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add four integration solver test files — `qint`, `hsb`, `qlt`, `mixed` — to `tests/integration/solver/`, each exercising the general solver framework (find_solution, solve_inequality_system, solve_system, solve) with BA-typed Tau wff formulas.

**Architecture:** Each per-BA file includes `test_tau_helpers.h` (full bas_pack, `node_t` = `node<tau_ba<qint,qlt,nlang_ba,bv,sbf_ba,hsb>, ...>`), defines a file-scope `check_solution` helper identical to the sbf_ba file, and defines test-suite-scoped helper functions. The mixed file uses the same node_t and combines two BA-typed constants in one formula. CMakeLists.txt is updated incrementally: one BA per task.

**Tech Stack:** C++23, CMake 3.22+, doctest v2.4.11, Tau solver framework

**Key API:**
- `qint_splitter_one()` → `qint` (from `boolean_algebras/qint.h`, included via `test_tau_helpers.h`)
- `hsb_splitter_one()` → `hsb` (from `boolean_algebras/hsb_splitter.tmpl.h`)
- `qlt_splitter_one()` → `qlt` (from `boolean_algebras/qlt.h`)
- `qint_type<node_t>()`, `hsb_type<node_t>()`, `qlt_type<node_t>()`, `tau_type<node_t>()` → `tref` (from `ba_types.h`, included transitively)
- `node_t::ba::splitter_one(type)` → `tref` used in `solver_options.splitter_one`

**Note on formula strings:** Tau wff formulas use `{<ba-syntax>}:type` for BA constants and `x : type` for typed variables. If a formula fails to parse (`.value()` throws on a null optional), verify the exact constant syntax against the relevant `.tgf` grammar file.

---

## File Map

| Action | Path |
|--------|------|
| Modify | `tests/integration/solver/CMakeLists.txt` |
| Create | `tests/integration/solver/test_integration-solver-qint.cpp` |
| Create | `tests/integration/solver/test_integration-solver-hsb.cpp` |
| Create | `tests/integration/solver/test_integration-solver-qlt.cpp` |
| Create | `tests/integration/solver/test_integration-solver-mixed.cpp` |

---

## Task 1: Verify baseline

**Files:** none modified

- [ ] **Step 1: Build with tests**

```bash
./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | tail -10
```

Expected: build succeeds.

- [ ] **Step 2: Run existing solver tests**

```bash
cd build-Debug && ctest -R "test_integration-solver" --output-on-failure
```

Expected: 3/3 pass (`bv_ba1`, `bv_ba2`, `sbf_ba`).

---

## Task 2: Add qint solver tests

**Files:**
- Modify: `tests/integration/solver/CMakeLists.txt`
- Create: `tests/integration/solver/test_integration-solver-qint.cpp`

- [ ] **Step 1: Add `qint` to CMakeLists.txt**

Edit `tests/integration/solver/CMakeLists.txt` — add `qint` to the `TESTS` list:

```cmake
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
	qint
)

foreach(X IN LISTS TESTS)
	add("test_integration-solver-${X}")
	# From this depth ${CMAKE_CURRENT_SOURCE_DIR}/.. in add() resolves to
	# tests/integration/ — add tests/ root so shared headers remain reachable.
	target_include_directories("test_integration-solver-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
	# Override working directory to tests/ (same as all other integration tests).
	set_tests_properties("test_integration-solver-${X}" PROPERTIES
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()
```

- [ ] **Step 2: Create `test_integration-solver-qint.cpp`**

Create `tests/integration/solver/test_integration-solver-qint.cpp`:

```cpp
// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	return tau::get(check).equals_T();
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return check_solution<node_t>(equation, solution.value());
	}

	TEST_CASE("one var: {[0, 1)}:qint x = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x = 0.") );
	}

	TEST_CASE("two vars: {[0, 1)}:qint x | {[1, 2)}:qint y = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x | {[1, 2)}:qint y = 0.") );
	}

	TEST_CASE("complement: {[0, 1)}:qint x | {[0, 1)}:qint' y = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x | {[0, 1)}:qint' y = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		inequality_system<node_t> system;
		for (const auto& ineq : inequalities)
			system.insert(get_nso_rr<node_t>(tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qint_type<node_t>()),
			.mode = solver_mode::general
		};
		auto solution = solve_inequality_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check && check_solution<node_t>(equation, solution.value());
		return check;
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[0, 1)}:qint' x != 0.") {
		CHECK( test_solve_inequality_system({
			"{[0, 1)}:qint x != 0.",
			"{[0, 1)}:qint' x != 0."
		}) );
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0.") {
		CHECK( test_solve_inequality_system({
			"{[0, 1)}:qint x != 0.",
			"{[1, 2)}:qint y != 0."
		}) );
	}

	TEST_CASE("x : qint != 0 && (x : qint)' != 0.") {
		CHECK( test_solve_inequality_system({
			"x : qint != 0.",
			"(x : qint)' != 0."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		equation_system<node_t> system;
		if (!equality.empty())
			system.first = get_nso_rr<node_t>(
				tau::get(equality)).value().main->get();
		for (const auto& ineq : inequalities)
			system.second.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qint_type<node_t>()),
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

	TEST_CASE("{[0, 1)}:qint x = 0 && {[0, 1)}:qint' x != 0.") {
		CHECK( test_solve_system(
			"{[0, 1)}:qint x = 0.",
			{"{[0, 1)}:qint' x != 0."}
		) );
	}

	TEST_CASE("{[0, 1)}:qint x | {[1, 2)}:qint y = 0 && {[1, 2)}:qint y != 0.") {
		CHECK( test_solve_system(
			"{[0, 1)}:qint x | {[1, 2)}:qint y = 0.",
			{"{[1, 2)}:qint y != 0."}
		) );
	}

	TEST_CASE("x : qint < y : qint") {
		CHECK( test_solve_system(
			"x : qint & (y : qint)' = 0.",
			{"x : qint & (y : qint)' | (x : qint)' & y : qint != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = qint_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	bool test_solve_min(const std::string& system,
			const tref type = qint_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string& system,
			const tref type = qint_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	TEST_CASE("x : qint != 0") {
		const char* system = "x : qint != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("x : qint != 0 && x : qint != 1") {
		const char* system = "x : qint != 0 && x : qint != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0") {
		const char* system = "{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: x : qint = 0 && x : qint != 0") {
		const char* system = "x : qint = 0 && x : qint != 0.";
		CHECK( !test_solve(system) );
	}
}
```

- [ ] **Step 3: Build**

```bash
./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | grep -E "error:|test_integration-solver-qint|100%"
```

Expected: binary `test_integration-solver-qint` links successfully, no compile errors.

- [ ] **Step 4: Run the new test**

```bash
cd build-Debug && ctest -R "test_integration-solver-qint$" --output-on-failure
```

Expected: `1/1 Test #XX: test_integration-solver-qint ... Passed`.

- [ ] **Step 5: Commit**

```bash
git add tests/integration/solver/CMakeLists.txt \
        tests/integration/solver/test_integration-solver-qint.cpp
git commit -m "$(cat <<'EOF'
test: add integration solver tests for qint BA

find_solution, solve_inequality_system, solve_system, and solve suites
using Tau wff formulas with {[lo, hi)}:qint typed constants and x:qint
typed variables.
EOF
)"
```

---

## Task 3: Add hsb solver tests

**Files:**
- Modify: `tests/integration/solver/CMakeLists.txt`
- Create: `tests/integration/solver/test_integration-solver-hsb.cpp`

- [ ] **Step 1: Add `hsb` to CMakeLists.txt**

Edit `tests/integration/solver/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
	qint
	hsb
)

foreach(X IN LISTS TESTS)
	add("test_integration-solver-${X}")
	target_include_directories("test_integration-solver-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
	set_tests_properties("test_integration-solver-${X}" PROPERTIES
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()
```

- [ ] **Step 2: Create `test_integration-solver-hsb.cpp`**

Create `tests/integration/solver/test_integration-solver-hsb.cpp`:

```cpp
// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	return tau::get(check).equals_T();
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return check_solution<node_t>(equation, solution.value());
	}

	TEST_CASE("one var: {x[0] < 0}:hsb a = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a = 0.") );
	}

	TEST_CASE("two vars: {x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0.") );
	}

	TEST_CASE("complement: {x[0] < 0}:hsb a | {x[0] < 0}:hsb' b = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a | {x[0] < 0}:hsb' b = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		inequality_system<node_t> system;
		for (const auto& ineq : inequalities)
			system.insert(get_nso_rr<node_t>(tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(hsb_type<node_t>()),
			.mode = solver_mode::general
		};
		auto solution = solve_inequality_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check && check_solution<node_t>(equation, solution.value());
		return check;
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[0] < 0}:hsb' a != 0.") {
		CHECK( test_solve_inequality_system({
			"{x[0] < 0}:hsb a != 0.",
			"{x[0] < 0}:hsb' a != 0."
		}) );
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0.") {
		CHECK( test_solve_inequality_system({
			"{x[0] < 0}:hsb a != 0.",
			"{x[1] < 0}:hsb b != 0."
		}) );
	}

	TEST_CASE("a : hsb != 0 && (a : hsb)' != 0.") {
		CHECK( test_solve_inequality_system({
			"a : hsb != 0.",
			"(a : hsb)' != 0."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		equation_system<node_t> system;
		if (!equality.empty())
			system.first = get_nso_rr<node_t>(
				tau::get(equality)).value().main->get();
		for (const auto& ineq : inequalities)
			system.second.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(hsb_type<node_t>()),
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

	TEST_CASE("{x[0] < 0}:hsb a = 0 && {x[0] < 0}:hsb' a != 0.") {
		CHECK( test_solve_system(
			"{x[0] < 0}:hsb a = 0.",
			{"{x[0] < 0}:hsb' a != 0."}
		) );
	}

	TEST_CASE("{x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0 && {x[1] < 0}:hsb b != 0.") {
		CHECK( test_solve_system(
			"{x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0.",
			{"{x[1] < 0}:hsb b != 0."}
		) );
	}

	TEST_CASE("a : hsb < b : hsb") {
		CHECK( test_solve_system(
			"a : hsb & (b : hsb)' = 0.",
			{"a : hsb & (b : hsb)' | (a : hsb)' & b : hsb != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	bool test_solve_min(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	TEST_CASE("a : hsb != 0") {
		const char* system = "a : hsb != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("a : hsb != 0 && a : hsb != 1") {
		const char* system = "a : hsb != 0 && a : hsb != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0") {
		const char* system = "{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: a : hsb = 0 && a : hsb != 0") {
		const char* system = "a : hsb = 0 && a : hsb != 0.";
		CHECK( !test_solve(system) );
	}
}
```

- [ ] **Step 3: Build**

```bash
./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | grep -E "error:|test_integration-solver-hsb|100%"
```

Expected: binary `test_integration-solver-hsb` links successfully.

- [ ] **Step 4: Run the new test**

```bash
cd build-Debug && ctest -R "test_integration-solver-hsb$" --output-on-failure
```

Expected: `1/1 Test #XX: test_integration-solver-hsb ... Passed`.

- [ ] **Step 5: Commit**

```bash
git add tests/integration/solver/CMakeLists.txt \
        tests/integration/solver/test_integration-solver-hsb.cpp
git commit -m "$(cat <<'EOF'
test: add integration solver tests for hsb BA

find_solution, solve_inequality_system, solve_system, and solve suites
using Tau wff formulas with {x[i] < 0}:hsb halfspace constants and
a:hsb typed variables.
EOF
)"
```

---

## Task 4: Add qlt solver tests

**Files:**
- Modify: `tests/integration/solver/CMakeLists.txt`
- Create: `tests/integration/solver/test_integration-solver-qlt.cpp`

- [ ] **Step 1: Add `qlt` to CMakeLists.txt**

Edit `tests/integration/solver/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
	qint
	hsb
	qlt
)

foreach(X IN LISTS TESTS)
	add("test_integration-solver-${X}")
	target_include_directories("test_integration-solver-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
	set_tests_properties("test_integration-solver-${X}" PROPERTIES
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()
```

- [ ] **Step 2: Create `test_integration-solver-qlt.cpp`**

Create `tests/integration/solver/test_integration-solver-qlt.cpp`:

```cpp
// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	return tau::get(check).equals_T();
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return check_solution<node_t>(equation, solution.value());
	}

	TEST_CASE("one var: {(0, 1)}:qlt x = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x = 0.") );
	}

	TEST_CASE("two vars: {(0, 1)}:qlt x | {[1, 2)}:qlt y = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x | {[1, 2)}:qlt y = 0.") );
	}

	TEST_CASE("complement: {(0, 1)}:qlt x | {(0, 1)}:qlt' y = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x | {(0, 1)}:qlt' y = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		inequality_system<node_t> system;
		for (const auto& ineq : inequalities)
			system.insert(get_nso_rr<node_t>(tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general
		};
		auto solution = solve_inequality_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check && check_solution<node_t>(equation, solution.value());
		return check;
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {(0, 1)}:qlt' x != 0.") {
		CHECK( test_solve_inequality_system({
			"{(0, 1)}:qlt x != 0.",
			"{(0, 1)}:qlt' x != 0."
		}) );
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0.") {
		CHECK( test_solve_inequality_system({
			"{(0, 1)}:qlt x != 0.",
			"{[1, 2)}:qlt y != 0."
		}) );
	}

	TEST_CASE("x : qlt != 0 && (x : qlt)' != 0.") {
		CHECK( test_solve_inequality_system({
			"x : qlt != 0.",
			"(x : qlt)' != 0."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		equation_system<node_t> system;
		if (!equality.empty())
			system.first = get_nso_rr<node_t>(
				tau::get(equality)).value().main->get();
		for (const auto& ineq : inequalities)
			system.second.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
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

	TEST_CASE("{(0, 1)}:qlt x = 0 && {(0, 1)}:qlt' x != 0.") {
		CHECK( test_solve_system(
			"{(0, 1)}:qlt x = 0.",
			{"{(0, 1)}:qlt' x != 0."}
		) );
	}

	TEST_CASE("{(0, 1)}:qlt x | {[1, 2)}:qlt y = 0 && {[1, 2)}:qlt y != 0.") {
		CHECK( test_solve_system(
			"{(0, 1)}:qlt x | {[1, 2)}:qlt y = 0.",
			{"{[1, 2)}:qlt y != 0."}
		) );
	}

	TEST_CASE("x : qlt < y : qlt") {
		CHECK( test_solve_system(
			"x : qlt & (y : qlt)' = 0.",
			{"x : qlt & (y : qlt)' | (x : qlt)' & y : qlt != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = qlt_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	bool test_solve_min(const std::string& system,
			const tref type = qlt_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string& system,
			const tref type = qlt_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	TEST_CASE("x : qlt != 0") {
		const char* system = "x : qlt != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("x : qlt != 0 && x : qlt != 1") {
		const char* system = "x : qlt != 0 && x : qlt != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0") {
		const char* system = "{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: x : qlt = 0 && x : qlt != 0") {
		const char* system = "x : qlt = 0 && x : qlt != 0.";
		CHECK( !test_solve(system) );
	}
}
```

- [ ] **Step 3: Build**

```bash
./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | grep -E "error:|test_integration-solver-qlt|100%"
```

Expected: binary `test_integration-solver-qlt` links successfully.

- [ ] **Step 4: Run the new test**

```bash
cd build-Debug && ctest -R "test_integration-solver-qlt$" --output-on-failure
```

Expected: `1/1 Test #XX: test_integration-solver-qlt ... Passed`.

- [ ] **Step 5: Commit**

```bash
git add tests/integration/solver/CMakeLists.txt \
        tests/integration/solver/test_integration-solver-qlt.cpp
git commit -m "$(cat <<'EOF'
test: add integration solver tests for qlt BA

find_solution, solve_inequality_system, solve_system, and solve suites
using Tau wff formulas with {(lo, hi)}:qlt typed constants and x:qlt
typed variables.
EOF
)"
```

---

## Task 5: Add mixed solver tests

**Files:**
- Modify: `tests/integration/solver/CMakeLists.txt`
- Create: `tests/integration/solver/test_integration-solver-mixed.cpp`

- [ ] **Step 1: Add `mixed` to CMakeLists.txt**

Edit `tests/integration/solver/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
	qint
	hsb
	qlt
	mixed
)

foreach(X IN LISTS TESTS)
	add("test_integration-solver-${X}")
	target_include_directories("test_integration-solver-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
	set_tests_properties("test_integration-solver-${X}" PROPERTIES
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()
```

- [ ] **Step 2: Create `test_integration-solver-mixed.cpp`**

Create `tests/integration/solver/test_integration-solver-mixed.cpp`:

```cpp
// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Mixed-BA solver tests: each formula combines constants from two distinct
// Boolean algebras in a single Tau wff equation, exercising the solver's
// ability to find solutions over heterogeneous BA-typed variables.

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	return tau::get(check).equals_T();
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return check_solution<node_t>(equation, solution.value());
	}

	TEST_CASE("qint + qlt: {[0, 1)}:qint x | {(0, 1)}:qlt y = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x | {(0, 1)}:qlt y = 0.") );
	}

	TEST_CASE("hsb + qint: {x[0] < 0}:hsb a | {[0, 1)}:qint b = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a | {[0, 1)}:qint b = 0.") );
	}

	TEST_CASE("qlt + hsb: {(0, 1)}:qlt x | {x[0] < 0}:hsb a = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x | {x[0] < 0}:hsb a = 0.") );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		equation_system<node_t> system;
		if (!equality.empty())
			system.first = get_nso_rr<node_t>(
				tau::get(equality)).value().main->get();
		for (const auto& ineq : inequalities)
			system.second.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
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

	TEST_CASE("qint + qlt: {[0, 1)}:qint x | {(0, 1)}:qlt y = 0 && {(0, 1)}:qlt y != 0.") {
		CHECK( test_solve_system(
			"{[0, 1)}:qint x | {(0, 1)}:qlt y = 0.",
			{"{(0, 1)}:qlt y != 0."}
		) );
	}

	TEST_CASE("hsb + qint: {x[0] < 0}:hsb a | {[0, 1)}:qint b = 0 && {[0, 1)}:qint b != 0.") {
		CHECK( test_solve_system(
			"{x[0] < 0}:hsb a | {[0, 1)}:qint b = 0.",
			{"{[0, 1)}:qint b != 0."}
		) );
	}

	TEST_CASE("qlt + hsb: {(0, 1)}:qlt x | {x[0] < 0}:hsb a = 0 && {x[0] < 0}:hsb a != 0.") {
		CHECK( test_solve_system(
			"{(0, 1)}:qlt x | {x[0] < 0}:hsb a = 0.",
			{"{x[0] < 0}:hsb a != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = tau_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	TEST_CASE("qint + qlt: {[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0") {
		const char* system = "{[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("hsb + qint: {x[0] < 0}:hsb a != 0 && {[0, 1)}:qint b != 0") {
		const char* system = "{x[0] < 0}:hsb a != 0 && {[0, 1)}:qint b != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("qlt + hsb: {(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0") {
		const char* system = "{(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: qint x = 0 && qint x != 0 (mixed system)") {
		const char* system = "{[0, 1)}:qint x = 0 && {(0, 1)}:qlt y != 0 && {[0, 1)}:qint x != 0.";
		CHECK( !test_solve(system) );
	}
}
```

- [ ] **Step 3: Build**

```bash
./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | grep -E "error:|test_integration-solver-mixed|100%"
```

Expected: binary `test_integration-solver-mixed` links successfully.

- [ ] **Step 4: Run the new test**

```bash
cd build-Debug && ctest -R "test_integration-solver-mixed$" --output-on-failure
```

Expected: `1/1 Test #XX: test_integration-solver-mixed ... Passed`.

- [ ] **Step 5: Commit**

```bash
git add tests/integration/solver/CMakeLists.txt \
        tests/integration/solver/test_integration-solver-mixed.cpp
git commit -m "$(cat <<'EOF'
test: add integration solver tests for mixed BA combinations

find_solution, solve_system, and solve suites using Tau wff formulas
that combine constants from two distinct Boolean algebras (qint+qlt,
hsb+qint, qlt+hsb) in a single equation.
EOF
)"
```

---

## Task 6: Final full verification

**Files:** none modified

- [ ] **Step 1: Clean rebuild**

```bash
./dev clean && ./dev debug -DTAU_BUILD_TESTS=ON 2>&1 | tail -10
```

Expected: clean build, no errors.

- [ ] **Step 2: Run all four new tests**

```bash
cd build-Debug && ctest -R "test_integration-solver-(qint|hsb|qlt|mixed)" --output-on-failure
```

Expected:
```
100% tests passed, 0 tests failed out of 4
```

- [ ] **Step 3: Run all integration tests to confirm no regressions**

```bash
cd build-Debug && ctest -R "test_integration" --output-on-failure 2>&1 | tail -10
```

Expected: all integration tests pass.

---

## Self-Review

**Spec coverage:**
- ✅ `test_integration-solver-qint.cpp` — configuration, find_solution, solve_inequality_system, solve_system, solve
- ✅ `test_integration-solver-hsb.cpp` — same five suites
- ✅ `test_integration-solver-qlt.cpp` — same five suites
- ✅ `test_integration-solver-mixed.cpp` — configuration, find_solution, solve_system, solve with two-BA formulas
- ✅ `CMakeLists.txt` updated incrementally per task
- ✅ minterm_iterator / minterm_range suites excluded (covered by sbf_ba, outside this spec's scope)

**Placeholder scan:** No TBD/TODO/placeholder steps. Every step contains exact file content or commands.

**Type consistency:** `check_solution<node_t>`, `solve<node_t>`, `solve_inequality_system<node_t>`, `solve_system<node_t>`, `find_solution<node_t>` — all use `node_t` from `test_tau_helpers.h` throughout. `qint_type<node_t>()`, `hsb_type<node_t>()`, `qlt_type<node_t>()`, `tau_type<node_t>()` used consistently per file. `solver_options` struct used uniformly.
