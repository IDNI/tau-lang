/**
 * @file bv_widening_options.h
 * @brief Runtime parameters for opt-in exact bitvector arithmetic.
 *
 * Deliberately dependency-free, like `heuristics/blast_placement.h`: the
 * api setters (`api::set_bv_widening`, `api::set_bv_max_width`) must see
 * these knobs' declarations wherever `api.h` is included, and
 * `tests/test_init.h` defines every test binary's `main()` and is included
 * before any tau header, so it applies the `TAU_BV_WIDENING` and
 * `TAU_BV_MAX_WIDTH` environment overrides to these knobs directly, before
 * the first test runs.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_WIDENING_OPTIONS_H__
#define __IDNI__TAU__BV_WIDENING_OPTIONS_H__

#include <cstddef>

namespace idni::tau_lang {

// Opt-in exact bitvector arithmetic: atoms are elaborated to compute at
// the minimum overflow-free width and truncate at the output assignment.
// Default false = today's modular semantics, pass fully inert.
// NOT thread-safe (single-threaded library assumption, as bv_blasting).
inline bool bv_widening = false;

// Cap on the computed width W; exceeding it is a loud elaboration error.
// Runtime parameter per project policy.
inline size_t bv_max_width = 1024;

} // namespace idni::tau_lang
#endif // __IDNI__TAU__BV_WIDENING_OPTIONS_H__
