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
	// BA2-5: after cleanup() the C/T pools are out of sync by design
	// (atexit ordering); interning then would alias fresh constants
	// with stale trees. Fail loudly instead.
	if (poisoned) throw std::logic_error(
		"ba_constants::get called after cleanup()");
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
typename ba_constants<node>::constant ba_constants<node>::get(
	size_t constant_id)
{
	// constant_id == 0 underflows to SIZE_MAX below; the DBG-only asserts
	// caught both that and an out-of-range id in debug, but release had
	// no check at all -- an unchecked, likely huge, out-of-bounds access.
	if (constant_id == 0 || constant_id > C().size())
		throw std::logic_error("ba_constants::get: invalid constant_id "
			+ std::to_string(constant_id));
	return C()[constant_id - 1].first;
}

template <NodeType node>
std::ostream& ba_constants<node>::dump(std::ostream& os) {
	os << "BA constants pool(" << C().size() << "):\n";
	for (size_t i = 0; i < C().size(); ++i) os << LOG_INDENT << "constant: "
		<< i+1 << " " << LOG_BA(C()[i]) << " : " << LOG_FM(T()[i]->get()) << "\n";
	return os;
}

template <NodeType node>
std::string ba_constants<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

template <NodeType node>
void ba_constants<node>::cleanup() {
	// Both pools live behind leaked function-local statics (see C()/T()
	// above) so neither ever runs a static destructor at exit -- no
	// atexit ordering hazard with the BDD/cvc5 backends remains, so all
	// three pools are simply cleared together here to keep their indices
	// aligned.
	C().clear();
	T().clear();
	index_().clear();
	poisoned = true; // BA2-5: any later get() must fail loudly, see header
}

} // namespace idni::tau_lang
