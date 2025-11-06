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
tref ba_constants<node>::get(const constant& constant, size_t type_id) {
	LOG_TRACE << "-- get(constant, type_id): "
		<< LOG_BA(constant) << ", " << LOG_BA_TYPE(type_id);
	// LOG_TRACE << dump_to_str();
	// TODO optimize
	for (size_t i = 0; i < C.size(); ++i) if (C[i] == std::make_pair(constant, type_id)) {
		LOG_TRACE << "-- returning already pooled: "
						<< i+1 << " " << LOG_FM(T[i]);
		return T[i];
	}
	C.push_back(std::make_pair(constant, type_id));
	size_t constant_id = C.size();
	node n = node::ba_constant(constant_id, type_id);
	tref r = tree<node>::get(n);
	T.push_back(r);
	// LOG_TRACE << "node constant: " << n;
	// LOG_TRACE << dump_to_str();
	// const auto& t = tree<node>::get(r);
	// LOG_TRACE << "node from tree:    " << t.value;
	LOG_TRACE << "-- returning pooled constant: " << LOG_FM(r);
	return r;
}

template <NodeType node>
tref ba_constants<node>::get(const constant& constant, tref type_tree) {
	return get(constant, ba_types<node>::id(type_tree));
}

template <NodeType node>
typename ba_constants<node>::constant ba_constants<node>::get(
	size_t constant_id)
{
	DBG(assert(constant_id > 0);)
	DBG(assert(constant_id <= C.size());)
	return C[constant_id - 1].first;
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	os << "BA constants pool(" << C.size() << "):\n";
	for (size_t i = 0; i < C.size(); ++i) os << LOG_INDENT << "constant: "
		<< i+1 << " " << LOG_BA(C[i]) << " : " << LOG_FM(T[i]) << "\n";
	return os;
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

template <NodeType node>
void ba_constants<node>::cleanup() {
	C.clear(); C.shrink_to_fit();
	T.clear(); T.shrink_to_fit();
}

} // namespace idni::tau_lang
