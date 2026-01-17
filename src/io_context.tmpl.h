// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "io_context.h"
#include "utility/term.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "io_context"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// constant streams
// -----------------------------------------------------------------------------

// -- console_input_stream --

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

// -- console_output_stream --

std::shared_ptr<serialized_constant_output_stream>
	console_output_stream::rebuild()
{
	return std::make_shared<console_output_stream>();
}

bool console_output_stream::put(const std::string& value) {
	std::cout << value << std::endl;
	return true;
}

// console_prompt_*_stream spacing helper
inline std::string spacing(const std::string& name, size_t max_length) {
	return (max_length <= name.length())
		? std::string{}
		: std::string(max_length - name.length(), ' ');
}

// -- console_prompt_input_stream --

console_prompt_input_stream::console_prompt_input_stream(
	const std::string& name) : name(name)
{
	max_length = std::max(max_length, name.length());
}

std::shared_ptr<serialized_constant_input_stream>
	console_prompt_input_stream::rebuild()
{
	return std::make_shared<console_prompt_input_stream>(name);
}

std::optional<std::string> console_prompt_input_stream::get(size_t time_point) {
	DBG(LOG_TRACE << "console_prompt_input_stream::get(name: "
		<< name << ", time_point: " << time_point << ", max_length: "
		<< max_length << ", name.length(): " << name.length() << ")";)

	std::cout << name << "[" << time_point << "]"
		<< spacing(name, max_length) << " := ";

#ifdef DEBUG
	std::optional<std::string> result = this->get();
	std::stringstream ss;
	ss << "console_prompt_input_stream::get[result]: ";
	if (result.has_value()) ss << "\"" << result.value() << "\"";
	else ss << "error";
	LOG_TRACE << ss.str();
	return result;
#endif // DEBUG

	return this->get();
}

// -- console_prompt_output_stream --

console_prompt_output_stream::console_prompt_output_stream(
	const std::string& name) : name(name)
{
	max_length = std::max(max_length, name.length());
}

std::shared_ptr<serialized_constant_output_stream>
	console_prompt_output_stream::rebuild()
{
	return std::make_shared<console_prompt_output_stream>(name);
}

bool console_prompt_output_stream::put(const std::string& value,
	size_t time_point)
{
	DBG(LOG_TRACE << "console_prompt_output_stream::put(name: "
		<< name << ", time_point: " << time_point << ", value: "
		<< value << ", max_length: " << max_length << ", name.length(): "
		<< name.length() << ")";)

	std::cout << name << "[" << time_point << "]"
		<< spacing(name, max_length) << " := ";
	bool result = this->put(value);

	DBG(LOG_TRACE << "console_prompt_output_stream::put[result]: " << result << ")";)
	return result;
}

// -- file_input_stream --

file_input_stream::file_input_stream(const std::string& filename)
	: serialized_constant_input_stream(), filename(filename)
{
	file.open(filename);
	if (!file.is_open())
		LOG_ERROR << "Failed to open file: '" << filename << "'";
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

// -- file_output_stream --

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

// -- vector_input_stream --

vector_input_stream::vector_input_stream(
	const std::vector<std::string>& values)
	: vector_input_stream(
		std::make_shared<std::vector<std::string>>(values),
		std::make_shared<size_t>(0)) {}

vector_input_stream::vector_input_stream()
	: vector_input_stream(std::vector<std::string>{}) {}

vector_input_stream::vector_input_stream(
	std::shared_ptr<std::vector<std::string>> values,
	std::shared_ptr<size_t> current)
	: serialized_constant_input_stream(), values(values), current(current)
{
#ifdef DEBUG
	std::stringstream ss;
	ss << "vector_input_stream::vector_input_stream({";
	bool first = true;
	for (const auto& value : *values)
		ss << (first ? (first = false, " ") : ", ") << "{ " << value << " }";
	ss << " }, " << *current << ")";
	LOG_TRACE << ss.str();
#endif
}

std::shared_ptr<serialized_constant_input_stream>
	vector_input_stream::rebuild()
{
	DBG(LOG_TRACE << "vector_input_stream::rebuild() values.size(): " << values->size() << " current: " << current;)
	return std::make_shared<vector_input_stream>(values, current);
}

std::optional<std::string> vector_input_stream::get() {
	if (*current >= values->size()) return {};
	DBG(LOG_TRACE << "vector_input_stream::get() = \"" << values->at(*current)
		<< "\" current: " << *current << " values.size(): " << values->size();)
	return values->at((*current)++);
}

void vector_input_stream::put(const std::string& value) {
	values->push_back(value);
	DBG(LOG_TRACE << "vector_input_stream::put(\"" << value << "\") values.size(): " << values->size();)
}

// -- vector_output_stream --

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

std::optional<std::string> vector_output_stream::get() {
	if (*current >= values->size()) return {};
	DBG(LOG_TRACE << "vector_output_stream::get() = \"" << values->at(*current)
		<< "\" current: " << *current << " values.size(): " << values->size();)
	return values->at((*current)++);
}

std::vector<std::string> vector_output_stream::get_values() const {
#ifdef DEBUG
	std::stringstream ss;
	ss << "vector_output_stream::get_values() = {";
	bool first = true;
	for (const auto& value : *values)
		ss << (first ? (first = false, " ") : ", ") << "{ " << value << " }";
	ss << " })";
	LOG_TRACE << ss.str();
#endif
	return *values;
}

void vector_output_stream::clear() {
	values->clear();
	*current = 0;
	DBG(LOG_TRACE << "vector_output_stream::clear_values() values.size(): " << values->size();)
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
	for (const auto& [var, type] : update) if (is_io_var<node>(var)) {
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
tref io_context<node>::add_input_console_no_prompt(const std::string& name, size_t type_id) {
	return add_input_console(name, type_id, std::make_shared<console_input_stream>());
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
tref io_context<node>::add_output_console_no_prompt(const std::string& name, size_t type_id) {
	return add_output_console(name, type_id, std::make_shared<console_output_stream>());
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
