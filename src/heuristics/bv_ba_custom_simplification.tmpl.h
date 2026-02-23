// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t inverse_of(size_t operation) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return tau::bf_sub;
	if (operation == tau::bf_mul) return tau::bf_div;
	if (operation == tau::bf_sub) return tau::bf_add;
	if (operation == tau::bf_div) return tau::bf_mul;
	return tau::nul; // null is not allowed in a term
}

template<NodeType node>
tref build_simplification(const trefs& arguments, size_t operation) {
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
tref build_simplification(const trefs& arguments, const trefs& inverses, size_t operation) {
	using tau = tree<node>;

	if (inverses.empty()) return build_simplification<node>(arguments, operation);
	return tau::get(tau::bf, tau::get(inverse_of<node>(operation),
		build_simplification<node>(arguments, operation),
		build_simplification<node>(inverses, operation)));
}

template<NodeType node>
void apply_block_operation(std::vector<std::pair<trefs, trefs>>& to_simplify, size_t inverse) {
	using tau = tree<node>;

	auto [right_args, right_invs] = to_simplify.back();	to_simplify.pop_back();

	auto from_args = inverse ? right_invs : right_args;
	auto from_invs = inverse ? right_args : right_invs;
	//auto [left_args, left_invs] = to_simplify.back();

	to_simplify.back().first.insert(to_simplify.back().first.end(), from_args.begin(), from_args.end());
	to_simplify.back().second.insert(to_simplify.back().second.end(), from_invs.begin(), from_invs.end());

#ifdef DEBUG
	LOG_TRACE << "apply_block_operation/left_args: ";
	for (const auto& arg : to_simplify.back().first) LOG_TRACE << "\t" << tau::get(arg).to_str() << " ";
	LOG_TRACE << "\n";
	LOG_TRACE << "apply_block_operation/left_invs: ";
	for (const auto& inv : to_simplify.back().second) LOG_TRACE << "\t" << tau::get(inv).to_str() << " ";
	LOG_TRACE << "\n";
#endif // DEBUG
}

template<NodeType node>
void end_block(std::vector<std::pair<trefs, trefs>>& to_simplify, subtree_map<node, tref>& changes, tref last_operation, size_t operation) {
	auto [arguments, inverses] = to_simplify.back(); to_simplify.pop_back();
	changes[last_operation] = build_simplification<node>(arguments, inverses, operation);

#ifdef DEBUG
	using tau = tree<node>;

	LOG_TRACE << "end_block/operation: " << LOG_NT(operation) << "\n";
	LOG_TRACE << "end_block/arguments: ";
	for (const auto& arg : arguments) LOG_TRACE << "\t" <<tau::get(arg).to_str() << " ";
	LOG_TRACE << "\n";
	LOG_TRACE << "end_block/inverses: ";
	for (const auto& inv : inverses) LOG_TRACE << "\t" <<tau::get(inv).to_str() << " ";
	LOG_TRACE << "\n";
	LOG_TRACE << "end_block/simplification: " << tau::get(changes[last_operation]).tree_to_str() << "\n";
#endif // DEBUG
}

template<NodeType node>
void end_block_and_clear(std::vector<std::pair<trefs, trefs>>& to_simplify, subtree_map<node, tref>& changes, tref last_operation, size_t operation) {
	end_block(to_simplify, changes, last_operation, operation);
	to_simplify.clear();
}

template<NodeType node>
subtree_map<node, tref> simplify_blocks(const tref& n) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	std::vector<std::pair<trefs, trefs>> to_simplify;
	size_t operation =  tau::nul;
	bool building_block = false;
	tref last_operation = nullptr;
	size_t last_blosk_index = 0;

	auto f = [&](tref n) -> bool {
		auto nt = tau::get(n).get_type();

		switch(nt) {
			case tau::variable: case tau::ba_constant: case tau::bf_t: case tau::bf_f: {

				DBG(LOG_TRACE << "simplify_block/f/" << LOG_NT(nt) << "/n: " << tau::get(n).to_str() << "\n";)

				return to_simplify.emplace_back(trefs{n}, trefs{}), true;
			}
			case tau::bf_add: case tau::bf_mul:
			case tau::bf_sub: case tau::bf_div: {
				auto inverse = (nt == tau::bf_sub || nt == tau::bf_div);
				if (!building_block) {
					operation = inverse ? inverse_of<node>(nt) : nt;
					building_block = true;

					DBG(LOG_TRACE << "simplify_block/f/" << LOG_NT(nt) << "/operation: " << LOG_NT(operation) << "\n";)
					DBG(LOG_TRACE << "simplify_block/f/" << LOG_NT(nt) << "/building_block: " << building_block << "\n";)
				}

				DBG(LOG_TRACE << "simplify_block/f/n/" << LOG_NT(nt) << ": " << tau::get(n).to_str() << "\n";)

				if (operation == nt || inverse_of<node>(operation) == nt) {
					apply_block_operation<node>(to_simplify, inverse);
					return last_operation = n, last_blosk_index = to_simplify.size() - 1, true;
				// we have to constants or variables separated to be procexsed
				// and we could change to a new block with a new operation
				} else if ((to_simplify.size() - 1) - last_blosk_index >= 2) {
					operation = inverse ? inverse_of<node>(nt) : nt;
					apply_block_operation<node>(to_simplify, inverse);
					return last_operation = n, last_blosk_index = to_simplify.size() - 1, true;
				}
				// we have no other choice to end the current block and start
				// searching for a block to simplify again
				end_block<node>(to_simplify, changes, last_operation, operation);
				return building_block = false, true;
			}
			case tau::bf_shr: case tau::bf_shl: case tau::bf_mod: case tau::bf_nor:
			case tau::bf_xnor: case tau::bf_nand: case tau::bf_or: case tau::bf_xor:
			case tau::bf_and: case tau::bf_neg: {
				if (!building_block) return true;
				end_block_and_clear<node>(to_simplify, changes, last_operation, operation);
				return building_block = false, last_operation = nullptr, last_blosk_index = 0, operation = tau::nul, true;
			}
			default: return true;
		}
	};

	// post_order<node>(n).search(f);
	pre_order<node>(n).search(idni::all,  idni::all, f);
	if (building_block) end_block<node>(to_simplify, changes, last_operation, operation);
	return changes;
}

template<NodeType node>
tref bv_ba_custom_simplification(const tref term) {
	auto changes = simplify_blocks<node>(term);
	auto result = rewriter::replace<node>(term, changes);

	DBG(LOG_TRACE << "bv_ba_custom_simplification/term: " << tree<node>::get(term).to_str() << "\n";)
	DBG(LOG_TRACE << "bv_ba_custom_simplification/result: " << tree<node>::get(result).to_str() << "\n";)

	return result;
}

} // namespace idni::tau_lang