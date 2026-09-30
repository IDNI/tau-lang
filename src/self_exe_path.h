// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__SELF_EXE_PATH_H__
#define __IDNI__TAU__SELF_EXE_PATH_H__

#include <string>
#include <vector>

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif !defined(__EMSCRIPTEN__)
#include <unistd.h>
#endif

namespace idni::tau_lang {

// The path of the running executable. The platform self-path call, not
// argv[0], so a PATH lookup or a symlink still names the real binary.
inline std::string self_exe_path() {
#if defined(__EMSCRIPTEN__)
	return {};
#elif defined(_WIN32) && !defined(__CYGWIN__)
	char buf[MAX_PATH];
	DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
	if (n == 0 || n == MAX_PATH) return {};
	return std::string(buf, n);
#elif defined(__APPLE__)
	uint32_t size = 0;
	_NSGetExecutablePath(nullptr, &size);
	std::string buf(size, '\0');
	if (_NSGetExecutablePath(buf.data(), &size) != 0) return {};
	return std::string(buf.c_str());
#else
	std::vector<char> buf(4096);
	for (;;) {
		ssize_t n = readlink("/proc/self/exe", buf.data(), buf.size());
		if (n < 0) return {};
		if (static_cast<size_t>(n) < buf.size())
			return std::string(buf.data(), static_cast<size_t>(n));
		buf.resize(buf.size() * 2);
	}
#endif
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SELF_EXE_PATH_H__
