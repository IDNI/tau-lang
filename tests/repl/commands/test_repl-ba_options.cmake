include(add_repl_test)

# A name no BA declares: an unknown algebra and an unknown option of a known
# algebra each have their own message (repl_evaluator.tmpl.h
# resolve_ba_option). A case naming an algebra outside the configured pack
# is skipped by add_repl_test itself.
add_repl_test(ba_options-no_such_family "set nope-blasting on"
	"No BA named in this pack.*name=nope" NO_FAIL_REGEX)
add_repl_test(ba_options-no_such_option "set bv-nosuch on"
	"BA has no option.*name=bv.*value=nosuch" NO_FAIL_REGEX)
add_repl_test(ba_options-found "set bv-blasting off. get bv-blasting"
	"bv-blasting: off")

# GitHub #124: the definitional elimination and its caps are bv-declared.
add_repl_test(ba_options-defelim_flag "get bv-definitional-elimination"
	"bv-definitional-elimination: on")
add_repl_test(ba_options-defelim_flag_off "set bv-definitional-elimination off. get bv-definitional-elimination"
	"bv-definitional-elimination: off")
add_repl_test(ba_options-defelim_count "set bv-defelim-max-atoms 5. get bv-defelim-max-atoms"
	"bv-defelim-max-atoms: *5")
add_repl_test(ba_options-defelim_rounds_unlimited "set bv-defelim-max-rounds 0. get bv-defelim-max-rounds"
	"bv-defelim-max-rounds: *unlimited")

# The verdict memo is keyed on the formula, so changing a BA-declared option
# between two queries of the same spec must drop it: the second `sat` is
# decided again (its temporal normalization reports its fixpoint again)
# instead of being answered from the first.
set(_defelim_spec "always (!(((o2[t-1]:bv[2] = o2[t]) -> (o2[t-2] = o1[t-2]:bv[2]))) && ((o1[t] = o2[t-2]) <-> (o1[t-2] != o2[t])))")
add_repl_test(ba_options-change_drops_verdict_memo
	"sat ${_defelim_spec}. set bv-definitional-elimination off. sat ${_defelim_spec}"
	"reached fixpoint.*: F.*reached fixpoint.*: F")

# nlang's LLM options. The environment writes each one at start, so every
# case blanks the text variables a developer's shell may export. `get` shows
# the option as set; nlang_llm.cpp resolves an empty one when it asks.
set(_llm_env TAU_NLANG_PROVIDER= TAU_NLANG_ENDPOINT= TAU_NLANG_MODEL=
	TAU_NLANG_API_KEY= OPENAI_API_KEY= ANTHROPIC_API_KEY= TAU_NLANG_EFFORT=)
add_repl_test(ba_options-nlang_provider_default "get nlang-provider"
	"nlang-provider: \\(none\\)" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_provider_set "set nlang-provider anthropic"
	"nlang-provider: anthropic" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_provider_bad_word "set nlang-provider nobody"
	"does not take the value.*nlang-provider: \\(none\\)" NO_FAIL_REGEX
	NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_endpoint_url
	"set nlang-endpoint http://localhost:8080/v1"
	"nlang-endpoint: http://localhost:8080/v1" NO_TRACE ENV ${_llm_env})
# A host name needs the quotes: bare, the text after its first '.' reads
# as a command of its own.
add_repl_test(ba_options-nlang_endpoint_with_dots
	"set nlang-endpoint = \\\"https://api.anthropic.com/v1\\\""
	"nlang-endpoint: https://api.anthropic.com/v1" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_model_with_dashes
	"set nlang-model my_model-4.5"
	"nlang-model: my_model-4.5" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_model_default_is_none "get nlang-model"
	"nlang-model: \\(none\\)" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_model_env_is_the_fallback "get nlang-model"
	"nlang-model: env-model" NO_TRACE ENV ${_llm_env} TAU_NLANG_MODEL=env-model)
add_repl_test(ba_options-nlang_model_flag_beats_env "get nlang-model"
	"nlang-model: flag-model" NO_TRACE
	ENV ${_llm_env} TAU_NLANG_MODEL=env-model FLAGS --nlang-model flag-model)
add_repl_test(ba_options-nlang_named_model_gets_no_effort
	"set nlang-provider anthropic. set nlang-model claude-haiku-4-5. get nlang-effort. get nlang-fallback"
	"nlang-effort: \\(none\\)[\r\n].*nlang-fallback: on" NO_TRACE
	ENV ${_llm_env})
add_repl_test(ba_options-nlang_effort_bad_word "set nlang-effort extreme"
	"does not take the value" NO_FAIL_REGEX NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_effort_set "set nlang-effort xhigh"
	"nlang-effort: xhigh" NO_TRACE ENV ${_llm_env})
