// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The fold layer's contracts, one case per fold, against the configured
// pack. Anything that needs a particular algebra is under its
// TAU_PACK_HAS_BA_<ID> define so the file builds in every pack.

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {
size_t tid(tref type_tree) { return ba_types<node_t>::id(type_tree); }

tref wff(const char* src) { return tau::get(src, parse_wff()).value_or(nullptr); }
}

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
}

// A pack of two solving algebras and one without a solver, built only to
// route through: the folds read nothing of a pack but its bas_tuple and the
// descriptors' owns_type / solve. Each solver answers its own name, so a
// test sees which one the fold reached.
namespace two_solvers {
struct solver_a {};
struct solver_b {};
struct no_solver {};
struct fake_node {
	using bas_tuple = std::tuple<solver_a, solver_b, no_solver>;
};
// solver_a owns two ids, as a parameterised family owns its widths
constexpr size_t a_8 = 101, a_16 = 102, b_id = 201, none_id = 301,
	nobody = 999;
}

namespace idni::tau_lang {
template <>
struct ba_descriptor<two_solvers::solver_a, two_solvers::fake_node> {
	static constexpr const char* type_name = "solver_a";
	static bool owns_type(size_t t) {
		return t == two_solvers::a_8 || t == two_solvers::a_16;
	}
	static std::optional<std::string> solve(tref) { return "solver_a"; }
};
template <>
struct ba_descriptor<two_solvers::solver_b, two_solvers::fake_node> {
	static constexpr const char* type_name = "solver_b";
	static bool owns_type(size_t t) { return t == two_solvers::b_id; }
	static std::optional<std::string> solve(tref) { return "solver_b"; }
};
template <>
struct ba_descriptor<two_solvers::no_solver, two_solvers::fake_node> {
	static constexpr const char* type_name = "no_solver";
	static bool owns_type(size_t t) { return t == two_solvers::none_id; }
};
}

TEST_SUITE("pack_solve routes by the owner of the type") {
	using namespace two_solvers;

	TEST_CASE("each solver answers for its own types only") {
		static_assert(ba_has_solve<fake_node, solver_a>);
		static_assert(ba_has_solve<fake_node, solver_b>);
		static_assert(!ba_has_solve<fake_node, no_solver>);
		CHECK(pack_solve<fake_node, std::string>(a_8, nullptr)
			== std::optional<std::string>("solver_a"));
		CHECK(pack_solve<fake_node, std::string>(a_16, nullptr)
			== std::optional<std::string>("solver_a"));
		CHECK(pack_solve<fake_node, std::string>(b_id, nullptr)
			== std::optional<std::string>("solver_b"));
	}

	TEST_CASE("an owner without solve and an unowned type answer nullopt") {
		CHECK_FALSE(pack_solve<fake_node, std::string>(none_id, nullptr)
			.has_value());
		CHECK_FALSE(pack_solve<fake_node, std::string>(nobody, nullptr)
			.has_value());
		CHECK_FALSE(pack_solve<fake_node, std::string>(size_t{0}, nullptr)
			.has_value());
	}

	TEST_CASE("pack_owner_index groups a BA's types and separates the BAs") {
		CHECK(pack_owner_index<fake_node>(a_8) == std::optional<size_t>(0));
		CHECK(pack_owner_index<fake_node>(a_16) == std::optional<size_t>(0));
		CHECK(pack_owner_index<fake_node>(b_id) == std::optional<size_t>(1));
		CHECK(pack_owner_index<fake_node>(none_id) == std::optional<size_t>(2));
		CHECK_FALSE(pack_owner_index<fake_node>(nobody).has_value());
		CHECK_FALSE(pack_owner_index<fake_node>(size_t{0}).has_value());
	}
}

