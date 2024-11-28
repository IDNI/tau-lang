// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __EXECUTION_H__
#define __EXECUTION_H__

#include <string>
#include <optional>
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "normal_forms.h"
#include "bdd_handle.h"
#include "variant_ba.h"

namespace idni::tau {

// TODO (MEDIUM) clean execution api code
template<typename... BAs>
struct step {
	step(library<nso<BAs...>> lib): lib(lib) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		return nso_rr_apply(lib, n);
	}

	library<nso<BAs...>> lib;
};

template<typename step_t, typename...BAs>
struct steps {

	steps(std::vector<step_t> libraries) : libraries(libraries) {}
	steps(step_t library) {
		libraries.push_back(library);
	}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		if (libraries.empty()) return n;
		auto nn = n;
		for (auto& lib : libraries) nn = lib(nn);
		return nn;
	}

	std::vector<step_t> libraries;
};

template<typename step_t, typename... BAs>
struct repeat_each {

	repeat_each(steps<step_t, BAs...> s) : s(s) {}
	repeat_each(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		for (auto& l: s.libraries) {
			std::set<nso<BAs...>> visited;
			while (true) {
				nn = l(nn);
				if (visited.find(nn) != visited.end()) break;
				visited.insert(nn);
			}
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename step_t, typename... BAs>
struct repeat_all {

	repeat_all(steps<step_t, BAs...> s) : s(s) {}
	repeat_all(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		std::set<nso<BAs...>> visited;
		while (true) {
			for (auto& l: s.libraries) nn = l(nn);
			auto nnn = s(nn);
			if (nnn == nn) break;
			nn = nnn;
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename step_t, typename... BAs>
struct repeat_once {

	repeat_once(steps<step_t, BAs...> s) : s(s) {}
	repeat_once(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		for(auto& l: s.libraries) {
			nn = l(nn);
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename...BAs>
steps<step<BAs...>, BAs...> operator|(const library<nso<BAs...>>& l, const library<nso<BAs...>>& r) {
	auto s = steps<step<BAs...>, BAs...>(step<BAs...>(l));
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename...BAs>
steps<repeat_each<step_t, BAs...>, BAs...> operator|(const repeat_each<step_t, BAs...>& l, const repeat_each<step_t, BAs...>& r) {
	auto s = steps<repeat_each<step_t, BAs...>, BAs...>(l);
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename...BAs>
steps<repeat_all<step_t, BAs...>, BAs...> operator|(const repeat_all<step_t, BAs...>& l, const repeat_all<step_t, BAs...>& r) {
	auto s = steps<repeat_all<step_t, BAs...>, BAs...>(l);
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename... BAs>
steps<step_t, BAs...> operator|(const steps<step_t, BAs...>& s, const step_t& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename step_t, typename... BAs>
steps<step_t, BAs...> operator|(const steps<step_t, BAs...>& s, const library<nso<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
steps<step<library<nso<BAs...>>, BAs...>, BAs...> operator|(const steps<step<library<nso<BAs...>>, BAs...>, BAs...>& s, const library<nso<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const library<nso<BAs...>>& l) {
	auto s = step<BAs...>(l);
	return s(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const steps<step_t, BAs...>& s) {
	return s(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_once<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_all<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_each<step_t, BAs...>& r) {
	return r(n);
}

} // namespace idni::tau
#endif // __EXECUTION_H__