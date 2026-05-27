// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_builders.tmpl.h - Algorithms A/B/C/D, solve, realize, safety, explain, CTL*
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {


// ── Algorithm A: binary T_3 type encoding ────────────────────────────────────
//
// TAU_LTL_ALG=A selects this path for single-stream pure-qlt formulas.
// Enumerates T_3 = (memory, input, output) order-types for (ℚ,<), binary-encodes
// them into ⌈log₂|T_3|⌉ system output Q-bits, and calls ltlsynt on the resulting
// propositional skeleton. No ABA oracle needed: the T_3 encoding is exact.
// Restricted to: all atoms qlt-type, single output var, lookback ≤ 1.

// Role of an io_var in the (m=memory, x=input, y=output) T_3 triple.
enum class T3VarRole { M, X, Y };

template <NodeType node>
static std::optional<T3VarRole> t3_role_of(tref io_var) {
	const std::string& nm = get_var_name<node>(io_var);
	if (nm.empty()) return std::nullopt;
	int_t shift = get_io_var_shift<node>(io_var);
	if (nm[0] == 'o' && shift == 0) return T3VarRole::Y;
	if (nm[0] == 'i' && shift == 0) return T3VarRole::X;
	if (nm[0] == 'o' && shift == 1) return T3VarRole::M;
	return std::nullopt;
}

// True iff all atoms are qlt-typed, have lookback ≤ 1, and each comparison
// side has at most one io_var (no compound expressions like o1 & i1).
template <NodeType node>
static bool is_algorithm_a_applicable(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	if (atoms.empty()) return false;
	for (auto& [f, _] : atoms) {
		if (!is_omcat_type_family<node>(find_ba_type<node>(f))) return false;
		auto bad = tau::get(f).find_top([](tref n) {
			if (!is_child<node>(n, tau::io_var)) return false;
			return get_io_var_shift<node>(n) > 1;
		});
		if (bad) return false;
		// Reject compound atoms where a comparison side has >1 io_var
		// (e.g. (o1 & i1) = {c}). T3 encoding only handles single-var sides.
		const auto& t = tau::get(f);
		if (t.has_child()) {
			tref lhs = t[0].first();
			tref rhs = t[0].second();
			if (lhs && tau::get(lhs).select_top(is_child<node, tau::io_var>).size() > 1)
				return false;
			if (rhs && tau::get(rhs).select_top(is_child<node, tau::io_var>).size() > 1)
				return false;
		}
	}
	return true;
}

// Returns true/false if the qlt comparison atom holds in T3, nullopt if unsupported.
template <NodeType node>
static std::optional<bool> qlt_atom_holds_in_type3(
	tref atom,
	const omcat::QltType3& T3,
	const std::vector<omcat::Rat>& constants)
{
	using tau = tree<node>;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return std::nullopt;
	auto op = t[0].value.nt;
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return std::nullopt;

	tref lhs = t[0].first();
	tref rhs = t[0].second();
	// find_top returns the PARENT node (whose first child is io_var) because
	// get_var_name / get_io_var_shift expect that parent, not the io_var itself.
	tref lhs_io = tau::get(lhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });
	tref rhs_io = tau::get(rhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });

	auto flip_rel = [](omcat::Rel r) {
		return r == omcat::Rel::LT ? omcat::Rel::GT :
		       r == omcat::Rel::GT ? omcat::Rel::LT : omcat::Rel::EQ;
	};
	auto rel_holds = [&](omcat::Rel rel, size_t nt) -> bool {
		if (nt == tau::bf_lt)   return rel == omcat::Rel::LT;
		if (nt == tau::bf_lteq) return rel != omcat::Rel::GT;
		if (nt == tau::bf_gt)   return rel == omcat::Rel::GT;
		if (nt == tau::bf_gteq) return rel != omcat::Rel::LT;
		if (nt == tau::bf_eq)   return rel == omcat::Rel::EQ;
		if (nt == tau::bf_neq)  return rel != omcat::Rel::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto rl = t3_role_of<node>(lhs_io);
		auto rr = t3_role_of<node>(rhs_io);
		if (!rl || !rr) return std::nullopt;
		omcat::Rel rel;
		auto p = std::make_pair(*rl, *rr);
		using R = T3VarRole;
		if      (p == std::make_pair(R::M, R::X)) rel = T3.rel_mx;
		else if (p == std::make_pair(R::X, R::M)) rel = flip_rel(T3.rel_mx);
		else if (p == std::make_pair(R::M, R::Y)) rel = T3.rel_my;
		else if (p == std::make_pair(R::Y, R::M)) rel = flip_rel(T3.rel_my);
		else if (p == std::make_pair(R::X, R::Y)) rel = T3.rel_xy;
		else if (p == std::make_pair(R::Y, R::X)) rel = flip_rel(T3.rel_xy);
		else return std::nullopt; // same role (e.g. y vs y)
		return rel_holds(rel, op);
	}

	if (lhs_io || rhs_io) {
		tref io_var = lhs_io ? lhs_io : rhs_io;
		tref const_side = lhs_io ? rhs : lhs;
		bool io_is_lhs = (lhs_io != nullptr);
		auto role = t3_role_of<node>(io_var);
		if (!role) return std::nullopt;
		omcat::QltType1 t1;
		if      (*role == T3VarRole::M) t1 = T3.restrict_m();
		else if (*role == T3VarRole::X) t1 = T3.restrict_x();
		else                            t1 = T3.restrict_y();
		auto cs = omcat::collect_qlt_constants<node>(const_side);
		if (cs.size() != 1) return std::nullopt;
		int j = -1;
		for (int k = 0; k < (int)constants.size(); ++k)
			if (omcat::cmp(constants[k], cs[0]) == 0) { j = k; break; }
		if (j < 0) return std::nullopt;
		auto eff = op;
		if (!io_is_lhs) {
			if      (eff == tau::bf_lt)   eff = tau::bf_gt;
			else if (eff == tau::bf_gt)   eff = tau::bf_lt;
			else if (eff == tau::bf_lteq) eff = tau::bf_gteq;
			else if (eff == tau::bf_gteq) eff = tau::bf_lteq;
		}
		if (eff == tau::bf_lt)   return t1.less_than(j);
		if (eff == tau::bf_lteq) return t1.less_than(j) || t1.equal_to(j);
		if (eff == tau::bf_gt)   return t1.greater_than(j);
		if (eff == tau::bf_gteq) return t1.greater_than(j) || t1.equal_to(j);
		if (eff == tau::bf_eq)   return t1.equal_to(j);
		if (eff == tau::bf_neq)  return !t1.equal_to(j);
		return std::nullopt;
	}
	return std::nullopt;
}

