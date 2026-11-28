# Inspecting a guarded rule without an interpreter

The native Python binding exposes `tau.describe(source)`. For C++ callers,
include `tau_rule_inspection.h` explicitly and call
`idni::tau_lang::describe(source)` with the configured Tau library linked.
The call parses source and resolves types. It does not create an interpreter,
solve the rule, or execute a step. Serialize it with other native Tau operations
in the same process.

```python
import tau

facts = tau.describe("always (o5[t]:bv[24] = i1[t-1]:bv[24]).")
if facts["status"] == "Described":
    assert facts["reads"]["i1"] == {
        "domain": "bv", "width": 24, "deepest_lookback": 1,
    }
    assert facts["writes"]["o5"] == {"domain": "bv", "width": 24}
    assert facts["required_history"] == 1
    assert facts["functional_shape"] == "proved_for_subset"
```

The current profile covers one `always` rule whose leaves assign the same
numbered current bit-vector output. Guards are Boolean combinations of supported
comparisons of numbered input streams and explicitly typed hexadecimal
bit-vector literals. A leaf assigns one such literal or input stream. Inputs
may use `[t]` or literal `[t-k]` offsets. The source envelope is at most 4096
ASCII bytes, and resolved widths must be between 1 and 384. Results carry
native typing diagnostics in `report`.

A successful result describes syntactic source references. It does not report
which inputs the normalized interpreter will request or which streams a caller
allows. Functional shape means each complete, well-typed input environment
determines one output under this profile. It does not guarantee that sufficient
history is available at startup or that a caller will admit the rule.
`Unsupported` has `functional_shape: "unknown"` and no partial read, write, or
history metadata. It does not mean the input rule is nonfunctional.
