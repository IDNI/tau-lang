// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "from_parser"

namespace idni::tau_lang {

//------------------------------------------------------------------------------
// Transformer from tau_parser::tree
//
// Converts parser tree nodes to tau tree nodes
// - terminals are collapsed into their parents
//     ie. shift > num > "10" becomes shift > num("10")
//     or  variable > type > "sbf" becomes variable > type("sbf")
// - does q_vars transformation from ex x, y to ex x ex y
// - transforms offset variables to captures
//
// Important terminals are stored in their pools outside of the tree
// and node's value is actually id in the pool (strings, constants)
//
// Transformer runs post_order search of a parse tree.
// When a node is reached, all its children are already transformed and stored
// in the map `m` of: parse tree node ref -> tau tree node ref.
//
// Transformation of a node is basically copying a node's value and recreating
// the node with children remapped through the map.
//
// See default in the transformer's switch for example of usual transformation.
//

template <NodeType node>
tref tree<node>::get(const tau_parser::tree& ptr, get_options options) {
	using type = typename node::type;

	// map of parse tree nodes' refs to tau tree nodes' refs
	std::unordered_map<tref, tref> m;
	// exist in map (to check if a node is transformed)
	auto m_ex = [&m](tref t) { return m.find(t) != m.end(); };
	// get tau tree node ref from parse tree node ref
	auto m_ref = [&m](tref t) { return m.at(t); };
	// get tau tree node instance from parse tree node ref
	auto m_get = [&m](tref t) { return get(m.at(t)); };

	size_t src = 0; // source dict id of the current constant
	bool error = false;

	std::map<size_t, tref> named_constants; // dict named constant names
	for (auto& [cn, c] : options.named_constants)
		named_constants[dict(cn)] = c;

	auto transformer = [&](tref t, tref parent) {
		DBG(LOG_TRACE << " -- transforming: "
			      << (parse_tree::get(t).print_to_str());)

		if (m_ex(t)) return true; // already transformed
		const auto& ptr = parse_tree::get(t); // get parse tree node
		if (ptr.is_t()) return true; // terminals are processed by their parents

		// get node type and parent node type
		type nt = static_cast<type>(ptr.get_nt());
		type parent_nt = static_cast<type>(parent == nullptr ? 0
					: parse_tree::get(parent).get_nt());

		// simple renaming transformations
		// TODO (LOW) this should be supported directly by the parser
		auto retype_nonterminal = [&nt, &parent_nt]() -> type {
			switch (nt) { // of intermediate nodes
				// bf_and_nosep and bf_neg_oprnd
				case bf_and_nosep_1st_oprnd:
				case bf_and_nosep_2nd_oprnd:
				case bf_neg_oprnd:
					nt = bf; break;
				case bf_and_nosep:
					nt = bf_and; break;
				default: break;
			}
			return static_cast<type>(nt);
		};
		// retype if needed
		nt = retype_nonterminal();

		// determine node types
		bool is_term = is_term_nt(nt, parent_nt);
		size_t ba_type = 0;

		bool unresolved_ref = !is_term && nt == ref;
		if (unresolved_ref) ba_type = 1; // is_term = 0 and ba_type = 1

		// helper to create a new node with current node type and provided children
		auto getx = [&nt, &is_term, &ba_type](const auto& ch) -> tref {
			LOG_TRACE << "getx: " << LOG_NT(nt) << " is_term: " << is_term << "\n";
			LOG_TRACE << "ch.size: " << ch.size() << "\n";
			auto n = node(nt, 0, is_term, ba_type);
			// if (nt != node::constant) std::cout << "getx node:" << n << "\n";
			return get(n, ch);
		};
		// helper to create a new node with current node type and provided data
		auto getx_data = [&nt, &is_term, &ba_type](size_t data) -> tref{
			return get(node(nt, data, is_term, ba_type));
		};

		// auto process_var = [](tref x) -> tref {
		// 	// LOG_TRACE << "Process var: " << LOG_FM_DUMP(x);
		// 	return x;
		// };

		// q_vars transformation (ex x,y to ex x ex y)
		auto process_quantifier_vars = [&ptr, &m_get, &getx]() -> tref {
			const auto& q_vars = m_get(ptr[0]);
			const auto& wff_expr = m_get(ptr[1]);
			trefs vars = q_vars.get_children();
			if (vars.empty()) // TODO (LOW) is this even reachable?
				return getx(trefs{ q_vars.get(), wff_expr.get() });
			tref x = wff_expr.only_child();
			for (size_t vi = 0; vi != vars.size(); ++vi) {
				// create a new quantifier node with var and new
				// children. Note the reversed order!
				x = getx(trefs{ vars[vars.size() - 1 - vi],
								get(wff, x) });
			}
			return x;
		};

		// takes data value from a preprocessed digit subnode
		// and negates if '-' sign is parsed (if children size == 2)
		auto process_integer = [&ptr, &m_get]() -> int_t {
			trefs ch = ptr.get_children(); // 1 or 2
			bool neg = ch.size() == 2;
			int_t i = static_cast<int_t>( // get data from child 0 or 1)
				m_get(ch[static_cast<size_t>(neg)]).data());
			return neg ? -i : i;
		};

		tref x = nullptr; // result of node transformation

		// takes already transformed bf_constant parser tree node, ie.
		// it takes tau tree bf_constant node with type
		// and processes it:
		// - check if they are named constants
		// - if not call get_ba_constant() which stores them in the pool
		//   and returns the bound constant node
		auto process_bf_constant =
			[&src, &error, &named_constants](tref x) -> tref
		{
			// LOG_TRACE << "binding: " << src << " " << get_type_sid<node>(x);
			// get type id from type subnode (or 0 = untyped)
			size_t ba_type_id = get_ba_type_id<node>(
						get_type_sid<node>(x));
			if (ba_type_id == 0) { // untyped
				// check if it's a named constant
				auto it = named_constants.find(src);
				if (it != named_constants.end()) {
					LOG_TRACE << "named bound: "
						<< LOG_FM(it->second);
					return it->second;
				}
			}
			// get the bound constant node
			tref n = get_ba_constant_from_source(src, ba_type_id);
			src = 0;
			if (n == nullptr || n == x)
				return error = true, nullptr;
			LOG_TRACE << (ba_type_id == 0 ? "un" : "") << "bound: "
								<< LOG_FM(n);
			return n;
		};

		switch (nt) {
			// tau tree terminals
			case digits: // preprocess digits
				// TODO: ??? check if number fits into data bitsize, if not, create an ext node
				// maybe only if --ext_nodes option is used?
				x = getx_data(std::stoul(ptr.get_terminals()));
				break;

			case integer: x = getx_data(
				static_cast<size_t>(process_integer())); break;

			// digital terminals (same as is_digital_nt())
			case num:
			case history_id:
				x = getx_data(m_get(ptr.only_child()).data());
				break;

			case wff_all:
			case wff_ex: x = process_quantifier_vars(); break;

			// preprocess source node
			case source:
				src = dict(ptr.get_terminals());
				DBG(LOG_TRACE << "BA constant source: `"
							<< dict(src) << "`");
				x = nullptr;
				break;

			case bf_t:
			case bf_f:
				ba_type = get_ba_type_id<node>(
							get_type_sid<node>(x));
				x = getx_data(0);
				break;

			// case variable: x = process_var(x); // break;

			default:
				if (is_string_nt(nt)) {
					x = getx_data(
						dict(ptr.get_terminals()));
					break;
				}

				// arbitrary nonterminal
				trefs ch;
				for (tref c : ptr.children()) {
					DBG(assert(c != nullptr && m_ex(c));)
					if (m_ref(c)) ch.push_back(m_ref(c));
				}
				// DBG(for (auto c : ch) LOG_TRACE << "child: " << LOG_FM_DUMP(c);)
				x = getx(ch);

				// process constant from a transformed node
				if (nt == bf_constant && src)
					if (x = process_bf_constant(x);
						x == nullptr)
							return false;
				break;
		}

		m.emplace(t, x); // store new node into map

// #ifdef DEBUG
// 		if (!x) LOG_TRACE << " -- not emplaced: `"
// 					<< parse_tree::get(t).value << "`\n";
// 		else LOG_TRACE << " -- emplaced: `"
// 			<< parse_tree::get(t).value << "` as "
// 			<< tree::get(x).get_type_name() << " `"
// 			<< tree::get(x) << "`\n";
// #endif // DEBUG
		return true;
	};
	DBG(std::stringstream ss;)
	DBG(LOG_TRACE << "parse tree: "
			<< (parse_tree::get(ptr.get()).print(ss), ss.str());)

	post_order<tau_parser::pnode>(ptr.get()).search(transformer);
	if (error) return nullptr;
	if (m.find(ptr.get()) == m.end()) return nullptr;

	DBG(LOG_TRACE << "transformed: " << tree::get(m.at(ptr.get())).to_str();)
	DBG(LOG_TRACE << "trans. tree: " << m_get(ptr.get()).dump_to_str();)
	tref transformed = m_ref(ptr.get());
	if (options.infer_ba_types) return infer_ba_types<node>(transformed);
	return transformed;
}

//------------------------------------------------------------------------------

template <NodeType node>
tref tree<node>::get(tau_parser::result& result, get_options options) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[tau] " << msg << "\n";
		return nullptr;
	}
	auto pt = parse_tree::get(result.get_shaped_tree2());
	return tree<node>::get(pt, options);
}

