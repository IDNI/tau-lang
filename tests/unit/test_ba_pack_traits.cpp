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