# The key is never printed: neither by the set that takes it nor by a get.
add_repl_test(ba_options-nlang_api_key_unset "get nlang-api-key"
	"nlang-api-key: unset" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_api_key_is_masked
	"set nlang-api-key sk-secret_1. get nlang-api-key. get"
	"nlang-api-key: set[\r\n].*nlang-api-key: set" NO_TRACE
	FAIL_REGEX "nlang-[a-z-]+: [^\r\n]*sk-secret_1" ENV ${_llm_env})
add_repl_test(ba_options-nlang_api_key_env_is_masked "get nlang-api-key"
	"nlang-api-key: set" NO_TRACE FAIL_REGEX "sk-secret_2"
	ENV ${_llm_env} TAU_NLANG_API_KEY=sk-secret_2)
add_repl_test(ba_options-nlang_api_key_flag "get nlang-api-key"
	"nlang-api-key: set" NO_TRACE FAIL_REGEX "sk-secret_3"
	ENV ${_llm_env} FLAGS --nlang-api-key sk-secret_3)
add_repl_test(ba_options-nlang_provider_flag_bad_word "get nlang-provider"
	"does not take the value.*name=nlang-provider" NO_FAIL_REGEX NO_TRACE
	ENV ${_llm_env} FLAGS --nlang-provider nobody)
add_repl_test(ba_options-nlang_text_is_not_a_flag "enable nlang-model"
	"takes a text, not a flag" NO_FAIL_REGEX NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_max_tokens "set nlang-max-tokens 512"
	"nlang-max-tokens: *512" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_max_tokens_default "get nlang-max-tokens"
	"nlang-max-tokens: *16000" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_max_tokens_env_is_the_fallback
	"get nlang-max-tokens" "nlang-max-tokens: *2048" NO_TRACE
	ENV ${_llm_env} TAU_NLANG_MAX_TOKENS=2048)
add_repl_test(ba_options-nlang_fallback_disable
	"set nlang-provider anthropic. disable nlang-fallback"
	"nlang-fallback: off" NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_fallback_env
	"set nlang-provider anthropic. get nlang-fallback"
	"nlang-fallback: off" NO_TRACE ENV ${_llm_env} TAU_NLANG_FALLBACK=off)
# The fallback is on by default, also for a model named later.
add_repl_test(ba_options-nlang_fallback_on_for_a_named_model
	"get nlang-fallback. set nlang-model claude-haiku-4-5. get nlang-fallback"
	"nlang-fallback: on[\r\n].*nlang-model: claude-haiku-4-5[\r\n].*nlang-fallback: on"
	NO_TRACE ENV ${_llm_env} FLAGS --nlang-provider anthropic)
# A provider the variable names must exist.
add_env_error_test(ba_options-nlang_provider_env_bad_word
	TAU_NLANG_PROVIDER=nobody REQUIRES nlang)
add_repl_test(ba_options-nlang_help_lists_the_text_options "help set"
	"nlang-provider +LLM oracle API" NO_FAIL_REGEX NO_TRACE REQUIRES nlang)
# A flag given on the command line is written even when it repeats the value
# in force before the options that follow it apply.
add_repl_test(ba_options-nlang_fallback_flag_given_off "get nlang-fallback"
	"nlang-fallback: off" NO_TRACE ENV ${_llm_env}
	FLAGS --nlang-provider anthropic --nlang-fallback false)
add_repl_test(ba_options-nlang_fallback_flag_given_on "get nlang-fallback"
	"nlang-fallback: on" NO_TRACE ENV ${_llm_env}
	FLAGS --nlang-provider anthropic --nlang-model claude-haiku-4-5
		--nlang-fallback)
# Quotes hold any text; empty quotes clear the option.
add_repl_test(ba_options-nlang_endpoint_quoted_with_a_query
	"set nlang-endpoint \\\"https://host.example.com/v1?api-version=2026-01&x=a+b\\\""
	"nlang-endpoint: https://host.example.com/v1\\?api-version=2026-01&x=a\\+b"
	NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_model_quoted_with_dots
	"set nlang-model \\\"gemini-2.0-flash\\\". get nlang-provider"
	"nlang-model: gemini-2.0-flash[\r\n].*nlang-provider: \\(none\\)"
	NO_TRACE ENV ${_llm_env})
add_repl_test(ba_options-nlang_model_cleared_by_empty_quotes
	"set nlang-model some-model. set nlang-model \\\"\\\""
	"nlang-model: some-model[\r\n].*nlang-model: \\(none\\)"
	NO_TRACE ENV ${_llm_env} TAU_NLANG_MODEL=env-model)
add_repl_test(ba_options-quoted_value_of_a_core_option
	"set severity \\\"info\\\"" "severity: *info" NO_TRACE)
