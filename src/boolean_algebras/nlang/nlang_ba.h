// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_BA_H__

#include <compare>
#include <string>
#include <optional>
#include <functional>
#include <memory>

#include "utility/hashing.h"
#include "tau_string_hash.h"
#include "tau_tree.h"
#include "tau_diagnostics.h"
#include "ba_constants.h"
#include "env_limits.h"
#include "splitter_types.h"
#include "boolean_algebras/nlang/parser/nlang_parser.generated.h"

namespace idni::tau_lang {

/** @brief Type tree of the nlang type. */
template <NodeType node> tref nlang_type();
/** @brief Type id of the nlang type. */
template <NodeType node> size_t nlang_type_id();


// Type definitions for nlang_ba


// -----------------------------------------------------------------------------
// nlang_ba — Natural Language Boolean Algebra (Lindenbaum-Tarski algebra)
//
// Elements are natural language propositions/statements represented as
// structural formula trees. The logical engine catches contradictions and
// tautologies structurally — A & ~A = bottom, A | ~A = top, ~~A = A —
// without any LLM calls. An OpenAI-compatible language model is only
// invoked on atomic propositions (leaf nodes of the formula tree).
//
// This minimises LLM mistakes: compound formulas built by the engine are
// simplified algebraically; only irreducible atoms need semantic judgement.
//
// Top = any tautology (canonical: "everything"), Bottom = any contradiction
// (canonical: "nothing"). The algebra is atomless: between any two distinct
// propositions p < q there always exists r with p < r < q.
// -----------------------------------------------------------------------------

// Configuration (environment variables):
//   TAU_LLM_API_KEY   — required; OpenAI-compatible API key (falls back to
//                       OPENAI_API_KEY). Without it every oracle query
//                       returns a conservative default and a warning is
//                       printed once.
//   TAU_LLM_ENDPOINT  — optional; API base URL (default:
//                       https://api.openai.com/v1)
//   TAU_LLM_MODEL     — optional; when unset no model is sent and the
//                       endpoint picks its own.
//   TAU_NLANG_HTTP_TIMEOUT — optional; seconds per request (default 15,
//                       0 = no cap). Environment fallback of the
//                       `nlang-http-timeout` option, which wins when given.

/**
 * @brief Wall-clock cap, in seconds, on each LLM HTTP request the nlang
 * oracle makes (curl's CURLOPT_TIMEOUT). Runtime parameter by policy
 * (nlang's own `nlang-http-timeout` CLI/REPL option); 0 = no cap.
 *
 * The sentinel -1 means "not set", in which case `TAU_NLANG_HTTP_TIMEOUT`
 * is consulted and 15 s applies when that is absent too; the option always
 * wins over the variable. Read through @ref nlang_http_timeout_sec.
 */
inline long nlang_http_timeout_sec_param = -1;

/**
 * @brief Effective per-request LLM HTTP timeout in seconds (0 = no cap).
 *
 * Precedence: @ref nlang_http_timeout_sec_param when set (>= 0), else
 * `TAU_NLANG_HTTP_TIMEOUT`, else 15.
 */
inline long nlang_http_timeout_sec() {
	if (nlang_http_timeout_sec_param >= 0)
		return nlang_http_timeout_sec_param;
	return (long) env_limit_count("TAU_NLANG_HTTP_TIMEOUT", 15);
}

// --- LLM API helpers (implemented in nlang_ba.cpp, linked via libTAU) ---
// The yes/no oracles answer from a process-wide cache first. Without an API
// key they warn once on stderr and answer false without caching; an empty
// or unrecognised reply is a cached false.

/**
 * @brief Send one chat-completion request to the configured endpoint.
 *
 * Blocking; bounded by nlang_http_timeout_sec().
 * @param prompt The user message.
 * @return The reply's content, or "" when no API key is set, the request
 * fails or the status is not 2xx (the last warns once on stderr).
 */
std::string llm_query(const std::string& prompt);
/// @brief Ask the oracle whether @p description is a contradiction.
/// @return true for "nothing", false for "everything", else the oracle's yes.
bool llm_is_empty(const std::string& description);
/// @brief Ask the oracle whether @p description is a tautology.
/// @return true for "everything", false for "nothing", else the oracle's yes.
bool llm_is_universal(const std::string& description);
/// @brief Ask the oracle whether @p a and @p b are logically equivalent.
/// @return true for equal strings; false when exactly one is "nothing" or
/// "everything"; else the oracle's yes (cached for the unordered pair).
bool llm_equivalent(const std::string& a, const std::string& b);
/**
 * @brief Ask the oracle for a statement strictly stronger than @p description.
 * @return "nothing" for "nothing", "it is raining" for "everything", else
 * the oracle's reply trimmed of blanks and dots, or `description + " and
 * specifically so"` when there is no key or no reply.
 */
std::string llm_stronger_statement(const std::string& description);
// llm_decompose declared after nlang_ba struct (return type needs nlang_ba::fptr)

// -----------------------------------------------------------------------------
// nlang_ba struct
// -----------------------------------------------------------------------------

/**
 * @brief An element of the natural-language Boolean algebra: a shared,
 * immutable formula tree over natural-language atoms.
 */
struct nlang_ba {
	/// Structural formula tree; children are shared, never mutated.
	struct formula {
		/// Shared handle to a formula node.
		using fptr = std::shared_ptr<formula>;
		/// Node kind.
		enum class kind : uint8_t { bot, top, atom, and_, or_, not_ };

