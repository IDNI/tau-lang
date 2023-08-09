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

#include "../src/doctest.h"
#include "../src/rewriting.h"

using namespace idni::rewriter;
using namespace std;

namespace testing = doctest;

// TODO move helper methods to a different file

sp_node<char> n(const char& value) {
	return make_node<char>(value, {});
}

sp_node<char> n(const char& value, const vector<sp_node<char>>& child) {
	return make_node<char>(value, child);
}

sp_node<char> d(const char& value) {
	vector<sp_node<char>> children;
	return std::make_shared<node<char>>(value, children);
}

sp_node<char> d(const char& value, const vector<sp_node<char>>& child) {
	return std::make_shared<node<char>>(value, child);
}

// tests suites

TEST_SUITE("node") {
	
	TEST_CASE("node constructor: given a value, the node has that value") {
		auto d1 = *d('a');
		CHECK( d1.value == 'a' );
		CHECK( d1.child.size() == 0 );
	}
	
	TEST_CASE("node constructor: given a value and children, the node has that value "
			"and children") {
		auto d1 = *d('a', {d('b'), d('c'), d('d')});
		CHECK( d1.value == 'a' );
		CHECK( d1.child.size() == 3 );
		CHECK( d1.child[0]->value == 'b' );
		CHECK( d1.child[1]->value == 'c' );
		CHECK( d1.child[2]->value == 'd' );
	}
	TEST_CASE("node order: given two simple nodes, the order is given by the order "
			"of the syms") {
		CHECK( *d('a') < *d('b') );
	}

	TEST_CASE("node order: given two nodes with same value and different children, "
			"one is bigger than the other") {
		auto d1 = *d('a', {d('b')});
		auto d2 = *d('a', {d('b'), d('c')});
		CHECK( ((d1 < d2) || (d2 < d1)) );
	}

	TEST_CASE("node order: given two nodes with the same value and children, neither "
			"of them is bigger than the other") {
		auto d1 = *d('a', {n('b'), n('c'), n('d')});
		auto d2 = *d('a', {n('b'), n('c'), n('d')});
		CHECK( (!(d1 < d2) && !(d2 < d1)) );
	}

	TEST_CASE("node equality: given two simple nodes with the same value, they "
			"are equal") {
		CHECK( *d('a') == *d('a') );
	}

	TEST_CASE("node equality: given two nodes with the same value and children, "
			"they are equal") {
		CHECK( *d('a', {n('b'), n('c'), n('d')}) == *d('a', {n('b'), n('c'), n('d')}) );
	}

	TEST_CASE("node equality: given two simple nodes with different syms, they "
			"are not equal") {
		CHECK( *d('a') != *d('b') );
	}

	TEST_CASE("node equality: given two nodes with different syms and same children, "
			"they are not equal") {
		CHECK( *d('a', {n('b'), n('c'), n('d')}) != *d('b', {n('b'), n('c'), n('d')}) );
	}

	TEST_CASE("node equality: given two nodes with same syms and different children, "
			"they are not equal") {
		CHECK(  *d('a', {d('b'), d('c'), d('d')}) !=  *d('a', {d('b'), d('c'), d('d')}) );
	}
}

TEST_SUITE("make_node") {

	TEST_CASE("make_node uniqueness: given two simple nodes with the same value, it "
			"returns the same node") {
		auto n1 = make_node<char>('a', {});
		auto n2 = make_node<char>('a', {});
		CHECK( n1 == n2 );		
	}

	TEST_CASE("make_node uniqueness: given two nodes with the same value and children, "
			"it returns the same node") {
		auto n1 = make_node<char>('a', {n('b'), n('c'), n('d')});
		auto n2 = make_node<char>('a', {n('b'), n('c'), n('d')});
		CHECK( n1 == n2 );		
	}

	TEST_CASE("make_node uniqueness: given two nodes with different value and same "
			"children, it returns the different nodes") {
		auto n1 = make_node<char>('a', {n('b'), n('c'), n('d')});
		auto n2 = make_node<char>('b', {n('b'), n('c'), n('d')});
		CHECK( n1 != n2 );		
	}

	TEST_CASE("make_node uniqueness: given two nodes with same value and different "
			"children, it returns the different nodes") {
		auto n1 = make_node<char>('a', {n('b'), n('c'), n('d')});
		auto n2 = make_node<char>('a', {n('b'), n('c'), n('e')});
		CHECK( n1 != n2 );		
	}
}

