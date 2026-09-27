// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __TAU_TEST_SCRATCH_DIR_H__
#define __TAU_TEST_SCRATCH_DIR_H__

#include <filesystem>
#include <random>
#include <string>
#include <system_error>

// A directory owned by this test process, under the system temp directory
// (TMPDIR when set). It is created on first use and removed at exit, so two
// runs of the same suite at once never write to the same path.
inline const std::filesystem::path& test_scratch_dir() {
	struct scratch {
		std::filesystem::path dir;
		scratch() {
			namespace fs = std::filesystem;
			std::error_code ec;
			fs::path base = fs::temp_directory_path(ec);
			if (ec) base = ".";
			std::random_device rd;
			std::mt19937_64 gen((uint64_t(rd()) << 32) ^ rd());
			for (int attempt = 0; attempt < 100; ++attempt) {
				fs::path p = base / ("tau_test_" + std::to_string(gen()));
				// create_directory is false when the path exists already
				if (fs::create_directory(p, ec) && !ec) { dir = p; return; }
			}
			dir = base;
		}
		~scratch() {
			std::error_code ec;
			if (dir.filename().string().starts_with("tau_test_"))
				std::filesystem::remove_all(dir, ec);
		}
	};
	static const scratch s;
	return s.dir;
}

// `name` inside test_scratch_dir().
inline std::filesystem::path test_scratch_path(const std::string& name) {
	return test_scratch_dir() / name;
}

#endif // __TAU_TEST_SCRATCH_DIR_H__
