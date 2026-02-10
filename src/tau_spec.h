// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_SPEC_H__
#define __IDNI__TAU__TAU_SPEC_H__

#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct tau_spec {
	using tau = tree<node>;

	tau_spec();
	bool parse(const std::string& tau_spec_part);
	bool is_eof() const;
	const std::vector<std::string>& errors() const;
	tref get();
	std::optional<rr<node>> get_nso_rr();

private:
	typename tau::get_options get_options() const;
	bool eof_check(const std::string& error_msg);
	std::pair<bool, std::string> parse_(
		const std::string& input,
		size_t part);
	bool parse_part(size_t part);
	bool parse_with_prev_part(size_t part);
	tref build_parse_tree();

	std::string current_part_{};
	std::string prev_part_{};
	std::vector<std::string> parts_{}; // TODO remove this if found unnecessary
	std::deque<tref> parsed_{};
	std::optional<std::string> eof_msg_{};
	std::vector<std::string> errors_{};
};

template <NodeType node>
std::ostream& operator<<(std::ostream& os, const tau_spec<node>& spec);

} // namespace idni::tau_lang

#include "tau_spec.tmpl.h"

#endif // __IDNI__TAU__TAU_SPEC_H__