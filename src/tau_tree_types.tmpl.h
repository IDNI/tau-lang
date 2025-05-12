// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "ref_types.h"
// #include "ba_types_checker_and_propagator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "types"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA type inference

template <NodeType node>
tref infer_ba_types(tref n) {
	using tau = tree<node>;
	LOG_TRACE_T("Infer BA types"); LOG_TRACE_F(n);
	// static typename node::ba_types_checker_and_propagator infer;
	// if ((n = infer(n)) == nullptr) return nullptr;
	LOG_TRACE_T("Inferred BA types"); LOG_TRACE_F(n);
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
	LOG_DEBUG_I("Begin ref type inferrence"); LOG_DEBUG_RR(nso_rr);
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
			// LOG_TRACE_T(r.second << " is " << node::name(t));
			if (t == tau::ref) {
				// right side is unresolved ref
				if (auto topt = ts.get(get_rr_sig<node>(body));
					topt.has_value())
				{
					t = topt.value();
					// LOG_TRACE_T("updating right side")
					// 	<< r.second;
					update_ref(body, t);
					changed = true;
				}
			}
			// update left side if right side is known
			if (t == tau::bf || t == tau::wff) {
				if (get_nt_type(head) == tau::ref) {
					// left side is unresolved ref
					// LOG_TRACE_T("updating left side")
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
						// LOG_TRACE_T("updating left side")
						// 	<< r.first;
						ts.done(get_rr_sig<node>(head));
						update_ref(head, t);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (t == tau::bf || t == tau::wff) {
					// LOG_TRACE_T("updating capture")
					// 	<< r.second;
					body = tau::get(t, body);
				}
			}
		}
	} while (changed); // inference fixed point

	// infer main if unresolved ref
	if (nn.main) {
		auto t = get_nt_type(nn.main->get());
		// LOG_TRACE_T("main (" << t.to_str() << ") is " << node::name(t));
		if (t == tau::ref) {
			// main is an unresolved ref
			if (auto topt = ts.get(get_rr_sig<node>(nn.main->get())); topt) {
				t = topt.value();
				// LOG_TRACE_T("updating main: " << TAU_TO_STR(nn.main));
				update_ref(nn.main->get(), t);
			}
		}
	}

	for (const auto& err : ts.errors()) LOG_ERROR << err;
	if (ts.errors().size()) return {};

	if (const auto& unresolved = ts.unresolved(); unresolved.size()) {
		std::stringstream ss;
		for (const auto& sig : unresolved) ss << " " << sig;
		LOG_ERROR << "Unknown ref type for:" << ss.str();
		return {};
	}
	LOG_DEBUG_I("End ref type inferrence");
	if (nso_rr != nn) LOG_DEBUG_RR(nn);
	return { nn };
}

} // namespace idni::tau_lang