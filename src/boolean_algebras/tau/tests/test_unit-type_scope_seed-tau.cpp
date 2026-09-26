// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau.h"

using namespace idni::tau_lang;
using node_t = tau_pack::node_t;
using tau = tree<node_t>;

namespace {

// a formula parsed without inference, as infer_ba_types receives it
tref untyped(const char* src) {
	tau::get_options opts;
	opts.parse.start = tau::wff;
	opts.infer_ba_types = false;
	opts.reget_with_hooks = false;
	tref fm = tau::get(src, opts).value_or(nullptr);
	REQUIRE(fm != nullptr);
	return fm;
}

// a specification (definitions and main) parsed without inference
tref untyped_spec(const char* src) {
	tau::get_options opts;
	opts.infer_ba_types = false;
	opts.reget_with_hooks = false;
	tref sp = tau::get(src, opts).value_or(nullptr);
	REQUIRE(sp != nullptr);
	return sp;
}

// a global scope of `n` output streams o1 .. on, keyed as the I/O context
// keys them (build_canonized_io_var): o5 of the sbf type, the rest tau, so
// that a stream typed from the scope is told apart from the tau default
subtree_map<node_t, size_t> scope_of(size_t n) {
	subtree_map<node_t, size_t> scope;
	const size_t tau_id = get_ba_type_id<node_t>(tau_type<node_t>());
	const size_t sbf_id = get_ba_type_id<node_t>(sbf_type<node_t>());
	for (size_t i = 1; i <= n; ++i)
		scope[build_canonized_io_var<node_t>("o" + std::to_string(i))] =
			i == 5 ? sbf_id : tau_id;
	return scope;
}

size_t type_of(const subtree_map<node_t, size_t>& m, const char* name) {
	auto it = m.find(build_canonized_io_var<node_t>(name));
	return it == m.end() ? 0 : it->second;
}

// `type_scope_seed_mode` lets the environment variable TAU_TYPE_SCOPE_SEED
// override the flag; run with it unset.
struct seed_mode {
	int saved = type_scope_seed;
	explicit seed_mode(int mode) {
		type_scope_seed = mode;
		type_scope_seed_hits = 0;
		type_scope_seed_mismatches = 0;
	}
	~seed_mode() { type_scope_seed = saved; }
};

} // namespace

