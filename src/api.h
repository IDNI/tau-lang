// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__API_H__
#define __IDNI__TAU__API_H__

// Highly unstable. Contains bugs. Subject to change.

// TODO (LOW) add non string api for execution?
// TODO (HIGH) tests
// TODO (HIGH) error handling
// TODO (HIGH) documentation
// TODO (HIGH) decide which parsing get_* methods or other methods will go private if any
// TODO (MEDIUM) parsing with `bool simplify = true` argument

#include "interpreter.h"

namespace idni::tau_lang {

struct stream_at {
	std::string name;
	size_t time_point;

	auto operator<=>(const stream_at& other) const = default;
};

struct interpreter_options {
	input_streams_remap  input_remaps;
	output_streams_remap output_remaps;
};

// API functions are as static methods in a template so it is possible to
// alias it with user defined node containing their chosen boolean algebras pack
// using tau_api = api<node<BAs...>>; tau_api::get_interpreter(...);
template <NodeType node>
struct api {
	using tau = tree<node>;
	using optional_string = std::optional<std::string>;
	using severity_level = boost::log::trivial::severity_level;

	static void set_charvar(bool state);
	static void set_indenting(bool state);
	static void set_highlighting(bool state);
	static void set_severity(severity_level level);

	// Parsing
	// ------------------------------------------------------------
	static tref get_term(const std::string& term);         // bf
	static htref geth_term(const std::string& term);

	static tref get_formula(const std::string& formula);   // wff
	static htref geth_formula(const std::string& formula);

	// rec_relation { bf_ref { ... } ,  bf { ... } }
	static tref get_function_def(const std::string& function_def);
	static htref geth_function_def(const std::string& function_def);

	// rec_relation { wff_ref { ... } , wff { ... } }
	static tref get_predicate_def(const std::string& predicate_def);
	static htref geth_predicate_def(const std::string& predicate_def);

	// input_def, output_def
	static tref get_stream_def(const std::string& stream_def);
	static htref geth_stream_def(const std::string& stream_def);

	// spec
	static tref get_spec(const std::string& spec);
	static htref geth_spec(const std::string& spec);

	// rec_relation     private?
	static tref get_definition(const std::string& definition);
	static htref geth_definition(const std::string& definition);

	// spec, wff, bf    private?
	static tref get_spec_or_term(const std::string& expression);
	static htref geth_spec_or_term(const std::string& expression);

	// wff, bf          private?
	static tref get_formula_or_term(const std::string& expression);
	static htref geth_formula_or_term(const std::string& expression);

	// Querying
	// ------------------------------------------------------------
	static bool contains(tref expression, typename node::type nt); // private?
	static bool contains(htref expression, typename node::type nt); // private?

	static bool is_term(const std::string& expression);
	static bool is_term(tref expression);
	static bool is_term(htref expression);

	static bool is_formula(const std::string& expression);
	static bool is_formula(tref expression);
	static bool is_formula(htref expression);

	// Using definitions
	// ------------------------------------------------------------
	static optional_string apply_def(
		const std::string& def,
		const std::string& expression);
	static tref apply_def(tref def, tref expression);
	static htref apply_def(htref def, htref expression);

	static optional_string apply_defs(
		const std::set<std::string>& defs,
		const std::string& expression);
	static tref apply_defs(std::set<tref> defs, tref expression);
	static htref apply_defs(std::set<htref> defs, htref expression);

	static optional_string apply_all_defs(
		const std::string& expression);
	static tref apply_all_defs(tref expression);
	static htref apply_all_defs(htref expression);

	// Printing
	// ------------------------------------------------------------
	static std::ostream& print(std::ostream& os, tref expression);
	static std::ostream& print(std::ostream& os, htref expression);

	static std::string to_str(tref expression);
	static std::string to_str(htref expression);

	// Formula and term building
	// ------------------------------------------------------------
	// TODO

	// ------------------------------------------------------------
	// Formula manipulation
	// ------------------------------------------------------------

	// Substitution
	// ------------------------------------------------------------
	static optional_string substitute(
		const std::string& expression,
		const std::string& that,
		const std::string& with);
	static tref substitute(tref expression, tref that, tref with);
	static htref substitute(htref expression, htref that, htref with);

	static optional_string substitute(
		const std::string& expression,
		const std::map<std::string, std::string>& that_with);
	static tref substitute(tref expression, std::map<tref, tref> that_with);
	static htref substitute(
		htref expression,
		std::map<htref, htref> that_with);

