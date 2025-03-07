// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BITVECTOR_BA_H__
#define __BITVECTOR_BA_H__

#include <z3++.h>

#include "../../parser/bitvector_parser.generated.h"
#include "../nso_rr.h"

namespace idni::tau_lang {

using bitvector_source_sym = idni::lit<char, char>;

struct bitvector_ba {

	enum type {
		i /* int */, u /* unsigned int */, l /* long */, ul /* usigned long */, b /* bits */
	};

	bitvector_ba() = delete;
	bitvector_ba(z3::expr e, const unsigned& size);
	bitvector_ba(int n, const unsigned& size);
	bitvector_ba(unsigned int n, const unsigned& size);
	bitvector_ba(long n, const unsigned& size);
	bitvector_ba(unsigned long n, const unsigned& size);

	bitvector_ba operator&(const bitvector_ba& x) const;
	bitvector_ba operator|(const bitvector_ba& x) const;
	bitvector_ba operator^(const bitvector_ba& x) const;
	bitvector_ba operator+(const bitvector_ba& x) const;
	bitvector_ba operator~() const;


	bool is_zero() const;
	bool is_one() const;

	unsigned size;
	z3::expr e;
};

std::strong_ordering operator<=>(const bitvector_ba& x, const bitvector_ba& y);
bool operator==(const bitvector_ba& x, const bitvector_ba& y);
bool operator!=(const bitvector_ba& x, const bitvector_ba& y);
bool operator==(const bitvector_ba& x, const bool& b);
bool operator==(const bool& b, const bitvector_ba& x);
bool operator!=(const bitvector_ba& x, const bool& b);
bool operator!=(const bool& b, const bitvector_ba& x);

bitvector_ba normalize(const bitvector_ba& /*b*/);
bool is_syntactic_one(const bitvector_ba& b);
bool is_syntactic_zero(const bitvector_ba& b);
bool is_closed(const bitvector_ba& x);

template<typename...BAs>
struct bitvector_ba_factory {

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
struct std::hash<idni::tau_lang::bitvector_ba> {
	size_t operator()(const idni::tau_lang::bitvector_ba& f) const noexcept;
};

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::bitvector_ba& x);

#include "bitvector_ba.tmpl.h"
#endif // __BITVECTOR_BA_H__