// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <functional>
#include <unordered_map>

namespace idni::tau_lang {

inline ocltl_delta_term_ptr ocltl_term_coordinate(size_t p) {
	return std::make_shared<const ocltl_delta_term>(
		ocltl_delta_term{ ocltl_term_kind::coordinate, p, nullptr, nullptr });
}

inline ocltl_delta_term_ptr ocltl_term_zero() {
	return std::make_shared<const ocltl_delta_term>(
		ocltl_delta_term{ ocltl_term_kind::zero, 0, nullptr, nullptr });
}

inline ocltl_delta_term_ptr ocltl_term_one() {
	return std::make_shared<const ocltl_delta_term>(
		ocltl_delta_term{ ocltl_term_kind::one, 0, nullptr, nullptr });
}

inline ocltl_delta_term_ptr ocltl_term_meet(
	ocltl_delta_term_ptr a, ocltl_delta_term_ptr b)
{
	return std::make_shared<const ocltl_delta_term>(ocltl_delta_term{
		ocltl_term_kind::meet, 0, std::move(a), std::move(b) });
}

inline ocltl_delta_term_ptr ocltl_term_join(
	ocltl_delta_term_ptr a, ocltl_delta_term_ptr b)
{
	return std::make_shared<const ocltl_delta_term>(ocltl_delta_term{
		ocltl_term_kind::join, 0, std::move(a), std::move(b) });
}

inline ocltl_delta_term_ptr ocltl_term_complement(ocltl_delta_term_ptr a) {
	return std::make_shared<const ocltl_delta_term>(ocltl_delta_term{
		ocltl_term_kind::complement, 0, std::move(a), nullptr });
}

inline std::vector<bool> ocltl_term_support(
	const ocltl_delta_term_ptr& t, size_t K)
{
	const size_t n = size_t{1} << K;
	std::unordered_map<const ocltl_delta_term*, std::vector<bool>> memo;
	std::function<const std::vector<bool>&(const ocltl_delta_term_ptr&)> go =
		[&](const ocltl_delta_term_ptr& node) -> const std::vector<bool>& {
		if (auto it = memo.find(node.get()); it != memo.end())
			return it->second;
		std::vector<bool> support(n, false);
		switch (node->kind) {
		case ocltl_term_kind::coordinate:
			for (size_t A = 0; A < n; ++A)
				support[A] = ((A >> node->coordinate) & 1) != 0;
			break;
		case ocltl_term_kind::zero:
			break;
		case ocltl_term_kind::one:
			support.assign(n, true);
			break;
		case ocltl_term_kind::meet: {
			const auto& l = go(node->left);
			const auto& r = go(node->right);
			for (size_t A = 0; A < n; ++A) support[A] = l[A] && r[A];
			break;
		}
		case ocltl_term_kind::join: {
			const auto& l = go(node->left);
			const auto& r = go(node->right);
			for (size_t A = 0; A < n; ++A) support[A] = l[A] || r[A];
			break;
		}
		case ocltl_term_kind::complement: {
			const auto& l = go(node->left);
			for (size_t A = 0; A < n; ++A) support[A] = !l[A];
			break;
		}
		}
		return memo.emplace(node.get(), std::move(support)).first->second;
	};
	return go(t);
}

inline ocltl_delta_atom ocltl_atom_coordinate_eq(size_t p, size_t q) {
	ocltl_delta_term_ptr pt = ocltl_term_coordinate(p);
	ocltl_delta_term_ptr qt = ocltl_term_coordinate(q);
	return { ocltl_term_join(
		ocltl_term_meet(pt, ocltl_term_complement(qt)),
		ocltl_term_meet(ocltl_term_complement(pt), qt)), false };
}

inline ocltl_delta_atom ocltl_atom_coordinate_const(size_t p, bool c) {
	ocltl_delta_term_ptr pt = ocltl_term_coordinate(p);
	return { c ? ocltl_term_complement(pt) : pt, false };
}

inline bool ocltl_phi_delta_direct(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms,
	const std::vector<bool>& sigma, const std::vector<bool>& rho, size_t D)
{
	const size_t K = dims.k();
	const size_t k_sigma = dims.d_m + dims.d_x;
	const size_t sigma_n = size_t{1} << k_sigma;
	const size_t rho_n = size_t{1} << dims.d_y;
	const size_t tau_n = size_t{1} << K;
	DBG(assert(sigma.size() == sigma_n);)
	DBG(assert(rho.size() == rho_n);)

	std::vector<std::vector<bool>> supports(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i)
		supports[i] = ocltl_term_support(atoms[i].term, K);

	// whether D forces atom i's support all-set: D_i itself, flipped for a negated atom.
	auto forces_all_set = [&](size_t i) {
		bool holds = (D >> i) & 1;
		return atoms[i].negate ? !holds : holds;
	};

	// a cell is excluded from the unset region when some atom needs it set.
	std::vector<bool> excluded(tau_n, false);
	for (size_t i = 0; i < atoms.size(); ++i)
		if (forces_all_set(i))
			for (size_t A = 0; A < tau_n; ++A)
				if (supports[i][A]) excluded[A] = true;

	// the maximal allowed unset region: rows/columns whose bit is 0, minus excluded cells.
	std::vector<bool> U(tau_n, false);
	std::vector<bool> row_has_U(sigma_n, false), col_has_U(rho_n, false);
	bool any_U = false;
	for (size_t C = 0; C < rho_n; ++C) {
		if (rho[C]) continue;
		for (size_t B = 0; B < sigma_n; ++B) {
			if (sigma[B]) continue;
			size_t A = B | (C << k_sigma);
			if (excluded[A]) continue;
			U[A] = true;
			any_U = true;
			row_has_U[B] = true;
			col_has_U[C] = true;
		}
	}
	if (!any_U) return false;
	for (size_t B = 0; B < sigma_n; ++B)
		if (!sigma[B] && !row_has_U[B]) return false;
	for (size_t C = 0; C < rho_n; ++C)
		if (!rho[C] && !col_has_U[C]) return false;
	for (size_t i = 0; i < atoms.size(); ++i) {
		if (forces_all_set(i)) continue;
		bool hit = false;
		for (size_t A = 0; A < tau_n; ++A)
			if (supports[i][A] && U[A]) { hit = true; break; }
		if (!hit) return false;
	}
	return true;
}

namespace ocltl_phi_delta_detail {

// Throws once `deadline` has passed; a no-op when there is none.
inline void check_deadline(
	const std::optional<std::chrono::steady_clock::time_point>& deadline)
{
	if (deadline && std::chrono::steady_clock::now() > *deadline)
		throw ocltl_phi_delta_timeout{};
}

// A fixed namespace shared by every build call, so repeated builds of the
// same or smaller shape reuse the same BDD variable ids instead of growing
// the global variable dictionary further.
inline const std::string& phi_delta_namespace() {
	static const std::string ns = "ocltl_phi_delta$";
	return ns;
}

inline ocltl_phi_delta_bdd bit_var(int_t id) {
	return bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(true, id);
}

// The live node count of phi_delta's own BDD instantiation.
inline size_t node_table_size() {
	return bdd<Bool, ocltl_phi_delta_bdd_options>::V.size();
}

// Throws once the node ceiling is already reached, before doing more work.
inline void check_node_ceiling(size_t max_nodes) {
	size_t n = node_table_size();
	if (n > max_nodes)
		throw ocltl_phi_delta_limit_exceeded{ "nodes", max_nodes, n };
}

// Throws when `total_vars` exceeds the variable ceiling.
inline void check_var_ceiling(size_t total_vars, size_t max_vars) {
	if (total_vars > max_vars)
		throw ocltl_phi_delta_limit_exceeded{ "vars", max_vars, total_vars };
}

inline ocltl_phi_delta_bdd iff(const ocltl_phi_delta_bdd& a,
	const ocltl_phi_delta_bdd& b, size_t max_nodes)
{
	check_node_ceiling(max_nodes);
	return ~(a ^ b);
}

// Left-fold AND over `v`; every element here is a distinct fresh tau-bit
// literal, so the result is the same chain regardless of fold order.
inline ocltl_phi_delta_bdd and_fold(
	const std::vector<ocltl_phi_delta_bdd>& v, size_t max_nodes)
{
	ocltl_phi_delta_bdd r = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
	for (const auto& x : v) {
		check_node_ceiling(max_nodes);
		r = r & x;
	}
	return r;
}

} // namespace ocltl_phi_delta_detail

inline ocltl_phi_delta_result ocltl_build_phi_delta(
	const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms,
	std::optional<std::chrono::steady_clock::time_point> deadline,
	ocltl_phi_delta_limits limits,
	std::optional<bool> force_sigma_major)
{
	using namespace ocltl_phi_delta_detail;

	const size_t K = dims.k();
	const size_t k_sigma = dims.d_m + dims.d_x;
	const size_t k_rho = dims.d_y;
	const size_t tau_n = size_t{1} << K;
	const size_t sigma_n = size_t{1} << k_sigma;
	const size_t rho_n = size_t{1} << k_rho;

	check_var_ceiling(tau_n + sigma_n + rho_n + atoms.size(), limits.max_vars);

	const std::string& ns = phi_delta_namespace();
	// the smaller of sigma's/rho's fiber count gets contiguous tau-bit ids.
	const bool sigma_major = force_sigma_major.value_or(k_sigma <= k_rho);

	// tau's mask bits: bit A stands for "minterm A is zero"; its low k_sigma bits are B, its high k_rho bits are C.
	std::vector<int_t> tau_ids(tau_n);
	std::vector<ocltl_phi_delta_bdd> tau_bits(tau_n);
	auto mint_tau = [&](size_t A) {
		check_node_ceiling(limits.max_nodes);
		tau_ids[A] = var_dict(ns + "tau$" + std::to_string(A));
		tau_bits[A] = bit_var(tau_ids[A]);
	};
	if (sigma_major) {
		for (size_t B = 0; B < sigma_n; ++B)
			for (size_t C = 0; C < rho_n; ++C) mint_tau(B | (C << k_sigma));
	} else {
		for (size_t C = 0; C < rho_n; ++C)
			for (size_t B = 0; B < sigma_n; ++B) mint_tau(B | (C << k_sigma));
	}

	std::vector<int_t> sigma_ids(sigma_n);
	for (size_t B = 0; B < sigma_n; ++B)
		sigma_ids[B] = var_dict(ns + "sigma$" + std::to_string(B));
	std::vector<int_t> rho_ids(rho_n);
	for (size_t C = 0; C < rho_n; ++C)
		rho_ids[C] = var_dict(ns + "rho$" + std::to_string(C));

	// sigma bit B's fiber: every A sharing B's low k_sigma bits.
	auto sigma_fiber_bits = [&](size_t B) {
		std::vector<ocltl_phi_delta_bdd> fiber;
		fiber.reserve(rho_n);
		for (size_t C = 0; C < rho_n; ++C)
			fiber.push_back(tau_bits[B | (C << k_sigma)]);
		return fiber;
	};
	// rho bit C's fiber: every A sharing C's high k_rho bits.
	auto rho_fiber_bits = [&](size_t C) {
		std::vector<ocltl_phi_delta_bdd> fiber;
		fiber.reserve(sigma_n);
		for (size_t B = 0; B < sigma_n; ++B)
			fiber.push_back(tau_bits[B | (C << k_sigma)]);
		return fiber;
	};

	ocltl_phi_delta_bdd relation = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;

	// tau is valid iff not every sigma bit is set.
	std::vector<ocltl_phi_delta_bdd> sigma_bit_vars(sigma_n);
	for (size_t B = 0; B < sigma_n; ++B) sigma_bit_vars[B] = bit_var(sigma_ids[B]);
	check_node_ceiling(limits.max_nodes);
	relation = relation & ~and_fold(sigma_bit_vars, limits.max_nodes);

	// build every fiber of the major dimension before touching any tau bit again.
	if (sigma_major) {
		for (size_t B = 0; B < sigma_n; ++B) {
			check_deadline(deadline);
			check_node_ceiling(limits.max_nodes);
			relation = relation & iff(bit_var(sigma_ids[B]),
				and_fold(sigma_fiber_bits(B), limits.max_nodes), limits.max_nodes);
		}
	} else {
		for (size_t C = 0; C < rho_n; ++C) {
			check_deadline(deadline);
			check_node_ceiling(limits.max_nodes);
			relation = relation & iff(bit_var(rho_ids[C]),
				and_fold(rho_fiber_bits(C), limits.max_nodes), limits.max_nodes);
		}
	}

	// D_i is atom i's minterm-support conjunction, negated for "!= 0" atoms.
	std::vector<int_t> delta_ids(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i) {
		check_deadline(deadline);
		const ocltl_delta_atom& atom = atoms[i];
		std::vector<bool> supp = ocltl_term_support(atom.term, K);
		std::vector<ocltl_phi_delta_bdd> support;
		support.reserve(tau_n / 2);
		for (size_t A = 0; A < tau_n; ++A) {
			if (supp[A]) support.push_back(tau_bits[A]);
			if ((A & 0x3ff) == 0) check_deadline(deadline);
		}
		int_t id = var_dict(ns + "delta$" + std::to_string(i));
		delta_ids[i] = id;
		ocltl_phi_delta_bdd eq_zero = and_fold(support, limits.max_nodes);
		check_node_ceiling(limits.max_nodes);
		relation = relation & iff(bit_var(id),
			atom.negate ? ~eq_zero : eq_zero, limits.max_nodes);
	}

	ocltl_phi_delta_stats stats;
	stats.tau_bits = tau_n;
	stats.sigma_bits = sigma_n;
	stats.rho_bits = rho_n;
	stats.delta_bits = atoms.size();
	stats.nodes_before_projection = ocltl_bdd_node_count(relation);

	// build each minor-dimension fiber, then immediately project its tau bits away.
	if (sigma_major) {
		for (size_t C = 0; C < rho_n; ++C) {
			check_deadline(deadline);
			check_node_ceiling(limits.max_nodes);
			relation = relation & iff(bit_var(rho_ids[C]),
				and_fold(rho_fiber_bits(C), limits.max_nodes), limits.max_nodes);
			for (size_t B = 0; B < sigma_n; ++B) {
				check_node_ceiling(limits.max_nodes);
				relation = relation->ex(tau_ids[B | (C << k_sigma)]);
			}
		}
	} else {
		for (size_t B = 0; B < sigma_n; ++B) {
			check_deadline(deadline);
			check_node_ceiling(limits.max_nodes);
			relation = relation & iff(bit_var(sigma_ids[B]),
				and_fold(sigma_fiber_bits(B), limits.max_nodes), limits.max_nodes);
			for (size_t C = 0; C < rho_n; ++C) {
				check_node_ceiling(limits.max_nodes);
				relation = relation->ex(tau_ids[B | (C << k_sigma)]);
			}
		}
	}

	ocltl_phi_delta_result result;
	result.relation = relation;
	result.sigma_vars = std::move(sigma_ids);
	result.rho_vars = std::move(rho_ids);
	result.delta_vars = std::move(delta_ids);
	stats.nodes_after_projection = ocltl_bdd_node_count(relation);
	result.stats = stats;
	return result;
}

inline size_t ocltl_bdd_node_count(const ocltl_phi_delta_bdd& f) {
	std::unordered_set<const void*> seen;
	std::vector<ocltl_phi_delta_bdd> stack{f};
	while (!stack.empty()) {
		ocltl_phi_delta_bdd h = stack.back();
		stack.pop_back();
		if (h->is_zero() || h->is_one()) continue;
		if (!seen.insert(h.get()).second) continue;
		auto node = h->get();
		stack.push_back(bdd_handle<Bool, ocltl_phi_delta_bdd_options>::get(node.h));
		stack.push_back(bdd_handle<Bool, ocltl_phi_delta_bdd_options>::get(node.l));
	}
	return seen.size();
}

} // namespace idni::tau_lang
