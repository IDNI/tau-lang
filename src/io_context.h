// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

/**
 * @file io_context.h
 * @brief I/O stream abstractions and context for Tau execution.
 *
 * Defines the abstract base classes for serialized input/output streams
 * (`serialized_constant_input_stream`, `serialized_constant_output_stream`),
 * concrete implementations (console, file, vector), and `io_context<node>`
 * which bundles stream mappings for the interpreter.
 */

#ifndef __IDNI__TAU__IO_CONTEXT_H__
#define __IDNI__TAU__IO_CONTEXT_H__

#include <functional>

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "io_context"

namespace idni::tau_lang {

/**
 * @brief Abstract base class for serialized constant input streams.
 *
 * Consumers read values one at a time via `get()`. An optional time-point
 * overload allows streams that vary by step number.
 */
struct serialized_constant_input_stream {
	virtual ~serialized_constant_input_stream() = default;
	/** @brief Produce a fresh, rewound copy of this stream. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() = 0;
	/** @brief Read the next value, or return `std::nullopt` at end-of-stream. */
	virtual std::optional<std::string> get() = 0;
	/**
	 * @brief Read the value for @p time_point (default: delegates to `get()`).
	 * @param time_point Simulation step number.
	 * @return Value at @p time_point, or `std::nullopt`.
	 */
	virtual std::optional<std::string> get(size_t /*time_point*/) {
		// DBG(LOG_TRACE << "serialized_constant_input_stream::get(time_point: " << time_point << ")";)
		return this->get();
	}
};

/**
 * @brief Abstract base class for serialized constant output streams.
 *
 * Consumers write values one at a time via `put()`. An optional time-point
 * overload allows streams that vary by step number.
 */
struct serialized_constant_output_stream {
	virtual ~serialized_constant_output_stream() = default;
	/** @brief Produce a fresh, empty copy of this stream. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() = 0;
	/**
	 * @brief Write @p value to the stream.
	 * @return `true` on success.
	 */
	virtual bool put(const std::string& value) = 0;
	/**
	 * @brief Write @p value for @p time_point (default: delegates to `put(value)`).
	 * @param value Value to write.
	 * @param time_point Simulation step number.
	 * @return `true` on success.
	 */
	virtual bool put(const std::string& value, size_t /*time_point*/) {
		// DBG(LOG_TRACE << "serialized_constant_output_stream::put(value: " << value << ", time_point: " << time_point << ")";)
		return this->put(value);
	}
};

/** @brief Map from IO variable tree nodes to their input stream instances. */
template <NodeType node>
using input_streams = subtree_map<node,
	std::shared_ptr<serialized_constant_input_stream>>;
/** @brief Map from IO variable tree nodes to their output stream instances. */
template <NodeType node>
using output_streams = subtree_map<node,
	std::shared_ptr<serialized_constant_output_stream>>;
/** @brief Map from IO variable name strings to input stream instances. */
using input_streams_remap  = std::map<std::string,
	std::shared_ptr<serialized_constant_input_stream>>;
/** @brief Map from IO variable name strings to output stream instances. */
using output_streams_remap = std::map<std::string,
	std::shared_ptr<serialized_constant_output_stream>>;

/**
 * @brief Input stream that reads from stdin without a prompt.
 */
struct console_input_stream : public serialized_constant_input_stream {
	virtual ~console_input_stream() = default;
	/** @brief Rebuild by returning a new `console_input_stream`. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	/** @brief Read one line from stdin. */
	virtual std::optional<std::string> get() override;
};

/**
 * @brief Output stream that writes to stdout without a prompt.
 */
struct console_output_stream : public serialized_constant_output_stream {
	virtual ~console_output_stream() = default;
	/** @brief Rebuild by returning a new `console_output_stream`. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	/** @brief Write @p value to stdout. */
	virtual bool put(const std::string& value) override;
};

/**
 * @brief Input stream that prompts the user before reading from stdin.
 */
struct console_prompt_input_stream : public console_input_stream {
	/** @brief Construct with variable @p name used as the prompt label. */
	console_prompt_input_stream(const std::string& name);
	virtual ~console_prompt_input_stream() = default;
	/** @brief Rebuild by returning a new `console_prompt_input_stream`. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	using console_input_stream::get;
	/** @brief Prompt with the step number @p time_point, then read. */
	virtual std::optional<std::string> get(size_t time_point) override;
private:
	std::string name;
	inline static size_t max_length = 0;
};

/** @brief Non-blocking input stream for the REPL's interactive `run`: holds
 * one `set()` value. `get()` returns it, or (when none is set) marks itself
 * `awaiting()` and returns "" so the step stops cleanly; the REPL then finds
 * the awaiting stream and prompts for a value (no exceptions, for WASM). The
 * prompt label and type handling are the REPL's concern, not the stream's. */
struct repl_pending_input_stream : public serialized_constant_input_stream {
	repl_pending_input_stream() = default;
	virtual ~repl_pending_input_stream() = default;
	/** @brief Rebuild by returning a new `repl_pending_input_stream`. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	virtual std::optional<std::string> get() override;
	/** @brief Return the set value, or "" after flagging `awaiting()`. */
	virtual std::optional<std::string> get(size_t time_point) override;
	/** @brief Set the value the REPL just read from the user. */
	void set(const std::string& value);
	/** @brief True when the last `get()` had no value and wants one. */
	bool awaiting() const { return awaiting_; }
	/** @brief Time point of the awaited value (valid when `awaiting()`). */
	size_t awaiting_time_point() const { return awaiting_time_point_; }
private:
	std::optional<std::string> pending_value;
	// Value already handed out for `delivered_time_point_`, kept so that a
	// step re-entered for another stream's value reads the same input again.
	std::optional<std::string> delivered_value;
	size_t delivered_time_point_ = 0;
	bool awaiting_ = false;
	size_t awaiting_time_point_ = 0;
};

/**
 * @brief Output stream that labels each write with the variable name.
 */
struct console_prompt_output_stream : public console_output_stream {
	/** @brief Construct with variable @p name used as the output label. */
	console_prompt_output_stream(const std::string& name);
	virtual ~console_prompt_output_stream() = default;
	/** @brief Rebuild by returning a new `console_prompt_output_stream`. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	using console_output_stream::put;
	/** @brief Write @p value labelled with @p time_point. */
	virtual bool put(const std::string& value, size_t time_point) override;
private:
	std::string name;
	inline static size_t max_length = 0;
};

/**
 * @brief Input stream that reads values line-by-line from a file.
 */
struct file_input_stream : public serialized_constant_input_stream {
	/** @brief Open the file at @p filename for reading. */
	file_input_stream(const std::string& filename);
	virtual ~file_input_stream();
	/** @brief Rebuild by reopening the file from the beginning. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	/** @brief Read the next line from the file. */
	virtual std::optional<std::string> get() override;
protected:
	const std::string filename;
	std::ifstream file;
};

/**
 * @brief Output stream that appends values line-by-line to a file.
 */
struct file_output_stream : public serialized_constant_output_stream {
	/** @brief Open the file at @p filename for writing. */
	file_output_stream(const std::string& filename);
	virtual ~file_output_stream();
	/** @brief Rebuild by reopening the file. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	/** @brief Write @p value followed by a newline to the file. */
	virtual bool put(const std::string& value) override;
protected:
	const std::string filename;
	std::ofstream file;
};

/**
 * @brief Input stream backed by an in-memory vector of strings.
 *
 * Used for testing or when injecting values via the C++ API.
 */
struct vector_input_stream : public serialized_constant_input_stream {
	/** @brief Construct an empty stream. */
	vector_input_stream();
	/** @brief Construct pre-loaded with @p values. */
	vector_input_stream(const std::vector<std::string>& values);
	/**
	 * @brief Construct sharing ownership of @p values and @p current position.
	 * @param values Shared value store.
	 * @param current Shared read cursor.
	 */
	vector_input_stream(std::shared_ptr<std::vector<std::string>> values,
		std::shared_ptr<size_t> current);
	virtual ~vector_input_stream() = default;
	/** @brief Rebuild by resetting the cursor to the beginning. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	/** @brief Return the next value, or `std::nullopt` when exhausted. */
	virtual std::optional<std::string> get() override;
	/** @brief Append @p value to the backing store. */
	virtual void put(const std::string& value);
protected:
	std::shared_ptr<std::vector<std::string>> values;
	std::shared_ptr<size_t> current;
};

/**
 * @brief Output stream that accumulates values in an in-memory vector.
 *
 * Used for testing or when capturing output via the C++ API.
 */
struct vector_output_stream : public serialized_constant_output_stream {
	/** @brief Construct with a fresh empty backing store. */
	vector_output_stream();
	/** @brief Construct sharing @p values as the backing store. */
	vector_output_stream(const std::shared_ptr<std::vector<std::string>>& values);
	virtual ~vector_output_stream() = default;
	/** @brief Rebuild by resetting the read cursor. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	/** @brief Append @p value to the backing store. */
	virtual bool put(const std::string& value) override;
	/** @brief Return the next stored value, or `std::nullopt` when exhausted. */
	virtual std::optional<std::string> get();
	/** @brief Return a copy of all stored values. */
	std::vector<std::string> get_values() const;
	/** @brief Clear the backing store. */
	virtual void clear();
protected:
	std::shared_ptr<std::vector<std::string>> values;
	std::shared_ptr<size_t> current;
};

/**
 * @brief One flattened member of a tuple-typed io stream, as recorded by the
 * ADT flattener (`adt_flatten_rewrite_io_def`, src/adt/adt_flatten.tmpl.h).
 *
 * `io_var` is the CANONIZED handle of this member's own flattened io var --
 * i.e. `tree<node>::geth(build_canonized_io_var<node>(name))` where `name`
 * is the dotted member name (e.g. `"p.a"`). This is deliberately the same
 * shape `ctx.inputs`/`ctx.outputs` are keyed by (see `adt_stream_layout`'s
 * file comment below) and the same shape `canonize<node>` (ba_types_inference.tmpl.h)
 * produces for a live occurrence of that member: the ADT flattener folds the
 * member path into the io var's OWN `var_name` (`io_var(var_name("p.a"),
 * offset)`), rather than keeping a sibling `member_path` node, specifically
 * so that `canonize<node>` -- which only ever inspects an io_var's own
 * `var_name` child and is otherwise unmodified by the ADT feature -- keeps
 * distinguishing members from each other. The visible cost is cosmetic: the
 * pretty-printer renders this as `p.a[t]` (var_name text, then the offset
 * bracket) rather than the design note's `p[t].a`.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_stream_component {
	std::vector<size_t> path;        ///< Member-name dict ids, outer -> inner.
	htref io_var = nullptr;          ///< Canonized handle of this member's own io var.
	tref  base_type = nullptr;       ///< Member's base BA type tree.
};

/**
 * @brief Layout record for one tuple-typed io stream: how a physical
 * stream's flattened members are grouped back together for the wire format.
 *
 * Built by the ADT flattener when a tuple-typed `input_def`/`output_def` is
 * expanded (`adt_flatten_rewrite_io_def`); consumed by `adt_tuple_reader`/
 * `adt_tuple_writer` below and by the interpreter's stream construction.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_stream_layout {
	size_t root_name_sid = 0;  ///< Dict id of the stream's root variable name (e.g. `"p"`).
	bool   is_input = true;    ///< `true` for an input stream, `false` for output.
	size_t stream_id = 0;      ///< File name dict id; `0` means console.
	std::vector<adt_stream_component<node>> components; ///< Flattened members, in type order.
};

/**
 * @brief Bundles stream mappings and type information for interpreter I/O.
 *
 * Usually populated manually or by Tau tree transformation and type inference.
 * Used by the interpreter as a description of I/O interfaces of a specification.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct io_context {
	subtree_htref_map<node, size_t> types;     ///< IO variable → BA type id.
	subtree_htref_map<node, size_t> inputs;    ///< IO variable → input stream name id.
	subtree_htref_map<node, size_t> outputs;   ///< IO variable → output stream name id.
	/// Tuple-typed io stream layouts, keyed by the stream's root variable
	/// name dict id (`adt_stream_layout::root_name_sid`). Populated by the
	/// ADT flattener; the root's own entry in `inputs`/`outputs` is removed
	/// once its members are registered there instead (see
	/// `adt_flatten_rewrite_io_def`, src/adt/adt_flatten.tmpl.h).
	std::map<size_t, adt_stream_layout<node>> adt_streams;
	input_streams_remap       input_remaps;    ///< Variable name → input stream.
	output_streams_remap      output_remaps;   ///< Variable name → output stream.
	/// Optional override for building a stream-id-0 console input stream;
	/// null keeps the blocking default. The REPL sets a non-blocking one.
	std::function<std::shared_ptr<serialized_constant_input_stream>(
		const std::string& name)> console_input_factory;

	/**
	 * @brief Return the BA type id of IO variable @p var.
	 * @param var IO variable node.
	 * @return BA type identifier.
	 */
	size_t type_of(tref var) const;
	/**
	 * @brief Update the BA types of IO variables from @p global_scope.
	 * @param global_scope Map of variable → type id from type inference.
	 */
	void update_types(const subtree_map<node, size_t>& global_scope);

