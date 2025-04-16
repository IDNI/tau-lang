// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TEST_HELPERS_H__
#define __TEST_HELPERS_H__

#include "doctest.h"
#include "depreciating/nso_rr_depreciating.h"
#include "depreciating/language_depreciating.h"
#include "depreciating/boolean_algebras/nso_ba_depreciating.h"

using namespace idni::tau_lang;
using namespace idni::tau_lang_depreciating;
using namespace idni::rewriter::depreciating;
namespace testing = doctest;

// TODO (LOW) most of this functions could be remove and replace by the standart API


namespace idni::tau_lang_depreciating {

	template<>
	struct nso_factory<Bool> {

		tau_<Bool> parse(const std::string&,
				const std::string&) const {
			throw std::logic_error("not implemented");
		}

		tau_<Bool> binding(const tau_<Bool>& n,
				const std::string& type) const {
			if (type != "bool") return n;
			return make_node<tau_sym<Bool>>(Bool(true), {});
		}

		std::vector<std::string> types() const {
			return { "bool" };
		}

		std::string default_type() const {
			return "bool";
		}

		std::string one(const std::string&) const {
			throw std::logic_error("not implemented");
		}

		std::string zero(const std::string&) const {
			throw std::logic_error("not implemented");
		}

		tau_<Bool> splitter_one(const std::string&) const {
			throw std::logic_error("not implemented");
		}

		std::optional<Bool> unpack_tau_ba(
			const std::variant<Bool>&) const {
			// There is no tau_ba
			return {};
		}

		static nso_factory<Bool>& instance() {
				static nso_factory<Bool> factory;
				return factory;
			}

	private:

		nso_factory() {};
	};

} // namespace idni::tau_lang

// helper functions
tau_<Bool> make_statement(const sp_tau_source_node& source) {
	tauify<Bool> tf;
	map_transformer<tauify<Bool>, sp_tau_source_node, tau_<Bool>> transform(tf);
	return post_order_traverser<
			map_transformer<tauify<Bool>, sp_tau_source_node, tau_<Bool>>,
			all_t,
			sp_node<tau_source_sym>,
			tau_<Bool>>(
		transform, all)(source);
}

tau_<Bool> make_named_bindings(const tau_<Bool>& statement, const bindings<Bool>& bs) {
	name_binder<Bool> nb(bs);
	bind_transformer<name_binder<Bool>, Bool> binder(nb);
	return post_order_traverser<
			bind_transformer<name_binder<Bool>, Bool>,
			all_t,
			tau_<Bool>>(
		binder, all)(statement);
}

tau_<Bool> make_factory_bindings(const tau_<Bool>& statement) {
	factory_binder<Bool> fb;
	bind_transformer<factory_binder<Bool>, Bool> binder(fb);
	return post_order_traverser<
			bind_transformer<factory_binder<Bool>, Bool>,
			all_t,
			tau_<Bool>>(
		binder, all)(statement);
}

std::ostream& print_tau(std::ostream &os, tau_<Bool> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(idni::tau_lang::overloaded {
		[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
		[&os](std::variant<Bool> v) { if (auto b = std::get<0>(v); b.b) os << "true"; else os << "false"; },
		[&os](size_t v) { os << v; }
	}, n->value);
	for (auto& d : n->child) print_tau(os, d, l + 1);
	os << "}";
	return os;
}

std::ostream& pretty_print_tau(std::ostream &os, tau_<Bool> n, size_t l = 0) {
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (!v.nt()) os << v.t(); },
		[&os](std::variant<Bool> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...bdd..."; },
		[&os](size_t v) { os << v; }
	}, n->value);
	for (auto& d : n->child) pretty_print_tau(os, d, l + 1);
	return os;
}
#endif // __TEST_HELPERS_H__