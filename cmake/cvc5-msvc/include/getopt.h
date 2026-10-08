/* To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md */

/* cvc5 parses its command line with getopt_long, in the generated
 * main/options.cpp of the library. The MSVC CRT has no <getopt.h>, so this
 * header gives the part of the GNU one that cvc5 uses.
 *
 * The state and the functions are static, so the header serves one
 * translation unit: a second one would get its own optind. Options are never
 * permuted, which is what the leading '+' of the cvc5 option string asks for.
 * No optreset is declared, so cvc5 resets the scan with optind = 0 only.
 */
#ifndef TAU_CVC5_MSVC_GETOPT_H
#define TAU_CVC5_MSVC_GETOPT_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

struct option {
	const char* name;
	int has_arg;
	int* flag;
	int val;
};

static char* optarg = NULL;
static int optind = 1;
static int opterr = 1;
static int optopt = '?';

/* The rest of a cluster of short options, as -abc, between two calls. */
static const char* tau_getopt_next = "";

/* The option string without its leading '+' or '-'. A ':' that follows makes
 * a missing argument return ':' and silences the messages. */
static __inline const char* tau_getopt_spec(const char* optstring,
	int* colon_mode)
{
	if (*optstring == '+' || *optstring == '-') ++optstring;
	*colon_mode = *optstring == ':';
	return optstring;
}

static __inline int tau_getopt_missing(const char* prog, int colon_mode,
	const char* dashes, const char* name, size_t len)
{
	if (colon_mode) return ':';
	if (opterr) fprintf(stderr, "%s: option requires an argument -- '%s%.*s'\n",
		prog, dashes, (int) len, name);
	return '?';
}

/* argv[optind] is "--name" or "--name=value" and <arg> points after the
 * dashes. A name matches in full, else as the prefix of one option only. */
static __inline int tau_getopt_long_option(int argc, char* const argv[],
	const char* arg, int colon_mode, const struct option* longopts,
	int* longindex)
{
	const char* eq = strchr(arg, '=');
	size_t len = eq ? (size_t) (eq - arg) : strlen(arg);
	int i, found = -1, exact = 0, ambiguous = 0;
	const struct option* o;

	for (i = 0; longopts && longopts[i].name; ++i) {
		if (strncmp(longopts[i].name, arg, len) != 0) continue;
		if (strlen(longopts[i].name) == len) {
			found = i, exact = 1;
			break;
		}
		if (found < 0) found = i;
		else if (longopts[i].has_arg != longopts[found].has_arg
			|| longopts[i].flag != longopts[found].flag
			|| longopts[i].val != longopts[found].val) ambiguous = 1;
	}
	++optind;
	if (found < 0 || (ambiguous && !exact)) {
		if (opterr && !colon_mode) fprintf(stderr,
			found < 0 ? "%s: unrecognized option '--%.*s'\n"
				: "%s: option '--%.*s' is ambiguous\n",
			argv[0], (int) len, arg);
		optopt = 0;
		return '?';
	}
	o = &longopts[found];
	if (eq) {
		if (o->has_arg == no_argument) {
			if (opterr && !colon_mode) fprintf(stderr,
				"%s: option '--%s' doesn't allow an argument\n",
				argv[0], o->name);
			optopt = o->val;
			return '?';
		}
		optarg = (char*) (eq + 1);
	} else if (o->has_arg == required_argument) {
		if (optind >= argc) {
			optopt = o->val;
			return tau_getopt_missing(argv[0], colon_mode, "--",
				o->name, strlen(o->name));
		}
		optarg = argv[optind++];
	}
	if (longindex) *longindex = found;
	if (o->flag) {
		*o->flag = o->val;
		return 0;
	}
	return o->val;
}

static __inline int getopt_long(int argc, char* const argv[],
	const char* optstring, const struct option* longopts, int* longindex)
{
	int colon_mode, c;
	const char* spec = tau_getopt_spec(optstring, &colon_mode);
	const char* decl;

	optarg = NULL;
	if (optind == 0) {
		optind = 1;
		tau_getopt_next = "";
	}
	if (*tau_getopt_next == '\0') {
		const char* arg;
		if (optind >= argc) return -1;
		arg = argv[optind];
		if (arg[0] != '-' || arg[1] == '\0') return -1;
		if (arg[1] == '-') {
			if (arg[2] == '\0') {
				++optind;
				return -1;
			}
			return tau_getopt_long_option(argc, argv, arg + 2,
				colon_mode, longopts, longindex);
		}
		tau_getopt_next = arg + 1;
	}
	c = (unsigned char) *tau_getopt_next++;
	decl = c == ':' || c == ';' ? NULL : strchr(spec, c);
	if (*tau_getopt_next == '\0') ++optind;
	if (!decl) {
		if (opterr && !colon_mode) fprintf(stderr,
			"%s: invalid option -- '%c'\n", argv[0], c);
		optopt = c;
		return '?';
	}
	if (decl[1] != ':') return c;
	/* The argument is the rest of this element, else the next element
	 * unless the option takes an optional one, as "x::". */
	if (*tau_getopt_next != '\0') {
		optarg = (char*) tau_getopt_next;
		++optind;
	} else if (decl[2] != ':') {
		if (optind >= argc) {
			char name = (char) c;
			tau_getopt_next = "";
			optopt = c;
			return tau_getopt_missing(argv[0], colon_mode, "",
				&name, 1);
		}
		optarg = argv[optind++];
	}
	tau_getopt_next = "";
	return c;
}

static __inline int getopt(int argc, char* const argv[], const char* optstring)
{
	return getopt_long(argc, argv, optstring, NULL, NULL);
}

#endif