	/** @brief Register a prompting console input stream for @p name with @p type_id. */
	tref add_input_console(const std::string& name, size_t type_id);
	/** @brief Register a prompting console output stream for @p name with @p type_id. */
	tref add_output_console(const std::string& name, size_t type_id);
	/** @brief Register a non-prompting console input stream for @p name with @p type_id. */
	tref add_input_console_no_prompt(const std::string& name, size_t type_id);
	/** @brief Register a non-prompting console output stream for @p name with @p type_id. */
	tref add_output_console_no_prompt(const std::string& name, size_t type_id);
	/** @brief Register a file input stream reading from @p filename for @p name with @p type_id. */
	tref add_input_file(const std::string& name, size_t type_id, const std::string& filename);
	/** @brief Register a file output stream writing to @p filename for @p name with @p type_id. */
	tref add_output_file(const std::string& name, size_t type_id, const std::string& filename);
	/** @brief Register an arbitrary input @p stream for @p name with @p type_id. */
	tref add_input(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_input_stream> stream);
	/** @brief Register an arbitrary output @p stream for @p name with @p type_id. */
	tref add_output(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_output_stream> stream);
	/** @brief Remove all registered streams and types. */
	void clear();
};

/**
 * @brief Pretty-print the I/O context @p ctx to @p os.
 * @tparam node Tree node type.
 */
