// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
typename node::type inverse_of(size_t operation) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return tau::bf_sub;
	if (operation == tau::bf_mul) return tau::bf_div;
	if (operation == tau::bf_sub) return tau::bf_add;
	if (operation == tau::bf_div) return tau::bf_mul;
	return tau::nul; // null is not allowed in a term
}

// Both overloads return/accept *bare* (unwrapped) values: a lone leaf
// operand as-is, or an unwrapped operation(...)-kind node for two or more
// folded operands. tau::bf-wrap is applied only where a value is used as an
// operand of another operation(...) call, never around the final result.
// The final result replaces either last_operation (a bf_add/bf_sub/bf_mul/
// bf_div node) or is used as an operand by the caller's own last_operation
// substitution, and last_operation's own parent already supplies its "bf"
// wrapper. Wrapping the final result again nests the whole term in one more
// redundant "bf" layer -- harmless for a lone leaf (leaves aren't
// re-wrapped by a later round), but for a multi-operand result this
// re-detects the same operation(...) node as a "new" block on every
// subsequent round and re-wraps it again, growing the term forever instead
// of reaching a fixpoint.
template<NodeType node>
std::pair<tref, tref> build_simplification(const trefs& arguments, size_t operation, [[maybe_unused]] size_t type) {
	using tau = tree<node>;

	tref ctes = nullptr;
	tref vars = nullptr;

	for (const tref& op : arguments) {
		auto nt = tau::get(op).get_type();
		switch (nt) {
			case tau::variable:
				vars = vars ? tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, op))
					: op;
				break;
			case tau::ba_constant: case tau::bf_t: case tau::bf_f:
				ctes = ctes ? tau::get(operation, tau::get(tau::bf, ctes), tau::get(tau::bf, op))
					: op;
				break;
			default: break;
		}
	}
	return {vars, ctes};
}

// Bare value for "args_side OP^-1 invs_side" (e.g. args_side - invs_side),
// or nullptr if both sides are absent. Never introduces a spurious identity
// operand: build_simplification's two build_simplification<node>(...) calls
// can each independently yield a null vars/ctes (e.g. "1 - X" has a
// constant only on the args side and a variable only on the invs side), and
// unconditionally tau::bf-wrapping a null tref crashes tree construction.
template<NodeType node>
tref combine_diff(size_t operation, size_t type, tref args_side, tref invs_side) {
	using tau = tree<node>;

	if (!args_side && !invs_side) return nullptr;
	if (!invs_side) return args_side;
	if (!args_side) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, invs_side));
	return tau::get(inverse_of<node>(operation), tau::get(tau::bf, args_side), tau::get(tau::bf, invs_side));
}

template<NodeType node>
tref build_simplification(const trefs& arguments, const trefs& inverses, size_t operation, size_t type) {
	using tau = tree<node>;

	// removing commons elements from arguments and inverses
	trefs args = arguments;
	trefs invs = inverses;
	for (const tref& arg : arguments) {
		auto it = std::find(invs.begin(), invs.end(), arg);
		if (it != invs.end()) {
			invs.erase(it);
			args.erase(std::find(args.begin(), args.end(), arg));
		}
	}

	if (invs.empty()) {
		auto [vars, ctes] = build_simplification<node>(args, operation, type);
		if (!vars && !ctes) return _0<node>(type);
		if (!vars) return ctes;
		if (!ctes) return vars;
		return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
	}

	if (args.empty()) {
		auto [vars, ctes] = build_simplification<node>(invs, operation, type);
		if (!vars && !ctes) return _0<node>(type);
		if (!vars) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, ctes));
		if (!ctes) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, vars));
		return tau::get(inverse_of<node>(operation),
			_0<node>(type),
			tau::get(tau::bf,
				tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes))));
	}

	auto [invs_vars, invs_ctes] = build_simplification<node>(invs, operation, type);
	auto [args_vars, args_ctes] = build_simplification<node>(args, operation, type);

	tref vars = combine_diff<node>(operation, type, args_vars, invs_vars);
	tref ctes = combine_diff<node>(operation, type, args_ctes, invs_ctes);
	if (!vars && !ctes) return _0<node>(type);
	if (!vars) return ctes;
	if (!ctes) return vars;
	return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
}

