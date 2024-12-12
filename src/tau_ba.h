// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TAU_BA_H__
#define __TAU_BA_H__

#include <iostream>

#include "satisfiability.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template <typename...BAs>
struct tau_ba;

// template <typename...BAs>
// bool is_satisfiable(const rr<nso<tau_ba<BAs...>, BAs...>>& tau_spec);

template <typename...BAs>
struct tau_ba {
	tau_ba(rules<nso<tau_ba<BAs...>, BAs...>>& rec_relations,
		nso<tau_ba<BAs...>, BAs...>& main);
	tau_ba(nso<tau_ba<BAs...>, BAs...>& main);
	auto operator<=>(const tau_ba<BAs...>&) const;
	tau_ba<BAs...> operator~() const;
	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const;
	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const;
	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const;
	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const;
	bool is_zero() const;
	bool is_one() const;
	// the type is ewquivalent to tau_spec<BAs...>
	const rr<nso<tau_ba<BAs...>, BAs...>> nso_rr;
private:
	nso<tau_ba<BAs...>, BAs...> rename(
		const nso<tau_ba<BAs...>, BAs...>& form) const;
	rewriter::rule<rr<nso<tau_ba<BAs...>, BAs...>>> rename(
		const rewriter::rule<nso<tau_ba<BAs...>, BAs...>>& rule) const;
	rules<nso<tau_ba<BAs...>, BAs...>> merge(
		const rules<nso<tau_ba<BAs...>, BAs...>>& rs1,
		const rules<nso<tau_ba<BAs...>, BAs...>>& rs2) const;
};

template <typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b);

template <typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other);

template <typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b);

template <typename...BAs>
bool operator!=(const bool& b, const tau_ba<BAs...>& other);

template<typename... BAs>
auto normalize (const tau_ba<BAs...>& fm);

template<typename... BAs>
auto splitter (const tau_ba<BAs...>& fm, splitter_type st);

template<typename... BAs>
auto tau_splitter_one();

template<typename... BAs>
bool is_closed (const tau_ba<BAs...>& fm);

template <typename...BAs>
using gssotc = nso<tau_ba<BAs...>, BAs...>;

template <typename...BAs>
using tau_spec = rr<gssotc<BAs...>>;

template <typename...BAs>
struct tau_ba_factory {
	std::optional<gssotc<BAs...>> parse(const std::string& src);
	gssotc<BAs...> binding(const gssotc<BAs...>& n);
	std::variant<tau_ba<BAs...>, BAs...> splitter_one() const;
	std::string one(std::string&) const;
	std::string zero(std::string&) const;
};

} // namespace idni::tau

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs);

#include "tau_ba.tmpl.h"
#endif // __TAU_BA_H__