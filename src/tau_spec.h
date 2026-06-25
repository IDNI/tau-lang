// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

/**
 * @file tau_spec.h
 * @brief Incremental Tau specification parser.
 *
 * `tau_spec<node>` accumulates Tau source parts (e.g., from a streaming REPL)
 * and produces a complete `rr<node>` (recurrence relation) when parsing
 * is complete.
 */

#ifndef __IDNI__TAU__TAU_SPEC_H__
#define __IDNI__TAU__TAU_SPEC_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Incremental parser for a Tau specification.
 *
 * Accepts source text in one or more parts and constructs an `rr<node>` once
 * all parts have been parsed successfully. Tracks parse errors internally.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_spec {
	using tau = tree<node>;

	/** @brief Construct an empty specification (no parts yet). */
	tau_spec();
	/**
	 * @brief Parse @p tau_spec_part and append it to the current specification.
	 * @param tau_spec_part Source string for the next part.
	 * @return `true` on success.
	 */
	bool parse(const std::string& tau_spec_part);
	/** @brief Return `true` if an EOF marker has been reached. */
	bool is_eof() const;
	/** @brief Return all parse errors collected so far. */
	const std::vector<std::string>& errors() const;
	/**
	 * @brief Return the most recently built formula tree, or `nullptr`.
	 * @return Latest parsed tree ref.
	 */
	tref get();
	/**
	 * @brief Append a pre-built formula @p expr to the specification.
	 * @param expr Formula to add.
	 * @return `true` on success.
	 */
	bool add(tref expr);
	/**
	 * @brief Finalize and return the complete recurrence-relation structure.
	 * @return `rr<node>` if parsing succeeded, or `std::nullopt`.
	 */
	std::optional<rr<node>> get_nso_rr();

private:
	/// @brief Build tree-get options from current parser state.
	typename tau::get_options get_options() const;
	/// @brief Check for EOF and record @p error_msg if reached.
	bool eof_check(const std::string& error_msg);
	/// @brief Internal parse of @p input as @p part index.
	std::pair<bool, std::string> parse_(
		const std::string& input,
		size_t part);
	/// @brief Parse the already-loaded @p part index.
	bool parse_part(size_t part);
	/// @brief Parse @p part together with the previous part for context.
	bool parse_with_prev_part(size_t part);
	/// @brief Combine all parsed pieces into one formula tree.
	tref build_parse_tree();

	std::string current_part_{};
	std::string prev_part_{};
	std::vector<std::string> parts_{}; // TODO remove this if found unnecessary
	std::deque<tref> parsed_{};
	std::optional<std::string> eof_msg_{};
	std::vector<std::string> errors_{};
	trefs defs_{};
	tref main_ = nullptr;
};

/**
 * @brief Pretty-print the specification @p spec to @p os.
 * @tparam node Tree node type.
 */
template <NodeType node>
std::ostream& operator<<(std::ostream& os, const tau_spec<node>& spec);

} // namespace idni::tau_lang

#include "tau_spec.tmpl.h"

#endif // __IDNI__TAU__TAU_SPEC_H__
