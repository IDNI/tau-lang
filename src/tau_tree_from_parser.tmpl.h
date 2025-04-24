// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

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
// in the map `m` - parse tree node ref -> tau tree node ref.
// each node needs to be transformed (usually just value is copied) and
// children has to be remapped through the map.
//
// See default in the transformer's switch for example of usual transformation.
//

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, const tau_parser::tree& ptr) {
	using type = typename node::type;

	// map of parse tree nodes' refs to tau tree nodes' refs
	std::unordered_map<tref, tref> m;
	// exist in map (is already transformed)
	auto m_ex = [&m](tref t) { return m.find(t) != m.end(); };
	// get tau tree node ref from parse tree node ref
	auto m_ref = [&m](tref t) { return m.at(t); };
	// get tau tree node instance from parse tree node ref
	auto m_get = [&m](tref t) { return get(m.at(t)); };

	std::string src; // source of the current constant
	bool error = false;

	auto transformer = [&m, &m_ex, &m_ref, &m_get, &bind, &src,&error](
		tref t, tref parent)
	{
		// parse_tree::get(t).dump(std::cout << "transforming: ") << "\n";

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
				// offset or ref_arg variable -> capture
				case variable:
					if (parent_nt == offsets
						|| parent_nt == ref_arg)
							nt = capture;
					break;
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
			// std::cout << "getx: " << node::name(nt) << " is_term: " << is_term << "\n";
			// std::cout << "ch.size: " << ch.size() << "\n";
			auto n = node(nt, 0, is_term, ba_type);
			// if (nt != node::constant)std::cout << "getx node:" << n << "\n";
			return get(n, ch);
		};
		// helper to create a new node with current node type and provided data
		auto getx_data = [&nt, &is_term, &ba_type](size_t data) -> tref{
			return get(node(nt, data, is_term, ba_type));
		};

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

		auto get_type = [](tref n) -> std::string {
			auto t = tt(n) | tau::type;
			if (t) return t | tt::string;
			return "untyped";
		};

		tref x = nullptr; // result of node transformation

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
			case memory_id:
				x = getx_data(m_get(ptr.only_child()).data());
				break;

			case wff_all:
			case wff_ex: x = process_quantifier_vars(); break;

			// preprocess source node
			case source:
				src = ptr.get_terminals();
				x = nullptr;
				break;

			default:
				if (is_string_nt(nt)) {
					x = getx_data(
                                                string_id(ptr.get_terminals()));
					break;
				}

				// arbitrary nonterminal
				trefs ch;
				for (tref c : ptr.children()) {
					DBG(assert(c != nullptr && m_ex(c));)
					if (m_ref(c)) ch.push_back(m_ref(c));
				}
				x = getx(ch);

				// call binder on a transformed bf_constant node
				if (nt == bf_constant) {
					if (!get(x)[0][0].is(binding)) break; // capture?
					BOOST_LOG_TRIVIAL(debug) << "tau tree transform calling binder: " << src << " " << get_type(x);
					auto nn = bind(src, tau::get_type_name(x));
					if (nn == nullptr || bind.error
						|| nn == x) return error = true,
									false;
					x = nn;
				}
				break;
		}

		m.emplace(t, x); // store new node into map

		// std::cout << "emplaced:" << parse_tree::get(t).value
		// 	<< " as " << tree::get(x).get_type_name() << "\n"";
		return true;
	};
	// parse_tree::get(pt.get()).print(std::cout << "parse tree: ") << "\n"";
	post_order<tau_parser::pnode>(ptr.get()).search(transformer);
	if (error) return nullptr;
	if (m.find(ptr.get()) == m.end()) return nullptr;
	// std::cout << "tau tree: " << tree::get(m.at(pt.get())) << "\n"";
	// tree::get(m_get(pt.get())).print_tree(std::cout << "tau tree: ") << "\n"";
	return m_ref(ptr.get());
}

template <NodeType node>
tref tree<node>::get(const tau_parser::tree& pt) {
	return get<ba_constants_binder_t>(ba_constants_binder_t::instance(), pt);
}

//------------------------------------------------------------------------------

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, tau_parser::result& result) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return nullptr; // Syntax error
	}
	auto pt = parse_tree::get(result.get_shaped_tree2());
	return tree<node>::template get<binder>(bind, pt);
}

template <NodeType node>
tref tree<node>::get(tau_parser::result& result) {
	return tree<node>::get<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), result);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, const std::string& source,
	parse_options options)
{
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get(const std::string& source, parse_options options) {
	ba_constants_binder_t binder(options.named_constants);
	return tree<node>::get<ba_constants_binder_t>(binder, source, options);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, std::istream& is, parse_options options) {
	auto result = tau_parser::instance().parse(is, options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get(std::istream& is, parse_options options) {
	ba_constants_binder_t binder(options.named_constants);
	return tree<node>::get<ba_constants_binder_t>(binder, is, options);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get_from_file(binder& bind, const std::string& filename,
	parse_options options)
{
	auto result = tau_parser::instance().parse(filename, options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get_from_file(const std::string& filename, parse_options options){
	ba_constants_binder_t binder(options.named_constants);
	return tree<node>::get<ba_constants_binder_t>(binder, filename, options);
}

template <NodeType node>
template <typename binder>
rewriter::library tree<node>::get_library(binder& bind, const std::string& str) {
	using tau = tree<node>;
	tau::parse_options opts; opts.start = tau::library;
	return get_rules(tau::get<binder>(bind, str, opts));
}

template <NodeType node>
rewriter::library tree<node>::get_library(const std::string& str) {
	return tree<node>::get_library<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), str);
}

template <NodeType node>
template <typename binder>
rewriter::builder tree<node>::get_builder(binder& bind, const std::string& source){
	using tau = tree<node>;
	tau::parse_options opts; opts.start = tau::builder;
	return get_builder(tau::get<binder>(bind, source, opts));
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(const std::string& source) {
	return tree<node>::get_builder<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), source);
}

template <NodeType node>
tree<node>::parse_options::parse_options() = default;

template <NodeType node>
tree<node>::parse_options::parse_options(const parse_options&) = default;

template <NodeType node>
tree<node>::parse_options::parse_options(const tau_parser::parse_options& opts)
	: tau_parser::parse_options(opts) {}

} // namespace idni::tau_lang