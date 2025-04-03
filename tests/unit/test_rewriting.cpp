// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"

using namespace idni;
using namespace idni::rewriter;

using chtree = idni::lcrs_tree<char>;

tref n(const char& value) {
	return chtree::get(value);
}

tref n(const char& value, const vector<tref>& children) {
	return chtree::get(value, children);
}

const chtree& d(const char& value) {
	return chtree::get(n(value));
}

const chtree& d(const char& value, const vector<tref>& children) {
	return chtree::get(n(value, children));
}

// tests suites

TEST_SUITE("node") {

	TEST_CASE("node constructor: given a value, the node has that value") {
		auto d1 = d('a');
		CHECK( d1.value == 'a' );
		CHECK( d1.children_size() == 0 );
	}

	TEST_CASE("node constructor: given a value and children, the node has that value "
			"and children") {
		auto d1 = d('a', {n('b'), n('c'), n('d')});
		CHECK( d1.value == 'a' );
		CHECK( d1.children_size() == 3 );
		CHECK( d1.child_tree(0).value == 'b' );
		CHECK( d1.child_tree(1).value == 'c' );
		CHECK( d1.child_tree(2).value == 'd' );
	}
	TEST_CASE("node order: given two simple nodes, the order is given by the order "
			"of the syms") {
		CHECK( d('a') < d('b') );
	}

	TEST_CASE("node order: given two nodes with same value and different children, "
			"one is bigger than the other") {
		auto d1 = d('a', {n('b')});
		auto d2 = d('a', {n('b'), n('c')});
		CHECK( ((d1 < d2) || (d2 < d1)) );
	}

	TEST_CASE("node order: given two nodes with the same value and children, neither "
			"of them is bigger than the other") {
		auto d1 = d('a', {n('b'), n('c'), n('d')});
		auto d2 = d('a', {n('b'), n('c'), n('d')});
		CHECK( (!(d1 < d2) && !(d2 < d1)) );
	}

	TEST_CASE("node equality: given two simple nodes with the same value, they "
			"are equal") {
		CHECK( d('a') == d('a') );
	}

	TEST_CASE("node equality: given two nodes with the same value and children, "
			"they are equal") {
		CHECK( d('a', {n('b'), n('c'), n('d')})
				== d('a', {n('b'), n('c'), n('d')}) );
	}

	TEST_CASE("node equality: given two simple nodes with different syms, they "
			"are not equal") {
		CHECK( d('a') != d('b') );
	}

	TEST_CASE("node equality: given two nodes with different syms and same children, "
			"they are not equal") {
		CHECK( d('a', {n('b'), n('c'), n('d')}) != d('b', {n('b'), n('c'), n('d')}) );
	}

	TEST_CASE("node equality: given two nodes with same syms and different children, "
			"they are equal") {
		CHECK( d('a', {n('b'), n('c'), n('d')})
				== d('a', {n('b'), n('c'), n('d')}) );
	}
}

TEST_SUITE("make_node") {

	TEST_CASE("make_node uniqueness: given two simple nodes with the same value, it "
			"returns the same node") {
		auto n1 = n('a');
		auto n2 = n('a');
		CHECK( n1 == n2 );
	}

	TEST_CASE("make_node uniqueness: given two nodes with the same value and children, "
			"it returns the same node") {
		auto n1 = n('a', {n('b'), n('c'), n('d')});
		auto n2 = n('a', {n('b'), n('c'), n('d')});
		CHECK( n1 == n2 );
	}

	TEST_CASE("make_node uniqueness: given two nodes with different value and same "
			"children, it returns the different nodes") {
		auto n1 = n('a', {n('b'), n('c'), n('d')});
		auto n2 = n('b', {n('b'), n('c'), n('d')});
		CHECK( n1 != n2 );
	}

	TEST_CASE("make_node uniqueness: given two nodes with same value and different "
			"children, it returns the different nodes") {
		auto n1 = n('a', {n('b'), n('c'), n('d')});
		auto n2 = n('a', {n('b'), n('c'), n('e')});
		CHECK( n1 != n2 );
	}
}

