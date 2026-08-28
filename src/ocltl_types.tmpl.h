// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_unit() {
	using desc = ba_descriptor<BA, Node>;
	auto t = desc::type_tree();
	auto c = desc::parse(desc::literal_one(t), t);
	DBG(assert(c.has_value() && std::holds_alternative<BA>(c->first));)
	return std::get<BA>(c->first);
}

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_minterm(const std::vector<BA>& a, size_t A) {
	if (a.empty()) { DBG(assert(A == 0);) return ocltl_unit<BA, Node>(); }
	// Some atomless BAs (tau_ba: a const nso_rr member) are constructible but
	// not copy/move-assignable -- accumulate via emplace (destroy+construct
	// in place), never operator=, so this works for every atomless BA alike.
	std::optional<BA> m((A & 1) ? a[0] : ~a[0]);
	for (size_t i = 1; i < a.size(); ++i)
		m.emplace(*m & (((A >> i) & 1) ? a[i] : ~a[i]));
	return *m;
}

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
ocltl_type_mask ocltl_type_of(const std::vector<BA>& a) {
	size_t k = a.size();
	DBG(assert(k <= ocltl_max_k);)
	ocltl_type_mask z = 0;
	for (size_t A = 0; A < ocltl_minterm_count(k); ++A)
		if (ba_descriptor<BA, Node>::is_zero(ocltl_minterm<BA, Node>(a, A)))
			z |= (ocltl_type_mask{1} << A);
	return z;
}

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_witness(const std::vector<BA>& a, ocltl_type_mask tau, splitter_type st) {
	using desc = ba_descriptor<BA, Node>;
	size_t k = a.size();
	DBG(assert(k + 1 <= ocltl_max_k);)
	// See ocltl_minterm above: emplace, never operator=, for tau_ba's sake.
	std::optional<BA> b(a.empty()
		? ~ocltl_unit<BA, Node>() : (a[0] & ~a[0])); // the zero element
	for (size_t A = 0; A < ocltl_minterm_count(k); ++A) {
		BA m = ocltl_minterm<BA, Node>(a, A);
		if (desc::is_zero(m)) continue;
		bool neg_b_zero = ((tau >> A) & 1) != 0;
		bool pos_b_zero = ((tau >> (A | (size_t{1} << k))) & 1) != 0;
		DBG(assert(!(neg_b_zero && pos_b_zero));) // the mask must extend tp(ā)
		if (pos_b_zero && !neg_b_zero) continue;              // b excludes m entirely
		else if (neg_b_zero && !pos_b_zero) b.emplace(*b | m); // b covers m entirely
		else b.emplace(*b | desc::splitter(m, st));            // atomless proper part
	}
	return *b;
}

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_witness_wide(const std::vector<BA>& a, const ocltl_type_mask_wide& tau,
	splitter_type st)
{
	using desc = ba_descriptor<BA, Node>;
	size_t k = a.size();
	DBG(assert(tau.size() == ocltl_wide_minterm_count(k + 1));)
	// See ocltl_minterm above: emplace, never operator=, for tau_ba's sake.
	std::optional<BA> b(a.empty()
		? ~ocltl_unit<BA, Node>() : (a[0] & ~a[0])); // the zero element
	for (size_t A = 0; A < ocltl_wide_minterm_count(k); ++A) {
		BA m = ocltl_minterm<BA, Node>(a, A);
		if (desc::is_zero(m)) continue;
		bool neg_b_zero = tau[A];
		bool pos_b_zero = tau[A | (size_t{1} << k)];
		DBG(assert(!(neg_b_zero && pos_b_zero));) // the mask must extend tp(ā)
		if (pos_b_zero && !neg_b_zero) continue;              // b excludes m entirely
		else if (neg_b_zero && !pos_b_zero) b.emplace(*b | m); // b covers m entirely
		else b.emplace(*b | desc::splitter(m, st));            // atomless proper part
	}
	return *b;
}

} // namespace idni::tau_lang
