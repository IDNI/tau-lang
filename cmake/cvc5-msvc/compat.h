/* To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md */

/* The POSIX names that cvc5 and CaDiCaL use and the MSVC CRT does not give.
 * The windows-x86_64-msvc producer forces this header into every clang-cl
 * compile with /FI. The CRT keeps access, close, write, isatty, fileno and
 * getpid under their POSIX names. It declares popen and pclose only as _popen
 * and _pclose, so the functions below give the POSIX names without a macro.
 */
#ifndef TAU_CVC5_MSVC_COMPAT_H
#define TAU_CVC5_MSVC_COMPAT_H

#if defined(_WIN32) && !defined(__MINGW32__)

#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef ptrdiff_t ssize_t;
#endif

#ifndef R_OK
#define R_OK 4
#endif
#ifndef W_OK
#define W_OK 2
#endif

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) (((m) & _S_IFMT) == _S_IFIFO)
#endif

/* _mktemp_s fills the XXXXXX suffix, and _O_EXCL makes the open fail when
 * another process takes the same name first. */
static __inline int mkstemp(char* name) {
	if (_mktemp_s(name, strlen(name) + 1) != 0) return -1;
	return _open(name, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY,
		_S_IREAD | _S_IWRITE);
}

static __inline FILE* popen(const char* command, const char* mode) {
	return _popen(command, mode);
}

static __inline int pclose(FILE* stream) {
	return _pclose(stream);
}

#endif

#endif
