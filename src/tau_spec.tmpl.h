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
bool tau_spec<node>::is_eof() const {
	return eof_msg_.has_value();
}

template <NodeType node>
const std::vector<std::string>& tau_spec<node>::errors() const {
	return errors_;
}

template <NodeType node>
tref tau_spec<node>::get() {
	auto fail = [&]() -> tref {
		DBG(for (const auto& error : errors())
		 	TAU_LOG_TRACE << "[tau] " << error << "\n";)
		return nullptr;
	};

	if (is_eof()) {
		errors_.push_back(eof_msg_.value());
		return fail();
	}

	tref ptree = build_parse_tree();
	if (!ptree) return fail();

	auto opts = get_options(); // transform to tau tree
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
typename tree<node>::get_options tau_spec<node>::get_options() const {
	auto& defs = definitions<node>::instance();
	return typename tau::get_options{ .parse = { .start = tau::spec_multiline },
		.definition_heads = defs.get_definition_heads(),
		.global_scope = defs.get_global_scope(),
		.context = defs.get_io_context()
	};
}
template <NodeType node>
bool tau_spec<node>::eof_check(const std::string& error_msg) {
	if (error_msg.find("Unexpected end of file") != std::string::npos) {
		eof_msg_ = error_msg;
		return true;
	}
	return false;
}

template <NodeType node>
std::pair<bool, std::string> tau_spec<node>::parse_(
	const std::string& input, size_t part)
{
	auto result = tau_parser::instance().parse(
		input.c_str(), input.size(),
		{ .start = tau_parser::spec_multiline });
	if (result.found) {
		DBG(TAU_LOG_TRACE << "parse OK";)
		eof_msg_ = {}; // clear eof
		parsed_[part] = result.get_shaped_tree2();
		if (!parsed_[part]) { // this should not happen
			DBG(assert(parsed_[part] != nullptr);)
			return { false, "Invalid parse tree" };
		}
		return { true, "" };
	} else {
		auto error_msg = result.parse_error.to_str(
				tau_parser::error::info_lvl::INFO_BASIC);
		DBG(TAU_LOG_TRACE << "parse failed: " << error_msg;)
		return { false, error_msg };
	}
};

template <NodeType node>
bool tau_spec<node>::parse_part(size_t part) {
	if (errors_.size()) return false;

	if (!eof_msg_) current_part_ = parts_[part];
	else current_part_ += "\n" + parts_[part];
	DBG(TAU_LOG_TRACE << "parsing_part: " << part << " \"" << current_part_ << "\"";)

	auto [found, error_msg] = parse_(current_part_, part);
	if (found) {
		prev_part_ = current_part_; // save for possible continuation
		current_part_ = "";
		return true; // success
	} else {
		parsed_[part] = nullptr;
		if (eof_check(error_msg)) return true; // expect more input
		// if error other than eof, reparse with previous part to
		// check if it is actually a continuation
		if (parse_with_prev_part(part)) return true;
		// if another error, fail with error message
		errors_.push_back(error_msg);
		return false;
	}
}

template <NodeType node>
bool tau_spec<node>::parse_with_prev_part(size_t part) {
	if (part == 0) return false;

	std::string with_prev = prev_part_ + "\n" + current_part_;
	DBG(TAU_LOG_TRACE << "parsing with_prev: " << part << " \"" << with_prev << "\"";)

	auto [found, error_msg] = parse_(with_prev, part);
	if (found) { // it is a continuation of the previous part
		// clear previous parsed part
		for (size_t p = part; p > 0; --p) if (parsed_[p - 1]) {
			parsed_[p - 1] = nullptr; break;
		}
		prev_part_ = with_prev; // save for possible continuation
		return true; // success
	} else {
		if (eof_check(error_msg)) return true; // expect more input
		// if error other than eof, parse attempt failed, return false silently
		return false;
	}
}

template <NodeType node>
tref tau_spec<node>::build_parse_tree() {
	using ptree = tau_parser::tree;
	trefs defs;
	tref main = nullptr;

	typename tau::get_options opts = get_options();
	opts.infer_ba_types = false; // disable type inference for printer
	auto ptree_to_str = [&opts](tref n) {
		tref tn = tau::get(ptree::get(n), opts);
		return tn ? TAU_TO_STR(tn) : ptree::get(n).print_in_line_to_str();
	};

	// find main formula in parsed parts. error when multiple mains.
	for (size_t i = 0; i < parts_.size(); ++i) if (parsed_[i]) {
		for (tref c : ptree::get(parsed_[i]).children()) {
			DBG(TAU_LOG_TRACE << "tau_spec::build_parse_tree spec part: " << ptree_to_str(c);)
			if (tau::get(c).is(tau::main)) {
				if (!main) { // first main found
					main = c;
				} else {
					std::stringstream ss; ss
						<< "Multiple main formulas: \""
						<<ptree_to_str(main)<<"\" and \""
						<<ptree_to_str(c) << "\"";
					errors_.push_back(ss.str());
					return nullptr;
				}
			} else defs.push_back(c);
		}
	}
	if (errors_.empty() && !main) {
		errors_.push_back("No main formula");
		DBG(TAU_LOG_TRACE << "definitions: " << defs.size();)
		DBG(for (auto c : defs) TAU_LOG_TRACE << "def: " << LOG_FM_DUMP(c);)
		return nullptr;
	}
	// build spec parse tree from parsed parts
	auto pnode = [](const auto& literal) -> tau_parser::pnode {
		return { tau_parser::instance().literal(literal), { 0, 0 } };
	};
	auto spec = pnode(tau::spec);
	if (defs.empty()) return ptree::get(spec, main); // only main formula
	return ptree::get(spec, // with definitions
		ptree::get(pnode(tau::definitions), defs),
		main);
}

} // namespace idni::tau_lang