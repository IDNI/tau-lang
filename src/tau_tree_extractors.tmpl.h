// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "extractors"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// various extractors

template <NodeType node>
size_t get_type_sid(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	static size_t untyped_sid = dict("untyped");
	if (auto t = tt(n) | tau::type; t) return t | tt::data;
	return untyped_sid;
}

template <NodeType node>
rr_sig get_rr_sig(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto r = tt(n); // traverse to ref if n is bf_ref or wff_ref
	if (auto r_as_child = r | tau::ref; r_as_child) r = r_as_child;
	return { rr_dict(r | tau::sym | tt::string),
		(r | tau::offsets || tau::offset).size(),
		(r | tau::ref_args || tau::ref_arg).size() };
}

template <NodeType node>
bool get_io_def(tref n, io_defs<node>& defs) {
	using tau = tree<node>;
	using tt = tau::traverser;

	const auto& t            = tau::get(n);
	size_t ba_type           = t.get_ba_type();
	size_t var_sid           = t[0].data();
	size_t stream_sid        = 0;

	if (auto fn = tt(n) | tau::stream | tau::q_file_name | tau::file_name;
		fn)
	{
		stream_sid = fn | tt::data;
		static const size_t console_sid = dict("console");
		if (stream_sid == console_sid) stream_sid = 0;
	}
	if (ba_type == 0) {
		defs[var_sid] = { 0, stream_sid };
		return true;
	}
	std::string ba_type_name = get_ba_type_name<node>(ba_type);
	for (const auto& fct_type : node::nso_factory::instance().types()) {
		if (ba_type_name == fct_type) {
			defs[var_sid] = { ba_type, stream_sid };
			return true;
		}
	}
	LOG_ERROR << "Invalid type " << ba_type_name;
	return false;
}

template <NodeType node>
bool get_io_defs(spec_context<node>& ctx, tref code) {
	using tau = tree<node>;
	for (tref r : tau::get(code).select_top(is<node, tau::input_def>))
		if (!get_io_def<node>(r, ctx.inputs))
				return false;
	for (tref r : tau::get(code).select_top(is<node, tau::output_def>))
		if (!get_io_def<node>(r, ctx.outputs))
				return false;
	LOG_TRACE << ctx;
	return true;
}

template <NodeType node>
tref resolve_io_vars(spec_context<node>& ctx, tref fm) {
	LOG_TRACE << "resolve_io_vars - fm: " << LOG_FM_DUMP(fm);
	using tau = tree<node>;
	auto resolve = [&ctx](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::io_var)) {
			size_t var_sid = get_var_name_sid<node>(n);
			for (const auto& [in_var_sid, def] : ctx.inputs)
				if (var_sid == in_var_sid)
					return t.replace_value(
						t.value.replace_data(1));
			for (const auto& [out_var_sid, def] : ctx.outputs)
				if (var_sid == out_var_sid)
					return t.replace_value(
						t.value.replace_data(2));

						static const auto io_prefixed_io_var =
				[](size_t var_sid) -> size_t
			{
				return (dict(var_sid)[0] == 'i' || dict(var_sid) == "this")
						? 1
						: ((dict(var_sid)[0] == 'o' ||
						dict(var_sid) == "u")
							? 2
							: 0);
			};
			size_t direction = io_prefixed_io_var(var_sid);
			DBG(LOG_TRACE << "io_prefixed_io_var: " << dict(var_sid)
				<< " " << (direction == 1 ? "IN"
					: (direction == 2 ? "OUT"
					: "UNRESOLVED I/O"));)
			if (direction) return t.replace_value(
					t.value.replace_data(direction));
		}
		return n;
	};
	tref resolved = pre_order<node>(fm).apply_unique(resolve);
	LOG_TRACE << "resolve_io_vars - resolved: " << LOG_FM_DUMP(resolved);
	return resolved;
}

template <NodeType node>
rewriter::rules get_rec_relations(spec_context<node>& ctx, tref rrs) {
	using tau = tree<node>;
	using tt = tau::traverser;
	rewriter::rules x;
	DBG(LOG_TRACE << "get_rec_relations: " << LOG_FM_DUMP(rrs);)
	auto t = tt(rrs);

	auto resolve_in_formula = [&x, &ctx](tt& t) {
		x.emplace_back(t | tt::first  | tt::handle,
			tau::geth(resolve_io_vars<node>(ctx,
						t | tt::second | tt::ref)));
	};

	if (t.is(tau::rec_relation)) return resolve_in_formula(t), x;
	if (t.is(tau::start)) t = t | tau::spec | tau::definitions;
	else if (t.is(tau::spec))  t = t | tau::definitions;
	else if (t.is(tau::tau_constant_source)) t = t | tau::rec_relations;
	t = t || tau::rec_relation;
	for (auto& r : t()) resolve_in_formula(r);

	DBG(LOG_TRACE << "get_rec_relations number of defs: " << x.size();)
	return x;
}

