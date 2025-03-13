// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "runtime.h"
#include "hooks.h"
#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool check_hook(const char* sample, const char* expected) {
	auto tau_sample = make_nso_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(sample, { .start = tau_parser::wff }).value();
	auto tau_expected = make_nso_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(expected, { .start = tau_parser::wff }).value();

	#ifdef DEBUG
	std::string str(sample);
	std::cout << "sample: " << str << " expected: " << tau_expected << " got: " << tau_sample << "\n";
	#endif // DEBUG

	return tau_sample == tau_expected;
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("wff operator hooks") {

	TEST_CASE("!") {
		CHECK( check_hook("!F", "T") );
		CHECK( check_hook("!T", "F") );

		CHECK( check_hook("!!(x = 0)", "(x = 0)") );
	}

	TEST_CASE("<->") {
		CHECK( check_hook("T<->T", "T") );
		CHECK( check_hook("T<->F", "F") );
		CHECK( check_hook("F<->T", "F") );
		CHECK( check_hook("F<->F", "T") );

		CHECK( check_hook("F<->(x = 0)", "!(x = 0)") );
		CHECK( check_hook("T<->(x = 0)", "(x = 0)") );
		CHECK( check_hook("(x = 0)<->F", "!(x = 0)") );
		CHECK( check_hook("(x = 0)<->T", "(x = 0)") );
		CHECK( check_hook("(x = 0)<->(x = 0)", "T") );
		CHECK( check_hook("!(x = 0)<->(x = 0)", "F") );
		CHECK( check_hook("(x = 0)<->!(x = 0)", "F") );
	}

	TEST_CASE("->") {
		CHECK( check_hook("T->T", "T") );
		CHECK( check_hook("T->F", "F") );
		CHECK( check_hook("F->T", "T") );
		CHECK( check_hook("F->F", "T") );

		CHECK( check_hook("F->(x = 0)", "T") );
		CHECK( check_hook("T->(x = 0)", "(x = 0)") );
		CHECK( check_hook("(x = 0)->F", "!(x = 0)") );
		CHECK( check_hook("(x = 0)->T", "T") );
		CHECK( check_hook("(x = 0)->(x = 0)", "T") );
	}

	TEST_CASE("<-") {
		CHECK( check_hook("T<-T", "T") );
		CHECK( check_hook("T<-F", "T") );
		CHECK( check_hook("F<-T", "F") );
		CHECK( check_hook("F<-F", "T") );

		CHECK( check_hook("F<-(x = 0)", "!(x = 0)") );
		CHECK( check_hook("T<-(x = 0)", "T") );
		CHECK( check_hook("(x = 0)<-F", "T") );
		CHECK( check_hook("(x = 0)<-T", "(x = 0)") );
		CHECK( check_hook("(x = 0)<-(x = 0)", "T") );
	}

	TEST_CASE("||") {
		CHECK( check_hook("T||T", "T") );
		CHECK( check_hook("T||F", "T") );
		CHECK( check_hook("F||T", "T") );
		CHECK( check_hook("F||F", "F") );

		CHECK( check_hook("T||(x = 0)", "T") );
		CHECK( check_hook("(x = 0)||T", "T") );
		CHECK( check_hook("F||(x = 0)", "(x = 0)") );
		CHECK( check_hook("(x = 0)||F", "(x = 0)") );
		CHECK( check_hook("(x = 0)||(x = 0)", "(x = 0)") );
		CHECK( check_hook("!(x = 0)||(x = 0)", "T") );
		CHECK( check_hook("(x = 0)||!(x = 0)", "T") );

	}

	TEST_CASE("&&") {
		CHECK( check_hook("T&&T", "T") );
		CHECK( check_hook("T&&F", "F") );
		CHECK( check_hook("F&&T", "F") );
		CHECK( check_hook("F&&F", "F") );

		CHECK( check_hook("T&&(x = 0)", "(x = 0)") );
		CHECK( check_hook("(x = 0)&&T", "(x = 0)") );
		CHECK( check_hook("F&&(x = 0)", "F") );
		CHECK( check_hook("(x = 0)&&F", "F") );
		CHECK( check_hook("(x = 0)&&(x = 0)", "(x = 0)") );
		CHECK( check_hook("!(x = 0)&&(x = 0)", "F") );
		CHECK( check_hook("(x = 0)&&!(x = 0)", "F") );
	}

	TEST_CASE("^") {
		CHECK( check_hook("T^T", "F") );
		CHECK( check_hook("T^F", "T") );
		CHECK( check_hook("F^T", "T") );
		CHECK( check_hook("F^F", "F") );

		CHECK( check_hook("(x = 0)^F", "(x = 0)") );
		CHECK( check_hook("F^(x = 0)", "(x = 0)") );
		CHECK( check_hook("(x = 0)^(x = 0)", "F") );
		CHECK( check_hook("!(x = 0)^(x = 0)", "T") );
		CHECK( check_hook("(x = 0)^!(x = 0)", "T") );
		CHECK( check_hook("(x = 0)^T", "!(x = 0)") );
		CHECK( check_hook("T^(x = 0)", "!(x = 0)") );

	}

	TEST_CASE("?") {
		CHECK( check_hook("T?T:F", "T") );
		CHECK( check_hook("T?T:T", "T") );
		CHECK( check_hook("T?F:T", "F") );
		CHECK( check_hook("T?F:F", "F") );
		CHECK( check_hook("F?T:F", "F") );
		CHECK( check_hook("F?F:F", "F") );
		CHECK( check_hook("F?F:T", "T") );
		CHECK( check_hook("F?T:T", "T") );

		CHECK( check_hook("F?(x=0):(y=0)", "(y=0)") );
		CHECK( check_hook("T?(x=0):(y=0)", "(x=0)") );
		CHECK( check_hook("F?(x=0):(x=0)", "(x=0)") );
		CHECK( check_hook("T?(x=0):(x=0)", "(x=0)") );
	}
}