// Evaluate a pure-output atom under a per-variable T₁ assignment (constant-output
// strategy). Variable name keyed — unlike qlt_atom_holds_in_type3 which uses
// fixed M/X/Y roles and can't distinguish o1 from o2. Treats o_k[t-s] (any
// shift) as aliasing to o_k, since the strategy is constant over time.
// Returns nullopt if the atom involves an input variable or can't be evaluated.
template <NodeType node>
static std::optional<bool> eval_pure_output_atom_at(
	tref atom,
	const std::map<std::string, int>& var_pos,
	const std::vector<omcat::Rat>& constants)
{
	using tau = tree<node>;
	if (atom_has_any_input<node>(atom)) return std::nullopt;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return std::nullopt;
	auto op = t[0].value.nt;
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return std::nullopt;

	tref lhs = t[0].first();
	tref rhs = t[0].second();
	tref lhs_io = tau::get(lhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });
	tref rhs_io = tau::get(rhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });

	auto lookup = [&](tref io_parent) -> std::optional<int> {
		const std::string& nm = get_var_name<node>(io_parent);
		auto it = var_pos.find(nm);
		if (it == var_pos.end()) return std::nullopt;
		return it->second;
	};

	auto rel_holds = [&](omcat::Rel rel, size_t o) -> bool {
		if (o == tau::bf_lt)   return rel == omcat::Rel::LT;
		if (o == tau::bf_lteq) return rel != omcat::Rel::GT;
		if (o == tau::bf_gt)   return rel == omcat::Rel::GT;
		if (o == tau::bf_gteq) return rel != omcat::Rel::LT;
		if (o == tau::bf_eq)   return rel == omcat::Rel::EQ;
		if (o == tau::bf_neq)  return rel != omcat::Rel::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto p1 = lookup(lhs_io);
		auto p2 = lookup(rhs_io);
		if (!p1 || !p2) return std::nullopt;
		omcat::QltType1 t1a{*p1, constants};
		omcat::QltType1 t1b{*p2, constants};
		omcat::Rel rel;
		if (*p1 == *p2) {
			// Same T₁ position: constant strategy picks same value → EQ.
			rel = omcat::Rel::EQ;
		} else {
			omcat::Rat va = t1a.realize();
			omcat::Rat vb = t1b.realize();
			int c = omcat::cmp(va, vb);
			rel = c < 0 ? omcat::Rel::LT : (c == 0 ? omcat::Rel::EQ : omcat::Rel::GT);
		}
		return rel_holds(rel, op);
	}
	if (lhs_io || rhs_io) {
		tref io = lhs_io ? lhs_io : rhs_io;
		bool io_is_lhs = (lhs_io != nullptr);
		auto p = lookup(io);
		if (!p) return std::nullopt;
		omcat::QltType1 t1{*p, constants};
		auto cs = omcat::collect_qlt_constants<node>(io_is_lhs ? rhs : lhs);
		// Range constant handling: {[a,b]} collects two endpoints.
		// Interpret `io_var = {[a,b]}` as closed-interval membership,
		// `!=` as non-membership.
		if (cs.size() == 2 && (op == tau::bf_eq || op == tau::bf_neq)) {
			omcat::Rat a = cs[0], b = cs[1];
			if (omcat::cmp(a, b) > 0) std::swap(a, b);
			int ja = -1, jb = -1;
			for (int k = 0; k < (int)constants.size(); ++k) {
				if (omcat::cmp(constants[k], a) == 0) ja = k;
				if (omcat::cmp(constants[k], b) == 0) jb = k;
			}
			if (ja < 0 || jb < 0) return std::nullopt;
			// Membership: pos_y ∈ [2*ja+1, 2*jb+1] (point-at-a through point-at-b).
			long lo_pos = 2L * ja + 1;
			long hi_pos = 2L * jb + 1;
			long pp = static_cast<long>(*p);
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wstrict-overflow"
			bool in_range = (pp >= lo_pos && pp <= hi_pos);
			#pragma GCC diagnostic pop
			return (op == tau::bf_eq) ? in_range : !in_range;
		}
		if (cs.size() != 1) return std::nullopt;
		int j = -1;
		for (int k = 0; k < (int)constants.size(); ++k)
			if (omcat::cmp(constants[k], cs[0]) == 0) { j = k; break; }
		if (j < 0) return std::nullopt;
		auto eff = op;
		if (!io_is_lhs) {
			if      (eff == tau::bf_lt)   eff = tau::bf_gt;
			else if (eff == tau::bf_gt)   eff = tau::bf_lt;
			else if (eff == tau::bf_lteq) eff = tau::bf_gteq;
			else if (eff == tau::bf_gteq) eff = tau::bf_lteq;
		}
		if (eff == tau::bf_lt)   return t1.less_than(j);
		if (eff == tau::bf_lteq) return t1.less_than(j) || t1.equal_to(j);
		if (eff == tau::bf_gt)   return t1.greater_than(j);
		if (eff == tau::bf_gteq) return t1.greater_than(j) || t1.equal_to(j);
		if (eff == tau::bf_eq)   return t1.equal_to(j);
		if (eff == tau::bf_neq)  return !t1.equal_to(j);
	}
	return std::nullopt;
}

// Pre-check: is the formula REALIZABLE via a constant-output strategy?
// Enumerates T₁ positions per output variable; for each combo, substitutes
// pure-output atom truth values into the LTL skeleton and uses `ltlfilt` to
// simplify. If any combo reduces to "1" (true), the system wins with that
// constant output choice. This fast-path avoids the expensive Algorithm B
// ltlsynt call for formulas with trivially-satisfiable U/W/R right-sides.
template <NodeType node>
static bool constant_output_realizable(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	std::set<std::string> out_names;
	for (auto& [f, _] : atoms) {
		auto ios = tau::get(f).select_top(is_child<node, tau::io_var>);
		for (tref io_parent : ios) {
			const std::string& nm = get_var_name<node>(io_parent);
			if (!nm.empty() && nm[0] == 'o') out_names.insert(nm);
		}
	}
	if (out_names.empty()) return false;

	auto constants = omcat::collect_qlt_constants<node>(fm);
	int T1_size = 2 * (int)constants.size() + 1;
	if (T1_size <= 0) return false;

	std::vector<std::string> out_vec(out_names.begin(), out_names.end());
	int n_out = (int)out_vec.size();
	unsigned long long total_u = 1;
	const unsigned long long CAP = 100ULL;
	for (int i = 0; i < n_out; ++i) {
		if (total_u > CAP) return false;
		total_u *= (unsigned long long)T1_size;
		if (total_u > CAP) return false;
	}
	long long total = (long long)total_u;

	std::string phi_star_base = ltl_skeleton<node>(fm, atoms);

	for (long long combo = 0; combo < total; ++combo) {
		std::map<std::string, int> var_pos;
		unsigned long long rem = (unsigned long long)combo;
		for (int i = 0; i < n_out; ++i) {
			var_pos[out_vec[i]] = (int)(rem % (unsigned long long)T1_size);
			rem /= (unsigned long long)T1_size;
		}

		std::string phi = phi_star_base;
		for (int i = (int)atoms.size(); i-- > 0; ) {
			auto val = eval_pure_output_atom_at<node>(
				atoms[i].first, var_pos, constants);
			if (!val) continue;
			std::string fp = "p" + std::to_string(i);
			std::string rep = *val ? "true" : "false";
			size_t pos = 0;
			while ((pos = phi.find(fp, pos)) != std::string::npos) {
				size_t end = pos + fp.size();
				bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi[pos-1])
				                         && phi[pos-1] != '_');
				bool r_ok = end >= phi.size()
				         || (!std::isalnum((unsigned char)phi[end])
				             && phi[end] != '_');
				if (l_ok && r_ok) { phi.replace(pos, fp.size(), rep); pos += rep.size(); }
				else pos = end;
			}
		}

		// Quick pre-filter: a formula with remaining input props can only
		// be a tautology if temporal operators make those props irrelevant.
		// The patterns "A U true", "A W true", "A R true" can propagate to 1.
		// Skip ltlfilt calls where none of these patterns are present.
		bool has_input_prop = false;
		for (size_t ci = 0; ci + 1 < phi.size(); ++ci)
			if (phi[ci] == 'p' && std::isdigit((unsigned char)phi[ci+1])) {
				has_input_prop = true; break;
			}
		if (has_input_prop) {
			bool maybe_taut = (phi.find("U true") != std::string::npos ||
			                   phi.find("W true") != std::string::npos ||
			                   phi.find("R true") != std::string::npos ||
			                   phi.find("U 1)") != std::string::npos  ||
			                   phi.find("W 1)") != std::string::npos  ||
			                   phi.find("R 1)") != std::string::npos);
			if (!maybe_taut) continue;
		}

		// Shell-escape for single-quoted arg.
		std::string escaped;
		for (char c : phi) {
			if (c == '\'') escaped += "'\\''";
			else escaped += c;
		}
		std::string cmd = "ltlfilt -f '" + escaped + "' 2>/dev/null";
		auto [out, rc] = run_cmd(cmd);
		while (!out.empty() && std::isspace((unsigned char)out.back())) out.pop_back();
		if (rc == 0 && out == "1") {
			LOG_DEBUG << "[ltl_aba] constant-output fast-path REALIZABLE "
			          << "(combo=" << combo << ")";
			return true;
		}
	}
	return false;
}