template <NodeType node>
rewriter::rules get_rec_relations(tref rrs) {
	spec_context<node> ctx;
	return get_rec_relations<node>(ctx, rrs);
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(spec_context<node>& ctx, tref r, bool wo_inference) {
	using tau = tree<node>;
	using tt = tau::traverser;
	DBG(LOG_TRACE << "get_nso_rr: " << LOG_FM(r);)
	const auto& t = tau::get(r).is(tau::start) ? tau::get(r)[0]
						   : tau::get(r);
	r = t.get();
	if (t.is(tau::bf) || t.is(tau::ref)) return { { {}, tau::geth(r) } };
	if (t.is(tau::rec_relation))
		return { { get_rec_relations<node>(ctx, r), (htref) nullptr } };
	if (t.is(tau::spec)) get_io_defs<node>(ctx, r);
	LOG_TRACE << "get_nso_rr - r: " << LOG_FM_DUMP(r);
	tref main_fm = resolve_io_vars<node>(ctx, tt(r) | tau::main
							| tau::wff | tt::ref);
	rewriter::rules rules = get_rec_relations<node>(ctx, r);
	DBG(LOG_TRACE << "rules: " << rules.size();)
	auto nso_rr = wo_inference
			? std::optional<rr<node>>{ { rules, tau::geth(main_fm) } }
			: get_nso_rr<node>(rules, main_fm);

	auto check_resolved_io_vars = [](htref form) {
		for (tref io_var : tau::get(form).select_all(is<node, tau::io_var>)) {
			// LOG_TRACE << "io_var: " << LOG_FM_DUMP(io_var);
			if (tau::get(io_var).data() == 0) {
				LOG_ERROR << "I/O variable is not defined "
					<< TAU_TO_STR(io_var);
				return false;
			}
		}
		return true;
	};
	if (nso_rr) {
		if (!check_resolved_io_vars(nso_rr.value().main)) return {};
		for (const auto& rec_relation : nso_rr.value().rec_relations)
			if (!check_resolved_io_vars(rec_relation.second))
				return {};
	}

#ifdef DEBUG
	if (nso_rr) LOG_TRACE << "get_nso_rr result: "<< LOG_RR(nso_rr.value());
	else LOG_TRACE << "get_nso_rr result: no value";
#endif
	return nso_rr;
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(tref r, bool wo_inference) {
	spec_context<node> ctx;
	return get_nso_rr<node>(ctx, r, wo_inference);
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(const rewriter::rules& rules,
	tref main_fm)
{
	return get_nso_rr<node>(rules, tree<node>::geth(main_fm));
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(const rewriter::rules& rules,
	const htref& main_fm)
{
	return infer_ref_types<node>({ rules, main_fm });
}

// -----------------------------------------------------------------------------

template <NodeType node>
void get_leaves(tref n, typename node::type branch, trefs& leaves) {
	using tau = tree<node>;
	auto add_leave = [&branch, &leaves](tref n) {
		const auto& t = tau::get(n);
		if (t.is(branch)) return true;
		if (t.child_is(branch)) return true;
		LOG_TRACE << "adding leaf: " << LOG_FM(n);
		return leaves.push_back(n), false;
	};
	pre_order<node>(n).visit(add_leave);
}

template <NodeType node>
trefs get_leaves(tref n, typename node::type branch) {
	trefs leaves;
	get_leaves<node>(n, branch, leaves);
	LOG_TRACE << "got leaves: " << leaves.size();
	for (tref l : leaves) LOG_TRACE << "leaf: " << LOG_FM(l);
	return leaves;
}

template <NodeType node>
trefs get_dnf_wff_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting dnf wff clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::wff_or);
}

template <NodeType node>
trefs get_dnf_bf_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting dnf bf clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::bf_or);
}

template <NodeType node>
trefs get_cnf_wff_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting cnf wff clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::wff_and);
}

template <NodeType node>
trefs get_cnf_bf_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting cnf bf clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::bf_and);
}

// -----------------------------------------------------------------------------

template <NodeType node>
size_t get_ba_type(tref n) {
	return tree<node>::get(n).get_ba_type();
}

