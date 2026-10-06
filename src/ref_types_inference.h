// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ref_types_inference.h
 * @brief Recurrence-relation signature dictionary and `rr_sig` type for type inference.
 *
 * Provides `rr_dict` (a string ↔ id registry for recurrence-relation names),
 * `rr_sig` (name × offset-arity × argument-arity triple), and the matching
 * `std::hash` specialization.
 */

#ifndef __IDNI__TAU__REF_TYPES_INFERENCE_H__
#define __IDNI__TAU__REF_TYPES_INFERENCE_H__

#include <functional>
#include <unordered_map>

#include "tau_tree.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

// rr name-id dict. Keep these `inline` without `static`: a process-wide
// registry needs one ODR-merged copy, or the same rr name maps to different
// ids in different translation units and rr_sig values stop being comparable.
/// Recurrence-relation names by id; id 0 is a placeholder, so real ids start
/// at 1.
inline std::vector<std::string> rr_v{ "dummy" };
/// Recurrence-relation ids by name; the inverse of `rr_v`.
inline std::map<std::string, size_t> rr_m{};

/**
 * @brief Map recurrence-relation name @p s to a stable integer id.
 * Interns @p s on first sight; grows the process-wide `rr_v`/`rr_m` registry
 * (not thread-safe).
 * @param s Name string to intern.
 * @return Integer id for @p s, at least 1, the same for every later call.
 */
inline size_t rr_dict(const std::string& s) {
	if (auto it = rr_m.find(s); it != rr_m.end()) return it->second;
	return rr_m.emplace(s, rr_v.size()), rr_v.push_back(s), rr_v.size() - 1;
};
/**
 * @brief Return the recurrence-relation name registered under @p i,
 * without validation; requires a valid id in `[0, rr_v.size())`.
 * @param i Id previously returned by `rr_dict(string)`.
 * @return The registered name (a reference into `rr_v`, invalidated when a
 * new name is interned).
 */
inline const std::string& rr_dict_name(size_t i) {
	DBG(assert(i < rr_v.size());)
	return rr_v[i];
}

/**
 * @brief Return the recurrence-relation name registered under @p i.
 *
 * An id at or past `rr_v.size()` is an out-of-range report, never a throw.
 * @param i Id previously returned by `rr_dict(string)`.
 * @return The name as a value, or an `out_of_range` error for an unknown id.
 */
inline result<std::string> rr_dict(size_t i) {
	result<std::string> r;
	if (i >= rr_v.size())
		return r.with_assert_check_error(code::out_of_range,
			"the recurrence-relation id is invalid",
			{{label::actual, i}, {label::limit, rr_v.size()}});
	return r.with_assert_check_value(std::string(rr_dict_name(i)));
}

/**
 * @brief Signature of a recurrence relation: name, offset arity, and argument arity.
 *
 * Ordering and equality use the default three-way comparison over all three fields.
 */
struct rr_sig {
	size_t name = 0;          ///< Name id from `rr_dict`.
	size_t offset_arity = 0;  ///< Number of offset (time-shift) parameters.
	size_t arg_arity = 0;     ///< Number of arguments.
	auto operator<=>(const rr_sig&) const = default;
};

/**
 * @brief Pretty-print @p s as `name[offset_arity]/arg_arity`.
 * @param os Destination stream.
 * @param s Signature to print.
 * @return @p os.
 */
inline std::ostream& operator<<(std::ostream& os, const rr_sig& s) {
	return os
		<< rr_dict_name(s.name)
		<< "[" << s.offset_arity << "]/"
		<< s.arg_arity;
}

} // namespace idni::tau_lang

/** @brief Hash specialization for `rr_sig` enabling use in unordered containers. */
template<>
struct std::hash<idni::tau_lang::rr_sig> {
	size_t operator()(const idni::tau_lang::rr_sig& s) const noexcept {
		std::uint64_t seed = 0;
		idni::hash_combine(seed, s.name);
		idni::hash_combine(seed, s.offset_arity);
		idni::hash_combine(seed, s.arg_arity);
		return static_cast<size_t>(seed);
	}
};

#endif // __IDNI__TAU__REF_TYPES_INFERENCE_H__