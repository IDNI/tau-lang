// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
tref tree<node>::get(const tau_parser::tree& ptr, get_options& options) {
	using type = typename node::type;

	// map of parse tree nodes' refs to tau tree nodes' refs
	std::unordered_map<tref, tref> m;
	// exist in map (to check if a node is transformed)
	auto m_ex = [&m](tref t) { return m.find(t) != m.end(); };
	// get tau tree node ref from parse tree node ref
	auto m_ref = [&m](tref t) { return m.at(t); };
	// get tau tree node instance from parse tree node ref
	auto m_get = [&m](tref t) { return get(m.at(t)); };

	bool error = false;

	std::map<size_t, tref> named_constants; // dict named constant names
	for (auto& [cn, c] : options.named_constants)
		named_constants[dict(cn)] = c;

	auto transformer = [&](tref t, [[maybe_unused]] tref parent) {
		// DBG(LOG_TRACE << " -- transforming: "
		// 	      << (parse_tree::get(t).print_to_str());)

		if (m_ex(t)) return true; // already transformed
		const auto& ptr = parse_tree::get(t); // get parse tree node
		if (ptr.is_t()) return true; // terminals are processed by their parents

		// get node type and parent node type
		type nt = static_cast<type>(ptr.get_nt());
		//type parent_nt = static_cast<type>(parent == nullptr ? 0
		//			: parse_tree::get(parent).get_nt());

		// simple renaming transformations
		// TODO (LOW) this should be supported directly by the parser
		auto retype_nonterminal = [&nt]() -> type {
			switch (nt) { // of intermediate nodes
				// bf_and_nosep and bf_neg_oprnd
				case bf_and_nosep_1st_oprnd:
				case bf_and_nosep_2nd_oprnd:
				case bf_neg_oprnd:
					nt = bf; break;
				case bf_and_nosep:
					nt = bf_and; break;
				case io_var_name:
					nt = var_name; break;
				default: break;
			}
			return static_cast<type>(nt);
		};
		// retype if needed
		nt = retype_nonterminal();

		// determine node types
		bool is_term = false; //is_term_nt(nt, parent_nt);
		size_t ba_type = 0;

		/*bool unresolved_ref = nt == ref && !is_term
			&& parent_nt != wff_ref && parent_nt != bf_ref;
		if (unresolved_ref) ba_type = 1; // is_term = 0 and ba_type = 1*/

		// helper to create a new node with current node type and provided children
		auto getx = [&nt, &is_term, &ba_type](const auto& ch) -> tref {
			// LOG_TRACE << "getx: " << LOG_NT(nt) << " is_term: " << is_term << "\n";
			// LOG_TRACE << "ch.size: " << ch.size() << "\n";
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
		auto process_quantifier_vars =
			[&ptr, &m_get, &getx](type nt) -> tref
		{
			const auto& q_vars = m_get(ptr.first());
			const auto& expr = m_get(ptr.second());
			trefs vars = q_vars.get_children();
			if (vars.empty()) // TODO (LOW) is this even reachable?
				return getx(trefs{ q_vars.get(), expr.get() });
			tref x = expr.only_child();
			for (size_t vi = 0; vi != vars.size(); ++vi) {
				// create a new quantifier node with var and new
				// children. Note the reversed order!
				x = getx(trefs{ vars[vars.size() - 1 - vi],
								get(nt, x) });
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

		// Convert the type information from the parse tree to the
		// type id used by the type system for tau trees
		auto process_type_tree = [&](size_t non_terminal) {
			switch (non_terminal) {
				case bf_t:
				case bf_f: {
					if (ptr.first()){
						ba_type = get_ba_type_id<node>(
							m_ref(ptr.first()));
						LOG_TRACE << "ba_type: "
							  << LOG_BA_TYPE(ba_type);
					}
					break;
				}
				case variable:
				case ba_constant: {
					if (ptr.second()) {
						ba_type = get_ba_type_id<node>(
								m_ref(ptr.second()));
						LOG_TRACE << "ba_type: "
							  << LOG_BA_TYPE(ba_type);
					}
					break;
				}
				case input_def:
				case output_def: {
					if (ptr.second() && ptr.second_tree().
						is(tau::typed))
					{
						ba_type = get_ba_type_id<node>(
							m_ref(ptr.second()));
						LOG_TRACE << "ba_type: "
							<< LOG_BA_TYPE(ba_type);
					}
					break;
				}
				case ref: {
					// Find possible type info
					const auto len = ptr.children_size();
					if (len >= 3 && ptr[2].is(typed)) {
						ba_type = get_ba_type_id<node>(
							m_ref(ptr[2].get()));
					} else if (len >= 4 && ptr[3].is(typed)) {
						ba_type = get_ba_type_id<node>(
							m_ref(ptr[3].get()));
					}
					LOG_TRACE << "ba_type: "
						  << LOG_BA_TYPE(ba_type);
					break;
				}
				default: break;
			}
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
			case history_id:
				x = getx_data(m_get(ptr.only_child()).data());
				break;

			case wff_all:
			case wff_ex: x = process_quantifier_vars(wff); break;
			case bf_fall:
			case bf_fex: x = process_quantifier_vars(bf); break;

			case bf_t:
			case bf_f:
				// Integrate type information
				process_type_tree(nt);
				x = getx_data(0);
				break;

			default:
				// Integrate type information
				process_type_tree(nt);
				if (is_string_nt(nt)) {
					x = getx_data(
						dict(ptr.get_terminals()));
					break;
				}

				// arbitrary nonterminal
				trefs ch;
				for (tref c : ptr.children()) {
					DBG(assert(c != nullptr && m_ex(c));)
					// Remove type information which is already saved in tau nodes
					if (m_ref(c) && !tau::get(c).is(typed))
						ch.push_back(m_ref(c));
				}
				// DBG(for (auto c : ch) LOG_TRACE << "child: " << LOG_FM_DUMP(c);)
				x = getx(ch);

				// if (nt == bf || nt == wff)
				// 	tau_lang::get_free_vars<node>(x);

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
	// DBG(LOG_TRACE << "reget with hooks: " << options.reget_with_hooks;)
	DBG(std::stringstream ss;)
	DBG(LOG_TRACE << "parse tree: "
			<< (parse_tree::get(ptr.get()).print(ss), ss.str());)

	auto using_hooks = tau::use_hooks;
	tau::use_hooks = false;
	// DBG(LOG_TRACE << "HOOKS DISABLED: " << tau::use_hooks;)
	post_order<tau_parser::pnode>(ptr.get()).search(transformer);
	if (error || m.find(ptr.get()) == m.end()) {
		// DBG(LOG_TRACE << "HOOKS ENABLED: " << tau::use_hooks;)
		return tau::use_hooks = using_hooks, nullptr;
	}
	DBG(LOG_TRACE << "transformed: " << tree::get(m.at(ptr.get())).to_str();)
	DBG(LOG_TRACE << "trans. tree: " << m_get(ptr.get()).dump_to_str();)
	tref transformed = m_ref(ptr.get());

	if (options.infer_ba_types) {
		auto result = infer_ba_types<node>(transformed,
			options.global_scope, options.definition_heads);
		transformed = result.first;
		// If type inference failed
		if (!transformed) {
			tau::use_hooks = using_hooks;
			return nullptr;
		}
		if (transformed) options.global_scope = std::move(result.second);
	}

	//Check for semantic errors in expression
	if (has_semantic_error<node>(transformed)) {
		tau::use_hooks = using_hooks;
		return nullptr;
	}
	tau::use_hooks = using_hooks;
	if (options.reget_with_hooks) transformed = reget(transformed);

#ifdef DEBUG
	// Check that all term nodes have been typed
	auto check = [](tref n) {
		if (is_term_nt(tau::get(n).value.nt) && !tau::get(n).is(tau::capture)) {
			if (tau::get(n).get_ba_type() == 0) {
				LOG_DEBUG << "Untyped term node: " << tau::get(n).tree_to_str();
				assert(false);
			}
		}
		return true;
	};
	auto visit = [](tref n) {
		if (tau::get(n).is(tau::offset)) return false;
		return true;
	};
	if (options.infer_ba_types && options.reget_with_hooks)
		pre_order<node>(transformed).visit(check, visit, identity);
#endif

	// As final step, convert the bound variables to a canonical numbered representation
	// This only makes sense in combination with type inference since
	// otherwise quantified variables cannot be correctly caught
	if (options.infer_ba_types)
		transformed = canonize_quantifier_ids<node>(transformed);
	// DBG(LOG_TRACE << "HOOKS ENABLED: " << tau::use_hooks;)
	return transformed;
}

//------------------------------------------------------------------------------

template <NodeType node>
tref tree<node>::get(tau_parser::result& result, get_options& options) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[tau] " << msg << "\n";
		return nullptr;
	}
	auto pt = parse_tree::get(result.get_shaped_tree2());
	return tree<node>::get(pt, options);
}

template<NodeType node>
tref tree<node>::get(const std::string& str) {
	get_options opts;
	return get(str, opts);
}

template <NodeType node>
tref tree<node>::get(const std::string& source, get_options& options) {
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options.parse);
	return tree<node>::get(result, options);
}

template <NodeType node>
tref tree<node>::get(std::istream& is, get_options& options) {
	auto result = tau_parser::instance().parse(is, options.parse);
	return tree<node>::get(result, options);
}

template <NodeType node>
tref tree<node>::get_from_file(const std::string& filename,
	get_options& options)
{
	auto result = tau_parser::instance().parse(filename, options.parse);
	return tree<node>::get(result, options);
}

} // namespace idni::tau_lang