		/// Node kind.
		kind k = kind::bot;
		/// The proposition of an atom.
		std::string atom_str;  // only for kind::atom
		/// The operands of a conjunction or disjunction.
		fptr lhs, rhs;         // for kind::and_, kind::or_
		/// The operand of a negation.
		fptr inner;            // for kind::not_

		/// New bottom node.
		static fptr mk_bot() {
			auto p = std::make_shared<formula>(); p->k = kind::bot; return p;
		}
		/// New top node.
		static fptr mk_top() {
			auto p = std::make_shared<formula>(); p->k = kind::top; return p;
		}
		/// New atom with proposition @p s (taken as is, even "nothing").
		static fptr mk_atom(std::string s) {
			auto p = std::make_shared<formula>();
			p->k = kind::atom; p->atom_str = std::move(s); return p;
		}
		/// New conjunction of @p l and @p r, unsimplified.
		static fptr mk_and(fptr l, fptr r) {
			auto p = std::make_shared<formula>();
			p->k = kind::and_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
		}
		/// New disjunction of @p l and @p r, unsimplified.
		static fptr mk_or(fptr l, fptr r) {
			auto p = std::make_shared<formula>();
			p->k = kind::or_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
		}
		/// New negation of @p i, unsimplified.
		static fptr mk_not(fptr i) {
			auto p = std::make_shared<formula>();
			p->k = kind::not_; p->inner = std::move(i); return p;
		}

		/// Canonical text: "nothing", "everything", the atom, or
		/// `not (..)`, `(..) and (..)`, `(..) or (..)`; parse_nlang reads it back.
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

		/// Structural (syntactic) equality of two trees.
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

		/// true iff one formula is syntactically the negation of the other.
		bool is_complement_of(const formula& o) const {
			if (k == kind::not_ && inner->struct_eq(o)) return true;
			if (o.k == kind::not_ && o.inner->struct_eq(*this)) return true;
			return false;
		}
	};
	/// Shared handle to a formula node.
	using fptr = formula::fptr;

	/// The formula; never null.
	fptr fm;

	/// Bottom.
	nlang_ba() : fm(formula::mk_bot()) {}
	/// Bottom for "nothing", top for "everything", else the atom @p s.
	explicit nlang_ba(std::string s) {
		if (s == "nothing")         fm = formula::mk_bot();
		else if (s == "everything") fm = formula::mk_top();
		else                        fm = formula::mk_atom(std::move(s));
	}

