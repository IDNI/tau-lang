// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_spec.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_spec"

namespace idni::tau_lang {

template <NodeType node>
tau_spec<node>::tau_spec() = default;

// read partially a part of a tau_spec
// (or multiple if a new line is encountered)
template <NodeType node>
bool tau_spec<node>::parse(const std::string& tau_spec_part) {
	std::istringstream ss(tau_spec_part);
	std::string line;
	while (std::getline(ss, line)) {
		parts_.emplace_back(line);
		parsed_.emplace_back(nullptr);
		if (!parse_part(parts_.size() - 1)) return false;
	}
	return true;
}

template <NodeType node>
bool tau_spec<node>::eof() const {
	return eof_;
}

template <NodeType node>
const std::vector<std::string>& tau_spec<node>::errors() const {
	return errors_;
}

template <NodeType node>
tref tau_spec<node>::get() {
	using tau = tree<node>;
	auto fail = [&]() -> tref {
		for (const auto& error : errors())
			TAU_LOG_ERROR << "[tau] " << error << "\n";
		return nullptr;
	};
	if (eof_) {
		errors_.push_back(last_eof_msg_);
		return fail();
	}
	tref ptree = build_parse_tree();
	if (!ptree) return fail();
	// DBG(TAU_LOG_TRACE << "ptree: " << ptree_to_str(ptree);)
	auto& defs = definitions<node>::instance();
	typename tau::get_options opts = {
		.definition_heads = defs.get_definition_heads(),
		.global_scope = defs.get_global_scope(),
		.context = defs.get_io_context()
	};
	tref spec = tau::get(tau_parser::tree::get(ptree), opts);
	if (!spec) return fail();
	DBG(TAU_LOG_TRACE << "getting spec: " << TAU_TO_STR(spec);)
	return spec;
}

template <NodeType node>
std::optional<rr<node>> tau_spec<node>::get_nso_rr() {
	tref spec = get();
	if (!spec) return {};
	return tau_lang::get_nso_rr<node>(spec);
}

template <NodeType node>
bool tau_spec<node>::parse_part(size_t part) {
	if (errors_.size()) return false;

	auto parse = [&](const std::string& input) {
		auto result = tau_parser::instance().parse(
			input.c_str(), input.size(),
			{ .start = tau_parser::spec_multiline });
		DBG(if (result.found) TAU_LOG_TRACE << "parse OK";)
		return result;
	};

	if (!eof_) current_part_ = parts_[part];
	else current_part_ += "\n" + parts_[part];
	DBG(TAU_LOG_TRACE << "parsing_part: " << part << " \"" << current_part_ << "\"";)
	auto result = parse(current_part_);
	if (result.found) {
		eof_ = false, last_eof_msg_ = "";
		parsed_[part] = result.get_shaped_tree2();
		if (!parsed_[part]) {
			DBG(TAU_LOG_TRACE << "parsed_[part] is nullptr";)
			return false;
		}
		DBG(TAU_LOG_TRACE << "prev_part_: " << prev_part_;)
		DBG(TAU_LOG_TRACE << "current_part_: " << current_part_;)
		prev_part_ = current_part_;
		current_part_ = "";
		DBG(TAU_LOG_TRACE << "current_part_: " << current_part_;)
		// DBG(TAU_LOG_TRACE << "parsed_[part]: " << ptree_to_str(parsed_[part]);)
		return true;
	}
	std::string error_msg = result.parse_error.to_str(
		tau_parser::error::info_lvl::INFO_BASIC);
	parsed_[part] = nullptr;
	DBG(TAU_LOG_TRACE << error_msg;)
	eof_ = error_msg.find("Unexpected end of file") != std::string::npos;
	if (eof_) {
		last_eof_msg_ = error_msg;
		return true;
	}

	// reparse with previous line to check if it is actually a continuation
	if (part > 0) {
		std::string with_prev = prev_part_ + "\n" + current_part_;
		DBG(TAU_LOG_TRACE << "with_prev: \"" << with_prev << "\"";)
		auto result_with_prev = parse(with_prev);
		if (!result_with_prev.found) {
			std::string error_msg = result_with_prev.parse_error.to_str(
				tau_parser::error::info_lvl::INFO_BASIC);
			DBG(TAU_LOG_TRACE << "error_msg_with_prev: \"" << error_msg << "\"";)
			eof_ = error_msg.find("Unexpected end of file") != std::string::npos;
			if (eof_) last_eof_msg_ = error_msg;
			DBG(TAU_LOG_TRACE << "eof_: " << eof_;)
			return true;
		} else {
			eof_ = false, last_eof_msg_ = "";
			// it is continuation of the previous line
			DBG(TAU_LOG_TRACE << "continuation at part: " << part << " of \"" << parts_[part] << "\"";)
			if (part > 0) {
				size_t p = part;
				do {
					--p;
					if (parsed_[p]) {
						parsed_[p] = nullptr;
						break;
					}
				} while (p > 0);
			}
			parsed_[part] = result_with_prev.get_shaped_tree2();

			if (parsed_[part]) {
				// DBG(TAU_LOG_TRACE << "parsed_[part]: " << ptree_to_str(parsed_[part]);)
			} else {
				DBG(TAU_LOG_TRACE << "parsed_[part] is nullptr";)
				return false;
			}
			DBG(TAU_LOG_TRACE << "prev_part_: " << prev_part_;)
			DBG(TAU_LOG_TRACE << "with_prev: \"" << with_prev << "\"";)
			DBG(TAU_LOG_TRACE << "current_part_: " << current_part_;)
			prev_part_ = with_prev;
			return true;
		}
	}

	DBG(TAU_LOG_TRACE << "parse failed: " << error_msg;)
	errors_.push_back(error_msg);
	return false;
}

template <NodeType node>
tref tau_spec<node>::build_parse_tree() {
	using tau = tree<node>;
	using ptree = tau_parser::tree;
	trefs ch;
	tref main = nullptr;

	typename tau::get_options opts;
	auto& defs = definitions<node>::instance();
	opts.context = defs.get_io_context();
	opts.definition_heads = defs.get_definition_heads();
	opts.global_scope = defs.get_global_scope();

	auto ptree_to_str = [&opts](tref n) {
		tref tn = tau::get(ptree::get(n), opts);
		return tn ? TAU_TO_STR(tn) : ptree::get(n).print_in_line_to_str();
	};

	for (size_t i = 0; i < parts_.size(); ++i) {
		if (parsed_[i]) {
			for (tref c : ptree::get(parsed_[i]).children()) {
				DBG(TAU_LOG_TRACE << "tau_spec::build_parse_tree spec part: " << ptree_to_str(c);)
				if (tau::get(c).is(tau::main)) {
					if (main == nullptr) {
						main = c;
					} else {
						std::stringstream ss;
						ss << "Multiple main formulas: \""
							<< ptree_to_str(main)
							<< "\" and \""
							<< ptree_to_str(c)
							<< "\"";
						errors_.push_back(ss.str());
						return nullptr;
					}
				} else ch.push_back(c);
			}
		}
	}
	if (errors_.empty() && !main) {
		std::stringstream ss;
		errors_.push_back((ss << "No main formula", ss.str()));
		DBG(TAU_LOG_TRACE << "children: " << ch.size();)
		DBG(for (auto c : ch) TAU_LOG_TRACE << "child: " << LOG_FM_DUMP(c);)
		return nullptr;
	}
	tau_parser::pnode spec{
		tau_parser::instance().literal(tau::spec), { 0, 0 } };
	tau_parser::pnode definitions{
		tau_parser::instance().literal(tau::definitions), { 0, 0 } };
	if (ch.size()) return ptree::get(spec, ptree::get(definitions, ch), main);
	else return ptree::get(spec, main);
}

} // namespace idni::tau_lang