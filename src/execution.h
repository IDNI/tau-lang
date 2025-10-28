// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// TODO (MEDIUM) clean execution api code

#ifndef __IDNI__TAU__EXECUTION_H__
#define __IDNI__TAU__EXECUTION_H__

#include "nso_rr.h"

namespace idni::tau_lang {

// TODO (MEDIUM) clean execution api code
template <NodeType node>
struct step {

	step(rewriter::library lib);

	tref operator()(tref n) const;

	rewriter::library lib;
};

template <NodeType node, typename step_t>
struct steps {

	steps(std::vector<step_t> libraries);
	steps(step_t library);

	tref operator()(tref n) const;

	std::vector<step_t> libraries;
};

template <NodeType node, typename step_t>
struct repeat_each {

	repeat_each(steps<node, step_t> s);
	repeat_each(step_t s);

	tref operator()(tref n) const;

	steps<node, step_t> s;
};

template <NodeType node, typename step_t>
struct repeat_all {

	repeat_all(steps<node, step_t> s);
	repeat_all(step_t s);

	tref operator()(tref n) const;

	steps<node, step_t> s;
};

template <NodeType node, typename step_t>
struct repeat_once {

	repeat_once(steps<node, step_t> s);
	repeat_once(step_t s);

	tref operator()(tref n) const;

	steps<node, step_t> s;
};

template <NodeType node>
steps<node, step<node>> to_steps(
	const std::initializer_list<rewriter::library>& libs);

template <NodeType node, typename step_t>
steps<repeat_each<node, step_t>, node> operator|(
	const repeat_each<node, step_t>& l, const repeat_each<node, step_t>& r);

template <NodeType node, typename step_t>
steps<repeat_all<node, step_t>, node> operator|(
	const repeat_all<node, step_t>& l, const repeat_all<node, step_t>& r);

template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const step_t& l);

template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l);

template <NodeType node>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l);

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const rewriter::library& l);

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const steps<step_t, node>& s);

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_once<node, step_t>& r);

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_all<node, step_t>& r);

template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_each<node, step_t>& r);

} // namespace idni::tau_lang

#include "execution.tmpl.h"

#endif // __IDNI__TAU__EXECUTION_H__