template <NodeType node>
std::ostream& operator<<(std::ostream& os, const io_context<node>& ctx);

// -----------------------------------------------------------------------------
// ADT tuple stream reader/writer
//
// One physical stream (console/file/in-memory) carries a JSON-like tuple
// literal per time point for a tuple-typed io stream (design doc
// private/2026-08-05-adt-design.md, section 4), while the flattened formula
// only ever sees the flat member io vars (`p.a`, `p.b`, ...). These classes
// bridge the two: `adt_tuple_reader`/`adt_tuple_writer` own the ONE physical
// stream and know the stream's `adt_stream_layout`; `adt_member_input_stream`/
// `adt_member_output_stream` are thin per-member adapters -- ordinary
// `serialized_constant_input_stream`/`serialized_constant_output_stream`
// implementations the interpreter can register into its usual `inputs`/
// `outputs` maps exactly like any other stream -- that share one reader/
// writer (and therefore one physical stream) per stream root. See
// io_context.tmpl.h for the wire-format parsing (via `adt_parser`, the wire
// grammar's generated parser) and formatting.
// -----------------------------------------------------------------------------

/**
 * @brief The expected nesting shape of a tuple stream's wire literal,
 * derived from an `adt_stream_layout`'s components (one entry per member
 * name at each nesting level; a leaf marks a flat member's own position).
 * Used by `adt_tuple_reader` to validate a parsed wire literal (unknown/
 * missing key, leaf/object shape mismatch) and to route each leaf to its
 * flat member path.
 */
