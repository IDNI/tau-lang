// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "nso_rr.h"

namespace idni::tau_lang {

/*
 * Struct to manage function and predicate definitions
 */
template<typename ... BAs>
struct definitions {
	size_t add (const tau<BAs...>& head, const tau<BAs...>& body) {
		// Check if rule is updated
		for (size_t i = 0; i < heads.size(); ++i) {
			if (heads[i] == head) {
				// update definition
				heads[i] = head;
				bodies[i] = body;
				return i;
			}
		}
		heads.push_back(head);
		bodies.push_back(body);
		return heads.size() - 1;
	}

	rules<tau<BAs...>> get () {
		rules<tau<BAs...>> r;
		for (size_t i = 0; i < heads.size(); ++i)
			r.emplace_back(heads[i], bodies[i]);
		return r;
	}

	size_t size () const {
		DBG(assert(heads.size() == bodies.size());)
		return heads.size();
	}

	std::pair<tau<BAs...>, tau<BAs...>> back () const {
		return std::make_pair(heads.back(), bodies.back());
	}

	std::pair<tau<BAs...>, tau<BAs...>> operator[](const size_t i) const {
		return std::make_pair(heads[i], bodies[i]);
	}

	static definitions& instance() {
		static definitions d;
		return d;
	}
private:
	definitions() = default;

	std::vector<tau<BAs...>> heads = {};
	std::vector<tau<BAs...>> bodies = {};
};

}

#endif //DEFINITIONS_H
