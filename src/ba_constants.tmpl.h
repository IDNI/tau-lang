// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_constants.h"
#include "nso_factory.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA types

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types"

template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_types<BAs...>::type_id(size_t type_sid) {
	if (types.empty()) types.push_back(string_id("untyped")),
			type_names_map.emplace(types.back(), 0);
	if (auto it = type_names_map.find(type_sid);
		it != type_names_map.end()) return it->second;
	return type_names_map.emplace(type_sid, types.size()),
		types.push_back(type_sid), types.size() - 1;
}

template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_types<BAs...>::type_id(const std::string& name) {
	// TODO properly initialize types with "untyped" as first element
	return type_id(string_id(name));
}

template <typename... BAs>
requires BAsPack<BAs...>
const std::string& ba_types<BAs...>::type_name(size_t tid) {
	if (tid >= types.size()) tid = 0;
	return string_from_id(types[tid]);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& ba_types<BAs...>::print_type(std::ostream& os, size_t tid) {
	return os << type_name(tid);
}

template <NodeType node>
size_t get_ba_type_id(size_t ba_type_sid) {
	return  node::ba_types::type_id(ba_type_sid);
}

template <NodeType node>
size_t get_ba_type_id(const std::string& ba_type_name) {
	return node::ba_types::type_id(ba_type_name);
}

template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id) {
	return node::ba_types::type_name(ba_type_id);
}

template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id) {
	return node::ba_types::print_type(os, ba_type_id);
}

// -----------------------------------------------------------------------------
// BA constants

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_constants"