	/// The bottom element.
	static nlang_ba bottom() { return nlang_ba{"nothing"}; }
	/// The top element.
	static nlang_ba top()    { return nlang_ba{"everything"}; }
	/// Same as nlang_ba(std::string); no parsing, no oracle.
	static nlang_ba from_string(const std::string& s) { return nlang_ba{s}; }

	/// Construct from a pre-built formula tree (used by parse_nlang after decompose).
	static nlang_ba from_fm(fptr f) { nlang_ba r; r.fm = std::move(f); return r; }

	// --- BA operations with structural simplifications ---
	// The engine catches A & ~A = bottom, A | ~A = top, ~~A = A without LLM calls.

	/// Join; folds bottom, top, `A | A` and `A | ~A` structurally.
	nlang_ba operator|(const nlang_ba& o) const {
		if (fm->k == formula::kind::bot) return o;
		if (o.fm->k == formula::kind::bot) return *this;
		if (fm->k == formula::kind::top || o.fm->k == formula::kind::top) return top();
		if (fm->struct_eq(*o.fm)) return *this;               // A | A = A
		if (fm->is_complement_of(*o.fm)) return top();         // A | ~A = top
		nlang_ba r; r.fm = formula::mk_or(fm, o.fm); return r;
	}

	/// Meet; folds bottom, top, `A & A` and `A & ~A` structurally.
	nlang_ba operator&(const nlang_ba& o) const {
		if (fm->k == formula::kind::bot || o.fm->k == formula::kind::bot) return bottom();
		if (fm->k == formula::kind::top) return o;
		if (o.fm->k == formula::kind::top) return *this;
		if (fm->struct_eq(*o.fm)) return *this;               // A & A = A
		if (fm->is_complement_of(*o.fm)) return bottom();      // A & ~A = bottom
		nlang_ba r; r.fm = formula::mk_and(fm, o.fm); return r;
	}

	/// Complement; folds bottom, top and `~~A`.
	nlang_ba operator~() const {
		if (fm->k == formula::kind::bot) return top();
		if (fm->k == formula::kind::top) return bottom();
		if (fm->k == formula::kind::not_) { nlang_ba r; r.fm = fm->inner; return r; } // ~~A = A
		nlang_ba r; r.fm = formula::mk_not(fm); return r;
	}

	/// Symmetric difference, as `(a | b) & ~(a & b)`.
	nlang_ba operator^(const nlang_ba& o) const {
		return (*this | o) & ~(*this & o);
	}

	/// Structural equality. It must stay consistent with std::hash (which
	/// hashes to_string()) and with the string-based `<`/`<=>` below, and
	/// must not query the oracle: the constant pool compares constants
	/// pairwise on every insertion. Use semantically_equal for the oracle.
	bool operator==(const nlang_ba& o) const {
		return fm->struct_eq(*o.fm);
	}
	/// Negation of structural equality.
	bool operator!=(const nlang_ba& o) const { return !(*this == o); }

	/// Semantic equivalence: structural equality first, then llm_equivalent
	/// on the printed forms (blocking I/O, bounded by nlang_http_timeout_sec()).
	/// Never for container keys or pooling identity.
	bool semantically_equal(const nlang_ba& o) const {
		if (fm->struct_eq(*o.fm)) return true;
		return llm_equivalent(to_string(), o.to_string());
	}

	/// Whether the formula is literally top (@p b true) or bottom (@p b
	/// false); no oracle.
	bool operator==(bool b) const {
		return b ? (fm->k == formula::kind::top) : (fm->k == formula::kind::bot);
	}
	/// Negation of `*this == b`.
	bool operator!=(bool b) const { return !(*this == b); }

