// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__UTILS_H__
#define __IDNI__TAU__UTILS_H__

namespace idni::tau_lang {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace idni::tau_lang

namespace idni::tau_lang_depreciating {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace idni::tau_lang_depreciating

#endif // __IDNI__TAU__UTILS_H__