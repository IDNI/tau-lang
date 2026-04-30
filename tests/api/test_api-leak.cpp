// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

TEST_SUITE("Tau API - leak test") {
	TEST_CASE("memory map grows unbounded without pruning") {
		// Use the exact genesis spec that reproduces the memory leak.
		// This spec has lookback=0, so memory entries from past steps
		// are never needed again, yet without pruning they accumulate
		// at 3 entries per step (i0, u, o0).
		auto maybe_i = tau_api::get_interpreter(
			"((!(i0[t]=0))? (u[t] = i0[t] && o0[t] = 0"
			" & { u[t] = i0[t] & { u[t] = i0[t] } })"
			" : o0[t] = 1).");
		REQUIRE(maybe_i.has_value());
		auto& interp = maybe_i.value();

		size_t memory_size_at_50 = 0;
		size_t memory_size_at_500 = 0;

		for (size_t step = 0; step < 500; ++step) {
			auto inputs = tau_api::get_inputs_for_step(interp);
			std::map<stream_at, std::string> assigned_inputs;
			for (auto& input_at : inputs) {
				assigned_inputs[input_at] = "T";
			}
			auto maybe_outputs = tau_api::step(interp, assigned_inputs);
			REQUIRE(maybe_outputs.has_value());

			if (step == 49) memory_size_at_50 = interp.memory.size();
			if (step == 499) memory_size_at_500 = interp.memory.size();
		}

		MESSAGE("memory.size() at step  50: " << memory_size_at_50);
		MESSAGE("memory.size() at step 500: " << memory_size_at_500);

		// The critical assertion: memory must NOT grow proportionally
		// to the number of steps.
		//
		// Without fix: memory grows linearly at ~3 entries/step:
		//   step  50 → 150 entries
		//   step 500 → 1500 entries
		//   ratio = 10x
		//
		// With fix: memory is pruned to a bounded window. The exact
		// size depends on lookback (0 for this spec), so memory stays
		// small or even 0. We assert that the ratio is less than 2x,
		// which is impossible for linear growth (10x) but trivially
		// satisfied for bounded memory.
		if (memory_size_at_50 > 0) {
			CHECK(memory_size_at_500 < memory_size_at_50 * 2);
		} else {
			// If memory is 0 at step 50, it must also be 0 at step 500
			// (pruning keeps nothing for lookback=0 specs)
			CHECK(memory_size_at_500 == 0);
		}
	}
}