template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba_algorithm_a(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	auto constants = omcat::collect_qlt_constants<node>(fm);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int n_types = (int)T3.size();
	LOG_DEBUG << "[ltl_aba:algA] T3 count=" << n_types
	          << " constants=" << constants.size();
	if (n_types == 0) return std::nullopt;

	int K = (int)atoms.size();
	// atom_mask[i] = T₃ type indices where D_i holds (true or undetermined).
	std::vector<std::vector<int>> atom_mask(K);
	for (int i = 0; i < K; ++i)
		for (int t = 0; t < n_types; ++t) {
			auto h = qlt_atom_holds_in_type3<node>(atoms[i].first, T3[t], constants);
			if (h != false) atom_mask[i].push_back(t); // true or undetermined: include
		}

	// Build per-T₃-type D-bitmask, then extract feasible (sigma, rho, A) triples.
	std::vector<int> type_A(n_types, 0);
	for (int i = 0; i < K; ++i)
		for (int t : atom_mask[i])
			type_A[t] |= (1 << i);

	int T1_size = 2 * (int)constants.size() + 1;
	std::vector<std::tuple<int,int,int>> feasible_set;
	feasible_set.reserve(n_types);
	for (int t = 0; t < n_types; ++t)
		feasible_set.emplace_back(T3[t].pos_m, T3[t].pos_y, type_A[t]);

	// Build phi* skeleton and rename p_i → D_i (highest index first).
	std::string phi_star = ltl_skeleton<node>(fm, atoms);
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1])
			                         && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
			         || (!std::isalnum((unsigned char)phi_star[end])
			             && phi_star[end] != '_');
			if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
			else pos = end;
		}
	}

	auto bundle = alg_a::build_algorithm_a_skeleton(T1_size, K, feasible_set, phi_star);
	LOG_DEBUG << "[ltl_aba:algA] skeleton: " << bundle.formula;

	// R-bits + D-bits are system outputs; phi* uses D_i directly as propositions.
	std::vector<std::string> input_props;
	std::vector<std::string> output_props(bundle.outs.begin(), bundle.outs.end());

	auto [realizable, hoa_text] = call_ltlsynt(bundle.formula, input_props, output_props);
	if (!realizable) return std::nullopt;

	LtlAbaSolution<node> sol;
	// Populate sol.atoms with the d_i propositions phi_star uses, so
	// downstream consumers (the codegen witness emitter in
	// cpp_codegen.tmpl.h, the safety-formula extractor in
	// ltl_to_safety_formula) can map AP names back to the original data
	// atoms and emit qlt witnesses, executable safety formulas, etc.
	// Previously sol.atoms was left empty, which forced the codegen down
	// the propositional fallback that emits `bool o_d_0` instead of
	// `double o1` — failing test_cpp_codegen_data_atoms's structural
	// checks.
	//
	// Also populate sol.output_props with the d_i names so
	// emit_cpp_program_data's Outputs-struct emission iterates over the
	// data-atom propositions and fills in `double <var>` fields.
	sol.atoms.reserve(atoms.size());
	sol.output_props.reserve(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i) {
		std::string name = "d_" + std::to_string(i);
		sol.atoms.emplace_back(atoms[i].first, name);
		sol.output_props.push_back(name);
	}
	sol.aut = parse_hoa(hoa_text);
	return sol;
}

// Algorithm B: P_σ binary encoding — adds ⌈log₂|T₂|⌉ input propositions for
// the T₂ = (pos_m, pos_x, rel_mx) type.  Needed for SOUNDNESS when the formula
// contains input-variable atoms (the system observes x's type via P-bits and can
// then pick the correct output type ρ).
template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba_algorithm_b(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	auto constants = omcat::collect_qlt_constants<node>(fm);
	auto T2 = omcat::enumerate_qlt_T2(constants);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int T2_size = (int)T2.size();
	int n_types = (int)T3.size();
	if (n_types == 0 || T2_size == 0) return std::nullopt;

	int K       = (int)atoms.size();
	int T1_size = 2 * (int)constants.size() + 1;

	// Compute D-bitmask per T₃ type.
	std::vector<int> type_A(n_types, 0);
	for (int i = 0; i < K; ++i)
		for (int t = 0; t < n_types; ++t) {
			auto h = qlt_atom_holds_in_type3<node>(atoms[i].first, T3[t], constants);
			if (h != false) type_A[t] |= (1 << i);
		}

	// Build T₂ lookup: (pos_m, pos_x, rel_mx) → T₂ index.
	std::map<std::tuple<int,int,int>, int> t2_lookup;
	for (int s = 0; s < T2_size; ++s)
		t2_lookup[{ T2[s].pos_m, T2[s].pos_x, (int)T2[s].rel }] = s;

	// Build feasible_set_b: (T2_idx, rho, A).
	std::vector<std::tuple<int,int,int>> feasible_set_b;
	feasible_set_b.reserve(n_types);
	for (int t = 0; t < n_types; ++t) {
		auto key = std::make_tuple(T3[t].pos_m, T3[t].pos_x, (int)T3[t].rel_mx);
		auto it  = t2_lookup.find(key);
		if (it == t2_lookup.end()) continue;
		feasible_set_b.emplace_back(it->second, T3[t].pos_y, type_A[t]);
	}

	// t2_pos_m[σ] = pos_m of T₂[σ].
	std::vector<int> t2_pos_m(T2_size);
	for (int s = 0; s < T2_size; ++s) t2_pos_m[s] = T2[s].pos_m;

	// Build phi* skeleton and rename p_i → d_i.
	std::string phi_star = ltl_skeleton<node>(fm, atoms);
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1])
			                         && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
			         || (!std::isalnum((unsigned char)phi_star[end])
			             && phi_star[end] != '_');
			if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
			else pos = end;
		}
	}

	auto bundle = alg_b::build_algorithm_b_skeleton(
		T1_size, T2_size, K, feasible_set_b, t2_pos_m, phi_star);
	LOG_DEBUG << "[ltl_aba:algB] T2=" << T2_size << " T1=" << T1_size
	          << " K=" << K << " n_pbits=" << bundle.n_pbits
	          << " n_rbits=" << bundle.n_rbits;

	auto [realizable, hoa_text] = call_ltlsynt(bundle.formula, bundle.ins, bundle.outs);
	if (!realizable) return std::nullopt;

	LtlAbaSolution<node> sol;
	sol.aut = parse_hoa(hoa_text);
	return sol;
}

