// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"


namespace idni::tau_lang {

/**
 * @brief Creates a rewriter rule from a head and body term.
 * @tparam node Node type
 * @param head The head of the rule
 * @param body The body of the rule
 * @return The constructed rewriter rule
 */
template<NodeType node>
inline static rewriter::rule make_rule(tref head, tref body) {
	using tau = tree<node>;

	return rewriter::rule(tau::geth(head), tau::geth(body));
}

/**
 * @brief Creates a recurrence relation (rr) from rules and a main formula.
 * @tparam node Node type
 * @param rules The rules, each defining one recurrence relation
 * @param main The main formula, usually a call of a rule's head
 * @return The constructed recurrence relation
 */
template<NodeType node>
static rr<node> make_rr(const rewriter::rules& rules, tref main) {
	using tau = tree<node>;

	return rr<node>{rules, tau::geth(main)};
}

} // namespace idni::tau_lang