// The inference of a formula depends on the scope entries of the streams
// it mentions and on no other: seeded by mention it gives the same tree
// and the same types as seeded from the whole scope. o5 carries the sbf
// type in the scope, so a stream typed from the scope is told apart from
// the tau default the inference falls back to.
TEST_SUITE("type scope seed") {

	TEST_CASE("by mention gives the tree of the whole scope") {
		auto scope = scope_of(800);
		tref fm = untyped("o5[t] = 0 && o777[t] != 0");
		tref whole, mention;
		{
			seed_mode m(0);
			whole = infer_ba_types<node_t>(fm, &scope, nullptr, {}).first;
		}
		{
			seed_mode m(1);
			mention = infer_ba_types<node_t>(fm, &scope, nullptr, {}).first;
		}
		REQUIRE(whole != nullptr);
		CHECK(whole == mention);
	}

	TEST_CASE("by mention types the mentioned streams as the scope does") {
		auto scope = scope_of(300);
		const size_t sbf_id = get_ba_type_id<node_t>(sbf_type<node_t>());
		REQUIRE(type_of(scope, "o5") == sbf_id);
		tref fm = untyped("o5[t] = 0 && o77[t] != 0");
		seed_mode m(1);
		auto [tree, types] = infer_ba_types<node_t>(fm, &scope, nullptr, {});
		REQUIRE(tree != nullptr);
		// o5 takes the scope's sbf, not the tau default; o77 the scope's tau
		CHECK(type_of(types, "o5") == sbf_id);
		CHECK(type_of(types, "o77") == type_of(scope, "o77"));
		CHECK(type_scope_seed_hits == 1);
	}

	TEST_CASE("without defaults: known stream from the scope, unknown untyped") {
		auto scope = scope_of(100);
		const size_t sbf_id = get_ba_type_id<node_t>(sbf_type<node_t>());
		tref fm = untyped("o5[t] = 0 && o9999[t] != 0");
		type_inference_options no_defaults{ .use_defaults = false };
		tref whole, mention;
		subtree_map<node_t, size_t> tw, tm;
		{
			seed_mode m(0);
			auto r = infer_ba_types<node_t>(fm, &scope, nullptr, no_defaults);
			whole = r.first; tw = r.second;
		}
		{
			seed_mode m(1);
			auto r = infer_ba_types<node_t>(fm, &scope, nullptr, no_defaults);
			mention = r.first; tm = r.second;
		}
		CHECK(whole == mention);
		CHECK(type_of(tm, "o5") == sbf_id);
		CHECK(type_of(tm, "o9999") == type_of(tw, "o9999"));
		CHECK(type_of(tm, "o9999") != sbf_id);
	}

	TEST_CASE("a stream the scope does not know is typed by the inference") {
		auto scope = scope_of(50);
		tref fm = untyped("o5[t] = 0 && o9999[t] != 0");
		tref whole, mention;
		subtree_map<node_t, size_t> tw, tm;
		{
			seed_mode m(0);
			auto r = infer_ba_types<node_t>(fm, &scope, nullptr, {});
			whole = r.first; tw = r.second;
		}
		{
			seed_mode m(1);
			auto r = infer_ba_types<node_t>(fm, &scope, nullptr, {});
			mention = r.first; tm = r.second;
		}
		REQUIRE(whole != nullptr);
		CHECK(whole == mention);
		CHECK(type_of(tw, "o9999") == type_of(tm, "o9999"));
	}

	// The parse with inference (tree::get with a global scope) merges its
	// result into the scope it was given: a stream the line does not
	// mention keeps its entry, the mentioned one keeps the scope's type.
	TEST_CASE("a parse with a global scope keeps the unmentioned streams") {
		auto scope = scope_of(20);
		const size_t sbf_id = get_ba_type_id<node_t>(sbf_type<node_t>());
		seed_mode m(1);
		tau::get_options opts;
		opts.parse.start = tau::wff;
		opts.global_scope = &scope;
		tref fm = tau::get("o5[t] = 0", opts).value_or(nullptr);
		REQUIRE(fm != nullptr);
		CHECK(scope.size() == 20);
		CHECK(type_of(scope, "o5") == sbf_id);
		CHECK(type_of(scope, "o6") == get_ba_type_id<node_t>(tau_type<node_t>()));
	}

	TEST_CASE("the shadow mode counts no disagreement") {
		auto scope = scope_of(400);
		seed_mode m(2);
		for (const char* src : { "o1[t] = 0", "o2[t] != 0 && o399[t] = 1",
				"(o3[t] = 0 || o4[t] != 0) && o400[t] = 0" }) {
			tref fm = untyped(src);
			auto r = infer_ba_types<node_t>(fm, &scope, nullptr, {});
			REQUIRE(r.first != nullptr);
		}
		CHECK(type_scope_seed_hits == 3);
		CHECK(type_scope_seed_mismatches == 0);
	}

	// An input or output definition mentions its stream by name, without
	// an io variable: the definition's stream is seeded from the scope as
	// well, so a definition contradicting the scope's type is rejected in
	// both modes, and an untyped one takes the scope's type.
	TEST_CASE("the stream an I/O definition declares is seeded") {
		auto scope = scope_of(30);
		const size_t sbf_id = get_ba_type_id<node_t>(sbf_type<node_t>());
		scope[build_canonized_io_var<node_t>("i1")] = sbf_id;
		tref conflicting = untyped_spec("i1:tau := in console. o7[t] = 0.");
		tref untyped_def = untyped_spec("i1 := in console. o7[t] = 0.");
		tref whole, mention;
		subtree_map<node_t, size_t> tm;
		{
			seed_mode m(0);
			whole = infer_ba_types<node_t>(conflicting, &scope, nullptr, {}).first;
		}
		{
			seed_mode m(1);
			mention = infer_ba_types<node_t>(conflicting, &scope, nullptr, {}).first;
			auto r = infer_ba_types<node_t>(untyped_def, &scope, nullptr, {});
			tm = r.second;
			REQUIRE(r.first != nullptr);
		}
		CHECK(whole == nullptr);
		CHECK(mention == nullptr);
		CHECK(type_of(tm, "i1") == sbf_id);
	}
}
