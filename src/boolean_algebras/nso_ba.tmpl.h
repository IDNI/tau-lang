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

		auto type_l = l | tau_parser::bf_constant | tau_parser::type;
		auto type_r = r | tau_parser::bf_constant | tau_parser::type;

		assert(type_l.has_value() && type_r.has_value());
		assert(type_l == type_r);

		return build_bf_constant<BAs...>(lc & rc, type_l.value());
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
	auto bf_constant_or = [](const auto& l, const auto& r) -> tau<BAs...> {
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

		auto type_l = l | tau_parser::bf_constant | tau_parser::type;
		auto type_r = r | tau_parser::bf_constant | tau_parser::type;

		assert(type_l.has_value() && type_r.has_value());
		assert(type_l == type_r);

		return build_bf_constant<BAs...>(lc | rc, type_l.value());
	};

	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_or(l, r);
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

		auto type_l = l | tau_parser::bf_constant | tau_parser::type;
		assert(type_l.has_value());

		return build_bf_constant<BAs...>(~lc, type_l.value());
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

		auto type_l = l | tau_parser::bf_constant | tau_parser::type;
		auto type_r = r | tau_parser::bf_constant | tau_parser::type;

		assert(type_l.has_value() && type_r.has_value());
		assert(type_l == type_r);

		return build_bf_constant<BAs...>(lc ^ rc, type_l.value());
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

	if (l->value != r->value) return false;
	if (l->child.size() != r->child.size()) return false;

	//compare children
	for (size_t i = 0; i < l->child.size(); ++i)
		if (!(l->child[i] == r->child[i])) return false;
	return true;
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
	using p = tau_parser;
	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& n) -> std::variant<BAs...> {
		return splitter(n, st);
	};

	assert(is_non_terminal(tau_parser::bf_constant, n));
	assert(std::holds_alternative<std::variant<BAs...>>(trim2(n)->value));
	auto ba_constant = get<std::variant<BAs...>>(trim2(n)->value);
	std::variant<BAs...> v = std::visit(_splitter, ba_constant);

	auto type = n | p::type;
	assert(type.has_value());

	return build_bf_constant<BAs...>(v, type.value());
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
	{ tau_parser::bf_variable,   idni::TC.WHITE() },
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