struct adt_shape_node {
	bool is_leaf = false;
	std::map<size_t, adt_shape_node> children; ///< Keyed by member name dict id.
};

/**
 * @brief Reads and validates one tuple wire literal per time point for a
 * tuple-typed input stream.
 *
 * Reads are memoized per time point: the first `leaf()` call for a given
 * time point reads and parses the physical stream's line once, validating it
 * against the layout (every component present, no unknown/duplicate key, no
 * leaf/object shape mismatch); later `leaf()` calls for the SAME time point
 * (one per flat member) reuse the parsed result instead of re-reading the
 * physical stream.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_tuple_reader {
	/**
	 * @brief Wrap @p physical, reading tuple literals validated against @p layout.
	 * @param physical The stream's single underlying physical stream.
	 * @param layout This stream's flattened member layout.
	 */
	adt_tuple_reader(std::unique_ptr<serialized_constant_input_stream> physical,
		const adt_stream_layout<node>& layout);
	/**
	 * @brief Return the raw leaf string at @p path for @p time_point.
	 * @param time_point Simulation step number.
	 * @param path Member path (dict ids, outer -> inner) to read.
	 * @return The leaf string; an empty string (`""`, `has_value()`) if the
	 * PHYSICAL stream's own read for @p time_point was itself empty -- no
	 * value yet (a non-blocking console stream, e.g. the REPL's
	 * `repl_pending_input_stream`, flags itself "awaiting" and returns `""`
	 * instead of blocking) or genuinely no more input (an exhausted
	 * `vector_input_stream`/`file_input_stream` signals end-of-input the
	 * same way) -- mirroring exactly what a plain (non-tuple) stream
	 * already does for `interpreter::read()`'s own `line.empty()` quiet
	 * path (`interpreter.tmpl.h`), so a tuple stream's "no value yet"/EOF
	 * is handled identically to a plain stream's, not logged as an error;
	 * or `std::nullopt` (after `LOG_ERROR`) on an actual physical read
	 * failure, a wire-format parse failure, or a missing/duplicate/unknown
	 * key or leaf/object shape mismatch against the layout -- those keep
	 * hard-error + memoized-failure semantics, unchanged.
	 */
	std::optional<std::string> leaf(size_t time_point,
		const std::vector<size_t>& path);
	/**
	 * @brief The group's single physical stream.
	 *
	 * For callers that need to see past this reader to introspect the
	 * actual physical stream -- e.g. the REPL's `continue_running`
	 * (`repl_evaluator.tmpl.h`), which scans `interpreter::inputs` for an
	 * awaiting `repl_pending_input_stream` and must look through an
	 * `adt_member_input_stream`'s shared reader to find it.
	 */
	std::shared_ptr<serialized_constant_input_stream> physical_stream() const {
		return physical;
	}
