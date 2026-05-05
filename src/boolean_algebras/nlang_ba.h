// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG_BA_H__

#include <compare>
#include <string>
#include <optional>
#include <functional>
#include <memory>

#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// nlang_ba — Natural Language Boolean Algebra (Lindenbaum-Tarski algebra)
//
// Elements are natural language propositions/statements represented as
// structural formula trees. The logical engine catches contradictions and
// tautologies structurally — A & ~A = bottom, A | ~A = top, ~~A = A —
// without any LLM calls. The DeepSeek oracle is only invoked on atomic
// propositions (leaf nodes of the formula tree).
//
// This minimises LLM mistakes: compound formulas built by the engine are
// simplified algebraically; only irreducible atoms need semantic judgement.
//
// Top = any tautology (canonical: "everything"), Bottom = any contradiction
// (canonical: "nothing"). The algebra is atomless: between any two distinct
// propositions p < q there always exists r with p < r < q.
// -----------------------------------------------------------------------------

// --- DeepSeek API helpers (implemented in nlang_ba.cpp, linked via libTAU) ---
std::string deepseek_query(const std::string& prompt);
bool deepseek_is_empty(const std::string& description);
bool deepseek_is_universal(const std::string& description);
bool deepseek_equivalent(const std::string& a, const std::string& b);
std::string deepseek_stronger_statement(const std::string& description);
// deepseek_decompose declared after nlang_ba struct (return type needs nlang_ba::fptr)

// -----------------------------------------------------------------------------
// nlang_ba struct
// -----------------------------------------------------------------------------

struct nlang_ba {
	// --- Structural formula tree ---
	struct formula {
		using fptr = std::shared_ptr<formula>;
		enum class kind : uint8_t { bot, top, atom, and_, or_, not_ };

		kind k = kind::bot;
		std::string atom_str;  // only for kind::atom
		fptr lhs, rhs;         // for kind::and_, kind::or_
		fptr inner;            // for kind::not_

		static fptr mk_bot() {
			auto p = std::make_shared<formula>(); p->k = kind::bot; return p;
		}
		static fptr mk_top() {
			auto p = std::make_shared<formula>(); p->k = kind::top; return p;
		}
		static fptr mk_atom(std::string s) {
			auto p = std::make_shared<formula>();
			p->k = kind::atom; p->atom_str = std::move(s); return p;
		}
		static fptr mk_and(fptr l, fptr r) {
			auto p = std::make_shared<formula>();
			p->k = kind::and_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
		}
		static fptr mk_or(fptr l, fptr r) {
			auto p = std::make_shared<formula>();
			p->k = kind::or_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
		}
		static fptr mk_not(fptr i) {
			auto p = std::make_shared<formula>();
			p->k = kind::not_; p->inner = std::move(i); return p;
		}

		std::string to_string() const {
			switch (k) {
			case kind::bot:  return "nothing";
			case kind::top:  return "everything";
			case kind::atom: return atom_str;
			case kind::not_: return "not (" + inner->to_string() + ")";
			case kind::and_: return "(" + lhs->to_string() + ") and (" + rhs->to_string() + ")";
			case kind::or_:  return "(" + lhs->to_string() + ") or (" + rhs->to_string() + ")";
			}
			return "";
		}

		bool struct_eq(const formula& o) const {
			if (k != o.k) return false;
			switch (k) {
			case kind::bot:
			case kind::top:  return true;
			case kind::atom: return atom_str == o.atom_str;
			case kind::not_: return inner->struct_eq(*o.inner);
			case kind::and_:
			case kind::or_:  return lhs->struct_eq(*o.lhs) && rhs->struct_eq(*o.rhs);
			}
			return false;
		}

		// true iff one formula is syntactically the negation of the other
		bool is_complement_of(const formula& o) const {
			if (k == kind::not_ && inner->struct_eq(o)) return true;
			if (o.k == kind::not_ && o.inner->struct_eq(*this)) return true;
			return false;
		}
	};
	using fptr = formula::fptr;

	fptr fm;

	nlang_ba() : fm(formula::mk_bot()) {}
	explicit nlang_ba(std::string s) {
		if (s == "nothing")         fm = formula::mk_bot();
		else if (s == "everything") fm = formula::mk_top();
		else                        fm = formula::mk_atom(std::move(s));
	}