template <NodeType node>
tref get_var_name_node(tref var) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto v = tt(var);
	if (v.is(tau::bf_constant) || v.is(tau::var_name)) return var;
	// TODO: refactor
	if (auto vn = v | tau::var_name; vn) return vn.value();
	if (auto vn = v | tau::io_var | tau::var_name; vn) return vn.value();
	if (auto vn = v | tau::uconst_name; vn) return vn.value();
	if (auto vn = v | tau::variable | tau::var_name; vn) return vn.value();
	if (auto vn = v | tau::variable | tau::io_var | tau::var_name;
		vn) return vn.value();
	if (auto vn = v | tau::variable | tau::uconst_name; vn) return vn.value();
	if (auto vn = v | tau::bf | tau::variable | tau::var_name;
		vn) return vn.value();
	if (auto vn = v | tau::bf | tau::variable | tau::io_var | tau::var_name;
		vn) return vn.value();
	if (auto vn = v | tau::bf | tau::variable | tau::uconst_name;
		vn) return vn.value();
	return nullptr;
}

template <NodeType node>
const std::string& get_var_name(tref var) {
	return tree<node>::get(get_var_name_node<node>(var)).get_string();
}

template <NodeType node>
size_t get_var_name_sid(tref var) {
	return tree<node>::get(get_var_name_node<node>(var)).data();
}

// -----------------------------------------------------------------------------
// Helpers for variables having io_var as child

template <NodeType node>
bool is_io_initial(tref io_var) {
	return tree<node>::get(io_var)[0][1][0].is_integer();
}

template <NodeType node>
bool is_io_shift(tref io_var) {
	using tau = tree<node>;
	return tau::get(io_var)[0][1][0].is(tau::shift);
}

template <NodeType node>
int_t get_io_time_point(tref io_var) {
	return tree<node>::get(io_var)[0][1][0].get_integer();
}

template <NodeType node>
int_t get_io_shift(tref io_var) {
	return tree<node>::get(io_var)[0][1][0][1].get_num();
}

template <NodeType node>
int_t get_io_var_shift(tref io_var) {
	// If there is a shift
	if (is_io_shift<node>(io_var)) return get_io_shift<node>(io_var);
	return 0;
}

template <NodeType node>
int_t get_max_shift(const trefs& io_vars, bool ignore_temps) {
	int_t max_shift = 0;
	for (tref v : io_vars) {
		if (ignore_temps && get_var_name<node>(v)[0] == '_')
			continue;
		max_shift = std::max(max_shift, get_io_var_shift<node>(v));
	}
	return max_shift;
}

template <NodeType node>
int_t get_max_initial(const trefs& io_vars) {
	int_t max_init = -1;
	for (tref v : io_vars) {
		if (is_io_initial<node>(v)) {
			int_t init = get_io_time_point<node>(v);
			max_init = std::max(max_init, init);
		}
	}
	return max_init;
}

template <NodeType node>
subtree_set<node> get_free_vars_from_nso(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "Begin get_free_vars_from_nso of " << LOG_FM(n);
	subtree_set<node> free_vars;
	LOG_TRACE << "End get_free_vars_from_nso" << LOG_FM(n);
	auto collector = [&free_vars](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff_all) || t.is(tau::wff_ex)) {
			tref var = t.find_top(
				(bool(*)(tref)) is_var_or_capture<node>);
			if (var) if (auto it = free_vars.find(var);
				it != free_vars.end())
			{
				LOG_TRACE << "removing quantified var: "
								<< LOG_FM(var);
				free_vars.erase(it);
			}
		} else if (is_var_or_capture<node>(n)) {
			if (auto offset_child = t() | tau::io_var | tau::offset
				| tt::only_child; offset_child)
			{
				if (is_var_or_capture<node>(
					offset_child.value()))
				{
					tref var = offset_child | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						LOG_TRACE << "removing var: "
							<< LOG_FM(offset_child
								.value());
						free_vars.erase(it);
					}
				} else if (offset_child.value_tree()
								.is(tau::shift))
				{
					tref var = offset_child
						| tt::first | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						LOG_TRACE << "removing var: "
							<< LOG_FM(offset_child
								.value());
						free_vars.erase(it);
					}
				}
			}
			LOG_TRACE << "inserting var: " << LOG_FM(n);
			free_vars.insert(n);
		}
	};
	post_order<node>(n).search(collector);
	return free_vars;
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <NodeType node>
bool has_temp_var(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	trefs io_vars = t.select_top(is<node, tau::io_var>);
	if (io_vars.empty())
		return t.find_top(is<node, tau::constraint>) != nullptr;
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

template <NodeType node>
bool has_open_tau_fm_in_constant(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto _closed = [](const auto& c) -> bool { return is_closed(c); };
	trefs consts = tau::get(fm).select_top(is_child<node, tau::bf_constant>);
	for (tref c : consts) {
		auto ba_const = tt(c) | tau::bf_constant | tt::ba_constant;
		if (!std::visit(_closed, ba_const)) {
			LOG_ERROR << "A Tau formula constant must be closed: "
								<< ba_const;
			return true;
		}
	}
	return false;
}

} // namespace idni::tau_lang