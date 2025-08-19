// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <map>
#include <memory>
#include <vector>

#include "tau_tree.h"

namespace idni::tau_lang {

struct resolver {

	struct scope {
		std::shared_ptr<scope> parent;
		std::map<tref, size_t> vars;

		scope open() {
			return scope{
				.parent = std::make_shared<scope>(*this),
				.vars = vars };
		}

		scope close() {
			if (parent) {
				parent->vars.insert(vars.begin(), vars.end());
				return *parent;
			}
			return *this;
		}
	};

	void open() {
		current = current.open();
	}

	void open(const tref& var) {
		open();
		current.vars.erase(var);
		add(var);
	}

	void open(const tref& var, const tref& var_type) {
		open(var);
		type(var, var_type);
	}

	void close() {
		current = current.close();
	}

	void close(const tref& var) {
		current.vars.erase(var);
		close();
	}

	size_t add(const tref& var) {
		if (current.vars.find(var) == current.vars.end()) {
			size_t idx = parent.size();
			parent.push_back(idx);
			rank.push_back(0);
			current.vars[var] = idx;
		}
		return current.vars[var];
	}

	size_t add(const tref& var, const tref& var_type) {
		auto idx = add(var);
		type(var, var_type);
		return idx;
	}

	size_t find(const size_t idx) {
		if (parent[idx] != idx) {
			parent[idx] = find(parent[idx]);
		}
		return parent[idx];
	}

	size_t find(const tref& var) {
		auto idx = current.vars.at(var);
		return find(idx);
	}

	void type(const tref& var, const tref& type) {
		types[find(var)] = type;
	}

	tref type_of(const tref& var) {
		auto it = types.find(find(var));
		if (it != types.end()) {
			return it->second;
		}
		return std::nullptr_t{};
	}

	void unite(const tref& var1, const tref& var2) {
		size_t root1 = find(var1);
		size_t root2 = find(var2);
		if (root1 != root2) {
			if (rank[root1] < rank[root2]) {
				parent[root1] = root2;
			} else if (rank[root1] > rank[root2]) {
				parent[root2] = root1;
			} else {
				parent[root2] = root1;
				++rank[root1];
			}
		}
	}

	bool connected(const tref& var1, const tref& var2) {
		return find(var1) == find(var2);
	}

	std::pair<std::map<tref, tref>, std::set<tref>> status() {
		std::map<tref, tref> typed;
		std::set<tref> untyped;

		for (const auto& [var, idx] : current.vars) {
			if (auto type = type_of(var); type) {
				typed[var] = type;
			} else {
				untyped.insert(var);
			}
		}

		return {std::move(typed), std::move(untyped)};
	}

	std::vector<size_t> parent;
	std::vector<size_t> rank;
	std::map<size_t, tref> types;
	scope current;
};

} // namespace idni::tau_lang