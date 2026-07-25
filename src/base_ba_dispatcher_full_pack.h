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

#ifndef __IDNI__TAU__BASE_BA_DISPATCHER_FULL_PACK_H__
#define __IDNI__TAU__BASE_BA_DISPATCHER_FULL_PACK_H__

namespace idni::tau_lang {

// Same alias as in base_ba_dispatcher.tmpl.h.  Repeated here because these
// declarations are included before that file's copy; an identical alias
// redeclaration is well-formed.
using tau_dqnbv = tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;

template <>
std::optional<ba_constants<node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::constant_with_type> ba_constants<node<qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::get( const std::string& constant_source, tref type_tree, [[maybe_unused]] const std::string options);

template <>
std::optional<ba_constants<node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::constant_with_type> ba_constants<node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb>>::get( const std::string& constant_source, tref type_tree, [[maybe_unused]] const std::string options);

template <>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_one( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_zero( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_closed( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>&);

template <>
std::vector<std::string> base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::types();

template <>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::default_type();

template <>
std::string base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::one( const tref type_tree);

template <>
std::string base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::zero( const tref type_tree);

template <>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter_one( tref type_tree);

template <>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem, splitter_type st);

template <>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::unpack_tau_ba( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>&);

template <>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::pack_tau_ba(tref);

template <>
std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::normalize( const std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v);

template <>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_symbol( tref symbol);

template <>
tref base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_term( tref term);

template <>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_one( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_syntactic_zero( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_one( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_zero( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
bool base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::is_closed( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem);

template <>
std::vector<std::string> base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::types();

template <>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::default_type();

template <>
std::string base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::one( tref type_tree);

template <>
std::string base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::zero( tref type_tree);

template <>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter_one( tref type_tree);

template <>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::unpack_tau_ba( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v);

template <>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::splitter( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& elem, splitter_type st);

template <>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::pack_tau_ba(tref c);

template <>
std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb> base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::normalize( const std::variant<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>& v);

template <>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_symbol( tref symbol);

template <>
tref base_ba_dispatcher<tau_dqnbv, qint, qlt, nlang_ba, bv, sbf_ba, hsb>::simplify_term( tref term);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BASE_BA_DISPATCHER_FULL_PACK_H__
