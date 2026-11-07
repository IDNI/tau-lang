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
std::vector<size_t>& ba_constants<node>::free_ids() {
	static std::vector<size_t>* f = new std::vector<size_t>();
	return *f;
}

template <NodeType node>
std::mutex& ba_constants<node>::mutex() {
	static std::mutex* m = new std::mutex();
	return *m;
}

template <NodeType node>
typename ba_constants<node>::pool_index& ba_constants<node>::index_() {
	static pool_index* i = new pool_index();
	return *i;
}

template <NodeType node>
void ba_constants<node>::forget_built_nodes() {
	// A gc callback runs after every sweep, collect() or not, and for
	// tree::clear_caches(): dropping a cached node is safe either way.
	static const bool registered = [] {
		std::unique_lock lock(bintree<node>::mutex());
		bintree<node>::gc_callbacks.push_back(
			[](const std::unordered_set<tref>& kept) {
				std::lock_guard lock(mutex());
				for (slot& s : C())
					if (s.built && !kept.contains(s.built))
						s.built = nullptr;
			});
		return true;
	}();
	(void)registered;
}

template <NodeType node>
void ba_constants<node>::collect(const std::unordered_set<tref>& survivors) {
	// destroyed after the lock is released: declared before it
	std::vector<constant> dead;
	std::lock_guard lock(mutex());
	++collects;
	auto& pool = C();
	std::vector<bool> named(pool.size()), freed(pool.size());
	for (tref t : survivors) {
		const node& v = bintree<node>::get(t).value;
		if (v.nt != node::type::ba_constant) continue;
		if (size_t id = v.get_ba_constant_id(); id && id <= pool.size())
			named[id - 1] = true;
	}
	bool any = false;
	for (size_t i = 0; i < pool.size(); ++i) {
		slot& s = pool[i];
		if (!s.value) continue;
		if (named[i] || s.used_at + 1 >= collects) continue;
		dead.push_back(std::move(s.value->first));
		s.value.reset();
		free_ids().push_back(i);
		freed[i] = any = true;
	}
	if (!any) return;
	// extract, not erase: the key holds a copy of the constant, destroyed
	// with the others after the lock is released
	for (auto it = index_().begin(); it != index_().end(); )
		if (freed[it->second]) {
			auto next = std::next(it);
			dead.push_back(std::move(index_().extract(it).key().first));
			it = next;
		} else ++it;
}

template <NodeType node>
size_t ba_constants<node>::size() {
	std::lock_guard lock(mutex());
	return C().size() - free_ids().size();
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant, size_t type_id) {
	// After cleanup() interning would pool into a freshly reset pool.
	// Poisoning is a programming error, not a reportable input failure,
	// so it is caught by the debug assert only.
	DBG(assert(!poisoned && "ba_constants::get called after cleanup()");)
	// No tracing here: this runs for every constant the bv evaluation
	// hooks fold during a step, and the pool lookup is index-backed.
	forget_built_nodes();
	size_t constant_id;
	{
		std::lock_guard lock(mutex());
		auto p = std::make_pair(constant, type_id);
		if (auto it = index_().find(p); it != index_().end()) {
			slot& s = C()[it->second];
			s.used_at = collects;
			if (s.built) return s.built;
			constant_id = it->second + 1;
		} else {
			size_t pos = C().size();
			if (!free_ids().empty())
				pos = free_ids().back(), free_ids().pop_back();
			else C().emplace_back();
			slot& s = C()[pos];
			s.value.emplace(p);
			s.built = nullptr;
			s.used_at = collects;
			index_().emplace(std::move(p), pos);
			constant_id = pos + 1;
		}
	}
	// Built without the pool's lock: the node's hash reads the pool, and a
	// sweep takes the tree store's lock before the pool's.
	tref r = tree<node>::get(node::ba_constant(constant_id, type_id));
	std::lock_guard lock(mutex());
	C()[constant_id - 1].built = r;
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
	std::lock_guard lock(mutex());
	if (constant_id == 0 || constant_id > C().size()
		|| !C()[constant_id - 1].value)
	{
		result<constant> r;
		return r.with_assert_check_error(code::out_of_range,
			"the Boolean-algebra constant id is invalid",
			{{label::actual, constant_id}, {label::limit, C().size()}});
	}
	return result<constant>{C()[constant_id - 1].value->first};
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
	// copied first: printing a constant may read the pool again
	std::vector<std::pair<size_t, constant>> live;
	{
		std::lock_guard lock(mutex());
		for (size_t i = 0; i < C().size(); ++i)
			if (C()[i].value) live.emplace_back(i + 1, C()[i].value->first);
	}
	os << "BA constants pool(" << live.size() << "):\n";
	for (const auto& [id, c] : live)
		os << LOG_INDENT << "constant: " << id << " " << LOG_BA(c) << "\n";
	return os;
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

template <NodeType node>
void ba_constants<node>::cleanup() {
	// The pools live behind leaked function-local statics (see C() and
	// index_() above), so none runs a static destructor at exit; they are
	// cleared together here to keep their indices aligned.
	std::lock_guard lock(mutex());
	C().clear();
	free_ids().clear();
	index_().clear();
	poisoned = true; // any later get() must fail loudly, see header
}

} // namespace idni::tau_lang
