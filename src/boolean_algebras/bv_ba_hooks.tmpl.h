// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba-hooks"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
tref term_add(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_add/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto add_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_add(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_add/add_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 + X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(t));
				return add_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 + 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return add_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 + X
		case tau::bf_f: return tau::get(symbol)[0].second();
		default: break;
	}
	switch (c2.value.nt) {
		// X + 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return add_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// x + 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } + { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return add_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_sub(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_sub/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto sub_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_sub(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_sub/sub_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 - X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(t));
				return sub_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 - 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				return tau::_0(c2.get_ba_type());
			}
			break;
		}
		// 0 - X cannot be simplified
		case tau::bf_f: {
			// 0 - 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return sub_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X - 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return sub_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// x - 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// X - X
	if (c1 == c2) return tau::_0(c2.get_ba_type());
	// { ... } - { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return sub_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_mul(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_mul/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto mul_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_mul(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_mul/mul_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 * X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mul_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 * 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mul_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 * X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X * 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mul_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X * 0
		case tau::bf_f: return tau::_0(c1.get_ba_type());
		default: break;
	}
	// {1} * X
	if (c1.is_ba_constant() && c1.get_ba_type() > 0) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		if (const bv cc = std::get<bv>(c1.get_ba_constant());
			cc.isBitVectorValue() && cc.getBitVectorValue(10) == "1") {
			return tau::get(symbol)[0].second();
		}
	}
	// X * {1}
	if (c2.is_ba_constant() && c2.get_ba_type() > 0) {
		DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
		if (const bv cc = std::get<bv>(c2.get_ba_constant());
			cc.isBitVectorValue() && cc.getBitVectorValue(10) == "1") {
			return tau::get(symbol)[0].first();
		}
	}
	// { ... } * { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return mul_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_div(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_div/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto div_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_div(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_div/div_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 / X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 / 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return div_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			// 1 / 0
			if (c2.is(tau::bf_f)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return div_consts(
					make_bitvector_top_elem(width),
					make_bitvector_bottom_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 / X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X / 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X / 0
		case tau::bf_f: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_bottom_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		default: break;
	}
	// X / X
	if (c1 == c2) {
		const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
		return tau::build_bf_ba_constant(make_bitvector_value(1, width), c2.get_ba_type());
	}
	// { ... } / { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return div_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_mod(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_mod/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto mod_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_mod(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_mod/mod_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 % X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 % 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mod_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			// 1 % 0
			if (c2.is(tau::bf_f)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mod_consts(
					make_bitvector_top_elem(width),
					make_bitvector_bottom_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 % X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X % 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X % 0
		case tau::bf_f: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_bottom_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		default: break;
	}
	// { ... } % { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return mod_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_shr(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_shr/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto shr_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_shr(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_shr/shr_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 >> X
		case tau::bf_t: {
			// 1 >> {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 >> 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shr_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 >> X
		case tau::bf_f: {
			// 0 >> {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					make_bitvector_bottom_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 0 >> 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shr_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X >> 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X >> 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } >> { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return shr_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_shl(tref symbol) {
	using tau = tree<node>;

	DBG(LOG_TRACE << "term_shl/symbol:" << LOG_FM_TREE(symbol) << "\n";)

	auto shl_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_shl(c1, c2);
		typename node::constant v = {res};
		auto new_symbol = tau::build_bf_ba_constant(v, type_id);
		DBG(LOG_TRACE << "term_shl/shl_constant:" << LOG_FM_TREE(new_symbol) << "\n";)
		return new_symbol;
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 << X
		case tau::bf_t: {
			// 1 << {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 << 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shl_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 << X
		case tau::bf_f: {
			// 0 << {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					make_bitvector_bottom_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
				}
			// 0 << 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shl_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X << 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X << 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } << { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return shl_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_nor(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_or(c1, c2));
}

template<NodeType node>
tref term_xnor(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_xor(c1, c2));
}

template<NodeType node>
tref term_nand(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_and(c1, c2));
}

template <typename ...BAs> requires BAsPack<BAs...>
tref base_ba_symbol_simplification(tref symbol, const bv&) {
	using node_t = node<BAs...>;
	using tau = tree<node_t>;

	switch (tau::get(symbol)[0].get_type()) {
		case tau::bf_add: return term_add<node_t>(symbol);
		case tau::bf_sub: return term_sub<node_t>(symbol);
		case tau::bf_mul: return term_mul<node_t>(symbol);
		case tau::bf_div: return term_div<node_t>(symbol);
		case tau::bf_mod: return term_mod<node_t>(symbol);
		case tau::bf_shr: return term_shr<node_t>(symbol);
		case tau::bf_shl: return term_shl<node_t>(symbol);
		case tau::bf_nor: return term_nor<node_t>(symbol);
		case tau::bf_xnor: return term_xnor<node_t>(symbol);
		case tau::bf_nand: return term_nand<node_t>(symbol);
		default: return symbol;
	}
}


} // namespace idni::tau_lang