	static nlang_ba bottom() { return nlang_ba{"nothing"}; }
	static nlang_ba top()    { return nlang_ba{"everything"}; }
	static nlang_ba from_string(const std::string& s) { return nlang_ba{s}; }

	// Construct from a pre-built formula tree (used by parse_nlang after decompose).
	static nlang_ba from_fm(fptr f) { nlang_ba r; r.fm = std::move(f); return r; }

	// --- BA operations with structural simplifications ---
	// The engine catches A & ~A = bottom, A | ~A = top, ~~A = A without LLM calls.

	nlang_ba operator|(const nlang_ba& o) const {
		if (fm->k == formula::kind::bot) return o;
		if (o.fm->k == formula::kind::bot) return *this;
		if (fm->k == formula::kind::top || o.fm->k == formula::kind::top) return top();
		if (fm->struct_eq(*o.fm)) return *this;               // A | A = A
		if (fm->is_complement_of(*o.fm)) return top();         // A | ~A = top
		nlang_ba r; r.fm = formula::mk_or(fm, o.fm); return r;
	}

	nlang_ba operator&(const nlang_ba& o) const {
		if (fm->k == formula::kind::bot || o.fm->k == formula::kind::bot) return bottom();
		if (fm->k == formula::kind::top) return o;
		if (o.fm->k == formula::kind::top) return *this;
		if (fm->struct_eq(*o.fm)) return *this;               // A & A = A
		if (fm->is_complement_of(*o.fm)) return bottom();      // A & ~A = bottom
		nlang_ba r; r.fm = formula::mk_and(fm, o.fm); return r;
	}

	nlang_ba operator~() const {
		if (fm->k == formula::kind::bot) return top();
		if (fm->k == formula::kind::top) return bottom();
		if (fm->k == formula::kind::not_) { nlang_ba r; r.fm = fm->inner; return r; } // ~~A = A
		nlang_ba r; r.fm = formula::mk_not(fm); return r;
	}

	nlang_ba operator^(const nlang_ba& o) const {
		return (*this | o) & ~(*this & o);
	}

	// Semantic equality: fast structural path first, then oracle
	bool operator==(const nlang_ba& o) const {
		if (fm->struct_eq(*o.fm)) return true;
		return deepseek_equivalent(to_string(), o.to_string());
	}
	bool operator!=(const nlang_ba& o) const { return !(*this == o); }

	// Fast structural check only (no oracle) — required by BA dispatcher
	bool operator==(bool b) const {
		return b ? (fm->k == formula::kind::top) : (fm->k == formula::kind::bot);
	}
	bool operator!=(bool b) const { return !(*this == b); }

	bool operator<(const nlang_ba& o) const { return to_string() < o.to_string(); }
	auto operator<=>(const nlang_ba& o) const { return to_string() <=> o.to_string(); }

	std::string to_string() const { return fm->to_string(); }
};

// --- stream output ---
inline std::ostream& operator<<(std::ostream& os, const nlang_ba& n) {
	return os << n.to_string();
}

// Decompose a natural language string into a structural formula tree via DeepSeek.
// Returns mk_atom(s) as fallback when the API is unavailable or response is malformed.
// Declared here (after nlang_ba) because the return type is nlang_ba::fptr.
nlang_ba::fptr deepseek_decompose(const std::string& s);

// --- Mutually recursive structural emptiness/universality checks ---
// Oracle is only called on atomic leaf nodes.
// and_-emptiness and or_-universality may fall back to oracle for non-structural cases.

// Forward declarations for mutual recursion
inline bool nlang_struct_is_empty(const nlang_ba::fptr& f);
inline bool nlang_struct_is_one(const nlang_ba::fptr& f);

inline bool nlang_struct_is_empty(const nlang_ba::fptr& f) {
	using K = nlang_ba::formula::kind;
	switch (f->k) {
	case K::bot:  return true;
	case K::top:  return false;
	case K::atom: return deepseek_is_empty(f->atom_str);
	case K::not_: return nlang_struct_is_one(f->inner);
	case K::and_:
		// A & B = bottom if A=bottom, B=bottom, A=~B, or semantically incompatible
		if (nlang_struct_is_empty(f->lhs) || nlang_struct_is_empty(f->rhs)) return true;
		if (f->lhs->is_complement_of(*f->rhs)) return true;
		return deepseek_is_empty(f->to_string());  // oracle fallback for non-structural cases
	case K::or_:
		// A | B = bottom iff both A=bottom and B=bottom (purely structural)
		return nlang_struct_is_empty(f->lhs) && nlang_struct_is_empty(f->rhs);
	}
	return false;
}

