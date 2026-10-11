// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file splitter_types.h
 * @brief Enumeration of splitter categories for Boolean algebra constants.
 */

#ifndef __IDNI__TAU__SPLITTER_TYPES_H__
#define __IDNI__TAU__SPLITTER_TYPES_H__

namespace idni::tau_lang {

/**
 * @brief Selects how large a splitter of a BA element should be.
 *
 * A splitter of x is some s with 0 < s < x. Each BA interprets the hint
 * itself and may ignore it (qint does).
 * - `lower` — a small splitter (for sbf: keep one clause of x).
 * - `middle` — about half of x (for sbf: keep about half the clauses).
 * - `upper` — a large splitter (for sbf: drop one clause of x); the default.
 * - `bad` — skip the structural attempts and split with a fresh symbol not
 *   occurring in x (for sbf: a new variable; for Tau formulas: a fresh
 *   uninterpreted constant, see tau_bad_splitter()).
 */
enum class splitter_type {
	lower, middle, upper, bad
};

} // namespace idni::tau_lang

#endif //__IDNI__TAU__SPLITTER_TYPES_H__
