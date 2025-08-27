// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "utils.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "printers"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree and comp. printers

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const std::variant<BAs...>& v) {
	std::visit(overloaded {
		[&os](const auto& a) { os << a; }
	}, v);
	return os;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n) {
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
	using node = node<BAs...>;
	using tau = tree<node>;
	os << node::name(n.nt);
#ifdef DEBUG
	if (bool print_nt_ids  = false; print_nt_ids) os << "(" << n.nt << ")";
	if (bool print_is_term = true; print_is_term && n.term) os << "*";
	if (n.term == 0 && n.ba == 1 && n.nt == tau_parser::ref)
		os << LOG_WARNING_COLOR << "?" << TC.CLEAR();
	if (n.data) os << "[" << n.data << "]";
	if (bool print_ba_type = true;
		print_ba_type && n.term && n.nt != tau::bf)
						os << " " << LOG_BA_TYPE(n.ba);
#endif
	if (n.nt == tau::integer) os << " { " << n.as_int() << " }";
	else if (n.nt == tau::bf_constant) {
		if (n.ba) os << " { " << ba_constants<node>::get(n.data)
				<< " } : " << get_ba_type_name<node>(n.ba);
		else os << " { " << dict(n.data) << " } : untyped";
	} else if (tau::is_digital_nt(n.nt)) os << " { " << n.data << " }";
	else if (n.nt == tau::uconst_name)
		os << "<" << dict(n.data) << ">";
	else if (tau::is_string_nt(n.nt))
		os << " { \"" << dict(n.data) << "\" }";
 	// else if (n.ext) os << "{EXT}";
	if (n.nt == tau::io_var) {
		if (n.data == 1) os << " IN";
		else if (n.data == 2) os << " OUT";
		else os << LOG_WARNING_COLOR << " I/O" << TC.CLEAR();
	}

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

template <NodeType node>
std::ostream& operator<<(std::ostream& os, const spec_context<node>& ctx) {
        os << "IO variables:";
        if (ctx.inputs.size() == 0 && ctx.outputs.size() == 0) os << " none";
        os << "\n";
        auto print_io = [&](size_t var_sid, const auto& s, bool output){
                os << "\t" << dict(var_sid) << " : "
                        << get_ba_type_name<node>(s.first) << " = "
                        << (output ? "out" : "in") << " "
                        << (s.second == 0 ? "console"
                                : "file(\"" + dict(s.second) + "\")")
                        << "\n";
        };
        for (const auto& [var_sid, desc] : ctx.inputs)
                print_io(var_sid, desc, false);
        for (const auto& [var_sid, desc] : ctx.outputs)
                print_io(var_sid, desc, true);
	return os;
}

template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rule& r) {
	return os << tree<node>::get(r.first) << " := "
			<< tree<node>::get(r.second) << ".";
}

template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rules& rs) {
	for (const auto& r : rs) print<node>(os, r) << " ";
	return os;
}

template <NodeType node>
std::ostream& print(std::ostream& os, const rr<node>& rr_) {
	print<node>(os, rr_.rec_relations);
	if (rr_.main) os << tree<node>::get(rr_.main);
	return os;
}

template <NodeType node>
std::string to_str(const rewriter::rule& r) {
	std::stringstream ss;
	return print<node>(ss, r), ss.str();
}

template <NodeType node>
std::string to_str(const rewriter::rules& rs) {
	std::stringstream ss;
	return print<node>(ss, rs), ss.str();
}

template <NodeType node>
std::string to_str(const rr<node>& rr_) {
	std::stringstream ss;
	return print<node>(ss, rr_), ss.str();
}

template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rule& r) {
	return (tree<node>::get(r.first).dump(os) << " := ",
			tree<node>::get(r.second).dump(os) << ".");
}

template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rules& rs) {
	for (const auto& r : rs) dump<node>(os, r);
	return os;
}

template <NodeType node>
std::ostream& dump(std::ostream& os, const rr<node>& rr_) {
	dump<node>(os, rr_.rec_relations);
	if (rr_.main) tree<node>::get(rr_.main).dump(os);
	return os;
}