TEST_SUITE("post_order_traverser") {

	struct collect_predicate {
		bool operator()(tref n) { return nodes.push_back(n), true; }
		trefs nodes;
	};

	template<typename wrapped_t>
	struct collect_visitor {
		collect_visitor(wrapped_t& wrapped) : wrapped(wrapped) { }
		tref operator()(tref n) {
			return nodes.push_back(wrapped(n)), nodes.back();
		}
		trefs nodes;
		wrapped_t& wrapped;
	};

	TEST_CASE("post_order_traverser: given a simple tree, it visits the root node") {
		tref root = n('a');
		collect_visitor<decltype(idni::identity)> visited{idni::identity};
		trefs expected {root};
		post_order_traverser<char, decltype(visited),
			decltype(all)>(visited, all)(root);
		CHECK( visited.nodes == expected );
	}

	TEST_CASE("post_order_traverser: given a tree with two children, it visits "
			"the children and then the root") {
		tref root = n('a', {n('b'), n('c')});
		collect_visitor<decltype(idni::identity)> visited{idni::identity};
		trefs expected {n('b'), n('c'), root};
		post_order_traverser<char, decltype(visited),
			decltype(all)>(visited, all)(root);
		CHECK( visited.nodes == expected );
	}

	TEST_CASE("post_order_traversal: given tree with underlying diamond like DAG, "
			"it visits the children and then the root") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		collect_visitor<decltype(idni::identity)> visited{idni::identity};
		trefs expected {n('d'), n('b', {n('d')}), n('c', {n('d')}), root};
		post_order_traverser<char, decltype(visited),
			decltype(all)>(visited, all)(root);
		CHECK( visited.nodes == expected );
	}
}

TEST_SUITE("map_transformer") {

	TEST_CASE("map_transformer: given a simple tree and a visitor, it returns a "
			"new tree with the changes applied by the visitor") {
		tref root = n('a');
		auto transform = [](char c) { return c == 'a' ? 'z' : c; };
		map_transformer<char, decltype(transform)> map{transform};
		tref expected { n('z') };
		auto result = post_order_traverser<char, decltype(map),
			decltype(all)>(map, all)(root);
		CHECK( result == expected );
	}

	TEST_CASE("map_transformer: given a tree with two children and a visitor, "
			"it returns a new tree with the changes applied by the visitor") {
		tref root = n('a', {n('b'), n('c')});
		auto transform = [](char c) { return c == 'b' ? 'z' : c; };
		map_transformer<char, decltype(transform)> map{transform};
		tref expected { n('a', {n('z'), n('c')}) };
		auto result = post_order_traverser<char, decltype(map),
			decltype(all)>(map , all)(root);
		CHECK( result == expected );
	}

	TEST_CASE("map_transformer: given a tree with underlying diamond like DAG "
			"and a visitor, it returns a new tree with the changes applied by "
			"the visitor") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto transform = [](char c) { return c == 'd' ? 'z' : c; };
		map_transformer<char, decltype(transform)> map{transform};
		tref expected { n('a', {n('b', {n('z')}), n('c', {n('z')})}) };
		auto result = post_order_traverser<char, decltype(map),
			decltype(all)>(map , all)(root);
		CHECK( result == expected );
	}
}

// TODO (LOW) write tests for replace_node_transformer

TEST_SUITE("replace_transformer") {

	TEST_CASE("replace_transformer: given a simple tree and a visitor, it returns "
			"a new tree with the provided replacements") {
		tref root = n('a');
		map<tref, tref> m;
		m[root] = n('z');
		replace_transformer<char> replace{m};
		tref expected { n('z') };
		auto result = post_order_traverser<char, decltype(replace),
			decltype(all)>(replace, all)(root);
		CHECK( result == expected );
	}

	TEST_CASE("replace_transform: given a tree with two children and a visitor, "
			"it returns a new tree with the provided replacements") {
		tref root = n('a', {n('b'), n('c')});
		map<tref, tref> m;
		m[n('b')] = n('z');
		replace_transformer<char> replace{m};
		tref expected { n('a', {n('z'), n('c')}) };
		auto result = post_order_traverser<char, decltype(replace), decltype(all)>(replace , all)(root);
		CHECK( result == expected );
	}

	TEST_CASE("replace_transform: given a tree with underlying diamond like DAG "
			"and a visitor, it returns a new tree with the provided replacements") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		map<tref, tref> m;
		m[n('d')] = n('z');
		replace_transformer<char> replace{m};
		tref expected { n('a', {n('b', {n('z')}), n('c', {n('z')})}) };
		auto result = post_order_traverser<char, decltype(replace), decltype(all)>(replace , all)(root);
		CHECK( result == expected );
	}
	// TODO (LOW) write tests corresponding to related functions
}