TEST_SUITE("rewriting transformer_identity") {

}

TEST_SUITE("post_order_traverser") {

	template <typename node_t>
	struct collect_predicate {
		vector<node_t> nodes;
		bool operator()(node_t n) {
			nodes.push_back(n);
			return true;
		}
	};

	template <typename wrapped_t, typename node_t>
	struct collect_visitor {

		collect_visitor(wrapped_t& wrapped) : wrapped(wrapped) { }

		node_t operator()(node_t n) {
			auto nn = wrapped(n);
			nodes.push_back(nn);
			return nn;
		}

		vector<node_t> nodes;
		wrapped_t& wrapped;
	};

	template <typename node_t>
	struct identity_visitor {

		node_t operator()(node_t n) {
			return n;
		}
	};

	TEST_CASE("post_order_traverser: given a simple tree, it visits the root node") {
		auto root = n('a');
		identity_visitor<sp_node<char>> identity;
		collect_visitor<identity_visitor<sp_node<char>>, sp_node<char>> visited{identity};
		vector<sp_node<char>> expected {root};
		post_order_traverser<decltype(visited), decltype(all<sp_node<char>>), sp_node<char>>(visited , all<sp_node<char>>)(root);
		CHECK( visited.nodes == expected );
	}

	TEST_CASE("post_order_traverser: given a tree with two children, it visits "
			"the children and then the root") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		identity_visitor<sp_node<char>> identity;
		collect_visitor<identity_visitor<sp_node<char>>, sp_node<char>> visited{identity};
		vector<sp_node<char>> expected {n('b'), n('c'), root};
		post_order_traverser<decltype(visited), decltype(all<sp_node<char>>), sp_node<char>>(visited , all<sp_node<char>>)(root);
		CHECK( visited.nodes == expected );
	}

	TEST_CASE("post_order_traversal: given tree with underlying diamond like DAG, "
			"it visits the children and then the root") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		identity_visitor<sp_node<char>> identity;
		collect_visitor<identity_visitor<sp_node<char>>, sp_node<char>> visited{identity};
		vector<sp_node<char>> expected {n('d'), n('b', {n('d')}), n('c', {n('d')}), root};
		post_order_traverser<decltype(visited), decltype(all<sp_node<char>>), sp_node<char>>(visited , all<sp_node<char>>)(root);
		CHECK( visited.nodes == expected );
	}
}

TEST_SUITE("map_transformer") {

	TEST_CASE("map_transformer: given a simple tree and a visitor, it returns a "
			"new tree with the changes applied by the visitor") {
		sp_node<char> root = n('a');
		auto transform = [](char c) { return c == 'a' ? 'z' : c; };
		map_transformer<decltype(transform), sp_node<char>> map{transform};
		sp_node<char> expected { n('z') };
		auto result = post_order_traverser<decltype(map), decltype(all<sp_node<char>>), sp_node<char>>(map , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}

	TEST_CASE("map_transformer: given a tree with two children and a visitor, "
			"it returns a new tree with the changes applied by the visitor") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto transform = [](char c) { return c == 'b' ? 'z' : c; };
		map_transformer<decltype(transform), sp_node<char>> map{transform};
		sp_node<char> expected { n('a', {n('z'), n('c')}) };
		auto result = post_order_traverser<decltype(map), decltype(all<sp_node<char>>), sp_node<char>>(map , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}

	TEST_CASE("map_transformer: given a tree with underlying diamond like DAG "
			"and a visitor, it returns a new tree with the changes applied by "
			"the visitor") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto transform = [](char c) { return c == 'd' ? 'z' : c; };
		map_transformer<decltype(transform), sp_node<char>> map{transform};
		sp_node<char> expected { n('a', {n('b', {n('z')}), n('c', {n('z')})}) };
		auto result = post_order_traverser<decltype(map), decltype(all<sp_node<char>>), sp_node<char>>(map , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}
}

TEST_SUITE("replace_transformer") {

	TEST_CASE("replace_transformer: given a simple tree and a visitor, it returns "
			"a new tree with the provided replacements") {
		sp_node<char> root = n('a');
		map<sp_node<char>, sp_node<char>> m;
		m[root] = n('z');
		replace_transformer<sp_node<char>> replace{m};
		sp_node<char> expected { n('z') };
		auto result = post_order_traverser<decltype(replace), decltype(all<sp_node<char>>), sp_node<char>>(replace , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}

	TEST_CASE("replace_transform: given a tree with two children and a visitor, "
			"it returns a new tree with the provided replacements") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		map<sp_node<char>, sp_node<char>> m;
		m[n('b')] = n('z');
		replace_transformer<sp_node<char>> replace{m};
		sp_node<char> expected { n('a', {n('z'), n('c')}) };
		auto result = post_order_traverser<decltype(replace), decltype(all<sp_node<char>>), sp_node<char>>(replace , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}

	TEST_CASE("replace_transform: given a tree with underlying diamond like DAG "
			"and a visitor, it returns a new tree with the provided replacements") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		map<sp_node<char>, sp_node<char>> m;
		m[n('d')] = n('z');
		replace_transformer<sp_node<char>> replace{m};
		sp_node<char> expected { n('a', {n('b', {n('z')}), n('c', {n('z')})}) };
		auto result = post_order_traverser<decltype(replace), decltype(all<sp_node<char>>), sp_node<char>>(replace , all<sp_node<char>>)(root);
		CHECK( result == expected );
	}
	// TODO add the tests corresponding to related functions
}

TEST_SUITE("select_top_predicate") {

	TEST_CASE("select_top_predicate: given a simple tree whose root satisfies the "
			"predicate, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {root};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty vector") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("selected_top_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the children satisfying the predicate") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('b'), n('c')};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty vector satisfying the "
			"predicate") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'd' || n->value == 'e'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector with only one bottom node "
			"satisfying the predicate") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'd'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('d')};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector the two top nodes satisfying "
			"the predicate") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('b', {n('d')}), n('c', {n('d')})};
		select_top_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	// TODO add the tests corresponding to related functions
}

