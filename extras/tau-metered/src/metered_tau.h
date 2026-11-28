#pragma once
#include <cstdint>
#include <filesystem>
#include <memory>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace idni::tau_lang::metered {

enum class status { finished, budget, unsupported, operational, invalid };
struct marker { std::string phase; uint64_t used; };
struct solver_call {
    unsigned operation;
    std::string source;
    std::vector<std::string> outputs;
    uint64_t charged_before, used;
    status completion;
    std::optional<std::string> answer_sha;
};

struct receipt {
    status completion = status::operational;
    uint64_t used = 0;
    std::optional<uint64_t> diagnostic_used;
    std::string phase, why;
    std::optional<uint32_t> site;
    std::optional<int32_t> tau_code;
    // The versioned Tau result packet. Absent in all cap/operational/refusal
    // cases, which cannot be confused with Tau's ordinary false results.
    std::optional<std::vector<uint8_t>> value;
    std::vector<marker> trace;
    std::map<std::string, unsigned> attempts;
    std::vector<solver_call> solvers;
};
struct input { std::string name; uint32_t time; std::string value; };
struct profile;
class interpreter {
public:
    ~interpreter();
    interpreter(interpreter&&) noexcept;
    interpreter& operator=(interpreter&&) noexcept;
    interpreter(const interpreter&) = delete;
    interpreter& operator=(const interpreter&) = delete;
    // Includes typed input admission, sort and serialization in the same meter.
    receipt step(std::span<const input>, uint64_t work_budget);
    receipt update(std::string_view source, uint64_t work_budget);
    receipt inspect(uint64_t work_budget) const;
private:
    struct core;
    std::unique_ptr<core> private_;
    explicit interpreter(std::unique_ptr<core>);
    friend class engine;
};
struct admission { receipt report; std::optional<interpreter> value; };

class engine {
public:
    // Only the qualified, guarded execution profile is accepted. The five
    // modules must be provisioned locally at the given immutable image root.
    explicit engine(const std::filesystem::path& image_root);
    static constexpr std::string_view profile_id() noexcept {
        return "tau-work-budget/1:a8c5d8d841bb267fc0b76f0f908c2afeef94d0c0dd1a336441c4972c40062962";
    }
    admission get_interpreter(std::string_view source, uint64_t work_budget) const;
private:
    std::shared_ptr<profile> profile_;
    std::optional<receipt> startup_error_;
};
}