TEST_SUITE("wff comparator hooks") {

	TEST_CASE("=") {
		CHECK( check_hook("1=1", "T") );
		CHECK( check_hook("0=0", "T") );
		CHECK( check_hook("1=0", "F") );
		CHECK( check_hook("0=1", "F") );

		CHECK( check_hook("1:sbf=1:sbf", "T") );
		CHECK( check_hook("0:sbf=0:sbf", "T") );
		CHECK( check_hook("1:sbf=0:sbf", "F") );
		CHECK( check_hook("0:sbf=1:sbf", "F") );

		CHECK( check_hook("1:sbf=1", "T") );
		CHECK( check_hook("0:sbf=0", "T") );
		CHECK( check_hook("1:sbf=0", "F") );
		CHECK( check_hook("0:sbf=1", "F") );

		CHECK( check_hook("1=1:sbf", "T") );
		CHECK( check_hook("0=0:sbf", "T") );
		CHECK( check_hook("1=0:sbf", "F") );
		CHECK( check_hook("0=1:sbf", "F") );

		CHECK( check_hook("1:tau=1:tau", "T") );
		CHECK( check_hook("0:tau=0:tau", "T") );
		CHECK( check_hook("1:tau=0:tau", "F") );
		CHECK( check_hook("0:tau=1:tau", "F") );

		CHECK( check_hook("1:tau=1", "T") );
		CHECK( check_hook("0:tau=0", "T") );
		CHECK( check_hook("1:tau=0", "F") );
		CHECK( check_hook("0:tau=1", "F") );

		CHECK( check_hook("1=1:tau", "T") );
		CHECK( check_hook("0=0:tau", "T") );
		CHECK( check_hook("1=0:tau", "F") );
		CHECK( check_hook("0=1:tau", "F") );

		CHECK( !check_hook("1:tau=1:sbf", "T") );
		CHECK( !check_hook("0:tau=0:sbf", "T") );
		CHECK( !check_hook("1:tau=0:sbf", "F") );
		CHECK( !check_hook("0:tau=1:sbf", "F") );

		CHECK( !check_hook("1:sbf=1:tau", "T") );
		CHECK( !check_hook("0:sbf=0:tau", "T") );
		CHECK( !check_hook("1:sbf=0:tau", "F") );
		CHECK( !check_hook("0:sbf=1:tau", "F") );

		CHECK( check_hook("{T}=0", "F") );
		CHECK( check_hook("{F}=0", "T") );
		CHECK( check_hook("{T}:tau=0", "F") );
		CHECK( check_hook("{F}:tau=0", "T") );
		CHECK( check_hook("{1}:sbf=0", "F") );
		CHECK( check_hook("{0}:sbf=0", "T") );
		CHECK( check_hook("x = y", "x + y = 0") );
	}

	TEST_CASE("!=") {
		CHECK( check_hook("1!=1", "F") );
		CHECK( check_hook("0!=0", "F") );
		CHECK( check_hook("1!=0", "T") );
		CHECK( check_hook("0!=1", "T") );

		CHECK( check_hook("1:sbf!=1:sbf", "F") );
		CHECK( check_hook("0:sbf!=0:sbf", "F") );
		CHECK( check_hook("1:sbf!=0:sbf", "T") );
		CHECK( check_hook("0:sbf!=1:sbf", "T") );

		CHECK( check_hook("1:sbf!=1", "F") );
		CHECK( check_hook("0:sbf!=0", "F") );
		CHECK( check_hook("1:sbf!=0", "T") );
		CHECK( check_hook("0:sbf!=1", "T") );

		CHECK( check_hook("1!=1:sbf", "F") );
		CHECK( check_hook("0!=0:sbf", "F") );
		CHECK( check_hook("1!=0:sbf", "T") );
		CHECK( check_hook("0!=1:sbf", "T") );

		CHECK( check_hook("1:tau!=1:tau", "F") );
		CHECK( check_hook("0:tau!=0:tau", "F") );
		CHECK( check_hook("1:tau!=0:tau", "T") );
		CHECK( check_hook("0:tau!=1:tau", "T") );

		CHECK( check_hook("1:tau!=1", "F") );
		CHECK( check_hook("0:tau!=0", "F") );
		CHECK( check_hook("1:tau!=0", "T") );
		CHECK( check_hook("0:tau!=1", "T") );

		CHECK( check_hook("1!=1:tau", "F") );
		CHECK( check_hook("0!=0:tau", "F") );
		CHECK( check_hook("1!=0:tau", "T") );
		CHECK( check_hook("0!=1:tau", "T") );

		CHECK( !check_hook("1:tau!=1:sbf", "F") );
		CHECK( !check_hook("0:tau!=0:sbf", "F") );
		CHECK( !check_hook("1:tau!=0:sbf", "T") );
		CHECK( !check_hook("0:tau!=1:sbf", "T") );

		CHECK( !check_hook("1:sbf!=1:tau", "F") );
		CHECK( !check_hook("0:sbf!=0:tau", "F") );
		CHECK( !check_hook("1:sbf!=0:tau", "T") );
		CHECK( !check_hook("0:sbf!=1:tau", "T") );

		CHECK( check_hook("{T}!=0", "T") );
		CHECK( check_hook("{F}!=0", "F") );
		CHECK( check_hook("{T}:tau!=0", "T") );
		CHECK( check_hook("{F}:tau!=0", "F") );
		CHECK( check_hook("{1}:sbf!=0", "T") );
		CHECK( check_hook("{0}:sbf!=0", "F") );
		CHECK( check_hook("x != y", "x + y != 0") );
	}

	TEST_CASE("<") {
		CHECK( check_hook("1<1", "F") );
		CHECK( check_hook("0<0", "F") );
		CHECK( check_hook("1<0", "F") );
		CHECK( check_hook("0<1", "T") );

		CHECK( check_hook("1:sbf<1:sbf", "F") );
		CHECK( check_hook("0:sbf<0:sbf", "F") );
		CHECK( check_hook("1:sbf<0:sbf", "F") );
		CHECK( check_hook("0:sbf<1:sbf", "T") );

		CHECK( check_hook("1:sbf<1", "F") );
		CHECK( check_hook("0:sbf<0", "F") );
		CHECK( check_hook("1:sbf<0", "F") );
		CHECK( check_hook("0:sbf<1", "T") );

		CHECK( check_hook("1<1:sbf", "F") );
		CHECK( check_hook("0<0:sbf", "F") );
		CHECK( check_hook("1<0:sbf", "F") );
		CHECK( check_hook("0<1:sbf", "T") );

		CHECK( check_hook("1:tau<1:tau", "F") );
		CHECK( check_hook("0:tau<0:tau", "F") );
		CHECK( check_hook("1:tau<0:tau", "F") );
		CHECK( check_hook("0:tau<1:tau", "T") );

		CHECK( check_hook("1:tau<1", "F") );
		CHECK( check_hook("0:tau<0", "F") );
		CHECK( check_hook("1:tau<0", "F") );
		CHECK( check_hook("0:tau<1", "T") );

		CHECK( check_hook("1<1:tau", "F") );
		CHECK( check_hook("0<0:tau", "F") );
		CHECK( check_hook("1<0:tau", "F") );
		CHECK( check_hook("0<1:tau", "T") );

		CHECK( !check_hook("1:tau<1:sbf", "F") );
		CHECK( !check_hook("0:tau<0:sbf", "F") );
		CHECK( !check_hook("1:tau<0:sbf", "F") );
		CHECK( !check_hook("0:tau<1:sbf", "T") );

		CHECK( !check_hook("1:sbf<1:tau", "F") );
		CHECK( !check_hook("0:sbf<0:tau", "F") );
		CHECK( !check_hook("1:sbf<0:tau", "F") );
		CHECK( !check_hook("0:sbf<1:tau", "T") );

		CHECK( check_hook("X<0", "F") );
		CHECK( check_hook("X<1", "X'!=0") );
	}

	TEST_CASE("!<"){
		CHECK( check_hook("1!<1", "T") );
		CHECK( check_hook("0!<0", "T") );
		CHECK( check_hook("1!<0", "T") );
		CHECK( check_hook("0!<1", "F") );

		CHECK( check_hook("1:sbf!<1:sbf", "T") );
		CHECK( check_hook("0:sbf!<0:sbf", "T") );
		CHECK( check_hook("1:sbf!<0:sbf", "T") );
		CHECK( check_hook("0:sbf!<1:sbf", "F") );

		CHECK( check_hook("1:sbf!<1", "T") );
		CHECK( check_hook("0:sbf!<0", "T") );
		CHECK( check_hook("1:sbf!<0", "T") );
		CHECK( check_hook("0:sbf!<1", "F") );

		CHECK( check_hook("1!<1:sbf", "T") );
		CHECK( check_hook("0!<0:sbf", "T") );
		CHECK( check_hook("1!<0:sbf", "T") );
		CHECK( check_hook("0!<1:sbf", "F") );

		CHECK( check_hook("1:tau!<1:tau", "T") );
		CHECK( check_hook("0:tau!<0:tau", "T") );
		CHECK( check_hook("1:tau!<0:tau", "T") );
		CHECK( check_hook("0:tau!<1:tau", "F") );

		CHECK( check_hook("1:tau!<1", "T") );
		CHECK( check_hook("0:tau!<0", "T") );
		CHECK( check_hook("1:tau!<0", "T") );
		CHECK( check_hook("0:tau!<1", "F") );

		CHECK( check_hook("1!<1:tau", "T") );
		CHECK( check_hook("0!<0:tau", "T") );
		CHECK( check_hook("1!<0:tau", "T") );
		CHECK( check_hook("0!<1:tau", "F") );

		CHECK( !check_hook("1:tau!<1:sbf", "T") );
		CHECK( !check_hook("0:tau!<0:sbf", "T") );
		CHECK( !check_hook("1:tau!<0:sbf", "T") );
		CHECK( !check_hook("0:tau!<1:sbf", "F") );

		CHECK( !check_hook("1:sbf!<1:tau", "T") );
		CHECK( !check_hook("0:sbf!<0:tau", "T") );
		CHECK( !check_hook("1:sbf!<0:tau", "T") );
		CHECK( !check_hook("0:sbf!<1:tau", "F") );

		CHECK( check_hook("X!<0", "T") );
		CHECK( check_hook("X!<1", "X'=0") );
	}

	TEST_CASE(">"){
		CHECK( check_hook("1>1", "F") );
		CHECK( check_hook("0>0", "F") );
		CHECK( check_hook("1>0", "T") );
		CHECK( check_hook("0>1", "F") );

		CHECK( check_hook("1:sbf>1:sbf", "F") );
		CHECK( check_hook("0:sbf>0:sbf", "F") );
		CHECK( check_hook("1:sbf>0:sbf", "T") );
		CHECK( check_hook("0:sbf>1:sbf", "F") );

		CHECK( check_hook("1:sbf>1", "F") );
		CHECK( check_hook("0:sbf>0", "F") );
		CHECK( check_hook("1:sbf>0", "T") );
		CHECK( check_hook("0:sbf>1", "F") );

		CHECK( check_hook("1>1:sbf", "F") );
		CHECK( check_hook("0>0:sbf", "F") );
		CHECK( check_hook("1>0:sbf", "T") );
		CHECK( check_hook("0>1:sbf", "F") );

		CHECK( check_hook("1:tau>1:tau", "F") );
		CHECK( check_hook("0:tau>0:tau", "F") );
		CHECK( check_hook("1:tau>0:tau", "T") );
		CHECK( check_hook("0:tau>1:tau", "F") );

		CHECK( check_hook("1:tau>1", "F") );
		CHECK( check_hook("0:tau>0", "F") );
		CHECK( check_hook("1:tau>0", "T") );
		CHECK( check_hook("0:tau>1", "F") );

		CHECK( check_hook("1>1:tau", "F") );
		CHECK( check_hook("0>0:tau", "F") );
		CHECK( check_hook("1>0:tau", "T") );
		CHECK( check_hook("0>1:tau", "F") );

		CHECK( !check_hook("1:tau>1:sbf", "F") );
		CHECK( !check_hook("0:tau>0:sbf", "F") );
		CHECK( !check_hook("1:tau>0:sbf", "T") );
		CHECK( !check_hook("0:tau>1:sbf", "F") );

		CHECK( !check_hook("1:sbf>1:tau", "F") );
		CHECK( !check_hook("0:sbf>0:tau", "F") );
		CHECK( !check_hook("1:sbf>0:tau", "T") );
		CHECK( !check_hook("0:sbf>1:tau", "F") );

		CHECK( check_hook("X>1", "F") );
		CHECK( check_hook("0>X", "F") );
	}

	TEST_CASE("!>"){
		CHECK( check_hook("1!>1", "T") );
		CHECK( check_hook("0!>0", "T") );
		CHECK( check_hook("1!>0", "F") );
		CHECK( check_hook("0!>1", "T") );

		CHECK( check_hook("1:sbf!>1:sbf", "T") );
		CHECK( check_hook("0:sbf!>0:sbf", "T") );
		CHECK( check_hook("1:sbf!>0:sbf", "F") );
		CHECK( check_hook("0:sbf!>1:sbf", "T") );

		CHECK( check_hook("1:sbf!>1", "T") );
		CHECK( check_hook("0:sbf!>0", "T") );
		CHECK( check_hook("1:sbf!>0", "F") );
		CHECK( check_hook("0:sbf!>1", "T") );

		CHECK( check_hook("1!>1:sbf", "T") );
		CHECK( check_hook("0!>0:sbf", "T") );
		CHECK( check_hook("1!>0:sbf", "F") );
		CHECK( check_hook("0!>1:sbf", "T") );

		CHECK( check_hook("1:tau!>1:tau", "T") );
		CHECK( check_hook("0:tau!>0:tau", "T") );
		CHECK( check_hook("1:tau!>0:tau", "F") );
		CHECK( check_hook("0:tau!>1:tau", "T") );

		CHECK( check_hook("1:tau!>1", "T") );
		CHECK( check_hook("0:tau!>0", "T") );
		CHECK( check_hook("1:tau!>0", "F") );
		CHECK( check_hook("0:tau!>1", "T") );

		CHECK( check_hook("1!>1:tau", "T") );
		CHECK( check_hook("0!>0:tau", "T") );
		CHECK( check_hook("1!>0:tau", "F") );
		CHECK( check_hook("0!>1:tau", "T") );

		CHECK( !check_hook("1:tau!>1:sbf", "T") );
		CHECK( !check_hook("0:tau!>0:sbf", "T") );
		CHECK( !check_hook("1:tau!>0:sbf", "F") );
		CHECK( !check_hook("0:tau!>1:sbf", "T") );

		CHECK( !check_hook("1:sbf!>1:tau", "T") );
		CHECK( !check_hook("0:sbf!>0:tau", "T") );
		CHECK( !check_hook("1:sbf!>0:tau", "F") );
		CHECK( !check_hook("0:sbf!>1:tau", "T") );

		CHECK( check_hook("X!>1", "T") );
		CHECK( check_hook("0!>X", "T") );
	}

	TEST_CASE("<="){
		CHECK( check_hook("1<=1", "T") );
		CHECK( check_hook("0<=0", "T") );
		CHECK( check_hook("1<=0", "F") );
		CHECK( check_hook("0<=1", "T") );

		CHECK( check_hook("1:sbf<=1:sbf", "T") );
		CHECK( check_hook("0:sbf<=0:sbf", "T") );
		CHECK( check_hook("1:sbf<=0:sbf", "F") );
		CHECK( check_hook("0:sbf<=1:sbf", "T") );

		CHECK( check_hook("1:sbf<=1", "T") );
		CHECK( check_hook("0:sbf<=0", "T") );
		CHECK( check_hook("1:sbf<=0", "F") );
		CHECK( check_hook("0:sbf<=1", "T") );

		CHECK( check_hook("1<=1:sbf", "T") );
		CHECK( check_hook("0<=0:sbf", "T") );
		CHECK( check_hook("1<=0:sbf", "F") );
		CHECK( check_hook("0<=1:sbf", "T") );

		CHECK( check_hook("1:tau<=1:tau", "T") );
		CHECK( check_hook("0:tau<=0:tau", "T") );
		CHECK( check_hook("1:tau<=0:tau", "F") );
		CHECK( check_hook("0:tau<=1:tau", "T") );

		CHECK( check_hook("1:tau<=1", "T") );
		CHECK( check_hook("0:tau<=0", "T") );
		CHECK( check_hook("1:tau<=0", "F") );
		CHECK( check_hook("0:tau<=1", "T") );

		CHECK( check_hook("1<=1:tau", "T") );
		CHECK( check_hook("0<=0:tau", "T") );
		CHECK( check_hook("1<=0:tau", "F") );
		CHECK( check_hook("0<=1:tau", "T") );

		CHECK( !check_hook("1:tau<=1:sbf", "T") );
		CHECK( !check_hook("0:tau<=0:sbf", "T") );
		CHECK( !check_hook("1:tau<=0:sbf", "F") );
		CHECK( !check_hook("0:tau<=1:sbf", "T") );

		CHECK( !check_hook("1:sbf<=1:tau", "T") );
		CHECK( !check_hook("0:sbf<=0:tau", "T") );
		CHECK( !check_hook("1:sbf<=0:tau", "F") );
		CHECK( !check_hook("0:sbf<=1:tau", "T") );

		CHECK( check_hook("X<=1", "T") );
	}

	TEST_CASE("!<="){
		CHECK( check_hook("1!<=1", "F") );
		CHECK( check_hook("0!<=0", "F") );
		CHECK( check_hook("1!<=0", "T") );
		CHECK( check_hook("0!<=1", "F") );

		CHECK( check_hook("1:sbf!<=1:sbf", "F") );
		CHECK( check_hook("0:sbf!<=0:sbf", "F") );
		CHECK( check_hook("1:sbf!<=0:sbf", "T") );
		CHECK( check_hook("0:sbf!<=1:sbf", "F") );

		CHECK( check_hook("1:sbf!<=1", "F") );
		CHECK( check_hook("0:sbf!<=0", "F") );
		CHECK( check_hook("1:sbf!<=0", "T") );
		CHECK( check_hook("0:sbf!<=1", "F") );

		CHECK( check_hook("1!<=1:sbf", "F") );
		CHECK( check_hook("0!<=0:sbf", "F") );
		CHECK( check_hook("1!<=0:sbf", "T") );
		CHECK( check_hook("0!<=1:sbf", "F") );

		CHECK( check_hook("1:tau!<=1:tau", "F") );
		CHECK( check_hook("0:tau!<=0:tau", "F") );
		CHECK( check_hook("1:tau!<=0:tau", "T") );
		CHECK( check_hook("0:tau!<=1:tau", "F") );

		CHECK( check_hook("1:tau!<=1", "F") );
		CHECK( check_hook("0:tau!<=0", "F") );
		CHECK( check_hook("1:tau!<=0", "T") );
		CHECK( check_hook("0:tau!<=1", "F") );

		CHECK( check_hook("1!<=1:tau", "F") );
		CHECK( check_hook("0!<=0:tau", "F") );
		CHECK( check_hook("1!<=0:tau", "T") );
		CHECK( check_hook("0!<=1:tau", "F") );

		CHECK( !check_hook("1:tau!<=1:sbf", "F") );
		CHECK( !check_hook("0:tau!<=0:sbf", "F") );
		CHECK( !check_hook("1:tau!<=0:sbf", "T") );
		CHECK( !check_hook("0:tau!<=1:sbf", "F") );

		CHECK( !check_hook("1:sbf!<=1:tau", "F") );
		CHECK( !check_hook("0:sbf!<=0:tau", "F") );
		CHECK( !check_hook("1:sbf!<=0:tau", "T") );
		CHECK( !check_hook("0:sbf!<=1:tau", "F") );

		CHECK( check_hook("X!<=1", "F") );
	}

	TEST_CASE(">="){
		CHECK( check_hook("1>=1", "T") );
		CHECK( check_hook("0>=0", "T") );
		CHECK( check_hook("1>=0", "T") );
		CHECK( check_hook("0>=1", "F") );

		CHECK( check_hook("1:sbf>=1:sbf", "T") );
		CHECK( check_hook("0:sbf>=0:sbf", "T") );
		CHECK( check_hook("1:sbf>=0:sbf", "T") );
		CHECK( check_hook("0:sbf>=1:sbf", "F") );

		CHECK( check_hook("1:sbf>=1", "T") );
		CHECK( check_hook("0:sbf>=0", "T") );
		CHECK( check_hook("1:sbf>=0", "T") );
		CHECK( check_hook("0:sbf>=1", "F") );

		CHECK( check_hook("1>=1:sbf", "T") );
		CHECK( check_hook("0>=0:sbf", "T") );
		CHECK( check_hook("1>=0:sbf", "T") );
		CHECK( check_hook("0>=1:sbf", "F") );

		CHECK( check_hook("1:tau>=1:tau", "T") );
		CHECK( check_hook("0:tau>=0:tau", "T") );
		CHECK( check_hook("1:tau>=0:tau", "T") );
		CHECK( check_hook("0:tau>=1:tau", "F") );

		CHECK( check_hook("1:tau>=1", "T") );
		CHECK( check_hook("0:tau>=0", "T") );
		CHECK( check_hook("1:tau>=0", "T") );
		CHECK( check_hook("0:tau>=1", "F") );

		CHECK( check_hook("1>=1:tau", "T") );
		CHECK( check_hook("0>=0:tau", "T") );
		CHECK( check_hook("1>=0:tau", "T") );
		CHECK( check_hook("0>=1:tau", "F") );

		CHECK( !check_hook("1:tau>=1:sbf", "T") );
		CHECK( !check_hook("0:tau>=0:sbf", "T") );
		CHECK( !check_hook("1:tau>=0:sbf", "T") );
		CHECK( !check_hook("0:tau>=1:sbf", "F") );

		CHECK( !check_hook("1:sbf>=1:tau", "T") );
		CHECK( !check_hook("0:sbf>=0:tau", "T") );
		CHECK( !check_hook("1:sbf>=0:tau", "T") );
		CHECK( !check_hook("0:sbf>=1:tau", "F") );

		CHECK( check_hook("X>=0", "T") );
		CHECK( check_hook("1>=X", "T") );
	}

	TEST_CASE("!>="){
		CHECK( check_hook("1!>=1", "F") );
		CHECK( check_hook("0!>=0", "F") );
		CHECK( check_hook("1!>=0", "F") );
		CHECK( check_hook("0!>=1", "T") );

		CHECK( check_hook("1:sbf!>=1:sbf", "F") );
		CHECK( check_hook("0:sbf!>=0:sbf", "F") );
		CHECK( check_hook("1:sbf!>=0:sbf", "F") );
		CHECK( check_hook("0:sbf!>=1:sbf", "T") );

		CHECK( check_hook("1:sbf!>=1", "F") );
		CHECK( check_hook("0:sbf!>=0", "F") );
		CHECK( check_hook("1:sbf!>=0", "F") );
		CHECK( check_hook("0:sbf!>=1", "T") );

		CHECK( check_hook("1!>=1:sbf", "F") );
		CHECK( check_hook("0!>=0:sbf", "F") );
		CHECK( check_hook("1!>=0:sbf", "F") );
		CHECK( check_hook("0!>=1:sbf", "T") );

		CHECK( check_hook("1:tau!>=1:tau", "F") );
		CHECK( check_hook("0:tau!>=0:tau", "F") );
		CHECK( check_hook("1:tau!>=0:tau", "F") );
		CHECK( check_hook("0:tau!>=1:tau", "T") );

		CHECK( check_hook("1:tau!>=1", "F") );
		CHECK( check_hook("0:tau!>=0", "F") );
		CHECK( check_hook("1:tau!>=0", "F") );
		CHECK( check_hook("0:tau!>=1", "T") );

		CHECK( check_hook("1!>=1:tau", "F") );
		CHECK( check_hook("0!>=0:tau", "F") );
		CHECK( check_hook("1!>=0:tau", "F") );
		CHECK( check_hook("0!>=1:tau", "T") );

		CHECK( !check_hook("1:tau!>=1:sbf", "F") );
		CHECK( !check_hook("0:tau!>=0:sbf", "F") );
		CHECK( !check_hook("1:tau!>=0:sbf", "F") );
		CHECK( !check_hook("0:tau!>=1:sbf", "T") );

		CHECK( !check_hook("1:sbf!>=1:tau", "F") );
		CHECK( !check_hook("0:sbf!>=0:tau", "F") );
		CHECK( !check_hook("1:sbf!>=0:tau", "F") );
		CHECK( !check_hook("0:sbf!>=1:tau", "T") );

		CHECK( check_hook("X!>=0", "F") );
		CHECK( check_hook("1!>=X", "F") );
	}
}
