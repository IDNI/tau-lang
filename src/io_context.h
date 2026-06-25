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
 * @brief Bundles stream mappings and type information for interpreter I/O.
 *
 * Usually populated manually or by Tau tree transformation and type inference.
 * Used by the interpreter as a description of I/O interfaces of a specification.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct io_context {
	subtree_map<node, size_t> types;           ///< IO variable → BA type id.
	subtree_map<node, size_t> inputs;          ///< IO variable → input stream name id.
	subtree_map<node, size_t> outputs;         ///< IO variable → output stream name id.
	input_streams_remap       input_remaps;    ///< Variable name → input stream.
	output_streams_remap      output_remaps;   ///< Variable name → output stream.

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

} // namespace idni::tau_lang

#include "io_context.tmpl.h"

#endif // __IDNI__TAU__IO_CONTEXT_H__