// TODO (HIGH) rewrite pp() to use visitor traverser  
template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	std::vector<size_t>& hl_path, size_t& depth, size_t parent,
	bool passthrough)
{
	using namespace idni;
	using namespace idni::tau_lang;
	using p = tau_parser;
#ifdef DEBUG
	static size_t rdepth = 0;
	if (++rdepth == 50) return rdepth--, stream << "... (reached limit)";
#endif
// #define DEBUG_PP
// #ifdef DEBUG_PP
// auto& p = tau_parser::instance();
// 	auto dbg = [&stream, &p](const auto& c) {
// 		if (std::holds_alternative<tau_source_sym>(c->value)) {
// 			auto tss = std::gettau_source_sym>(c->value);
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
	static auto is_to_wrap = [](const tau<BAs...>& n,
		size_t parent)
	{
		static const std::set<size_t> no_wrap_for = {
			p::constraint,
			p::bf_splitter,
			p::bf_ref,
			p::bf_neg,
			p::bf_constant,
			p::bf_t,
			p::bf_f,
			p::wff_ref,
			p::wff_neg,
			p::wff_t,
			p::wff_f,
			p::capture,
			p::bf_variable,
			p::uninterpreted_constant,
			p::ref_args,
			p::start
		};
		// lower number = higher priority
		static const std::map<size_t, size_t> prio = {
			{ p::start,                             0 },
			// cli commands
			{ p::help_cmd,                         50 },
			{ p::file_cmd,                         50 },
			{ p::normalize_cmd,                    50 },
			{ p::run_cmd,                          50 },
			{ p::solve_cmd,                        50 },
			{ p::dnf_cmd,                          50 },
			{ p::cnf_cmd,                          50 },
			{ p::anf_cmd,                          50 },
			{ p::nnf_cmd,                          50 },
			{ p::pnf_cmd,                          50 },
			{ p::mnf_cmd,                          50 },
			{ p::onf_cmd,                          50 },
			{ p::inst_cmd,                         50 },
			{ p::subst_cmd,                        50 },
			{ p::def_rr_cmd,                       50 },
			{ p::def_list_cmd,                     50 },
			{ p::history_list_cmd,                 50 },
			{ p::history_print_cmd,                50 },
			{ p::history_store_cmd,                50 },
			{ p::sat_cmd,                        50 },
			{ p::main,                             60 },
			{ p::bf_rule,                          60 },
			{ p::wff_rule,                         60 },
			{ p::ref,                              80 },
			{ p::wff,                              90 },
			// wff
			{ p::wff_sometimes,                   380 },
			{ p::wff_always,                      390 },
			{ p::wff_conditional,                 400 },
			{ p::wff_all,                         430 },
			{ p::wff_ex,                          440 },
			{ p::wff_imply,                       450 },
			{ p::wff_equiv,                       460 },
			{ p::wff_or,                          470 },
			{ p::wff_and,                         480 },
			{ p::wff_xor,                         490 },
			{ p::wff_neg,                         500 },
			{ p::bf_interval,                     501 },
			{ p::bf_eq,                           502 },
			{ p::bf_neq,                          503 },
			{ p::bf_less_equal,                   504 },
			{ p::bf_nleq,                         505 },
			{ p::bf_greater,                      506 },
			{ p::bf_ngreater,                     507 },
			{ p::bf_greater_equal,                508 },
			{ p::bf_ngeq,                         509 },
			{ p::bf_less,                         510 },
			{ p::bf_nless,                        511 },
			{ p::wff,                             580 },
			// bf
			{ p::variable,                        710 },
			{ p::bf_or,                           720 },
			{ p::bf_and,                          730 },
			{ p::bf_xor,                          740 },
			{ p::bf_neg,                          750 },
			{ p::bf,                              790 },

			{ p::rec_relation,                    800 },
			{ p::ref_args,                        800 },
			{ p::bf_rule,                         800 },
			{ p::wff_rule,                        800 },
			{ p::binding,                         800 },
			{ p::wff_builder_body,                800 },

		};
		static const std::set<size_t> wrap_child_for = {
			};
		if (std::holds_alternative<tau_source_sym>(n->value)) {
			auto tss = std::get<tau_source_sym>(n->value);
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

	if (std::holds_alternative<tau_source_sym>(n->value)) {
		auto& ch = n->child;
		auto tss = std::get<tau_source_sym>(n->value);
		auto ppch = [&](size_t i) -> std::ostream& {
			return pp(stream, ch[i], hl_path, depth, tss.n());
		};
		auto indent = [&depth, &stream]() {
			if (!pretty_printer_indenting) return;
			for (size_t i = 0; i < depth; ++i) stream << "\t";
		};
		auto break_line = [&]() {
			if (!pretty_printer_indenting) return;
			stream << "\n", indent();
		};
		auto break_if_needed = [&]() -> bool {
			if (!pretty_printer_indenting) return false;
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
			case p::wff_all:  stream << "all";   break;
			case p::wff_ex:   stream << "ex";    break;
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
			bool is_hilight = pretty_printer_highlighting;
			if (is_hilight) pretty_printer_highlighting = false;
			pp(ss, ch[0], hl_path, depth, tss.n());
			if (is_hilight) pretty_printer_highlighting = true;
			auto str = ss.str();
			if (is_hilight)
				pp(stream, ch[0], hl_path, depth, tss.n());
			else stream << str;
			char lc = str[str.size()-1];
			if (isdigit(lc) // || lc == '}'
				|| is_child_non_terminal(p::bf_constant, ch[0]))
						stream << " ";
			pp(stream, ch[1], hl_path, depth, tss.n());
		};
		// static auto is_type_node = [](tau_sym<BAs...>& s) {
		// 	return std::holds_alternative<tau_source_sym>(s)
		// 		&& std::get<tau_source_sym>(s).nt()
		// 		&& std::get<tau_source_sym>(s).n()
		// 					== tau_parser::type;
		// };
		if (tss.nt()) { //stream /*<< "*" << tss.nts << "-"*/ << tau_parser::instance().name(tss.n()) << ":";
			// indenting and breaklines
			bool indented = false;
			if (pretty_printer_indenting)
				if (find(indents.begin(), indents.end(),
					tss.n()) != indents.end())
						indented = true, depth++;
			// syntax highlighting color start
			bool hl_pop = false;
			if (pretty_printer_highlighting)
				if (auto it = hl_colors.find(tss.n());
					it != hl_colors.end())
						hl_path.push_back(tss.n()),
						hl_pop = true,
						stream << it->second;
			switch (tss.n()) {
			case p::main:
			case p::builder:
				postfix_nows("."); break;
			case p::rec_relation:
				infix(":=");  stream << "."; break;
			case p::wff_rule:
				infix("::="); stream << "."; break;
			case p::bf_rule:
				infix(":=");  stream << "."; break;
			case p::bf_builder_body:  prefix("=:");  break;
			case p::wff_builder_body: prefix("=::"); break;
			case p::in:
			case p::out:
				infix_nows("["); stream << "]"; break;
			// wrappable by parenthesis
			case p::bf:
			case p::wff:
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
			case p::shift:
				if (ch.size() == 1) pass();
				else infix_nows("-");
				break;
			case p::scope_id:
				stream << "_", ppch(0), stream << "_ ";
				break;
			case p::type: {
					std::stringstream ss;
					print_terminals(ss, n);
					if (ss.tellp()) stream << ss.str();
					else stream << "untyped";
				}
				break;
			case p::bf_constant:
				stream << "{ "; // fall through
			case p::bf_variable:
				ppch(0);
				if (tss.n() == tau_parser::bf_constant)
					stream << " }";
				if (ch.size() > 1) // && is_type_node(ch[1]->value))
					stream << " : ", ppch(1);
				break;
			// nodes to wrap
			case p::constraint: wrap("[", "]"); break;
			case p::bf_splitter: wrap("S(", ")"); break;
				wrap("{ ", " }"); break;
			case p::builder_head:
				wrap("(" , ")"); break;
			case p::offsets:
				stream << "[", sep(","), stream << "]"; break;
			case p::ref_args:
				stream << "(", sep(","), stream << ")"; break;
			// unary operators
			case p::wff_neg:        prefix_nows("!"); break;
			case p::bf_neg:         postfix_nows("'"); break;
			case p::wff_sometimes:  prefix("sometimes"); break;
			case p::wff_always:     prefix("always"); break;
			//
			// binary operators
			case p::bf_and:         print_bf_and(); break;
			case p::bf_or:          infix_nows("|"); break;
			case p::bf_xor:         infix_nows("+"); break;
			case p::bf_eq:             infix("="); break;
			case p::bf_neq:            infix("!="); break;
			case p::bf_less_equal:     infix("<="); break;
			case p::bf_nleq:           infix("!<="); break;
			case p::bf_greater:        infix(">"); break;
			case p::bf_ngreater:       infix("!>"); break;
			case p::bf_greater_equal:  infix(">="); break;
			case p::bf_ngeq:           infix("!>="); break;
			case p::bf_less:           infix("<"); break;
			case p::bf_nless:          infix("!<"); break;
			case p::ctn_neq:           infix("!="); break;
			case p::ctn_eq:            infix("=");  break;
			case p::ctn_greater_equal: infix(">="); break;
			case p::ctn_greater:       infix(">");  break;
			case p::ctn_less_equal:    infix("<="); break;
			case p::ctn_less:          infix("<");  break;
			case p::wff_and:        infix("&&"); break;
			case p::wff_or:         infix("||"); break;
			case p::wff_xor:        infix("^"); break;
			case p::wff_imply:      infix("->"); break;
			case p::wff_equiv:      infix("<->"); break;
			// ternary operators
			case p::bf_interval:    infix2("<=", "<="); break;
			case p::wff_conditional:infix2("?", ":"); break;
			// quantifiers
			case p::wff_all:
			case p::wff_ex:         quant(); break;
			// cli commands
			case p::cli:           sep(". "); break;
			case p::rel_memory:    prefix_nows("%-"); break;
			case p::abs_memory:    prefix_nows("%"); break;
			case p::quit_cmd:      stream << "quit"; break;
			case p::version_cmd:   stream << "version"; break;
			case p::clear_cmd:     stream << "clear"; break;
			case p::help_cmd:      prefix("help"); break;
			case p::file_cmd:      prefix("file"); break;
			case p::valid_cmd:     prefix("valid"); break;
			case p::sat_cmd:       prefix("sat"); break;
			case p::unsat_cmd:     prefix("unsat"); break;
			case p::solve_cmd:     prefix("solve"); break;
			case p::run_cmd:       prefix("run"); break;
			case p::normalize_cmd: prefix("normalize"); break;
			case p::inst_cmd:
			case p::subst_cmd:
				stream << (tss.n() == tau_parser::inst_cmd
						? "instantiate" : "substitute")
					<< " ", ppch(1) << " [", ppch(2)
					<< " / ", ppch(3) << "]";
				break;
			case p::dnf_cmd:       prefix("dnf"); break;
			case p::cnf_cmd:       prefix("cnf"); break;
			case p::anf_cmd:       prefix("anf"); break;
			case p::nnf_cmd:       prefix("nnf"); break;
			case p::pnf_cmd:       prefix("pnf"); break;
			case p::mnf_cmd:       prefix("mnf"); break;
			case p::snf_cmd:       prefix("snf"); break;
			case p::onf_cmd:       prefix("onf"); break;
			case p::def_print_cmd:
			case p::def_rr_cmd:    prefix("def"); break;
			case p::def_list_cmd:  stream << "def"; break;
			case p::history_list_cmd:
						stream << "history"; break;
			case p::history_print_cmd:
			case p::history_store_cmd:
							prefix("history"); break;
			case p::get_cmd:       prefix("get"); break;
			case p::set_cmd:       prefix("set"); break;
			case p::toggle_cmd:    prefix("toggle"); break;
			// just print terminals for these
			case p::in_var_name:
			case p::out_var_name:
			case p::chars:
			case p::capture:
			case p::sym:
			case p::ctnvar:
			case p::num:
			case p::integer:
			case p::source:
			case p::named_binding:
			case p::uninter_const_name:
			case p::option_name:
			case p::option_value:
				print_terminals(stream, n);
				break;
			// constants
			case p::uninterpreted_constant:
							wrap("<", ">"); break;
			// fixed point calculation fallback
			case p::fp_fallback: prefix(" fallback"); break;
			// simple symbols
			case p::first_sym: stream << "first"; break;
			case p::last_sym: stream << "last"; break;
			case p::bf_f:  stream << '0'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
			case p::bf_t:  stream << '1'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
			case p::wff_f: stream << 'F'; break;
			case p::wff_t: stream << 'T'; break;
			// for the rest skip value and just passthrough to child
			default: for (const auto& c : n->child)
					pp(stream, c, hl_path, depth, parent);
				break;
			}
			// indenting and breaklines
			if (pretty_printer_indenting && indented)
				depth--;
			// syntax highlighting color end
			if (pretty_printer_highlighting && hl_pop) {
				hl_path.pop_back();
				stream << TC.CLEAR();
				if (hl_path.size()) // restore the prev color
					stream << hl_colors.at(hl_path.back());
			}
		}
		else if (!tss.is_null()) stream << tss.t();
	} else stream << n->value;
	DBG(rdepth--;)
	return stream;
}
