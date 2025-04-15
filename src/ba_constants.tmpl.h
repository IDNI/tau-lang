// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_constants.h"

namespace idni::tau_lang {

template <typename... BAs>
size_t ba_constants<BAs...>::get(const std::variant<BAs...>& b) {
	// TODO optimize
	if (auto it = std::find(C.begin(), C.end(), b); it != C.end())
		return it - C.begin();
	C.push_back(b);
	return C.size() - 1;
}

template <typename... BAs>
tref ba_constants<BAs...>::get(const std::variant<BAs...>& b,
	const std::string& type_name)
{
	return tree<node<BAs...>>::get_ba_constant(get(b),
						type_id(string_id(type_name)));
}

template <typename... BAs>
std::variant<BAs...> ba_constants<BAs...>::get(size_t constant_id) {
	return C[constant_id];
}

template <typename... BAs>
template <typename BA>
BA ba_constants<BAs...>::get(size_t constant_id) {
	DBG(assert((is<BA>(constant_id)));)
	return std::get<BA>(C[constant_id]);
}

template <typename... BAs>
template <typename BA>
BA ba_constants<BAs...>::get(tref t) {
	return get<BA>(tau::get(t).get_ba_constant_id());
}

template <typename... BAs>
template <typename BA>
bool ba_constants<BAs...>::is(tref t) {
	return is<BA>(tau::get(t).get_ba_constant_id());
}

template <typename... BAs>
template <typename BA>
bool ba_constants<BAs...>::is(size_t constant_id) {
	return std::holds_alternative<BA>(C[constant_id]);
}

template <typename... BAs>
size_t ba_constants<BAs...>::type_id(size_t type_sid) {
	if (auto it = types_map.find(type_sid); it != types_map.end())
		return it->second;
	return types_map.emplace(type_sid, types.size()),
		types.push_back(type_sid), types.size() - 1;
}

template <typename... BAs>
size_t ba_constants<BAs...>::type_id(const std::string& type_name) {
	if (types_map.size() == 0) {
		types.push_back(string_id("untyped"));
		types_map.emplace(types.back(), 0);
	}
	return type_id(string_id(type_name));
}

template <typename... BAs>
const ba_constants<BAs...>::std::string& type_name(size_t tid) {
	if (tid >= ba_constants<BAs...>::types.size()) tid = 0;
	return string_from_id(ba_constants<BAs...>::types[tid]);
}

template <typename... BAs>
ba_constants<BAs...>& ba_constants<BAs...>::instance() {
	static ba_constants<BAs...> binder;
	return binder;
}

template <typename... BAs>
ba_constants<BAs...>::ba_constants() {
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
}

template <typename... BAs>
ba_constants<BAs...>::ba_constants(
	const named_constants_map& named_constants)
	: named_constants(named_constants)
{
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
}

template <typename... BAs>
tref ba_constants<BAs...>::bind(const std::string& src,
	const std::string& type_name)
{
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
	error = false;
	if (auto it = named_constants.find(src);
		it != named_constants.end()) return it->second;
	static const std::string untyped = "untyped";
	auto nn = nso_factory<BAs...>::instance().binding(src,
				type_name.empty() ? untyped : type_name);
	if (!nn) return error = true, nullptr;
	return nn;
}

template <typename... BAs>
tref ba_constants<BAs...>::operator()(const std::string& src,
	const std::string& type_name)
{
	return bind(src, type_name);
}

} // idni::tau_lang namespace