template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba(tref fm)
{
	// Past operators (S, T) are handled at skeleton level via DFA temporal
	// testers (ppLTLTT approach), not by AST-level compile-away.  The
	// formula is passed to extract_data_atoms unchanged — S/T nodes are
	// transparent to atom extraction (they are temporal operators, not
	// data atoms).  The skeleton generation intercepts S/T and emits
	// DFA state variables + constraints instead.
	LtlAbaSolution<node> sol;
	sol.atoms = extract_data_atoms<node>(fm);

	// Past operators (S, T) require the ppLTLTT temporal tester encoding
	// in the default path.  Algorithm A/B/D use ltl_skeleton() which
	// passes S/T through as literal operators — those paths don't have
	// the DFA state-variable machinery.  Skip them when S/T are present.
	const bool has_past = has_past_operators<node>(fm);

	// Algorithm D gate: synthesis parity game × T_1 product game with Zielonka.
	// Builds the propositional synthesis game for φ*(D_i) (no structural
	// constraints in formula), then solves the data product game directly.
	if (!has_past) {
		const bool alg_d_mode = [] {
			const char* v = std::getenv("TAU_LTL_ALG");
			return v && std::string_view(v) == "D";
		}();
		bool alg_d_has_input = false;
		for (auto& [f, _] : sol.atoms)
			if (atom_has_any_input<node>(f)) { alg_d_has_input = true; break; }
		if (alg_d_mode && !alg_d_has_input
		    && is_algorithm_a_applicable<node>(sol.atoms)) {
			auto constants = omcat::collect_qlt_constants<node>(fm);
			auto T3 = omcat::enumerate_qlt_T3(constants);
			int K = (int)sol.atoms.size();
			int T1_size = 2 * (int)constants.size() + 1;

			// Compute D-bitmask for each T3 type
			std::vector<int> type_A(T3.size(), 0);
			for (int i = 0; i < K; ++i) {
				for (int t = 0; t < (int)T3.size(); ++t) {
					auto h = qlt_atom_holds_in_type3<node>(sol.atoms[i].first, T3[t], constants);
					if (h != false) type_A[t] |= (1 << i);
				}
			}

			// Build φ*(D_i)
			std::string phi_star = ltl_skeleton<node>(fm, sol.atoms);
			for (int i = K; i-- > 0; ) {
				std::string fp = "p" + std::to_string(i);
				std::string td = "d_" + std::to_string(i);
				size_t pos = 0;
				while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
					size_t end = pos + fp.size();
					bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1]) && phi_star[pos-1] != '_');
					bool r_ok = end >= phi_star.size() || (!std::isalnum((unsigned char)phi_star[end]) && phi_star[end] != '_');
					if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
					else pos = end;
				}
			}

			LOG_DEBUG << "[ltl_aba:algD] T3=" << T3.size() << " T1=" << T1_size
			          << " K=" << K << " phi_star=" << phi_star;

			bool realizable = alg_d::solve_algorithm_d(phi_star, T1_size, T3, type_A, K);
			LOG_DEBUG << "[ltl_aba:algD] result=" << (realizable ? "REALIZABLE" : "UNREALIZABLE");

			if (!realizable) return std::nullopt;

			// Realizable: call ltlsynt for the strategy automaton.
			// Use the simplified propositional formula; data execution may need
			// the full structural-constraint formula for perfect correctness but
			// this gives a usable strategy for most test cases.
			std::vector<std::string> D_outs;
			for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));
			auto [real2, hoa_text] = call_ltlsynt(phi_star, {}, D_outs);
			if (!real2) {
				// Propositional call disagrees — fall through to default path
				LOG_DEBUG << "[ltl_aba:algD] ltlsynt disagreed; falling through";
			} else {
				sol.aut = parse_hoa(hoa_text);
				return sol;
			}
		} else if (alg_d_mode) {
			LOG_DEBUG << "[ltl_aba:algD] not applicable (input variables, non-qlt, or large lookback);"
			             " falling through to default";
		}
	}

	// Algorithm A/B gate.
	// Algorithm B (P_σ binary encoding) is needed for soundness when any qlt atom
	// has an input io_var (e.g. o1=i1, o1>i1[t-1]).  Without P_σ the system cannot
	// observe the input's T₁ type and may fail to realise the strategy.
	// For pure-output qlt atoms (no input vars), Algorithm A is both sound and
	// produces a far smaller formula (T₁-only, not T₂×T₁×2^K constraints).
	if (!has_past) {
		const char* alg_env = std::getenv("TAU_LTL_ALG");
		const bool alg_b_mode = !alg_env || std::string_view(alg_env) == "B";
		const bool alg_a_mode = alg_env && std::string_view(alg_env) == "A";
		if (is_algorithm_a_applicable<node>(sol.atoms)) {
			// Check whether any atom has an input variable.
			bool any_input = false;
			for (auto& [f, _] : sol.atoms)
				if (atom_has_any_input<node>(f)) { any_input = true; break; }

			// Algorithm A's T_3 encoding only handles atoms whose
			// truth value is decidable from a T_3 type plus the
			// formula's named rational constants.  Atoms involving
			// the qlt boolean-algebra extremes `{top}:qlt` /
			// `{bot}:qlt` (or any constant whose finite-rational
			// witness is empty) yield qlt_atom_holds_in_type3 ==
			// nullopt for every type, leaving the atom completely
			// unconstrained in the symbolic encoding.  Without this
			// guard, ltlsynt happily synthesises a strategy where
			// `α` and `¬α` both hold simultaneously, returning
			// REALIZABLE for direct contradictions like
			// `F(o1={top}) && G(o1!={top})`.  Falling through to
			// the default add_consistency_constraints + ABA-oracle
			// path catches these correctly.
			bool alg_a_can_classify = true;
			{
				auto a_constants = omcat::collect_qlt_constants<node>(fm);
				auto a_T3        = omcat::enumerate_qlt_T3(a_constants);
				if (a_T3.empty())
					alg_a_can_classify = false;
				else for (auto& [f, _] : sol.atoms) {
					bool any_determined = false;
					for (auto& t : a_T3) {
						auto h = qlt_atom_holds_in_type3<node>(
						    f, t, a_constants);
						if (h.has_value()) {
							any_determined = true;
							break;
						}
					}
					if (!any_determined) {
						alg_a_can_classify = false;
						LOG_DEBUG << "[ltl_aba] atom outside T_3 "
						             "(top/bot qlt constant?) — "
						             "skipping Algorithm A";
						break;
					}
				}
			}

			// Algorithm A's T_3 encoding has a SINGLE current-output slot
			// (Y) and a SINGLE past-output slot (M).  Two distinct output
			// variables (o1, o2, …) get conflated into the same slot,
			// making every multi-output atom collapse to a single rational
			// witness.  Concretely, `o1[t]>0 && o2[t]<0` becomes "Y>0 &&
			// Y<0" — unsatisfiable in any T_3 type — so Algorithm A
			// returns spurious UNREALIZABLE.  Multi-output specs must
			// fall through to the default ABA-oracle path which builds
			// disjoint per-variable constraints.
			//
			// (Multi-input is fine: t3_role_of merges i_k → X but those
			// flow through Algorithm B's P_σ encoding which is
			// distinguisher-friendly.  The conflation is harmful only on
			// the OUTPUT side.)
			std::set<std::string> distinct_output_names;
			for (auto& [f, _] : sol.atoms) {
				const auto& t = tree<node>::get(f);
				if (!t.has_child()) continue;
				auto add_side = [&](tref side) {
					if (!side) return;
					tref iv = tree<node>::get(side).find_top([](tref n) {
						return is_child<node>(n, tree<node>::io_var); });
					if (!iv) return;
					const std::string& nm = get_var_name<node>(iv);
					if (!nm.empty() && nm[0] == 'o')
						distinct_output_names.insert(nm);
				};
				add_side(t[0].first());
				add_side(t[0].second());
			}
			if (distinct_output_names.size() > 1) {
				alg_a_can_classify = false;
				LOG_DEBUG << "[ltl_aba] multiple output vars ("
				          << distinct_output_names.size()
				          << ") — Algorithm A's single-Y/M slot would "
				             "conflate them; falling through to default "
				             "ABA-oracle path";
			}

			if (!any_input && alg_a_can_classify) {
				// Pure-output: Algorithm A is sound and fast.
				LOG_DEBUG << "[ltl_aba] using Algorithm A (pure-output)";
				return solve_ltl_aba_algorithm_a<node>(fm, sol.atoms);
			}
			if (alg_a_mode)
				LOG_DEBUG << "[ltl_aba] TAU_LTL_ALG=A ignored because input variables are present";
			// Algorithm B is only sound when the same T_3-classification
			// holds: it shares the symbolic atom-mask with Algorithm A.
			// Atoms that don't classify (top/bot qlt constants etc.)
			// must fall through to the default add_consistency_constraints
			// path, which uses the ABA oracle directly and catches the
			// pairwise-infeasibility constraints those atoms induce.
			if (alg_b_mode && alg_a_can_classify) {
				// Fast-path: constant-output strategy check. If the system
				// can pick fixed output values that reduce the formula to a
				// tautology over remaining (input) atoms, REALIZABLE.
				// Catches trivially-satisfiable U/W/R right-sides that
				// Algorithm B's large P_σ-encoded formula would make
				// ltlsynt time out on.
				if (constant_output_realizable<node>(fm, sol.atoms)) {
					LtlAbaSolution<node> trivial;
					trivial.atoms = sol.atoms;
					// num_states = 0 signals trivially realizable.
					return trivial;
				}
				// Has input vars: Algorithm B required for soundness.
				LOG_DEBUG << "[ltl_aba] using Algorithm B (P_σ binary encoding)";
				return solve_ltl_aba_algorithm_b<node>(fm, sol.atoms);
			}
			if (!alg_a_can_classify)
				LOG_DEBUG << "[ltl_aba] T_3 cannot classify atoms — "
				             "falling through to default ABA-oracle path";
		} else if (alg_b_mode) {
			LOG_DEBUG << "[ltl_aba] Alg B not applicable; using default path";
		}
	} // if (!has_past)

	if (sol.atoms.empty()) {
		// Purely propositional: no io_var atoms.
		if (has_past) {
			std::vector<PastTemporalTester> testers;
			sol.skeleton = skeleton_str_with_testers<node>(fm, sol.atoms, testers);
			append_tester_constraints(sol.skeleton, testers);
			std::vector<std::string> out_props;
			for (const auto& t : testers)
				out_props.push_back(t.state_var);
			auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, out_props);
			if (!real) return std::nullopt;
			sol.aut = parse_hoa(hoa);
			return sol;
		}
		sol.skeleton = ltl_skeleton<node>(fm, sol.atoms);
		auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, {});
		if (!real) return std::nullopt;
		sol.aut = parse_hoa(hoa);
		return sol;
	}

	for (auto& [f, name] : sol.atoms) {
		LOG_DEBUG << "[ltl_aba] atom " << name << " = "
		         << tree<node>::get(f).to_str()
		         << " lookback=" << (atom_has_lookback<node>(f) ? "yes" : "no");
	}

	// Classify atoms as input (only i* vars) or output (has o* var).
	for (auto& [f, name] : sol.atoms) {
		if (is_pure_input_atom<node>(f))
			sol.input_props.push_back(name);
		else
			sol.output_props.push_back(name);
	}

	// Build skeleton with DFA temporal testers for past operators (S, T).
	// The ppLTLTT approach (Azzopardi et al., ATVA'23) replaces each S/T
	// subformula with a fresh propositional state variable and encodes
	// the DFA transition + initial condition as pure future-LTL constraints
	// (G, X, propositional) that ltlsynt handles natively.
	auto [skel, testers] = ltl_skeleton_with_testers<node>(fm, sol.atoms);
	sol.skeleton = std::move(skel);
	add_consistency_constraints<node>(sol.atoms, sol.skeleton,
		nullptr, has_past /*polarity_complete*/);

	// Append DFA tester constraints and register state variables as outputs.
	append_tester_constraints(sol.skeleton, testers);
	for (const auto& t : testers) {
		sol.output_props.push_back(t.state_var);
		LOG_DEBUG << "[ltl_aba] ppLTLTT tester: " << t.state_var
		          << " init=" << t.initial_value
		          << " trans=" << t.transition
		          << " negate=" << t.negate_output;
	}

	LOG_DEBUG << "[ltl_aba] LTL skeleton: " << sol.skeleton;
	LOG_DEBUG << "[ltl_aba] inputs:  " << [&]{
		std::string s; for (auto& p : sol.input_props)  s += p + " "; return s; }();
	LOG_DEBUG << "[ltl_aba] outputs: " << [&]{
		std::string s; for (auto& p : sol.output_props) s += p + " "; return s; }();

	auto [realizable, hoa_text] =
	    call_ltlsynt(sol.skeleton, sol.input_props, sol.output_props);
	if (!realizable) return std::nullopt;

	sol.aut = parse_hoa(hoa_text);
	return sol;
}