template <typename... BAs>
requires BAsPack<BAs...>
std::pair<size_t, size_t> ba_constants<BAs...>::get(
	const std::variant<BAs...>& b, size_t ba_type)
{
	LOG_TRACE << "(BAC) -- ba_constants::get: " << b << " " << ba_type;
	size_t constant_id = get(b);
	LOG_TRACE << "(BAC) -- constant_id: " << constant_id
		<< " ba_type: " << ba_type;
	return *(ba_type_map.emplace(constant_id, ba_type).first);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::pair<size_t, size_t> ba_constants<BAs...>::get(
	const std::variant<BAs...>& b, const std::string& ba_type_name)
{
	// DBG(std::cout << "BAC get: " << b << " " << type_name << "\n";)
	return get(b, ba_types<BAs...>::type_id(ba_type_name));
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> ba_constants<BAs...>::get(size_t constant_id) {
	// std::cout << "BAC get: " << constant_id << " " << C.size() << "\n";
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return C[constant_id];
}

template <typename... BAs>
requires BAsPack<BAs...>
template <typename BA>
requires OneOfBAs<BA, BAs...>
BA ba_constants<BAs...>::get(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	DBG(assert((is<BA>(constant_id)));)
	return std::get<BA>(C[constant_id]);
}

template <typename... BAs>
requires BAsPack<BAs...>
template <typename BA>
requires OneOfBAs<BA, BAs...>
BA ba_constants<BAs...>::get(tref t) {
	DBG(assert(t != nullptr);)
	return get<BA>(tree<node<BAs...>>::get(t).get_ba_constant_id());
}

template <typename... BAs>
requires BAsPack<BAs...>
template <typename BA>
requires OneOfBAs<BA, BAs...>
bool ba_constants<BAs...>::is(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return std::holds_alternative<BA>(C[constant_id]);
}

template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_constants<BAs...>::type_of(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return ba_type_map.at(constant_id);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& ba_constants<BAs...>::print(std::ostream& os, size_t constant_id) {
	return ba_types<BAs...>::print_type(
		print_constant(os, constant_id) << " : ", type_of(constant_id));
}

// print the constant value to the stream
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& ba_constants<BAs...>::print_constant(std::ostream& os, size_t constant_id) {
	std::variant<BAs...> v = get(constant_id);
	return os << "{ " << v << " }";
}

// -----------------------------------------------------------------------------
// internal insertion of a constant into the pool
template <typename... BAs>
requires BAsPack<BAs...>
size_t ba_constants<BAs...>::get(const std::variant<BAs...>& b) {
	// TODO better initialization of C
	// currently reserves 0 position for null by just copying first inserted element
	if (C.empty()) C.push_back(b);
	// TODO optimize
	if (auto it = std::find(C.begin() + 1, C.end(), b); it != C.end())
		return it - C.begin();
	C.push_back(b);
	// DBG(std::cout << "BAC got: " << b << " with constant_id: " << C.size() - 1 << "\n";)
	return C.size() - 1;
}

// -----------------------------------------------------------------------------
// binder

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_constants_binder"

template <typename... BAs>
requires BAsPack<BAs...>
ba_constants_binder<BAs...>::ba_constants_binder() {
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
}

template <typename... BAs>
requires BAsPack<BAs...>
ba_constants_binder<BAs...>::ba_constants_binder(
	const std::map<std::string, std::pair<size_t, size_t>>& named_constants)
	: named_constants(named_constants)
{
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
}

// binds the constant to a tree from BA constant variant and type name string
template <typename... BAs>
requires BAsPack<BAs...>
tref ba_constants_binder<BAs...>::bind(const std::variant<BAs...>& constant,
	const std::string& type_name)
{
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
	LOG_DEBUG << "(BACB) -- ba_constants_binder::bind: "
					<< constant << " " << type_name;
	return tree<node<BAs...>>::get_ba_constant(
				ba_constants<BAs...>::get(constant, type_name));
}

// binds the constant to a tree from BA constant variant and type name string
template <typename... BAs>
requires BAsPack<BAs...>
tref ba_constants_binder<BAs...>::bind(const std::variant<BAs...>& constant,
	size_t type_id)
{
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
	LOG_TRACE << "(BACB) -- ba_constants_binder::bind: " << constant
		<< " " << type_id << " " << ba_types<BAs...>::type_name(type_id);
	return tree<node<BAs...>>::get_ba_constant(
				ba_constants<BAs...>::get(constant, type_id));
}

template <typename... BAs>
requires BAsPack<BAs...>
tref ba_constants_binder<BAs...>::operator()(const std::string& src,
	const std::string& type_name)
{
	// named binding
	if (auto it = named_constants.find(src);
		it != named_constants.end())
	{
		tref n = tree<node<BAs...>>::get_ba_constant(it->second);
		tree<node<BAs...>>::get(n).dump(std::cout << "tr: ") << "\n";
		LOG_TRACE
			<< "(BACB) -- ba_constants_binder::operator() named binding: `"
			<< src << "` constant: `" << tree<node<BAs...>>::get(n)
			<< "` cid: " << it->second.first << " tid: "
			<< it->second.second << " "
			<< " " << ba_types<BAs...>::type_name(it->second.second);
		return n;
	}

	// factory binding
	static const std::string untyped = "untyped";
	error = false;
	auto n = nso_factory<BAs...>::instance().binding(src,
				type_name.empty() ? untyped : type_name);
	if (!n) return error = true, nullptr;

	// debug info
	const auto& t = tree<node<BAs...>>::get(n);
	size_t cid = t.get_ba_constant_id();
	size_t tid = ba_constants<BAs...>::type_of(cid);
	LOG_TRACE
		<< "(BACB) -- ba_constants_binder::operator() factory binding: `"
		<< src << " constant: `" << t << "`"
		<< "` cid: " << cid << " tid: " << tid
		<< " " << ba_types<BAs...>::type_name(tid);

	return n;
}

template <typename... BAs>
requires BAsPack<BAs...>
ba_constants_binder<BAs...>& ba_constants_binder<BAs...>::instance() {
	static ba_constants_binder<BAs...> binder;
	return binder;
}

} // idni::tau_lang namespace
