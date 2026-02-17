// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

struct api_fixture {
        strings terms, formulas, function_defs, predicate_defs, stream_defs, specs;
        api_fixture() :
                terms(init_terms()),
                formulas(init_formulas()),
                function_defs(init_function_defs()),
                predicate_defs(init_predicate_defs()),
                stream_defs(init_stream_defs()),
                specs(init_specs())
        {}

        strings init_terms() {
            return {
                "x",
                "y & 1",
                "z * z",
                "x + y",
                "z * z",
                "x * y",
                "x / y",
                "x % y",
                "xyz",
            };
        }

        strings init_formulas() {
            return {
                "p = 0",
                "F && q = 1",
                "q(x, y) || T",
                "r[t](x, y, z) > x + 1",
                "x = 0 && z = 0 || y = 1",
                "x > y",
                "z < y",
            };
        }

        strings init_function_defs() {
            return {
                "i(x) := x + 1",
                "c(x, y) := xy",
                "f(x) := x + 1",
                "g(x) := x|x^x",
                "o[n](x, y, z) := g(y) | f(z) ^ o[n-1](x, y, z)",
            };
        }

        strings init_predicate_defs() {
            return {
                "P(x) := x > 0",
                "Q[t](x, y) := x = y",
                "R[t](x, y, z) := x > y && y > z",
            };
        }

        strings init_stream_defs() {
            return {
                "i := in file(\"file.txt\")",
                "o := out file(\"file.txt\")",
                "o0 := out console",
            };
        }

        strings init_specs() {
            return {
                "o[t] = i[t].",
                "o[t] = i[t] && o[t-1] = i[t-1].",
                "o[t] = o(i[t]) && o(0) || o(1).",
            };
        }
};
