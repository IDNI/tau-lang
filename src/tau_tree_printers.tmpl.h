// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree and comp. printers

template <BAsPack... BAs>

std::ostream& operator<<(std::ostream& os, const std::variant<BAs...>& v) {
	std::visit(overloaded {
		[&os](const auto& a) { os << a; }
	}, v);
	return os;
}

template <BAsPack... BAs>
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n) {
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
	using tau = tree<node<BAs...>>;
	if (n.term) os << "T:";
	if (n.data) os << "[" << n.data << "]";
	if (n.nt == tau::integer) os << n.as_int() << " ";
	else if (n.nt == tau::bf_constant)
				os << ba_constants<BAs...>::get(n.data) << " ";
	else if (tau::is_digital_nt(n.nt)) os << n.data << " ";
	else if (tau::is_string_nt(n.nt))
				os << "\"" << string_from_id(n.data) << "\" ";
	// else if (n.ext) os << "{EXT}";
	os << tau::node::name(n.nt);
#ifdef DEBUG
	if (bool print_nt_ids = true; print_nt_ids) os << "(" << n.nt << ")";
	if (n.data) os << " #" << n.data;
#endif
	return os;
}

template <NodeType node>
std::ostream& operator<<(std::ostream& os, const tree<node>& t) {
	return t.print(os);
}

template <NodeType node>
std::ostream& operator<<(std::ostream& os,
	const typename tree<node>::traverser& tt)
{
	if (!tt.has_value()) return os << "(no value)";
	if (tt.values().size() == 1) return os << tree<node>::get(tt.value());
	for (const auto& c : tt.values()) os << tree<node>::get(c) << "\n";
	return os << "\n\n";
}

template <BAsPack... BAs>
std::ostream& print(std::ostream& os, const rewriter::rule& r) {
	return os << tree<node<BAs...>>::get(r.first) << " := "
			<< tree<node<BAs...>>::get(r.second) << ".";
}

template <BAsPack... BAs>
std::ostream& print(std::ostream& os, const rewriter::rules& rs) {
	for (const auto& r : rs) print<BAs...>(os, r) << " ";
	return os;
}

template <BAsPack... BAs>
std::ostream& print(std::ostream& os, const rr& rr_) {
	print<BAs...>(os, rr_.rec_relations);
	if (rr_.main) os << tree<node<BAs...>>::get(rr_.main);
	return os;
}

template <NodeType node>
std::ostream& tree<node>::print_tree(std::ostream& os, size_t s) const {
	for (size_t i = 0; i < s; i++) os << "\t";
	os << this->value;
	// DBG(os << " *" << get();)
	os << "\n";
	for (const auto& ch : children()) get(ch).print_tree(os, s + 1);
	return os;
}

