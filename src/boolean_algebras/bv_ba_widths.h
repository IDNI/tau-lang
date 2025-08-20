// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "resolver.h"

namespace idni::tau_lang {

template<NodeType node >
tref annotate_bv_widths(const typename tree<node>::traverser& form, resolver& widths) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case tau::wff_always: {
			auto expr = annotate_bv_widths(form | tt::first, widths);
			return tau::get(tau::wff_always, expr);
		}
		case tau::wff_sometimes: {
			auto expr = annotate_bv_widths(form | tt::first, widths);
			return tau::get(tau::wff_sometimes, expr);
		}
		case tau::wff: {
			auto expr = annotate_bv_widths(form | tt::first, widths);
			return tau::get(tau::wff, expr);
		}
		case tau::bv: {
			auto expr = annotate_bv_widths(form | tt::first, widths);
			return tau::get(tau::bv, expr);
		}
		case tau::wff_neg: {
			auto expr = annotate_bv_widths(form | tt::first, widths);
			return tau::get(tau::wff_neg, expr);
		}
		case tau::wff_and: {
			auto left = annotate_bv_widths(form | tt::first, widths);
			auto right = annotate_bv_widths(form | tt::second, widths);
			return tau::get(tau::wff_and, {left, right});
		}
		case tau::wff_or: {
			auto left = annotate_bv_widths(form | tt::first, widths);
			auto right = annotate_bv_widths(form | tt::second, widths);
			return tau::get(tau::wff_or, {left, right});
		}
		case tau::wff_ex:
		case tau::wff_all: {
			// first of all, we create a new "empty" scope
			widths.open();
			// add variables with widths to a new scope with
			// the corresponding widths
			auto quantifieds = (form | tt::first).value().select_top(is<node, tau::variable>);
			for(auto var: quantifieds) {
				if (auto type = tt(var) | tau::type; type) {
					if (auto bv_type = type | tau::bv_type; bv_type) {
						// if the type is of kind bv, we add the variable to the
						// scope and type it
						// we ignore the others as we are not interested in them
						auto untyped = tau::get(tau::variable, { (tt(var) | tt::child(0)).value() });
						widths.current.vars.erase(untyped);
						widths.add(untyped, type.value());
					}
				} else {
					// we have an untyped variable, we add it as it is
					widths.add(var);
				}
			}

			// infer widths for the body and replace quantified bitvector variables
			// by they corresponding annoated versions
			auto body = annotate_bv_widths(form | tt::second, widths);
			// we gather type annotations from widths status
			std::map<tref, tref> changes;
			for (const auto& [var, _] : widths.current.vars) {
				if (auto type = widths.type_of(var); type) {
					auto typed = tau::get(tau::variable, {(var | tt::child(0)).value(), type.value()});
					changes[var] = typed;
				}
			}
			auto new_body = rewriter::replace<node>(body, changes);
			auto new_prefix = rewriter::replace<node>(form | tt::first, changes);
			auto new_form = tau::get(form.value_tree().get_type(),
				{new_prefix, new_body});
			// remove quantified variables from the scope, close it and return
			// the result
			for (auto var: quantifieds) {
				auto untyped = tau::get(tau::variable, { ( tt(var) | tt::first).value() });
				widths.current.vars.erase(untyped);
			}
			widths.close();
			return new_form;
		}
		case tau::bv_eq:
		case tau::bv_neq:
		case tau::bv_lteq:
		case tau::bv_nleq:
		case tau::bv_gt:
		case tau::bv_ngt:
		case tau::bv_gteq:
		case tau::bv_ngteq:
		case tau::bv_lt:
		case tau::bv_nlt: {
			// collect all variables, add them to the scope, unite all of them and,
			// if possible, type them
			auto vars = (form | tt::first).value().select_top(is<node, tau::variable>);
			if (vars.size() > 0) {
				auto untyped_0 = tau::get(tau::variable, { (tt(vars[0]) | tt::first).value() });
				widths.add(untyped_0);
				for (size_t i = 1; i < vars.size(); ++i) {
					auto untyped_i = tau::get(tau::variable, { (tt(vars[i]) | tt::first).value() });
					widths.add(untyped_i);
					widths.unite(untyped_0, untyped_i);
				}
				if (auto type_bv = (form | tt::first).value().find_top(is<node, tau::bv_type>); type_bv) {
					auto type = tau::get(tau::type, type_bv.value());
					widths.type(untyped_0, type);
				}
			}
			return form;
		}
		default: {
			// in the rest of the cases, we simply return the original form
			// as there is nothing to change
			return form;
		}
	}
}

