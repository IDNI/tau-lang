// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Test stub standing in for Spot's ltlsynt: every synthesis call answers
// UNREALIZABLE at once, and every --print-game-hoa call outlasts any small
// ltl-timeout, so the data game is never built. With an input atom reading
// several steps, no check can then decide the spec: it is UNKNOWN. A program
// and not a script, so a Windows build starts it as ltlsynt.exe.

#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i)
		if (std::strcmp(argv[i], "--print-game-hoa") == 0) {
			std::this_thread::sleep_for(std::chrono::seconds(60));
			return 0;
		}
	std::puts("UNREALIZABLE");
	return 1;
}
