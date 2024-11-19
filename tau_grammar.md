- `name` is a sequence of letters and positive numbers starting with a letter
- `number` is any positive number

```
spec_with_defs  => (func_pred_def)* spec.
func_pred_def   => predicate_def | function_def

spec  => 
local_spec | always local_spec | sometimes local_spec 
| (spec && spec) | (spec || spec) | !spec

local_spec => 
(local_spec "&&" local_spec) | "!" local_spec 
| (local_spec "^" local_spec) | (local_spec "||" local_spec)
| (local_spec "->" local_spec) | (local_spec "<->" local_spec) 
| (local_spec "?" local_spec ":" local_spec)
| (term "=" term) | (term "!=" term) | (term "<" term)
| (term "!<" term) | (term "<=" term) | (term "!<=" term)
| (term ">" term) | (term "!>" term)
| "all" variable local_spec
| "ex" variable local_spec | predicate | T | F

term => 
(term "&" term) | term "'" 
| (term "+" term) | (term "|" term)
| function | constant | uninterpreted_constant 
| variable | stream_variable | "0" | "1"

variable[charvar] => letter number
variable[var]     => name
stream_variable   => input_stream_variable | output_stream_variable

input_stream_variable => "i" number "[" index "]"
output_stream_variable => "o" number "[" index "]"

function_def      => function ":=" term
function          => name "[" index+  "]" "(" [ variable ("," variable)* ] ")"
predicate_def     => predicate ":=" spec
predícate         => name "[" index+  "]" "(" [ variable ("," variable)* ] ")"

index             => number | variable | variable "-" number

uninterpreted_constant => "<" [name] ":" name ">"

constant => "{" (spec | sbf) "}" [":" base_boolean_algebra_type]

base_boolean_algebra_type => "tau" | "sbf"

sbf => 
(sbf "&" sbf) | sbf "'" | (sbf "^" sbf) 
| (sbf "+" sbf) | (sbf "|" sbf)
| variable | "0" | "1"

```