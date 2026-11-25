#include <metered_tau.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <tuple>

using namespace idni::tau_lang::metered;

static void check(bool good, const char* label) {
    if (!good) { std::cerr << label << '\n'; std::exit(EXIT_FAILURE); }
}

static uint32_t word(std::span<const uint8_t>& bytes) {
    check(bytes.size() >= 4, "short result word");
    uint32_t n = 0;
    for (unsigned i = 0; i < 4; ++i) n |= uint32_t(bytes[i]) << (8*i);
    bytes = bytes.subspan(4);
    return n;
}

static std::string_view field(std::span<const uint8_t>& bytes) {
    const auto size = word(bytes);
    check(bytes.size() >= size, "short result field");
    const auto* data = reinterpret_cast<const char*>(bytes.data());
    bytes = bytes.subspan(size);
    return {data, size};
}

static void finished(const receipt& r, uint64_t used) {
    check(r.completion == status::finished && r.tau_code == 0 &&
          r.value && r.used == used, "finished receipt");
}

static void stopped(const receipt& r, uint64_t used) {
    check(r.completion == status::budget && !r.value && !r.tau_code &&
          r.used == used, "budget receipt");
}

static void output(const receipt& r, std::string_view name, uint32_t time,
                   std::string_view value) {
    check(r.value.has_value(), "missing step bytes");
    std::span<const uint8_t> bytes(*r.value);
    field(bytes); // Diagnostics precede the typed outputs.
    check(word(bytes) == 1 && field(bytes) == name && word(bytes) == time &&
          field(bytes) == value && bytes.empty(), "step output");
}

static void unchanged(interpreter& i, const receipt& before) {
    constexpr uint64_t hi = 1'000'000'000;
    const auto after = i.inspect(hi);
    check(after.completion == status::finished && after.tau_code == 0 &&
          before.value && after.value == before.value &&
          after.used == before.used, "refusal changed interpreter");
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: tau-metered-replay IMAGE_DIRECTORY\n";
        return 2;
    }
    try {
        engine invalid_images(std::filesystem::path(argv[1]) / "tau.raw.wasm");
        const auto bad_profile = invalid_images.get_interpreter("o1[t]:sbf = 1.", 1);
        check(bad_profile.report.completion == status::operational &&
              bad_profile.report.phase == "profile" && bad_profile.report.used == 0 &&
              !bad_profile.report.value && !bad_profile.value, "invalid profile report");
        const engine tau(argv[1]);
        constexpr uint64_t hi = 1'000'000'000;
        constexpr auto temporal = "(i1[t]:qlt > {0}:qlt) until ({0}:qlt < o1[t]:qlt).";
        {
            auto bootstrap = tau.get_interpreter(temporal, 0);
            stopped(bootstrap.report, 0);
            check(!bootstrap.value, "bootstrap published an interpreter");
            auto child = tau.get_interpreter(temporal, 162'367'194);
            stopped(child.report, 162'367'194);
            check(!child.value && child.report.phase == "spot" &&
                  !child.report.solvers.empty() &&
                  child.report.solvers.back().completion == status::budget,
                  "Spot cap is not a charged refusal");
            auto publish = tau.get_interpreter(temporal, 196'038'036);
            stopped(publish.report, 196'038'036);
            check(!publish.value && publish.report.phase == "publish", "admission cap");
            auto a = tau.get_interpreter(temporal, 196'038'037);
            finished(a.report, 196'038'037);
            check(a.value.has_value(), "missing temporal interpreter");
            auto& i = *a.value;
            const auto before = i.inspect(hi);
            stopped(i.step({}, 4'079'684), 4'079'684);
            unchanged(i, before);
            const auto s = i.step({}, 4'079'685);
            finished(s, 4'079'685);
            output(s, "o1", 0, "1");
        }
        {
            const std::string digits(95, '0');
            auto a = tau.get_interpreter(
                "((o5[t]:bv[384] + i2[t]:bv[384]) = { #x" + digits + "1 }:bv[384]).",
                199'694'636);
            finished(a.report, 199'694'636);
            check(a.value.has_value(), "missing bit-vector interpreter");
            const std::array in{input{"i2", 0, "{ #x" + digits + "2 }:bv[384]"}};
            const auto s = a.value->step(in, 51'639'467);
            finished(s, 51'639'467);
            output(s, "o5", 0,
                   "3940200619639447921227904010014361380507973927046544666794829"
                   "3404245721771497210611414266254884915640806627990306815");
        }
        for (const auto [update, cost, accepted, next] : std::array{
                 std::tuple{"G (o1[t]:qlt = i1[t-1]:qlt)", 197'025'898ULL, 1U, 11'419'558ULL},
                 std::tuple{"F", 10'898'428ULL, 0U, 5'048'395ULL}}) {
            auto a = tau.get_interpreter("G (o1[t]:qlt = i1[t]:qlt).", 140'961'755);
            finished(a.report, 140'961'755);
            check(a.value.has_value(), "missing update interpreter");
            auto& i = *a.value;
            const std::array first{input{"i1", 0, "{1}:qlt"}};
            const std::array second{input{"i1", 1, "{2}:qlt"}};
            finished(i.step(first, 4'933'590), 4'933'590);
            const auto before = i.inspect(hi);
            stopped(i.update(update, cost - 1), cost - 1);
            unchanged(i, before);
            const auto u = i.update(update, cost);
            finished(u, cost);
            std::span<const uint8_t> bytes(*u.value);
            field(bytes);
            check(word(bytes) == accepted && bytes.empty(), "update decision");
            if (!accepted) unchanged(i, before);
            const auto s = i.step(second, next);
            finished(s, next);
            output(s, "o1", 1, "2");
        }
        std::cout << "Pinned profile replay passed: Spot, BV384, caps and update rollback\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