TEST_SUITE("select_top_predicate") {

	TEST_CASE("select_top_predicate: given a simple tree whose root satisfies the "
			"predicate, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		trefs selected;
		trefs expected{ root };
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty vector") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		trefs selected;
		trefs expected {};
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		CHECK( selected == expected );
	}


	TEST_CASE("selected_top_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the children satisfying the predicate") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) {
			char v = chtree::get(n).value;
			return v == 'b' || v == 'c';
		};
		trefs selected;
		trefs expected {n('b'), n('c')};
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		// TODO revise hashing and get with children. since these should match but they are not
		std::cout << "These pointers should match but they are not. Probably hashing issue?\n"
			"Or get with children changes identity of a node?\n";
		std::cout << "\tselected: " << chtree::get(selected[0]).value << " *" << selected[0]<< "\n";
		std::cout << "\texpected: " << chtree::get(expected[0]).value << " *" << expected[0] << "\n";
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty vector satisfying the "
			"predicate") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) {
			char v = chtree::get(n).value;
			return v == 'd' || v == 'e';
		};
		trefs selected;
		trefs expected {};
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector with only one bottom node "
			"satisfying the predicate") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) {
			return chtree::get(n).value == 'd';
		};
		trefs selected;
		trefs expected {n('d')};
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector the two top nodes satisfying "
			"the predicate") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) {
			char v = chtree::get(n).value;
			return v == 'b' || v == 'c';
		};
		trefs selected;
		trefs expected {n('b', {n('d')}), n('c', {n('d')})};
		select_top_predicate<decltype(predicate)>
					select{ predicate, selected };
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity, select)(root);
		CHECK( selected == expected );
	}

	// TODO (LOW) write tests corresponding to related functions
}

// TODO (MEDIUM) write tests for select_subnodes predicates
// TODO (MEDIUM) write tests for select_subnodes functions


TEST_SUITE("select_all_predicate") {

	TEST_CASE("select_all_predicate: given a simple tree whose root satisfies "
			"the predicate, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		trefs selected;
		trefs expected {root};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty vector") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		trefs selected;
		trefs expected {};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the children satisfying the predicate") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		trefs selected;
		trefs expected {n('b'), n('c')};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty vector satisfying the "
			"predicate") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'
					|| chtree::get(n).value == 'e'; };
		trefs selected;
		trefs expected {};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector with only one bottom node "
			"satisfying the predicate") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'; };
		trefs selected;
		trefs expected {n('d')};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector the two top nodes satisfying "
			"the predicate") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		trefs selected;
		trefs expected {n('b', {n('d')}), n('c', {n('d')})};
		select_all_predicate<decltype(predicate)> select{predicate, selected};
		post_order_traverser<char, decltype(idni::identity),
			decltype(select)>(idni::identity , select)(root);
		CHECK( selected == expected );
	}
}

TEST_SUITE("find_top_predicate") {

	TEST_CASE("find_top_predicate: given a simple tree whose root satisfies the "
			"predicate, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		tref found;
		tref expected{ root };
		find_top_predicate<decltype(predicate)> find{predicate, found};
		post_order_traverser<char, decltype(idni::identity),
			decltype(find)>(idni::identity , find)(root);
		CHECK( found == expected );
	}

	TEST_CASE("find_top_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty optional") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		tref found;
		find_top_predicate<decltype(predicate)> find{predicate, found};
		post_order_traverser<char, decltype(idni::identity),
			decltype(find)>(idni::identity , find)(root);
		CHECK( found == nullptr );
	}

	TEST_CASE("find_top_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the first child satisfying the predicate") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		tref found;
		tref expected = n('b');
		find_top_predicate<decltype(predicate)> find{predicate, found};
		post_order_traverser<char, decltype(idni::identity),
			decltype(find)>(idni::identity , find)(root);
		CHECK( found == expected );
	}

	TEST_CASE("find_top_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty optional") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'
					|| chtree::get(n).value == 'e'; };
		tref found;
		find_top_predicate<decltype(predicate)> find{predicate, found};
		post_order_traverser<char, decltype(idni::identity),
			decltype(find)>(idni::identity , find)(root);
		CHECK( found == nullptr );
	}

	TEST_CASE("find_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns the top node satisfying the predicate") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		tref found;
		tref expected = n('c', {n('d')});
		find_top_predicate<decltype(predicate)> find{predicate, found};
		post_order_traverser<char, decltype(idni::identity),
			decltype(find)>(idni::identity , find)(root);
		CHECK( found == expected );
	}
}

