// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__EXECUTION_H__
#define __IDNI__TAU__EXECUTION_H__

#include "normal_forms.h"
#include "nso_rr.h"

namespace idni::tau_lang {

// TODO (MEDIUM) clean execution api code
template <NodeType node>
struct step {

	step(rewriter::library lib): lib(lib) {}

	tref operator()(tref n) const { return nso_rr_apply<node>(lib, n); }

	rewriter::library lib;
};

template<NodeType node, typename step_t>
struct steps {

	steps(std::vector<step_t> libraries) : libraries(libraries) {}
	steps(step_t library) { libraries.push_back(library); }

	tref operator()(tref n) const {
		if (libraries.empty()) return n;
		auto nn = n;
		for (auto& lib : libraries) nn = lib(nn);
		return nn;
	}

	std::vector<step_t> libraries;
};

template <NodeType node, typename step_t>
struct repeat_each {

	repeat_each(steps<node, step_t> s) : s(s) {}
	repeat_each(step_t s) : s(steps<node, step_t>(s)) {}

	tref operator()(tref n) const {
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

	steps<node, step_t> s;
};

template <NodeType node, typename step_t>
struct repeat_all {

	repeat_all(steps<node, step_t> s) : s(s) {}
	repeat_all(step_t s) : s(steps<node, step_t>(s)) {}

	tref operator()(tref n) const {
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

	steps<node, step_t> s;
};

template <NodeType node, typename step_t>
struct repeat_once {

	repeat_once(steps<node, step_t> s) : s(s) {}
	repeat_once(step_t s) : s(steps<node, step_t>(s)) {}

	tref operator()(tref n) const {
		auto nn = n;
		for(auto& l: s.libraries) {
			nn = l(nn);
		}
		return nn;
	}

	steps<node, step_t> s;
};

template <NodeType node>
steps<step<node>, node> operator|(const rewriter::library& l,
	const rewriter::library& r)
{
	auto s = steps<step<node>, node>(step<node>(l));
	s.libraries.push_back(r);
	return s;
}

template <NodeType node, typename step_t>
steps<repeat_each<node, step_t>, node> operator|(
	const repeat_each<node, step_t>& l, const repeat_each<node, step_t>& r)
{
	auto s = steps<repeat_each<node, step_t>, node>(l);
	s.libraries.push_back(r);
	return s;
}

template <NodeType node, typename step_t>
steps<repeat_all<node, step_t>, node> operator|(
	const repeat_all<node, step_t>& l, const repeat_all<node, step_t>& r)
{
	auto s = steps<repeat_all<node, step_t>, node>(l);
	s.libraries.push_back(r);
	return s;
}

template <NodeType node, typename step_t>
steps<step<node>, node> operator|(const steps<step<node>, node>& s,
	const step_t& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node, typename step_t>
steps<step<node>, node> operator|(const steps<step<node>, node>& s,
	const rewriter::library& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node>
steps<step<node>, node> operator|(const steps<step<node>, node>& s,
	const rewriter::library& l)
{
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template <NodeType node>
tref operator|(const tref& n, const rewriter::library& l) {
	auto s = step<node>(l);
	return s(n);
}

template <NodeType node, typename step_t>
tref operator|(const tref& n, const steps<step_t, node>& s) {
	return s(n);
}

template <NodeType node, typename step_t>
tref operator|(const tref& n, const repeat_once<node, step_t>& r) {
	return r(n);
}

template <NodeType node, typename step_t>
tref operator|(const tref& n, const repeat_all<node, step_t>& r) {
	return r(n);
}

template <NodeType node, typename step_t>
tref operator|(const tref& n, const repeat_each<node, step_t>& r) {
	return r(n);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__EXECUTION_H__