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

	size_t multiply2(size_t x, size_t y, size_t m) {
		if (y == 0) return 0;
		if (y & 1) {
			size_t result = multiply2(x << 1, y >> 1, m);
			return addition(result, x, m);
		}
		return multiply2(x << 1, y >> 1, m);
	}

	TEST_CASE("multiplication2") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 1; n != 255; ++n)
				for (size_t k = 1; k != 255; ++k)
					CHECK( multiply2(n, k, m) == ((n * k) % m) );
	}


	std::pair<size_t, size_t> division(size_t a, size_t b) {
		if (b == 0) return {0, 0}; // division by zero is not defined, return 0 for testing purposes
		if (a < b) return {0, a};
		auto [q, r] = division(a, b << 1);
		q = q << 1;
		if (r >= b) {
			q = q + 1;
			r = r - b;
		}
		return {q, r};
	}

	TEST_CASE("division & modulo") {
		for (size_t m = 2; m != 256; m = m << 1)
			for (size_t n = 1; n != 255; ++n)
				for (size_t k = 1; k != 255; ++k) {
					auto [q, r] = division(n, k);
					CHECK( q % m == ((n / k) % m) );
					CHECK( r % m == ((n % k) % m) );
				}
	}



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
