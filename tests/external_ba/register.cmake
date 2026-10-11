# Registers the out-of-tree BA in tests/external_ba with the pack resolver.
# Pass to a configure as -DTAU_EXTERNAL_BAS=<abs path to this file>; the BA
# then joins the pack whenever "ext" is listed in TAU_BAS.

tau_register_ba(ext
	PATH   "${CMAKE_CURRENT_LIST_DIR}"
	HEADER ext_ba.h
	TYPE   ext_ba)