// ── is_ltl_aba_realizable ─────────────────────────────────────────────────────

template <NodeType node>
bool is_ltl_aba_realizable(tref fm, int_t start_time, bool output) {
	LOG_DEBUG << "[ltl_aba] is_ltl_aba_realizable: " << LOG_FM(fm);

	// Safety fast-path: if the formula has no full-LTL operators AND
	// no Boolean combinations of models, it is a pure G/safety formula
	// the safety pipeline can decide on its own.  In that case the
	// entire six-phase LTL pipeline (data-atom extraction →
	// propositional skeleton → ltlsynt subprocess → HOA parse → ABA
	// oracle on every transition) reduces to a single
	// is_tau_formula_sat call.  Skipping ltlsynt (5–50 ms per call) on
	// pure safety specs is the largest single per-call win on the
	// synthesis hot path.
	//
	// The Boolean-combs-of-models guard is what prevents an infinite
	// recursion with is_tau_formula_sat: that function routes formulas
	// like `(G A) || (G B)` here precisely because it can't handle
	// them itself.  Without the guard, this fast-path would route back.
	if (!has_ltl_operators<node>(fm)
	    && has_no_boolean_combs_of_models<node>(fm)) {
		LOG_DEBUG << "[ltl_aba] safety fast-path "
		             "(no full-LTL operators, single G)";
		return is_tau_formula_sat<node>(fm, start_time, output);
	}

	auto maybe = solve_ltl_aba<node>(fm);
	LOG_DEBUG << "[ltl_aba] solve_ltl_aba returned: " << maybe.has_value();

	if (!maybe) {
		if (output) LOG_DEBUG << "[ltl_aba] UNREALIZABLE (propositional)";
		return false;
	}

	auto& sol = *maybe;
	LOG_DEBUG << "[ltl_aba] atoms=" << sol.atoms.size()
	          << " states=" << sol.aut.num_states;

	// Trivially realizable: no states produced.
	if (sol.aut.num_states == 0) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
		return true;
	}

	// Purely propositional (no data atoms) — ltlsynt verdict is final.
	if (sol.atoms.empty()) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE (propositional)";
		return true;
	}

	LOG_DEBUG << "[ltl_aba] strategy has " << sol.aut.num_states << " state(s)";

	// ABA oracle: verify each strategy transition guard is ABA-feasible.
	// ABA inconsistency between proposition pairs has already been handled
	// by the consistency constraints added to the skeleton (see solve_ltl_aba).
	// Here we only check individual guards via existential satisfiability.
	for (int s = 0; s < sol.aut.num_states; ++s) {
		for (auto& e : sol.aut.edges[s]) {
			LOG_DEBUG << "[ltl_aba] checking edge " << s << "->[" << e.guard_label << "]->" << e.dst;
			if (!guard_is_aba_feasible<node>(
			        e.guard_label, sol.aut.aps, sol.atoms)) {
				LOG_DEBUG << "[ltl_aba] ABA infeasible edge "
				          << s << "→" << e.dst
				          << " guard=[" << e.guard_label << "]";
				if (output)
					LOG_INFO << "[ltl_aba] UNREALIZABLE "
					            "(propositionally realizable but ABA-infeasible)";
				return false;
			}
		}
	}

	if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
	return true;
}

