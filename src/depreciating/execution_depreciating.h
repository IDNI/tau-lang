// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __EXECUTION_DEPRECIATING_H__
#define __EXECUTION_DEPRECIATING_H__

#include <string>
#include <optional>
#include <boost/log/trivial.hpp>

#include "boolean_algebras/variant_ba.h"
#include "parser.h"
#include "normal_forms_depreciating.h"
#include "boolean_algebras/bdds/bdd_handle.h"

namespace idni::tau_lang_depreciating {

// TODO (MEDIUM) clean execution api code
template<typename... BAs>
struct step {
	step(idni::tau_lang_depreciating::library<tau_<BAs...>> lib): lib(lib) {}

	tau_<BAs...> operator()(const tau_<BAs...>& n) const {
		return nso_rr_apply(lib, n);
	}

	library<tau_<BAs...>> lib;
};

template<typename step_t, typename...BAs>
struct steps {

	steps(std::vector<step_t> libraries) : libraries(libraries) {}
	steps(step_t library) {
		libraries.push_back(library);
	}

	tau_<BAs...> operator()(const tau_<BAs...>& n) const {
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

	tau_<BAs...> operator()(const tau_<BAs...>& n) const {
		auto nn = n;
		for (auto& l: s.libraries) {
			std::set<tau_<BAs...>> visited;
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

	tau_<BAs...> operator()(const tau_<BAs...>& n) const {
		auto nn = n;
		std::set<tau_<BAs...>> visited;
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

	tau_<BAs...> operator()(const tau_<BAs...>& n) const {
		auto nn = n;
		for(auto& l: s.libraries) {
			nn = l(nn);
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename...BAs>
steps<step<BAs...>, BAs...> operator|(const library<tau_<BAs...>>& l, const library<tau_<BAs...>>& r) {
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
steps<step_t, BAs...> operator|(const steps<step_t, BAs...>& s, const library<tau_<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
steps<step<library<tau_<BAs...>>, BAs...>, BAs...> operator|(const steps<step<library<tau_<BAs...>>, BAs...>, BAs...>& s, const library<tau_<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& n, const library<tau_<BAs...>>& l) {
	auto s = step<BAs...>(l);
	return s(n);
}

template<typename step_t, typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& n, const steps<step_t, BAs...>& s) {
	return s(n);
}

template<typename step_t, typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& n, const repeat_once<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& n, const repeat_all<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& n, const repeat_each<step_t, BAs...>& r) {
	return r(n);
}

} // namespace idni::tau_lang_depreciating
#endif // __EXECUTION_DEPRECIATING_H__