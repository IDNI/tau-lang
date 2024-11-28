// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt
#ifndef __BOOL_BA_H__
#define __BOOL_BA_H__

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
	auto operator<=>(const Bool& x) const = default;

	bool is_zero() const;
	bool is_one() const;

	bool b;
};

Bool normalize (const Bool& b);

std::ostream& operator<<(std::ostream& os, const Bool& b);

#endif // __BOOL_BA_H__
