#include "tau.h"
#include "tau_pack.h"
#include "tau_compile.h"
#include "tau_rule_description.h"
#include "tau_rule_inspection.h"
#include <algorithm>
#include <limits>
#include <optional>

namespace idni::tau_lang {
namespace {
template <NodeType Node>
std::optional<unsigned short> concrete_bv_width(size_t type_id, tref type_tree) {
    return pack_owner_apply<Node>(type_id, [&]<class BA>() -> std::optional<unsigned short> {
        if constexpr (ba_has_type_tree_for<Node, BA>) {
            using D = ba_descriptor<BA, Node>;
            if (std::string_view(D::type_name) == "bv") return D::type_param(type_tree);
        }
        return std::nullopt;
    });
}
rule_inspection unsupported(const char* why, report diagnostics = {}) {
    return {rule_inspection_status::unsupported, why, {}, {}, 0, std::move(diagnostics)};
}
}

rule_inspection describe(const std::string& source) {
    namespace tl = idni::tau_lang;
    using node_t = tau_pack::node_t;
    using tree_t = tree<node_t>;
    if (source.empty() || source.size() > 4096 || source.find('\0') != source.npos
        || std::any_of(source.begin(),source.end(),[](unsigned char ch){ return ch > 127; }))
        return unsupported("source envelope");
    const auto raw = describe_detail::describe_source(source);
    if (raw.status == describe_detail::source_status::parse_error) return unsupported("syntax");
    if (raw.status != describe_detail::source_status::matched) return unsupported("source shape");
		tl::compile_detail::scoped_clean_definitions<node_t> guard;
		tl::tau_spec<node_t> spec;
		spec.keep_as_written();
		spec.parse(source);
		auto typed = spec.get();
		if (!typed.has_value() || typed.report().has_error()) return unsupported("typing", std::move(typed).report());
        auto outside = [&](const char* reason) { return unsupported(reason, std::move(typed).report()); };
		std::map<std::string, rule_read_metadata> reads;
		std::map<std::string, std::uint32_t> writes;
		unsigned deepest = 0;
		for (auto v : tree_t::get(typed.value()).select_top(
			tl::is_child<node_t, tree_t::io_var>))
		{
			const auto& x = tree_t::get(v);
			const auto& io = x[0];
			if (io.children_size() != 2 || io[1].children_size() != 1)
				return outside("offset tree");
			const auto& off = io[1][0];
			bool shifted = off.is(tree_t::shift);
			if (shifted && (off.children_size() != 2 || !off[1].is_num()))
				return outside("shift tree");
			auto t = shifted ? off.first() : off.get();
			if (!t || !tree_t::get(t).is(tree_t::variable)
				|| tl::get_var_name<node_t>(t) != "t")
				return outside("nonliteral or nonrelative offset");
			const auto lag = shifted ? off[1].get_num() : 0;
			if (lag > std::numeric_limits<unsigned>::max())
				return outside("oversized lookback");
			auto resolved = x.get_ba_type_tree();
			if (!resolved.has_value()) return outside("unresolved type");
			auto width = concrete_bv_width<node_t>(x.get_ba_type(), resolved.value());
			if (!width || *width > 384) return outside("nonconcrete or out-of-profile bitvector");
			const std::string name = tl::get_var_name<node_t>(v);
			if (name.size() < 2 || (name[0] != 'i' && name[0] != 'o')
				|| name.find_first_not_of("0123456789", 1) != std::string::npos)
				return outside("outside numbered I/O profile");
			const bool in = name[0] == 'i';
			if ((x.is_input_variable() && !in) || (x.is_output_variable() && in))
				return outside("direction");
			if (in) {
				auto& row = reads.try_emplace(name, rule_read_metadata{*width, 0u}).first->second;
				if (row.width != *width) return outside("input width");
				row.deepest_lookback = std::max(row.deepest_lookback, static_cast<unsigned>(lag));
				deepest = std::max(deepest, row.deepest_lookback);
			} else {
				if (lag || writes.try_emplace(name, *width).first->second != *width)
					return outside("noncurrent output or conflicting type");
			}
		}

    if (reads.size() != raw.lookbacks.size() || writes.size() != 1 || !writes.contains(raw.output))
        return outside("typed observations differ from source");
    for (const auto& [name, row] : reads) {
        auto found = raw.lookbacks.find(name);
        if (found == raw.lookbacks.end() || row.deepest_lookback != found->second)
            return outside("typed observations differ from source");
    }
    return {rule_inspection_status::described, {}, std::move(reads), std::move(writes), deepest, std::move(typed).report()};
}
}