TEST_SUITE("pack_owner_apply") {
	TEST_CASE("stops at the owner and answers nullopt for a type nobody owns") {
		std::vector<std::string> visited;
		auto probe = [&]<typename BA>() -> std::optional<std::string> {
			visited.push_back(ba_descriptor<BA, node_t>::type_name);
			return std::string(ba_descriptor<BA, node_t>::type_name);
		};
#ifdef TAU_PACK_HAS_BA_SBF
		const size_t sbf_id = tid(ba_descriptor<sbf_ba, node_t>::type_tree());
		auto r = pack_owner_apply<node_t>(sbf_id, probe);
		REQUIRE(r.has_value());
		CHECK(*r == "sbf");
		CHECK(visited.size() == 1);
		visited.clear();
#endif
		CHECK_FALSE(pack_owner_apply<node_t>(tid(untyped_type<node_t>()), probe).has_value());
		CHECK(visited.empty());
		CHECK_FALSE(pack_owner_apply<node_t>(size_t{0}, probe).has_value());
		CHECK(visited.empty());
	}
}

TEST_SUITE("owner-gated folds answer for the owner and empty otherwise") {
	TEST_CASE("pack_zero_constant / pack_value_constant") {
		CHECK(pack_zero_constant<node_t>(tid(untyped_type<node_t>())) == nullptr);
		CHECK(pack_value_constant<node_t>(tid(untyped_type<node_t>()), 1) == nullptr);
		CHECK(pack_zero_constant<node_t>(size_t{0}) == nullptr);
#ifdef TAU_PACK_HAS_BA_BV
		const size_t bv8 = ba_descriptor<bv, node_t>::type_id_for(8);
		CHECK(pack_zero_constant<node_t>(bv8) != nullptr);
		CHECK(pack_value_constant<node_t>(bv8, 1) != nullptr);
#endif
#ifdef TAU_PACK_HAS_BA_SBF
		const size_t sbf_id = tid(ba_descriptor<sbf_ba, node_t>::type_tree());
		CHECK(pack_value_constant<node_t>(sbf_id, 0) != nullptr);
		CHECK(pack_zero_constant<node_t>(sbf_id) == nullptr);
#endif
	}
	TEST_CASE("pack_modular_width / pack_modular_value") {
		CHECK(pack_modular_width<node_t>(tid(untyped_type<node_t>())) == 0);
		CHECK(pack_modular_width<node_t>(size_t{0}) == 0);
#ifdef TAU_PACK_HAS_BA_BV
		const size_t bv8 = ba_descriptor<bv, node_t>::type_id_for(8);
		CHECK(pack_modular_width<node_t>(bv8) == 8);
		CHECK(pack_modular_value<node_t>(bv8, pack_value_constant<node_t>(bv8, 200))
			== std::optional<uint64_t>{ 200 });
		CHECK(pack_modular_value<node_t>(bv8, build_bf_t_type<node_t>(bv8))
			== std::optional<uint64_t>{ 255 });
#endif
#ifdef TAU_PACK_HAS_BA_SBF
		const size_t sbf_id = tid(ba_descriptor<sbf_ba, node_t>::type_tree());
		CHECK(pack_modular_width<node_t>(sbf_id) == 0);
#endif
	}
	TEST_CASE("pack_type_decides_closed / pack_decide_closed") {
		CHECK_FALSE(pack_type_decides_closed<node_t>(size_t{0}));
		CHECK(pack_decide_closed<node_t>(size_t{0}, tree<node_t>::_T())
			== std::nullopt);
#ifdef TAU_PACK_HAS_BA_BV
		using tau = tree<node_t>;
		const size_t bv8 = ba_descriptor<bv, node_t>::type_id_for(8);
		CHECK(pack_type_decides_closed<node_t>(bv8));
		tau::get_options opts{ .parse = { .start = tau::wff } };
		auto closed = [&](const char* src) {
			tref f = tau::get(src, opts).value_or(nullptr);
			REQUIRE(f != nullptr);
			return pack_decide_closed<node_t>(bv8, f);
		};
		CHECK(closed("all x:bv[8] ex y:bv[8] x * y = x")
			== std::optional<bool>{ true });
		CHECK(closed("ex x:bv[8] all y:bv[8] x + y = y")
			== std::optional<bool>{ true });
		CHECK(closed("ex x:bv[8] all y:bv[8] x * y = {1}:bv[8]")
			== std::optional<bool>{ false });
#endif
#ifdef TAU_PACK_HAS_BA_SBF
		const size_t sbf_id = tid(ba_descriptor<sbf_ba, node_t>::type_tree());
		CHECK_FALSE(pack_type_decides_closed<node_t>(sbf_id));
#endif
	}
	TEST_CASE("pack_dense_order_compare / pack_dense_order_between") {
		CHECK_FALSE(pack_type_is_dense_order<node_t>(size_t{0}));
		CHECK(pack_dense_order_between<node_t>(size_t{0}, nullptr, nullptr)
			== nullptr);
#ifdef TAU_PACK_HAS_BA_QLT
		const size_t q = qlt_type_id<node_t>();
		CHECK(pack_type_is_dense_order<node_t>(q));
		tref zero = pack_zero_constant<node_t>(q);
		tref mid = pack_dense_order_between<node_t>(q, nullptr, nullptr);
		REQUIRE(zero != nullptr);
		REQUIRE(mid != nullptr);
		CHECK(pack_dense_order_compare<node_t>(q, zero, mid) == 0);
		tref above = pack_dense_order_between<node_t>(q, zero, nullptr);
		tref below = pack_dense_order_between<node_t>(q, nullptr, zero);
		REQUIRE(above != nullptr);
		REQUIRE(below != nullptr);
		CHECK(pack_dense_order_compare<node_t>(q, below, zero) == -1);
		CHECK(pack_dense_order_compare<node_t>(q, above, zero) == 1);
		tref between = pack_dense_order_between<node_t>(q, below, zero);
		REQUIRE(between != nullptr);
		CHECK(pack_dense_order_compare<node_t>(q, below, between) == -1);
		CHECK(pack_dense_order_compare<node_t>(q, between, zero) == -1);
		// the type's 0 is no point of the order
		CHECK_FALSE(pack_dense_order_compare<node_t>(q, zero,
			build_bf_f_type<node_t>(q)).has_value());
		// no rational of the representation lies beyond the extreme ones
		auto point = [&](long long v) {
			const qlt_rational r(v, 1);
			return tau::get(tau::bf, tau::get_ba_constant(
				typename node_t::constant(qlt{ { { { r, qlt_bound::CLOSED },
					{ r, qlt_bound::CLOSED } } } }), q));
		};
		tref max = point(std::numeric_limits<long long>::max());
		tref min = point(std::numeric_limits<long long>::min() + 1);
		tref past_max = pack_dense_order_between<node_t>(q, max, nullptr);
		tref past_min = pack_dense_order_between<node_t>(q, nullptr, min);
		CHECK((!past_max
			|| pack_dense_order_compare<node_t>(q, past_max, max) == 1));
		CHECK((!past_min
			|| pack_dense_order_compare<node_t>(q, past_min, min) == -1));
#endif
	}
	TEST_CASE("pack_type_is_atomless agrees with every descriptor's flag") {
		pack_visit_all<node_t>([]<typename BA>() {
			using desc = ba_descriptor<BA, node_t>;
			const bool fold = pack_type_is_atomless<node_t>(tid(desc::type_tree()));
			CHECK(fold == desc::atomless);
		});
		CHECK_FALSE(pack_type_is_atomless<node_t>(tid(untyped_type<node_t>())));
		CHECK_FALSE(pack_type_is_atomless<node_t>(size_t{0}));
	}
	TEST_CASE("pack_type_is_non_aba_omcat agrees with every descriptor's flag") {
		pack_visit_all<node_t>([]<typename BA>() {
			using desc = ba_descriptor<BA, node_t>;
			const bool fold = pack_type_is_non_aba_omcat<node_t>(tid(desc::type_tree()));
			CHECK(fold == desc::non_aba_omcat);
		});
		CHECK_FALSE(pack_type_is_non_aba_omcat<node_t>(size_t{0}));
	}
	TEST_CASE("pack_type_output_always_satisfiable is the declared flag and nothing else") {
		pack_visit_all<node_t>([]<typename BA>() {
			using desc = ba_descriptor<BA, node_t>;
			const bool fold = pack_type_output_always_satisfiable<node_t>(tid(desc::type_tree()));
			CHECK(fold == ba_output_always_satisfiable_v<node_t, BA>);
		});
		CHECK_FALSE(pack_type_output_always_satisfiable<node_t>(size_t{0}));
	}
	TEST_CASE("pack_type_has_arith_ops short-circuits on the null type id") {
		CHECK_FALSE(pack_type_has_arith_ops<node_t>(size_t{0}));
		CHECK_FALSE(pack_type_has_arith_ops<node_t>(tid(untyped_type<node_t>())));
	}
	TEST_CASE("pack_type_has_codegen_witness is the declared capability of the owner") {
		pack_visit_all<node_t>([]<typename BA>() {
			using desc = ba_descriptor<BA, node_t>;
			const bool fold = pack_type_has_codegen_witness<node_t>(tid(desc::type_tree()));
			CHECK(fold == ba_has_codegen_witness<node_t, BA>);
		});
		CHECK_FALSE(pack_type_has_codegen_witness<node_t>(size_t{0}));
	}
	TEST_CASE("pack_omcat_qe declines a type without the theory") {
		CHECK_FALSE(pack_omcat_qe<node_t>(tid(untyped_type<node_t>()), nullptr, nullptr).has_value());
		CHECK_FALSE(pack_omcat_qe<node_t>(size_t{0}, nullptr, nullptr).has_value());
	}
	TEST_CASE("pack_omcat_qe_residual is nullptr without an owner or without the capability") {
		CHECK(pack_omcat_qe_residual<node_t>(tid(untyped_type<node_t>()), nullptr, nullptr) == nullptr);
		CHECK(pack_omcat_qe_residual<node_t>(size_t{0}, nullptr, nullptr) == nullptr);
		size_t without = 0;
		pack_visit_all<node_t>([&]<typename BA>() {
			if constexpr (!ba_has_omcat_qe_residual<node_t, BA>) {
				using desc = ba_descriptor<BA, node_t>;
				CHECK(pack_omcat_qe_residual<node_t>(tid(desc::type_tree()), nullptr, nullptr) == nullptr);
				++without;
			}
		});
#ifdef TAU_PACK_HAS_BA_SBF
		CHECK(without > 0);
#endif
	}
	TEST_CASE("pack_codegen_witness / pack_codegen_constant_expr are empty without an owner") {
		CHECK_FALSE(pack_codegen_witness<node_t>(size_t{0}, nullptr, nullptr).has_value());
		CHECK_FALSE(pack_codegen_constant_expr<node_t>(size_t{0}, nullptr).has_value());
	}
	TEST_CASE("pack_literal_incomplete: nullopt without an owner, an answer from one") {
		CHECK_FALSE(pack_literal_incomplete<node_t>(nullptr, "1").has_value());
		CHECK_FALSE(pack_literal_incomplete<node_t>(untyped_type<node_t>(), "1").has_value());
#ifdef TAU_PACK_HAS_BA_SBF
		tref sbf_t = ba_descriptor<sbf_ba, node_t>::type_tree();
		auto complete = pack_literal_incomplete<node_t>(sbf_t, "1");
		REQUIRE(complete.has_value());
		CHECK_FALSE(*complete);
#endif
	}
	TEST_CASE("pack_type_tree round-trips every family through pack_type_family_param") {
		pack_visit_all<node_t>([]<typename BA>() {
			using desc = ba_descriptor<BA, node_t>;
			tref t = pack_type_tree<node_t>(desc::type_name);
			REQUIRE(t != nullptr);
			auto fp = pack_type_family_param<node_t>(t);
			REQUIRE(fp.has_value());
			CHECK(fp->first == std::string(desc::type_name));
		});
		CHECK(pack_type_tree<node_t>("no_such_family") == nullptr);
		CHECK_FALSE(pack_type_family_param<node_t>(untyped_type<node_t>()).has_value());
#ifdef TAU_PACK_HAS_BA_BV
		tref b8 = pack_type_tree<node_t>("bv", 8);
		REQUIRE(b8 != nullptr);
		auto fp = pack_type_family_param<node_t>(b8);
		REQUIRE(fp.has_value());
		CHECK(fp->first == "bv");
		CHECK(fp->second == std::optional<unsigned short>(8));
#endif
	}
	TEST_CASE("pack_type_tree refuses a parameter for an unparameterised family") {
#ifdef TAU_PACK_HAS_BA_SBF
		CHECK(pack_type_tree<node_t>("sbf") != nullptr);
		CHECK(pack_type_tree<node_t>("sbf", 8) == nullptr);
#endif
#ifdef TAU_PACK_HAS_BA_BV
		CHECK(pack_type_tree<node_t>("bv", 8) != nullptr);
#endif
	}
	TEST_CASE("the eight comparison-hook existence folds") {
		const size_t none = tid(untyped_type<node_t>());
		CHECK_FALSE(pack_ba_type_has_wff_lt_hook<node_t>(none));
		CHECK_FALSE(pack_ba_type_has_wff_lt_hook<node_t>(size_t{0}));
#ifdef TAU_PACK_HAS_BA_BV
		const size_t bv8 = ba_descriptor<bv, node_t>::type_id_for(8);
		CHECK(pack_ba_type_has_wff_lt_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_nlt_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_lteq_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_nlteq_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_gt_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_ngt_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_gteq_hook<node_t>(bv8));
		CHECK(pack_ba_type_has_wff_ngteq_hook<node_t>(bv8));
#endif
#ifdef TAU_PACK_HAS_BA_SBF
		CHECK_FALSE(pack_ba_type_has_wff_lt_hook<node_t>(tid(ba_descriptor<sbf_ba, node_t>::type_tree())));
#endif
	}
}