	// Normal forms
	// ------------------------------------------------------------
	static optional_string boole_normal_form(
		const std::string& expression);
	static tref boole_normal_form(tref expression);
	static htref boole_normal_form(htref expression);

	static optional_string dnf(const std::string& expression);
	static tref dnf(tref expression);
	static htref dnf(htref expression);

	static optional_string cnf(const std::string& expression);
	static tref cnf(tref expression);
	static htref cnf(htref expression);

	static optional_string nnf(const std::string& expression);
	static tref nnf(tref expression);
	static htref nnf(htref expression);

	// Procedures
	// ------------------------------------------------------------
	// Applies local simplifications like 1 & 0 := 0
	// calls syntactic_formula_simplification
	static optional_string simplify(const std::string& expression);
	static tref simplify(tref expression);
	static htref simplify(htref expression);

	// Applies cheap non-local simplifications like symbolic clause contradiction
	static optional_string syntactic_term_simplification(
		const std::string& term);
	static tref syntactic_term_simplification(tref term);
	static htref syntactic_term_simplification(htref term);

	// Applies cheap non-local simplifications like symbolic clause contradiction and simple equality reasoning
	static optional_string syntactic_formula_simplification(
		const std::string& formula);
	static tref syntactic_formula_simplification(tref formula);
	static htref syntactic_formula_simplification(htref formula);

	static optional_string normalize_term(const std::string& term);
	static tref normalize_term(tref term);
	static htref normalize_term(htref term);

	static optional_string normalize_formula(const std::string& fm);
	static tref normalize_formula(tref fm);
	static htref normalize_formula(htref fm);

	static optional_string anti_prenex(const std::string& fm);
	static tref anti_prenex(tref fm);
	static htref anti_prenex(htref fm);

	static optional_string eliminate_quantifiers(const std::string& fm);
	static tref eliminate_quantifiers(tref fm);
	static htref eliminate_quantifiers(htref fm);

	static bool realizable(const std::string& spec);
	static bool realizable(tref spec);
	static bool realizable(htref spec);

	static bool unrealizable(const std::string& spec);
	static bool unrealizable(tref spec);
	static bool unrealizable(htref spec);

	static bool sat(const std::string& formula);
	static bool sat(tref formula);
	static bool sat(htref formula);

	static bool unsat(const std::string& formula);
	static bool unsat(tref formula);
	static bool unsat(htref formula);

	static bool valid(const std::string& formula);
	static bool valid(tref formula);
	static bool valid(htref formula);

	static bool valid_spec(const std::string& spec);
	static bool valid_spec(tref spec);
	static bool valid_spec(htref spec);

	// Solving
	// ------------------------------------------------------------
	// The resulting map maps variables to their solution
	static std::optional<std::map<std::string, std::string>> solve(
		const std::string& formula,
		solver_mode mode = solver_mode::general);
	static std::optional<subtree_map<node, tref>> solve(
		tref formula,
		solver_mode mode = solver_mode::general);
	static std::optional<std::map<htref, htref>> solve(
		htref formula,
		solver_mode mode = solver_mode::general);

	static std::optional<std::map<std::string, std::string>> lgrs(
		const std::string& equation);
	static std::optional<std::map<htref, htref>> lgrs(htref equation);
	static std::optional<subtree_map<node, tref>> lgrs(tref equation);

	// Execution
	// ------------------------------------------------------------
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& spec);
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& spec,
		interpreter_options& options);
	static std::optional<interpreter<node>> get_interpreter(
		tau_spec<node>& spec);
	static std::optional<interpreter<node>> get_interpreter(
		tau_spec<node>& spec,
		interpreter_options& options);
	// In order to get the inputs needed for the next step
	static std::vector<stream_at> get_inputs_for_step(interpreter<node>& i);
	// Returned map is mapping of output streams to assigned values
	static std::optional<std::map<stream_at, std::string>> step(
		interpreter<node>& i,
		std::map<stream_at, std::string> inputs);
	// Returned map is mapping of output streams to assigned values
	static std::optional<std::map<stream_at, std::string>> step(
		interpreter<node>& i);

private:
	static std::optional<rr<node>> get_nso_rr(tref expression);
};


} // namespace idni::tau_lang

#include "api.tmpl.h"
#include "api.tmpl.htref.h"
#include "api.tmpl.string.h"

#endif // __IDNI__TAU__API_H__