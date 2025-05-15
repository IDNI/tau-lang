// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT

#include <cassert>
#include "doctest.h"
#include "defs.h"
#include "logging.h"

using namespace std;

namespace idni {}
namespace idni::tau_lang {}

using namespace idni;
using namespace idni::tau_lang;

namespace testing = doctest;

int main(int argc, char** argv) {
	DBG(std::cout << "Logging severity level: " << logging::level() << "\n";)
#ifdef TAU_LOG_TRACE_TESTS
	logging::trace();
	std::cout << "Logging severity level set: " << logging::level() << "\n";
#endif // TAU_LOG_TRACE_TESTS

	return doctest::Context(argc, argv).run();
}
