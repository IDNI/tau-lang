// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BITVECTOR_BA_H__
#define __BITVECTOR_BA_H__

#include <z3++.h>

#include "../../parser/z3_parser.generated.h"
#include "../nso_rr.h"

namespace idni::tau_lang {

using z3_ba = std::shared_ptr<z3::expr>;
using z3_source_sym = idni::lit<char, char>;
using z3_sym = std::variant<z3_source_sym, z3_ba>;

static z3::context ctx;

z3_ba operator&(const z3_ba& x, const z3_ba& y);
z3_ba operator|(const z3_ba& x, const z3_ba& y);
z3_ba operator^(const z3_ba& x, const z3_ba& y);
z3_ba operator+(const z3_ba& x, const z3_ba& y);
z3_ba operator~(const z3_ba& x);


std::strong_ordering operator<=>(const z3_ba& x, const z3_ba& y);
bool operator==(const z3_ba& x, const z3_ba& y);
bool operator!=(const z3_ba& x, const z3_ba& y);
bool operator==(const z3_ba& x, const bool& b);
bool operator==(const bool& b, const z3_ba& x);
bool operator!=(const z3_ba& x, const bool& b);
bool operator!=(const bool& b, const z3_ba& x);

z3_ba normalize(const z3_ba& /*b*/);
bool is_syntactic_one(const z3_ba& b);
bool is_syntactic_zero(const z3_ba& b);
bool is_closed(const z3_ba& x);

template<typename...BAs>
struct z3_ba_factory {

	/**
	 * @brief parses a bitvector from a string
	 *
	 * @param src source string
	 * @return optional parsed node if parsing successful
	 */
	std::optional<tau<BAs...>> parse(const std::string& src);

	/**
	 * @brief builds a bitvector bounded node from a parsed terminals of a source binding
	 *
	 * @param sn tau code node with parsed bitvector
	 * @return bounded constant
	 */
	tau<BAs...> binding(const tau<BAs...>& sn);

	std::variant<BAs...> splitter_one () const;

	std::string one() const;

	std::string zero() const;
private:

	inline static std::map<std::string, tau<BAs...>> cache;
};

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
template<>
struct std::hash<idni::tau_lang::z3_ba> {
	size_t operator()(const idni::tau_lang::z3_ba& f) const noexcept;
};

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::z3_ba& x);

#include "z3_ba.tmpl.h"
#endif // __BITVECTOR_BA_H__