template<NodeType node>
void apply_block_operation(std::vector<std::pair<trefs, trefs>>& to_simplify, size_t inverse) {

#ifdef DEBUG
	using tau = tree<node>;
#endif // DEBUG

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
void end_block(std::vector<std::pair<trefs, trefs>>& to_simplify,
		subtree_map<node, tref>& changes, tref last_operation, size_t operation, size_t type) {
	auto [arguments, inverses] = to_simplify.back(); to_simplify.pop_back();
	changes[last_operation] = build_simplification<node>(arguments, inverses, operation, type);

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
void end_block_and_clear(std::vector<std::pair<trefs, trefs>>& to_simplify, subtree_map<node, tref>& changes,
		tref last_operation, size_t operation, size_t type) {
	end_block(to_simplify, changes, last_operation, operation, type);
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
	size_t last_block_index = 0;
	auto type = tau::get(n).get_ba_type();

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
					return last_operation = n, last_block_index = to_simplify.size() - 1, true;
				// we have to constants or variables separated to be procexsed
				// and we could change to a new block with a new operation
				} else if ((to_simplify.size() - 1) - last_block_index >= 2) {
					operation = inverse ? inverse_of<node>(nt) : nt;
					apply_block_operation<node>(to_simplify, inverse);
					return last_operation = n, last_block_index = to_simplify.size() - 1, true;
				}
				// we have no other choice to end the current block and start
				// searching for a block to simplify again
				end_block<node>(to_simplify, changes, last_operation, operation, type);
				return building_block = false, true;
			}
			case tau::bf_shr: case tau::bf_shl: case tau::bf_mod: case tau::bf_nor:
			case tau::bf_xnor: case tau::bf_nand: case tau::bf_or: case tau::bf_xor:
			case tau::bf_and: case tau::bf_neg: {
				if (!building_block) return true;
				end_block_and_clear<node>(to_simplify, changes, last_operation, operation, type);
				return building_block = false, last_operation = nullptr, last_block_index = 0, operation = tau::nul, true;
			}
			default: return true;
		}
	};

	post_order<node>(n).search(f);
	if (building_block) end_block<node>(to_simplify, changes, last_operation, operation, type);
	return changes;
}

template<NodeType node>
tref bv_ba_custom_simplification(const tref term) {
	// Loop simplify_blocks to a fixpoint. Previously current was inserted
	// into visited every iteration right before the loop condition
	// checked for it, so the condition was always false and the loop ran
	// exactly once, discarding any further simplification. Guard against
	// a longer oscillating cycle (as done in repeat_all, see RR-2) with a
	// visited set, and against an ever-growing rewrite with max_rounds.
	tref current = term;
	std::unordered_set<tref> visited{current};
	constexpr size_t max_rounds = 1'000'000;
	size_t round = 0;
	for (; round < max_rounds; ++round) {
		auto changes = simplify_blocks<node>(current);
		tref next = rewriter::replace<node>(current, changes);
		if (next == current) break;
		if (visited.contains(next)) { current = next; break; }
		visited.insert(next);
		current = next;
	}
	if (round == max_rounds)
		LOG_ERROR << "bv_ba_custom_simplification: exceeded " << max_rounds
			<< " rounds without reaching a fixpoint or cycle, giving up";

#ifdef DEBUG
	LOG_TRACE << "bv_ba_custom_simplification/term: " << LOG_FM(term) << "\n";
	LOG_TRACE << "bv_ba_custom_simplification/final: " << LOG_FM(current) << "\n";
#endif // DEBUG

	return current;
}

} // namespace idni::tau_lang