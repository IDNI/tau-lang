// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

Bool::Bool() : b(false) {}
Bool::Bool(bool b) : b(b) {}

const Bool& Bool::zero() { static Bool b(false); return b; }
const Bool& Bool::one() { static Bool b(true); return b; }

Bool Bool::operator&(const Bool& x) const {
	return (this->b == false) ? zero() : x;
}

Bool Bool::operator|(const Bool& x) const {
	return (this->b == true) ? one() : x;
}

Bool Bool::operator^(const Bool& x) const {
	return (this->b == true) ? ~x : x;
}

Bool Bool::operator+(const Bool& x) const {
	return (this->b == true) ? ~x : x;
}

Bool Bool::operator~() const {
	return (this->b == true) ? zero() : one();
}

bool Bool::is_zero() const {
	return !b;
}

bool Bool::is_one() const {
	return b;
}

std::strong_ordering operator<=>(const Bool& x, const Bool& y) { return x.b <=> y.b; }
bool operator==(const Bool& x, const Bool& y) { return x.b == y.b; }
bool operator!=(const Bool& x, const Bool& y) { return x.b != y.b; }
bool operator==(const Bool& x, const bool& b) { return x.b == b; }
bool operator==(const bool& b, const Bool& x) { return x.b == b; }
bool operator!=(const Bool& x, const bool& b) { return x.b != b; }
bool operator!=(const bool& b, const Bool& x) { return x.b != b; }

template <typename...BAs>
std::optional<tau<BAs...>> bool_ba_factory<BAs...>::parse(
	const std::string& /*src*/)
{
	// TODO implement
	return {};
}

template <typename...BAs>
tau<BAs...> bool_ba_factory<BAs...>::binding(const tau<BAs...>& /*sn*/) {
	// TODO implement
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> bool_ba_factory<BAs...>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

Bool normalize(const Bool &b) {
	return b;
}

bool is_syntactic_one(const Bool& b) {
	return b.is_one();
}

bool is_syntactic_zero(const Bool& b) {
	return b.is_zero();
}

} // namespace idni::tau_lang

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::Bool& b) {
	return os << (b.b ? 1 : 0);
}
