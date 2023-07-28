// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __NORMALIZER2_H__
#define __NORMALIZER2_H__

#include <string>
#include <optional>

#include "rewriting.h"
#include "program.h"



namespace idni::tau {

// execute one step of the program
template<typename... BAs>
program<BAs...> program_step(const program<BAs...>& p, const library<BAs...>& l) {
	auto main = p.main;
	auto main_after_prog = apply(p.rec_relations, main);
	// TODO sys rules should be applied while possible
	auto main_after_sys = apply(l.system, main_after_prog);
	return program<BAs...>(main_after_sys, p.rec_relations);
}

// tau system library, used to define the tau system of rewriting rules
static constexpr char* system = 
	// distributivity
	"(??X bf_or ??Y) bf_and ??Z-> (??X bf_and ??Y) bf_or (??X bf_and ??Z))."
	"??X bf_and (??Y bf_or ??Z)-> (??X bf_and ??Y) bf_or (??X bf_and ??Z))."
	// push negations inwards
	"bf_neg (bf_neg ??X)-> ??X."
	"bf_neg (??X bf_and ??Y)-> (bf_neg ??X) bf_or (bf_neg ??Y)."
	"bf_neg (??X bf_or ??Y)-> (bf_neg ??X) bf_and (bf_neg ??Y)."
	"bf_neg (bf_all ??X (??Y))-> bf_ex ??X ( bf_neg (??Y))."
	// ...
	;

template<typename... BAs>
program<BAs...> normalizer(std::string source, bindings<BAs...> bs) {
	auto prog_source = make_tau_source(source);
	auto sys_source = make_tau_source(system);
	auto prog = make_program(prog_source, bs);
	auto lib = make_library<BAs...>(sys_source);
	auto nprog = program_step(prog, lib);
	while (prog.main != nprog.main) {
		prog = nprog;
		nprog = program_step(prog, lib);
	}
	return prog;
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
