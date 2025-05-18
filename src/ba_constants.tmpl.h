// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"
#include "ba_constants.h"
#include "nso_factory.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_constants"

template <NodeType node>
std::pair<size_t, size_t> ba_constants<node>::get(
	const bas_variant& b, size_t ba_type)
{
	LOG_TRACE << "get: " << LOG_BA(b) << " " << LOG_BA_TYPE(ba_type);
	size_t constant_id = get(b);
	LOG_TRACE << "constant_id: " << constant_id
		  << " ba_type: "    << LOG_BA_TYPE(ba_type);
	return *(ba_type_map.emplace(constant_id, ba_type).first);
}

template <NodeType node>
std::pair<size_t, size_t> ba_constants<node>::get(
	const bas_variant& b, const std::string& ba_type_name)
{
	return get(b, get_ba_type_id<node>(ba_type_name));
}

template <NodeType node>
typename ba_constants<node>::bas_variant ba_constants<node>::get(
	size_t constant_id)
{
	// std::cout << "BAC get: " << constant_id << " " << C.size() << "\n";
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return C[constant_id];
}

template <NodeType node>
template <typename BA>
BA ba_constants<node>::get(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	DBG(assert((is<BA>(constant_id)));)
	return std::get<BA>(C[constant_id]);
}

template <NodeType node>
template <typename BA>
BA ba_constants<node>::get(tref t) {
	DBG(assert(t != nullptr);)
	return get<BA>(tree<node>::get(t).get_ba_constant_id());
}

template <NodeType node>
template <typename BA>
bool ba_constants<node>::is(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return std::holds_alternative<BA>(C[constant_id]);
}

template <NodeType node>
size_t ba_constants<node>::type_of(size_t constant_id) {
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id < C.size());)
	return ba_type_map.at(constant_id);
}

template <NodeType node>
std::ostream& ba_constants<node>::print(std::ostream& os, size_t constant_id) {
	return ba_types<node>::print_type(
		print_constant(os, constant_id) << " : ", type_of(constant_id));
}

// print the constant value to the stream
template <NodeType node>
std::ostream& ba_constants<node>::print_constant(std::ostream& os,
	size_t constant_id)
{
	bas_variant v = get(constant_id);
	return os << "{ " << v << " }";
}

// -----------------------------------------------------------------------------
// internal insertion of a constant into the pool
template <NodeType node>
size_t ba_constants<node>::get(const bas_variant& b) {
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

template <NodeType node>
ba_constants_binder<node>::ba_constants_binder() {}

template <NodeType node>
ba_constants_binder<node>::ba_constants_binder(
	const std::map<std::string, std::pair<size_t, size_t>>& named_constants)
	: named_constants(named_constants) {}

// binds the constant to a tree from BA constant variant and type name string
template <NodeType node>
tref ba_constants_binder<node>::bind(const bas_variant& constant,
	const std::string& type_name)
{
	LOG_TRACE << "bind: " << LOG_BA(constant) << " " << LOG_BA(type_name);
	return tree<node>::get_ba_constant(
				ba_constants<node>::get(constant, type_name));
}

// binds the constant to a tree from BA constant variant and type name string
template <NodeType node>
tref ba_constants_binder<node>::bind(const bas_variant& constant,
	size_t type_id)
{
	LOG_TRACE << "bind: " << LOG_BA(constant) << " " <<LOG_BA_TYPE(type_id);
	return tree<node>::get_ba_constant(
				ba_constants<node>::get(constant, type_id));
}

template <NodeType node>
tref ba_constants_binder<node>::operator()(const std::string& src,
	const std::string& type_name)
{
	// named binding
	if (auto it = named_constants.find(src);
		it != named_constants.end())
	{
		tref n = tree<node>::get_ba_constant(it->second);
		tree<node>::get(n).dump(std::cout << "tr: ") << "\n";
		LOG_TRACE << "operator() named binding: `" << src << "` "
						<< "constant: " << LOG_FM(n);
		return n;
	}

	// factory binding
	static const std::string untyped = "untyped";
	error = false;
	auto n = node::nso_factory::instance().binding(src, type_name.empty()
							? untyped : type_name);
	if (!n) return error = true, nullptr;

	LOG_TRACE  << "operator() factory binding: `" << src << "`"
						<< " constant: " << LOG_FM(n);
	return n;
}

template <NodeType node>
ba_constants_binder<node>& ba_constants_binder<node>::instance() {
	static ba_constants_binder<node> binder;
	return binder;
}

} // namespace idni::tau_lang