// TODO (LOW) write tests for replace

TEST_SUITE("logical predicates") {

	TEST_CASE("true_predicate: given a node, it always returns true") {
		auto t = true_predicate;
		CHECK( t(n('a')) );
	}

	TEST_CASE("false_predicate: given a node, it always returns false") {
		auto f = false_predicate;
		CHECK( !f(n('a')) );
	}

	TEST_CASE("and_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate;
		auto f = false_predicate;
		CHECK( and_predicate(t, t)(n('a')) );
		CHECK( !and_predicate(t, f)(n('a')) );
		CHECK( !and_predicate(f, t)(n('a')) );
		CHECK( !and_predicate(f, f)(n('a')) );
	}

	TEST_CASE("or_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate;
		auto f = false_predicate;
		CHECK( or_predicate(t, t)(n('a')) );
		CHECK( or_predicate(t, f)(n('a')) );
		CHECK( or_predicate(f, t)(n('a')) );
		CHECK( !or_predicate(f, f)(n('a')) );
	}

	TEST_CASE("neg_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate;
		auto f = false_predicate;
		CHECK( !neg_predicate(t)(n('a')) );
		CHECK( neg_predicate(f)(n('a')) );
	}
}

TEST_SUITE("trim_top") {

	TEST_CASE("trim_top: given a simple tree and a predicate not satisfied by "
			"the root, it returns the tree itself whatever the predicate") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		tref expected {root};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a simple tree and a predicate satisfied by the "
			"root, it returns the tree itself whatever the predicate") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		tref expected {root};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -the right one matching "
			"the predicate-, it returns the tree without the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		tref expected {n('a', {n('b')})};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -the left one matching "
			"the predicate-, it returns the tree without the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		tref expected {n('a', {n('c')})};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -both matching the "
			"predicate-, it returns the root") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		tref expected { n('a') };
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -none matching the "
			"predicate-, it returns the given tree") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'
					|| chtree::get(n).value == 'e'; };
		tref expected { root };
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with underlying diamond like DAG and a "
			"predicate not satisfied by the nodes, it returns the tree itself") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'e'; };
		tref expected {root};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with underlying diamond like DAG and a "
			"predicate satisfied by the bottom, it returns the tree without it") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'; };
		tref expected {n('a', {n('b'), n('c')})};
		CHECK( trim_top<decltype(predicate)>(root, predicate) == expected );
	}
}

TEST_SUITE("select_top") {

	TEST_CASE("select_top: given a simple tree and a predicate not satisfied "
			"by the root, it returns the an empty collection") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		CHECK( select_top<decltype(predicate)>(root, predicate).empty() );
	}

	TEST_CASE("select_top: given a simple tree and a predicate satisfied by "
			"the root, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		vector<tref> expected {root};
		CHECK( select_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with two children -the right one "
			"matching the predicate-, it returns the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		vector<tref> expected {n('c')};
		CHECK( select_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with two children -the left one "
			"matching the predicate-, it returns the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		vector<tref> expected {n('b')};
		CHECK( select_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with two children -both matching "
			"the predicate-, it returns the left one") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		vector<tref> expected { n('b'), n('c') };
		CHECK( select_top<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with underlying diamond like DAG and "
			"a predicate satisfied by the bottom, it returns the bottom") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'd'; };
		vector<tref> expected {n('d')};
		CHECK( select_top<decltype(predicate)>(root, predicate) == expected );
	}
}

TEST_SUITE("select_all") {

	TEST_CASE("select_all: given a simple tree and a predicate not satisfied "
			"by the root, it returns an empty collection") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		CHECK( select_all<decltype(predicate)>(root, predicate).empty() );
	}

	TEST_CASE("select_all: given a simple tree and a predicate satisfied by "
			"the root, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		vector<tref> expected {root};
		CHECK( select_all<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -the right one "
			"matching the predicate-, it returns the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		vector<tref> expected {n('c')};
		CHECK( select_all<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -the left one "
			"matching the predicate-, it returns the matching child") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		vector<tref> expected {n('b')};
		CHECK( select_all<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -both matching the "
			"predicate-, it returns both") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		vector<tref> expected { n('b'), n('c') };
		CHECK( select_all<decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -both matching the "
			"predicate-, it returns both") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		vector<tref> expected { n('b'), n('c') };
		CHECK( select_all<decltype(predicate)>(root, predicate) == expected );
	}
}

