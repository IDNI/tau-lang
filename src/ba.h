// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#include <tuple>

// Infraestructure required for the product boolean algebra
namespace details
{
    template<typename Int, typename, Int Begin, bool Increasing>
    struct integer_range_impl;
 
    template<typename Int, Int... N, Int Begin>
    struct integer_range_impl<Int, std::integer_sequence<Int, N...>, Begin, true> {
        using type = std::integer_sequence<Int, N+Begin...>;
    };
 
    template<typename Int, Int... N, Int Begin>
    struct integer_range_impl<Int, std::integer_sequence<Int, N...>, Begin, false> {
        using type = std::integer_sequence<Int, Begin-N...>;
    };
}
 
template<typename Int, Int Begin, Int End>
using integer_range = typename details::integer_range_impl<
    Int, std::make_integer_sequence<Int, (Begin<End) ? End-Begin : Begin-End>,
    Begin, (Begin<End) >::type;
 
template<std::size_t Begin, std::size_t End>
using index_range = integer_range<std::size_t, Begin, End>;
 
template<size_t SN, size_t DN, class TSrc, class TDest, class Func>
void __assign(TSrc&& source, TDest& target, Func f)
{
    std::get<DN>(target) = f(std::get<SN>(std::forward<TSrc>(source)));	
}
 
template<size_t To, class TSrc, class TDest, class Func, size_t...Is, size_t...DIs>
void __neg(TSrc&& source, TDest& target, Func f, std::index_sequence<Is...>, std::index_sequence<DIs...>)
{
    using expander = int[];
    (void)expander { 0, (__assign<Is,DIs>(std::forward<TSrc>(source),target,f), 0)... };
}
 
template<size_t To, class TSrc, class TDest, class Func>
void __neg(TSrc&& source, TDest& target, Func f)
{
    __neg<To>(std::forward<TSrc>(source), target, f,
                    index_range<0,To>(), index_range<0, To>());
}

template<size_t SN, size_t DN, class TSrc, class TDest, class Func>
void __assign(TSrc&& source, TSrc&& sourceB, TDest& target, Func f)
{
    std::get<DN>(target) = f(std::get<SN>(std::forward<TSrc>(source)), std::get<SN>(std::forward<TSrc>(sourceB)));	
}
 
template<size_t To, class TSrc, class TDest, class Func, size_t...Is, size_t...DIs>
void __and(TSrc&& source, TSrc&& sourceB, TDest& target, Func f, std::index_sequence<Is...>, std::index_sequence<DIs...>)
{
    using expander = int[];
    (void)expander { 0, (__assign<Is,DIs>(std::forward<TSrc>(source), std::forward<TSrc>(sourceB),target,f), 0)... };
}
 
template<size_t To, class TSrc, class TDest, class Func>
void __and(TSrc&& source, TSrc&& sourceB, TDest& target, Func f)
{
    __and<To>(std::forward<TSrc>(source), std::forward<TSrc>(sourceB), target, f,
                    index_range<0,To>(), index_range<0, To>());
}

// The product boolean algebra
template <typename...BAS>
struct ba_product: std::tuple<BAS...> {

	template <typename... OBAS> 
	friend ba_product<OBAS...> operator!(ba_product<OBAS...>& thiz);
	template <typename... OBAS> 
	friend ba_product<OBAS...> operator&(ba_product<OBAS...>& thiz, ba_product<OBAS...>& that);

	ba_product(): std::tuple<BAS...>() {}
	ba_product(BAS... bas): std::tuple<BAS...>(bas...) {}

	auto operator<=>(const ba_product<BAS...>& that) const = default;
};

template <typename... BAS>
ba_product<BAS...> operator!(ba_product<BAS...>& thiz) {
	ba_product<BAS...> result;
	__neg<sizeof...(BAS)>(thiz, result, [](auto&& a) { return !a; });
	return result;
}

template <typename... BAS>
ba_product<BAS...> operator&(ba_product<BAS...>& thiz, ba_product<BAS...>& that) {
	ba_product<BAS...> result;
	__and<sizeof...(BAS)>(thiz, that, result, [](auto&& a, auto&& b) { return a & b; });
	return result;
}
