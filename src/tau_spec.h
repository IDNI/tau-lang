// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
	 *
	 * Each line is one part. A part that ends early (unexpected end of
	 * file) is kept pending and joined with the next one; a part that fails
	 * on its own is retried joined to the previous part, as a continuation.
	 * @param tau_spec_part Source string for the next part(s).
	 * @return `true` when every line parsed or awaits more input; `false` on
	 * the first syntax error, which is recorded in errors(), or when an
	 * earlier error is still recorded.
	 */
	bool parse(const std::string& tau_spec_part);
	/** @brief Return `true` while the last part ended early and more input is expected. */
	bool is_eof() const;
	/** @brief Return all parse errors collected so far. */
	const std::vector<std::string>& errors() const;
	/**
	 * @brief Build the spec tree from every part parsed and every formula
	 * added so far.
	 *
	 * Types are inferred against the global definitions, whose io context
	 * and global scope this updates. The build follows the mode set by
	 * keep_warm_ups() or keep_as_written(); by default construction hooks
	 * run and quantifier ids are canonized.
	 * @return The spec tree; an error when input is still pending (recorded
	 * in errors() until a later parse() continues it), when a parse error is
	 * recorded, when there is no main formula or several, or when
	 * transformation or type inference fails.
	 */
	result<tref> get();
	/**
	 * @brief Append a pre-built tree @p expr to the specification.
	 *
	 * A `spec` contributes its main formula and definitions, a `wff` or `bf`
	 * becomes the main formula, an io def or `rec_relation` a definition,
	 * and a `type_def` replaces any earlier type of the same name.
	 * @param expr Tree to add; may be null.
	 * @return `false` when @p expr is null, of another node kind, or a second
	 * main formula (recorded in errors()); `true` otherwise.
	 */
	bool add(tref expr);
	/**
	 * @brief Finalize and return the complete recurrence-relation structure.
	 * @return the `rr<node>` with its report; a failed report when parsing
	 * or resolution fails.
	 */
	result<rr<node>> get_nso_rr();
	/**
	 * @brief Keep the warm-up of each clause of the main formula at its
	 * lookback as written (pin_written_warm_ups) in what get() builds,
	 * read positively. Execution sets it.
	 */
	void keep_warm_ups() { mode_ = build_mode::pinned; }
	/**
	 * @brief Build the specification as written: types inferred, no
	 * construction hook run, so a decision procedure can still read the
	 * warm-up of each clause (pin_written_warm_ups) under the polarity it
	 * decides. The procedures that decide it fold it themselves.
	 */
	void keep_as_written() { mode_ = build_mode::as_written; }
	/// @brief The main formula as it was added, or `nullptr` until a
	/// parsed spec is built.
	tref main() const { return main_; }

private:
	/// @brief Build tree-get options from current parser state.
	typename tau::get_options get_options() const;
	/// @brief Record @p error_msg as the pending end-of-input message and
	/// return `true` when it reports an unexpected end of file.
	bool eof_check(const std::string& error_msg);
	/// @brief Parse @p input into `parsed_[part]`; returns whether it parsed
	/// and, when not, the parser's error message.
	std::pair<bool, std::string> parse_(
		const std::string& input,
		size_t part);
	/// @brief Parse part @p part, joined to a pending one; `false` on a
	/// recorded error.
	bool parse_part(size_t part);
	/// @brief Retry @p part joined to the previous part, as its continuation;
	/// `true` when that parses or ends early.
	bool parse_with_prev_part(size_t part);
	/// @brief Combine all parsed parts into one `spec` parse tree; an error
	/// (also recorded in errors()) for several main formulas or none.
	result<tref> build_parse_tree();

	std::string current_part_{};
	std::string prev_part_{};
	std::vector<std::string> parts_{}; // TODO remove this if found unnecessary
	std::deque<tref> parsed_{};
	std::optional<std::string> eof_msg_{};
	// GR-R4: set when get() recorded eof_msg_ in errors_; a later
	// continuation parse() removes that entry again (non-sticky).
	bool eof_error_reported_ = false;
	std::vector<std::string> errors_{};
	trefs defs_{};
	tref main_ = nullptr;
	enum class build_mode { folded, as_written, pinned };
	build_mode mode_ = build_mode::folded;
	// span a spec's parts, since a type declared in one part must stay
	// live for the parts parsed after it; mutable so the const
	// get_options() can hand out their addresses
	mutable htrefs type_defs_{};
	mutable tau_dynamic_context names_{};

	// TT2-2: the streaming operator reads parts_/parsed_.
	template <NodeType n>
	friend std::ostream& operator<<(std::ostream&, const tau_spec<n>&);
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