TEST_SUITE("select_all_predicate") {

	TEST_CASE("select_all_predicate: given a simple tree whose root satisfies "
			"the predicate, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {root};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}
	
	TEST_CASE("select_all_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty vector") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the children satisfying the predicate") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('b'), n('c')};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty vector satisfying the "
			"predicate") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'd' || n->value == 'e'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector with only one bottom node "
			"satisfying the predicate") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'd'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('d')};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}

	TEST_CASE("select_all_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns a vector the two top nodes satisfying "
			"the predicate") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> selected;
		vector<sp_node<char>> expected {n('b', {n('d')}), n('c', {n('d')})};
		select_all_predicate<decltype(predicate), sp_node<char>> select{predicate, selected};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(select), sp_node<char>>(identity , select)(root);		
		CHECK( selected == expected );
	}
}

TEST_SUITE("find_top_predicate") {

	TEST_CASE("find_top_predicate: given a simple tree whose root satisfies the "
			"predicate, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		optional<sp_node<char>> found;
		sp_node<char> expected {root};
		find_top_predicate<decltype(predicate), sp_node<char>> find{predicate, found};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(find), sp_node<char>>(identity, find)(root);		
		CHECK( found.value() == expected );
	}

	TEST_CASE("find_top_predicate: given a simple tree whose root does not "
			"satisfy the predicate, it returns an empty optional") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		optional<sp_node<char>> found;
		find_top_predicate<decltype(predicate), sp_node<char>> find{predicate, found};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(find), sp_node<char>>(identity, find)(root);		
		CHECK( !found );
	}

	TEST_CASE("find_top_predicate: given a tree with two children that satisfy "
			"the predicate, it returns the first child satisfying the predicate") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		optional<sp_node<char>> found;
		sp_node<char> expected {n('b')};
		find_top_predicate<decltype(predicate), sp_node<char>> find{predicate, found};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(find), sp_node<char>>(identity, find)(root);		
		CHECK( found.value() == expected );
	}

	TEST_CASE("find_top_predicate: given a tree with two children that do not "
			"satisfy the predicate, it returns an empty optional") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'd' || n->value == 'e'; };
		optional<sp_node<char>> found;
		find_top_predicate<decltype(predicate), sp_node<char>> find{predicate, found};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(find), sp_node<char>>(identity, find)(root);		
		CHECK( !found );
	}

	TEST_CASE("find_top_predicate: given a tree with underlying diamond like "
			"DAG and a visitor, it returns the top node satisfying the predicate") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		optional<sp_node<char>> found;
		sp_node<char> expected {n('c', {n('d')})};
		find_top_predicate<decltype(predicate), sp_node<char>> find{predicate, found};
		identity_transformer<sp_node<char>> identity;
		post_order_traverser<decltype(identity), decltype(find), sp_node<char>>(identity, find)(root);		
		CHECK( found.value() == expected );
	}
}

