// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "io_context.h"
#include "utility/term.h"
#include "adt_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "io_context"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// constant streams
// -----------------------------------------------------------------------------

// -- console_input_stream --

inline std::shared_ptr<serialized_constant_input_stream>
	console_input_stream::rebuild()
{
	return std::make_shared<console_input_stream>();
}

inline std::optional<std::string> console_input_stream::get() {
	std::string line;
	term::enable_getline_mode();
	std::getline(std::cin, line);
	term::disable_getline_mode();
	return line;
}

// -- console_output_stream --

inline std::shared_ptr<serialized_constant_output_stream>
	console_output_stream::rebuild()
{
	return std::make_shared<console_output_stream>();
}

inline bool console_output_stream::put(const std::string& value) {
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

inline console_prompt_input_stream::console_prompt_input_stream(
	const std::string& name) : name(name)
{
	max_length = std::max(max_length, name.length());
}

inline std::shared_ptr<serialized_constant_input_stream>
	console_prompt_input_stream::rebuild()
{
	return std::make_shared<console_prompt_input_stream>(name);
}

inline std::optional<std::string> console_prompt_input_stream::get(size_t time_point) {
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

// -- repl_pending_input_stream --

inline std::shared_ptr<serialized_constant_input_stream>
	repl_pending_input_stream::rebuild()
{
	return std::make_shared<repl_pending_input_stream>();
}

inline void repl_pending_input_stream::set(const std::string& value) {
	pending_value = value;
	awaiting_ = false;
}

inline std::optional<std::string> repl_pending_input_stream::get() {
	return get(0);
}

inline std::optional<std::string> repl_pending_input_stream::get(
	size_t time_point)
{
	if (pending_value) {
		delivered_value = std::move(*pending_value);
		delivered_time_point_ = time_point;
		pending_value.reset();
		awaiting_ = false;
		return *delivered_value;
	}
	// interpreter::read() aborts the whole step at the first stream without
	// a value, and the REPL then collects exactly one value before entering
	// the step again -- so a step needing several console inputs re-reads
	// the streams that already answered. Hand the same value out again for
	// the same time point; consuming it once made each restart re-prompt the
	// earlier streams and lose their values.
	if (delivered_value && delivered_time_point_ == time_point) {
		awaiting_ = false;
		return *delivered_value;
	}
	// A new time point: the step this value belonged to is done with it.
	delivered_value.reset();
	// no value yet: flag it and return "" so read() stops the step cleanly
	// (the REPL scans for the awaiting stream and prompts for a value)
	awaiting_ = true;
	awaiting_time_point_ = time_point;
	return std::string{};
}

// -- console_prompt_output_stream --

inline console_prompt_output_stream::console_prompt_output_stream(
	const std::string& name) : name(name)
{
	max_length = std::max(max_length, name.length());
}

inline std::shared_ptr<serialized_constant_output_stream>
	console_prompt_output_stream::rebuild()
{
	return std::make_shared<console_prompt_output_stream>(name);
}

inline bool console_prompt_output_stream::put(const std::string& value,
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

inline file_input_stream::file_input_stream(const std::string& filename)
	: serialized_constant_input_stream(), filename(filename)
{
	file.open(filename);
	if (!file.is_open())
		LOG_ERROR << "Failed to open file: '" << filename << "'";
}

inline file_input_stream::~file_input_stream() {
	if (file.is_open()) file.close();
}

inline std::shared_ptr<serialized_constant_input_stream>
	file_input_stream::rebuild()
{
	return std::make_shared<file_input_stream>(filename);
}

inline std::optional<std::string> file_input_stream::get() {
	std::string line;
	std::getline(file, line);
	DBG(LOG_TRACE << "file_input_stream(\"" << filename << "\"): get() = \"" << line << "\"";)
	return line;
}

// -- file_output_stream --

inline file_output_stream::file_output_stream(const std::string& filename)
	: serialized_constant_output_stream(), filename(filename)
{
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): open";)
	file.open(filename);
	if (!file.is_open())
		LOG_ERROR << "Failed to open file: '" << filename << "'";
}

inline std::shared_ptr<serialized_constant_output_stream>
	file_output_stream::rebuild()
{
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): rebuild";)
	return std::make_shared<file_output_stream>(filename);
}

inline file_output_stream::~file_output_stream() {
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): close";)
	if (file.is_open()) file.close();
}

