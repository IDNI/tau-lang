// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__IO_CONTEXT_H__
#define __IDNI__TAU__IO_CONTEXT_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "io_context"

namespace idni::tau_lang {

// abstract base class for input streams
struct serialized_constant_input_stream {
	virtual ~serialized_constant_input_stream() = default;
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() = 0;
	virtual std::optional<std::string> get() = 0;
	// optional override for time_point-dependent input streams
	virtual std::optional<std::string> get(size_t /*time_point*/) {
		// DBG(LOG_TRACE << "serialized_constant_input_stream::get(time_point: " << time_point << ")";)
		return this->get();
	}
};

// abstract base class for output streams
struct serialized_constant_output_stream {
	virtual ~serialized_constant_output_stream() = default;
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() = 0;
	virtual bool put(const std::string& value) = 0;
	virtual bool put(const std::string& value, size_t /*time_point*/) {
		// DBG(LOG_TRACE << "serialized_constant_output_stream::put(value: " << value << ", time_point: " << time_point << ")";)
		return this->put(value);
	}
};

template <NodeType node>
using input_streams = subtree_map<node,
	std::shared_ptr<serialized_constant_input_stream>>;
template <NodeType node>
using output_streams = subtree_map<node,
	std::shared_ptr<serialized_constant_output_stream>>;
using input_streams_remap  = std::map<std::string,
	std::shared_ptr<serialized_constant_input_stream>>;
using output_streams_remap = std::map<std::string,
	std::shared_ptr<serialized_constant_output_stream>>;

// console (without prompt) input and output streams
struct console_input_stream : public serialized_constant_input_stream {
	virtual ~console_input_stream() = default;
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	virtual std::optional<std::string> get() override;
};

struct console_output_stream : public serialized_constant_output_stream {
	virtual ~console_output_stream() = default;
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	virtual bool put(const std::string& value) override;
};

// console (with prompt) input and output streams
struct console_prompt_input_stream : public console_input_stream {
	console_prompt_input_stream(const std::string& name);
	virtual ~console_prompt_input_stream() = default;
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	using console_input_stream::get;
	virtual std::optional<std::string> get(size_t time_point) override;
private:
	std::string name;
};

struct console_prompt_output_stream : public console_output_stream {
	console_prompt_output_stream(const std::string& name);
	virtual ~console_prompt_output_stream() = default;
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	using console_output_stream::put;
	virtual bool put(const std::string& value, size_t time_point) override;
private:
	std::string name;
};

// file input and output streams
struct file_input_stream : public serialized_constant_input_stream {
	file_input_stream(const std::string& filename);
	virtual ~file_input_stream();
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	virtual std::optional<std::string> get() override;
protected:
	const std::string filename;
	std::ifstream file;
};

struct file_output_stream : public serialized_constant_output_stream {
	file_output_stream(const std::string& filename);
	virtual ~file_output_stream();
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	virtual bool put(const std::string& value) override;
protected:
	const std::string filename;
	std::ofstream file;
};

// vector input and output streams (used for testing or by using API)
struct vector_input_stream : public serialized_constant_input_stream {
	vector_input_stream(const std::vector<std::string>& values);
	vector_input_stream(std::shared_ptr<std::vector<std::string>> values,
		std::shared_ptr<size_t> current);
	virtual ~vector_input_stream() = default;
	virtual std::shared_ptr<serialized_constant_input_stream> rebuild() override;
	virtual std::optional<std::string> get() override;
protected:
	std::shared_ptr<std::vector<std::string>> values;
	std::shared_ptr<size_t> current = 0;
};

struct vector_output_stream : public serialized_constant_output_stream {
	vector_output_stream();
	vector_output_stream(const std::shared_ptr<std::vector<std::string>>& values);
	virtual ~vector_output_stream() = default;
	virtual std::shared_ptr<serialized_constant_output_stream> rebuild() override;
	virtual bool put(const std::string& value) override;
	std::vector<std::string> get_values() const;
protected:
	std::shared_ptr<std::vector<std::string>> values;
};

// context for input and output streams and their types
// It's usually populated manually or by Tau tree transformation and type inference
// It's used by interpreter as a description of IO interfaces of a specification.
template <NodeType node>
struct io_context {
	subtree_map<node, size_t> types;           // var -> ba type id
	subtree_map<node, size_t> inputs;          // var -> stream name id
	subtree_map<node, size_t> outputs;         // var -> stream name id
	input_streams_remap       input_remaps;    // var name -> stream
	output_streams_remap      output_remaps;   // var name -> stream
	// returns type of an IO variable contained in the context
	size_t type_of(tref var) const;
	// updates types of IO variables by global_scope we get from type inference
	void update_types(const subtree_map<node, size_t>& global_scope);
	tref add_input_console(const std::string& name, size_t type_id);
	tref add_output_console(const std::string& name, size_t type_id);
	tref add_input_console_no_prompt(const std::string& name, size_t type_id);
	tref add_output_console_no_prompt(const std::string& name, size_t type_id);
	tref add_input_file(const std::string& name, size_t type_id, const std::string& filename);
	tref add_output_file(const std::string& name, size_t type_id, const std::string& filename);
	tref add_input(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_input_stream> stream);
	tref add_output(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_output_stream> stream);
};

template <NodeType node>
std::ostream& operator<<(std::ostream& os, const io_context<node>& ctx);

} // namespace idni::tau_lang

#include "io_context.tmpl.h"

#endif // __IDNI__TAU__IO_CONTEXT_H__