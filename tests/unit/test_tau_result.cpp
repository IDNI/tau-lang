// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau_result.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

namespace {

using namespace idni::tau_lang;

static tau_diagnostic mk(level l, code c, std::string m) {
	return tau_diagnostic{ .level = l, .code = c, .message = std::move(m) };
}

static tau_result<int> child_ok() {
	return tau_result<int>(7, {
		mk(level::warning, code::invalid_argument, "child-warning"),
		mk(level::info, code::unknown, "child-info")
	});
}

static tau_result<int> child_fail() {
	return fail<int>({
		mk(level::error, code::invalid_state, "child-error")
	});
}

static tau_result<int> parent_ok_with_absorb() {
	tau_result<int> r;
	auto v = r.absorb(child_ok());
	if (!v) return r;
	r.log_info(code::unknown, "parent-info");
	r.set_value(*v + 1);
	return r;
}

static tau_result<int> parent_fail_with_absorb() {
	tau_result<int> r;
	r.log_warning(code::unknown, "parent-warning-before");
	auto v = r.absorb(child_fail());
	if (!v) return r;
	r.set_value(1);
	return r;
}

TEST_SUITE("tau_result") {

TEST_CASE("invariant: error diagnostic clears value") {
	tau_result<int> r(42);
	CHECK(r.has_value());
	r.add_diagnostic(mk(level::error, code::internal_error, "boom"));
	CHECK_FALSE(r.has_value());
	CHECK(r.errors().size() == 1);
}

TEST_CASE("warnings and infos keep value") {
	tau_result<int> r(11);
	r.add_diagnostic(mk(level::warning, code::unknown, "w"));
	r.add_diagnostic(mk(level::info, code::unknown, "i"));
	CHECK(r.has_value());
	CHECK(r.value() == 11);
	CHECK(r.warnings().size() == 1);
	CHECK(r.infos().size() == 1);
	CHECK(r.errors().empty());
}

TEST_CASE("diagnostic filtering and order") {
	tau_result<int> r(1);
	r.add_diagnostic(mk(level::info, code::unknown, "i1"));
	r.add_diagnostic(mk(level::warning, code::unknown, "w1"));
	r.add_diagnostic(mk(level::info, code::unknown, "i2"));
	const auto& all = r.diagnostics();
	REQUIRE(all.size() == 3);
	CHECK(all[0].message == "i1");
	CHECK(all[1].message == "w1");
	CHECK(all[2].message == "i2");
	auto infos = r.infos();
	REQUIRE(infos.size() == 2);
	CHECK(infos[0].message == "i1");
	CHECK(infos[1].message == "i2");
}

TEST_CASE("absorb merges diagnostics and returns value") {
	auto r = parent_ok_with_absorb();
	REQUIRE(r.has_value());
	CHECK(r.value() == 8);
	const auto& d = r.diagnostics();
	REQUIRE(d.size() == 3);
	CHECK(d[0].message == "child-warning");
	CHECK(d[1].message == "child-info");
	CHECK(d[2].message == "parent-info");
}

TEST_CASE("absorb early-return on failure preserves order") {
	auto r = parent_fail_with_absorb();
	CHECK_FALSE(r.has_value());
	const auto& d = r.diagnostics();
	REQUIRE(d.size() == 2);
	CHECK(d[0].message == "parent-warning-before");
	CHECK(d[1].message == "child-error");
	CHECK(r.errors().size() == 1);
}

TEST_CASE("absorb propagates measured as sub-part") {
	tau_result<int> parent;
	parent.measured().name = "parent";

	tau_result<int> child(42);
	child.measured().name = "child";
	child.measured().ms   = 1.5;

	auto v = parent.absorb(std::move(child));
	REQUIRE(v.has_value());
	CHECK(*v == 42);
	REQUIRE(parent.measured().parts.size() == 1);
	CHECK(parent.measured().parts[0].name == "child");
	CHECK(parent.measured().parts[0].ms   > 0);
}

TEST_CASE("absorb_into discards child value, merges diagnostics and measured") {
	tau_result<int> parent(99);
	tau_result<std::string> child;
	child.log_warning(code::unknown, "side-effect");
	child.measured().name = "side-op";

	parent.absorb_into(std::move(child));
	CHECK(parent.has_value());
	CHECK(parent.value() == 99);
	CHECK(parent.warnings().size() == 1);
	REQUIRE(parent.measured().parts.size() == 1);
	CHECK(parent.measured().parts[0].name == "side-op");
}

TEST_CASE("log_error/warning/info methods") {
	tau_result<int> r(5);
	r.log_info(code::unknown, "note");
	r.log_warning(code::invalid_argument, "warn");
	CHECK(r.has_value());
	CHECK(r.infos().size() == 1);
	CHECK(r.warnings().size() == 1);
	r.log_error(code::internal_error, "fail");
	CHECK_FALSE(r.has_value());
	CHECK(r.errors().size() == 1);
}

TEST_CASE("log_error/warning/info free factories") {
	auto e = log_error(code::type_error, "bad type");
	CHECK(e.level == level::error);
	CHECK(e.code  == code::type_error);
	CHECK(e.message == "bad type");

	auto w = log_warning(code::unknown, "heads up");
	CHECK(w.level == level::warning);

	auto i = log_info(code::unknown, "fyi");
	CHECK(i.level == level::info);
}

TEST_CASE("fail<T> factory") {
	auto r = fail<int>({ log_error(code::invalid_state, "bad") });
	CHECK_FALSE(r.has_value());
	REQUIRE(r.errors().size() == 1);
	CHECK(r.errors()[0].message == "bad");
}

TEST_CASE("bool_result") {
	tau_result<bool> r(true);
	CHECK(r.has_value());
	CHECK(r.value() == true);
	r.log_error(code::internal_error, "boom");
	CHECK_FALSE(r.has_value());
}

TEST_CASE("code numeric stability") {
	CHECK(static_cast<uint32_t>(code::unknown) == 0);
	CHECK(static_cast<uint32_t>(code::invalid_argument) == 1);
	CHECK(static_cast<uint32_t>(code::internal_error) == 2);
	CHECK(static_cast<uint32_t>(code::solver_error) == 3);
	CHECK(static_cast<uint32_t>(code::io_error) == 4);
	CHECK(static_cast<uint32_t>(code::type_error) == 5);
	CHECK(static_cast<uint32_t>(code::unsupported_operation) == 6);
	CHECK(static_cast<uint32_t>(code::invalid_state) == 7);
	CHECK(static_cast<uint32_t>(code::not_found) == 8);
	CHECK(static_cast<uint32_t>(code::unsat) == 9);
}

}

} // namespace
