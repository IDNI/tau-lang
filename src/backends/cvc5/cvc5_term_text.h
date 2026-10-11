// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// A cvc5 term written as text and built again from it, so a question can
// reach another process of the same program. Only terms over Booleans and
// bit-vectors are written; the text is not meant for any other reader.

#ifndef __IDNI__TAU__BACKENDS__CVC5_TERM_TEXT_H__
#define __IDNI__TAU__BACKENDS__CVC5_TERM_TEXT_H__

#include <charconv>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "backends/cvc5/cvc5.h"

namespace idni::tau_lang {

/**
 * @brief Writes @p t as text that cvc5_term_from_text builds again.
 *
 * One record per distinct term, each child before its parent: the kind,
 * the sort, the payload (a symbol, a value or operator indices) and the
 * indices of the child records.
 * @param t The term.
 * @return The text, or nullopt when @p t holds a sort other than Boolean or
 * bit-vector, or an operator index that is not a 32-bit number.
 */
inline std::optional<std::string> cvc5_term_to_text(const cvc5::Term& t) {
	std::unordered_map<uint64_t, size_t> index;
	std::vector<cvc5::Term> order;
	std::vector<std::pair<cvc5::Term, bool>> stack{ { t, false } };
	while (!stack.empty()) {
		auto [x, expanded] = stack.back();
		stack.pop_back();
		if (index.contains(x.getId())) continue;
		if (!expanded) {
			stack.push_back({ x, true });
			for (size_t i = x.getNumChildren(); i-- > 0; )
				stack.push_back({ x[i], false });
			continue;
		}
		index.emplace(x.getId(), order.size());
		order.push_back(x);
	}
	std::ostringstream os;
	os << order.size() << '\n';
	for (const auto& x : order) {
		const auto k = x.getKind();
		os << static_cast<int64_t>(k) << ' ';
		const cvc5::Sort s = x.getSort();
		if (s.isBoolean()) os << 'b';
		else if (s.isBitVector()) os << 'v' << s.getBitVectorSize();
		else if (k == cvc5::Kind::VARIABLE_LIST) os << 'n';
		else return std::nullopt;
		os << ' ';
		if (k == cvc5::Kind::CONSTANT || k == cvc5::Kind::VARIABLE) {
			const std::string sym = x.hasSymbol() ? x.getSymbol() : "";
			// the length goes first, so a symbol may hold any byte
			os << 's' << sym.size() << ' ' << sym;
		} else if (k == cvc5::Kind::CONST_BITVECTOR)
			os << x.getBitVectorValue(2);
		else if (k == cvc5::Kind::CONST_BOOLEAN)
			os << (x.getBooleanValue() ? '1' : '0');
		else if (x.hasOp() && x.getOp().isIndexed()) {
			cvc5::Op op = x.getOp();
			os << 'i' << op.getNumIndices();
			for (size_t i = 0; i < op.getNumIndices(); ++i) {
				const cvc5::Term v = op[i];
				if (!v.isUInt32Value()) return std::nullopt;
				os << ',' << v.getUInt32Value();
			}
		} else os << '-';
		os << ' ' << x.getNumChildren();
		for (size_t i = 0; i < x.getNumChildren(); ++i)
			os << ' ' << index.at(x[i].getId());
		os << '\n';
	}
	return os.str();
}

/**
 * @brief Builds the term that cvc5_term_to_text wrote as @p text, in
 * `cvc5_term_manager`.
 * @param text Text of cvc5_term_to_text, from a process of the same program.
 * @return The term, or nullopt when @p text is not such text.
 */
inline std::optional<cvc5::Term> cvc5_term_from_text(const std::string& text) {
	auto& tm = cvc5_term_manager;
	auto number = [](const std::string& w) -> std::optional<uint32_t> {
		uint32_t v = 0;
		auto [p, ec] = std::from_chars(w.data(), w.data() + w.size(), v);
		if (ec != std::errc{} || p != w.data() + w.size())
			return std::nullopt;
		return v;
	};
	std::istringstream in(text);
	size_t count = 0;
	if (!(in >> count) || count == 0) return std::nullopt;
	std::vector<cvc5::Term> built;
	built.reserve(count);
	for (size_t r = 0; r < count; ++r) {
		int64_t kind_value = 0;
		std::string sort, payload;
		if (!(in >> kind_value >> sort >> payload)) return std::nullopt;
		const auto k = static_cast<cvc5::Kind>(kind_value);
		cvc5::Sort s;
		if (sort == "b") s = tm.getBooleanSort();
		else if (sort[0] == 'v') {
			auto w = number(sort.substr(1));
			if (!w || *w == 0) return std::nullopt;
			s = tm.mkBitVectorSort(static_cast<uint32_t>(*w));
		} else if (sort != "n") return std::nullopt;
		std::string sym;
		if (payload[0] == 's') {
			auto len_value = number(payload.substr(1));
			if (!len_value) return std::nullopt;
			const size_t len = static_cast<size_t>(*len_value);
			in.get();
			sym.resize(len);
			if (len && !in.read(sym.data(),
				static_cast<std::streamsize>(len))) return std::nullopt;
		}
		size_t n = 0;
		if (!(in >> n)) return std::nullopt;
		std::vector<cvc5::Term> ch(n);
		for (auto& c : ch) {
			size_t i = 0;
			if (!(in >> i) || i >= built.size()) return std::nullopt;
			c = built[i];
		}
		if (k == cvc5::Kind::CONSTANT || k == cvc5::Kind::VARIABLE) {
			if (s.isNull()) return std::nullopt;
			auto name = sym.empty() ? std::nullopt
				: std::optional<std::string>(sym);
			built.push_back(k == cvc5::Kind::CONSTANT
				? tm.mkConst(s, name) : tm.mkVar(s, name));
		} else if (k == cvc5::Kind::CONST_BITVECTOR) {
			if (!s.isBitVector()) return std::nullopt;
			built.push_back(tm.mkBitVector(s.getBitVectorSize(),
				payload, 2));
		} else if (k == cvc5::Kind::CONST_BOOLEAN)
			built.push_back(tm.mkBoolean(payload == "1"));
		else if (payload[0] == 'i') {
			std::vector<uint32_t> idx;
			std::istringstream ps(payload.substr(1));
			std::string part;
			std::getline(ps, part, ',');
			auto m = number(part);
			if (!m) return std::nullopt;
			for (uint32_t i = 0; i < *m; ++i) {
				auto v = std::getline(ps, part, ',')
					? number(part) : std::nullopt;
				if (!v) return std::nullopt;
				idx.push_back(*v);
			}
			built.push_back(tm.mkTerm(tm.mkOp(k, idx), ch));
		} else built.push_back(tm.mkTerm(k, ch));
	}
	return built.back();
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__CVC5_TERM_TEXT_H__
