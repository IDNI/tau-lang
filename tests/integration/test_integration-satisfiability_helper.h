// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

tref create_spec(const char* spec) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(spec));
	return nso_rr.value().main->get();
}