TEST_SUITE("accumulating folds") {
	TEST_CASE("pack_preprocess is the identity when every BA's own switch is off") {
		tref fm = wff("x = 0");
		REQUIRE(fm != nullptr);
		pack_set_preprocessing<node_t>(false);
		auto pre = pack_preprocess<node_t>(fm);
		REQUIRE(pre.has_value());
		CHECK(pre.value() == fm);
	}
#ifdef TAU_PACK_HAS_BA_EXT
	TEST_CASE("pack_preprocess stops the chain and keeps the failing report") {
		tref fm = wff("x = 0");
		REQUIRE(fm != nullptr);
		ext_ba::fail_preprocess_ = true;
		auto pre = pack_preprocess<node_t>(fm);
		ext_ba::fail_preprocess_ = false;
		CHECK_FALSE(pre.has_value());
		CHECK(pre.has_error());
	}
#endif
	TEST_CASE("pack_can_solve declines a formula no BA can translate") {
		// every core caller asks pack_sat_status only after this says yes
		tref fm = wff("x = 0");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(pack_can_solve<node_t>(fm));
	}
#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("pack_sat_status answers definitely for a solvable formula") {
		tref sat = wff("x = { 1 }:bv[8]");
		tref unsat = wff("x = { 0 }:bv[8] && x = { 1 }:bv[8]");
		REQUIRE(sat != nullptr);
		REQUIRE(unsat != nullptr);
		REQUIRE(pack_can_solve<node_t>(sat));
		REQUIRE(pack_can_solve<node_t>(unsat));
		CHECK(pack_sat_status<node_t>(sat) == std::optional<bool>(true));
		CHECK(pack_sat_status<node_t>(unsat) == std::optional<bool>(false));
	}

	TEST_CASE("pack_solve hands a bv formula to bv by its type") {
		tref fm = wff("x = { 1 }:bv[8]");
		REQUIRE(fm != nullptr);
		const size_t bv8 = ba_descriptor<bv, node_t>::type_id_for(8);
		auto sol = pack_solve<node_t, solution<node_t>>(bv8, fm);
		REQUIRE(sol.has_value());
		CHECK(sol->size() == 1);
#ifdef TAU_PACK_HAS_BA_SBF
		const size_t sbf_id = tid(ba_descriptor<sbf_ba, node_t>::type_tree());
		CHECK_FALSE(pack_solve<node_t, solution<node_t>>(sbf_id, fm)
			.has_value());
#endif
	}
