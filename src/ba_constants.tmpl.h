// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ba_types.h"
#include "ba_constants.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_constants"

template <NodeType node>
typename ba_constants<node>::constant_pool& ba_constants<node>::C() {
	static constant_pool* c = new constant_pool();
	return *c;
}

template <NodeType node>
htrefs& ba_constants<node>::T() {
	static htrefs* t = new htrefs();
	return *t;
}

template <NodeType node>
typename ba_constants<node>::pool_index& ba_constants<node>::index_() {
	static pool_index* i = new pool_index();
	return *i;
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant, size_t type_id) {
	// After cleanup() interning would pool into a freshly reset pool.
	// Poisoning is a programming error, not a reportable input failure,
	// so it is caught by the debug assert only.
	DBG(assert(!poisoned && "ba_constants::get called after cleanup()");)
	// No tracing here: this runs for every constant the bv evaluation
	// hooks fold during a step, and the pool lookup is index-backed.
	auto p = std::make_pair(constant, type_id);
	if (auto it = index_().find(p); it != index_().end())
		return T()[it->second]->get();
	C().emplace_back(p);
	size_t constant_id = C().size();
	index_().emplace(std::move(p), constant_id - 1);
	node n = node::ba_constant(constant_id, type_id);
	tref r = tree<node>::get(n);
	T().push_back(tree<node>::geth(r));
	return r;
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant, tref type_tree) {
	return get(constant, ba_types<node>::id(type_tree));
}

template <NodeType node>
result<typename ba_constants<node>::constant> ba_constants<node>::get(
	size_t constant_id)
{
	// constant_id == 0 underflows to SIZE_MAX below, so validate it here.
	if (constant_id == 0 || constant_id > C().size()) {
		result<constant> r;
		return r.with_assert_check_error(code::out_of_range,
			"the Boolean-algebra constant id is invalid",
			{{label::actual, constant_id}, {label::limit, C().size()}});
	}
	return result<constant>{C()[constant_id - 1].first};
}

/**
 * @internal
 * @brief Split @p value written as `{ v }` or `{ v }:T`, optionally ended by
 * the REPL's `.`, into `v` and `T` without spaces (`T` empty when absent).
 * @return nullopt when @p value is not exactly one braced constant, as in
 * `{ a } & { b }`.
 * @endinternal
 */
inline std::optional<std::pair<std::string, std::string>>
	split_braced_constant(std::string_view value)
{
	auto is_space = [](char c) {
		return std::isspace(static_cast<unsigned char>(c)) != 0; };
	auto trim = [&](std::string_view s) {
		while (!s.empty() && is_space(s.front())) s.remove_prefix(1);
		while (!s.empty() && is_space(s.back())) s.remove_suffix(1);
		return s;
	};
	value = trim(value);
	if (value.empty() || value.front() != '{') return std::nullopt;
	size_t depth = 0, close = std::string_view::npos;
	for (size_t i = 0; i < value.size(); ++i) {
		if (value[i] == '{') ++depth;
		else if (value[i] == '}' && --depth == 0) { close = i; break; }
	}
	if (close == std::string_view::npos) return std::nullopt;
	std::string_view rest = trim(value.substr(close + 1));
	if (rest.ends_with('.')) rest = trim(rest.substr(0, rest.size() - 1));
	std::string type;
	if (!rest.empty()) {
		if (rest.front() != ':') return std::nullopt;
		for (char c : rest.substr(1)) if (!is_space(c)) type += c;
		if (type.empty()) return std::nullopt;
	}
	return std::make_pair(std::string(trim(value.substr(1, close - 1))),
		std::move(type));
}

template <NodeType node>
result<typename node::constant_with_type> ba_constants<node>::get_input(
	const std::string& value, tref type_tree)
{
	result<typename node::constant_with_type> r;
	auto braced = split_braced_constant(value);
	if (!braced) return get(value, type_tree);
	auto& [source, type] = *braced;
	if (!type.empty()) {
		TAU_TRY(std::string stream_type,
			get_ba_type_name<node>(get_ba_type_id<node>(type_tree)));
		// The name carries a leading ':', a subtype as `[n]`.
		std::string_view expected = stream_type;
		if (expected.starts_with(':')) expected.remove_prefix(1);
		std::string_view family = expected.substr(0, expected.find('['));
		if (type != expected && type != family)
			return r.with_assert_check_error(code::type_error,
				"the type of the input value is not the type of the stream",
				{{label::value, truncate_for_message(value)},
				 {label::expected, std::string(expected)},
				 {label::type_name, type}});
	}
	TAU_TRY(auto cnst, get(source, type_tree));
	return r.with_value(std::move(cnst));
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	os << "BA constants pool(" << C().size() << "):\n";
	for (size_t i = 0; i < C().size(); ++i) os << LOG_INDENT << "constant: "
		<< i+1 << " " << LOG_BA(C()[i].first) << " : " << LOG_FM(T()[i]->get()) << "\n";
	return os;
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

template <NodeType node>
void ba_constants<node>::cleanup() {
	// All three pools live behind leaked function-local statics (see
	// C()/T()/index_() above), so none runs a static destructor at exit;
	// they are cleared together here to keep their indices aligned.
	C().clear();
	T().clear();
	index_().clear();
	poisoned = true; // any later get() must fail loudly, see header
}

} // namespace idni::tau_lang
