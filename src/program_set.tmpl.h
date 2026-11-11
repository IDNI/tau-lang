// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__PROGRAM_SET_TMPL_H__
#define __IDNI__TAU__PROGRAM_SET_TMPL_H__

#include "program_set.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "program_set"

namespace idni::tau_lang {

template <NodeType node>
program_set_expr<node> program_set_expr<node>::literal(tref spec) {
	program_set_expr e;
	e.op = kind::literal;
	e.spec = spec;
	return e;
}

template <NodeType node>
program_set_expr<node> program_set_expr<node>::complement(program_set_expr e) {
	program_set_expr c;
	c.op = kind::complement;
	c.args.push_back(std::move(e));
	return c;
}

template <NodeType node>
program_set_expr<node> program_set_expr<node>::intersection(
	program_set_expr a, program_set_expr b)
{
	program_set_expr c;
	c.op = kind::intersection;
	c.args.push_back(std::move(a));
	c.args.push_back(std::move(b));
	return c;
}

template <NodeType node>
program_set_expr<node> program_set_expr<node>::set_union(
	program_set_expr a, program_set_expr b)
{
	program_set_expr c;
	c.op = kind::set_union;
	c.args.push_back(std::move(a));
	c.args.push_back(std::move(b));
	return c;
}

namespace program_set_detail {

// A BA element that stands for a specification: the tau algebra.
template <typename T>
concept holds_spec = requires(const T& c) { c.nso_rr.main; c.nso_rr.rec_relations; };

// The main formula of the tau constant @p c, or an error when the constant
// carries recurrence relations, which a literal cannot apply.
template <NodeType node>
result<tref> spec_of_constant(const typename node::constant& c) {
	result<tref> r;
	tref main = nullptr;
	bool has_rr = false, is_spec = false;
	std::visit([&](const auto& v) {
		using T = std::decay_t<decltype(v)>;
		if constexpr (holds_spec<T>) {
			is_spec = true;
			has_rr = !v.nso_rr.rec_relations.empty();
			main = v.nso_rr.main ? v.nso_rr.main->get() : nullptr;
		}
	}, c);
	if (!is_spec) return r.with_error(code::invalid_argument,
		"a program-set literal is a constant of type tau");
	if (has_rr) return r.with_error(code::unsupported_operation,
		"a program-set literal carries recurrence relations, which it "
		"cannot apply");
	if (!main) return r.with_value(tree<node>::_T());
	return r.with_value(main);
}

} // namespace program_set_detail

/** @internal @copydoc program_set_of_term @endinternal */
template <NodeType node>
result<program_set_expr<node>> program_set_of_term(tref term) {
	using tau = tree<node>;
	using expr = program_set_expr<node>;
	result<expr> r;
	if (!term) return r.with_error(code::invalid_argument,
		messages::invalid_arguments);
	const auto& t = tau::get(term);
	if (!t.is(tau::bf) || !t.has_child())
		return r.with_error(code::invalid_argument,
			"a program-set expression is a term of type tau",
			{{label::value, truncate_for_message(t.to_str())}});
	const auto& c = t[0];
	auto operand = [&](size_t i) { return program_set_of_term<node>(c.child(i)); };
	// A connective is read over all its children, since a built tree may
	// hold more than two.
	auto fold = [&](auto combine) -> result<expr> {
		result<expr> r;
		TAU_TRY(expr acc, operand(0));
		for (size_t i = 1; i < c.children_size(); ++i) {
			TAU_TRY(expr next, operand(i));
			acc = combine(std::move(acc), std::move(next));
		}
		return r.with_value(std::move(acc));
	};
	if (c.is(tau::bf_and)) return fold(expr::intersection);
	if (c.is(tau::bf_or)) return fold(expr::set_union);
	if (c.is(tau::bf_neg)) {
		TAU_TRY(expr a, operand(0));
		return r.with_value(expr::complement(std::move(a)));
	}
	if (c.is(tau::bf_xor)) {
		// a + b = (a & b') | (a' & b)
		TAU_TRY(expr a, operand(0));
		TAU_TRY(expr b, operand(1));
		return r.with_value(expr::set_union(
			expr::intersection(a, expr::complement(b)),
			expr::intersection(expr::complement(a), b)));
	}
	if (c.is_ba_constant()) {
		if (!is_tau_type<node>(c.get_ba_type()))
			return r.with_error(code::invalid_argument,
				"a program-set literal is a constant of type tau",
				{{label::value, truncate_for_message(t.to_str())}});
		TAU_TRY(tref spec, program_set_detail::spec_of_constant<node>(
			c.get_ba_constant()));
		return r.with_value(expr::literal(spec));
	}
	if (c.is(tau::bf_t)) return r.with_value(expr::literal(tau::_T()));
	if (c.is(tau::bf_f)) return r.with_value(expr::literal(tau::_F()));
	return r.with_error(code::invalid_argument,
		"a program-set expression is a Boolean combination of tau "
		"constants",
		{{label::value, truncate_for_message(t.to_str())}});
}

/** @internal @copydoc nnf_program_set @endinternal */
template <NodeType node>
program_set_expr<node> nnf_program_set(const program_set_expr<node>& e) {
	using expr = program_set_expr<node>;
	using kind = typename expr::kind;
	std::function<expr(const expr&, bool)> push = [&](const expr& x,
		bool negate) -> expr
	{
		switch (x.op) {
		case kind::literal:
			return negate ? expr::complement(expr::literal(x.spec))
				: expr::literal(x.spec);
		case kind::complement:
			return push(x.args[0], !negate);
		case kind::intersection:
			return negate
				? expr::set_union(push(x.args[0], true),
					push(x.args[1], true))
				: expr::intersection(push(x.args[0], false),
					push(x.args[1], false));
		case kind::set_union:
			return negate
				? expr::intersection(push(x.args[0], true),
					push(x.args[1], true))
				: expr::set_union(push(x.args[0], false),
					push(x.args[1], false));
		}
		return x;
	};
	return push(e, false);
}

namespace program_set_detail {

// The literal @p spec prepared as a specification is for a decision, or
// the error of a refused one.
template <NodeType node>
result<tref> prepare_literal(tref spec) {
	using tau = tree<node>;
	result<tref> r;
	if (!spec) return r.with_error(code::invalid_argument,
		messages::invalid_arguments);
	if (!tau::get(spec).is(tau::wff))
		return r.with_error(code::invalid_argument,
			"a program-set literal is a specification",
			{{label::value, truncate_for_message(tau::get(spec).to_str())}});
	auto refuse = [&](const char* what) {
		return r.with_error(code::unsupported_operation,
			std::string("a program-set literal is a plain specification; "
				"this one holds ") + what,
			{{label::value, truncate_for_message(tau::get(spec).to_str())}});
	};
	if (has_ctl_star_operators<node>(spec))
		return refuse("a CTL* operator (A, E or -)");
	if (tau::get(spec).find_top(is<node, tau::ref>))
		return refuse("a reference to a definition");
	if (tau::get(spec).find_top([](tref n) {
		const auto& t = tau::get(n);
		return t.is_ba_constant() && is_tau_type<node>(t.get_ba_type()); }))
		return refuse("a constant of type tau");
	if (has_program_set_stream<node>(spec))
		return refuse("a hidden stream of a program-set expression");
	tref prepared = unnest_nested_always<node>(spec);
	prepared = scope_bare_clauses<node>(prepared);
	TAU_TRY(prepared, shift_forward_references<node>(prepared));
	return r.with_value(prepared);
}

// The input streams of @p fm, name and type id, appended to @p inputs in
// the order they are first met.
template <NodeType node>
void collect_inputs(tref fm,
	std::vector<std::pair<std::string, size_t>>& inputs)
{
	using tau = tree<node>;
	for (tref v : tau::get(fm).select_all(is_child<node, tau::io_var>)) {
		tref iov = tau::trim(v);
		if (io_var_direction<node>(iov) != 1) continue;
		std::string name = get_var_name<node>(iov);
		if (std::ranges::none_of(inputs, [&](const auto& p) {
			return p.first == name; }))
			inputs.emplace_back(name, tau::get(v).get_ba_type());
	}
}

// The top-level conjuncts of @p fm.
template <NodeType node>
void split_clauses(tref fm, trefs& out) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (t.child_is(tau::wff_and)) {
		for (tref c : t[0].get_children()) split_clauses<node>(c, out);
	} else out.push_back(fm);
}

template <NodeType node>
struct compiler {
	using tau = tree<node>;
	using expr = program_set_expr<node>;
	using kind = typename expr::kind;