template <NodeType node>
std::string dump_to_str(const rewriter::rule& r) {
	std::stringstream ss;
	return dump<node>(ss, r), ss.str();
}

template <NodeType node>
std::string dump_to_str(const rewriter::rules& rs) {
	std::stringstream ss;
	return dump<node>(ss, rs), ss.str();
}

template <NodeType node>
std::string dump_to_str(const rr<node>& rr_) {
	std::stringstream ss;
	return dump<node>(ss, rr_), ss.str();
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

template <NodeType node>
std::string tree<node>::to_str() const {
	std::stringstream ss;
	return print(ss), ss.str();
}

template <NodeType node>
std::string tree<node>::tree_to_str() const {
	std::stringstream ss;
	return print_tree(ss), ss.str();
}

template <NodeType node>
std::string tree<node>::dump_to_str(bool subtree) const {
	std::stringstream ss;
	return dump(ss, subtree), ss.str();
}

template <NodeType node>
std::ostream& tree<node>::dump(std::ostream& os, bool subtree) const {
	return dump(os, get(), subtree);
}

template <NodeType node>
const tree<node>& tree<node>::dump(bool subtree) const {
	return dump(std::cout, subtree), *this;
}

template <NodeType node>
std::ostream& tree<node>::dump(std::ostream& os, tref n, bool subtree) {
	const auto& t = get(n);
	t.print(os) << " \t\t " << t.value;
	if (bool print_lcrs_pointers = false;
		print_lcrs_pointers)
	{
		os << " [" << n ;
		if (t.has_right_sibling()) os << " >> " << t.right_sibling();
		if (t.has_child())         os << " __ " << t.left_child();
		os << "] ";
	}
	if (subtree) t.print_in_line(os << " \t#\t");
	return os;
}

//------------------------------------------------------------------------------
// print

// pretty printer settings
inline bool pretty_printer_highlighting = false;
inline bool pretty_printer_indenting    = false;

template <NodeType node>
std::ostream& tree<node>::print(std::ostream& os) const {

// TODO review indenting and syntax highlighting

	std::vector<size_t> hl_path;
	size_t depth = 0;
	std::unordered_set<tref> wraps, indented, highlighted;
	char last_written_char = 0;
	typename node::type last_quant_nt = nul;
	std::unordered_map<tref, size_t> chpos; // child positions if tracked

	auto is_to_wrap = [](size_t nt, size_t pt) {
		static const std::set<size_t> no_wrap_for = {
			bf_splitter, bf_ref, bf_neg, bf_constant, bf_t, bf_f,
			wff_ref, wff_neg, wff_t, wff_f, constraint, capture,
			variable, ref_args, start
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
			{ wff_ex,             430 },
			{ wff_all,            430 },
			{ wff_imply,          450 },
			{ wff_equiv,          460 },
			{ wff_or,             470 },
			{ wff_and,            480 },
			{ wff_xor,            490 },
			{ wff_neg,            500 },
			{ bf_interval,        501 },
			{ bf_eq,              502 },
			{ bf_neq,             503 },
			{ bf_lteq,            504 },
			{ bf_nlteq,           505 },
			{ bf_gt,              506 },
			{ bf_ngt,             507 },
			{ bf_gteq,            508 },
			{ bf_ngteq,           509 },
			{ bf_lt,              510 },
			{ bf_nlt,             511 },
			{ wff,                580 },
			{ bf_fall,            700 },
			{ bf_fex,             710 },
			{ bf_or,              720 },
			{ bf_xor,             730 },
			{ bf_and,             740 },
			{ bf_neg,             750 },
			{ bf,                 790 },
			{ bv_eq,              800 },
			{ bv_neq,             801 },
			{ bv_lteq,            802 },
			{ bv_nlteq,           803 },
			{ bv_gt,              804 },
			{ bv_ngt,             805 },
			{ bv_gteq,            806 },
			{ bv_ngteq,           807 },
			{ bv_lt,              808 },
			{ bv_nlt,             809 },
			{ bv_or,              810 },
			{ bv_nor,             811 },
			{ bv_xor,             812 },
			{ bv_xnor,            813 },
			{ bv_and,             814 },
			{ bv_nand,            815 },
			{ bv_shl,             816 },
			{ bv_shr,             817 },
			{ bv_add,             818 },
			{ bv_sub,             819 },
			{ bv_mul,             820 },
			{ bv_div,             821 },
			{ bv_mod,             822 },
			{ bv_min,             823 },
			{ bv_max,             824 },
			{ bv_neg,             825 },
			{ bv,                 826 },
			{ bv_constant,        827 },
			{ bf_matcher,         898 },
			{ bf_body,            899 },
			{ rec_relation,       900 },
			{ ref_args,           900 },
			{ bf_rule,            900 },
			{ wff_rule,           900 },
			{ wff_builder_body,   900 },
			{ wff_matcher,        900 },
			{ wff_body,           900 },
		};

		if (no_wrap_for.find(nt) != no_wrap_for.end())
			return false;
		auto p_it = prio.find(pt);
		auto n_it = prio.find(nt);
		if (p_it == prio.end() || n_it == prio.end()) {
			DBG(LOG_DEBUG << "No priority set for "
				<< (p_it == prio.end()  ? node::name(pt)
							: node::name(nt));)
			return false;
		}
		// std::cerr << "\n"
		// 	<< node::name(parent_type) << " vs " << node::name(node_type)
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
		{ bf,            TC.LIGHT_GREEN() },
		{ bv,         	 TC.GREEN() },
		{ variable,      TC.WHITE() },
		{ capture,       TC.BLUE() },
		{ wff_all,       TC.MAGENTA() },
		{ wff_ex,        TC.LIGHT_MAGENTA() },

		{ rec_relation,  TC.YELLOW() },
		{ constraint,    TC.LIGHT_MAGENTA() },
		{ io_var,        TC.WHITE() },
		{ bf_constant,   TC.LIGHT_CYAN() }

		// { rule,          TC.BG_YELLOW() },
		// { builder,       TC.BG_LIGHT_YELLOW() }
	};
	auto syntax_highlight = [&](size_t nt) {
		if (!pretty_printer_highlighting) return false;
		if (auto it = hl_colors.find(nt); it == hl_colors.end())
			return false;
		else return hl_path.push_back(nt), os << it->second, true;
	};

	auto out = [&](const auto& what) {
		std::stringstream ss; std::string str = (ss << what, ss.str());
		if (str.size()) last_written_char = str[str.size()-1];
#ifdef PRETTY_PRINTER_LOGGING_ENABLED
		std::cerr << "<" << str << ">";
#endif
		os << str;
	};

	auto on_enter = [&](tref ref, tref parent) {
		DBG(assert(ref != nullptr);)
		const auto& t = get(ref);
		size_t nt = t.get_type();
		size_t pnt = parent ? get(parent).get_type() : nul;
#ifdef PRETTY_PRINTER_LOGGING_ENABLED
		std::cerr << "[" << node::name(nt)
			  << " " << node::name(pnt) << "]";
		// t.print_in_line(std::cerr << "<") << ">";
#endif
		if (inc_indent(nt)) indented.insert(ref);
		if (syntax_highlight(nt)) highlighted.insert(ref);

		// track the position of the child for on_between
		auto track_chpos = [&]() { chpos[ref] = 0; };

		switch (nt) {
			case bf_f:              out('0'); break;
			case bf_t:              out('1'); break;
			case wff_f:             out('F'); break;
			case wff_t:             out('T'); break;
			case wff_neg:           out("!"); break;
			case first_sym:         out("first"); break;
			case last_sym:          out("last"); break;
			case fp_fallback:       out(" fallback "); break;
			// wraps
			case builder_head:
			case ref_args:          out("("); break;
			case constraint:
			case offsets:           out("["); break;
			case offset:            if (pnt == io_var) out("[");
						break;
			case bf_splitter:       out("S("); break;
			case bf_constant:
				if (t.get_ba_type() > 0) {
					out("{ ");
					out(tau::get(ref).get_ba_constant());
					out(" } : ");
					out(t.get_ba_type_name());
				} else {
					out("{ ");
					out(dict(t.data()));
					out(" } : untyped");
				}
				break;
			case scope_id: out(" _"), out(t.data()), out("_"); break;
			case wff:
			case bf:
				if (parent && is_to_wrap(t.first_tree()
					.get_type(), pnt))
				{
					wraps.insert(ref), out("(");
					if (static_cast<node::type>(nt) == wff)
							depth++, break_line();
				}
				break;

			case wff_all:
				if (last_quant_nt == wff_all) out(", ");
				else last_quant_nt = wff_all, out("all ");
				break;
			case wff_ex:
				if (last_quant_nt == wff_ex) out(", ");
				else last_quant_nt = wff_ex, out("ex ");
				break;
			case bf_fall:
				if (last_quant_nt == bf_fall) out(", ");
				else last_quant_nt = bf_fall, out("fall ");
				break;
			case bf_fex:
				if (last_quant_nt == bf_fex) out(", ");
				else last_quant_nt = bf_fex, out("fex ");
				break;

			case wff_sometimes:     out("sometimes "); break;
			case wff_always:        out("always "); break;

			case bf_builder_body:   out(" =: "); break;
			case wff_builder_body:  out(" =:: "); break;

			case rel_history:       out("%-"); break;
			case abs_history:       out("%"); break;
			case dnf_cmd:           out("dnf "); break;
			case cnf_cmd:           out("cnf "); break;
			case anf_cmd:           out("anf "); break;
			case nnf_cmd:           out("nnf "); break;
			case pnf_cmd:           out("pnf "); break;
			case mnf_cmd:           out("mnf "); break;
			case snf_cmd:           out("snf "); break;
			case onf_cmd:           out("onf "); break;
			case def_print_cmd:
			case def_rr_cmd:
			case def_list_cmd:      out("def"); break;
			case history_list_cmd:  out("history"); break;
			case history_print_cmd:
			case history_store_cmd: out("history "); break;
			case get_cmd:           out("get"); break;
			case set_cmd:           out("set "); break;
			case toggle_cmd:        out("toggle "); break;
			case quit_cmd:          out("quit"); break;
			case version_cmd:       out("version"); break;
			case clear_cmd:         out("clear"); break;
			case help_cmd:          out("help"); break;
			case file_cmd:          out("file"); break;
			case valid_cmd:         out("valid "); break;
			case sat_cmd:           out("sat "); break;
			case unsat_cmd:         out("unsat "); break;
			case solve_cmd:         out("solve "); break;
			case run_cmd:           out("run "); break;
			case normalize_cmd:     out("normalize "); break;
			case inst_cmd:          track_chpos();
						out("instantiate "); break;
			case subst_cmd:         track_chpos();
						out("substitute "); break;
			case wff_conditional:   track_chpos(); break;
			case bv_constant:
				if (auto first = tau::tt(ref) | tt::first | tt::ref; first) out(first);
				else out(t.get_bv_constant());
				break;
			default:
				if (is_string_nt(nt)) {
					if (nt == uconst_name) out("<");
					out(dict(t.data()));
					if (nt == uconst_name) out(">");
				}
				else if (is_digital_nt(nt)) out(t.data());
				else if (t.is_integer()) out(t.get_integer());
		}
		return true;
	};
	auto on_between = [&](tref left, tref parent) {
		if (parent == nullptr) return true;
		const auto& t = get(left), p = get(parent);
		size_t pnt = p.get_type();
#ifdef PRETTY_PRINTER_LOGGING_ENABLED
		size_t nt = t.get_type();
		std::cerr << "[|" << node::name(nt)
			  << " "  << node::name(pnt) << "] \n";
#endif
		// increment the position of the child
		auto inc_chpos = [&chpos, &parent]() { return chpos[parent]++;};
		// clear tracking of the child position
		auto chpos_end = [&chpos, &parent]() { chpos.erase(parent); };

		switch (pnt) {
			case bf_and:
				if (isdigit(last_written_char)
					|| t.child_is(tau::bf_constant))
						out(" ");
				break;
			case bf_or:             out("|"); break;
			case bf_xor:            out("+"); break;
			case bf_eq:             out(" = "); break;
			case bf_neq:            out(" != "); break;
			case bf_lteq:           out(" <= "); break;
			case bf_nlteq:          out(" !<= "); break;
			case bf_gt:             out(" > "); break;
			case bf_ngt:            out(" !> "); break;
			case bf_gteq:           out(" >= "); break;
			case bf_ngteq:          out(" !>= "); break;
			case bf_lt:             out(" < "); break;
			case bf_nlt:            out(" !< "); break;

			case bv_eq:             out(" =_ "); break;
			case bv_neq:            out(" !=_ "); break;
			case bv_lteq:           out(" <=_ "); break;
			case bv_nlteq:          out(" !<=_ "); break;
			case bv_gt:             out(" >_ "); break;
			case bv_ngt:            out(" !>_ "); break;
			case bv_gteq:           out(" >=_ "); break;
			case bv_ngteq:          out(" !>=_ "); break;
			case bv_lt:             out(" <_ "); break;
			case bv_nlt:            out(" !<_ "); break;

			case bv_add:            out("+"); break;
			case bv_sub:            out("-"); break;
			case bv_mul:            out("*"); break;
			case bv_div:            out("/"); break;
			case bv_mod:            out("%"); break;
			case bv_and:            out("&"); break;
			case bv_nand:           out("!&"); break;
			case bv_or:             out("|"); break;
			case bv_nor:            out("!|"); break;
			case bv_xor:            out("(+)"); break;
			case bv_xnor:           out("!(+)"); break;
			case bv_shl:            out("<<"); break;
			case bv_shr:            out(">>"); break;

			case bv_max:            out("max "); break;
			case bv_min:            out("min "); break;

			case ctn_neq:           out(" != "); break;
			case ctn_eq:            out(" = "); break;
			case ctn_gteq:          out(" >= "); break;
			case ctn_gt:            out(" > "); break;
			case ctn_lteq:          out(" <= "); break;
			case ctn_lt:            out(" < "); break;

			case wff_and:           out(" && "); break;
			case wff_or:            out(" || "); break;
			case wff_xor:           out(" ^ "); break;
			case wff_imply:         out(" -> "); break;
			case wff_equiv:         out(" <-> "); break;

			case bf_interval:       out(" <= "); break;

			case rec_relation:      out(" := "); break;
			case wff_rule:          out(" ::= "); break;
			case bf_rule:           out(" := "); break;
			case ref_args:
			case offsets:           out(", "); break;
			case shift:             out("-"); break;
			case variable:
			case bf_constant:       out(" : "); break;

			case wff_all:
			case wff_ex:
			case bf_fall:
			case bf_fex:
				if (!t.right_sibling_tree()
					.child_is(last_quant_nt)) out(" ");
				break;

			case cli:               out(". "); break;
			case wff_conditional:
				if (inc_chpos()) chpos_end(), out(" : ");
				else out(" ? ");
				break;

			case inst_cmd:
			case subst_cmd:
				switch (inc_chpos()) {
					case 1: out(" ["); break;
					case 2: out(" / "); break;
					case 3: chpos_end(); break;
				}
		};
		return true;
	};
	auto on_leave = [&](tref ref, tref parent) {
		const auto& t = get(ref);
		size_t nt = t.get_type(),
			pnt = parent ? get(parent).get_type() : nul;
#ifdef PRETTY_PRINTER_LOGGING_ENABLED
		std::cerr << "\n\t[/" << node::name(nt)
			  << " "      << node::name(pnt) << "]";
#endif
		// t.print_tree( << "leaving: ") << "\n";
		switch (nt) {
			case bf_neg:            out("'"); break;
			case main:
			case builder:
			case rec_relation:
			case wff_rule:
			case bf_rule:           out("."); break;
			case constraint:
			case offsets:
			case inst_cmd:
			case subst_cmd:         out("]"); break;
			case offset:            if (pnt == io_var) out("]");
						break;
			case bf_splitter:
			case builder_head:
			case ref_args:          out(")"); break;
			case bf:
			case wff:
				if (wraps.find(ref) != wraps.end()) {
					wraps.erase(ref), out(")");
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

} // namespace idni::tau_lang
