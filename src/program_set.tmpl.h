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

// The main formula of the tau constant @p c, its recurrence relations
// applied (nso_rr_apply), or the term the constant came from when it has
// one, or an error when the relations cannot be applied.
template <NodeType node>
result<tref> spec_of_constant(const typename node::constant& c) {
	result<tref> r;
	tref main = nullptr;
	bool is_spec = false;
	std::optional<result<tref>> applied;
	std::visit([&](const auto& v) {
		using T = std::decay_t<decltype(v)>;
		if constexpr (holds_spec<T>) {
			is_spec = true;
			if (v.term && v.term->get()) main = v.term->get();
			else if (!v.nso_rr.rec_relations.empty())
				applied = nso_rr_apply<node>(v.nso_rr);
			else main = v.nso_rr.main ? v.nso_rr.main->get() : nullptr;
		}
	}, c);
	if (!is_spec) return r.with_error(code::invalid_argument,
		"a program-set literal is a constant of type tau");
	if (applied) { TAU_TRY(main, std::move(*applied)); }
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
		// an expression element of the tau algebra holds its term
		if (tau::get(spec).is(tau::bf))
			return program_set_of_term<node>(spec);
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
	if (tau::get(spec).find_top(is<node, tau::ref>))
		return refuse("a reference to a definition");
	if (tau::get(spec).find_top([](tref n) {
		const auto& t = tau::get(n);
		return t.is_ba_constant() && is_tau_type<node>(t.get_ba_type()); }))
		return refuse("a constant of type tau");
	if (has_program_set_stream<node>(spec))
		return refuse("a hidden stream of a program-set expression");
	tref prepared = unnest_nested_always<node>(spec);
	// A conjunct with no future operator is its `always`, written out so
	// that the literal keeps that reading beside the clauses of a
	// complement or a union, and under the negation of the input-free
	// combination, where `!ψ` would be the invariant `always !ψ` and not
	// `sometimes !ψ`.
	prepared = always_of_bare_conjuncts<node>(prepared);
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

// Whether @p n is an uninterpreted constant as a literal holds it: a `bf`
// over a variable named by a uconst_name.
template <NodeType node>
bool is_parameter_constant(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.is(tau::bf) && t.child_is(tau::variable)
		&& tau::get(tau::trim(n)).child_is(tau::uconst_name);
}

// The parameters of @p fm (is_parameter_constant), appended to @p params
// in the order they are first met; one entry per name.
template <NodeType node>
void collect_parameters(tref fm,
	std::vector<program_set_parameter<node>>& params)
{
	using tau = tree<node>;
	for (tref c : tau::get(fm).select_all(is_parameter_constant<node>)) {
		tref var = tau::trim(c);
		std::string name = tau::get(tau::trim(var)).get_string();
		if (std::ranges::none_of(params, [&](const auto& p) {
			return p.name == name; }))
			params.push_back({ name, tau::get(var).get_ba_type(), c });
	}
}

// The part of a stream name that spells the parameter @p name: its
// letters and digits, every other character as `_`, no leading `_`.
inline std::string parameter_stream_suffix(const std::string& name) {
	std::string s;
	for (char ch : name)
		if (std::isalnum((unsigned char) ch)) s += ch;
		else if (!s.empty()) s += '_';
	return s;
}

// The variable of a value key: the key itself, or the variable under a
// `bf` (program_set_parameter::constant, build_bf_uconst).
template <NodeType node>
tref value_key_variable(tref key) {
	using tau = tree<node>;
	return tau::get(key).is(tau::bf) && tau::get(key).has_child()
		? tau::trim(key) : key;
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

// The body of @p clause when it is an `always` with no temporal operator
// inside, a statement of the always part of its specification; nullptr
// otherwise. A parenthesis around the statement is read through.
template <NodeType node>
tref always_part_body(tref clause) {
	using tau = tree<node>;
	while (tau::get(clause).child_is(tau::wff_parenthesis))
		clause = tau::get(clause)[0].first();
	const auto& t = tau::get(clause);
	if (!t.child_is(tau::wff_always)) return nullptr;
	tref body = t[0].first();
	return tau::get(body).find_top(is_temporal_quantifier<node>)
		? nullptr : body;
}

// The start of the always part of the prepared literal @p spec: the
// deepest lookback among its statements, which a decision merges into
// one clause.
template <NodeType node>
int_t always_part_lookback(tref spec) {
	trefs clauses;
	split_clauses<node>(spec, clauses);
	int_t l = 0;
	for (tref c : clauses)
		if (tref b = always_part_body<node>(c))
			l = std::max(l, body_max_lookback<node>(b));
	return l;
}

// The prepared literal @p spec with each statement of its always part
// read at the step and at the 1..d previous ones (delay_part): the part
// then reads lookback d deeper and starts d steps later, where its copies
// reach back to the steps the written part started at. The decision
// merges the always parts of the literals into one clause with one start,
// and so written each literal keeps its own.
template <NodeType node>
tref delay_always_part(tref spec, int_t d) {
	using tau = tree<node>;
	if (d <= 0) return spec;
	// the written literals must reach the decision procedures unfolded
	use_hooks_guard<node> hooks_off(false);
	trefs clauses;
	split_clauses<node>(spec, clauses);
	tref out = nullptr;
	for (tref c : clauses) {
		tref n = c;
		if (tref b = always_part_body<node>(c)) {
			tref body = b;
			for (int_t j = 1; j <= d; ++j)
				body = tau::build_wff_and(body, delay_part<node>(b, j));
			n = tau::build_wff_always(body);
		}
		out = out ? tau::build_wff_and(out, n) : n;
	}
	return out;
}

template <NodeType node>
struct compiler {
	using tau = tree<node>;
	using expr = program_set_expr<node>;
	using kind = typename expr::kind;

	program_set_compiled<node>& out;
	// The deepest start of an always part among the positive literals:
	// every always part of the compiled specification starts there.
	int_t start = 0;
	size_t next = 0;

	std::string fresh() { return "w_ps" + std::to_string(next++); }

	// The positive literal @p spec, its always part started at `start`.
	tref positive(tref spec) {
		return delay_always_part<node>(spec,
			start - always_part_lookback<node>(spec));
	}

	// The plain Boolean combination of the literals: the input-free case.
	tref plain(const expr& e) {
		switch (e.op) {
		case kind::literal: return positive(e.spec);
		case kind::complement:
			return tau::build_wff_neg(e.args[0].spec);
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
	// chooses the operand whose clauses hold. Its constancy is a statement
	// of the always part, so it is written back to `start` like a literal.
	tref set_union(const expr& e) {
		const std::string b = fresh() + "_b";
		const size_t type = get_ba_type_id<node>(bool_carrier_type<node>());
		out.selectors.push_back(b);
		out.hidden.emplace_back(b, type);
		tref bname = tau::build_var_name(b);
		tref b_prev = build_out_var_at_t_minus<node>(b, 1, type);
		tref constant = tau::build_bf_eq(
			build_out_var_at_t<node>(bname, type), b_prev);
		for (int_t j = 1; j <= start; ++j)
			constant = tau::build_wff_and(constant, tau::build_bf_eq(
				build_out_var_at_t_minus<node>(b, (size_t)j, type),
				build_out_var_at_t_minus<node>(b, (size_t)j + 1, type)));
		tref fm = tau::build_wff_always(constant);
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
		case kind::literal: return positive(e.spec);
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

// A complemented literal holding a CTL* operator is refused: a tree
// property has no per-path negation, so the guard construction does not
// apply. A positive one is conjoined like any clause.
template <NodeType node>
result<void> refuse_complemented_ctl_star(const program_set_expr<node>& e) {
	using tau = tree<node>;
	using expr = program_set_expr<node>;
	result<void> r;
	if (e.op == expr::kind::complement
		&& e.args.front().op == expr::kind::literal
		&& has_ctl_star_operators<node>(e.args.front().spec))
		return r.with_error(code::unsupported_operation,
			"a complemented program-set literal is a plain specification; "
			"this one holds a CTL* operator (A, E or -)",
			{{label::value, truncate_for_message(
				tau::get(e.args.front().spec).to_str())}});
	for (const auto& a : e.args)
		TAU_TRY_VOID(refuse_complemented_ctl_star<node>(a));
	return r;
}

template <NodeType node>
void collect_literal_inputs(const program_set_expr<node>& e,
	std::vector<std::pair<std::string, size_t>>& inputs)
{
	if (e.op == program_set_expr<node>::kind::literal)
		collect_inputs<node>(e.spec, inputs);
	for (const auto& a : e.args) collect_literal_inputs<node>(a, inputs);
}

// The deepest start of an always part among the positive literals of the
// NNF @p e.
template <NodeType node>
int_t positive_literals_start(const program_set_expr<node>& e) {
	using kind = typename program_set_expr<node>::kind;
	if (e.op == kind::literal) return always_part_lookback<node>(e.spec);
	if (e.op == kind::complement) return 0;
	int_t k = 0;
	for (const auto& a : e.args)
		k = std::max(k, positive_literals_start<node>(a));
	return k;
}

} // namespace program_set_detail

/** @internal @copydoc program_set_parameters @endinternal */
template <NodeType node>
std::vector<program_set_parameter<node>> program_set_parameters(
	const program_set_expr<node>& e)
{
	std::vector<program_set_parameter<node>> params;
	std::function<void(const program_set_expr<node>&)> walk =
		[&](const program_set_expr<node>& x)
	{
		if (x.op == program_set_expr<node>::kind::literal)
			program_set_detail::collect_parameters<node>(x.spec, params);
		for (const auto& a : x.args) walk(a);
	};
	walk(e);
	return params;
}

/** @internal @copydoc program_set_with_values @endinternal */
template <NodeType node>
program_set_expr<node> program_set_with_values(const program_set_expr<node>& e,
	const subtree_map<node, tref>& values)
{
	using tau = tree<node>;
	using expr = program_set_expr<node>;
	expr out;
	out.op = e.op;
	if (e.op == expr::kind::literal) {
		// The keys are the constants as this literal holds them, matched
		// by their variable: a key built elsewhere may differ in its `bf`.
		subtree_map<node, tref> changes;
		for (tref c : tau::get(e.spec).select_all(
			program_set_detail::is_parameter_constant<node>))
		{
			tref var = tau::trim(c);
			for (const auto& [key, value] : values)
				if (program_set_detail::value_key_variable<node>(key)
					== var) { changes[c] = value; break; }
		}
		out.spec = changes.empty() ? e.spec
			: substitute<node>(e.spec, changes);
		return out;
	}
	for (const auto& a : e.args)
		out.args.push_back(program_set_with_values<node>(a, values));
	return out;
}

/** @internal @copydoc compile_program_set @endinternal */
template <NodeType node>
result<program_set_compiled<node>> compile_program_set(
	const program_set_expr<node>& e, bool universal_parameters)
{
	using tau = tree<node>;
	result<program_set_compiled<node>> r;
	TAU_TRY(auto prepared, program_set_detail::prepare_literals<node>(e));
	// the selectors in the carrier the literals' types name
	trefs literal_specs;
	auto collect = [&](const auto& self, const program_set_expr<node>& x)
		-> void {
		if (x.op == program_set_expr<node>::kind::literal)
			literal_specs.push_back(x.spec);
		for (const auto& a : x.args) self(self, a);
	};
	collect(collect, prepared);
	bool_carrier_scope<node> carrier(literal_specs);
	program_set_compiled<node> out;
	program_set_detail::collect_literal_inputs<node>(prepared, out.inputs);
	out.parameters = program_set_parameters<node>(prepared);
	auto nnf = nnf_program_set<node>(prepared);
	TAU_TRY_VOID(program_set_detail::refuse_complemented_ctl_star<node>(nnf));
	program_set_detail::compiler<node> c{ out,
		program_set_detail::positive_literals_start<node>(nnf) };
	// The hidden input of each parameter stands where the constant stood,
	// kept at the value the environment picks at step 0. The lag is read
	// from step 1 on: a lag on a hidden stream gives no warm-up
	// (body_max_lookback), and a value before step 0 would be the
	// system's to choose, as a hidden output's is.
	tref kept = nullptr;
	if (universal_parameters && !out.parameters.empty()) {
		subtree_map<node, tref> to_stream;
		tref step_positive = tau::build_wff_ctn_gt(
			tau::get(tau::ctnvar, "t"), tau::get(node(tau::num, (size_t) 0)));
		for (const auto& p : out.parameters) {
			const std::string w = c.fresh() + "_p_"
				+ program_set_detail::parameter_stream_suffix(p.name);
			out.parameter_inputs.push_back(w);
			tref wname = tau::build_var_name(w);
			to_stream[p.constant] = build_in_var_at_t<node>(wname, p.type);
			tref keep = tau::build_wff_always(tau::build_wff_imply(
				step_positive, tau::build_bf_eq(
					build_in_var_at_t<node>(wname, p.type),
					build_in_var_at_t_minus<node>(wname, 1, p.type))));
			kept = kept ? tau::build_wff_and(kept, keep) : keep;
		}
		nnf = program_set_with_values<node>(nnf, to_stream);
	}
	if (out.inputs.empty()) {
		out.input_free = true;
		out.formula = c.plain(nnf);
	} else out.formula = c.emit(nnf);
	if (kept) out.formula = tau::build_wff_imply(kept, out.formula);
	LOG_DEBUG << "Program-set expression compiled: " << LOG_FM(out.formula);
	return r.with_value(std::move(out));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PROGRAM_SET_TMPL_H__