	program_set_compiled<node>& out;
	size_t next = 0;

	std::string fresh() { return "w_ps" + std::to_string(next++); }

	// The plain Boolean combination of the literals: the input-free case.
	tref plain(const expr& e) {
		switch (e.op) {
		case kind::literal: return e.spec;
		case kind::complement:
			return tau::build_wff_neg(plain(e.args[0]));
		case kind::intersection:
			return tau::build_wff_and(plain(e.args[0]), plain(e.args[1]));
		case kind::set_union:
			return tau::build_wff_or(plain(e.args[0]), plain(e.args[1]));
		}
		return nullptr;
	}

	// The clause of the complemented literal @p spec, of warm-up l: the
	// guard `always (x[t] = z[t-1] && … && x[t-l] = z[t-l-1])` starts at
	// step l with the literal and reaches back to the first input, so
	// every step from 0 on follows the prediction; a relative step is read
	// under a temporal operator only.
	tref negative(tref spec) {
		const std::string n = fresh();
		const int_t l = body_max_lookback<node>(spec);
		program_set_negative<node> neg{ spec, l, {} };
		tref follow = nullptr;
		auto conj = [](tref acc, tref x) {
			return acc ? tau::build_wff_and(acc, x) : x; };
		for (const auto& [x, type] : out.inputs) {
			const std::string z = n + "_d_" + x;
			neg.predictions.push_back(z);
			out.hidden.emplace_back(z, type);
			tref xname = tau::build_var_name(x);
			for (int_t j = 0; j <= l; ++j)
				follow = conj(follow, tau::build_bf_eq(j == 0
					? build_in_var_at_t<node>(xname, type)
					: build_in_var_at_t_minus<node>(xname, (size_t)j, type),
					build_out_var_at_t_minus<node>(z, (size_t)j + 1, type)));
		}
		out.negatives.push_back(std::move(neg));
		return tau::build_wff_imply(tau::build_wff_always(follow),
			tau::build_wff_neg(spec));
	}

