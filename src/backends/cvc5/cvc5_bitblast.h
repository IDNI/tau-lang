// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cvc5_bitblast.h - Deciding a narrow bit-vector formula on a BDD of its bits

#ifndef __IDNI__TAU__BACKENDS__CVC5_BITBLAST_H__
#define __IDNI__TAU__BACKENDS__CVC5_BITBLAST_H__

#include <cvc5/cvc5.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "backends/bdds/data_bdd.h"

namespace idni::tau_lang {

/**
 * @brief Decides the satisfiability of a cvc5 formula over narrow bit-vectors
 * by building the BDD of its bits.
 *
 * Every bit of every variable is a BDD variable, the same bit of all
 * variables side by side, so a comparison of two values stays linear in
 * their width. Operators become circuits (bit_circuits) and a quantifier
 * quantifies the bits of its variables, so a formula of any quantifier
 * alternation is decided exactly: the BDD of a closed formula is true or
 * false, and a formula with free constants is satisfiable when its BDD is
 * not false.
 *
 * cvc5's instantiation-based procedure has no bound on such formulas: a
 * product under alternating quantifiers, or a chain of a few hundred nested
 * existentials, keeps it busy for minutes on 8-bit values that have only
 * 256 of them each. The BDD answers these in milliseconds; what it cannot
 * hold it declines.
 *
 * @param f A Boolean cvc5 term.
 * @param max_width Widest bit-vector sort accepted.
 * @param max_nodes Most nodes live at once before giving up. Once the
 * table fills, the nodes of the terms no longer read are freed, and the
 * term that ran out of nodes is built again, within a few times the room
 * it had, when that more than doubled its room.
 * @return The verdict, or nullopt when a sort is wider than @p max_width,
 * two values that are neither constant nor the same are multiplied at more
 * than 10 bits, an operator has no circuit here (division, remainder, and
 * anything not built by the make_term_* / make_bitvector_* builders), or
 * the BDD outgrows @p max_nodes, or the time @p deadline passes, which
 * sets @p late. nullopt says nothing about the formula.
 * @param deadline Time after which the BDD stops and the call declines;
 * no limit by default.
 * @param late Optional out flag (may be null); on return it is true exactly
 * when the BDD stopped at @p deadline.
 */
inline std::optional<bool> cvc5_bitblast_sat(const cvc5::Term& f,
	size_t max_width, size_t max_nodes,
	data_bdd::clock::time_point deadline = data_bdd::clock::time_point::max(),
	bool* late = nullptr)
{
	using cvc5::Kind;
	using cvc5::Term;
	using bits = bit_circuits::bits;
	using id = data_bdd::id;
	constexpr size_t max_product_width = 10;

	// A binder's variable list has no value of its own.
	auto first_operand = [](const Term& t) -> size_t {
		const Kind k = t.getKind();
		return k == Kind::FORALL || k == Kind::EXISTS ? 1 : 0;
	};
	// Variables and free constants, outermost first, and their widths.
	std::unordered_map<Term, size_t> index;
	std::vector<size_t> widths;
	{
		std::unordered_set<Term> seen;
		std::vector<Term> todo{ f };
		// reversed pushes keep the first child first, so the outer
		// binders take the lower indices
		while (!todo.empty()) {
			Term t = todo.back();
			todo.pop_back();
			if (!seen.insert(t).second) continue;
			const Kind k = t.getKind();
			if (k == Kind::VARIABLE || k == Kind::CONSTANT) {
				if (!t.getSort().isBitVector()) return std::nullopt;
				const size_t w = t.getSort().getBitVectorSize();
				if (w == 0 || w > max_width) return std::nullopt;
				index.emplace(t, widths.size());
				widths.push_back(w);
				continue;
			}
			if (t.getSort().isBitVector()
				&& t.getSort().getBitVectorSize() > max_width)
					return std::nullopt;
			// The BDD of a product of two independent values grows about
			// sevenfold with every two bits: 85 thousand nodes at 10
			// bits, 600 thousand at 12, 4 million at 14. A square and a
			// product by a constant stay small.
			if (k == Kind::BITVECTOR_MULT
				&& t.getSort().getBitVectorSize() > max_product_width)
			{
				size_t variable = 0;
				for (size_t i = 0; i < t.getNumChildren(); ++i)
					if (t[i].getKind() != Kind::CONST_BITVECTOR
						&& t[i] != t[0]) ++variable;
				if (variable && t[0].getKind() != Kind::CONST_BITVECTOR)
					return std::nullopt;
			}
			for (size_t i = t.getNumChildren(); i-- > 0; )
				todo.push_back(t[i]);
		}
	}
	const size_t nv = widths.size();
	auto bdd_var = [nv](size_t v, size_t bit) {
		return (uint32_t)(bit * nv + v);
	};
	size_t max_w = 1;
	for (size_t w : widths) max_w = std::max(max_w, w);

	// the memo of the operations grows with them; past four entries a
	// node the attempt costs more than it is worth
	data_bdd bdd(max_nodes, 4 * max_nodes);
	bdd.stop_at(deadline);
	struct tell_late {
		const data_bdd& bdd; bool* late;
		~tell_late() { if (late) *late = bdd.late; }
	} tell{ bdd, late };

	// A Boolean term takes one entry, a bit-vector term one per bit.
	std::unordered_map<Term, bits> val;
	auto each_value = [&](auto&& mark) {
		for (const auto& [_, x] : val)
			for (id e : x) mark(e);
	};
	// Drops the values no unevaluated term reads, which no term asks for
	// again. Every unevaluated term is reached from f through unevaluated
	// terms, since a term is evaluated after its operands.
	auto drop_values_read = [&] {
		std::unordered_set<Term> seen, read;
		std::vector<Term> todo{ f };
		while (!todo.empty()) {
			const Term t = todo.back();
			todo.pop_back();
			if (!seen.insert(t).second) continue;
			if (val.contains(t)) {
				read.insert(t);
				continue;
			}
			for (size_t i = first_operand(t); i < t.getNumChildren(); ++i)
				todo.push_back(t[i]);
		}
		std::erase_if(val, [&](const auto& e) {
			return !read.contains(e.first);
		});
	};
	std::vector<std::pair<Term, bool>> stack{ { f, false } };
	while (!stack.empty()) {
		auto [t, expanded] = stack.back();
		stack.pop_back();
		if (val.contains(t)) continue;
		const Kind k = t.getKind();
		if (!expanded) {
			if (k == Kind::VARIABLE || k == Kind::CONSTANT) {
				const size_t v = index.at(t);
				bits out(widths[v]);
				for (size_t b = 0; b < widths[v]; ++b)
					out[b] = bdd.var(bdd_var(v, b));
				val.emplace(t, std::move(out));
				continue;
			}
			if (k == Kind::CONST_BOOLEAN) {
				val.emplace(t, bits{ t.getBooleanValue()
					? data_bdd::T : data_bdd::F });
				continue;
			}
			if (k == Kind::CONST_BITVECTOR) {
				// most significant digit first
				const std::string s = t.getBitVectorValue(2);
				bits out(t.getSort().getBitVectorSize(), data_bdd::F);
				for (size_t b = 0; b < s.size() && b < out.size(); ++b)
					if (s[s.size() - 1 - b] == '1')
						out[b] = data_bdd::T;
				val.emplace(t, std::move(out));
				continue;
			}
			stack.push_back({ t, true });
			for (size_t i = t.getNumChildren(); i-- > first_operand(t); )
				if (!val.contains(t[i])) stack.push_back({ t[i], false });
			continue;
		}
		const size_t room = bdd.room();
		const size_t n = t.getNumChildren();
		auto arg = [&](size_t i) -> const bits& { return val.at(t[i]); };
		auto one = [&](size_t i) { return arg(i)[0]; };
		bits out;
		switch (k) {
		case Kind::NOT: out = { bdd.neg(one(0)) }; break;
		case Kind::AND: case Kind::OR: {
			id acc = k == Kind::AND ? data_bdd::T : data_bdd::F;
			for (size_t i = 0; i < n; ++i)
				acc = k == Kind::AND ? bdd.conj(acc, one(i))
					: bdd.disj(acc, one(i));
			out = { acc };
			break;
		}
		case Kind::XOR: out = { bdd.exor(one(0), one(1)) }; break;
		case Kind::IMPLIES:
			out = { bdd.disj(bdd.neg(one(0)), one(1)) }; break;
		case Kind::EQUAL: case Kind::DISTINCT: {
			if (n != 2) return std::nullopt;
			const id e = bit_circuits::same(bdd, arg(0), arg(1));
			out = { k == Kind::EQUAL ? e : bdd.neg(e) };
			break;
		}
		case Kind::ITE:
			out = bit_circuits::select(bdd, one(0), arg(1), arg(2));
			break;
		case Kind::BITVECTOR_ULT:
			out = { bit_circuits::less(bdd, arg(0), arg(1)) }; break;
		case Kind::BITVECTOR_UGT:
			out = { bit_circuits::less(bdd, arg(1), arg(0)) }; break;
		case Kind::BITVECTOR_ULE:
			out = { bdd.neg(bit_circuits::less(bdd, arg(1), arg(0))) };
			break;
		case Kind::BITVECTOR_UGE:
			out = { bdd.neg(bit_circuits::less(bdd, arg(0), arg(1))) };
			break;
		case Kind::BITVECTOR_NOT:
			out = arg(0);
			for (auto& e : out) e = bdd.neg(e);
			break;
		case Kind::BITVECTOR_AND: case Kind::BITVECTOR_OR:
		case Kind::BITVECTOR_XOR: case Kind::BITVECTOR_NAND:
		case Kind::BITVECTOR_NOR: case Kind::BITVECTOR_XNOR: {
			if (n != 2) return std::nullopt;
			const bits& x = arg(0);
			const bits& y = arg(1);
			out.resize(x.size());
			for (size_t i = 0; i < x.size(); ++i) {
				switch (k) {
				case Kind::BITVECTOR_AND:
					out[i] = bdd.conj(x[i], y[i]); break;
				case Kind::BITVECTOR_OR:
					out[i] = bdd.disj(x[i], y[i]); break;
				case Kind::BITVECTOR_XOR:
					out[i] = bdd.exor(x[i], y[i]); break;
				case Kind::BITVECTOR_NAND:
					out[i] = bdd.neg(bdd.conj(x[i], y[i])); break;
				case Kind::BITVECTOR_NOR:
					out[i] = bdd.neg(bdd.disj(x[i], y[i])); break;
				default:
					out[i] = bdd.iff(x[i], y[i]); break;
				}
			}
			break;
		}
		case Kind::BITVECTOR_ADD:
			out = arg(0);
			for (size_t i = 1; i < n; ++i)
				out = bit_circuits::add(bdd, out, arg(i));
			break;
		case Kind::BITVECTOR_SUB:
			out = bit_circuits::sub(bdd, arg(0), arg(1)); break;
		case Kind::BITVECTOR_MULT:
			// a product gets a quarter of the table, so the attempt
			// gives up early on one that explodes
			out = arg(0);
			for (size_t i = 1; i < n && !bdd.full; ++i) {
				bits p;
				if (!bit_circuits::mul(bdd, out, arg(i),
					bdd.size() + max_nodes / 4, p) && !bdd.full)
						return std::nullopt;
				out = std::move(p);
			}
			break;
		case Kind::BITVECTOR_SHL: case Kind::BITVECTOR_LSHR:
			out = bit_circuits::shifted(bdd, arg(0), arg(1),
				k == Kind::BITVECTOR_SHL);
			break;
		case Kind::BITVECTOR_ZERO_EXTEND:
			out = arg(0);
			out.resize(t.getSort().getBitVectorSize(), data_bdd::F);
			break;
		case Kind::BITVECTOR_EXTRACT: {
			cvc5::Op op = t.getOp();
			const uint32_t hi = op[0].getUInt32Value();
			const uint32_t lo = op[1].getUInt32Value();
			const bits& x = arg(0);
			if (hi < lo || hi >= x.size()) return std::nullopt;
			out.assign(x.begin() + lo, x.begin() + hi + 1);
			break;
		}
		case Kind::FORALL: case Kind::EXISTS: {
			std::vector<bool> qs(nv * max_w, false);
			for (size_t i = 0; i < t[0].getNumChildren(); ++i) {
				auto it = index.find(t[0][i]);
				if (it == index.end()) return std::nullopt;
				for (size_t b = 0; b < widths[it->second]; ++b)
					qs[bdd_var(it->second, b)] = true;
			}
			out = { bdd.quantify(one(1), qs, k == Kind::EXISTS) };
			break;
		}
		default: return std::nullopt;
		}
		// A term built again may grow the table by four times the room it
		// ran out of, and a little more: one that needs more is declined
		// at the cost of a small part of the table instead of all of it.
		if (bdd.full) {
			drop_values_read();
			bdd.collect(each_value);
			if (!bdd.worth_redoing(room)) return std::nullopt;
			bdd.grow_at_most(4 * room + max_nodes / 64);
			stack.push_back({ t, true });
			continue;
		}
		bdd.grow_freely();
		val.emplace(t, std::move(out));
		if (bdd.wants_collect()) {
			drop_values_read();
			bdd.collect(each_value);
		}
	}
	const bits& r = val.at(f);
	if (r.size() != 1 || bdd.full) return std::nullopt;
	return r[0] != data_bdd::F;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__CVC5_BITBLAST_H__
