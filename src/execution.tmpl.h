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
						: libraries(libraries) {}

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
		std::set<tref> visited;
		while (true) {
			nn = l(nn);
			if (visited.find(nn) != visited.end()) break;
			visited.insert(nn);
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
	std::set<tref> visited;
	while (true) {
		for (auto& l: s.libraries) nn = l(nn);
		auto nnn = s(nn);
		if (nnn == nn) break;
		nn = nnn;
	}
	return nn;
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
// to_steps

template <NodeType node>
steps<node, step<node>> to_steps(
	const std::initializer_list<rewriter::library>& libs)
{
	std::vector<step<node>> s;
	for (auto& l : libs) {
		// LOG_TRACE << "to_steps library " << l.size() << " rules";
		// for (auto& r : l)
		// 	LOG_TRACE << "to_steps rule: " << to_str<node>(r);
		s.emplace_back(l);
	}
	return steps<node, step<node>>(std::move(s));
}

// -----------------------------------------------------------------------------
// operator|

template <NodeType node, typename step_t>
steps<repeat_each<node, step_t>, node> operator|(
	const repeat_each<node, step_t>& l,
	const repeat_each<node, step_t>& r)
{
	auto s = steps<node, repeat_each<node, step_t>>(l);
	s.libraries.push_back(r);
	return s;
}

template <NodeType node, typename step_t>
steps<repeat_all<node, step_t>, node> operator|(
	const repeat_all<node, step_t>& l,
	const repeat_all<node, step_t>& r)
{
	auto s = steps<repeat_all<node, step_t>, node>(l);
	s.libraries.push_back(r);
	return s;
}

template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const step_t& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l)
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
	const steps<step_t, node>& s)
{
	using tt = typename tree<node>::traverser;
	return n | tt::f(s);
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
