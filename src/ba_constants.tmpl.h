// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ba_types.h"
#include "ba_constants.h"

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
	const auto p = std::make_pair(constant, type_id);
	for (size_t i = 0; i < C.size(); ++i) if (C[i] == p) {
		LOG_TRACE << "-- returning already pooled: "
						<< i+1 << " " << LOG_FM(T[i]->get());
		return T[i]->get();
	}
	C.emplace_back(std::move(p));
	size_t constant_id = C.size();
	node n = node::ba_constant(constant_id, type_id);
	tref r = tree<node>::get(n);
	T.push_back(tree<node>::geth(r));
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
	// constant_id == 0 underflows to SIZE_MAX below; the DBG-only asserts
	// caught both that and an out-of-range id in debug, but release had
	// no check at all -- an unchecked, likely huge, out-of-bounds access.
	if (constant_id == 0 || constant_id > C.size())
		throw std::logic_error("ba_constants::get: invalid constant_id "
			+ std::to_string(constant_id));
	return C[constant_id - 1].first;
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	os << "BA constants pool(" << C.size() << "):\n";
	for (size_t i = 0; i < C.size(); ++i) os << LOG_INDENT << "constant: "
		<< i+1 << " " << LOG_BA(C[i]) << " : " << LOG_FM(T[i]->get()) << "\n";
	return os;
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

template <NodeType node>
void ba_constants<node>::cleanup() {
	// Release all elements so cvc5::Term references are dropped before
	// cvc5's TermManager is destroyed.  Do NOT call shrink_to_fit(): the
	// atexit callback that invokes cleanup() runs before C and T's own
	// static destructors.  If shrink_to_fit() frees the backing storage
	// here, the subsequent static-destructor pass tries to free the same
	// block a second time, producing a "double free detected" abort.
	// The backing storage is released exactly once by C/T's own destructors.
	C.clear();
	T.clear();
}

} // namespace idni::tau_lang
