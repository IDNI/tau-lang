#ifndef IDNI_TAU_RULE_DESCRIPTION_H
#define IDNI_TAU_RULE_DESCRIPTION_H

#include <cstdint>
#include <map>
#include <string>
#include <string_view>

namespace idni::tau_lang::describe_detail {

enum class source_status { unsupported, parse_error, matched };
struct source_facts {
	source_status status = source_status::unsupported;
	std::map<std::string, std::uint32_t> lookbacks;
	std::string output;
};

source_facts describe_source(std::string_view source);

} // namespace idni::tau_lang::describe_detail
#endif