TEST_SUITE("logical predicates") {

	TEST_CASE("true_predicate: given a node, it always returns true") {
		auto t = true_predicate<sp_node<char>>(); 
		CHECK( t(n('a')) );
	}

	TEST_CASE("false_predicate: given a node, it always returns false") {
		auto f = false_predicate<sp_node<char>>(); 
		CHECK( !f(n('a')) );
	}

	TEST_CASE("and_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate<sp_node<char>>(); 
		auto f = false_predicate<sp_node<char>>(); 
		CHECK( and_predicate(t, t)(d('a')) );
		CHECK( !and_predicate(t, f)(d('a')) );
		CHECK( !and_predicate(f, t)(d('a')) );
		CHECK( !and_predicate(f, f)(d('a')) );
	}

	TEST_CASE("or_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate<sp_node<char>>(); 
		auto f = false_predicate<sp_node<char>>(); 
		CHECK( or_predicate(t, t)(d('a')) );
		CHECK( or_predicate(t, f)(d('a')) );
		CHECK( or_predicate(f, t)(d('a')) );
		CHECK( !or_predicate(f, f)(d('a')) );
	}

	TEST_CASE("neg_predicate: given a true and a false predicate, it computes "
			"the true table") {
		auto t = true_predicate<sp_node<char>>(); 
		auto f = false_predicate<sp_node<char>>(); 
		CHECK( !neg_predicate(t)(d('a')) );
		CHECK( neg_predicate(f)(d('a')) );
	}
}

TEST_SUITE("trim_top") {

	TEST_CASE("trim_top: given a simple tree and a predicate not satisfied by "
			"the root, it returns the tree itself whatever the predicate") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		sp_node<char> expected {root};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a simple tree and a predicate satisfied by the "
			"root, it returns the tree itself whatever the predicate") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		sp_node<char> expected {root};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -the right one matching "
			"the predicate-, it returns the tree without the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		sp_node<char> expected {n('a', {n('b')})};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with two children -the left one matching "
			"the predicate-, it returns the tree without the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		sp_node<char> expected {n('a', {n('c')})};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}	

	TEST_CASE("trim_top: given a tree with two children -both matching the "
			"predicate-, it returns the root") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		sp_node<char> expected { n('a') }; 
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}	

	TEST_CASE("trim_top: given a tree with two children -none matching the "
			"predicate-, it returns the given tree") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'd' || n->value == 'e'; };
		sp_node<char> expected { root }; 
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with underlying diamond like DAG and a "
			"predicate not satisfied by the nodes, it returns the tree itself") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'e'; };
		sp_node<char> expected {root};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("trim_top: given a tree with underlying diamond like DAG and a "
			"predicate satisfied by the bottom, it returns the tree without it") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'd'; };
		sp_node<char> expected {n('a', {n('b'), n('c')})};
		CHECK( trim_top<decltype(predicate), char>(root, predicate) == expected );
	}
}

TEST_SUITE("select_top") {

	TEST_CASE("select_top: given a simple tree and a predicate not satisfied "
			"by the root, it returns the an empty collection") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		CHECK( select_top<decltype(predicate), char>(root, predicate).empty() );
	}

	TEST_CASE("select_top: given a simple tree and a predicate satisfied by "
			"the root, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		vector<sp_node<char>> expected {root};
		CHECK( select_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with two children -the right one "
			"matching the predicate-, it returns the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		vector<sp_node<char>> expected {n('c')};
		CHECK( select_top<decltype(predicate), char>(root, predicate) == expected );
	}	

	TEST_CASE("select_top: given a tree with two children -the left one "
			"matching the predicate-, it returns the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		vector<sp_node<char>> expected {n('b')};
		CHECK( select_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_top: given a tree with two children -both matching "
			"the predicate-, it returns the left one") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> expected { n('b'), n('c') }; 
		CHECK( select_top<decltype(predicate), char>(root, predicate) == expected );
	}	

	TEST_CASE("select_top: given a tree with underlying diamond like DAG and "
			"a predicate satisfied by the bottom, it returns the bottom") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { return n->value == 'd'; };
		vector<sp_node<char>> expected {n('d')};
		CHECK( select_top<decltype(predicate), char>(root, predicate) == expected );
	}
}

