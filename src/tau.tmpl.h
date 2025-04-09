#include "tau.h"

namespace idni::tau_lang {

static inline bool is_string_nt(size_t nt) {
	using p = node::type;
	static const std::set<size_t> string_nts{
		p::sym, p::type, p::source, p::named_binding, p::capture,
		p::variable, p::in_var_name, p::out_var_name,
		p::uninter_const_name, p::file_name, p::ctnvar, p::option_name,
		p::option_value
	};
	return string_nts.contains(nt);
}

static inline bool is_digital_nt(size_t nt) {
	using p = node::type;
	static const std::set<size_t> digital_nts{
		p::num, p::memory_id
	};
	return digital_nts.contains(nt);
}

std::ostream& operator<<(std::ostream& os, const node& n) {
	if (n.nt == node::type::integer) os << n.as_int() << " ";
	else if (n.nt == node::type::bf_constant) os << " {C" << n.data << "} ";
	else if (is_digital_nt(n.nt)) os << n.data << " ";
	else if (is_string_nt(n.nt)) os << "\"" << S[n.data] << "\" ";
	// else if (n.ext) os << "{EXT}";
	os << tau_parser::instance().name(n.nt);
#ifdef DEBUG
	if (bool print_nt_ids = true; print_nt_ids) os << "(" << n.nt << ")";
	if (n.data) os << " #" << n.data;
#endif
	return os;
}

//------------------------------------------------------------------------------
// handles

template <typename node>
tref tree<node>::get() const { return base_t::get(); }
template <typename node>
const tree<node>& tree<node>::get(const tref id) {
	return (const tree&) base_t::get(id);
}
template <typename node>
const tree<node>& tree<node>::get(const htree::sp& h) {
	return (const tree&) base_t::get(h);
}
template <typename node>
htree::sp tree<node>::geth(tref h) {return base_t::geth(h);}

//------------------------------------------------------------------------------
// creation with tref childs

template <typename node>
tref tree<node>::get(const node& v) { return base_t::get(v); }

template <typename node>
tref tree<node>::get(const node& v, tref child) {
	return base_t::get(v, child);
}

template <typename node>
tref tree<node>::get(const node& v, tref ch1, tref ch2) {
	return base_t::get(v, ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node& v, const tref* ch, size_t len) {
	return base_t::get(v, ch, len);
}

template <typename node>
tref tree<node>::get(const node& v, const trefs& children) {
	return base_t::get(v, children);
}

template <typename node>
tref tree<node>::get(const node& v, const std::initializer_list<tref>& ch) {
	return base_t::get(v, ch);
}


//------------------------------------------------------------------------------
// creation with node childs

template <typename node>
tref tree<node>::get(const node& v, const node& child) {
	return base_t::get(v, child);
}

template <typename node>
tref tree<node>::get(const node& v, const node& ch1, const node& ch2)
{
	return get(v, ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node& v, const node* ch, size_t len)
{
	return base_t::get(v, ch, len);
}

template <typename node>
tref tree<node>::get(const node& v, const std::vector<node>& ch)
{
	return base_t::get(v, ch);
}

template <typename node>
tref tree<node>::get(const node& v, const std::initializer_list<node>& ch)
{
	return base_t::get(v, ch);
}

//------------------------------------------------------------------------------
// creation of node::type and children

template <typename node>
tref tree<node>::get(const node::type& nt) { return get(node(nt)); }

template <typename node>
tref tree<node>::get(const node::type& nt, tref ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, tref ch1, tref ch2) {
	return get(node(nt), ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const tref* ch, size_t len) {
	return get(node(nt), ch, len);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const trefs& ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const std::initializer_list<tref>& ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const std::string& s) {
	size_t sid = 0;
	if (auto it = SM.find(s); it != SM.end()) sid = it->second;
	else sid = (SM.emplace(s, S.size()), S.push_back(s), S.size() - 1);
	return get(node(nt, sid));
}

//------------------------------------------------------------------------------
// terminals

template <typename node>
tref tree<node>::get_num(size_t n) {
	return get(node(tree<node>::num, n));
}

template <typename node>
tref tree<node>::get_integer(int_t n) {
	return get(node(node::type::integer, static_cast<size_t>(n)));
}

template <typename node>
tref tree<node>::get_ba_constant_id(size_t c) {
	return get(node(node::type::bf_constant, c));
}

//------------------------------------------------------------------------------
// transform from tau_parser::tree

// Converts parser tree nodes to tau tree nodes
// - terminals are collapsed into their parents
//     ie. shift > num > "10" becomes shift > num("10")
//     or  variable > type > "sbf" becomes variable > type("sbf")
// - does q_vars transformation from ex x, y to ex x ex y
// - transforms offset variables to captures
// important terminals are stored in their pools outside of the tree
// and node's value is actually id in the pool (strings, constants)
// for constants,
//     binder requires type and a tref. binder stores it and returns its id
// TODO constants binding
template <typename node>
template <typename binder>
tref tree<node>::get(const tau_parser::tree& pt, binder& bind) {
	std::unordered_map<tref, tref> m;
	auto transformer = [&m, &bind](tref t, tref parent) {
		// parse_tree::get(t).dump(std::cout << "transforming: ", true) << std::endl;
		if (m.find(t) != m.end()) return true;
		const auto& tr = parse_tree::get(t);
		if (tr.is_t()) return true;
		auto nt = tr.get_nt();
		auto parent_nt = parent != nullptr
			? parse_tree::get(parent).get_nt() : 0;
		tref x;
		// q_vars transformation (ex x,y to ex x ex y)
		if (nt == node::type::wff_ex || nt == node::type::wff_all) {
			const auto& q_vars = get(m.at(tr[0]));
			const auto& expr = get(m.at(tr[1]));
			trefs vars = q_vars.get_children();
			if (vars.empty())
				x = get(static_cast<node::type>(nt), {
						q_vars.get(), expr.get() });
			else {
				x = expr.only_child();
				for (size_t vi = 0; vi != vars.size(); ++vi) {
					// create a new quantifier node with var and new children
					x = get(static_cast<node::type>(nt), {
						vars[vars.size() - 1 - vi],
						get(node::type::wff, x) });
				}
			}
		}
		// tau tree terminals
		else if (nt == node::type::digits) { // preprocess digits
			x = get(nt, std::stoul(tr.get_terminals()));
		} else if (nt == node::type::integer) {
			bool neg = false;
			tref d = nullptr;
			for (tref c : tr.children()) {
				if (d != nullptr) neg = true;
				d = c;
			}
			int_t i = static_cast<int_t>(get(m.at(tr.only_child()))
								.data());
			x = get_integer(neg ? -i : i);
		} else if (nt == node::type::bf_constant) {
			// call binder with a type and a tref, store it and get the id
			size_t cid = bind(t);
			if (bind.error) return false;
			x = get_ba_constant_id(cid);
		} else if (is_digital_nt(nt)) {
			size_t v = get(m.at(tr.only_child())).data();
			x = get(node(nt, v));
		} else if (is_string_nt(nt))
			x = get(static_cast<node::type>(nt), tr.get_terminals());
		else {  // all other nonterminals
			trefs ch; // transform children
			for (tref c : tr.children()) {
				DBG(assert(c != nullptr);)
				DBG(assert(m.find(c) != m.end());)
				ch.push_back(m.at(c));
			}
			switch (nt) { // transform intermediate nodes
				// bf_and_nosep and bf_neg_oprnd
				case node::type::bf_and_nosep_1st_oprnd:
				case node::type::bf_and_nosep_2nd_oprnd:
				case node::type::bf_neg_oprnd:
					nt = node::type::bf; break;
				case node::type::bf_and_nosep:
					nt = node::type::bf_and; break;
				// offset variable -> capture
				case node::type::variable:
					if (parent_nt == node::type::offsets)
						nt = node::type::capture;
					break;
			}
			x = get(static_cast<node::type>(nt), ch);
		}
		m.emplace(t, x);
		// std::cout << "emplaced:" << parse_tree::get(t).value
		// 	<< " as " << tree::get(x).get_type_name() << std::endl;
		return true;
	};
	// parse_tree::get(pt.get()).print(std::cout << "parse tree: ") << std::endl;
	idni::post_order<tau_parser::pnode>(pt.get()).search(transformer);
	if (m.find(pt.get()) == m.end()) return nullptr;
	// std::cout << "tau tree: " << tree::get(m.at(pt.get())) << std::endl;
	// tree::get(m.at(pt.get())).print_tree(std::cout << "tau tree: ") << std::endl;
	return m.at(pt.get());
}

//------------------------------------------------------------------------------

template <typename tau, typename... BAs>
struct constant_binder {
	using tt = tau::tt;
	size_t operator()(tref n) const {
		if (error) return 0;
		auto t = tt(n);
		auto binding = t | tau::constant | tau::binding;
		if (!binding) return 0;
		auto type_node = t | tau::type;
		auto tid = type_node | tt::num;
		return tid;
		// TODO rewrite this for new tree
		if (tid > 0) {
			// auto nn = nso_factory<BAs...>::instance()
			// 	.binding(binding.value(), S[tid]);
			// if (!nn) return error = true, 0;
			// if (nn != binding.value())
			// 	return tree::get(type::bf_constant,
			// 		tree::get(type::constant, nn),
			// 					type_node);
			// return n;
		}
		// auto nn = nso_factory<BAs...>::instance()
		// 	.binding(binding.value(), "");
		// if (!nn) return error = true, 0;
		// return tree::get(type::bf_constant,
		// 	tree::get(type::constant, nn));
		return tid;
	}
	bool error = false;
};

template <typename N>
template <typename... BAs>
tref tree<N>::get(tau_parser::result& result) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return nullptr; // Syntax error
	}
	constant_binder<tree<N>, BAs...> binder;
	return tree<N>::template get<decltype(binder)>(
			parse_tree::get(result.get_shaped_tree2()), binder);
}

template <typename N>
template <typename... BAs>
tref tree<N>::get(const std::string& source, parse_options options) {
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options);
	return tree<N>::template get<BAs...>(result);
}

template <typename N>
template <typename... BAs>
tref tree<N>::get(std::istream& is, parse_options options) {
	auto result = tau_parser::instance().parse(is, options);
	return tree<N>::template get<BAs...>(result);
}

template <typename N>
template <typename... BAs>
tref tree<N>::get_from_file(const std::string& filename, parse_options options){
	auto result = tau_parser::instance().parse(filename, options);
	return tree<N>::template get<BAs...>(result);
}

template <typename N>
rewriter::library tree<N>::get_library(const std::string& str) {
	auto rs = tt(tree::get(str, { .start = node::type::library }))
				| node::type::rules || node::type::rule;
	rewriter::library lib;
	for (auto r : rs.traversers()) {
		r = r | tt::first;
		// tree::get(r.value()).print(std::cout << "rule: ");
		lib.emplace_back(tree::geth(r | tt::first | tt::ref),
				tree::geth(r | tt::second | tt::ref));
	}
	return lib;
}

//------------------------------------------------------------------------------

template <typename node>
size_t tree<node>::children_size() const {
	return base_t::children_size();
}

template <typename node>
bool tree<node>::get_children(tref *child, size_t& len) const {
	return base_t::get_children(child, len);
}

template <typename node>
tref_range<node> tree<node>::children() const {
	return base_t::children();
}

template <typename node>
tree_range<tree<node>> tree<node>::children_trees() const{
	return tree_range<tree<node>>(this->l);
}

template <typename node>
tref tree<node>::only_child() const {
	return base_t::only_child();
}

template <typename node>
trefs tree<node>::get_children() const {
	return base_t::get_children();
}

template <typename node>
tref tree<node>::child(size_t n) const { return base_t::child(n);}

template <typename node>
tref tree<node>::first() const { return child(0); }

template <typename node>
tref tree<node>::second() const { return child(1); }

template <typename node>
tref tree<node>::third() const { return child(2); }

template <typename node>
const tree<node>& tree<node>::child_tree(size_t n) const {
	tref c = child(n); DBG(assert(c != nullptr);)
	return get(c);
}

template <typename node>
const tree<node>& tree<node>::operator[](size_t n) const {
	return child_tree(n);
}

template <typename node>
const tree<node>& tree<node>::first_tree() const {
	return child_tree(0);
}

template <typename node>
const tree<node>& tree<node>::second_tree() const {
	return child_tree(1);
}

template <typename node>
const tree<node>& tree<node>::third_tree() const {
	return child_tree(2);
}

template <typename node>
const tree<node>& tree<node>::only_child_tree() const {
	tref c = only_child(); DBG(assert(c != nullptr);)
	return get(c);
}



template <typename node>
std::ostream& tree<node>::print_tree(std::ostream& os, size_t s) const {
	for (size_t i = 0; i < s; i++) os << "\t";
	os << this->value;
	// DBG(os << " *" << get();)
	os << "\n";
	for (const auto& ch : children()) get(ch).print_tree(os, s + 1);
	return os;
}

//------------------------------------------------------------------------------
// fast access helpers

template <typename node>
size_t tree<node>::data() const { return this->value.data; }

template <typename node>
size_t tree<node>::child_data() const { return first_tree().value.data; }

template <typename node>
bool tree<node>::is(size_t nt) const {
	return this->value.nt == nt;
}

template <typename node>
bool tree<node>::is_string() const { return is_string_nt(this->value.nt); }

template <typename node>
bool tree<node>::is_integer() const { return is(node::type::integer); }

template <typename node>
bool tree<node>::is_num() const { return is(node::type::num); }

template <typename node>
bool tree<node>::is_ba_constant() const { return is(node::type::bf_constant); }

template <typename node>
node::type tree<node>::get_type() const {
	return static_cast<node::type>(this->value.nt);
}

template <typename node>
const std::string& tree<node>::get_type_name() const {
	return tau_parser::instance().name(this->value.nt);
}

template <typename node>
const std::string& tree<node>::get_string() const {
	DBG(assert(is_string());)
	return S[this->value.data];
}

template <typename node>
int_t tree<node>::get_integer() const {
	DBG(assert(is_integer());)
	return static_cast<int_t>(this->value.data);
}

template <typename node>
size_t tree<node>::get_num() const {
	DBG(assert(is_num());)
	return this->value.data;
}


template <typename node>
size_t tree<node>::get_ba_constant_id() const {
	DBG(assert(is_ba_constant());)
	return this->value.data;
}

//------------------------------------------------------------------------------

template <typename node>
tree<node>::traverser::traverser() : has_value_(false) {}
template <typename node>
tree<node>::traverser::traverser(tref r) : has_value_(r != nullptr),
			values_(has_value_ ? trefs{ r } : trefs{}) {}
template <typename node>
tree<node>::traverser::traverser(const htree::sp& h) : has_value_(h != nullptr),
			values_(has_value_ ? trefs{ h->get() } : trefs{}) {}
template <typename node>
tree<node>::traverser::traverser(const trefs& refs) { set_values(refs); }

template <typename node>
bool tree<node>::traverser::has_value() const { return has_value_; }
template <typename node>
tree<node>::traverser::operator bool() const { return has_value(); }

template <typename node>
tref tree<node>::traverser::value() const { return values_.front(); }

template <typename node>
const tree<node>& tree<node>::traverser::value_tree()
	const { return get(values_.front()); }

template <typename node>
const tree<node>& tree<node>::traverser::operator[](
	size_t n) const { return value_tree()[n]; }

template <typename node>
const trefs& tree<node>::traverser::values() const { return values_; }

template <typename node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::traversers() const
{
	std::vector<traverser> tv;
	for (const auto& v : values_) tv.emplace_back(v);
	return tv;
}

template <typename node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::operator()() const
{
	return traversers();
}

template <typename node>
bool tree<node>::traverser::empty() const { return values_.empty(); }

template <typename node>
size_t tree<node>::traverser::size() const { return values_.size(); }

template <typename node>
typename tree<node>::traverser
	tree<node>::traverser::operator|(size_t nt) const
{
	if (!has_value()) return traverser();
	for (tref c : get(value()).children()) {
		if (get(c).is(nt)) return { c }; 
	}
	return {};
}

template <typename node>
typename tree<node>::traverser
	tree<node>::traverser::operator||(size_t nt) const
{
	trefs r;
	for (const auto& v : values())
		for (const tref& c : get(v).children()) {
			if (get(c).is(nt)) r.push_back(c); 
		}
	return traverser(r);
}

template <typename node>
template <typename result_type>
result_type tree<node>::traverser::operator|(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <typename node>
template <typename result_type>
result_type tree<node>::traverser::operator||(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <typename node>
void tree<node>::traverser::set_values(const trefs& refs) {
	for (tref t : refs) if (t != nullptr) values_.push_back(t);
	has_value_ = values_.size();
}

//------------------------------------------------------------------------------
// print
template <typename node>
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
					: tau_parser::instance().name(nt)) << "\n";
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
			case bf_constant:       os << "{ "; break;
			case bf:
			case wff:
				if (parent && is_to_wrap(t.first_tree().get_type(),
					get(parent).get_type()))
				{
					wraps.insert(ref), os << "(";
					if (nt == wff) depth++, break_line();
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
			case inst_cmd:          track_chpos(); os << "instantiate "; break;
			case subst_cmd:         track_chpos(); os << "substitute "; break;
			case wff_conditional:   track_chpos(); break;
			default:
				if (is_string_nt(nt)) os << S[t.data()];
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
		auto inc_chpos = [&chpos, &parent]() { return chpos[parent]++; };
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
			case bf_constant:  os << " }"; break;
			case bf:
			case wff:
				if (wraps.find(ref) != wraps.end()) {
					wraps.erase(ref), os << ")";
					if (nt == wff) depth--, break_line();
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
