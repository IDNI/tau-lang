// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "builders.h"
#include "queries.h"

namespace idni::tau_lang {

template <typename... BAs>
tau<BAs...> operator&(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_and = [](const auto& l, const auto& r) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc & rc);
	};

	// trivial cases
	if ( l == _0<BAs...> || r == _0<BAs...> ) return _0<BAs...>;
	if ( l == _1<BAs...> ) return r;
	if ( r == _1<BAs...> ) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_and(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_and<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r)) {
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
			&& is_non_terminal<tau_parser::wff, BAs...>(r))
		return build_wff_and<BAs...>(l, r);
	throw std::logic_error("nso_ba and: wrong types");
}

template <typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_or<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_or<BAs...>(l, r);
	throw std::logic_error("nso_ba or: wrong types");
}

template <typename... BAs>
tau<BAs...> operator~(const tau<BAs...>& l) {
	auto bf_constant_neg = [](const auto& l) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(~lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return _1<BAs...>;
	if (l == _1<BAs...>) return _0<BAs...>;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_neg(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return build_bf_neg<BAs...>(l);
	if (is_child_non_terminal<tau_parser::bf_eq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(~ll);
	}
	if (is_child_non_terminal<tau_parser::bf_neq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(~ll);
	}
	if (is_non_terminal<tau_parser::wff>(l))
		return build_wff_neg<BAs...>(l);
	throw std::logic_error("nso_ba neg: wrong types");
}

template <typename... BAs>
tau<BAs...> operator^(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_xor = [](const auto& l, const auto& r) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc ^ rc);
	};

	// trivial cases
	if (l == _0<BAs...>) return r;
	if (r == _0<BAs...>) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_xor(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_xor<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_xor<BAs...>(l, r);
	throw std::logic_error("nso_ba xor: wrong types");
}

template <typename... BAs>
tau<BAs...> operator+(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return l ^ r;
}

template <typename... BAs>
bool is_zero(const tau<BAs...>& l) {
	auto bf_constant_is_zero = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_zero(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return true;
	if (l == _1<BAs...>) return false;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_zero(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _0<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _F<BAs...>;
	throw std::logic_error("nso_ba is_zero: wrong types");
}

template <typename... BAs>
bool is_one(const tau<BAs...>& l) {
	auto bf_constant_is_one = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_one(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return false;
	if (l == _1<BAs...>) return true;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_one(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _1<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _T<BAs...>;
	throw std::logic_error("nso_ba is_one: wrong types");
}

// We overload the == operator for tau in order to account for typed constants
template <typename... BAs>
bool operator==(const tau<BAs...>& l, const tau<BAs...>& r) {
	if (r == nullptr && l == nullptr) return true;
	if (r == nullptr || l == nullptr) return false;

	if (std::addressof(*l) == std::addressof(*r)) return true;
	if (l->hash != r->hash) return false;

	// check if typed bf_f or bf_t
	if (is_non_terminal(tau_parser::bf_f, l)
			&& is_non_terminal(tau_parser::bf_f, r)) {
		// In case it is untyped, ignore other type
		return (!l->child.empty() && !r->child.empty())
			? (l->child[0] == r->child[0])
			: true;
	}

	if (is_non_terminal(tau_parser::bf_t, l)
			&& is_non_terminal(tau_parser::bf_t, r)) {
		// In case it is untyped, ignore other type
		return (!l->child.empty() && !r->child.empty())
			? (l->child[0] == r->child[0])
			: true;
	}

	return (l->value == r->value && l->child == r->child);
}

// Also define != again in terms of ==
template <typename... BAs>
bool operator!=(const tau<BAs...>& l, const tau<BAs...>& r) {
	return !(l == r);
}

// We overload spaceship operator in order to have deterministic operators across
// program runs
// In this comparison typed and non-typed Tau constants are considered different
template<typename... BAs>
std::weak_ordering operator<=>(const tau<BAs...>& l, const tau<BAs...>& r) {
	if (l == nullptr && r == nullptr) return std::weak_ordering::equivalent;
	if (l == nullptr) return std::weak_ordering::less;
	if (r == nullptr) return std::weak_ordering::greater;

	// Identical objects compare equivalent
	if (std::addressof(*l) == std::addressof(*r)) return std::weak_ordering::equivalent;
	// If the hash is different, compare hash
	if (l->hash != r->hash) return l->hash <=> r->hash;
	// If value is different, compare value
	if (l->value != r->value) return l->value <=> r->value;
	// If value is same, compare children
	// For performance reasons, put the child length first
	if (l->child.size() != r->child.size())
		return l->child.size() <=> r->child.size();
	return l->child <=> r->child;
}

// We list all ordering operators explicitly
template<typename... BAs>
bool operator<(const tau<BAs...>& l, const tau<BAs...>& r) {
	return (l <=> r) < 0;
}
template<typename... BAs>
bool operator<=(const tau<BAs...>& l, const tau<BAs...>& r) {
	return (l <=> r) <= 0;
}
template<typename... BAs>
bool operator>(const tau<BAs...>& l, const tau<BAs...>& r) {
	return (l <=> r) > 0;
}
template<typename... BAs>
bool operator>=(const tau<BAs...>& l, const tau<BAs...>& r) {
	return (l <=> r) >= 0;
}

template <typename... BAs>
bool operator==(const tau<BAs...>& l, const bool& r) {
	return r ? is_one(l) : is_zero(l);
}

template <typename... BAs>
bool operator==(const bool l, const tau<BAs...>& r) {
	return r == l;
}

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template <typename... BAs>
tau<BAs...> splitter(const tau<BAs...>& n,
	splitter_type st)
{
	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& n) -> std::variant<BAs...> {
		return splitter(n, st);
	};

	assert(std::holds_alternative<std::variant<BAs...>>(trim2(n)->value));
	auto ba_constant = get<std::variant<BAs...>>(trim2(n)->value);
	std::variant<BAs...> v = std::visit(_splitter, ba_constant);
	return build_bf_constant<BAs...>(v);
}

} // namespace idni::tau_lang



// outputs a tau<...> to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	std::vector<size_t>& hl_path, size_t& depth,
	size_t parent = tau_parser::start, bool passthrough = false);

template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	size_t parent = tau_parser::start, bool passthrough = false)
{
	std::vector<size_t> hl_path;
	size_t depth = 0;
	return pp(stream, n, hl_path, depth, parent, passthrough);
}

template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::tau<BAs...>& n) { return pp(stream, n); }

