// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Explicit specializations of the base BA dispatcher and constant parser for
// the legacy <bv, sbf_ba> pack.
//
// These used to be defined `inline` in base_ba_dispatcher.tmpl.h, which every
// test translation unit includes.  Their bodies call parse_bv/parse_sbf/
// parse_tau over the pack, so merely *seeing* them instantiated the entire
// <bv, sbf_ba> pipeline in every TU -- measured with clang -ftime-trace at
// 40.6%% of all template-instantiation time in a TU that does not use the pack
// at all, and 1158 KB / 5972 emitted weak symbols per object.
//
// Only four targets actually use this pack (tests/test_sbf_ba_helpers.h and
// tests/integration/solver/test_integration-solver-sbf.cpp), so the
// definitions now live in one translation unit compiled into libTAU and every
// other TU sees declarations only.
//
// plugins_compile_time.md R3; plugins_infra.md R6 wants this glue deleted
// outright once the pack becomes a configure-time choice.

#include "tau.h"

namespace idni::tau_lang {

template <>
std::optional<ba_constants<node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::constant_with_type>
ba_constants<node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::get(
	const std::string& constant_source, tref type_tree,
	[[maybe_unused]] const std::string options)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree))
		return parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_qlt_type<node_t>(type_tree))
		return parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_nlang_type<node_t>(type_tree))
		return parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_bv_type_family<node_t>(type_tree))
		return parse_bv<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source, type_tree);
	if (is_hsb_type<node_t>(type_tree))
		return parse_hsb<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	return parse_sbf<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
}

template <>
std::optional<ba_constants<node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
	qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::constant_with_type>
ba_constants<node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
	qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::get(
	const std::string& constant_source, tref type_tree,
	[[maybe_unused]] const std::string options)
{
	using node_t = node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
		qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree))
		return parse_qint<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_qlt_type<node_t>(type_tree))
		return parse_qlt<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_nlang_type<node_t>(type_tree))
		return parse_nlang<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_sbf_type<node_t>(type_tree))
		return parse_sbf<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	if (is_bv_type_family<node_t>(type_tree))
		return parse_bv<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source, type_tree);
	if (is_hsb_type<node_t>(type_tree))
		return parse_hsb<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
			qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
	return parse_tau<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(constant_source);
}

template<>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	if (std::holds_alternative<qint>(elem))
		return is_qint_one(std::get<qint>(elem));
	if (std::holds_alternative<qlt>(elem))
		return is_qlt_one(std::get<qlt>(elem));
	if (std::holds_alternative<nlang_ba>(elem))
		return is_nlang_one(std::get<nlang_ba>(elem));
	if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_one(std::get<bv>(elem));
	if (std::holds_alternative<hsb>(elem))
		return is_hsb_one(std::get<hsb>(elem));
	return is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	if (std::holds_alternative<qint>(elem))
		return is_qint_zero(std::get<qint>(elem));
	if (std::holds_alternative<qlt>(elem))
		return is_qlt_zero(std::get<qlt>(elem));
	if (std::holds_alternative<nlang_ba>(elem))
		return is_nlang_zero(std::get<nlang_ba>(elem));
	if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_zero(std::get<bv>(elem));
	if (std::holds_alternative<hsb>(elem))
		return is_hsb_zero(std::get<hsb>(elem));
	return is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_one(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	return base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one(elem);
}

template<>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_zero(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	return base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero(elem);
}

template<>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_closed(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>&)
{
	return true;
}

template<>
std::vector<std::string> base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::types() {
	return { "sbf", "bv", "qint", "qlt", "nlang", "hsb" };
}

template<>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::default_type() {
	return sbf_type<node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>>();
}

template<>
std::string base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::one(
	const tref type_tree)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree)) return "top";
	if (is_qlt_type<node_t>(type_tree))   return "top";
	if (is_nlang_type<node_t>(type_tree)) return "everything";
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	if (is_hsb_type<node_t>(type_tree)) return "top";
	return "1";
}