private:
	/// @brief `read_time_point`'s outcome for @p time_point: `ok` (parsed
	/// and validated, memoized), `empty` (the physical stream itself had
	/// no value yet/EOF -- deliberately NOT memoized, so a later call for
	/// the same @p time_point re-consults the physical stream once it has
	/// something), or `failed` (a physical read/parse/validation error,
	/// already `LOG_ERROR`'d and memoized).
	enum class read_status { ok, empty, failed };
	read_status read_time_point(size_t time_point);

	std::shared_ptr<serialized_constant_input_stream> physical;
	adt_stream_layout<node> layout;
	adt_shape_node shape; ///< Member-path shape derived from `layout`, built once.
	std::optional<size_t> memo_time_point;
	bool memo_ok = false;
	std::map<std::vector<size_t>, std::string> memo_leaves;
};

/**
 * @brief Per-member input stream adapter sharing one `adt_tuple_reader` (and
 * therefore one physical stream) with its sibling members.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_member_input_stream : public serialized_constant_input_stream {
	std::shared_ptr<adt_tuple_reader<node>> reader; ///< Shared with sibling members.
	std::vector<size_t> path;                        ///< This member's own path.
	virtual ~adt_member_input_stream() = default;
	/** @brief Rebuild by returning a new adapter sharing the same reader/path. */
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	/** @brief Route to `reader->leaf(time_point, path)`. */
	virtual std::optional<std::string> get(size_t time_point) override;
	/** @brief Sequential variant: reads at an internally tracked, self-advancing time point. */
	virtual std::optional<std::string> get() override;
