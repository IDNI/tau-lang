// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
bool is_simplifiable(const size_t operation) {
	using tau = tree<node>;

	return operation == tau::bf_add || operation == tau::bf_mul;
}

template<NodeType node>
size_t inverse_of(size_t operation) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return tau::bf_sub;
	if (operation == tau::bf_mul) return tau::bf_div;
	return tau::nul; // null is not allowed in a term
}

template<NodeType node>
bool is_variable_or_constant(const tref n) {
	using tau = tree<node>;

	return is<node, tau::variable>(n) || is<node, tau::ba_constant>(n)
		|| is<node, tau::bf_t>(n) || is<node, tau::bf_f>(n);
}

template<NodeType node>
tref simplify(const trefs& arguments, size_t operation) {
	using tau = tree<node>;

	tref ctes = nullptr;
	tref vars = nullptr;

	for (const tref& op : arguments) {
		auto nt = tau::get(op).get_type();
		switch (nt) {
			case tau::variable:
				vars = vars	? tau::get(tau::bf, tau::get(operation, vars, tau::get(tau::bf, op)))
					: tau::get(tau::bf, op);
				break;
			case tau::ba_constant: case tau::bf_t: case tau::bf_f:
				ctes = ctes ? tau::get(tau::bf, tau::get(operation, ctes, tau::get(tau::bf, op)))
					: tau::get(tau::bf, op);
				break;
			default: break;
		}
	}

	if (vars && ctes) return tau::get(tau::bf, tau::get(operation, vars, ctes));
	else if (vars) return vars;
	else if (ctes) return ctes;
	else return tau::get(tau::bf, arguments[0]);
}

template<NodeType node>
tref simplify(const trefs& arguments, const trefs& inverses, size_t operation) {
	using tau = tree<node>;

	if (inverses.empty()) return simplify<node>(arguments, operation);
	return tau::get(tau::bf, tau::get(inverse_of<node>(operation),
		simplify<node>(arguments, operation),
		simplify<node>(inverses, operation)));
}

template<NodeType node>
subtree_map<node, tref> simplify(const tref& n, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	std::vector<std::pair<trefs, trefs>> to_simplify;
	size_t operation =  tau::nul;
	bool searching = true;
	tref last_block = nullptr;
	size_t last_blosk_index = 0;

	auto f = [&](tref n) -> bool {
		auto nt = tau::get(n).get_type();
		// we skip bf symbols
		if (nt == tau::bf) return true;
		if (searching) { // searching for the start of the block of operations to simplify
			if (is_simplifiable<node>(nt) // we have two children
					&& is_variable_or_constant<node>(tau::get(n).first())
					&& is_variable_or_constant<node>(tau::get(n).second())) {
				to_simplify.emplace_back(trefs{tau::get(n).first(), tau::get(n).second()}, trefs{});
				return searching = false, last_block = n, last_blosk_index = to_simplify.size() - 1, operation = nt, true;
			} else if (is_simplifiable<node>(inverse_of<node>(nt)) // we also have two children
					&& is_variable_or_constant<node>(tau::get(n).first())
					&& is_variable_or_constant<node>(tau::get(n).second())) {
				to_simplify.emplace_back(trefs{tau::get(n).first()}, trefs{tau::get(n).second()});
				return searching = false, last_block = n, last_blosk_index = to_simplify.size() - 1, operation = nt, true;
			}
		}
		// we reading the block of operations to simplify so we push the leaves
		if (is_variable_or_constant<node>(n)) {
			to_simplify.emplace_back(trefs{n}, trefs{});
			return true;
		}
		// if the operation correspond to the one we are simplifying
		if (operation == nt) {
			auto [right_args, right_invs] = to_simplify.back();
			to_simplify.pop_back();
			auto [left_args, left_invs] = to_simplify.back();
			left_args.insert(left_args.end(), right_args.begin(), right_args.end());
			left_invs.insert(left_invs.end(), right_invs.begin(), right_invs.end());
			return last_block = n, last_blosk_index = to_simplify.size() - 1, true;
		}
		// if the operation is the inverse of the one we are simplifying
		if (inverse_of<node>(operation) == nt) {
			auto [right_args, right_invs] = to_simplify.back();
			to_simplify.pop_back();
			auto [left_args, left_invs] = to_simplify.back();
			left_args.insert(left_args.end(), right_invs.begin(), right_invs.end());
			left_invs.insert(left_invs.end(), right_args.begin(), right_args.end());
			return last_block = n, last_blosk_index = to_simplify.size() - 1, true;
		}
		// otherwise, we simplify the block as other opeartions are involved rightnow
		changes[last_block] = simplify<node>(to_simplify[last_blosk_index].first, to_simplify[last_blosk_index].second, operation);
		return to_simplify.clear(), searching = true, last_block = nullptr, last_blosk_index = 0, operation = tau::nul, true;
	};

	post_order<node>(n).search(f);
	return changes;
}

template<NodeType node>
tref bv_ba_custom_simplification(const tref term) {
	subtree_map<node, tref> changes;
	tref simplified = term;
	do {
		simplified = rewriter::replace<node>(simplified, changes);
		changes = simplify<node>(simplified, changes);
	} while (!changes.empty());
	return simplified;
}

} // namespace idni::tau_lang