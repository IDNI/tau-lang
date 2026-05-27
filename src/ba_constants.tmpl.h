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
	// Release constant values (which may include cvc5::Term, bdd_handle, etc.)
	// before those backends' static destructors run.  C.clear() is the only
	// mandatory step: the cvc5 Terms are stored in C's pair<constant, size_t>
	// elements, not in T's htree shared-ptrs.
	//
	// T is intentionally NOT cleared here.  T holds shared_ptr<htree> whose
	// control blocks can be corrupted at atexit time if the BDD pool or
	// htree pool static destructors have already partially run (order is
	// implementation-defined when static locals in different TUs are involved).
	// T's own inline-static destructor runs in a safe, well-ordered context and
	// will release the htrees correctly.
	//
	// Do NOT call shrink_to_fit() on either C or T: the atexit callback runs
	// before C and T's own static destructors.  shrink_to_fit() would free the
	// backing storage here, and the subsequent static-destructor pass would
	// attempt to free it a second time, producing a "double free" abort.
	C.clear();
	// T is left for its own static destructor (safe; htrees hold no cvc5/bdd refs).
}

} // namespace idni::tau_lang