template<>
std::string base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::zero(
	const tref type_tree)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree)) return "bot";
	if (is_qlt_type<node_t>(type_tree))   return "bot";
	if (is_nlang_type<node_t>(type_tree)) return "nothing";
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	if (is_hsb_type<node_t>(type_tree)) return "bot";
	return "0";
}

template<>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter_one(
	tref type_tree)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	using tau = tree<node_t>;
	if (is_qint_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qint_splitter_one()),
			qint_type<node_t>()));
	if (is_qlt_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qlt_splitter_one()),
			qlt_type<node_t>()));
	if (is_nlang_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(nlang_splitter_one()),
			nlang_type<node_t>()));
	if (is_sbf_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(sbf_splitter_one()),
			sbf_type<node_t>()));
	if (is_hsb_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(hsb_splitter_one()),
			hsb_type<node_t>()));
	return nullptr; // no splitter for bv
}

template<>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem,
	splitter_type st)
{
	if (std::holds_alternative<qint>(elem))
		return qint_splitter(std::get<qint>(elem), st);
	if (std::holds_alternative<qlt>(elem))
		return qlt_splitter(std::get<qlt>(elem), st);
	if (std::holds_alternative<nlang_ba>(elem))
		return nlang_splitter(std::get<nlang_ba>(elem), st);
	if (std::holds_alternative<hsb>(elem))
		return hsb_splitter(std::get<hsb>(elem), st);
	DBG(assert(!std::holds_alternative<bv>(elem));)
	return sbf_splitter(std::get<sbf_ba>(elem), st);
}

template<>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::unpack_tau_ba(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>&)
{
	return nullptr; // no tau_ba in this pack
}

template<>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::pack_tau_ba(tref)
{
	return {}; // no tau_ba in this pack
}

template<>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::normalize(
	const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v)
{
	if (std::holds_alternative<qint>(v))
		return normalize_qint(std::get<qint>(v));
	if (std::holds_alternative<qlt>(v))
		return normalize_qlt(std::get<qlt>(v));
	if (std::holds_alternative<nlang_ba>(v))
		return normalize_nlang(std::get<nlang_ba>(v));
	if (std::holds_alternative<bv>(v))
		return normalize_bv(std::get<bv>(v));
	if (std::holds_alternative<hsb>(v))
		return normalize_hsb(std::get<hsb>(v));
	return normalize_sbf(std::get<sbf_ba>(v));
}

template<>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_symbol(
	tref symbol)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_term(
	tref term)
{
	using node_t = node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_term<node_t>(term) : term;
}

template<>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	if (std::holds_alternative<tau_dqnbv>(elem))
		return std::get<tau_dqnbv>(elem).is_one();
	if (std::holds_alternative<qint>(elem))
		return is_qint_one(std::get<qint>(elem));
	if (std::holds_alternative<qlt>(elem))
		return is_qlt_one(std::get<qlt>(elem));
	if (std::holds_alternative<nlang_ba>(elem))
		return is_nlang_one(std::get<nlang_ba>(elem));
	if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_one(std::get<bv>(elem));
	if (std::holds_alternative<hsb>(elem))
		return is_hsb_one(std::get<hsb>(elem));
	return is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	if (std::holds_alternative<tau_dqnbv>(elem))
		return std::get<tau_dqnbv>(elem).is_zero();
	if (std::holds_alternative<qint>(elem))
		return is_qint_zero(std::get<qint>(elem));
	if (std::holds_alternative<qlt>(elem))
		return is_qlt_zero(std::get<qlt>(elem));
	if (std::holds_alternative<nlang_ba>(elem))
		return is_nlang_zero(std::get<nlang_ba>(elem));
	if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_zero(std::get<bv>(elem));
	if (std::holds_alternative<hsb>(elem))
		return is_hsb_zero(std::get<hsb>(elem));
	return is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_one(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	return base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one(elem);
}

template<>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_zero(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	return base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero(elem);
}

template<>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_closed(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem)
{
	return std::holds_alternative<tau_dqnbv>(elem)
		? is_tau_closed<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(std::get<tau_dqnbv>(elem))
		: true;
}