#endif
	TEST_CASE("component-factoring switch round-trips through the pack") {
		const bool before = pack_ba_component_factoring_enabled<node_t>();
		pack_set_ba_component_factoring<node_t>(!before);
		CHECK(pack_ba_component_factoring_enabled<node_t>()
			== (tau_pack::has_tau_ba ? !before : false));
		pack_set_ba_component_factoring<node_t>(before);
	}
}

TEST_SUITE("options") {
	TEST_CASE("pack_find_ba_option distinguishes its three outcomes") {
		auto r = pack_find_ba_option<node_t>("no_such_family", "blasting");
		CHECK(r.status == ba_option_lookup_status::no_such_family);
		CHECK(r.option == nullptr);
#ifdef TAU_PACK_HAS_BA_BV
		auto f = pack_find_ba_option<node_t>("bv", "blasting");
		CHECK(f.status == ba_option_lookup_status::found);
		REQUIRE(f.option != nullptr);
		CHECK(f.option->kind == ba_option_kind::flag);
		auto n = pack_find_ba_option<node_t>("bv", "no_such_option");
		CHECK(n.status == ba_option_lookup_status::no_such_option);
		CHECK(n.option == nullptr);
#endif
#ifdef TAU_PACK_HAS_BA_SBF
		CHECK(pack_find_ba_option<node_t>("sbf", "anything").status
			== ba_option_lookup_status::no_such_option);
#endif
	}
	TEST_CASE("pack_ba_options is de-duplicated per (family, name)") {
		const auto& opts = pack_ba_options<node_t>();
		for (size_t i = 0; i < opts.size(); ++i)
			for (size_t j = i + 1; j < opts.size(); ++j) {
				const bool dup = opts[i].family == opts[j].family
					&& std::string(opts[i].option.name)
						== opts[j].option.name;
				CHECK_FALSE(dup);
			}
	}
}

