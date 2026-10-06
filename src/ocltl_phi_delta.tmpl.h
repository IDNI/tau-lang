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

// Return the size of the node table of phi_delta's own BDD instantiation.
inline size_t node_table_size() {
	return bdd<Bool, ocltl_phi_delta_bdd_options>::V.size();
}

} // namespace ocltl_phi_delta_detail

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
