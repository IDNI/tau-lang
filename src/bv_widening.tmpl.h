// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "bv_widening.h"            // Only for IDE resolution, not really needed.

#include <algorithm>
#include <stdexcept>
#include <string>

namespace idni::tau_lang {

// Read the literal shift amount out of a `bf_shl` operator node, or `0`
// when it is not a literal bitvector constant (a variable shift amount
// executes at the left operand's width and may wrap -- see the `bf_shl`
// rule in bv_widening.h). A shift amount too large to fit `unsigned long
// long` (an exceedingly wide bitvector literal) is treated the same way:
// `std::stoull` throwing is caught and folded into the same "no known
// growth" fallback as a non-constant amount, rather than propagating an
// uncaught exception out of needed_width.
//
// `op` is the operator node directly under a `bf` wrapper (i.e. `n[0]`
// for the `bf` node whose operator is `bf_shl`). Its second child (`op[1]`)
// is itself a `bf` wrapper around the shift-amount operand, and `op[1][0]`
// drills one level further to the actual leaf/operator -- the same
// two-level access idiom `term_shl` uses to read its operands
// (bv_ba_hooks.tmpl.h:596-597: `tau::get(symbol)[0][1][0]`).
template <NodeType node>
size_t bf_shl_shift_amount(const tree<node>& op) {
	using tau = tree<node>;

	const tau& amount = op[1][0];
	if (amount.is_ba_constant() && amount.get_ba_type() > 0
			&& is_bv_type_family<node>(amount.get_ba_type()))
	{
		const auto c = amount.get_ba_constant();
		try {
			return static_cast<size_t>(
				std::stoull(std::get<bv>(c).getBitVectorValue(10)));
		} catch (const std::exception&) {
			return 0; // too large (or malformed) to parse: no known growth
		}
	}
	return 0;
}

template <NodeType node>
size_t needed_width(tref bf_node, size_t base_w, size_t& maxW) {
	using tau = tree<node>;

	const tau& n = tau::get(bf_node);
	DBG(assert(n.is(tau::bf));)
	// bf -> op (bf_add/bf_mul/.../variable/ba_constant/...); each operand of
	// a binary/unary op is itself a `bf` wrapper, so recursion is over
	// `op.child(i)` (see term_add's `tau::get(symbol)[0][0]`,
	// bv_ba_hooks.tmpl.h:29-30, for the same one-level-under-`bf` access).
	const tau& op = n[0];

	auto rec = [&](size_t i) -> size_t {
		return needed_width<node>(op.child(i), base_w, maxW);
	};

	size_t w = 0;
	switch (op.value.nt) {
	// Leaves: contribute the caller-supplied base width. Deliberately does
	// not consult get_ba_type() here -- callers of needed_width may parse
	// without type inference and pass base_w explicitly (see bv_widening.h).
	case tau::variable:
	case tau::ba_constant:
	case tau::bf_t:
	case tau::bf_f:
		w = base_w;
		break;
	// Transparent wrapper: "(" bf ")" contributes its inner width unchanged.
	case tau::bf_parenthesis: {
		size_t inner = rec(0);
		if (inner == 0) return 0;
		w = inner;
		break;
	}
	// Boundary: the cast's own target width, regardless of its operand.
	// The target type id is attached directly to the bf_cast node at
	// parse-to-tree time (tau_tree_from_parser.tmpl.h, case bf_cast), so no
	// type inference is required to read it here.
	case tau::bf_cast:
		w = get_bv_width<node>(op.get_ba_type());
		break;
	case tau::bf_add: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r) + 1;
		break;
	}
	case tau::bf_sub: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r);
		break;
	}
	case tau::bf_mul: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l + r;
		break;
	}
	case tau::bf_div:
	case tau::bf_mod: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l; // keep the dividend's width; result magnitude is bounded by it
		break;
	}
	case tau::bf_min:
	case tau::bf_max:
	case tau::bf_and:
	case tau::bf_or:
	case tau::bf_xor:
	case tau::bf_nand:
	case tau::bf_nor:
	case tau::bf_xnor: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r);
		break;
	}
	case tau::bf_neg: {
		size_t l = rec(0);
		if (l == 0) return 0;
		w = l; // complement runs at the operand's own width
		break;
	}
	case tau::bf_shr: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l; // a right shift never grows the needed width
		break;
	}
	case tau::bf_shl: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l + bf_shl_shift_amount<node>(op); // 0 when the amount is not a literal
		break;
	}
	default:
		return 0; // opaque subterm (bf_ref, capture, ...): caller skips the atom
	}
	maxW = std::max(maxW, w);
	return w;
}

} // namespace idni::tau_lang
