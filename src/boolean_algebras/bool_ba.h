// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt
#ifndef __BOOL_BA_H__
#define __BOOL_BA_H__

#include <ostream>

#include "defs.h"
#include "nso_rr.h"

namespace idni::tau_lang {

struct Bool {
	Bool();
	Bool(bool b);

	static const Bool& zero();
	static const Bool& one();

	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator^(const Bool& x) const;
	Bool operator+(const Bool& x) const;
	Bool operator~() const;

	bool is_zero() const;
	bool is_one() const;

	bool b;
};

std::strong_ordering operator<=>(const Bool& x, const Bool& y);
bool operator==(const Bool& x, const Bool& y);
bool operator!=(const Bool& x, const Bool& y);
bool operator==(const Bool& x, const bool& b);
bool operator==(const bool& b, const Bool& x);
bool operator!=(const Bool& x, const bool& b);
bool operator!=(const bool& b, const Bool& x);

Bool normalize(const Bool& b);
bool is_syntactic_one(const Bool& b);
bool is_syntactic_zero(const Bool& b);


template<typename...BAs>
struct bool_ba_factory {

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

template<>
struct std::hash<idni::tau_lang::Bool> {
	size_t operator()(const idni::tau_lang::Bool& b) {
		return b.b ? 1 : 0;
	}
};

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::Bool& b);

#include "bool_ba.tmpl.h"
#endif // __BOOL_BA_H__