	// The clauses of a union: a selector constant from before step 0
	// chooses the operand whose clauses hold.
	tref set_union(const expr& e) {
		const std::string b = fresh() + "_b";
		const size_t type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
		out.selectors.push_back(b);
		out.hidden.emplace_back(b, type);
		tref bname = tau::build_var_name(b);
		tref b_prev = build_out_var_at_t_minus<node>(b, 1, type);
		tref fm = tau::build_wff_always(tau::build_bf_eq(
			build_out_var_at_t<node>(bname, type), b_prev));
		// `always (b[t-1] = 0)` is `b = 0`, b being constant, and keeps
		// the relative step under a temporal operator.
		for (size_t i = 0; i < 2; ++i) {
			tref sel = tau::build_wff_always(i == 0
				? tau::build_bf_eq_0(b_prev) : tau::build_bf_neq_0(b_prev));
			trefs clauses;
			split_clauses<node>(emit(e.args[i]), clauses);
			for (tref c : clauses)
				fm = tau::build_wff_and(fm,
					tau::build_wff_imply(sel, c));
		}
		return fm;
	}

	tref emit(const expr& e) {
		switch (e.op) {
		case kind::literal: return e.spec;
		case kind::complement: return negative(e.args[0].spec);
		case kind::intersection:
			return tau::build_wff_and(emit(e.args[0]), emit(e.args[1]));
		case kind::set_union: return set_union(e);
		}
		return nullptr;
	}
};

// @p e with every literal prepared, or the error of a refused one.
template <NodeType node>
result<program_set_expr<node>> prepare_literals(const program_set_expr<node>& e)
{
	using expr = program_set_expr<node>;
	result<expr> r;
	expr out;
	out.op = e.op;
	if (e.op == expr::kind::literal) {
		TAU_TRY(out.spec, prepare_literal<node>(e.spec));
		return r.with_value(std::move(out));
	}
	for (const auto& a : e.args) {
		TAU_TRY(expr p, prepare_literals<node>(a));
		out.args.push_back(std::move(p));
	}
	return r.with_value(std::move(out));
}

template <NodeType node>
void collect_literal_inputs(const program_set_expr<node>& e,
	std::vector<std::pair<std::string, size_t>>& inputs)
{
	if (e.op == program_set_expr<node>::kind::literal)
		collect_inputs<node>(e.spec, inputs);
	for (const auto& a : e.args) collect_literal_inputs<node>(a, inputs);
}

} // namespace program_set_detail

/** @internal @copydoc compile_program_set @endinternal */
template <NodeType node>
result<program_set_compiled<node>> compile_program_set(
	const program_set_expr<node>& e)
{
	result<program_set_compiled<node>> r;
	TAU_TRY(auto prepared, program_set_detail::prepare_literals<node>(e));
	program_set_compiled<node> out;
	program_set_detail::collect_literal_inputs<node>(prepared, out.inputs);
	auto nnf = nnf_program_set<node>(prepared);
	program_set_detail::compiler<node> c{ out };
	if (out.inputs.empty()) {
		out.input_free = true;
		out.formula = c.plain(nnf);
	} else out.formula = c.emit(nnf);
	LOG_DEBUG << "Program-set expression compiled: " << LOG_FM(out.formula);
	return r.with_value(std::move(out));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PROGRAM_SET_TMPL_H__