TEST_SUITE("select_all") {

	TEST_CASE("select_all: given a simple tree and a predicate not satisfied "
			"by the root, it returns an empty collection") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		CHECK( select_all<decltype(predicate), char>(root, predicate).empty() );
	}

	TEST_CASE("select_all: given a simple tree and a predicate satisfied by "
			"the root, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		vector<sp_node<char>> expected {root};
		CHECK( select_all<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -the right one "
			"matching the predicate-, it returns the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		vector<sp_node<char>> expected {n('c')};
		CHECK( select_all<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -the left one "
			"matching the predicate-, it returns the matching child") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		vector<sp_node<char>> expected {n('b')};
		CHECK( select_all<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -both matching the "
			"predicate-, it returns both") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> expected { n('b'), n('c') }; 
		CHECK( select_all<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("select_all: given a tree with two children -both matching the "
			"predicate-, it returns both") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		vector<sp_node<char>> expected { n('b'), n('c') }; 
		CHECK( select_all<decltype(predicate), char>(root, predicate) == expected );
	}
}

TEST_SUITE("find_top") {

	TEST_CASE("find_top: given a simple tree and a predicate not satisfied by "
			"the root, it returns an empty optional") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		CHECK( !find_top<decltype(predicate), char>(root, predicate) );
	}

	TEST_CASE("find_top: given a simple tree and a predicate satisfied by the "
			"root, it returns the root") {
		sp_node<char> root = n('a');
		auto predicate = [](sp_node<char> n) { return n->value == 'a'; };
		optional<sp_node<char>> expected {root};
		CHECK( find_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -the right one matching "
			"the predicate-, it returns the matching node") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'c'; };
		optional<sp_node<char>> expected {n('c')};
		CHECK( find_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -the left one matching "
			"the predicate-, it returns the matching node") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b'; };
		optional<sp_node<char>> expected {n('b')};
		CHECK( find_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with two children -both matching the "
			"predicate-, it returns the first matching node") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		auto predicate = [](sp_node<char> n) { return n->value == 'b' || n->value == 'c'; };
		optional<sp_node<char>> expected { n('b') }; 
		CHECK( find_top<decltype(predicate), char>(root, predicate) == expected );
	}

	TEST_CASE("find_top: given a tree with an underlying diamond like DAG "
			"and a predicate satisfied by the bottom nodes, it returns the "
			"first matching node") {
		sp_node<char> root = n('a', {n('b', {n('d')}), n('c', {n('d')})});
		auto predicate = [](sp_node<char> n) { 
			return n->value == 'b' || n->value == 'c' || n->value == 'd'; };
		optional<sp_node<char>> expected { n('b', {n('d')}) }; 
		CHECK( find_top<decltype(predicate), char>(root, predicate) == expected );
	}
}

