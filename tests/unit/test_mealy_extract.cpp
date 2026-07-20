// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// src/mealy_extract.h (extract_mealy) had zero test coverage anywhere in the
// repo -- grepping for `extract_mealy`/`mealy::` outside the header itself
// turns up nothing. It's the skeleton for Algorithm D's Phase 4 (winning-set
// -> concrete Mealy machine), and is fully self-contained (only <map>,
// <unordered_map>, <vector>), so it's unit-testable directly with a
// hand-built witness map and a plain lambda successor -- no BA/solver
// machinery required.

#include "test_init.h"
#include "mealy_extract.h"

using namespace idni::tau_lang::mealy;

TEST_SUITE("mealy_extract") {

	TEST_CASE("single self-loop vertex emits one edge back to itself") {
		std::vector<int> winning = {0};
		std::map<std::pair<int,int>, int> witness = { {{0, 0}, 7} };
		auto successor = [](int, int) { return 0; };

		Mealy m = extract_mealy(winning, 0, witness, 1, successor);

		CHECK(m.num_states == 1);
		CHECK(m.initial_state == 0);
		REQUIRE(m.edges.size() == 1);
		CHECK(m.edges[0].from_state == 0);
		CHECK(m.edges[0].input_sigma == 0);
		CHECK(m.edges[0].output_tau == 7);
		CHECK(m.edges[0].to_state == 0);
	}

	TEST_CASE("state indices are dense positions in winning_vertices, not the vertex values") {
		// Winning vertices are given out of order and non-contiguous: the
		// resulting state index must be the position in the vector (0,1,2),
		// not the raw vertex id (5,2,9).
		std::vector<int> winning = {5, 2, 9};
		std::map<std::pair<int,int>, int> witness = {
			{{5, 0}, 100}, {{2, 0}, 200}, {{9, 0}, 300},
		};
		// successor cycles 5 -> 2 -> 9 -> 5
		auto successor = [](int v, int) {
			if (v == 5) return 2;
			if (v == 2) return 9;
			return 5;
		};

		Mealy m = extract_mealy(winning, 0, witness, 1, successor);

		CHECK(m.num_states == 3);
		REQUIRE(m.edges.size() == 3);
		auto find_edge = [&](int from) -> const Edge& {
			for (auto& e : m.edges) if (e.from_state == from) return e;
			FAIL("no edge found for from_state");
			static Edge dummy{};
			return dummy;
		};
		// vertex 5 -> state 0, successor vertex 2 -> state 1
		CHECK(find_edge(0).to_state == 1);
		CHECK(find_edge(0).output_tau == 100);
		// vertex 2 -> state 1, successor vertex 9 -> state 2
		CHECK(find_edge(1).to_state == 2);
		CHECK(find_edge(1).output_tau == 200);
		// vertex 9 -> state 2, successor vertex 5 -> state 0
		CHECK(find_edge(2).to_state == 0);
		CHECK(find_edge(2).output_tau == 300);
	}

	TEST_CASE("missing witness for a (vertex, sigma) pair silently skips that edge") {
		std::vector<int> winning = {0};
		// Only sigma=0 has a witness; sigma=1 has none.
		std::map<std::pair<int,int>, int> witness = { {{0, 0}, 42} };
		auto successor = [](int, int) { return 0; };

		Mealy m = extract_mealy(winning, 0, witness, /*num_T2=*/2, successor);

		REQUIRE(m.edges.size() == 1);
		CHECK(m.edges[0].input_sigma == 0);
	}

	TEST_CASE("successor landing outside the winning set silently skips that edge") {
		std::vector<int> winning = {0, 1};
		std::map<std::pair<int,int>, int> witness = {
			{{0, 0}, 1}, // successor(0,1) below leads out of the winning set
			{{1, 0}, 2}, // successor(1,2) stays inside
		};
		auto successor = [](int v, int tau) {
			if (v == 0) return 99; // not in winning_vertices
			(void)tau;
			return 0;
		};

		Mealy m = extract_mealy(winning, 0, witness, 1, successor);

		REQUIRE(m.edges.size() == 1);
		CHECK(m.edges[0].from_state == 1); // vertex 1 -> state 1
		CHECK(m.edges[0].to_state == 0);   // vertex 0 -> state 0
	}

	TEST_CASE("empty winning set yields an empty machine with no edges") {
		std::vector<int> winning = {};
		std::map<std::pair<int,int>, int> witness;
		auto successor = [](int, int) { return 0; };

		Mealy m = extract_mealy(winning, 0, witness, 3, successor);

		CHECK(m.num_states == 0);
		CHECK(m.edges.empty());
	}

	TEST_CASE("initial_state is stored verbatim, independent of winning_vertices content") {
		std::vector<int> winning = {0, 1, 2};
		std::map<std::pair<int,int>, int> witness;
		auto successor = [](int, int) { return 0; };

		Mealy m = extract_mealy(winning, /*initial_state=*/2, witness, 1, successor);

		CHECK(m.initial_state == 2);
	}
}
