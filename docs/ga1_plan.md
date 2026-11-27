# Prepared plans for current-only GA1 rules

A GA1 rule is a current-only guarded definition of one fixed-width
bit-vector output. The rule uses current bit-vector inputs, comparisons,
Boolean combinations and conditionals. Each branch must define that same
output. GA1 is a narrow opt-in profile, not a new interpretation of general
Tau. A rule outside the profile is refused, not run by an alternate evaluator.

## Worker protocol

Run `tau ga1` and send one UTF-8 JSON request per line on stdin. This command
is separate from the REPL. It responds with one JSON line per request. The
worker holds only the plans explicitly created or loaded in that worker.
The caller decides what to persist, which plan is committed to each slot,
what state to pass, and when to drop plans.

A build returns a plan and its initial state:

```json
{"call":{"schema":"taubudget/2","op":"get_interpreter","text":"always ( o5[t]:bv[24] = {1}:bv[24] ).","strategy":"dag","format":"packed","cap":100000}}
```

On success the reply holds `outcome`, `post` and
`plan:{format,digest,saved}`. Here `saved` is lowercase hex representing
the original canonical bytes and `digest` is their SHA-256. Persist the
bytes and the expected digest together at the caller. The header records
the format, compiler and execution semantics. The default compiler accepts
LF and CRLF only at whitespace positions; `compiler:"ga1c/1"` selects
the original single-line profile. Both use `taubudget/2` step semantics.

A fresh worker loads saved bytes without reading, parsing or compiling
the source:

```json
{"call":{"schema":"taubudget/2","op":"load","format":"packed","bytes":"<saved>","bind":"<digest>","cap":16386}}
```

For the `varint` binary format the fields are the same. For `tba1` use
`artifact` with the ASCII saved text after decoding it from hex. The
expected `bind` digest comes from the caller's trusted record; it is
not a claim of authorship or authority by the transport. The optional
`compiler` checks the saved header against the caller's expected origin.

Step the held plan with the inputs named in the build output, encoded as
fixed-width lowercase hexadecimal (without a prefix):

```json
{"call":{"schema":"taubudget/2","op":"step","state":{"plan":"<digest>","revision":0,"time":0},"inputs":{},"eval":"lazy","cap":10000}}
```

Only a Completed step advances the returned clock. A failed or unavailable
step returns the caller's pre-state unchanged and does not trigger a load,
source rebuild or solver call. Restore using the committed saved bytes
and retry if the plan is not held. An account's active plan selection and
revision policy are outside this API. Compute receipts are scoped to GA1;
they are not a general engine-wide work meter.

Use `{"save":"<digest>"}` to return held bytes without any step-time state,
and `{"drop":"<digest>"}` to release the worker's ownership of a plan.
Dropping one worker's plan does not affect another worker. A process
exit releases its held plans. Callers that retain a C++ plan handle
elsewhere retain its storage until they also release that handle. No GA1
plan holds engine tree references or solver state. For C++ embed the
pure core and worker through `ga1/worker.h`; make one worker per thread and
serialize its calls.

## Verification

`test_ga1_worker` checks all three formats, fresh-worker restore,
identical step receipts, a wrong binding, release, and restore after drop.
Binary and JSON use the same semantics and keep their distinct digest
identities. Cross-platform interoperability can be checked by producing
the saved bytes on one target, consuming them on the other, and comparing
bytes, digests and step receipts; compiler ABI and native interpreter
state are not encoded in the wire bytes.
