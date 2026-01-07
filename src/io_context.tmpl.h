// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "io_context.h"
#include "utility/term.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "io_context"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// constant streams
// -----------------------------------------------------------------------------

std::shared_ptr<serialized_constant_input_stream>
	console_input_stream::rebuild()
{
	return std::make_shared<console_input_stream>();
}

std::optional<std::string> console_input_stream::get() {
	std::string line;
	term::enable_getline_mode();
	std::getline(std::cin, line);
	term::disable_getline_mode();
	return line;
}

std::shared_ptr<serialized_constant_output_stream>
	console_output_stream::rebuild()
{
	return std::make_shared<console_output_stream>();
}

bool console_output_stream::put(const std::string& value) {
	std::cout << value << std::endl;
	return true;
}

file_input_stream::file_input_stream(const std::string& filename)
	: serialized_constant_input_stream(), filename(filename)
{
	file.open(filename);
	if (!file.is_open())
		LOG_ERROR << "Failed to reopen file: '" << filename << "'";
}

file_input_stream::~file_input_stream() {
	if (file.is_open()) file.close();
}

std::shared_ptr<serialized_constant_input_stream>
	file_input_stream::rebuild()
{
	return std::make_shared<file_input_stream>(filename);
}

std::optional<std::string> file_input_stream::get() {
	std::string line;
	std::getline(file, line);
	DBG(LOG_TRACE << "file_input_stream(\"" << filename << "\"): get() = \"" << line << "\"";)
	return line;
}

file_output_stream::file_output_stream(const std::string& filename)
	: serialized_constant_output_stream(), filename(filename)
{
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): open";)
	file.open(filename);
	if (!file.is_open())
		LOG_ERROR << "Failed to open file: '" << filename << "'";
}

std::shared_ptr<serialized_constant_output_stream>
	file_output_stream::rebuild()
{
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): rebuild";)
	return std::make_shared<file_output_stream>(filename);
}

file_output_stream::~file_output_stream() {
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): close";)
	if (file.is_open()) file.close();
}

bool file_output_stream::put(const std::string& value) {
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): put(\"" << value << "\")";)
	file << value << std::endl;
	return true;
}

vector_input_stream::vector_input_stream(const std::vector<std::string>& values)
	: serialized_constant_input_stream(), values(values)
{
	DBG(LOG_TRACE << "vector_input_stream::vector_input_stream() values.size(): " << values.size();)
	for (const auto& value : values) {
		DBG(LOG_TRACE << "vector_input_stream::vector_input_stream() value: " << value << "\n";)
	}
}

std::shared_ptr<serialized_constant_input_stream>
	vector_input_stream::rebuild()
{
	DBG(LOG_TRACE << "vector_input_stream::rebuild() values.size(): " << values.size();)
	return std::make_shared<vector_input_stream>(values);
}

std::optional<std::string> vector_input_stream::get() {
	if (current >= values.size()) return {};
	DBG(LOG_TRACE << "vector_input_stream::get() = \"" << values[current] << "\" current: " << current << " values.size(): " << values.size();)
	return values[current++];
}

vector_output_stream::vector_output_stream()
	: serialized_constant_output_stream(),
	values(std::make_shared<std::vector<std::string>>())
{
	DBG(LOG_TRACE << "vector_output_stream::vector_output_stream()";)
}

vector_output_stream::vector_output_stream(const std::shared_ptr<std::vector<std::string>>& values)
	: serialized_constant_output_stream(),
	values(values)
{
	DBG(LOG_TRACE << "vector_output_stream::vector_output_stream(values) values.size(): " << values->size();)
}

std::shared_ptr<serialized_constant_output_stream>
	vector_output_stream::rebuild()
{
	DBG(LOG_TRACE << "vector_output_stream::rebuild()";)
	return std::make_shared<vector_output_stream>(values);
}

bool vector_output_stream::put(const std::string& value) {
	values->push_back(value);
	DBG(LOG_TRACE << "vector_output_stream::put(\"" << values->back() << "\") values.size(): " << values->size();)
	return true;
}

std::vector<std::string> vector_output_stream::get_values() const {
	DBG(LOG_TRACE << "vector_output_stream::get_values() values.size(): " << values->size();)
	for (const auto& value : *values) {
		DBG(LOG_TRACE << "vector_output_stream::get_values() value: " << value << "\n";)
	}
	return *values;
}

// -----------------------------------------------------------------------------
// IO context
// -----------------------------------------------------------------------------

template <NodeType node>
size_t io_context<node>::type_of(tref var) const {
	if (auto it = types.find(canonize<node>(var));
		it != types.end()) return it->second;
	return 0;
}

template <NodeType node>
void io_context<node>::update_types(
	const subtree_map<node, size_t>& update)
{
	// update types of inputs and outputs
	// and create a default console stream if not defined
	for (auto& [var, type] : update) if (is_io_var<node>(var)) {
		types[var] = type;
		std::string name = get_var_name<node>(var);
		DBG(LOG_TRACE << "updating stream: " << name;)
		bool is_input = name == "this" || name[0] == 'i';
		DBG(assert(is_input || name == "u" || name[0] == 'o');) // TODO: should this raise an undefined io stream error?
		auto& streams = is_input ? inputs : outputs;
		if (streams.find(var) == streams.end()) streams[var] = 0;
	}
}

template <NodeType node>
tref io_context<node>::add_input_console(const std::string& name, size_t type_id) {
	tref var = build_canonized_io_var<node>(name);
	types.emplace(var, type_id);
	inputs.emplace(var, 0);
	return var;
}

template <NodeType node>
tref io_context<node>::add_input_file(const std::string& name, size_t type_id, const std::string& filename) {
	tref var = build_canonized_io_var<node>(name);
	types.emplace(var, type_id);
	inputs.emplace(var, dict(filename));
	return var;
}

template <NodeType node>
tref io_context<node>::add_output_console(const std::string& name, size_t type_id) {
	tref var = build_canonized_io_var<node>(name);
	types.emplace(var, type_id);
	outputs.emplace(var, 0);
	return var;
}

template <NodeType node>
tref io_context<node>::add_output_file(const std::string& name, size_t type_id, const std::string& filename) {
	tref var = build_canonized_io_var<node>(name);
	types.emplace(var, type_id);
	outputs.emplace(var, dict(filename));
	return var;
}

template <NodeType node>
tref io_context<node>::add_input(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_input_stream> stream) {
	tref var = add_input_console(name, type_id);
	input_remaps.emplace(name, stream);
	return var;
}

template <NodeType node>
tref io_context<node>::add_output(const std::string& name, size_t type_id, std::shared_ptr<serialized_constant_output_stream> stream) {
	tref var = add_output_console(name, type_id);
	output_remaps.emplace(name, stream);
	return var;
}

} // namespace idni::tau_lang