template <NodeType node>
tref tree<node>::get(const std::string& source, get_options options) {
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options.parse);
	return tree<node>::get(result, options);
}

template <NodeType node>
tref tree<node>::get(std::istream& is, get_options options) {
	auto result = tau_parser::instance().parse(is, options.parse);
	return tree<node>::get(result, options);
}

template <NodeType node>
tref tree<node>::get_from_file(const std::string& filename,
	get_options options)
{
	auto result = tau_parser::instance().parse(filename, options.parse);
	return tree<node>::get(result, options);
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(tref ref) {
	return tau_lang::get_builder<node>(ref);
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(const std::string& source) {
	return tau_lang::get_builder<node>(source);
}

template <NodeType node>
rewriter::rules tree<node>::get_rules(tref ref) {
	return tau_lang::get_rules<node>(ref);
}

template <NodeType node>
rewriter::rules tree<node>::get_rules(const std::string& source) {
	return tau_lang::get_rules<node>(source);
}

template <NodeType node>
rewriter::library tree<node>::get_library(tref ref) {
	return tau_lang::get_library<node>(ref);
}

template <NodeType node>
rewriter::library tree<node>::get_library(const std::string& str) {
	return tau_lang::get_library<node>(str);
}

template <NodeType node>
rewriter::builder get_builder(tref ref) {
	using tau = tree<node>;
	using tt = tau::traverser;
	DBG(assert(ref != nullptr);)
	tt b(ref);
	auto body = b | tau::builder_body | tt::only_child;
	DBG(assert((body | tt::nt) == tau::bf_builder_body
		|| (body | tt::nt) == tau::wff_builder_body);)
	return { tau::geth(b | tau::builder_head | tt::ref),
		 tau::geth(body | tt::first | tt::ref) };
}

template <NodeType node>
rewriter::builder get_builder(const std::string& source){
	using tau = tree<node>;
	typename tau::get_options opts{ .parse = { .start = tau::builder },
						   .infer_ba_types = false };
	return get_builder<node>(tau::get(source, opts));
}

template <NodeType node>
rewriter::rules get_rules(tref r) {
	using tau = tree<node>;
	using tt = tau::traverser;
	tt rules(r);
	DBG(assert(rules.is(tau::rules));)
	auto rs = rules || tau::rule || tt::first;
	rewriter::rules x;
	for (auto r : rs.traversers()) {
		// tree::get(r.value()).print(std::cout << "rule: ");
		x.emplace_back( tau::geth(r| tt::first  | tt::first | tt::ref),
				tau::geth(r| tt::second | tt::first | tt::ref));
	}
	return x;
}

template <NodeType node>
rewriter::library get_rules(const std::string& str) {
	using tau = tree<node>;
	typename tau::get_options opts{ .parse = { .start = tau::rules },
					.infer_ba_types = false };
	return get_rules<node>(tau::get(str, opts));
}

template <NodeType node>
rewriter::rules get_library(tref r) {
	using tau = tree<node>;
	const auto& lib = tau::get(r);
	DBG(assert(lib.is(tau::library));)
	return get_rules<node>(lib.first());
}

template <NodeType node>
rewriter::library get_library(const std::string& str) {
	using tau = tree<node>;
	typename tau::get_options opts{ .parse = { .start = tau::library },
					.infer_ba_types = false };
	return get_library<node>(tau::get(str, opts));
}

} // namespace idni::tau_lang