// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"

TEST_SUITE("bitblasting predicates") {

	size_t addition(size_t x, size_t y, size_t m) {
		if (!y) return x % m;
		return addition(x ^ y, (x & y) << 1, m) % m;
	}

	TEST_CASE("addition") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 0; n != 255; ++n)
				for (size_t k = 0; k != 255; ++k)
					CHECK( addition(n, k, m) == ((n + k) % m) );
    }

	size_t subtract(size_t x, size_t y, size_t m) {
		if (!y) return x % m;
		return subtract(x ^ y, (~x & y) << 1, m) % m;
	}

	TEST_CASE("subtraction") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 0; n != 255; ++n)
				for (size_t k = 0; k != 255; ++k)
					CHECK( subtract(n, k, m) == ((n - k) % m) );
	}

	size_t multiply(size_t x, size_t y, size_t m) {
		size_t result = 0;
		while (y) {
			if (y & 1) result = addition(result, x, m);
			x = addition(x, x, m);
			y = y >> 1;
		}
		return result % m;
	}

	TEST_CASE("multiplication") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 0; n != 255; ++n)
				for (size_t k = 0; k != 255; ++k)
					CHECK( multiply(n, k, m) == ((n * k) % m) );
	}

	/*std::pair<size_t, size_t> divide(size_t x, size_t y, size_t m) {
		if (!y) return {0, 0}; // division by zero is not defined, return 0 for testing purposes
		size_t q = 0;
		while (x >= y) {
			x = subtract(x, y, m);
			q = addition(q, 1, m);
		}
		return {q % m, x % m};
	}

	TEST_CASE("divide") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 0; n != 255; ++n)
				for (size_t k = 1; k != 255; ++k) { // avoid divide by zero
					LOG_INFO << "n: " << n << ", k: " << k << ", m: " << m << "\n";
					LOG_INFO << "quotient: " << divide(n, k, m).first << ", remainder: " << divide(n, k, m).second << "\n";
					LOG_INFO << "real quotient: " << (n / k) % m << ", remainder: " << (n % k) % m << "\n";
					CHECK( divide(n, k, m) == std::make_pair((n / k) % m, (n % k) % m) );
				}
	}*/

	size_t shr(size_t x, size_t y, size_t m) {
		return (x >> y) % m;
	}

	size_t shl(size_t x, size_t y, size_t m) {
		return (x << y) % m;
	}

	/*size_t mod(size_t x, size_t y, size_t m) {
		if (!y) return 0; // modulo by zero is not defined, return 0 for testing purposes
		while (x >= y) x = subtract(x, y, m);
		return x % m;
	}*/
}
