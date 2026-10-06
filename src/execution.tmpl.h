// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "execution.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "execution"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// step

template <NodeType node>
step<node>::step(rewriter::library lib) : lib(lib) {}

template <NodeType node>
tref step<node>::operator()(tref n) const {
	return nso_rr_apply<node>(lib, n);
}

// -----------------------------------------------------------------------------
// steps

template <NodeType node, typename step_t>
steps<node, step_t>::steps(std::vector<step_t> libraries)
						: libraries(std::move(libraries)) {}

template <NodeType node, typename step_t>
steps<node, step_t>::steps(step_t library) { libraries.push_back(library); }

template <NodeType node, typename step_t>
tref steps<node, step_t>::operator()(tref n) const {
	if (libraries.empty()) return n;
	auto nn = n;
	for (auto& lib : libraries) nn = lib(nn);
	return nn;
}

// -----------------------------------------------------------------------------
// repeat_each

template <NodeType node, typename step_t>
repeat_each<node, step_t>::repeat_each(steps<node, step_t> s) : s(s) {}

template <NodeType node, typename step_t>
repeat_each<node, step_t>::repeat_each(step_t s)
					: s(steps<node, step_t>(s)) {}

template <NodeType node, typename step_t>
tref repeat_each<node, step_t>::operator()(tref n) const {
	auto nn = n;
	for (auto& l: s.libraries) {
		// `visited` catches a fixpoint or a cycle; max_rewrite_rounds
		// (0 = unlimited) bounds an ever-growing rewrite.
		std::unordered_set<tref> visited;
		bool settled = false;
		for (size_t round = 0;
			!max_rewrite_rounds || round < max_rewrite_rounds; ++round)
		{
			nn = l(nn);
			if (visited.contains(nn)) { settled = true; break; }
			visited.insert(nn);
		}
		if (!settled) {
			LOG_ERROR << "Rewriting did not reach a fixpoint after "
				<< max_rewrite_rounds << " rounds (max-rewrite-rounds)"
				" and is still growing; the definitions in use are "
				"most likely non-terminating for this argument";
			return nullptr;
		}
	}
	return nn;
}

// -----------------------------------------------------------------------------
// repeat_all

template <NodeType node, typename step_t>
repeat_all<node, step_t>::repeat_all(steps<node, step_t> s) : s(s) {}

template <NodeType node, typename step_t>
repeat_all<node, step_t>::repeat_all(step_t s)
					: s(steps<node, step_t>(s)) {}

template <NodeType node, typename step_t>
tref repeat_all<node, step_t>::operator()(tref n) const {
	auto nn = n;
	std::unordered_set<tref> visited;
	// Apply the whole sequence once per round; stop on a fixpoint or,
	// using visited, a longer oscillating cycle. max_rewrite_rounds
	// (0 = unlimited) bounds an ever-growing rewrite (one that never
	// repeats a prior state), which visited alone cannot detect.
	for (size_t round = 0;
		!max_rewrite_rounds || round < max_rewrite_rounds; ++round) {
		nn = s(nn);
		if (visited.contains(nn)) return nn;
		visited.insert(nn);
	}
	// Returning the partially rewritten formula would hand the caller a
	// half-expanded term indistinguishable from a real result; a rewrite
	// that never settles has no result, so report the failure instead.
	LOG_ERROR << "Rewriting did not reach a fixpoint after "
		<< max_rewrite_rounds << " rounds (max-rewrite-rounds) and is "
		"still growing; the definitions in use are most likely "
		"non-terminating for this argument";
	return nullptr;
}

// -----------------------------------------------------------------------------
// repeat_once

template <NodeType node, typename step_t>
repeat_once<node, step_t>::repeat_once(steps<node, step_t> s) : s(s) {}

template <NodeType node, typename step_t>
repeat_once<node, step_t>::repeat_once(step_t s)
					: s(steps<node, step_t>(s)) {}

template <NodeType node, typename step_t>
tref repeat_once<node, step_t>::operator()(tref n) const {
	auto nn = n;
	for(auto& l: s.libraries) {
		nn = l(nn);
	}
	return nn;
}

// -----------------------------------------------------------------------------
// operator|

template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const step_t& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const rewriter::library& l)
{
	using tt = typename tree<node>::traverser;
	return n | tt::f(step<node>(l));
}

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_once<node, step_t>& r)
{
	using tt = typename tree<node>::traverser;
	return n | tt::f(r);
}

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_all<node, step_t>& r)
{
	using tt = typename tree<node>::traverser;
	return n | tt::f(r);
}

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_each<node, step_t>& r)
{
	using tt = typename tree<node>::traverser;
	return n | tt::f(r);
}

} // namespace idni::tau_lang
