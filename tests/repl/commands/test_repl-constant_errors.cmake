include(add_repl_test)

#
# Each refusal of a constant of the qlt, qint and nlang parsers, through the
# typed constant it comes from.
#

# constant_error(<name> <ba> <constant> <message regex>)
function(constant_error name ba constant message)
	add_repl_test(constant_error-${name} "n o1[t]:${ba} = {${constant}}:${ba}"
		"${message}" NO_FAIL_REGEX NO_TRACE FLAGS --color false REQUIRES ${ba})
endfunction()

constant_error(qlt_interval_not_rational qlt "[1/0,2]"
	"Not a rational or a name in a qlt interval \\(value=1/0\\)")
constant_error(qlt_singleton_not_rational qlt "3/0"
	"Not a rational or a name as a qlt singleton \\(value=3/0\\)")
constant_error(qlt_interval_reversed qlt "[2,1]"
	"Empty qlt interval \\(value=2, 1\\)")
constant_error(qlt_interval_open_point qlt "(1,1)"
	"Empty qlt interval \\(value=1, 1\\)")
constant_error(qlt_infinity_singleton qlt "inf"
	"An infinity is not a qlt singleton \\(value=inf\\)")
constant_error(qlt_negative_infinity_singleton qlt "-inf"
	"An infinity is not a qlt singleton \\(value=-inf\\)")
constant_error(qlt_names_its_type qlt "3/0"
	"parsing a constant failed for its type \\(value=3/0 type name=:qlt")

constant_error(qint_interval_not_rational qint "[1/0,2)"
	"Not a rational in a qint interval \\(value=1/0\\)")
constant_error(qint_interval_reversed qint "[2,1)"
	"Empty qint interval \\(value=2, 1\\)")
constant_error(qint_interval_empty qint "[1,1)"
	"Empty qint interval \\(value=1, 1\\)")
constant_error(qint_closed_upper_bound qint "[0,1]" "Syntax Error: Unexpected '\\]'")
constant_error(qint_open_lower_bound qint "(1,2)" "Syntax Error: Unexpected '\\('")
constant_error(qint_name qint "x" "Syntax Error: Unexpected 'x'")