TEST_SUITE("find_top") {

	TEST_CASE("find_top: given a simple tree and a predicate not satisfied by "
			"the root, it returns an empty optional") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		CHECK( !find_top<char, decltype(predicate)>(root, predicate) );
	}

	TEST_CASE("find_top: given a simple tree and a predicate satisfied by the "
			"root, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		tref expected {root};
		CHECK( find_top<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -the right one matching "
			"the predicate-, it returns the matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		tref expected {n('c')};
		CHECK( find_top<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -the left one matching "
			"the predicate-, it returns the matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		tref expected {n('b')};
		CHECK( find_top<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -both matching the "
			"predicate-, it returns the first matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		tref expected { n('b') };
		CHECK( find_top<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with an underlying diamond like DAG "
			"and a predicate satisfied by the bottom nodes, it returns the "
			"first matching node") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
						|| chtree::get(n).value == 'c'
						|| chtree::get(n).value == 'd';};
		tref expected { n('b', {n('d')}) };
		CHECK( find_top<char, decltype(predicate)>(root, predicate) == expected );
	}
}

// TODO (LOW) write tests for while_not_found_predicate
// TODO (LOW) write tests for find_visitor

TEST_SUITE("find_bottom") {

	TEST_CASE("find_bottom: given a simple tree and a predicate not satisfied by "
			"the root, it returns an empty optional") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		CHECK( !find_bottom<char, decltype(predicate)>(root, predicate) );
	}

	TEST_CASE("find_bottom: given a simple tree and a predicate satisfied by the "
			"root, it returns the root") {
		tref root = n('a');
		auto predicate = [](tref n) { return chtree::get(n).value == 'a'; };
		tref expected {root};
		CHECK( find_bottom<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_bottom: given a tree with two children -the right one matching "
			"the predicate-, it returns the matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'c'; };
		tref expected {n('c')};
		CHECK( find_bottom<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_bottom: given a tree with two children -the left one matching "
			"the predicate-, it returns the matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'; };
		tref expected {n('b')};
		CHECK( find_bottom<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_bottom: given a tree with two children -both matching the "
			"predicate-, it returns the first matching node") {
		tref root = n('a', {n('b'), n('c')});
		auto predicate = [](tref n) { return chtree::get(n).value == 'b'
					|| chtree::get(n).value == 'c'; };
		tref expected = n('b');
		CHECK( find_bottom<char, decltype(predicate)>(root, predicate) == expected );
	}

	TEST_CASE("find_bottom: given a tree with an underlying diamond like DAG "
			"and a predicate satisfied by the bottom nodes, it returns the "
			"first matching node") {
		tref root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](tref n) {
			auto v = chtree::get(n).value;
			return v == 'b' || v == 'c' || v == 'd';};
		tref expected { n('d') };
		CHECK( find_bottom<char, decltype(predicate)>(root, predicate) == expected );
	}
}

TEST_SUITE("pattern_matcher") {

	struct is_capture_predicate {

		bool operator()(const tref& n) const {
			auto v = chtree::get(n).value;
			return v == 'X' || v == 'Y' || v == 'Z';
		}
	};

	static auto is_capture = is_capture_predicate();

	TEST_CASE("pattern_matcher: given a simple tree and a simple capture, it "
			"returns a environment with the capture") {
		tref root = n('a');
		tref pattern = n('X');
		environment expected { {pattern, root} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a simple tree and a simple ignore, it "
			"returns an empty environment") {
		tref root = n('a');
		tref pattern = n('I');
		environment expected;
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( !matcher.matched );
	}

	TEST_CASE("pattern_matcher: given a tree with two children, a same "
			"structure tree pattern -both of them captures-,"
			" it returns a environment with both captures") {
		tref root = n('a', {n('b'), n('c')});
		tref pattern = n('a', {n('X'), n('Y')});
		environment expected { {n('X'), n('b')}, {n('Y'), n('c')} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two equal children and a "
			"same structure pattern -both of them the same captures-,"
			", it returns a environment with one capture") {
		tref root = n('a', {n('b'), n('b')});
		tref pattern = n('a', {n('X'), n('X')});
		environment expected { {n('X'), n('b')} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two different children and a "
			"same structure pattern -both of them the same captures-,"
			", it returns a environment with one capture") {
		tref root = n('a', {n('b'), n('c')});
		tref pattern = n('a', {n('X'), n('X')});
		environment expected { };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( !matcher.matched );
		CHECK( matcher.env == expected);
	}
}

TEST_SUITE("pattern_matcher") {

	struct is_capture_predicate {

		bool operator()(const tref& n) const {
			auto v = chtree::get(n).value;
			return v == 'X' || v == 'Y' || v == 'Z';
		}
	};

	static auto is_capture = is_capture_predicate();

	TEST_CASE("pattern_matcher: given a simple tree and a simple "
			"capture, it returns a environment with the capture") {
		tref root = n('a');
		tref pattern = n('X');
		environment expected { {pattern, root} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a simple tree and a simple "
			"ignore, it returns an empty environment") {
		tref root = n('a');
		tref pattern = n('I');
		environment expected;
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( !matcher.matched );
	}

	TEST_CASE("pattern_matcher: given a tree with two children, a "
			"same structure tree pattern -both of them captures-, it returns "
			"a environment with both captures") {
		tref root = n('a', {n('b'), n('c')});
		tref pattern = n('a', {n('X'), n('Y')});
		environment expected { {n('X'), n('b')}, {n('Y'), n('c')} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two equal children "
			"and a same structure pattern -both of them the same captures-, it "
			"returns a environment with one capture") {
		tref root = n('a', {n('b'), n('b')});
		tref pattern = n('a', {n('X'), n('X')});
		environment expected { {n('X'), n('b')} };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two different "
			"children and a same structure pattern -both of them the same captures-,"
			" it returns a environment with one capture") {
		tref root = n('a', {n('b'), n('c')});
		tref pattern = n('a', {n('X'), n('X')});
		environment expected { };
		environment matched;
		auto matcher = pattern_matcher<char,
			is_capture_predicate>(pattern, matched, is_capture);
		matcher(root);
		CHECK( !matcher.matched );
		CHECK( matcher.env == expected);
	}
}

TEST_SUITE("apply") {

	struct is_capture_predicate {

		bool operator()(const tref& n) const {
			auto v = chtree::get(n).value;
			return v == 'X' || v == 'Y' || v == 'Z';
		}
	};

	TEST_CASE("apply: given tree with one child and a environment that "
	 		"transform a node with one children into two, it returns the "
			"tree with the environment applied") {
		tref root {n('a', {n('b')})};
		tref pattern {n('a', {n('X')})};
		tref env {n('a', {n('X'), n('X')})};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a', {n('b'), n('b')});
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with one child and a environment that "
			"transform that ignore the children node and replace the root node , "
			"it returns the tree with the environment applied") {
		tref root {n('a', {n('b')})};
		tref pattern {n('a', {n('X')})};
		tref env {n('a')};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a');
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		tref root {n('a', {n('b'), n('c')})};
		tref pattern {n('a', {n('X'), n('Y')})};
		tref env {n('a', {n('Y'), n('X')})};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a', {n('c'), n('b')});
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		tref root {n('a', {n('b'), n('c', {n('d'), n('e')})})};
		tref pattern {n('a', {n('X'), n('c', {n('Y'), n('Z')})})};
		tref env {n('a', {n('Y'), n('c', {n('Z'), n('X')})})};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a', {n('d'), n('c', {n('e'), n('b')})});
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given a tree with a diamond like DAG and a environment "
			"that breaks the diamond like shape, it returns the tree with the "
			"environment applied") {
		tref root {n('a', {n('b', {n('d')}), n('c', {n('d')})})};
		tref pattern {n('b', {n('X')})};
		tref env {n('b', {n('e')})};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a', {n('b', {n('e')}), n('c', {n('d')})});
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given a tree with a diamond like DAG and a environment "
			"that swaps the intermediate children, it returns the tree with the "
			"environment applied") {
		tref root {n('a', {n('b', {n('d')}), n('c', {n('d')})})};
		tref pattern {n('a', {n('X'), n('Y')})};
		tref env {n('a', {n('Y'), n('X')})};
		rule r{ chtree::geth(pattern), chtree::geth(env) };
		tref expected = n('a', {n('c', {n('d')}), n('b', {n('d')})});
		is_capture_predicate is_capture;
		auto replaced = apply_rule<char, is_capture_predicate>(r, root, is_capture);
		CHECK( replaced == expected );
	}
}