inline bool file_output_stream::put(const std::string& value) {
	DBG(LOG_TRACE << "file_output_stream(\"" << filename << "\"): put(\"" << value << "\")";)
	file << value << std::endl;
	return file.good();
}

// -- vector_input_stream --

inline vector_input_stream::vector_input_stream(
	const std::vector<std::string>& values)
	: vector_input_stream(
		std::make_shared<std::vector<std::string>>(values),
		std::make_shared<size_t>(0)) {}

inline vector_input_stream::vector_input_stream()
	: vector_input_stream(std::vector<std::string>{}) {}

inline vector_input_stream::vector_input_stream(
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

inline std::shared_ptr<serialized_constant_input_stream>
	vector_input_stream::rebuild()
{
	DBG(LOG_TRACE << "vector_input_stream::rebuild() values.size(): " << values->size() << " current: " << current;)
	return std::make_shared<vector_input_stream>(values, current);
}

inline std::optional<std::string> vector_input_stream::get() {
	// Signal exhaustion the same way console_input_stream/file_input_stream
	// signal EOF: an empty string (the graceful "no more inputs" quit
	// path in interpreter::read), not nullopt (the hard-error path) --
	// running out of a finite, pre-supplied input list is the expected
	// end of a scripted/test run, not a broken stream.
	if (*current >= values->size()) return std::string{};
	DBG(LOG_TRACE << "vector_input_stream::get() = \"" << values->at(*current)
		<< "\" current: " << *current << " values.size(): " << values->size();)
	return values->at((*current)++);
}

inline void vector_input_stream::put(const std::string& value) {
	values->push_back(value);
	DBG(LOG_TRACE << "vector_input_stream::put(\"" << value << "\") values.size(): " << values->size();)
}

// -- vector_output_stream --

inline vector_output_stream::vector_output_stream()
	: serialized_constant_output_stream(),
	values(std::make_shared<std::vector<std::string>>()),
	current(std::make_shared<size_t>(0))
{
	DBG(LOG_TRACE << "vector_output_stream::vector_output_stream()";)
}

inline vector_output_stream::vector_output_stream(const std::shared_ptr<std::vector<std::string>>& values)
	: serialized_constant_output_stream(),
	values(values),
	current(std::make_shared<size_t>(0))
{
	DBG(LOG_TRACE << "vector_output_stream::vector_output_stream(values) values.size(): " << values->size();)
}

inline std::shared_ptr<serialized_constant_output_stream>
	vector_output_stream::rebuild()
{
	DBG(LOG_TRACE << "vector_output_stream::rebuild()";)
	return std::make_shared<vector_output_stream>(values);
}

inline bool vector_output_stream::put(const std::string& value) {
	values->push_back(value);
	DBG(LOG_TRACE << "vector_output_stream::put(\"" << values->back() << "\") values.size(): " << values->size();)
	return true;
}

inline std::optional<std::string> vector_output_stream::get() {
	if (*current >= values->size()) return {};
	DBG(LOG_TRACE << "vector_output_stream::get() = \"" << values->at(*current)
		<< "\" current: " << *current << " values.size(): " << values->size();)
	return values->at((*current)++);
}

inline std::vector<std::string> vector_output_stream::get_values() const {
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

inline void vector_output_stream::clear() {
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
		htref hvar = tree<node>::geth(var);
		types[hvar] = type;
		// If already registered as input or output during parsing, skip name-based categorization.
		if (inputs.contains(var) || outputs.contains(var)) continue;
		std::string name = get_var_name<node>(var);
		DBG(LOG_TRACE << "updating stream: " << name;)
		const bool is_input  = name == "this"
					|| (!name.empty() && name[0] == 'i');
		const bool is_output = name == "u"
					|| (!name.empty() && name[0] == 'o');
		// EX-2: this used to be a DBG assert carrying a TODO asking
		// whether it should raise an undefined-io-stream error. It
		// should. As written, a name matching neither shape aborted a
		// debug build outright, while a release build (assert compiled
		// out) silently filed it under `outputs` -- so `zzz[t] = 0.`
		// crashed one configuration and became an output stream in the
		// other. Report it and register nothing: both configurations
		// now reach get_nso_rr's "I/O variable is not defined".
		if (!is_input && !is_output) {
			LOG_ERROR << "Undefined I/O stream: " << name
				<< " (a stream name must be \"this\", \"u\", or"
				   " start with 'i' or 'o')\n";
			continue;
		}
		auto& streams = is_input ? inputs : outputs;
		if (streams.find(var) == streams.end()) streams[hvar] = 0;
	}
}

template <NodeType node>
tref io_context<node>::add_input_console(const std::string& name, size_t type_id) {
	tref var = build_canonized_io_var<node>(name);
	htref hvar = tree<node>::geth(var);
	types.emplace(hvar, type_id);
	inputs.emplace(hvar, 0);
	return var;
}

template <NodeType node>
tref io_context<node>::add_input_console_no_prompt(const std::string& name, size_t type_id) {
	return add_input_console(name, type_id);
}

template <NodeType node>
tref io_context<node>::add_input_file(const std::string& name, size_t type_id, const std::string& filename) {
	tref var = build_canonized_io_var<node>(name);
	htref hvar = tree<node>::geth(var);
	types.emplace(hvar, type_id);
	inputs.emplace(hvar, dict(filename));
	return var;
}

template <NodeType node>
tref io_context<node>::add_output_console(const std::string& name, size_t type_id) {
	tref var = build_canonized_io_var<node>(name);
	htref hvar = tree<node>::geth(var);
	types.emplace(hvar, type_id);
	outputs.emplace(hvar, 0);
	return var;
}

template <NodeType node>
tref io_context<node>::add_output_console_no_prompt(const std::string& name, size_t type_id) {
	return add_output_console(name, type_id);
}

template <NodeType node>
tref io_context<node>::add_output_file(const std::string& name, size_t type_id, const std::string& filename) {
	tref var = build_canonized_io_var<node>(name);
	htref hvar = tree<node>::geth(var);
	types.emplace(hvar, type_id);
	outputs.emplace(hvar, dict(filename));
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

template <NodeType node>
void io_context<node>::clear() {
	types.clear();
	inputs.clear();
	outputs.clear();
	adt_streams.clear();
	input_remaps.clear();
	output_remaps.clear();
}

// -----------------------------------------------------------------------------
// ADT tuple wire format: parsing and shape validation
//
// Not templated on `node`: this only ever deals with plain strings/dict ids
// (member names) and the wire parser's own parse tree (`adt_parser`, from
// parser/adt.tgf), never with a tau tree. Kept as free functions/types at
// namespace scope so `adt_tuple_reader<node>` (templated) can reuse them
// unchanged regardless of `node`.
// -----------------------------------------------------------------------------

/** @brief Build the expected nesting shape from a layout's member paths. */
inline adt_shape_node adt_build_shape(
	const std::vector<std::vector<size_t>>& paths)
{
	adt_shape_node root;
	for (const auto& path : paths) {
		adt_shape_node* cur = &root;
		for (size_t i = 0; i < path.size(); ++i) {
			auto& child = cur->children[path[i]];
			if (i + 1 == path.size()) child.is_leaf = true;
			cur = &child;
		}
	}
	return root;
}

/** @brief Dotted description of a member path, for error messages. */
inline std::string adt_path_str(const std::vector<size_t>& path) {
	std::string s;
	for (size_t i = 0; i < path.size(); ++i) {
		if (i) s += ".";
		s += dict(path[i]);
	}
	return s;
}

/**
 * @brief A parsed wire literal: either a leaf string or a nested object
 * (member name dict id -> value), mirroring the wire grammar's
 * `leaf_value`/`tuple_value` alternation.
 */
struct adt_wire_value {
	bool is_leaf = false;
	std::string leaf;
	std::map<size_t, adt_wire_value> object;
};

// First descendant of @p nt reached by a plain pre-order walk from @p n
// (stopping AT the match, not descending into it). Safe to use for every
// single-slot lookup below (member_name within a member_key, value_chars
// within a leaf_value, and the outermost tuple_value from the parse's
// shaped root) because in adt.tgf's grammar each of those targets is
// reached before any same-typed node that could occur deeper (e.g. inside
// a NESTED tuple_value) -- see the file header of adt_wire_collect_members
// below for the one case (collecting a tuple_value's OWN member_value
// children) that is genuinely order-sensitive and therefore does NOT use
// this helper. NOT safe, and therefore NOT used, for a member_value's own
// key/value slots: see adt_parse_wire_tuple's own direct-children scan and
// its comment for why.
inline tref adt_wire_find(tref n, size_t nt) {
	if (!n) return nullptr;
	const auto& t = adt_parser::tree::get(n);
	if (!t.is_nt()) return nullptr;
	if (t.get_nt() == nt) return n;
	for (tref c : t.get_children())
		if (tref f = adt_wire_find(c, nt); f) return f;
	return nullptr;
}

// Collect @p tuple_value_node's own member_value children WITHOUT
// descending into a member_value's own nested tuple_value (a member whose
// value is itself a nested tuple owns a SEPARATE set of member_value
// children, collected separately when that member's value is processed) --
// unlike adt_wire_find above, this recursion stops precisely AT each
// member_value match rather than at the first node of the target type
// anywhere in the subtree, which is what makes it safe to call on a node
// that (transitively) contains more than one member_value.
//
// This is intentionally generic about what stands between tuple_value and
// its member_values: the grammar's own repetition wrapper nonterminals
// (__E_tuple_value_0/1, spelled out in adt.tgf's generated productions)
// never actually survive shaping -- node_to_inline
// (external/parser/src/parser_result.tmpl.h) unconditionally inlines any
// node whose name contains "__E_" (its EBNF-desugaring prefix), regardless
// of adt.tgf's own shaping_options -- so the real post-shaping structure is
// flatter than the raw grammar suggests (member_values end up interspersed
// with whatever surviving nodes remain, like the `_`/',' siblings, not
// wrapped in the __E_ nodes at all). Recursing generically through whatever
// children are actually there, rather than hardcoding those wrapper names,
// is what keeps this correct regardless.
inline void adt_wire_collect_members(tref n, trefs& out) {
	const auto& t = adt_parser::tree::get(n);
	if (!t.is_nt()) return;
	if (t.get_nt() == adt_parser::member_value) { out.push_back(n); return; }
	for (tref c : t.get_children()) adt_wire_collect_members(c, out);
}

/** @brief Parse one `tuple_value` parse-tree node into an `adt_wire_value`. */
inline std::optional<adt_wire_value> adt_parse_wire_tuple(tref tuple_value_node) {
	adt_wire_value result;
	trefs members;
	for (tref c : adt_parser::tree::get(tuple_value_node).get_children())
		adt_wire_collect_members(c, members);
	for (tref mv : members) {
		// member_value's key/value slots are found via a DIRECT-CHILDREN
		// scan, not adt_wire_find's "first match anywhere" pre-order search:
		// the parser framework unconditionally auto-inlines every node whose
		// name contains "__E_" (its EBNF-desugaring prefix -- see
		// node_to_inline, external/parser/src/parser_result.tmpl.h --
		// regardless of the shaping_options.to_inline/inline_char_classes
		// settings adt.tgf's own grammar_options configure), so
		// __E_member_value_2 (member_value's grammar-level `leaf_value |
		// tuple_value` alternation wrapper) never actually exists in the
		// shaped tree: its one child is spliced directly into member_value's
		// own children instead. A "first match anywhere" search for
		// leaf_value/tuple_value would therefore be unsafe here in a way it
		// isn't for adt_wire_find's other single-slot lookups: if mv's OWN
		// value is a nested tuple_value, that nested object's member_values
		// each have their own leaf_value/tuple_value descendants, and a
		// recursive "first match" search would find one of THOSE instead of
		// (correctly) concluding mv's own value is the tuple_value it
		// recursed past to get there. Scanning only mv's direct children
		// avoids this entirely.
		tref key_node = nullptr, leaf_node = nullptr, nested_node = nullptr;
		for (tref c : adt_parser::tree::get(mv).get_children()) {
			const auto& ct = adt_parser::tree::get(c);
			if (!ct.is_nt()) continue;
			if (ct.get_nt() == adt_parser::member_key) key_node = c;
			else if (ct.get_nt() == adt_parser::leaf_value) leaf_node = c;
			else if (ct.get_nt() == adt_parser::tuple_value) nested_node = c;
		}
		tref name_node = key_node
			? adt_wire_find(key_node, adt_parser::member_name) : nullptr;
		if (!name_node) {
			LOG_ERROR << "(Error) ADT wire: malformed member key\n";
			return std::nullopt;
		}
		size_t key_sid = dict(adt_parser::tree::get(name_node).get_terminals());
		if (result.object.contains(key_sid)) {
			LOG_ERROR << "(Error) ADT wire: duplicate key '"
				<< dict(key_sid) << "'\n";
			return std::nullopt;
		}
		adt_wire_value v;
		if (leaf_node) {
			tref vc = adt_wire_find(leaf_node, adt_parser::value_chars);
			v.is_leaf = true;
			v.leaf = vc ? adt_parser::tree::get(vc).get_terminals() : std::string{};
		} else {
			if (!nested_node) {
				LOG_ERROR << "(Error) ADT wire: malformed member value for '"
					<< dict(key_sid) << "'\n";
				return std::nullopt;
			}
			auto sub = adt_parse_wire_tuple(nested_node);
			if (!sub) return std::nullopt;
			v = std::move(*sub);
		}
		result.object.emplace(key_sid, std::move(v));
	}
	return result;
}

/** @brief Parse @p src (one wire literal line) into an `adt_wire_value`. */
inline std::optional<adt_wire_value> adt_parse_wire(const std::string& src) {
	auto result = adt_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		LOG_ERROR << "(Error) ADT wire: "
			<< result.parse_error.to_str(adt_parser::error::info_lvl::INFO_BASIC)
			<< "\n";
		return std::nullopt;
	}
	tref shaped = result.get_shaped_tree2();
	tref tv = adt_wire_find(shaped, adt_parser::tuple_value);
	if (!tv) {
		LOG_ERROR << "(Error) ADT wire: no tuple literal found in '" << src << "'\n";
		return std::nullopt;
	}
	return adt_parse_wire_tuple(tv);
}

// Validate @p wv against @p shape, collecting every leaf into @p out keyed
// by its full path (built up via @p path as the recursion descends).
// LOG_ERROR + false on a leaf/object shape mismatch, an unknown key, or a
// missing key.
inline bool adt_validate_collect(const adt_shape_node& shape,
	const adt_wire_value& wv, std::vector<size_t>& path,
	std::map<std::vector<size_t>, std::string>& out)
{
	if (shape.is_leaf) {
		if (!wv.is_leaf) {
			LOG_ERROR << "(Error) ADT wire: expected a leaf value at '"
				<< adt_path_str(path) << "', got a nested object\n";
			return false;
		}
		out[path] = wv.leaf;
		return true;
	}
	if (wv.is_leaf) {
		LOG_ERROR << "(Error) ADT wire: expected a nested object at '"
			<< adt_path_str(path) << "', got a leaf value\n";
		return false;
	}
	for (const auto& [key_sid, child_val] : wv.object) {
		auto it = shape.children.find(key_sid);
		if (it == shape.children.end()) {
			LOG_ERROR << "(Error) ADT wire: unknown key '" << dict(key_sid)
				<< "' at '" << adt_path_str(path) << "'\n";
			return false;
		}
		path.push_back(key_sid);
		bool ok = adt_validate_collect(it->second, child_val, path, out);
		path.pop_back();
		if (!ok) return false;
	}
	if (wv.object.size() != shape.children.size())
		for (const auto& [key_sid, child_shape] : shape.children) {
			(void)child_shape;
			if (!wv.object.contains(key_sid)) {
				LOG_ERROR << "(Error) ADT wire: missing key '" << dict(key_sid)
					<< "' at '" << adt_path_str(path) << "'\n";
				return false;
			}
		}
	return true;
}

// -----------------------------------------------------------------------------
// ADT tuple wire format: formatting (writer side)
// -----------------------------------------------------------------------------

// Ordered (insertion-order, NOT sorted) nesting tree built from a layout's
// components in layout order, so the formatted literal's member order
// matches the type's declared member order rather than dict-id order.
struct adt_fmt_node {
	bool is_leaf = false;
	std::string leaf;
	std::vector<std::pair<std::string, adt_fmt_node>> children;
};

inline void adt_fmt_insert(adt_fmt_node& node, const std::vector<size_t>& path,
	size_t depth, const std::string& leaf)
{
	if (depth == path.size()) { node.is_leaf = true; node.leaf = leaf; return; }
	std::string key = dict(path[depth]);
	for (auto& [k, child] : node.children)
		if (k == key) return adt_fmt_insert(child, path, depth + 1, leaf);
	node.children.emplace_back(key, adt_fmt_node{});
	adt_fmt_insert(node.children.back().second, path, depth + 1, leaf);
}

inline std::string adt_fmt_print(const adt_fmt_node& node) {
	if (node.is_leaf) return "\"" + node.leaf + "\"";
	std::string s = "{ ";
	bool first = true;
	for (const auto& [k, child] : node.children) {
		if (!first) s += ", ";
		first = false;
		s += k + ": " + adt_fmt_print(child);
	}
	s += " }";
	return s;
}

// -----------------------------------------------------------------------------
// adt_tuple_reader
// -----------------------------------------------------------------------------

template <NodeType node>
adt_tuple_reader<node>::adt_tuple_reader(
	std::unique_ptr<serialized_constant_input_stream> physical,
	const adt_stream_layout<node>& layout)
	: physical(std::move(physical)), layout(layout)
{
	std::vector<std::vector<size_t>> paths;
	for (const auto& c : this->layout.components) paths.push_back(c.path);
	shape = adt_build_shape(paths);
}

template <NodeType node>
typename adt_tuple_reader<node>::read_status
	adt_tuple_reader<node>::read_time_point(size_t time_point)
{
	if (memo_time_point && *memo_time_point == time_point)
		return memo_ok ? read_status::ok : read_status::failed;

	auto line = physical->get(time_point);
	if (!line) {
		LOG_ERROR << "(Error) ADT: failed to read tuple stream at time point "
			<< time_point << "\n";
		memo_time_point = time_point;
		memo_leaves.clear();
		memo_ok = false;
		return read_status::failed;
	}
	if (line->empty()) {
		// No value YET (a non-blocking console stream -- e.g. the REPL's
		// repl_pending_input_stream -- flags itself "awaiting" and returns
		// "" instead of blocking) or genuinely no more input (an exhausted
		// vector_input_stream/file_input_stream signals end-of-input the
		// same way; see those classes' own get()). Either way this is NOT a
		// malformed-wire-literal parse failure, so -- unlike every other
		// path below -- deliberately do NOT memoize it: a later call for
		// the SAME time_point (the REPL resuming this reader's physical
		// stream after the user answers) must re-consult the physical
		// stream, not reuse a stale "no value yet" memo forever. Leaving
		// memo_time_point untouched also means whatever time_point it
		// already held (a prior successful or genuinely-failed read) stays
		// validly memoized for ITS OWN time_point. Returning `empty` (not
		// `failed`) is what lets `leaf()` propagate this quietly instead of
		// logging an error -- see `leaf()`'s own comment.
		return read_status::empty;
	}

	memo_time_point = time_point;
	memo_leaves.clear();
	memo_ok = false;
	auto wv = adt_parse_wire(*line);
	if (!wv) return read_status::failed;
	std::vector<size_t> path;
	if (!adt_validate_collect(shape, *wv, path, memo_leaves)) return read_status::failed;
	memo_ok = true;
	return read_status::ok;
}

template <NodeType node>
std::optional<std::string> adt_tuple_reader<node>::leaf(size_t time_point,
	const std::vector<size_t>& path)
{
	switch (read_time_point(time_point)) {
	case read_status::failed: return std::nullopt; // already LOG_ERROR'd
	case read_status::empty:
		// Propagate the physical stream's own emptiness AS an empty leaf
		// (not nullopt): interpreter::read() (interpreter.tmpl.h) treats a
		// present-but-empty value as its quiet "no value yet"/EOF path
		// (`if (line.empty()) return { value, true };`), exactly like a
		// plain (non-tuple) stream's own `get()` already does -- returning
		// nullopt here instead (the pre-fix behavior) took the read()'s
		// OTHER branch, `!maybe_line.has_value()`, which LOG_ERRORs
		// "Failed to read from input stream" on every single awaiting/EOF
		// cycle, not just on an actual failure.
		return std::string{};
	case read_status::ok: break;
	}
	auto it = memo_leaves.find(path);
	if (it == memo_leaves.end()) {
		LOG_ERROR << "(Error) ADT: no leaf at '" << adt_path_str(path)
			<< "' for time point " << time_point << "\n";
		return std::nullopt;
	}
	return it->second;
}

// -----------------------------------------------------------------------------
// adt_member_input_stream
// -----------------------------------------------------------------------------

template <NodeType node>
std::shared_ptr<serialized_constant_input_stream>
	adt_member_input_stream<node>::rebuild()
{
	auto r = std::make_shared<adt_member_input_stream<node>>();
	r->reader = reader;
	r->path = path;
	return r;
}

template <NodeType node>
std::optional<std::string> adt_member_input_stream<node>::get(size_t time_point) {
	return reader->leaf(time_point, path);
}

template <NodeType node>
std::optional<std::string> adt_member_input_stream<node>::get() {
	return get(next_time_point++);
}

// -----------------------------------------------------------------------------
// adt_tuple_writer
// -----------------------------------------------------------------------------

template <NodeType node>
adt_tuple_writer<node>::adt_tuple_writer(
	std::unique_ptr<serialized_constant_output_stream> physical,
	const adt_stream_layout<node>& layout)
	: physical(std::move(physical)), layout(layout) {}

template <NodeType node>
std::string adt_tuple_writer<node>::format(
	const std::map<std::vector<size_t>, std::string>& leaves) const
{
	adt_fmt_node root;
	for (const auto& comp : layout.components) {
		auto it = leaves.find(comp.path);
		const std::string& leaf = it != leaves.end() ? it->second : std::string{};
		adt_fmt_insert(root, comp.path, 0, leaf);
	}
	return adt_fmt_print(root);
}

template <NodeType node>
bool adt_tuple_writer<node>::collect(size_t time_point,
	const std::vector<size_t>& path, const std::string& leaf)
{
	// Completeness is judged by which PATHS have been collected, not by
	// rec.size() alone: a repeated collect() for the same (time_point,
	// path) -- an interpreter retry, a Task 8 wiring bug, a spec writing
	// the same output var twice in one step -- must never overwrite the
	// original entry in place, since map::operator[] assignment keeps
	// rec.size() unchanged on an overwrite, silently making the record
	// permanently one member short of ever completing (a real bug: no
	// LOG_ERROR, no exception, just a pending entry that never fires). A
	// repeat is treated as record corruption: LOG_ERROR (naming the
	// stream, path, and time point) and discard the ENTIRE pending record
	// for this time point -- no partial trust in a record one of whose
	// members has already proven unreliable, mirroring the reader's own
	// discipline (a duplicate key anywhere in a wire literal fails the
	// WHOLE parse, not just that key). Any components that had already
	// arrived for this time point are lost; if the genuinely-missing
	// member(s) arrive afterward, they start a FRESH record that -- having
	// lost the discarded contributions -- can only complete if every
	// component (including the one that just repeated) is collected again
	// from scratch; in the common case (each output stream calls collect()
	// exactly once per time point) that means the record for this time
	// point is never emitted, which is the intended fail-safe: better a
	// silently-dropped step (logged) than a step written from stale data.
	auto pit = pending.find(time_point);
	if (pit != pending.end() && pit->second.contains(path)) {
		LOG_ERROR << "(Error) ADT: duplicate write to '"
			<< dict(layout.root_name_sid) << "." << adt_path_str(path)
			<< "' for time point " << time_point << "\n";
		pending.erase(pit);
		return false;
	}
	auto& rec = pending[time_point];
	rec[path] = leaf;
	if (rec.size() < layout.components.size()) return true; // still buffering
	std::string literal = format(rec);
	pending.erase(time_point);
	return physical->put(literal, time_point);
}

// -----------------------------------------------------------------------------
// adt_member_output_stream
// -----------------------------------------------------------------------------

template <NodeType node>
std::shared_ptr<serialized_constant_output_stream>
	adt_member_output_stream<node>::rebuild()
{
	auto r = std::make_shared<adt_member_output_stream<node>>();
	r->writer = writer;
	r->path = path;
	return r;
}

template <NodeType node>
bool adt_member_output_stream<node>::put(const std::string& value, size_t time_point) {
	return writer->collect(time_point, path, value);
}

template <NodeType node>
bool adt_member_output_stream<node>::put(const std::string& value) {
	return put(value, next_time_point++);
}

// -----------------------------------------------------------------------------
// ADT stream introspection helpers (member -> owning layout, wire-shaped hint)
// -----------------------------------------------------------------------------

template <NodeType node>
const adt_stream_layout<node>* find_adt_stream_for_member(
	const io_context<node>& ctx, tref var)
{
	using tau = tree<node>;
	for (const auto& [root_sid, layout] : ctx.adt_streams) {
		(void)root_sid;
		for (const auto& c : layout.components)
			if (tau::subtree_equals(c.io_var->get(), var)) return &layout;
	}
	return nullptr;
}

template <NodeType node>
std::string adt_wire_hint(const adt_stream_layout<node>& layout) {
	adt_fmt_node root;
	for (const auto& c : layout.components)
		adt_fmt_insert(root, c.path, 0, std::string{});
	return adt_fmt_print(root);
}

} // namespace idni::tau_lang
