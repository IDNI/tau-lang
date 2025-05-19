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
tref ba_constants<node>::get(
	const std::string& constant_source, const std::string& type_name)
{
	return get(dict(constant_source), get_ba_type_id<node>(type_name));
}

template <NodeType node>
tref ba_constants<node>::get(const std::string& constant_source, size_t type_id)
{
	return get(dict(constant_source), type_id);
}

template <NodeType node>
tref ba_constants<node>::get(size_t constant_source_sid, size_t type_id)
{
	LOG_TRACE << " -- get(size_t, size_t): " << constant_source_sid
		<< " " << type_id;
	if (type_id == 0) LOG_TRACE << " -- untyped: " << dict(constant_source_sid);
	else LOG_TRACE << " -- typed: " << get_ba_type_name<node>(type_id);
	if (type_id == 0)
		return tree<node>::get(
			node::ba_constant(constant_source_sid, type_id));

	// binding
	return tree<node>::get_ba_constant(
		node::nso_factory::instance().parse(
					dict(constant_source_sid),
					get_ba_type_name<node>(type_id)));
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant,
			     const std::string& type_name)
{
	return get(constant, get_ba_type_id<node>(type_name));
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant, size_t type_id) {
	LOG_TRACE << "get: " << LOG_BA(constant) << " " << LOG_BA_TYPE(type_id);
	LOG_TRACE << " -- ba_constants:" << ba_constants<node>::dump_to_str();
	// TODO optimize
	for (size_t i = 0; i < C.size(); ++i) if (C[i] == constant) {
		LOG_TRACE << "returning existing: " << i+1 << " " << LOG_FM(T[i]);
		return T[i];
	}
	C.push_back(constant);
	size_t constant_id = C.size();
	LOG_TRACE << " -- values for node::ba_constant: " << constant_id
		<< " " << type_id;
	node n = node::ba_constant(constant_id, type_id);
	LOG_TRACE << " -- created node::ba_constant: " << n;
	tref r = tree<node>::get(n);
	const auto& t = tree<node>::get(r);
	T.push_back(r);
	LOG_TRACE << " got node: " << t.value;
	LOG_TRACE << "returning bound: " << LOG_FM(r);
	return r;
}

template <NodeType node>
typename ba_constants<node>::constant ba_constants<node>::get(
	size_t constant_id)
{
	// std::cout << "BAC get: " << constant_id << " " << C.size() << "\n";
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id <= C.size());)
	return C[constant_id - 1];
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	for (size_t i = 0; i < C.size(); ++i) {
		os << "\nconstant_id: " << i + 1 << " " << LOG_BA(C[i]) << " : "
			<< LOG_FM(T[i]);
	}
	return os << "\n";
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

// template <NodeType node>
// tref ba_constants_binder<node>::operator()(const std::string& src,
// 	const std::string& type_name)
// {
// 	LOG_TRACE << "binding operator(): `" << LOG_BA(src) << "` "
// 					<< "constant: " << LOG_BA(type_name);
// 	// untyped binding
// 	static const std::string untyped = "untyped";
// 	static const size_t untid = get_ba_type_id<node>(untyped);
// 	bool unresolved = type_name.empty() || type_name == untyped;
// 	if (unresolved) { // untyped binding
// 		tref n = tree<node>::get_ba_constant(dict(src), untid);
// 		LOG_TRACE << "operator() untyped binding: "
// 			<< "`" << src << "` " << "constant: " << LOG_FM(n);
// 		return n;
// 	}

// 	// named binding
// 	if (auto it = named_constants.find(src);
// 		it != named_constants.end())
// 	{
// 		tref n = tree<node>::get_ba_constant(it->second);
// 		tree<node>::get(n).dump(std::cout << "tr: ") << "\n";
// 		LOG_TRACE << "operator() named binding: "
// 			<< "`" << src << "` " << "constant: " << LOG_FM(n);
// 		return n;
// 	}

// 	// factory binding
// 	error = false;
// 	auto n = node::nso_factory::instance().binding(src, type_name.empty()
// 							? untyped : type_name);
// 	if (!n) {
// 		LOG_TRACE  << "operator() factory binding: "
// 			<< "`" << src << "`" << " failed";
// 		error = true;
// 		return nullptr;
// 	}
	
// 	LOG_TRACE  << "operator() factory binding: "
// 		<< "`" << src << "`" << " constant: " << LOG_FM_DUMP(n);
// 	return n;
// }



} // namespace idni::tau_lang