TEST_SUITE("carrier ranking") {
	TEST_CASE("ba_carrier_rank is a position in the comma list, -1 when absent") {
		static_assert(ba_carrier_rank("bv,sbf,bool", "bv") == 0);
		static_assert(ba_carrier_rank("bv,sbf,bool", "sbf") == 1);
		static_assert(ba_carrier_rank("bv,sbf,bool", "bool") == 2);
		static_assert(ba_carrier_rank("bv,sbf,bool", "b") == -1);
		static_assert(ba_carrier_rank("bv,sbf,bool", "bvx") == -1);
		static_assert(ba_carrier_rank("bv", "bv") == 0);
		static_assert(ba_carrier_rank("sbf", "bv") == -1);
		CHECK(true);
	}
	TEST_CASE("pack_bool_carrier_type is a type some BA declaring can_host_bool owns") {
		tref carrier = pack_bool_carrier_type<node_t>();
		REQUIRE(carrier != nullptr);
		bool owned_by_a_host = false;
		pack_visit_all<node_t>([&]<typename BA>() {
			if constexpr (ba_can_host_bool_v<node_t, BA>)
				if (ba_descriptor<BA, node_t>::matches_type(carrier))
					owned_by_a_host = true;
		});
		CHECK(owned_by_a_host);
	}
}
