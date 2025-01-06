// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TAU_BA_H__
#define __TAU_BA_H__

#include <iostream>

#include "satisfiability.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of tau,...

namespace idni::tau_lang {

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template <typename...BAs>
struct tau_ba {
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau<tau_ba_t, BAs...>;
	tau_ba(rules<tau_nso_t>& rec_relations, tau_nso_t& main);
	tau_ba(tau_nso_t& main);
	auto operator<=>(const tau_ba_t&) const;
	tau_ba_t operator~() const;
	tau_ba_t operator&(const tau_ba_t& other) const;
	tau_ba_t operator|(const tau_ba_t& other) const;
	tau_ba_t operator+(const tau_ba_t& other) const;
	tau_ba_t operator^(const tau_ba_t& other) const;
	bool is_zero() const;
	bool is_one() const;
	// the type is equivalent to tau_spec<BAs...>
	const rr<tau_nso_t> nso_rr;
private:
	tau_nso_t rename(const tau_nso_t& form) const;
	rewriter::rule<rr<tau_nso_t>> rename(
				const rewriter::rule<tau_nso_t>& rule) const;
	rules<tau_nso_t> merge(const rules<tau_nso_t>& rs1,
				const rules<tau_nso_t>& rs2) const;
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

template <typename... BAs>
using tau_nso = tau<tau_ba<BAs...>, BAs...>;

template <typename...BAs>
using tau_spec = rr<tau_nso<BAs...>>;

template <typename...BAs>
struct tau_ba_factory {
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau_nso<BAs...>;
	std::optional<tau_nso_t> parse(const std::string& src);
	tau_nso_t binding(const tau_nso_t& n);
	std::variant<tau_ba_t, BAs...> splitter_one() const;
	std::string one(std::string&) const;
	std::string zero(std::string&) const;
};

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
template <typename... BAs>
struct std::hash<idni::tau_lang::tau_ba<BAs...>> {
	size_t operator()(const idni::tau_lang::tau_ba<BAs...>& f) const noexcept {
		using namespace idni::tau_lang;
		return hash<rr<tau<tau_ba<BAs...>, BAs...>>>{}(f.nso_rr);
	}
};

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau_lang::tau_ba<BAs...>& rs);

#include "tau_ba.tmpl.h"
#endif // __TAU_BA_H__