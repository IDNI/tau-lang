#include "tau_rule_description.h"
#include "tau_parser.generated.h"

#include <algorithm>
#include <bit>
#include <charconv>
#include <optional>
#include <vector>

namespace idni::tau_lang::describe_detail {
namespace {
using parser = tau_parser;
using ref = idni::tref;
constexpr std::size_t max_depth = 96;
constexpr std::uint32_t max_offset = 2147483647;

const parser::tree& node(ref r) { return parser::tree::get(r); }
bool is(ref r, std::size_t k) { return r && node(r).is(k); }

std::vector<ref> children(ref r) {
	std::vector<ref> out;
	for (auto c : node(r).children()) if (node(c).is_nt()) out.push_back(c);
	return out;
}

ref only(ref r, std::size_t type) {
	if (!is(r, type)) return nullptr;
	auto ns = children(r);
	return ns.size() == 1 ? ns[0] : nullptr;
}

std::optional<std::uint32_t> decimal(std::string_view s) {
	std::uint32_t value = 0;
	auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
	if (ec != std::errc{} || end != s.data() + s.size() || value > max_offset)
		return std::nullopt;
	return value;
}

bool current(ref r) {
	return is(only(r, parser::variable), parser::var_name)
		&& node(r).get_terminals() == "t";
}

struct walk {
	source_facts facts;
	std::map<std::string, std::uint32_t> aliases;

	std::optional<std::uint32_t> bv_type(ref type, ref width = nullptr) const {
		if (!is(type, parser::type_ref)) return {};
		auto ts = children(type);
		if (ts.empty() || ts.size() > 2 || !is(ts[0], parser::type_ref_name)) return {};
		if (ts.size() == 2) {
			if (width) return {};
			width = ts[1];
		}
		const auto name = node(ts[0]).get_terminals();
		if (name == "bv") {
			if (!is(width, parser::subtype)) return {};
			auto result = decimal(node(width).get_terminals());
			return result && *result && *result <= 384 ? result : std::nullopt;
		}
		auto found = aliases.find(name);
		if (width || found == aliases.end()) return {};
		return found->second;
	}

	bool alias(ref def) {
		auto a = children(def);
		if (a.size() != 2 || !is(a[0], parser::new_type_name) || !is(a[1], parser::type_body))
			return false;
		auto name = node(a[0]).get_terminals();
		if (!name.starts_with("amt_") || aliases.contains(name)) return false;
		auto parts = children(a[1]);
		if (parts.empty() || parts.size() > 2) return false;
		auto width = bv_type(parts[0], parts.size() == 2 ? parts[1] : nullptr);
		return width && aliases.emplace(std::move(name), *width).second;
	}

	static bool literal(std::string_view text, std::uint32_t width) {
		if (!text.starts_with("#x") || text.size() < 3) return false;
		std::uint32_t bits = 0;
		for (char c : text.substr(2)) {
			unsigned value = 0;
			if (c >= '0' && c <= '9') value = static_cast<unsigned>(c - '0');
			else if (c >= 'a' && c <= 'f') value = static_cast<unsigned>(c - 'a' + 10);
			else if (c >= 'A' && c <= 'F') value = static_cast<unsigned>(c - 'A' + 10);
			else return false;
			bits = bits ? bits + 4 : static_cast<std::uint32_t>(std::bit_width(value));
			if (bits > width) return false;
		}
		return true;
	}

	bool atom(ref r, bool write = false) {
		r = only(r, parser::bf);
		if (!r) return false;
		auto ns = children(r);
		if (ns.empty() || ns.size() > 2) return false;
		std::optional<std::uint32_t> width;
		if (ns.size() == 2 && !(width = bv_type(only(ns[1], parser::typed)))) return false;
		if (!write && is(r, parser::ba_constant))
			return width && is(ns[0], parser::source) && literal(node(ns[0]).get_terminals(), *width);
		if (!is(r, parser::variable) || !is(ns[0], parser::io_var)) return false;
		auto io = children(ns[0]);
		if (io.size() != 2 || !is(io[0], parser::io_var_name)) return false;
		auto name = node(io[0]).get_terminals();
		if (name.size() < 2 || name[0] != (write ? 'o' : 'i')
			|| (name.size() > 2 && name[1] == '0') || !decimal(std::string_view(name).substr(1))) return false;
		auto off = only(io[1], parser::offset);
		std::uint32_t lag = 0;
		if (!current(off)) {
			if (write || !is(off, parser::shift)) return false;
			auto sh = children(off);
			if (sh.size() != 2 || !current(sh[0]) || !is(sh[1], parser::num)) return false;
			auto n = decimal(node(sh[1]).get_terminals());
			if (!n) return false;
			lag = *n;
		}
		if (write) {
			if (!facts.output.empty() && facts.output != name) return false;
			facts.output = std::move(name);
		} else {
			auto& lookback = facts.lookbacks[std::move(name)];
			lookback = std::max(lookback, lag);
		}
		return true;
	}

	bool condition(ref r, std::size_t depth) {
		if (depth >= max_depth || !(r = only(r, parser::wff))) return false;
		auto ns = children(r);
		switch (node(r).get_nt()) {
			case parser::wff_t: case parser::wff_f: return ns.empty();
			case parser::wff_neg: return ns.size() == 1 && condition(ns[0], depth + 1);
			case parser::wff_and: case parser::wff_or: return ns.size() == 2
				&& condition(ns[0], depth + 1) && condition(ns[1], depth + 1);
			case parser::bf_eq: case parser::bf_neq: case parser::bf_lt:
			case parser::bf_lteq: case parser::bf_gt: case parser::bf_gteq:
				return ns.size() == 2 && atom(ns[0]) && atom(ns[1]);
			default: return false;
		}
	}

	bool assignment(ref r, std::size_t depth) {
		if (depth >= max_depth || !(r = only(r, parser::wff))) return false;
		auto ns = children(r);
		switch (node(r).get_nt()) {
			case parser::wff_conditional: return ns.size() == 3
				&& condition(ns[0], depth + 1) && assignment(ns[1], depth + 1)
				&& assignment(ns[2], depth + 1);
			case parser::bf_eq: return ns.size() == 2 && atom(ns[0], true) && atom(ns[1]);
			default: return false;
		}
	}

	bool run(ref root) {
		if (!is(root, parser::spec_multiline)) return false;
		auto top = children(root);
		if (top.empty() || !is(top.back(), parser::main)) return false;
		for (std::size_t i = 0; i + 1 < top.size(); ++i)
			if (!is(top[i], parser::type_def) || !alias(top[i])) return false;
		return assignment(only(only(only(top.back(), parser::main), parser::wff),
			parser::wff_always), 0);
	}
};

} // namespace

source_facts describe_source(std::string_view source) {
	parser local;
	auto p = local.parse(source.data(), source.size(), {.start=parser::spec_multiline});
	if (!p.found) return {source_status::parse_error, {}, {}};
	walk reader;
	if (!reader.run(p.get_shaped_tree2())) return {};
	reader.facts.status = source_status::matched;
	return std::move(reader.facts);
}

} // namespace idni::tau_lang::describe_detail
