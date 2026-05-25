// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("Tau_splitter_upper_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(G ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (F o1[t] != 0 && o1[t] != 1) && (F o1[t] = 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}


	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(G ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (F o1[t] != 0 && o1[t] != 1) && (F o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(F o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(G o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_9") {
		const char *sample = "(G o1[t] = i1[t]).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_10") {
		const char *sample = "(G o1[0] = 0) && (F o1[0]|o1[1] != 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_11") {
		const char *sample = "xy = 0 && x != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s ));
	}

	TEST_CASE("Tau_splitter_12") {
		const char *sample = "{<:a><:b>|<:c><:d> = 0} & y = 0 && {<:a><:b>|<:c><:d> = 0} & y < {<:a><:b>|<:c><:d> = 0}.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_13") {
		const char *sample = "{<:a><:b>|<:c><:d> = 0} & y != 0 && { <:a><:b>|<:c><:d> = 0 } : tau ({ <:a><:b>|<:c><:d> = 0 } : tau y)' != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_14") {
		const char *sample = "x = 0 && y != 0 || z = 0 && k = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}
}

// SO-26: the formula-level ("nso") middle/lower splitter suites that used to
// live here are gone for good reason, not just commented out by accident:
// splitter.tmpl.h's split_path() explicitly does `assert(false)` for
// splitter_type::middle and splitter_type::lower ("For now we only support
// upper splitters ... TODO: bring back middle and lower splitter?"), so
// nso_tau_splitter()/tau_splitter(tref, splitter_type) (the overload behind
// get_nso_rr_tau_splitter()) cannot be exercised with middle/lower without
// hitting that assertion in debug builds (and silently no-op'ing in release).
//
// Per review finding SO-20, middle and lower are, however, still fully
// implemented one layer down, directly on BA constants: see
// bdd_handle.h's hbdd::splitter(), which dispatches to
// rm_all_except_one_clause() (lower) and rm_half_clauses() (middle). That is
// the code path reachable via the constant-node tau_splitter(const tree&,
// splitter_type) overload (splitter.tmpl.h:44) used by e.g. sbf_splitter().
// The suites below exercise that surviving code path directly on sbf_ba
// constants instead of going through the disabled formula-level splitter.
namespace idni::tau_lang {
inline sbf_ba parse_sbf_value(const char* src) {
	auto opt = parse_sbf<bv, sbf_ba>(src);
	assert(opt.has_value());
	return std::get<sbf_ba>(opt.value().first);
}
} // namespace idni::tau_lang

TEST_SUITE("Tau_splitter_middle_tests") {
	TEST_CASE("Tau_splitter_1: a|b") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a|b");
		sbf_ba s = sbf_splitter(elem, splitter_type::middle);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_2: a|b|c") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a|b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::middle);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_3: a&b|c") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a&b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::middle);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_4: single clause (a) falls back to a bad splitter") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a");
		sbf_ba s = sbf_splitter(elem, splitter_type::middle);
		// Already a single clause: rm_half_clauses() collapses to F, which
		// bdd_handle::splitter() rejects (it requires the result to be
		// neither F nor unchanged), so it silently falls back to the
		// generic "bad splitter" (split_clause: conjunct with a fresh,
		// never-used variable). That fallback is still a valid splitter:
		// satisfiable and a strict subset of the original.
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}
#ifndef DEBUG
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(G ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (F o1[t] != 0 && o1[t] != 1) && (F o1[t] = 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(G ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (F o1[t] != 0 && o1[t] != 1) && (F o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
#endif
	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(F o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(G o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
}

TEST_SUITE("Tau_splitter_lower_tests") {
	TEST_CASE("Tau_splitter_1: a|b") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a|b");
		sbf_ba s = sbf_splitter(elem, splitter_type::lower);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_2: a|b|c") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a|b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::lower);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_3: a&b|c") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a&b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::lower);
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}

	TEST_CASE("Tau_splitter_4: single clause (a) falls back to a bad splitter") {
		bdd_init<Bool>();
		sbf_ba elem = parse_sbf_value("a");
		sbf_ba s = sbf_splitter(elem, splitter_type::lower);
		// Already a single clause: rm_all_except_one_clause() returns the
		// bdd unchanged, which bdd_handle::splitter() rejects (it requires
		// the result to differ from the input), so it falls back to the
		// same generic "bad splitter" fallback as the middle-splitter case
		// above. Still a valid splitter: satisfiable and a strict subset.
		CHECK(!is_sbf_zero(s));
		CHECK(s != elem);
		CHECK((s & elem) == s);
	}
}

TEST_SUITE("Tau_splitter_coeff") {

	TEST_CASE("Tau_splitter_coeff1") {
		bdd_init<Bool>();
		const char *sample = "{x3}:sbf & {y}:sbf w != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_coeff2") {
		bdd_init<Bool>();
		const char *sample = "({x}:sbf|{y}:sbf) w != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_coeff3") {
		bdd_init<Bool>();
		const char *sample = "{x}:sbf f = 0 && {y}:sbf g = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}
}

TEST_SUITE("Tau_splitter_uniter_const") {
	TEST_CASE("Tau_splitter_tau_uniter_const") {
		bdd_init<Bool>();
		const char *src = "<:split1> = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(src, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		auto s_str = tau::get(s).to_str();
		CHECK(s_str == "<:split1> = 0 && <:split2> != 0");
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}