template<>
std::vector<std::string> base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::types() {
	return { "sbf", "tau", "bv", "qint", "qlt", "nlang", "hsb" };
}

template<>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::default_type() {
	return tau_type<node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>>();
}

template<>
std::string base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::one(
	tref type_tree)
{
	using node_t = node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree)) return "top";
	if (is_qlt_type<node_t>(type_tree))   return "top";
	if (is_nlang_type<node_t>(type_tree)) return "everything";
	if (is_sbf_type<node_t>(type_tree))   return "1";
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	if (is_hsb_type<node_t>(type_tree)) return "top";
	return "T";
}

template<>
std::string base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::zero(
	tref type_tree)
{
	using node_t = node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	if (is_qint_type<node_t>(type_tree)) return "bot";
	if (is_qlt_type<node_t>(type_tree))   return "bot";
	if (is_nlang_type<node_t>(type_tree)) return "nothing";
	if (is_sbf_type<node_t>(type_tree))   return "0";
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	if (is_hsb_type<node_t>(type_tree)) return "bot";
	return "F";
}

template<>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter_one(
	tref type_tree)
{
	using node_t = node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	using tau = tree<node_t>;
	DBG(assert(!is_bv_type_family<node_t>(type_tree)));
	if (is_qint_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qint_splitter_one()),
			qint_type<node_t>()));
	if (is_qlt_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qlt_splitter_one()),
			qlt_type<node_t>()));
	if (is_nlang_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(nlang_splitter_one()),
			nlang_type<node_t>()));
	if (is_sbf_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(sbf_splitter_one()),
			sbf_type<node_t>()));
	if (is_hsb_type<node_t>(type_tree))
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(hsb_splitter_one()),
			hsb_type<node_t>()));
	return tau::get(tau::bf, tau::get_ba_constant(
		typename tau::constant(tau_splitter_one<qint, qlt, nlang_ba, bv, sbf_ba, hsb>()),
		tau_type<node_t>()));
}

template<>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::unpack_tau_ba(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v)
{
	if (!std::holds_alternative<tau_dqnbv>(v)) return {};
	return std::get<tau_dqnbv>(v).nso_rr.main->get();
}

template<>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem,
	splitter_type st)
{
	DBG(assert(!std::holds_alternative<bv>(elem));)
	if (std::holds_alternative<tau_dqnbv>(elem)) {
		auto unpacked = unpack_tau_ba(elem);
		return tau_splitter<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>(unpacked, st);
	}
	if (std::holds_alternative<qint>(elem))
		return qint_splitter(std::get<qint>(elem), st);
	if (std::holds_alternative<qlt>(elem))
		return qlt_splitter(std::get<qlt>(elem), st);
	if (std::holds_alternative<nlang_ba>(elem))
		return nlang_splitter(std::get<nlang_ba>(elem), st);
	if (std::holds_alternative<hsb>(elem))
		return hsb_splitter(std::get<hsb>(elem), st);
	return sbf_splitter(std::get<sbf_ba>(elem), st);
}

template<>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::pack_tau_ba(tref c)
{
	tau_dqnbv t{c};
	return {t};
}

template<>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>
base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::normalize(
	const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v)
{
	if (std::holds_alternative<tau_dqnbv>(v))
		return normalize_tau(std::get<tau_dqnbv>(v));
	if (std::holds_alternative<qint>(v))
		return normalize_qint(std::get<qint>(v));
	if (std::holds_alternative<qlt>(v))
		return normalize_qlt(std::get<qlt>(v));
	if (std::holds_alternative<nlang_ba>(v))
		return normalize_nlang(std::get<nlang_ba>(v));
	if (std::holds_alternative<bv>(v))
		return normalize_bv(std::get<bv>(v));
	if (std::holds_alternative<hsb>(v))
		return normalize_hsb(std::get<hsb>(v));
	return normalize_sbf(std::get<sbf_ba>(v));
}

template<>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_symbol(
	tref symbol)
{
	using node_t = node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_term(
	tref term)
{
	using node_t = node<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_term<node_t>(term) : term;
}

} // namespace idni::tau_lang