// ── Multi-state Mealy → safety formula ───────────────────────────────────────
//
// For a k-state Mealy machine, encodes the strategy as
// an always(phi) formula with lookback 1.
//
// Approach: introduce k auxiliary output bitvector variables o__ltl_s0__,
// o__ltl_s1__, ..., o__ltl_s{k-1}__ representing the automaton state in a
// one-hot encoding.  The always-formula encodes:
//   (a) one-hot constraint: exactly one state bit is true at every step,
//   (b) transition relation: if si[t-1]=1 and the edge guard holds, then
//       s_{dst}[t]=1 and the data atoms satisfy the guard's output conditions.
//
// The synthesis chooses the initial state bits si[-1] freely; any valid
// initialization satisfies the formula (since the strategy is realizable).

template <NodeType node>
static tref parse_sv_eq(const std::string& name, int shift, int value)
{
	using tau = tree<node>;
	std::string t_str = (shift == 0) ? "t" : ("t-" + std::to_string(-shift));
	// Use the default BV bitwidth explicitly — bare ":bv" is rejected by
	// the grammar since the merge that made bitwidths mandatory.
	std::string bv_type_str = ":bv[" + std::to_string(default_bv_size) + "]";
	std::string expr = name + "[" + t_str + "]" + bv_type_str + " = { "
	                 + std::to_string(value) + " }";
	// Use wff start symbol; keep all type inference defaults enabled so that
	// bitvector constants ({0}, {1}) are properly resolved.
	typename tau::get_options opts;
	opts.parse.start = tau::wff;
	return tau::get(expr, std::move(opts));
}

template <NodeType node>
static tref encode_mealy_as_safety(const LtlAbaSolution<node>& sol)
{
	using tau = tree<node>;
	const auto& aut = sol.aut;
	int k = aut.num_states;

	// Auxiliary output state variable names. Use "ms" (Mealy-state) prefix to
	// avoid collision with compile-away S-operator variables (o__ltl_sN__).
	std::vector<std::string> sv;
	for (int i = 0; i < k; ++i)
		sv.push_back("o__ltl_ms" + std::to_string(i) + "__");

	// ── (a) One-hot constraint at the current step ────────────────────────
	tref at_least = tau::_F();
	for (int i = 0; i < k; ++i)
		at_least = tau::build_wff_or(at_least, parse_sv_eq<node>(sv[i], 0, 1));

	tref at_most = tau::_T();
	for (int i = 0; i < k; ++i)
		for (int j = i + 1; j < k; ++j)
			at_most = tau::build_wff_and(at_most,
			    tau::build_wff_neg(
			        tau::build_wff_and(parse_sv_eq<node>(sv[i], 0, 1),
			                          parse_sv_eq<node>(sv[j], 0, 1))));

	tref one_hot = tau::build_wff_and(at_least, at_most);

	// ── (b) Transition rules with lookback-1 ──────────────────────────────
	// For each source state s:
	//   si[t-1]=1  →  ∨_edges_from_s (guard_formula ∧ s_{dst}[t]=1)
	tref trans = tau::_T();
	for (int s = 0; s < k; ++s) {
		tref prev_s = parse_sv_eq<node>(sv[s], -1, 1);
		tref edges_disj = tau::_F();
		for (const auto& e : aut.edges[s]) {
			if (e.dst < 0 || e.dst >= k) {
				LOG_ERROR << "[ltl_aba] HOA edge dst " << e.dst
				          << " out of range [0," << k << "), skipping";
				continue;
			}
			tref guard_fm = guard_to_aba<node>(
			    e.guard_label, aut.aps, sol.atoms);
			tref next_d = parse_sv_eq<node>(sv[e.dst], 0, 1);
			edges_disj = tau::build_wff_or(edges_disj,
			    tau::build_wff_and(guard_fm, next_d));
		}
		if (!aut.edges[s].empty()) {
			tref rule = tau::build_wff_or(
			    tau::build_wff_neg(prev_s), edges_disj);
			trans = tau::build_wff_and(trans, rule);
		}
	}

	tref body = normalize_non_temp<node>(tau::build_wff_and(one_hot, trans));
	LOG_DEBUG << "[ltl_aba] multi-state safety body: " << LOG_FM(body);
	return tau::build_wff_always(body);
}

// Build the fixed-time initial-state and initial-output conditions for a
// multi-state Mealy encoding.  These are returned separately so the caller
// can combine them with the safety formula AFTER normalisation (normalising
// them together can cause the solver to report unsatisfiability at step 0).
//
// Returned pair: { sv[initial_state][0]={1},  init_out_disjunction }
// init_out_disjunction = ∨_e ( guard_e(t=0) ∧ sv[dst_e][1]={1} )
// If the initial state has no outgoing edges (shouldn't happen for a
// realizable formula), the second element is nullptr.
template <NodeType node>
static std::pair<tref,tref>
encode_mealy_initial_conditions(const LtlAbaSolution<node>& sol,
                                const std::vector<std::string>& sv)
{
	using tau = tree<node>;
	const auto& aut = sol.aut;
	const int k     = (int)sv.size();
	const int init_s = aut.initial_state;
	if (init_s < 0 || init_s >= k) return {nullptr, nullptr};

	// (1) sv[initial_state][0] = {1}
	tref sv_tmpl = parse_sv_eq<node>(sv[init_s], 0, 1);
	auto sv_io   = tau::get(sv_tmpl).select_top(is_child<node, tau::io_var>);
	tref init_sv = fm_at_time_point<node>(sv_tmpl, sv_io, 0);

	// (2) ∨_e ( guard_e(t=0) ∧ sv[dst_e][1]={1} )
	tref init_out = nullptr;
	for (const auto& e : aut.edges[init_s]) {
		if (e.dst < 0 || e.dst >= k) continue;
		tref gfm   = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		auto gvars = tau::get(gfm).select_top(is_child<node, tau::io_var>);
		tref g0    = fm_at_time_point<node>(gfm, gvars, 0);
		tref sv_t  = parse_sv_eq<node>(sv[e.dst], 0, 1);
		auto sv_t_io = tau::get(sv_t).select_top(is_child<node, tau::io_var>);
		tref sv1   = fm_at_time_point<node>(sv_t, sv_t_io, 1);
		tref edge  = tau::build_wff_and(g0, sv1);
		init_out   = init_out ? tau::build_wff_or(init_out, edge) : edge;
	}

	return {init_sv, init_out};
}

// ── ltl_to_safety_formula ─────────────────────────────────────────────────────
//
// `_full` does the work and returns BOTH the safety formula AND the
// LtlAbaSolution (when one was synthesised). The interpreter caches the
// solution so it can introspect the Mealy state at runtime, visualise the
// strategy, etc. — info that would otherwise be discarded after encoding.
//
// The thin wrapper `ltl_to_safety_formula(fm)` discards the solution to
// preserve the existing single-return API for callers that don't need it.

