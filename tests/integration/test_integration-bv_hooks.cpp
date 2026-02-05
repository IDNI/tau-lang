// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}


// TODO (HIGH) make test cases for each operator, for example:
// 0+1 = x1 :bv && 1+1 = x2 :bv && 1+0 = x3 :bv &&  1+{8} = x4 :bv &&  {8}+1 = x5 :bv &&  {8} + {8} = x6:bv
// 0-1 = x1 :bv && 1-1 = x2 :bv && 1-0 = x3 :bv &&  1-{8} = x4 :bv &&  {8}-1 = x5 :bv &&  {8} - {8} = x6:bv
// 0*1 = x1 :bv && 1*1 = x2 :bv && 1*0 = x3 :bv &&  1*{8} = x4 :bv &&  {8}*1 = x5 :bv &&  {8} * {8} = x6:bv
// 0/1 = x1 :bv && 1/1 = x2 :bv && 1/0 = x3 :bv &&  1/{8} = x4 :bv &&  {8}/1 = x5 :bv &&  {8} / {8} = x6:bv
// 0%1 = x1 :bv && 1%1 = x2 :bv && 1%0 = x3 :bv &&  1%{8} = x4 :bv &&  {8}%1 = x5 :bv &&  {8} % {8} = x6:bv
// 0>>1 = x1 :bv && 1>>1 = x2 :bv && 1>>0 = x3 :bv &&  1>>{8} = x4 :bv &&  {8}>>1 = x5 :bv &&  {8} >> {8} = x6:bv
// 0<<1 = x1 :bv && 1<<1 = x2 :bv && 1<<0 = x3 :bv &&  1<<{8} = x4 :bv &&  {8}<<1 = x5 :bv &&  {8} << {8} = x6:bv

TEST_SUITE("bv operator hooks") {



}