// << for node<tau_sym>
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n)
{
	return stream << std::make_shared<
			idni::rewriter::node<idni::tau_lang::tau_sym<BAs...>>>(n);
}

// old operator<< renamed to print_terminals and replaced by
// pp pretty priniter
template <typename... BAs>
std::ostream& print_terminals(std::ostream& stream,
	const idni::tau_lang::tau<BAs...>& n)
{
	stream << n->value;
	for (const auto& c : n->child) print_terminals<BAs...>(stream, c);
	return stream;
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream,
			const idni::tau_lang::sp_tau_source_node& n);

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream,
					const idni::tau_lang::tau_source_node& n);

inline static const std::map<size_t, std::string> hl_colors = {
	{ tau_parser::bf,            idni::TC.LIGHT_GREEN() },
	{ tau_parser::variable,      idni::TC.WHITE() },
	{ tau_parser::capture,       idni::TC.BLUE() },
	{ tau_parser::wff_all,       idni::TC.MAGENTA() },
	{ tau_parser::wff_ex,        idni::TC.LIGHT_MAGENTA() },

	{ tau_parser::rec_relation,  idni::TC.YELLOW() },
	{ tau_parser::constraint,    idni::TC.LIGHT_MAGENTA() },
	{ tau_parser::io_var,        idni::TC.WHITE() },
	{ tau_parser::constant,      idni::TC.LIGHT_CYAN() }

	// { tau_parser::rule,          idni::TC.BG_YELLOW() },
	// { tau_parser::builder,       idni::TC.BG_LIGHT_YELLOW() }
};

