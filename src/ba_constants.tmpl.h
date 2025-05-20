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
	LOG_TRACE << " -- get(size_t, size_t): "
				<< constant_source_sid << ", " << type_id;
	if (type_id == 0)
		LOG_TRACE << " -- untyped: " << dict(constant_source_sid);
	else LOG_TRACE << " -- typed: " << get_ba_type_name<node>(type_id);

	return type_id == 0
		? tree<node>::get( // untyped contains source sid
			node::ba_constant(constant_source_sid, type_id))
		: tree<node>::get_ba_constant(
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
	LOG_TRACE << "-- get(constant, type_id): "
		<< LOG_BA(constant) << ", " << LOG_BA_TYPE(type_id);
	LOG_TRACE << "ba_constants:" << ba_constants<node>::dump_to_str();
	// TODO optimize
	for (size_t i = 0; i < C.size(); ++i) if (C[i] == constant) {
		LOG_TRACE << "-- returning already pooled: "
						<< i+1 << " " << LOG_FM(T[i]);
		return T[i];
	}
	C.push_back(constant);
	size_t constant_id = C.size();
	node n = node::ba_constant(constant_id, type_id);
	LOG_TRACE << "node::ba_constant: " << n;
	tref r = tree<node>::get(n);
	const auto& t = tree<node>::get(r);
	T.push_back(r);
	LOG_TRACE << "node from tree:    " << t.value;
	LOG_TRACE << "-- returning pooled constant: " << LOG_FM(r);
	return r;
}

template <NodeType node>
typename ba_constants<node>::constant ba_constants<node>::get(
	size_t constant_id)
{
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id <= C.size());)
	return C[constant_id - 1];
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	for (size_t i = 0; i < C.size(); ++i)
		os << "\n" << LOG_INDENT << "constant: " << i + 1
			<< " " << LOG_BA(C[i]) << " : " << LOG_FM(T[i]);
	return os << "\n";
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

} // namespace idni::tau_lang