template <NodeType node>
std::tuple<tref, std::optional<LtlAbaSolution<node>>>
ltl_to_safety_formula_full(tref fm) {
	using tau = tree<node>;
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: " << LOG_FM(fm);

	// Fast path: if all LTL operators are past (S/T), compile them away and
	// return G(curr && rhs) safety invariants for each S operator.
	// No Mealy synthesis is needed on this path; the returned solution is empty.
	{
		auto [compiled_fast, safety_fm, init_fm, _aux] = compile_since_trigger<node>(fm);
		if (!has_ltl_operators<node>(compiled_fast)) {
			LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: "
			          << "pure past-LTL, returning safety formula";
			return {tau::build_wff_and(safety_fm, init_fm), std::nullopt};
		}
	}

	auto maybe = solve_ltl_aba<node>(fm);
	if (!maybe) {
		LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: not realizable";
		return {nullptr, std::nullopt};
	}

	auto& sol = *maybe;

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty()) return {tau::_T(), std::move(sol)};

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0) return {tau::_T(), std::move(sol)};

	if (aut.num_states > 1) {
		LOG_INFO << "[ltl_aba] Multi-state strategy ("
		         << aut.num_states
		         << " states) — encoding with auxiliary one-hot state bits";
		tref encoded = encode_mealy_as_safety<node>(sol);
		return {encoded, std::move(sol)};
	}

	// Single-state strategy: the self-loop guard is the perpetual output constraint.
	if (aut.edges.empty() || aut.edges[0].empty())
		return {tau::build_wff_always(tau::_T()), std::move(sol)};

	// Build the disjunction of ABA guard formulas over all edges from state 0.
	tref combined = tau::_F();
	for (const auto& e : aut.edges[0]) {
		tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		tref norm_guard = normalize_non_temp<node>(guard_fm);
		combined = tau::build_wff_or(combined, norm_guard);
	}
	tref simplified = normalize_non_temp<node>(combined);
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula result: always("
	          << LOG_FM(simplified) << ")";
	return {tau::build_wff_always(simplified), std::move(sol)};
}

template <NodeType node>
tref ltl_to_safety_formula(tref fm) {
	auto [safety, _sol] = ltl_to_safety_formula_full<node>(fm);
	return safety;
}

// ── ltl_explain ───────────────────────────────────────────────────────────────

template <NodeType node>
bool ltl_explain(tref fm, std::ostream& out) {
	using tau = tree<node>;

	if (!has_ltl_operators<node>(fm)) {
		out << "Formula has no LTL operators (treated as G(phi))\n";
		// Fall through to the existing safety pipeline.
		bool sat = is_tau_formula_sat<node>(fm, 0, false);
		out << (sat ? "REALIZABLE" : "UNREALIZABLE") << "\n";
		return sat;
	}

	// ── Data atoms (S/T handled at skeleton level via ppLTLTT testers) ───
	auto atoms = extract_data_atoms<node>(fm);
	out << "\nData atoms (" << atoms.size() << "):\n";
	for (auto& [f, name] : atoms)
		out << "  " << name << "  :=  " << tau::get(f).to_str() << "\n";

	// ── Input / output classification ─────────────────────────────────────
	std::vector<std::string> input_props, output_props;
	for (auto& [f, name] : atoms) {
		if (is_pure_input_atom<node>(f))
			input_props.push_back(name);
		else
			output_props.push_back(name);
	}
	out << "\nInput propositions:  ";
	for (size_t i = 0; i < input_props.size(); ++i) {
		if (i) out << ", ";
		out << input_props[i];
	}
	if (input_props.empty()) out << "(none)";
	out << "\n";

	out << "Output propositions: ";
	for (size_t i = 0; i < output_props.size(); ++i) {
		if (i) out << ", ";
		out << output_props[i];
	}
	if (output_props.empty()) out << "(none)";
	out << "\n";

	// ── Skeleton with DFA temporal testers (ppLTLTT approach) ────────────
	auto [skeleton, testers] = ltl_skeleton_with_testers<node>(fm, atoms);
	std::vector<std::string> consistency_constraints;
	add_consistency_constraints<node>(atoms, skeleton, &consistency_constraints,
		has_past_operators<node>(fm) /*polarity_complete*/);
	append_tester_constraints(skeleton, testers);
	for (const auto& t : testers)
		output_props.push_back(t.state_var);

	out << "\nABA consistency constraints added (" << consistency_constraints.size() << "):\n";
	for (auto& c : consistency_constraints)
		out << "  " << c << "\n";
	if (consistency_constraints.empty())
		out << "  (none)\n";

	if (!testers.empty()) {
		out << "\nppLTLTT temporal testers (" << testers.size() << "):\n";
		for (const auto& t : testers)
			out << "  " << t.state_var << ": G(X(" << t.state_var
			    << ") <-> " << t.transition << ")"
			    << (t.negate_output ? " [negated output]" : "") << "\n";
	}

	// ── LTL skeleton ─────────────────────────────────────────────────────
	out << "\nLTL skeleton: " << skeleton << "\n";

	// ── Call ltlsynt ─────────────────────────────────────────────────────
	out << "\nCalling ltlsynt...\n";
	auto [realizable, hoa_text] = call_ltlsynt(skeleton, input_props, output_props);

	if (!realizable) {
		out << "\nUNREALIZABLE\n";
		return false;
	}

	// ── Parse HOA strategy ───────────────────────────────────────────────
	HoaAutomaton aut = parse_hoa(hoa_text);

	out << "\nStrategy: " << aut.num_states << " state(s), initial state "
	    << aut.initial_state << "\n";
	out << "Atomic propositions: ";
	for (size_t i = 0; i < aut.aps.size(); ++i) {
		if (i) out << ", ";
		out << aut.aps[i] << " (AP" << i << ")";
	}
	if (aut.aps.empty()) out << "(none)";
	out << "\n";

	for (int s = 0; s < aut.num_states; ++s) {
		out << "  state " << s;
		if (aut.state_accepting[s]) out << " [accepting]";
		out << ":\n";
		for (auto& e : aut.edges[s]) {
			out << "    --[" << e.guard_label << "]--> " << e.dst;
			if (e.accepting) out << " [accepting]";
			out << "\n";
		}
		if (aut.edges[s].empty())
			out << "    (no outgoing edges)\n";
	}

	// ── ABA oracle checks ────────────────────────────────────────────────
	if (!atoms.empty()) {
		out << "\nABA oracle checks:\n";
		bool all_feasible = true;
		for (int s = 0; s < aut.num_states; ++s) {
			for (auto& e : aut.edges[s]) {
				tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, atoms);
				bool feasible = aba_existential_feasible<node>(guard_fm);
				out << "  state " << s << " --[" << e.guard_label
				    << "]--> " << e.dst << " : ";
				if (feasible) {
					out << "feasible\n";
				} else {
					out << "INFEASIBLE\n";
					out << "    (formula: " << tau::get(guard_fm).to_str() << ")\n";
					all_feasible = false;
				}
			}
		}
		if (!all_feasible) {
			out << "\nUNREALIZABLE (ABA-infeasible transition)\n";
			return false;
		}
	}

	// ── Safety formula ───────────────────────────────────────────────────
	// Re-derive via solve path (atoms already computed above, just encode)
	tref safety = ltl_to_safety_formula<node>(fm);
	if (safety) {
		out << "\nSafety formula: " << tau::get(safety).to_str() << "\n";
	} else {
		out << "\nSafety formula: (not encodable as simple always(phi))\n";
	}

	out << "\nREALIZABLE\n";
	return true;
}

