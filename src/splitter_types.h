// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file splitter_types.h
 * @brief Enumeration of splitter categories for Boolean algebra constants.
 */

#ifndef __IDNI__TAU__SPLITTER_TYPES_H__
#define __IDNI__TAU__SPLITTER_TYPES_H__

namespace idni::tau_lang {

/**
 * @brief Identifies which part of a BA constant's interval a splitter targets.
 *
 * - `lower` — a value strictly below the mid-point of the interval.
 * - `middle` — the mid-point itself.
 * - `upper` — a value strictly above the mid-point.
 * - `bad` — a placeholder that indicates no valid splitter was found.
 */
enum class splitter_type {
	lower, middle, upper, bad
};

} // namespace idni::tau_lang

#endif //__IDNI__TAU__SPLITTER_TYPES_H__
