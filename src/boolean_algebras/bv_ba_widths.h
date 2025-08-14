// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "nso_rr.h"
#include "resolver.h"

namespace idni::tau_lang {

template<typename...BAs>
tau<BAs...> annotate_bv_widths(const tau<BAs...>& form, resolver<BAs...>& widths) {
	auto nt = std::get<tau_source_sym>(form->value).n();
	switch (nt) {
		case tau_parser::wff_always: {
			auto expr = annotate_bv_widths(form->child[0], widths);
			return wrap(tau_parser::wff_always, expr);
		}
		case tau_parser::wff_sometimes: {
			auto expr = annotate_bv_widths(form->child[0], widths);
			return wrap(tau_parser::wff_sometimes, expr);
		}
		case tau_parser::wff: {
			auto expr = annotate_bv_widths(form->child[0], widths);
			return wrap(tau_parser::wff, expr);
		}
		case tau_parser::bv: {
			auto expr = annotate_bv_widths(form->child[0], widths);
			return wrap(tau_parser::bv, expr);
		}
		case tau_parser::wff_neg: {
			auto expr = annotate_bv_widths(form->child[0], widths);
			return wrap(tau_parser::wff_neg, expr);
		}
		case tau_parser::wff_and: {
			auto left = annotate_bv_widths(form->child[0], widths);
			auto right = annotate_bv_widths(form->child[1], widths);
			return wrap(tau_parser::wff_and, {left, right});
		}
		case tau_parser::wff_or: {
			auto left = annotate_bv_widths(form->child[0], widths);
			auto right = annotate_bv_widths(form->child[1], widths);
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
			auto body = annotate_bv_widths(form->child[1], widths);
			// we gather type annotations from widths status
			std::map<tau<BAs...>, tau<BAs...>> changes;
			for (const auto& [var, _] : widths.current.vars) {
				if (auto type = widths.type_of(var); type) {
					auto typed = wrap(tau_parser::variable, {var->child[0], type});

					#ifdef DEBUG
					print_tau_tree(std::cout, var);
					print_tau_tree(std::cout, typed);
					#endif // DEBUG

					changes[var] = typed;
				}
			}
			auto new_body = replace(body, changes);
			auto new_prefix = replace(form->child[0], changes);
			auto new_form = make_node(form->value, {new_prefix, new_body});
			// remove quantified variables from the scope, close it and return
			// the result
			for (auto var: quantifieds) {
				auto untyped = wrap(tau_parser::variable, {var->child[0]});
				widths.current.vars.erase(untyped);
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
			// collect all variables, add them to the scope, unite all of them and,
			// if possible, type them
			auto vars = select_top(form, is_non_terminal<tau_parser::variable, BAs...>);
			if (vars.size() > 0) {
				auto untyped_0 = wrap(tau_parser::variable, {vars[0]->child[0]});
				widths.add(untyped_0);
				for (size_t i = 1; i < vars.size(); ++i) {
					auto untyped_i = wrap(tau_parser::variable, {vars[i]->child[0]});
					widths.add(untyped_i);
					widths.unite(untyped_0, untyped_i);
				}
				if (auto type_bv = find_top(form, is_non_terminal<tau_parser::bv_type, BAs...>); type_bv) {
					auto type = wrap(tau_parser::type, type_bv.value());
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

template<typename...BAs>
tau<BAs...> annotate_bv_widths(const tau<BAs...>& form) {
	resolver<BAs...> widths;
	auto annotated = annotate_bv_widths(form, widths);
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& [var, _] : widths.current.vars) {
		if (auto type = widths.type_of(var); type) {
			auto typed = wrap(tau_parser::variable, {var->child[0], type});
			changes[var] = typed;
		}
	}
	auto new_form = replace(annotated, changes);
	return new_form;
}

template<typename...BAs>
tau<BAs...> annotate_bv_ctes(const tau<BAs...>& form, resolver<BAs...>& widths) {
	auto nt = std::get<tau_source_sym>(form->value).n();
	switch (nt) {
		case tau_parser::wff_always: {
			auto expr = annotate_bv_ctes(form->child[0], widths);
			return wrap(tau_parser::wff_always, expr);
		}
		case tau_parser::wff_sometimes: {
			auto expr = annotate_bv_ctes(form->child[0], widths);
			return wrap(tau_parser::wff_sometimes, expr);
		}
		case tau_parser::wff: {
			auto expr = annotate_bv_ctes(form->child[0], widths);
			return wrap(tau_parser::wff, expr);
		}
		case tau_parser::bv: {
			auto expr = annotate_bv_ctes(form->child[0], widths);
			return wrap(tau_parser::bv, expr);
		}
		case tau_parser::wff_neg: {
			auto expr = annotate_bv_ctes(form->child[0], widths);
			return wrap(tau_parser::wff_neg, expr);
		}
		case tau_parser::wff_and: {
			auto left = annotate_bv_ctes(form->child[0], widths);
			auto right = annotate_bv_ctes(form->child[1], widths);
			return wrap(tau_parser::wff_and, {left, right});
		}
		case tau_parser::wff_or: {
			auto left = annotate_bv_ctes(form->child[0], widths);
			auto right = annotate_bv_ctes(form->child[1], widths);
			return wrap(tau_parser::wff_or, {left, right});
		}
		case tau_parser::wff_ex:
		case tau_parser::wff_all: {
			auto new_body = annotate_bv_ctes(form->child[1], widths);
			auto new_form = make_node(form->value, {form->child[0], new_body});
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
			// collect all ctes and, if possible, type them
			if (auto bv_type = find_top(form, is_non_terminal<tau_parser::bv_type, BAs...>); bv_type) {
				std::map<tau<BAs...>, tau<BAs...>> changes;
				auto ctes = select_top(form, is_non_terminal<tau_parser::bitvector, BAs...>);
				for (const auto& cte : ctes) {
					if (auto type = cte | tau_parser::bv_type; !type) {
						auto typed = wrap(tau_parser::bitvector, {cte->child[0], bv_type.value()});
						changes[cte] = typed;
					}
				}
				auto new_form = replace(form, changes);
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

template<typename...BAs>
tau<BAs...> annotate_bv_ctes(const tau<BAs...>& form) {
	resolver<BAs...> widths;
	return annotate_bv_ctes(form, widths);
}

} // namespace idni::tau_lang