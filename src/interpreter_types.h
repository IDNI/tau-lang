// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__INTERPRETER_TYPES_H__
#define __IDNI__TAU__INTERPRETER_TYPES_H__

#include <map>
#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node> // variable string id -> { ba type id, stream/filename string id }
using io_defs = std::map<size_t, std::pair<size_t, size_t>>;

// context for a spec contains input and output stream definitions
template <NodeType node>
struct spec_context {
	io_defs<node> inputs;
	io_defs<node> outputs;
};

template <NodeType node>
std::ostream& operator<<(std::ostream& os, const spec_context<node>& ctx);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__INTERPRETER_TYPES_H__