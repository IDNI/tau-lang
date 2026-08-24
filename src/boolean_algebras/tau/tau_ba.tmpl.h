// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/tau/tau_ba.h"

#include <mutex>
#include <unordered_map>

#include "tau_spec.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_ba"

namespace idni::tau_lang {

namespace detail {

// Process-lifetime memo for nested Tau-SAT decisions on immutable tau
// constants: is_zero/is_one/normalize_tau each run a full nested Tau-SAT
// decision on the constant's `nso_rr`, and trees are hash-consed so caching
// by rr<node> identity turns a repeat decision into a lookup. Tau formulas
// are immutable, so no invalidation is needed; guarded by a mutex since
// concurrent solve/interpreter use isn't ruled out.
template <typename node>
struct tau_decision_cache {
	static std::mutex& mtx() {
		static std::mutex m;
		return m;
	}
	static std::unordered_map<rr<node>, bool>& is_zero_memo() {
		static std::unordered_map<rr<node>, bool> m;
		return m;
	}
	static std::unordered_map<rr<node>, bool>& is_one_memo() {
		static std::unordered_map<rr<node>, bool> m;
		return m;
	}
	static std::unordered_map<rr<node>, rr<node>>& normalize_memo() {
		static std::unordered_map<rr<node>, rr<node>> m;
		return m;
	}
	static std::unordered_map<rr<node>, tref>& splitter_normalize_memo() {
		static std::unordered_map<rr<node>, tref> m;
		return m;
	}
};

} // namespace detail

template <typename... BAs>
requires BAsPack<BAs...>
static tref normalized_tau_ba_main(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	return normalize_temporal_quantifiers<node, false>(fm.nso_rr.main->get());
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, htref main)
		: nso_rr({ rec_relations, main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(const rewriter::rules& rec_relations, tref main)
		: nso_rr({ rec_relations, tau::geth(main) }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(htref main) : nso_rr({ main }) {}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba(tref main) : nso_rr({ tau::geth(main) }) {}

template<typename ... BAs> requires BAsPack<BAs...>
tau_ba<BAs...>::tau_ba() : nso_rr() {}

template <typename... BAs>
requires BAsPack<BAs...>
auto tau_ba<BAs...>::operator<=>(const tau_ba<BAs...>&) const = default;

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator~() const {
	// Push the negation in at the end in order to keep normalized forms
	// after double negation of formulas
	auto nmain = tau::geth(
		to_nnf<node>(tau::build_wff_neg(normalized_tau_ba_main(*this))));
	auto nrec_relations = nso_rr.rec_relations;
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator&(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
	auto nmain = tau::geth(tau::build_wff_and(lhs, rhs));
	auto nrec_relations =
		rewriter::merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator|(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
	auto nmain = tau::geth(tau::build_wff_or(lhs, rhs));
	auto nrec_relations = rewriter::merge(nso_rr.rec_relations,
					      other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator+(const tau_ba<BAs...>& other) const {
	auto lhs = normalized_tau_ba_main(*this);
	auto rhs = normalized_tau_ba_main(other);
	auto nmain = tau::geth(tau::build_wff_xor(lhs, rhs));
	rewriter::rules nrec_relations = rewriter::merge(nso_rr.rec_relations,
						other.nso_rr.rec_relations);
	return tau_ba<BAs...>(nrec_relations, nmain);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_ba<BAs...>::operator^(const tau_ba<BAs...>& other) const {
	return *this + other;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_zero() const {
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::is_zero_memo();
		if (auto it = memo.find(nso_rr); it != memo.end()) return it->second;
	}
	tref normalized = normalizer<node>(nso_rr);
	bool result = !is_tau_formula_sat<node>(normalized);
	std::lock_guard<std::mutex> lock(cache::mtx());
	cache::is_zero_memo().emplace(nso_rr, result);
	return result;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool tau_ba<BAs...>::is_one() const {
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::is_one_memo();
		if (auto it = memo.find(nso_rr); it != memo.end()) return it->second;
	}
	tref normalized = normalizer<node>(nso_rr);
	bool result = is_tau_impl<node>(tau::_T(), normalized);
	std::lock_guard<std::mutex> lock(cache::mtx());
	cache::is_one_memo().emplace(nso_rr, result);
	return result;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	return b ? other.is_one() : other.is_zero();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& lhs, const tau_ba<BAs...>& rhs) {
	return lhs.nso_rr.main == rhs.nso_rr.main &&
		lhs.nso_rr.rec_relations == rhs.nso_rr.rec_relations;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> normalize_tau(const tau_ba<BAs...>& fm) {
	using node = typename tau_ba<BAs...>::node;
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::normalize_memo();
		if (auto it = memo.find(fm.nso_rr); it != memo.end())
			return tau_ba<BAs...>(it->second.rec_relations, it->second.main);
	}
	tref result = nso_rr_apply<node>(fm.nso_rr);
	result = simp_tau_unsat_valid<node>(result);
	tau_ba<BAs...> out(tree<node>::geth(result));
	std::lock_guard<std::mutex> lock(cache::mtx());
	cache::normalize_memo().emplace(fm.nso_rr, out.nso_rr);
	return out;
}

// Memoized normalizer<node>(nso_rr) for the splitter's normalized-formula
// precondition (ba_descriptor<tau_ba<...>>::splitter), called once per
// candidate inside atomless_choose_value's splitter ladder. Deliberately a
// separate cache from normalize_memo: normalizer() only normalizes, unlike
// normalize_tau's own simp_tau_unsat_valid pass, so the two aren't
// interchangeable.
template <typename node>
tref normalize_for_splitter(const rr<node>& nso_rr) {
	using cache = detail::tau_decision_cache<node>;
	{
		std::lock_guard<std::mutex> lock(cache::mtx());
		auto& memo = cache::splitter_normalize_memo();
		if (auto it = memo.find(nso_rr); it != memo.end())
			return it->second;
	}
	tref result = normalizer<node>(nso_rr);
	std::lock_guard<std::mutex> lock(cache::mtx());
	cache::splitter_normalize_memo().emplace(nso_rr, result);
	return result;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_one(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_T();
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_syntactic_zero(const tau_ba<BAs...>& fm) {
	return tree<node<tau_ba<BAs...>, BAs...>>::get(fm.nso_rr.main).equals_F();
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st) {
	tref s = tau_splitter<tau_ba<BAs...>, BAs...>(
		normalizer<node<tau_ba<BAs...>, BAs...>>(fm.nso_rr), st);
	return tau_ba<BAs...>(tree<node<tau_ba<BAs...>, BAs...>>::geth(s));
}

template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one() {
	return tau_ba<BAs...>(tau_bad_splitter<tau_ba<BAs...>, BAs...>());
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_closed(const tau_ba<BAs...>& fm) {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;
	tref simp_fm = nso_rr_apply<node>(fm.nso_rr);
	if (!simp_fm) return false;
	simp_fm = apply_defs_to_spec<node>(simp_fm);
	if (!simp_fm) return false;
	if (tau::get(simp_fm).find_top(is<node, tau::ref>))
		return false;
	const trefs& vars = get_free_vars<node>(simp_fm);
	for (tref v : vars) {
		const tau& t = tau::get(v);
		if (!(t.child_is(tau::io_var)
			|| t.child_is(tau::uconst_name)))
				return false;
	}
	return true;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<tau_ba<BAs...>, BAs...>::constant_with_type>
	parse_tau(const std::string& src)
{
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	// parse source
	tau_spec<node> s;
	std::optional<rr<node>> maybe_nso_rr;
	if (!s.parse(src) || !(maybe_nso_rr = s.get_nso_rr())) {
		// TODO: pass these errors on so api users can handle them
		for (const auto& error : s.errors())
			TAU_LOG_ERROR << "[tau] " << error;
		return {};
	}
	// compute final result
	return typename node::constant_with_type{
		std::variant<tau_ba<BAs...>, BAs...>(
			tau_ba<BAs...>(maybe_nso_rr.value().rec_relations,
				       maybe_nso_rr.value().main)),
		tau_type<node>() };
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs) {
	return print<node<tau_ba<BAs...>, BAs...>>(os, rs.nso_rr);
}

} // namespace idni::tau_lang