// ── CTL* operators detection ─────────────────────────────────────────────────

template <NodeType node>
bool has_ctl_star_operators(tref fm) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tau::wff_A || nt == tau::wff_E
		    || nt == tau::wff_semantic_neg;
	}) != nullptr;
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// ── CTL* → LTL reduction ────────────────────────────────────────────────────
//
// Implements the Bloem/Schewe/Khalimov reduction (arXiv:1711.10636).
//
// Algorithm:
//   1. Bottom-up traversal of the CTL* formula tree
//   2. For each E χ subformula:
//      - Create a fresh witness output variable w_i
//      - Replace E χ with w_i
//      - Add constraint G(w_i → translate_path(χ))
//   3. For each A χ subformula:
//      - Rewrite to ¬(E ¬χ) and apply step 2
//   4. The final LTL formula is: translated_root ∧ ⋀_i G(w_i → χ_i_LTL)

namespace ctl_star_detail {

// Counter for generating unique witness variable names
static int witness_counter = 0;

static std::string fresh_witness_name() {
	return "w_" + std::to_string(witness_counter++);
}

// Reset counter for each new reduction
static void reset_witness_counter() {
	witness_counter = 0;
}

} // namespace ctl_star_detail

// Recursive bottom-up translation of a CTL* state/path formula to LTL.
// Witness constraints are accumulated in `constraints` (each is a G(w → χ) pair).
// New witness output names are accumulated in `witnesses`.
template <NodeType node>
static tref translate_ctl_star(tref fm,
		std::vector<std::pair<std::string, tref>>& constraints,
		std::vector<std::string>& witnesses) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return fm;

	auto nt = t[0].value.nt;

	// Handle E χ: introduce witness output
	if (nt == tau::wff_E) {
		tref inner = t[0].child(0);
		// Recursively translate the inner path formula
		tref translated_inner = translate_ctl_star<node>(
			inner, constraints, witnesses);
		// Create fresh witness variable
		std::string wname = ctl_star_detail::fresh_witness_name();
		witnesses.push_back(wname);
		// Build witness as a wff: (o_w_i[t] = 1) serves as the
		// propositional witness for the E-subformula.
		// We use the default bv type for the witness output.
		size_t bv_tid = get_ba_type_id<node>(bv_type<node>());
		tref w_bf = build_out_var_at_t<node>(
			build_var_name<node>(wname), bv_tid, "t");
		tref bf_one = build_bf_t_type<node>(bv_tid);
		tref witness_wff = tau::build_bf_eq(w_bf, bf_one);
		// Add constraint: G(witness → translated_path)
		tref implication = tau::build_wff_imply(witness_wff, translated_inner);
		tref always_constraint = tau::build_wff_always(implication);
		constraints.emplace_back(wname, always_constraint);
		return witness_wff;
	}

	// Handle A χ: rewrite as ¬(E ¬χ)
	if (nt == tau::wff_A) {
		tref inner = t[0].child(0);
		// A χ ≡ ¬E¬χ
		tref negated_inner = tau::build_wff_neg(inner);
		// Build E(¬χ) and translate it
		tref e_neg = tau::build_wff_E(negated_inner);
		tref translated_e = translate_ctl_star<node>(
			e_neg, constraints, witnesses);
		// Return ¬(translated E(¬χ))
		return tau::build_wff_neg(translated_e);
	}

	// Handle semantic negation -φ: unrealizability check
	// -φ means "φ is not realizable by the system"
	// Equivalent to: the environment can force ¬φ
	// We encode as: swap inputs/outputs in the inner formula
	if (nt == tau::wff_semantic_neg) {
		tref inner = t[0].child(0);
		return apply_semantic_negation<node>(inner);
	}

	// For all other nodes, recursively translate children
	// Reconstruct the node with translated children
	auto& op = t[0];
	size_t nch = op.children_size();
	if (nch == 0) return fm;

	// Check if any child has CTL* operators
	bool has_ctl = false;
	for (size_t i = 0; i < nch; ++i) {
		if (has_ctl_star_operators<node>(op.child(i))) {
			has_ctl = true;
			break;
		}
	}
	if (!has_ctl) return fm;

	// Translate children and rebuild
	std::vector<tref> new_children;
	new_children.reserve(nch);
	for (size_t i = 0; i < nch; ++i) {
		new_children.push_back(
			translate_ctl_star<node>(op.child(i), constraints, witnesses));
	}

	// Rebuild node with same operator but new children
	if (nch == 1) {
		// Unary operators: neg, sometimes, always, F
		switch (nt) {
		case tau::wff_neg:      return tau::build_wff_neg(new_children[0]);
		case tau::wff_sometimes:return tau::build_wff_sometimes(new_children[0]);
		case tau::wff_always:   return tau::build_wff_always(new_children[0]);
		case tau::wff_F:        return tau::build_wff_F(new_children[0]);
		default:                return fm;
		}
	} else if (nch == 2) {
		// Binary operators
		switch (nt) {
		case tau::wff_and:   return tau::build_wff_and(new_children[0], new_children[1]);
		case tau::wff_or:    return tau::build_wff_or(new_children[0], new_children[1]);
		case tau::wff_imply: return tau::build_wff_imply(new_children[0], new_children[1]);
		case tau::wff_equiv: return tau::build_wff_equiv(new_children[0], new_children[1]);
		case tau::wff_xor:   return tau::build_wff_xor(new_children[0], new_children[1]);
		case tau::wff_U:     return tau::build_wff_U(new_children[0], new_children[1]);
		case tau::wff_R:     return tau::build_wff_R(new_children[0], new_children[1]);
		case tau::wff_W:     return tau::build_wff_W(new_children[0], new_children[1]);
		case tau::wff_S:     return tau::build_wff_S(new_children[0], new_children[1]);
		case tau::wff_T:     return tau::build_wff_T(new_children[0], new_children[1]);
		default:             return fm;
		}
	} else if (nch == 3 && nt == tau::wff_conditional) {
		return tau::build_wff_conditional(
			new_children[0], new_children[1], new_children[2]);
	}
	return fm;
}

template <NodeType node>
CtlStarReduction<node> reduce_ctl_star_to_ltl(tref fm) {
	using tau = tree<node>;
	ctl_star_detail::reset_witness_counter();

	std::vector<std::pair<std::string, tref>> constraints;
	std::vector<std::string> witnesses;

	tref translated = translate_ctl_star<node>(fm, constraints, witnesses);

	// Build the conjunction: translated_root ∧ constraint_1 ∧ ... ∧ constraint_n
	tref result = translated;
	for (auto& [name, constraint] : constraints) {
		result = tau::build_wff_and(result, constraint);
	}

	return CtlStarReduction<node>{result, witnesses};
}

// ── Semantic negation implementation ─────────────────────────────────────────
//
// -φ means "there is no winning system strategy satisfying φ"
// Equivalent to: the environment wins for ¬φ
// In synthesis terms: swap input/output roles and synthesize ¬φ
//
// For the AST level, semantic negation rewrites the formula to prepare it for
// synthesis with swapped roles. The actual role swap happens at the synthesis
// call site.

template <NodeType node>
tref apply_semantic_negation(tref fm) {
	using tau = tree<node>;
	// Semantic negation at the AST level is represented as wff_semantic_neg.
	// The actual player-role swap (input ↔ output) is performed at the
	// synthesis layer when the satisfiability / realizability check is
	// invoked. Here we simply wrap the formula in the semantic negation
	// node so downstream passes can detect and handle it.
	return tau::build_wff_semantic_neg(fm);
}

} // namespace idni::tau_lang
