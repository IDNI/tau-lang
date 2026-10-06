set(TAU_BA_ID nlang)
set(TAU_BA_TYPE nlang_ba)
set(TAU_BA_HEADER boolean_algebras/nlang/nlang_ba.h)
set(TAU_BA_GRAMMAR parser/nlang.tgf)
set(TAU_BA_LINK_LIBS CURL::libcurl)
set(TAU_BA_REQUIRES_PACKAGES CURL)
set(TAU_BA_SOURCES
	boolean_algebras/nlang/nlang_ba.cpp
	boolean_algebras/nlang/nlang_llm.cpp
)
set(TAU_BA_TESTS
	tests/test_nlang_grammar.cpp
	tests/test_nlang_oracle.cpp
	tests/test_nlang_llm.cpp
)

# nlang's HTTP oracle links curl, which has no wasm port.
set(TAU_BA_UNSUPPORTED_TARGETS wasm32-emscripten)
set(TAU_BA_UNSUPPORTED_REASON_wasm32-emscripten "curl has no wasm port")