inline static const std::vector<size_t> breaks = {
	tau_parser::wff_and, tau_parser::wff_or, tau_parser::wff_xor,
	tau_parser::wff_imply, tau_parser::wff_equiv,
	tau_parser::wff_all, tau_parser::wff_ex
};

inline static const std::vector<size_t> indents = {
	tau_parser::wff_sometimes, tau_parser::wff_always,
	tau_parser::wff_conditional,
	tau_parser::wff_all, tau_parser::wff_ex,
	tau_parser::wff_imply, tau_parser::wff_equiv
};

template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	std::vector<size_t>& hl_path, size_t& depth, size_t parent,
	bool passthrough)
{
	using namespace idni;
// #define DEBUG_PP
// #ifdef DEBUG_PP
// auto& p = tau_parser::instance();
// 	auto dbg = [&stream, &p](const auto& c) {
// 		if (std::holds_alternative<idni::tau_lang::tau_source_sym>(c->value)) {
// 			auto tss = std::get<idni::tau_lang::tau_source_sym>(c->value);
// 			if (tss.nt()) stream << " NT:" << p.name(tss.n()) << " ";
// 			else if (tss.is_null()) stream << " <NULL> ";
// 			else stream << " T:'" << tss.t() << "' ";
// 		} else stream << " NONLIT:`" <<c->value << "` ";
// 	};
// 	stream << "\n";
// 	dbg(n);
// 	stream << "    child.size: " << n->child.size() << "\n";
// 	for (const auto& c : n->child)
// 		stream << "\t", dbg(c), stream << "\n";
// #endif // DEBUG_PP
	static auto is_to_wrap = [](const idni::tau_lang::tau<BAs...>& n,
		size_t parent)
	{
		static const std::set<size_t> no_wrap_for = {
			tau_parser::constraint,
			tau_parser::bf_splitter,
			tau_parser::bf_ref,
			tau_parser::bf_neg,
			tau_parser::bf_constant,
			tau_parser::bf_t,
			tau_parser::bf_f,
			tau_parser::wff_ref,
			tau_parser::wff_neg,
			tau_parser::wff_t,
			tau_parser::wff_f,
			tau_parser::capture,
			tau_parser::variable,
			tau_parser::uninterpreted_constant,
			tau_parser::ref_args,
			tau_parser::start
		};
		// lower number = higher priority
		static const std::map<size_t, size_t> prio = {
			{ tau_parser::start,                             0 },
			// cli commands
			{ tau_parser::help_cmd,                         50 },
			{ tau_parser::file_cmd,                         50 },
			{ tau_parser::normalize_cmd,                    50 },
			{ tau_parser::run_cmd,                          50 },
			{ tau_parser::solve_cmd,                        50 },
			{ tau_parser::dnf_cmd,                          50 },
			{ tau_parser::cnf_cmd,                          50 },
			{ tau_parser::anf_cmd,                          50 },
			{ tau_parser::nnf_cmd,                          50 },
			{ tau_parser::pnf_cmd,                          50 },
			{ tau_parser::mnf_cmd,                          50 },
			{ tau_parser::onf_cmd,                          50 },
			{ tau_parser::inst_cmd,                         50 },
			{ tau_parser::subst_cmd,                        50 },
			{ tau_parser::def_rr_cmd,                       50 },
			{ tau_parser::def_list_cmd,                     50 },
			{ tau_parser::history_list_cmd,                 50 },
			{ tau_parser::history_print_cmd,                50 },
			{ tau_parser::history_store_cmd,                50 },
			{ tau_parser::sat_cmd,                        50 },
			{ tau_parser::main,                             60 },
			{ tau_parser::bf_rule,                          60 },
			{ tau_parser::wff_rule,                         60 },
			{ tau_parser::ref,                              80 },
			{ tau_parser::wff,                              90 },
			// wff
			{ tau_parser::wff_sometimes,                   380 },
			{ tau_parser::wff_always,                      390 },
			{ tau_parser::wff_conditional,                 400 },
			{ tau_parser::wff_all,                         430 },
			{ tau_parser::wff_ex,                          440 },
			{ tau_parser::wff_imply,                       450 },
			{ tau_parser::wff_equiv,                       460 },
			{ tau_parser::wff_or,                          470 },
			{ tau_parser::wff_and,                         480 },
			{ tau_parser::wff_xor,                         490 },
			{ tau_parser::wff_neg,                         500 },
			{ tau_parser::bf_interval,                     501 },
			{ tau_parser::bf_eq,                           502 },
			{ tau_parser::bf_neq,                          503 },
			{ tau_parser::bf_less_equal,                   504 },
			{ tau_parser::bf_nleq,                         505 },
			{ tau_parser::bf_greater,                      506 },
			{ tau_parser::bf_ngreater,                     507 },
			{ tau_parser::bf_greater_equal,                508 },
			{ tau_parser::bf_ngeq,                         509 },
			{ tau_parser::bf_less,                         510 },
			{ tau_parser::bf_nless,                        511 },
			{ tau_parser::wff,                             580 },
			// bf
			{ tau_parser::bf_or,                           720 },
			{ tau_parser::bf_and,                          730 },
			{ tau_parser::bf_xor,                          740 },
			{ tau_parser::bf_neg,                          750 },
			{ tau_parser::bf,                              790 },

			{ tau_parser::rec_relation,                    800 },
			{ tau_parser::ref_args,                        800 },
			{ tau_parser::bf_rule,                         800 },
			{ tau_parser::wff_rule,                        800 },
			{ tau_parser::binding,                         800 },
		};
		static const std::set<size_t> wrap_child_for = {
			};
		if (std::holds_alternative<idni::tau_lang::tau_source_sym>(n->value)) {
			auto tss = std::get<idni::tau_lang::tau_source_sym>(n->value);
			if (!tss.nt() || no_wrap_for.find(tss.n())
						!= no_wrap_for.end())
								return false;
			if (prio.find(tss.n()) == prio.end()) {
				std::cerr << "No priority for " << tss.n() << "\n";
				return false;
			}
			if (prio.find(parent) == prio.end()) {
				std::cerr << "No priority for " << parent << "\n";
				return false;
			}
			// tau_parser& p = tau_parser::instance();
			// std::cerr
			// 	<< p.name(parent) << " vs " << p.name(tss.n())
			// 	//<< "(" << parent << ")
			// 	<< " " << prio.at(parent)
			// 	<< (prio.at(parent) > prio.at(tss.n())
			// 		? " > " : " <= ")
			// 	<< prio.at(tss.n())
			// 	// << " (" << tss.n() << ")"
			// 	<< "\n";
			return prio.at(parent) > prio.at(tss.n()) ? true
					: wrap_child_for.contains(parent);
		}
		return false;
	};

	if (passthrough) { // passthrough
		for (const auto& c : n->child)
			pp(stream, c, hl_path, depth, parent);
		return stream;
	}

	if (std::holds_alternative<idni::tau_lang::tau_source_sym>(n->value)) {
		auto& ch = n->child;
		auto tss = std::get<idni::tau_lang::tau_source_sym>(n->value);
		auto ppch = [&](size_t i) -> std::ostream& {
			return pp(stream, ch[i], hl_path, depth, tss.n());
		};
		auto indent = [&depth, &stream]() {
			if (!idni::tau_lang::pretty_printer_indenting) return;
			for (size_t i = 0; i < depth; ++i) stream << "\t";
		};
		auto break_line = [&]() {
			if (!idni::tau_lang::pretty_printer_indenting) return;
			stream << "\n", indent();
		};
		auto break_if_needed = [&]() -> bool {
			if (!idni::tau_lang::pretty_printer_indenting) return false;
			if (find(breaks.begin(), breaks.end(), tss.n())
				!= breaks.end()) return break_line(), true;
			return false;
		};
		auto sep = [&](const std::string& separator) {
			for (size_t i = 0; i < ch.size(); ++i) {
				if (i > 0) stream << separator << " ";
				ppch(i);
			}
		};
		auto pass = [&]() {
			sep("");
		};
		// auto pass_nows = [&]() {
		// 	for (const auto& c : ch)
		// 		pp(stream, c, hl_path, depth, tss.n());
		// };
		auto infix_nows = [&](const std::string& op) {
			ppch(0);
			if (ch.size() == 1) return;
			stream << op, break_if_needed();
			ppch(1);
		};
		auto infix = [&](const std::string& op) {
			infix_nows(std::string(" ") + op + " ");
		};
		auto infix2 = [&](const std::string& op1,
			const std::string& op2)
		{
			infix(op1);
			stream << " " << op2 << " ";
			ppch(2);
		};
		auto postfix_nows = [&](const std::string& postf) {
			pass(), stream << postf;
		};
		auto prefix_nows = [&](const std::string& pref) {
			stream << pref, pass();
		};
		auto prefix = [&](const std::string& pref) {
			prefix_nows(pref + " ");
		};
		auto wrap = [&](const std::string& pref,
			const std::string& postf)
		{
			stream << pref, pass(), stream << postf;
		};
		auto quant = [&]() {
			using namespace idni::tau_lang;
			size_t quant_nt = tss.n();
			auto qch = ch;
			switch (quant_nt) {
			case tau_parser::wff_all:  stream << "all";   break;
			case tau_parser::wff_ex:   stream << "ex";    break;
			}
			tau<BAs...> expr;
			size_t expr_nt;
			do {
				pp(stream << " ", qch[0], hl_path, depth, quant_nt);
				expr = qch[1]->child[0];
				expr_nt = expr | non_terminal_extractor<BAs...>
					| optional_value_extractor<size_t>;
				if (expr_nt == quant_nt) {
					stream << ",", qch = expr->child;
				} else {
					if (!break_if_needed()) stream << " ";
					pp(stream, expr, hl_path, depth, tss.n());
					break;
				}
			} while (true);
		};
		auto print_bf_and = [&]() {
			std::stringstream ss;
			bool is_hilight = idni::tau_lang::pretty_printer_highlighting;
			if (is_hilight)
				idni::tau_lang::pretty_printer_highlighting = false;
			pp(ss, ch[0], hl_path, depth, tss.n());
			if (is_hilight)
				idni::tau_lang::pretty_printer_highlighting = true;
			auto str = ss.str();
			if (is_hilight)
				pp(stream, ch[0], hl_path, depth, tss.n());
			else stream << str;
			char lc = str[str.size()-1];
			if (isdigit(lc) // || lc == '}'
				|| idni::tau_lang::is_child_non_terminal(
					tau_parser::bf_constant, ch[0]))
						stream << " ";
			pp(stream, ch[1], hl_path, depth, tss.n());
		};
		if (tss.nt()) { //stream /*<< "*" << tss.nts << "-"*/ << tau_parser::instance().name(tss.n()) << ":";
			// indenting and breaklines
			bool indented = false;
			if (idni::tau_lang::pretty_printer_indenting)
				if (find(indents.begin(), indents.end(),
					tss.n()) != indents.end())
						indented = true, depth++;
			// syntax highlighting color start
			bool hl_pop = false;
			if (idni::tau_lang::pretty_printer_highlighting)
				if (auto it = hl_colors.find(tss.n());
					it != hl_colors.end())
						hl_path.push_back(tss.n()),
						hl_pop = true,
						stream << it->second;
			switch (tss.n()) {
			case tau_parser::main:
			case tau_parser::builder:
				postfix_nows("."); break;
			case tau_parser::rec_relation:
				infix(":=");  stream << "."; break;
			case tau_parser::wff_rule:
				infix("::="); stream << "."; break;
			case tau_parser::bf_rule:
				infix(":=");  stream << "."; break;
			case tau_parser::bf_builder_body:  prefix("=:");  break;
			case tau_parser::wff_builder_body: prefix("=::"); break;
			case tau_parser::in:
			case tau_parser::out:
				infix_nows("["); stream << "]"; break;
			// wrappable by parenthesis
			case tau_parser::bf:
			case tau_parser::wff:
			{
				bool wrap = is_to_wrap(ch[0], parent);
				if (wrap) {
					stream << "(";
					if (tss.n() == tau_parser::wff)
						depth++, break_line();
				}
				pp(stream, ch[0], hl_path, depth, parent);
				if (wrap) {
					if (tss.n() == tau_parser::wff)
						depth--, break_line();
					stream << ")";
				}
			} break;
			case tau_parser::shift:
				if (ch.size() == 1) pass();
				else infix_nows("-");
				break;
			case tau_parser::bf_constant:
				stream << "{ ", ppch(0) << " }";
				if (ch.size() > 1) stream << " : ", ppch(1);
				break;
			// nodes to wrap
			case tau_parser::constraint: wrap("[", "]"); break;
			case tau_parser::bf_splitter: wrap("S(", ")"); break;
				wrap("{ ", " }"); break;
			case tau_parser::builder_head:
				wrap("(" , ")"); break;
			case tau_parser::offsets:
				stream << "[", sep(","), stream << "]"; break;
			case tau_parser::ref_args:
				stream << "(", sep(","), stream << ")"; break;
			// unary operators
			case tau_parser::wff_neg:        prefix_nows("!"); break;
			case tau_parser::bf_neg:         postfix_nows("'"); break;
			case tau_parser::wff_sometimes:  prefix("sometimes"); break;
			case tau_parser::wff_always:     prefix("always"); break;
			//
			// binary operators
			case tau_parser::bf_and:         print_bf_and(); break;
			case tau_parser::bf_or:          infix_nows("|"); break;
			case tau_parser::bf_xor:         infix_nows("+"); break;
			case tau_parser::bf_eq:             infix("="); break;
			case tau_parser::bf_neq:            infix("!="); break;
			case tau_parser::bf_less_equal:     infix("<="); break;
			case tau_parser::bf_nleq:           infix("!<="); break;
			case tau_parser::bf_greater:        infix(">"); break;
			case tau_parser::bf_ngreater:       infix("!>"); break;
			case tau_parser::bf_greater_equal:  infix(">="); break;
			case tau_parser::bf_ngeq:           infix("!>="); break;
			case tau_parser::bf_less:           infix("<"); break;
			case tau_parser::bf_nless:          infix("!<"); break;
			case tau_parser::ctn_neq:           infix("!="); break;
			case tau_parser::ctn_eq:            infix("=");  break;
			case tau_parser::ctn_greater_equal: infix(">="); break;
			case tau_parser::ctn_greater:       infix(">");  break;
			case tau_parser::ctn_less_equal:    infix("<="); break;
			case tau_parser::ctn_less:          infix("<");  break;
			case tau_parser::wff_and:        infix("&&"); break;
			case tau_parser::wff_or:         infix("||"); break;
			case tau_parser::wff_xor:        infix("^"); break;
			case tau_parser::wff_imply:      infix("->"); break;
			case tau_parser::wff_equiv:      infix("<->"); break;
			// ternary operators
			case tau_parser::bf_interval:    infix2("<=", "<="); break;
			case tau_parser::wff_conditional:infix2("?", ":"); break;
			// quantifiers
			case tau_parser::wff_all:
			case tau_parser::wff_ex:         quant(); break;
			// cli commands
			case tau_parser::cli:           sep(". "); break;
			case tau_parser::rel_memory:    prefix_nows("%-"); break;
			case tau_parser::abs_memory:    prefix_nows("%"); break;
			case tau_parser::quit_cmd:      stream << "quit"; break;
			case tau_parser::version_cmd:   stream << "version"; break;
			case tau_parser::clear_cmd:     stream << "clear"; break;
			case tau_parser::help_cmd:      prefix("help"); break;
			case tau_parser::file_cmd:      prefix("file"); break;
			case tau_parser::valid_cmd:     prefix("valid"); break;
			case tau_parser::sat_cmd:       prefix("sat"); break;
			case tau_parser::unsat_cmd:     prefix("unsat"); break;
			case tau_parser::solve_cmd:     prefix("solve"); break;
			case tau_parser::run_cmd:       prefix("run"); break;
			case tau_parser::normalize_cmd: prefix("normalize"); break;
			case tau_parser::inst_cmd:
			case tau_parser::subst_cmd:
				stream << (tss.n() == tau_parser::inst_cmd
						? "instantiate" : "substitute")
					<< " ", ppch(1) << " [", ppch(2)
					<< " / ", ppch(3) << "]";
				break;
			case tau_parser::dnf_cmd:       prefix("dnf"); break;
			case tau_parser::cnf_cmd:       prefix("cnf"); break;
			case tau_parser::anf_cmd:       prefix("anf"); break;
			case tau_parser::nnf_cmd:       prefix("nnf"); break;
			case tau_parser::pnf_cmd:       prefix("pnf"); break;
			case tau_parser::mnf_cmd:       prefix("mnf"); break;
			case tau_parser::snf_cmd:       prefix("snf"); break;
			case tau_parser::onf_cmd:       prefix("onf"); break;
			case tau_parser::def_print_cmd:
			case tau_parser::def_rr_cmd:    prefix("def"); break;
			case tau_parser::def_list_cmd:  stream << "def"; break;
			case tau_parser::history_list_cmd:
						stream << "history"; break;
			case tau_parser::history_print_cmd:
			case tau_parser::history_store_cmd:
							prefix("history"); break;
			case tau_parser::get_cmd:       prefix("get"); break;
			case tau_parser::set_cmd:       prefix("set"); break;
			case tau_parser::toggle_cmd:    prefix("toggle"); break;
			// just print terminals for these
			case tau_parser::in_var_name:
			case tau_parser::out_var_name:
			case tau_parser::chars:
			case tau_parser::capture:
			case tau_parser::sym:
			case tau_parser::ctnvar:
			case tau_parser::num:
			case tau_parser::type:
			case tau_parser::source:
			case tau_parser::named_binding:
			case tau_parser::uninter_const_name:
			case tau_parser::option_name:
			case tau_parser::option_value:
				print_terminals(stream, n);
				break;
			// constants
			case tau_parser::uninterpreted_constant:
							wrap("<", ">"); break;
			// fixed point calculation fallback
			case tau_parser::fp_fallback: prefix(" fallback"); break;
			// simple symbols
			case tau_parser::first_sym: stream << "first"; break;
			case tau_parser::last_sym: stream << "last"; break;
			case tau_parser::bf_f:  stream << '0'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
			case tau_parser::bf_t:  stream << '1'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
			case tau_parser::wff_f: stream << 'F'; break;
			case tau_parser::wff_t: stream << 'T'; break;
			// for the rest skip value and just passthrough to child
			default: for (const auto& c : n->child)
					pp(stream, c, hl_path, depth, parent);
				break;
			}
			// indenting and breaklines
			if (idni::tau_lang::pretty_printer_indenting && indented)
				depth--;
			// syntax highlighting color end
			if (idni::tau_lang::pretty_printer_highlighting && hl_pop) {
				hl_path.pop_back();
				stream << TC.CLEAR();
				if (hl_path.size()) // restore the prev color
					stream << hl_colors.at(hl_path.back());
			}
		}
		else if (!tss.is_null()) stream << tss.t();
	} else stream << n->value;
	return stream;
}