inline bool nlang_struct_is_one(const nlang_ba::fptr& f) {
	using K = nlang_ba::formula::kind;
	switch (f->k) {
	case K::top:  return true;
	case K::bot:  return false;
	case K::atom: return deepseek_is_universal(f->atom_str);
	case K::not_: return nlang_struct_is_empty(f->inner);
	case K::or_:
		// A | B = top if A=top, B=top, A=~B, or semantically universal
		if (nlang_struct_is_one(f->lhs) || nlang_struct_is_one(f->rhs)) return true;
		if (f->lhs->is_complement_of(*f->rhs)) return true;
		return deepseek_is_universal(f->to_string());  // oracle fallback for non-structural cases
	case K::and_:
		// A & B = top iff both A=top and B=top (purely structural)
		return nlang_struct_is_one(f->lhs) && nlang_struct_is_one(f->rhs);
	}
	return false;
}

// --- free functions expected by the dispatcher ---

inline bool is_nlang_zero(const nlang_ba& x) {
	return nlang_struct_is_empty(x.fm);
}

inline bool is_nlang_one(const nlang_ba& x) {
	return nlang_struct_is_one(x.fm);
}

// Normalize: reduce contradictions to bottom, tautologies to top.
// Structural checks are tried first; oracle only invoked on atoms or unresolved compounds.
inline nlang_ba normalize_nlang(const nlang_ba& x) {
	using K = nlang_ba::formula::kind;
	if (x.fm->k == K::bot || x.fm->k == K::top) return x;
	if (nlang_struct_is_empty(x.fm)) return nlang_ba::bottom();
	if (nlang_struct_is_one(x.fm))   return nlang_ba::top();
	return x;
}

inline tref simplify_nlang_symbol(tref sym) { return sym; }
inline tref simplify_nlang_term(tref t) { return t; }

// Splitter: return a logically stronger statement strictly implying x.
// For or_-formulas: the left operand is structurally stronger (A ≤ A|B).
// For other non-bottom formulas: delegate to oracle.
inline nlang_ba nlang_splitter(const nlang_ba& x, splitter_type /*st*/) {
	using K = nlang_ba::formula::kind;
	if (x.fm->k == K::bot) return nlang_ba::bottom();
	if (x.fm->k == K::or_) { nlang_ba r; r.fm = x.fm->lhs; return r; }
	return nlang_ba{deepseek_stronger_statement(x.to_string())};
}

// A fixed non-trivial contingent proposition (neither tautology nor contradiction).
inline nlang_ba nlang_splitter_one() {
	return nlang_ba{"it is raining"};
}

// --- parsing ---
// The source string is taken as-is as the natural language description.
// We strip surrounding braces or quotes if present.
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_nlang(
	const std::string& src)
{
	std::string s = src;
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	auto last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	// Strip surrounding braces
	if (!s.empty() && s.front() == '{' && s.back() == '}')
		s = s.substr(1, s.size() - 2);
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	// Strip surrounding quotes
	if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
		s = s.substr(1, s.size() - 2);
	else if (s.size() >= 2 && s.front() == '\'' && s.back() == '\'')
		s = s.substr(1, s.size() - 2);

	if (s.empty()) return {};

	// Decompose via DeepSeek so compound sentences (e.g. "A and B") become
	// structural and_(atom(A), atom(B)) trees that the engine can simplify.
	// Falls back to mk_atom(s) when the API is unavailable or response is malformed.
	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ nlang_ba::from_fm(deepseek_decompose(s)) },
		nlang_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

// Hash specialization for nlang_ba
template<>
struct std::hash<idni::tau_lang::nlang_ba> {
	size_t operator()(const idni::tau_lang::nlang_ba& n) const noexcept {
		return std::hash<std::string>{}(n.to_string());
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG_BA_H__
