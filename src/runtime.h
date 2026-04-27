// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"

namespace idni::tau_lang {


template <>
struct nso_factory<bv, sbf_ba> {
	using node_t = node<bv, sbf_ba>;
	using tau = tree<node_t>;

	static bool is_syntactic_one(const std::variant<bv, sbf_ba>& elem);

	static bool is_syntactic_zero(const std::variant<bv, sbf_ba>& elem);

	static bool is_one(const std::variant<bv, sbf_ba>& elem);

	static bool is_zero(const std::variant<bv, sbf_ba>& elem);

	static bool is_closed(const std::variant<bv, sbf_ba>& elem);

	static std::vector<std::string> types();

	static tref default_type();

	static std::string one(const tref type_tree);

	static std::string zero(const tref type_tree);

	static tref splitter_one();

	static std::variant<bv, sbf_ba> splitter(const std::variant<bv, sbf_ba>& elem, splitter_type st);

	static std::variant<bv, sbf_ba> splitter(const std::variant<bv, sbf_ba>& elem);

	static tref unpack_tau_ba(const std::variant<bv, sbf_ba>&);

	static std::variant<bv, sbf_ba> pack_tau_ba(tref);

	static std::variant<bv, sbf_ba> normalize(const std::variant<bv, sbf_ba>& v);

	static tref simplify_symbol(tref symbol);

	static tref simplify_term(tref term);
};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba> {
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;

	static 	bool is_syntactic_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static bool is_syntactic_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static bool is_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static bool is_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static bool is_closed(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static std::vector<std::string> types();

	static tref default_type();

	static std::string one(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static std::string zero(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static tref splitter_one(tref type_tree);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> splitter(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem, splitter_type st);
	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> splitter(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);

	static tref unpack_tau_ba(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> pack_tau_ba(tref c);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> normalize(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static tref simplify_symbol(tref symbol);

	static tref simplify_term(tref term);
};


template<>
struct nso_factory<bv, Bool> {
	using node_t = node<bv, Bool>;
	using tau = tree<node_t>;

	static bool is_syntactic_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	static bool is_syntactic_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	static bool is_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	static bool is_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	static bool is_closed(const std::variant<bv, Bool>&) {
		// We return true as bv and Bool are closed (for our pourposes)
		return true;
	}

	static std::vector<std::string> types() { return { "bool" }; }

	static tref default_type() { return bool_type<node_t>(); }

	static std::string one(const tref) { return "1"; }

	static std::string zero(const tref) { return "0"; }

	static std::variant<bv, Bool> splitter_one(const std::string) { return Bool(true);}

	// There is no tau_ba
	static tref unpack_tau_ba(const std::variant<bv, Bool>&) { return nullptr; }

	static std::variant<bv, Bool> pack_tau_ba(tref) { return {}; }

	static std::variant<bv, Bool> normalize(const std::variant<bv, Bool>& v) {
		return std::holds_alternative<bv>(v)
			? std::variant<bv, Bool>(normalize_bv(std::get<bv>(v)))
			: std::variant<bv, Bool>(normalize_bool(std::get<Bool>(v)));
	}

	static tref simplify_symbol(tref symbol) {
		auto ba_type = tau::get(symbol).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
	}

	static tref simplify_term(tref term) {
		auto ba_type = tau::get(term).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
	}
};


template <>
inline std::optional<ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, sbf_ba>>(type_tree))
		return parse_bv<bv, sbf_ba>(constant_source, type_tree);
	return parse_sbf<bv, sbf_ba>(constant_source);
}

template <>
inline std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (is_bv_type_family<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source, type_tree);
	return parse_tau<bv, sbf_ba>(constant_source);
}

template <>
std::optional<typename ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,
		[[maybe_unused]] tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (constant_source == "1" || constant_source == "true")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(true), bool_type<node<bv, Bool>>() };
	if (constant_source == "0" || constant_source == "false")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(false), bool_type<node<bv, Bool>>() };
	return {};
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__