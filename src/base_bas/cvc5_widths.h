// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include "nso_rr.h"
#include "resolver.h"

namespace idni::tau_lang {

template<typename...BAs>
tau<BAs...> annotate_bv_widths(const tau<BAs...>& form, resolver<BAs...>& widths = resolver<BAs...>{}) {
	auto nt = std::get<tau_source_sym>(form->value).n();
	switch (nt) {
		case tau_parser::wff_always: {
			auto expr = infer_bv_width(form->child[0], widths);
			return wrap(tau_parser::wff_always, expr);
		}
		case tau_parser::wff_sometimes: {
			auto expr = infer_bv_width(form->child[0], widths);
			return wrap(tau_parser::wff_sometimes, expr);
		}
		case tau_parser::wff: {
			auto expr = infer_bv_width(form->child[0], widths);
			return wrap(tau_parser::wff, expr);
		}
		case tau_parser::bv: {
			auto expr = infer_bv_width(form->child[0], widths);
			return wrap(tau_parser::bv, expr);
		}
		case tau_parser::wff_neg: {
			auto expr = infer_bv_width(form->child[0], widths);
			return wrap(tau_parser::wff_neg, expr);
		}
		case tau_parser::wff_and: {
			auto left = infer_bv_width(form->child[0], widths);
			auto right = infer_bv_width(form->child[1], widths);
			return wrap(tau_parser::wff_and, {left, right});
		}
		case tau_parser::wff_or: {
			auto left = infer_bv_width(form->child[0], widths);
			auto right = infer_bv_width(form->child[1], widths);
			return wrap(tau_parser::wff_or, {left, right});
		}
		case tau_parser::wff_ex: 
		case tau_parser::wff_all: {
			// first of all, we create a new "empty" scope
			widths.open();
			// add variables with widths to a new scope with
			// the corresponding widths
			auto quantifieds = select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>);
			for(auto var: quantifieds) {
				if (auto type = var | tau_parser::type; type) {
					if (auto bv_type = type | tau_parser::bv_type; bv_type) {
						// if the type is of kind bv, we add the variable to the
						// scope and type it
						// we ignore the others as we are not interested in them
						auto untyped = wrap(tau_parser::variable, {var->child[0]});
						widths.current.erase(untyped);
						widths.add(untyped, type.value());
					} 
				} else {
					// we have an untyped variable, we add it as is
					widths.add(var);
				}
			}

			// infer widths for the body and replace quantified bitvector variables
			// by they corresponding annoated versions
			auto body = annotate_bv_widths(form->child[1], widths);
			// we gather type annotations from widths status
			std::map<tau<BAs...>, tau<BAs...>> changes;
			for (const auto& var : quantifieds) {
				auto untyped = wrap(tau_parser::variable, {var->child[0]});
				auto typed = wrap(tau_parser::variable, {var->child[0], widths.type_of(untyped)});
				changes[untyped] = typed;
			}
			auto new_form = replace(form, changes);
			// remove quantified variables from the scope, close it and return 
			// the result
			for (auto var: quantifieds) {
				auto untyped = wrap(tau_parser::variable, {var->child[0]});
				widths.current.erase(untyped);
			}
			widths.close();
			return new_form;
		}
		case tau_parser::bv_eq: 
		case tau_parser::bv_neq:
		case tau_parser::bv_less_equal:
		case tau_parser::bv_nleq:
		case tau_parser::bv_greater:
		case tau_parser::bv_ngreater:
		case tau_parser::bv_greater_equal:
		case tau_parser::bv_ngeq:
		case tau_parser::bv_less:
		case tau_parser::bv_nless: {
			// collect all variables, add missing ones to scope with
			// the corresponding widths and unite all of them if possible
			auto vars = select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>);
			if (vars.size() > 0) {
				auto untyped_0 = wrap(tau_parser::variable, {vars[0]->child[0]});
				if (auto type = vars[0] | tau_parser::type; type) {
					widths.add(untyped_0, type.value());
				} else {
					widths.add(untyped_0);
				}
				for (int i = 1; i < vars.size(); i++) {
					auto untyped_i = wrap(tau_parser::variable, {vars[i]->child[0]});
					if (auto type = vars[i] | tau_parser::type; type) {
						widths.add(untyped_i, type.value());
					} else {
						widths.add(untyped_i);
					}
					widths.unite(untyped_0, untyped_i);
				}
			}
		}
		default: {
			// in the rest of the cases, we simply return the original form
			// as there is nothing to change
			return form;
		}

	}
}

} // namespace idni::tau_lang