	/// Order by printed form; a total order, not the algebra's order.
	bool operator<(const nlang_ba& o) const { return to_string() < o.to_string(); }
	/// Three-way comparison by printed form.
	auto operator<=>(const nlang_ba& o) const { return to_string() <=> o.to_string(); }

	/// The canonical text of the formula (formula::to_string).
	std::string to_string() const { return fm->to_string(); }
};

/// Print the canonical text of @p n.
inline std::ostream& operator<<(std::ostream& os, const nlang_ba& n) {
	return os << n.to_string();
}

/**
 * @brief Decompose a natural-language statement into a formula tree with
 * the oracle (llm_query), cached per statement.
 * @param s The statement.
 * @return Bottom for "nothing", top for "everything"; `mk_atom(s)` when no
 * API key is set (not cached) or the reply has no JSON object; a malformed
 * subobject becomes the atom @p s in place.
 */
nlang_ba::fptr llm_decompose(const std::string& s);

// --- Mutually recursive structural emptiness/universality checks ---
// Oracle is only called on atomic leaf nodes.
// and_-emptiness and or_-universality may fall back to oracle for non-structural cases.

/// Whether @p f is bottom: structural where possible, else llm_is_empty on
/// an atom or on an unresolved conjunction.
inline bool nlang_struct_is_empty(const nlang_ba::fptr& f);
/// Whether @p f is top: structural where possible, else llm_is_universal on
/// an atom or on an unresolved disjunction.
inline bool nlang_struct_is_one(const nlang_ba::fptr& f);

inline bool nlang_struct_is_empty(const nlang_ba::fptr& f) {
	using K = nlang_ba::formula::kind;
	switch (f->k) {
	case K::bot:  return true;
	case K::top:  return false;
	case K::atom: return llm_is_empty(f->atom_str);
	case K::not_: return nlang_struct_is_one(f->inner);
	case K::and_:
		// A & B = bottom if A=bottom, B=bottom, A=~B, or semantically incompatible
		if (nlang_struct_is_empty(f->lhs) || nlang_struct_is_empty(f->rhs)) return true;
		if (f->lhs->is_complement_of(*f->rhs)) return true;
		return llm_is_empty(f->to_string());  // oracle fallback for non-structural cases
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
	case K::atom: return llm_is_universal(f->atom_str);
	case K::not_: return nlang_struct_is_empty(f->inner);
	case K::or_:
		// A | B = top if A=top, B=top, A=~B, or semantically universal
		if (nlang_struct_is_one(f->lhs) || nlang_struct_is_one(f->rhs)) return true;
		if (f->lhs->is_complement_of(*f->rhs)) return true;
		return llm_is_universal(f->to_string());  // oracle fallback for non-structural cases
	case K::and_:
		// A & B = top iff both A=top and B=top (purely structural)
		return nlang_struct_is_one(f->lhs) && nlang_struct_is_one(f->rhs);
	}
	return false;
}

// --- free functions expected by the dispatcher ---

// The dispatcher's is_syntactic_zero/one contract is a cheap check on the
// normalization hot path, so these probes are oracle-free: atoms and
// unresolved compounds answer false instead of costing an HTTPS round-trip
// per uncached atom. Oracle-backed emptiness/universality stays in
// normalize_nlang below, which folds contradictions/tautologies to bot/top so
// later syntactic probes see them structurally.

/// Whether @p f is top by structure alone (no oracle).
inline bool nlang_syntactic_is_one(const nlang_ba::fptr& f);

/// Whether @p f is bottom by structure alone (no oracle).
inline bool nlang_syntactic_is_empty(const nlang_ba::fptr& f) {
	using K = nlang_ba::formula::kind;
	switch (f->k) {
	case K::bot:  return true;
	case K::top:  return false;
	case K::atom: return false;
	case K::not_: return nlang_syntactic_is_one(f->inner);
	case K::and_:
		if (nlang_syntactic_is_empty(f->lhs)
			|| nlang_syntactic_is_empty(f->rhs)) return true;
		return f->lhs->is_complement_of(*f->rhs);
	case K::or_:
		return nlang_syntactic_is_empty(f->lhs)
			&& nlang_syntactic_is_empty(f->rhs);
	}
	return false;
}

inline bool nlang_syntactic_is_one(const nlang_ba::fptr& f) {
	using K = nlang_ba::formula::kind;
	switch (f->k) {
	case K::top:  return true;
	case K::bot:  return false;
	case K::atom: return false;
	case K::not_: return nlang_syntactic_is_empty(f->inner);
	case K::or_:
		if (nlang_syntactic_is_one(f->lhs)
			|| nlang_syntactic_is_one(f->rhs)) return true;
		return f->lhs->is_complement_of(*f->rhs);
	case K::and_:
		return nlang_syntactic_is_one(f->lhs)
			&& nlang_syntactic_is_one(f->rhs);
	}
	return false;
}

/// Oracle-free zero test of @p x (nlang_syntactic_is_empty).
inline bool is_nlang_zero(const nlang_ba& x) {
	return nlang_syntactic_is_empty(x.fm);
}

/// Oracle-free one test of @p x (nlang_syntactic_is_one).
inline bool is_nlang_one(const nlang_ba& x) {
	return nlang_syntactic_is_one(x.fm);
}

/// Normalize: reduce contradictions to bottom, tautologies to top, else @p x.
/// Structural checks are tried first; the oracle is only asked about atoms
/// and unresolved compounds (nlang_struct_is_empty / nlang_struct_is_one).
inline nlang_ba normalize_nlang(const nlang_ba& x) {
	using K = nlang_ba::formula::kind;
	if (x.fm->k == K::bot || x.fm->k == K::top) return x;
	if (nlang_struct_is_empty(x.fm)) return nlang_ba::bottom();
	if (nlang_struct_is_one(x.fm))   return nlang_ba::top();
	return x;
}

/// Symbol simplification hook; nlang has none, so @p sym is returned.
inline tref simplify_nlang_symbol(tref sym) { return sym; }
/// Term simplification hook; nlang has none, so @p t is returned.
inline tref simplify_nlang_term(tref t) { return t; }

/// Splitter: return a logically stronger statement implying @p x.
/// Bottom for bottom; the left operand of a disjunction (A <= A|B);
/// otherwise the atom llm_stronger_statement gives. The splitter type is
/// ignored.
inline nlang_ba nlang_splitter(const nlang_ba& x, splitter_type /*st*/) {
	using K = nlang_ba::formula::kind;
	if (x.fm->k == K::bot) return nlang_ba::bottom();
	if (x.fm->k == K::or_) { nlang_ba r; r.fm = x.fm->lhs; return r; }
	return nlang_ba{llm_stronger_statement(x.to_string())};
}

/// A fixed non-trivial contingent proposition (neither tautology nor contradiction).
inline nlang_ba nlang_splitter_one() {
	return nlang_ba{"it is raining"};
}

// --- parsing ---

/// Walk a shaped nlang parse tree and build a formula tree.
/// @param t A traverser pointing at a 'formula' node.
/// @return The tree; atoms are trimmed of surrounding blanks, and a
/// connective missing an operand becomes bottom.
inline nlang_ba::fptr nlang_eval_parse_tree(
	const nlang_parser::tree::traverser& t)
{
	using tt = nlang_parser::tree::traverser;
	using type = nlang_parser::nonterminal;

	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;

	switch (nt) {
	case type::nlang_bot: return nlang_ba::formula::mk_bot();
	case type::nlang_top: return nlang_ba::formula::mk_top();

	case type::nlang_atom: {
		std::string s = n | tt::terminals;
		s.erase(0, s.find_first_not_of(" \t\n\r"));
		auto last = s.find_last_not_of(" \t\n\r");
		if (last != std::string::npos) s = s.substr(0, last + 1);
		return nlang_ba::formula::mk_atom(std::move(s));
	}

	case type::nlang_not: {
		auto children = (n | tt::children)();
		if (children.empty()) return nlang_ba::formula::mk_bot();
		return nlang_ba::formula::mk_not(nlang_eval_parse_tree(children[0]));
	}

	case type::nlang_and: {
		auto children = (n | tt::children)();
		if (children.size() < 2) return nlang_ba::formula::mk_bot();
		return nlang_ba::formula::mk_and(
			nlang_eval_parse_tree(children[0]),
			nlang_eval_parse_tree(children[1]));
	}

	case type::nlang_or: {
		auto children = (n | tt::children)();
		if (children.size() < 2) return nlang_ba::formula::mk_bot();
		return nlang_ba::formula::mk_or(
			nlang_eval_parse_tree(children[0]),
			nlang_eval_parse_tree(children[1]));
	}

	default:
		return nlang_ba::formula::mk_atom(t | tt::terminals);
	}
}

/// Grammar-based parse of @p s; no oracle.
/// @return The formula tree, or nullopt if the input doesn't match.
inline std::optional<nlang_ba::fptr> parse_nlang_grammar(const std::string& s) {
	auto result = nlang_parser::instance().parse(s.c_str(), s.size());
	if (!result.found) return std::nullopt;

	auto t = nlang_parser::tree::traverser(result.get_shaped_tree2())
		| nlang_parser::formula;
	if (!t.has_value()) return std::nullopt;

	return nlang_eval_parse_tree(t);
}

/**
 * @brief Parse an nlang constant.
 *
 * The source string is taken as the natural language description or a
 * structural formula in canonical form (see nlang_ba::formula::to_string()).
 * Surrounding braces and quotes are stripped.
 *
 * Parsing order:
 *   1. Grammar: handles canonical structural forms (nothing, everything,
 *      not (...), (...) and (...), (...) or (...), bare atoms).
 *   2. If grammar yields a bare atom, llm_decompose is tried to split it
 *      into a compound formula (handles "A and B" natural language input).
 *   3. llm_decompose on the whole string when the grammar fails (e.g.
 *      unbalanced parens).
 *
 * May query the oracle (blocking I/O).
 * @tparam BAs The Boolean algebras of the node pack.
 * @param src The constant's source text.
 * @return The constant with the nlang type; a parse error only for an empty
 * literal.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_nlang(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	std::string s = strip_ba_constant_source(src, /*strip_quotes=*/true);

	if (s.empty()) {
		r.error(code::parse_error, "Empty nlang literal");
		return r;
	}

	nlang_ba::fptr fm;
	if (auto gr = parse_nlang_grammar(s); gr) {
		fm = *gr;
		// Bare atom from grammar — let DeepSeek try to find compound structure
		// (handles natural language like "A and B" without explicit parentheses).
		if (fm->k == nlang_ba::formula::kind::atom) {
			auto decomposed = llm_decompose(fm->atom_str);
			if (decomposed->k != nlang_ba::formula::kind::atom)
				fm = decomposed;
		}
	} else {
		fm = llm_decompose(s);
	}

	return r.with_value(typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ nlang_ba::from_fm(std::move(fm)) },
		ba_descriptor<nlang_ba, node<BAs...>>::type_tree() });
}

/// Content hash in uint64_t, the same on every platform.
inline std::uint64_t nlang_hash(const nlang_ba& n) {
	return tau_string_hash(n.to_string());
}

} // namespace idni::tau_lang

/// Hash of nlang_ba: nlang_hash, consistent with structural operator==.
template<>
struct std::hash<idni::tau_lang::nlang_ba> {
	size_t operator()(const idni::tau_lang::nlang_ba& n) const noexcept {
		return static_cast<size_t>(idni::tau_lang::nlang_hash(n));
	}
};

#include "boolean_algebras/nlang/nlang_descriptor.tmpl.h"
#include "boolean_algebras/nlang/nlang_types.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_BA_H__