template<NodeType node>
tref annotate_bv_widths(const tref& form) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	resolver widths;
	auto annotated = annotate_bv_widths(tt(form), widths);
	std::map<tref, tref> changes;
	for (const auto& [var, _] : widths.current.vars) {
		if (auto type = widths.type_of(var); type) {
			auto typed = tau::get(tau::variable, { (tt(var) | tt::first).value(), type});
			changes[var] = typed;
		}
	}
	auto new_form = rewriter::replace<node>(annotated, changes);
	return new_form;
}

template<NodeType node>
tref annotate_bv_ctes(const typename tree<node>::traverser& form, resolver& widths) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case tau::wff_always: {
			auto expr = annotate_bv_ctes(form | tt::first, widths);
			return tau::get(tau::wff_always, expr);
		}
		case tau::wff_sometimes: {
			auto expr = annotate_bv_ctes(form | tt::first, widths);
			return tau::get(tau::wff_sometimes, expr);
		}
		case tau::wff: {
			auto expr = annotate_bv_ctes(form | tt::first, widths);
			return tau::get(tau::wff, expr);
		}
		case tau::bv: {
			auto expr = annotate_bv_ctes(form | tt::first, widths);
			return tau::get(tau::bv, expr);
		}
		case tau::wff_neg: {
			auto expr = annotate_bv_ctes(form | tt::first, widths);
			return tau::get(tau::wff_neg, expr);
		}
		case tau::wff_and: {
			auto left = annotate_bv_ctes(form | tt::first, widths);
			auto right = annotate_bv_ctes(form | tt::second, widths);
			return tau::get(tau::wff_and, {left, right});
		}
		case tau::wff_or: {
			auto left = annotate_bv_ctes(form | tt::first, widths);
			auto right = annotate_bv_ctes(form | tt::second, widths);
			return tau::get(tau::wff_or, {left, right});
		}
		case tau::wff_ex:
		case tau::wff_all: {
			auto new_body = annotate_bv_ctes(form | tt::child(1), widths);
			auto new_form = tau::get(form.value_tree().get_type, { (form | tt::first).value(), new_body});
			return new_form;
		}
		case tau::bv_eq:
		case tau::bv_neq:
		case tau::bv_lteq:
		case tau::bv_nleq:
		case tau::bv_greater:
		case tau::bv_ngreater:
		case tau::bv_greater_equal:
		case tau::bv_ngeq:
		case tau::bv_less:
		case tau::bv_nless: {
			// collect all ctes and, if possible, type them
			if (auto bv_type = form.value_tree().find_top(is<node, tau::bv_type>); bv_type) {
				std::map<tref, tref> changes;
				auto ctes = form.value_tree().select_top(is<node, tau::bv_constant>);
				for (const auto& cte : ctes) {
					if (auto type = tt(cte) | tau::bv_type; !type) {
						auto typed = tau::get(tau::bitvector, { (tt(cte) | tt::first).value(), bv_type});
						changes[cte] = typed;
º					}
				}
				auto new_form = rewriter::replace<node>(form, changes);
				return new_form;
			}
			return form;
		}
		default: {
			// in the rest of the cases, we simply return the original form
			// as there is nothing to change
			return form;
		}
	}
}

template<NodeType node>
tref annotate_bv_ctes(const tref& form) {
	resolver widths;
	return annotate_bv_ctes(form, widths);
}

} // namespace idni::tau_lang