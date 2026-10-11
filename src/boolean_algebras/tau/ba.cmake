# Reserved wrapper id: no TAU_BA_TYPE. When "tau" is in TAU_BAS, the resolver
# prepends tau_ba<base...> to the node<> pack (see cmake/tau_bas.cmake).
set(TAU_BA_ID tau)
set(TAU_BA_HEADER boolean_algebras/tau/tau_ba.h)
set(TAU_BA_TESTS
	tests/test_integration-solver-tau.cpp
)
