// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The reading of the Boolean operations on tau-typed values during a run.

#ifndef __IDNI__TAU__BA_READING_H__
#define __IDNI__TAU__BA_READING_H__

namespace idni::tau_lang {

/**
 * @brief Depth of the interpreter activity in progress (tau_ba_trace_scope).
 * Inside a run the Boolean operations on tau-typed stream values keep the
 * trace reading, the one the pointwise revision checks use, whatever the
 * `ba-complement` option says (ba_complement_program).
 */
inline unsigned tau_ba_trace_depth = 0;

/// Marks an interpreter activity for its lifetime (tau_ba_trace_depth).
struct tau_ba_trace_scope {
	tau_ba_trace_scope() { ++tau_ba_trace_depth; }
	~tau_ba_trace_scope() { --tau_ba_trace_depth; }
	tau_ba_trace_scope(const tau_ba_trace_scope&) = delete;
	tau_ba_trace_scope& operator=(const tau_ba_trace_scope&) = delete;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BA_READING_H__
