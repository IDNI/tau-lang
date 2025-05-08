// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "ref_types.h"
// #include "ba_types_checker_and_propagator.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA type inference

template <NodeType node>
tref infer_ba_types(tref n) {
	BOOST_LOG_TRIVIAL(trace) << "(T) infer BA types: " << tree<node>::get(n);
	// static typename node::ba_types_checker_and_propagator_t infer;
	// if ((n = infer(n)) == nullptr) return nullptr;
	BOOST_LOG_TRIVIAL(trace) << "(T) inferred BA types: " << tree<node>::get(n);
	return n;
}

//------------------------------------------------------------------------------
// rec relations type checking and inference

template <NodeType node>
std::optional<rr> infer_ref_types(const rr& nso_rr) {
	ref_types<node> ts(nso_rr);
	return infer_ref_types(nso_rr, ts);
}

template <NodeType node>
std::optional<rr> infer_ref_types(const rr& nso_rr,
	ref_types<node>& ts)
{
	using tau = tree<node>;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin ref type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	static auto get_nt_type = [](tref r) -> typename node::type {
                return tree<node>::get(r).get_type();
	};
	static auto update_ref = [](tref r, const node::type& t) {
		//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
		r = tau::get(t, tau::get(t == tau::wff ? tau::wff_ref
                                                : tau::bf_ref, r));
		//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
	};
	rr nn = nso_rr;
	// inference loop
	bool changed;
	do {
		changed = false;
		for (auto& r : nn.rec_relations) {
                        tref head = r.first->get();
                        tref body = r.second->get();
			// check type of the right side
			typename node::type t = get_nt_type(body);
			// BOOST_LOG_TRIVIAL(trace) << "(T) " << r.second
			// 			<< " is " << (node::name(t));
			if (t == tau::ref) {
				// right side is unresolved ref
				if (auto topt = ts.get(get_rr_sig<node>(body));
					topt.has_value())
				{
					t = topt.value();
					// BOOST_LOG_TRIVIAL(trace)
					// 	<< "(T) updating right side: "
					// 	<< r.second;
					update_ref(body, t);
					changed = true;
				}
			}
			// update left side if right side is known
			if (t == tau::bf || t == tau::wff) {
				if (get_nt_type(head) == tau::ref) {
					// left side is unresolved ref
					// BOOST_LOG_TRIVIAL(trace)
					// 	<< "(T) updating left side: "
					// 	<< r.first;
					ts.add(head, t);
					update_ref(head, t);
					changed = true;
				}
			}
			// infer capture's type from the left side if known
			if (t == tau::capture) {
				t = get_nt_type(head);
				// left side is an unresolved ref
				if (t == tau::ref) {
					auto topt = ts.get(get_rr_sig<node>(head));
					if (topt.has_value()) { // if we know
						t = topt.value(); // update
						// BOOST_LOG_TRIVIAL(trace)
						// 	<< "(T) updating left "
						// 	"side: " << r.first;
						ts.done(get_rr_sig<node>(head));
						update_ref(head, t);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (t == tau::bf || t == tau::wff) {
					// BOOST_LOG_TRIVIAL(trace)
					// 	<< "(T) updating capture: "
					// 	<< r.second;
					//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
					body = tau::get(t, body);
					//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
				}
			}
		}
	} while (changed); // inference fixed point

	// infer main if unresolved ref
	if (nn.main) {
		auto t = get_nt_type(nn.main->get());
		// BOOST_LOG_TRIVIAL(trace) << "(T) main " << nn.main
		// 				<< " is " << (node::name(t));
		if (t == tau::ref) {
			// main is an unresolved ref
			if (auto topt = ts.get(get_rr_sig<node>(nn.main->get())); topt) {
				t = topt.value();
				// BOOST_LOG_TRIVIAL(trace)
				// 	<< "(T) updating main: " << nn.main;
				update_ref(nn.main->get(), t);
			}
		}
	}

	for (const auto& err : ts.errors())
		BOOST_LOG_TRIVIAL(error) << "(Error) " << err;
	if (ts.errors().size()) return {};

	if (const auto& unresolved = ts.unresolved(); unresolved.size()) {
		std::stringstream ss;
		for (const auto& sig : unresolved) ss << " " << sig;
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unknown ref type for:" << ss.str();
		return {};
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End ref type inferrence"; // << ": " << nn;
	return { nn };
}

} // namespace idni::tau_lang