#ifndef IDNI_TAU_RULE_INSPECTION_H
#define IDNI_TAU_RULE_INSPECTION_H
#include "tau_diagnostics.h"

#include <cstdint>
#include <map>
#include <string>

namespace idni::tau_lang {

enum class rule_inspection_status { described, unsupported };
struct rule_read_metadata {
    std::uint32_t width;
    std::uint32_t deepest_lookback;
    bool operator==(const rule_read_metadata&) const = default;
};
struct rule_inspection {
    rule_inspection_status status = rule_inspection_status::unsupported;
    std::string reason;
    std::map<std::string, rule_read_metadata> reads;
    std::map<std::string, std::uint32_t> writes;
    std::uint32_t required_history = 0;
    report diagnostics;
};

/// Serialized configured-pack API. No interpreter is built. Described means
/// functional shape for the restricted complete typed environment; it is not
/// a startup proof. All described streams are concrete bitvectors. Unsupported has no metadata, and does not prove the
/// input is nonfunctional. Offsets are syntactic, not semantic dependencies.
rule_inspection describe(const std::string& source);

}
#endif