private:
	size_t next_time_point = 0;
};

/**
 * @brief Buffers flat member values per time point for a tuple-typed output
 * stream and formats/writes the nested tuple literal once every member has
 * been collected.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_tuple_writer {
	/**
	 * @brief Wrap @p physical, formatting tuple literals per @p layout.
	 * @param physical The stream's single underlying physical stream.
	 * @param layout This stream's flattened member layout.
	 */
	adt_tuple_writer(std::unique_ptr<serialized_constant_output_stream> physical,
		const adt_stream_layout<node>& layout);
	/**
	 * @brief Buffer @p leaf at @p path for @p time_point.
	 *
	 * Once every layout component for @p time_point has been collected,
	 * formats the nested tuple literal (members in layout order, nesting
	 * rebuilt from their paths) and `put()`s it to the physical stream.
	 * @return `true` while still buffering, or the physical stream's `put()`
	 * result once the record completed.
	 */
	bool collect(size_t time_point, const std::vector<size_t>& path,
		const std::string& leaf);
private:
	std::string format(const std::map<std::vector<size_t>, std::string>& leaves) const;

	std::shared_ptr<serialized_constant_output_stream> physical;
	adt_stream_layout<node> layout;
	std::map<size_t, std::map<std::vector<size_t>, std::string>> pending; ///< time_point -> path -> leaf.
};

/**
 * @brief Per-member output stream adapter sharing one `adt_tuple_writer`
 * (and therefore one physical stream) with its sibling members.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct adt_member_output_stream : public serialized_constant_output_stream {
	std::shared_ptr<adt_tuple_writer<node>> writer; ///< Shared with sibling members.
	std::vector<size_t> path;                        ///< This member's own path.
	virtual ~adt_member_output_stream() = default;
	/** @brief Rebuild by returning a new adapter sharing the same writer/path. */
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	/** @brief Route to `writer->collect(time_point, path, value)`. */
	virtual bool put(const std::string& value, size_t time_point) override;
	/** @brief Sequential variant: writes at an internally tracked, self-advancing time point. */
	virtual bool put(const std::string& value) override;
private:
	size_t next_time_point = 0;
};

/**
 * @brief Find the `adt_stream_layout` that @p var is a flattened member of.
 *
 * Used by callers that need to go from a member io var (as stored in
 * `interpreter::inputs`/`outputs`) back to its stream's root -- e.g. the
 * REPL's `continue_running`, to label an awaiting console prompt with the
 * tuple stream's own root name rather than one member's dotted name.
 * @param ctx IO context whose `adt_streams` to search.
 * @param var Canonized io var to look up.
 * @return Pointer to the owning layout (borrowed; valid as long as @p ctx
 * outlives it and isn't mutated), or `nullptr` if @p var isn't a member of
 * any tuple-typed stream currently registered in @p ctx.
 */
template <NodeType node>
const adt_stream_layout<node>* find_adt_stream_for_member(
	const io_context<node>& ctx, tref var);

/**
 * @brief A wire-format-shaped hint for @p layout, with placeholder (empty)
 * leaf values -- e.g. `{ a: "", b: "" }` -- for prompting a tuple-typed
 * stream's whole literal at once (one physical stream, one wire literal per
 * time point; see the design note above `adt_shape_node`).
 */
template <NodeType node>
std::string adt_wire_hint(const adt_stream_layout<node>& layout);

} // namespace idni::tau_lang

#include "io_context.tmpl.h"

#endif // __IDNI__TAU__IO_CONTEXT_H__
