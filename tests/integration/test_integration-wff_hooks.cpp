// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

bool check_hook(const char* sample, const char* expected) {
	tref tau_sample   = tau::get(sample, parse_wff());
	tref tau_expected = tau::get(expected, parse_wff());

#ifdef DEBUG
	using node = node_t;
	std::cout << "sample: " << string(sample) << " expected: ";
	if (tau_expected == 0) cout << "nullptr";
	else cout << TAU_DUMP_TO_STR(tau_expected);
	cout << "\n\tgot: ";
	if (tau_sample == 0) cout << "nullptr";
	else cout << TAU_DUMP_TO_STR(tau_sample);
	cout << "\n";
#endif // DEBUG

	return tau::subtree_equals(tau_sample, tau_expected);
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