//------------------------------------------------------------------------------
// print
template <NodeType node>
std::ostream& tree<node>::print(std::ostream& os) const {

// TODO wff_all, wff_ex (print all x all y as all x, y)
// TODO bf_and spacing
// TODO review indenting and syntax highlighting

// #define DEBUG_TRAVERSAL 1

	std::vector<size_t> hl_path;
	size_t depth = 0;
	std::unordered_map<tref, size_t> chpos;
	std::unordered_set<tref> wraps, indented, highlighted;
	auto is_to_wrap = [](size_t nt, size_t pt) {
		static const std::set<size_t> no_wrap_for = {
			bf_splitter, bf_ref, bf_neg, bf_constant, bf_t, bf_f,
			wff_ref, wff_neg, wff_t, wff_f, constraint, capture,
			variable, uninterpreted_constant, ref_args, start
		};
		// priority map (lower number = higher priority)
		static const std::map<size_t, size_t> prio = {
			{ start,                0 },
			{ help_cmd,            50 },
			{ file_cmd,            50 },
			{ normalize_cmd,       50 },
			{ run_cmd,             50 },
			{ solve_cmd,           50 },
			{ dnf_cmd,             50 },
			{ cnf_cmd,             50 },
			{ anf_cmd,             50 },
			{ nnf_cmd,             50 },
			{ pnf_cmd,             50 },
			{ mnf_cmd,             50 },
			{ onf_cmd,             50 },
			{ inst_cmd,            50 },
			{ subst_cmd,           50 },
			{ def_rr_cmd,          50 },
			{ def_list_cmd,        50 },
			{ history_list_cmd,    50 },
			{ history_print_cmd,   50 },
			{ history_store_cmd,   50 },
			{ sat_cmd,             50 },
			{ main,                60 },
			{ bf_rule,             60 },
			{ wff_rule,            60 },
			{ ref,                 80 },
			{ wff_sometimes,      380 },
			{ wff_always,         390 },
			{ wff_conditional,    400 },
			{ wff_all,            430 },
			{ wff_ex,             440 },
			{ wff_imply,          450 },
			{ wff_equiv,          460 },
			{ wff_or,             470 },
			{ wff_and,            480 },
			{ wff_xor,            490 },
			{ wff_neg,            500 },
			{ bf_interval,        501 },
			{ bf_eq,              502 },
			{ bf_neq,             503 },
			{ bf_less_equal,      504 },
			{ bf_nleq,            505 },
			{ bf_greater,         506 },
			{ bf_ngreater,        507 },
			{ bf_greater_equal,   508 },
			{ bf_ngeq,            509 },
			{ bf_less,            510 },
			{ bf_nless,           511 },
			{ wff,                580 },
			{ bf_or,              720 },
			{ bf_and,             730 },
			{ bf_xor,             740 },
			{ bf_neg,             750 },
			{ bf,                 790 },
			{ bf_matcher,         795 },
			{ bf_body,            795 },
			{ rec_relation,       800 },
			{ ref_args,           800 },
			{ bf_rule,            800 },
			{ wff_rule,           800 },
			{ binding,            800 },
		};
		
		if (no_wrap_for.find(nt) != no_wrap_for.end())
			return false;
		auto p_it = prio.find(pt);
		auto n_it   = prio.find(nt);
		if (p_it == prio.end() || n_it == prio.end()) {
#ifdef DEBUG
			std::cerr << "No priority for " << (p_it == prio.end()
					? tau_parser::instance().name(pt)
					: tau_parser::instance().name(nt))<<"\n";
#endif
			return false;
		}
		// tau_parser& p = tau_parser::instance();
		// std::cerr << "\n"
		// 	<< p.name(parent_type) << " vs " << p.name(node_type)
		// 	//<< "(" << parent_type << ")
		// 	<< " " << parent_it->second
		// 	<< (parent_it->second > node_it->second
		// 		? " > " : " <= ")
		// 	<< node_it->second
		// 	// << " (" << node_type << ")"
		// 	<< "\n";
		return p_it->second > n_it->second;
	};

	auto indent = [&depth, &os]() {
		if (!pretty_printer_indenting) return;
		for (size_t i = 0; i < depth; ++i) os << "\t";
	};
	auto break_line = [&os, &indent]() {
		if (!pretty_printer_indenting) return;
		os << "\n", indent();
	};
	// auto break_if_needed = [&break_line](size_t nt) -> bool {
	// 	static const std::vector<size_t> breaks = {
	// 		wff_and, wff_or, wff_xor, wff_imply, wff_equiv,
	// 		wff_all, wff_ex
	// 	};
	// 	if (!pretty_printer_indenting) return false;
	// 	if (find(breaks.begin(), breaks.end(), nt)
	// 		!= breaks.end()) return break_line(), true;
	// 	return false;
	// };
	auto inc_indent = [&](size_t nt) {
		static const std::vector<size_t> indents = {
			wff_sometimes, wff_always, wff_conditional,
			wff_all, wff_ex, wff_imply, wff_equiv
		};
		bool ret = pretty_printer_indenting &&
			find(indents.begin(), indents.end(), nt)
							!= indents.end();
		if (ret) depth++;
		return ret;
	};

	static const std::map<size_t, std::string> hl_colors = {
		{ bf,            idni::TC.LIGHT_GREEN() },
		{ variable,      idni::TC.WHITE() },
		{ capture,       idni::TC.BLUE() },
		{ wff_all,       idni::TC.MAGENTA() },
		{ wff_ex,        idni::TC.LIGHT_MAGENTA() },

		{ rec_relation,  idni::TC.YELLOW() },
		{ constraint,    idni::TC.LIGHT_MAGENTA() },
		{ io_var,        idni::TC.WHITE() },
		{ constant,      idni::TC.LIGHT_CYAN() }

		// { rule,          idni::TC.BG_YELLOW() },
		// { builder,       idni::TC.BG_LIGHT_YELLOW() }
	};
	auto syntax_highlight = [&](size_t nt) {
		if (!pretty_printer_highlighting) return false;
		if (auto it = hl_colors.find(nt); it == hl_colors.end())
			return false;
		else return hl_path.push_back(nt), os << it->second, true;
	};

	auto on_enter = [&](tref ref, tref parent) {
		const auto& t = get(ref);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "[" << t.get_type_name() << "]";
#endif
		// t.print_tree(os << "entering: ") << "\n";
		size_t nt = t.get_type();

		auto track_chpos = [&]() { chpos[ref] = 0; };

		if (inc_indent(nt)) indented.insert(ref);
		if (syntax_highlight(nt)) highlighted.insert(ref);

		switch (nt) {
			case bf_f:              os << '0'; break;
			case bf_t:              os << '1'; break;
			case wff_f:             os << 'F'; break;
			case wff_t:             os << 'T'; break;
			case wff_neg:           os << "!"; break;
			case bf_neg:            os << "'"; break;
			case first_sym:         os << "first"; break;
			case last_sym:          os << "last"; break;
			case fp_fallback:       os << " fallback "; break;
			// wraps
			case builder_head:
			case ref_args:          os << "("; break;
			case constraint:
			case offsets:           os << "["; break;
			case bf_splitter:       os << "S("; break;
			case bf_constant:	ba_constants_t::print(os,
							t.get_ba_constant_id());
						break;
			case bf:
			case wff:
				if (parent && is_to_wrap(
					t.first_tree().get_type(),
					get(parent).get_type()))
				{
					wraps.insert(ref), os << "(";
					if (static_cast<node::type>(nt) == wff)
							depth++, break_line();
				}
				break;

			case wff_all:           os << "all "; break;
			case wff_ex:            os << "ex "; break;

			case wff_sometimes:     os << "sometimes "; break;
			case wff_always:        os << "always "; break;

			case bf_builder_body:   os << " =: "; break;
			case wff_builder_body:  os << " =:: "; break;

			case rel_memory:        os << "%-"; break;
			case abs_memory:        os << "%"; break;
			case dnf_cmd:           os << "dnf "; break;
			case cnf_cmd:           os << "cnf "; break;
			case anf_cmd:           os << "anf "; break;
			case nnf_cmd:           os << "nnf "; break;
			case pnf_cmd:           os << "pnf "; break;
			case mnf_cmd:           os << "mnf "; break;
			case snf_cmd:           os << "snf "; break;
			case onf_cmd:           os << "onf "; break;
			case def_print_cmd:
			case def_rr_cmd:
			case def_list_cmd:      os << "def"; break;
			case history_list_cmd:  os << "history"; break;
			case history_print_cmd:
			case history_store_cmd: os << "history "; break;
			case get_cmd:           os << "get"; break;
			case set_cmd:           os << "set "; break;
			case toggle_cmd:        os << "toggle "; break;
			case quit_cmd:          os << "quit"; break;
			case version_cmd:       os << "version"; break;
			case clear_cmd:         os << "clear"; break;
			case help_cmd:          os << "help"; break;
			case file_cmd:          os << "file"; break;
			case valid_cmd:         os << "valid "; break;
			case sat_cmd:           os << "sat "; break;
			case unsat_cmd:         os << "unsat "; break;
			case solve_cmd:         os << "solve "; break;
			case run_cmd:           os << "run "; break;
			case normalize_cmd:     os << "normalize "; break;
			case inst_cmd:          track_chpos();
						os << "instantiate "; break;
			case subst_cmd:         track_chpos();
						os << "substitute "; break;
			case wff_conditional:   track_chpos(); break;
			default:
				if (is_string_nt(nt))
						os << string_from_id(t.data());
				else if (is_digital_nt(nt)) os << t.data();
				else if (t.is_integer()) os << t.get_integer();
				else if (t.is_ba_constant()) os << "{C"
					<< t.get_ba_constant_id() << "}";
		}
		return true;
	};
	auto on_between = [&](tref /*left*/, tref parent) {
		const auto& t = get(parent);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "[|" << t.get_type_name() << "] \n";
#endif
		auto inc_chpos = [&chpos, &parent]() { return chpos[parent]++;};
		auto chpos_end = [&chpos, &parent]() { chpos.erase(parent); };

		size_t nt = t.get_type();
		switch (nt) {
			case bf_and:            break;
			case bf_or:             os << "|"; break;
			case bf_xor:            os << "+"; break;
			case bf_eq:             os << " = "; break;
			case bf_neq:            os << " != "; break;
			case bf_less_equal:     os << " <= "; break;
			case bf_nleq:           os << " !<= "; break;
			case bf_greater:        os << " > "; break;
			case bf_ngreater:       os << " !> "; break;
			case bf_greater_equal:  os << " >= "; break;
			case bf_ngeq:           os << " !>= "; break;
			case bf_less:           os << " < "; break;
			case bf_nless:          os << " !< "; break;

			case ctn_neq:           os << " != "; break;
			case ctn_eq:            os << " = "; break;
			case ctn_greater_equal: os << " >= "; break;
			case ctn_greater:       os << " > "; break;
			case ctn_less_equal:    os << " <= "; break;
			case ctn_less:          os << " < "; break;

			case wff_and:           os << " && "; break;
			case wff_or:            os << " || "; break;
			case wff_xor:           os << " ^ "; break;
			case wff_imply:         os << " -> "; break;
			case wff_equiv:         os << " <-> "; break;

			case bf_interval:       os << " <= "; break;

			case rec_relation:      os << " := "; break;
			case wff_rule:          os << " ::= "; break;
			case bf_rule:           os << " := "; break;
			case in:
			case out:               os << "["; break;
			case shift:             os << "-"; break;
			case bf_constant:       os << " : "; break;

			case cli:               os << ". "; break;
			case wff_conditional:
				if (inc_chpos()) chpos_end(), os << " : ";
				else os << " ? ";
				break;
			case inst_cmd:
			case subst_cmd:
				switch (inc_chpos()) {
					case 1: os << " ["; break;
					case 2: os << " / "; break;
					case 3: chpos_end(); break;
				}
		};
		return true;
	};
	auto on_leave = [&](tref ref) {
		const auto& t = get(ref);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "\n[/" << t.get_type_name() << "]";
#endif
		// t.print_tree( << "leaving: ") << "\n";
		size_t nt = t.get_type();
		switch (nt) {
			case main: 
			case builder:
			case rec_relation:
			case wff_rule:
			case bf_rule:      os << "."; break;
			case in:
			case out:
			case constraint:
			case offsets:
			case inst_cmd:
			case subst_cmd:    os << "]"; break;
			case bf_splitter:
			case builder_head:
			case ref_args:     os << ")"; break;
			case bf:
			case wff:
				if (wraps.find(ref) != wraps.end()) {
					wraps.erase(ref), os << ")";
					if (static_cast<node::type>(nt) == wff)
							depth--, break_line();
				}
				break;
		}
		if (pretty_printer_highlighting
			&& highlighted.find(ref) != highlighted.end())
		{
			hl_path.pop_back(), os << TC.CLEAR();
			if (hl_path.size()) os << hl_colors.at(hl_path.back());
			highlighted.erase(ref);
		}
		if (pretty_printer_indenting
			&& indented.find(ref) != indented.end())
				depth--, indented.erase(ref);
	};
	auto visit_all = [](tref) { return true; };
	pre_order<node>(this->get()).visit(on_enter, visit_all,
						on_leave, on_between);
	return os;
}

} // idni::tau_lang namespace