TEST_SUITE("pattern_matcher") {

	struct is_capture_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'X' || n->value == 'Y' || n->value == 'Z';
		}
	};

	struct is_ignore_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'I';
		}
	};
	
	static auto is_ignore = is_ignore_predicate();
	static auto is_capture = is_capture_predicate();

	TEST_CASE("pattern_matcher: given a simple tree and a simple capture, it "
			"returns a environment with the capture") {
		sp_node<char> root = n('a');
		sp_node<char> pattern = n('X');
		environment<sp_node<char>> expected { {pattern, root} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher(pattern, matched, is_ignore, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}
	
	TEST_CASE("pattern_matcher: given a simple tree and a simple ignore, it "
			"returns an empty environment") {
		sp_node<char> root = n('a');
		sp_node<char> pattern = n('I');
		environment<sp_node<char>> expected;
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher(pattern, matched, is_ignore, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two children, a same "
			"structure tree pattern -both of them captures-,"
			" it returns a environment with both captures") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		sp_node<char> pattern = n('a', {n('X'), n('Y')});
		environment<sp_node<char>> expected { {n('X'), n('b')}, {n('Y'), n('c')} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher(pattern, matched, is_ignore, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two equal children and a "
			"same structure pattern -both of them the same captures-,"
			", it returns a environment with one capture") {
		sp_node<char> root = n('a', {n('b'), n('b')});
		sp_node<char> pattern = n('a', {n('X'), n('X')});
		environment<sp_node<char>> expected { {n('X'), n('b')} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher(pattern, matched, is_ignore, is_capture);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher: given a tree with two different children and a "
			"same structure pattern -both of them the same captures-,"
			", it returns a environment with one capture") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		sp_node<char> pattern = n('a', {n('X'), n('X')});
		environment<sp_node<char>> expected { };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher(pattern, matched, is_ignore, is_capture);
		matcher(root);
		CHECK( !matcher.matched );
		CHECK( matcher.env == expected);
	}
}

TEST_SUITE("pattern_matcher_with_skip") {

	struct is_capture_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'X' || n->value == 'Y' || n->value == 'Z';
		}
	};

	struct is_ignore_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'I';
		}
	};

	struct is_skip_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'S';
		}
	};
	
	static auto is_ignore = is_ignore_predicate();
	static auto is_capture = is_capture_predicate();
	static auto is_skip = is_skip_predicate();

	TEST_CASE("pattern_matcher_with_skip: given a simple tree and a simple "
			"capture, it returns a environment with the capture") {
		sp_node<char> root = n('a');
		sp_node<char> pattern = n('X');
		environment<sp_node<char>> expected { {pattern, root} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}
	
	TEST_CASE("pattern_matcher_with_skip: given a simple tree and a simple "
			"ignore, it returns an empty environment") {
		sp_node<char> root = n('a');
		sp_node<char> pattern = n('I');
		environment<sp_node<char>> expected;
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher_with_skip: given a tree with two children, a "
			"same structure tree pattern -both of them captures-, it returns "
			"a environment with both captures") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		sp_node<char> pattern = n('a', {n('X'), n('Y')});
		environment<sp_node<char>> expected { {n('X'), n('b')}, {n('Y'), n('c')} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher_with_skip: given a tree with two equal children "
			"and a same structure pattern -both of them the same captures-, it "
			"returns a environment with one capture") {
		sp_node<char> root = n('a', {n('b'), n('b')});
		sp_node<char> pattern = n('a', {n('X'), n('X')});
		environment<sp_node<char>> expected { {n('X'), n('b')} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher_with_skip: given a tree with two different "
			"children and a same structure pattern -both of them the same captures-,"
			" it returns a environment with one capture") {
		sp_node<char> root = n('a', {n('b'), n('c')});
		sp_node<char> pattern = n('a', {n('X'), n('X')});
		environment<sp_node<char>> expected { };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( !matcher.matched );
		CHECK( matcher.env == expected);
	}

	TEST_CASE("pattern_matcher_with_skip: given a tree with children (several "
			"of them satisfying is_skip an two of them not satisfying is_skip), "
			"an a similar structure tree pattern -with two to captures-, it "
			"returns a environment with both captures") {
		sp_node<char> root = n('a', {n('S'), n('b'), n('S'), n('c'), n('S')});
		sp_node<char> pattern = n('a', {n('X'), n('S'), n('S'), n('Y')});
		environment<sp_node<char>> expected { {n('X'), n('b')}, {n('Y'), n('c')} };
		environment<sp_node<char>> matched;
		auto matcher = pattern_matcher_with_skip(pattern, matched, is_ignore, 
			is_capture, is_skip);
		matcher(root);
		CHECK( matcher.matched );
		CHECK( matcher.env == expected);
	}
}

TEST_SUITE("apply") {

	struct is_capture_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'X' || n->value == 'Y' || n->value == 'Z';
		}
	};

	struct is_ignore_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'I';
		}
	};

	TEST_CASE("apply: given tree with one child and a environment that "
	 		"transform a node with one children into two, it returns the "
			"tree with the environment applied") {
		sp_node<char> root {n('a', {n('b')})};
		sp_node<char> pattern {n('a', {n('X')})};
		sp_node<char> environment {n('a', {n('X'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('b'), n('b')});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with one child and a environment that "
			"transform that ignore the children node and replace the root node , "
			"it returns the tree with the environment applied") {
		sp_node<char> root {n('a', {n('b')})};
		sp_node<char> pattern {n('a', {n('I')})};
		sp_node<char> environment {n('a')};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a');
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b'), n('c')})};
		sp_node<char> pattern {n('a', {n('X'), n('Y')})};
		sp_node<char> environment {n('a', {n('Y'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('c'), n('b')});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b'), n('c', {n('d'), n('e')})})};
		sp_node<char> pattern {n('a', {n('X'), n('c', {n('Y'), n('Z')})})};
		sp_node<char> environment {n('a', {n('Y'), n('c', {n('Z'), n('X')})})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('d'), n('c', {n('e'), n('b')})});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given a tree with a diamond like DAG and a environment "
			"that breaks the diamond like shape, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b', {n('d')}), n('c', {n('d')})})};
		sp_node<char> pattern {n('b', {n('X')})};
		sp_node<char> environment {n('b', {n('e')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('b', {n('e')}), n('c', {n('d')})});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}

	TEST_CASE("apply: given a tree with a diamond like DAG and a environment "
			"that swaps the intermediate children, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b', {n('d')}), n('c', {n('d')})})};
		sp_node<char> pattern {n('a', {n('X'), n('Y')})};
		sp_node<char> environment {n('a', {n('Y'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('c', {n('d')}), n('b', {n('d')})});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		auto replaced = apply(rule, root, is_ignore, is_capture) ;
		CHECK( replaced == expected );
	}
}

TEST_SUITE("apply_with_skip") {

	struct is_capture_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'X' || n->value == 'Y' || n->value == 'Z';
		}
	};

	struct is_ignore_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'I';
		}
	};

	struct is_skip_predicate {

		bool operator()(const sp_node<char>& n) {
			return n->value == 'S';
		}
	};

	TEST_CASE("apply_with_skip: given tree with one child and a environment that "
	 		"transform a node with one children into two, it returns the "
			"tree with the environment applied") {
		sp_node<char> root {n('a', {n('b')})};
		sp_node<char> pattern {n('a', {n('X')})};
		sp_node<char> environment {n('a', {n('X'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('b'), n('b')});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}

	TEST_CASE("apply_with_skip: given tree with one child and a environment that "
			"transform that ignore the children node and replace the root node , "
			"it returns the tree with the environment applied") {
		sp_node<char> root {n('a', {n('b')})};
		sp_node<char> pattern {n('a', {n('I')})};
		sp_node<char> environment {n('a')};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a');
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}

	TEST_CASE("apply_with_skip: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b'), n('c')})};
		sp_node<char> pattern {n('a', {n('X'), n('Y')})};
		sp_node<char> environment {n('a', {n('Y'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('c'), n('b')});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}

	TEST_CASE("apply_with_skip: given tree with two children and a environment that "
			"transform that swaps the children, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b'), n('c', {n('d'), n('e')})})};
		sp_node<char> pattern {n('a', {n('X'), n('c', {n('Y'), n('Z')})})};
		sp_node<char> environment {n('a', {n('Y'), n('c', {n('Z'), n('X')})})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('d'), n('c', {n('e'), n('b')})});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}

	TEST_CASE("apply_with_skip: given a tree with a diamond like DAG and a environment "
			"that breaks the diamond like shape, it returns the tree with the "
			"environment applied") {
		sp_node<char> root {n('a', {n('b', {n('d')}), n('c', {n('d')})})};
		sp_node<char> pattern {n('b', {n('X')})};
		sp_node<char> environment {n('b', {n('e')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('b', {n('e')}), n('c', {n('d')})});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}
	TEST_CASE("apply_with_skip: given a tree with children (several of them satisfying "
			"is_skip an two of them not satisfying is_skip), an a similar "
			"structure tree pattern -with two to captures-, it returns a "
			"environment with both captures") {
		sp_node<char> root = n('a', {n('S'), n('b'), n('S'), n('c'), n('S')});
		sp_node<char> pattern = n('a', {n('X'), n('S'), n('S'), n('Y')});
		sp_node<char> environment {n('a', {n('Y'), n('X')})};
		rule<sp_node<char>> rule {pattern, environment};
		sp_node<char> expected = n('a', {n('c'), n('b')});
		is_ignore_predicate is_ignore;
		is_capture_predicate is_capture;
		is_skip_predicate is_skip;
		auto replaced = apply_with_skip(rule, root, is_ignore, is_capture, is_skip);
		CHECK( replaced == expected );
	}
}

// TODO write tests for find_bottom
// TODO write tests for find_visitor
// TODO write tests for while_not_found
