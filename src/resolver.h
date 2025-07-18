// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <map>
#include <memory>
#include <vector>

#include "nso_rr.h"

template<typename...BAs>
struct resolver {

	struct scope {
		std::shared_ptr<scope> parent;
		std::map<tau<BAs...>, size_t> vars;

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

	void open(const tau<BAs...>& var) {
		open();
		current.vars.erase(var);
		add(var);
	}

	void open(const tau<BAs...>& var, const tau<BAs...>& var_type) {
		open(var);
		type(var, var_type);
	}

	void close() {
		current = current.close();
	}

	void close(const tau<BAs...>& var) {
		current.vars.erase(var);
		close();
	}

	size_t add(const tau<BAs...>& var) {
		if (current.vars.find(var) == current.vars.end()) {
			size_t idx = parent.size();
			parent.push_back(idx);
			rank.push_back(0);
			current.vars[var] = idx;
		}
		return current.vars[var];
	}

	size_t find(const size_t idx) {
		if (parent[idx] != idx) {
			parent[idx] = find(parent[idx]);
		}
		return parent[idx];
	}

	size_t find(const tau<BAs...>& var) {
		auto idx = current.vars.at(var);
		return find(idx);
	}

	void type(const tau<BAs...>& var, const tau<BAs...>& type) {
		types[find(var)] = type;
	}

	tau<BAs...> type_of(const tau<BAs...>& var) {
		auto it = types.find(find(var));
		if (it != types.end()) {
			return it->second;
		}
		return std::nullptr_t{};
	}

	void unite(const tau<BAs...>& var1, const tau<BAs...>& var2) {
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

	bool connected(const tau<BAs...>& var1, const tau<BAs...>& var2) {
		return find(var1) == find(var2);
	}

	std::pair<std::map<tau<BAs...>, tau<BAs...>>, std::set<tau<BAs...>>> status() {
		std::map<tau<BAs...>, tau<BAs...>> typed;
		std::set<tau<BAs...>> untyped;

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
	std::map<size_t, tau<BAs...>> types;
	scope current;
};