// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ref_types_inference.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ref_types_inference"

namespace idni::tau_lang {

std::ostream& operator<<(std::ostream& os, const rr_sig& s);

template <NodeType node>
ref_types<node>::ref_types(const rr<node>& nso_rr) { get_ref_types(nso_rr); }

// returns false if any error or unresolved ref
template <NodeType node>
bool ref_types<node>::ok() const { return errors_.empty() && unresolved().empty(); }

// returns set of errors
template <NodeType node>
const std::set<std::string>& ref_types<node>::errors() const { return errors_; }

// returns set of unresolved refs
template <NodeType node>
std::set<rr_sig> ref_types<node>::unresolved() const {
	std::set<rr_sig> unres(todo_);
	std::erase_if(unres, [this](const rr_sig& x) {
		return fpcalls_.contains(x);
	});
	return unres;
}

// returns known type of a ref, or no value
template <NodeType node>
std::optional<typename node::type> ref_types<node>::get(
	const rr_sig& sig)
{
	rr_sig s(sig);
	auto fpopt = fpcall(s);
	if (fpopt) s = fpopt.value();
	if (auto it = types_.find(s); it != types_.end()) {
		LOG_TRACE << "Looking for type of " << s << " found "
							<< LOG_NT(it->second);
		return { it->second };
	}
	LOG_TRACE << "Looking for type of " << LOG_RR_SIG(sig) << " failed";
	return {};
}

// returns ref to calculate fp by provided by fp call sig, or no value
template <NodeType node>
std::optional<rr_sig>  ref_types<node>::fpcall(const rr_sig& fp_sig) const {
	if (auto it = fpcalls_.find(fp_sig); it != fpcalls_.end())
		return { it->second };
	return {};
}

// std::ostream& print(std::ostream& os) const {
// 	os << "Types:\n";
// 	for (const auto& [sig, t] : types_)
// 		os << "\t" << sig << " : " << node::name(t) << "\n";
// 	if (auto unres = unresolved(); !unres.empty()) {
// 		os << "Unresolved:\n";
// 		for (const auto& sig : unres)
// 			os << "\t" << sig << "\n";
// 	}
// 	if (errors_.size()) {
// 		os << "Errors:\n";
// 		for (const auto& err : errors_)
// 			os << "\t" << err << "\n";
// 	}
// 	return os;
// }

template <NodeType node>
void ref_types<node>::done(const rr_sig& sig) {
	LOG_TRACE << "ref type done " << LOG_RR_SIG(sig);
	todo_.erase(sig), done_.insert(sig);
}

template <NodeType node>
void ref_types<node>::todo(const rr_sig& sig) {
	if (done_.contains(sig) || todo_.contains(sig)) return;
	LOG_TRACE << "ref type todo " << LOG_RR_SIG(sig);
	todo_.insert(sig);
}

template <NodeType node>
void ref_types<node>::add_fpcall(const rr_sig& sig) {
	// TODO (LOW) decide how to call fp calculation for various
	// offset arity rels with otherwise same signature.
	// We currently call the rel with the least offset arity.
	// Should we provide a way how to specify exact relation to call? 
	rr_sig fp_sig(sig);
	fp_sig.offset_arity = 0;
	if (auto fp_exists = fpcall(fp_sig); fp_exists) {
		if (sig.offset_arity < fp_exists.value().offset_arity)
			fpcalls_[fp_sig] = sig;
	} else fpcalls_.emplace(fp_sig, sig);
}

// add sig with type t, and if it's already typed, check it equals to t
template <NodeType node>
bool ref_types<node>::add(tref n, node::type t) {
	auto r = n;
	if (auto r_as_child = tt(n) | tau::ref; r_as_child)
		r = r_as_child.value();
	auto sig = get_rr_sig<node>(r);
	if (auto fp_sig = fpcall(sig); fp_sig) { // if fp_call
		LOG_TRACE << "FP call " << LOG_RR_SIG(fp_sig.value()) << " for "
				<< LOG_RR_SIG(sig) << " : " << LOG_NT(t);
		sig = fp_sig.value(); // use actual relation's sig
	}
	typename node::type new_type = t;
	auto it = types_.find(sig);
	if (it != types_.end()) {
		auto& rt = it->second;
		std::stringstream err;
		if (rt != t) err << "Type mismatch. ";
		if (err.tellp()) {
			err     << sig <<  "() : " << node::name(new_type)
				<< " declared as " << node::name(rt);
			errors_.insert(err.str());
			return false;
		}
	} else {
		types_[sig] = new_type, done(sig);
		LOG_TRACE << "Found type of " << LOG_RR_SIG(sig) << " : "
					      << LOG_NT(types_[sig]);
		return true;
	}
	return false;
};

template <NodeType node>
bool ref_types<node>::get_types(tref n, bool def) {
	const auto& t = tau::get(n);
	// collect all refs to do
	for (tref r : t.select_all(is<node, tau::ref>)){
		auto sig = get_rr_sig<node>(r);
		todo(sig);
		if (def	&& sig.offset_arity > 0) add_fpcall(sig);
	}
	// collect all wff typed refs
	for (tref r : t.select_all(is<node, tau::wff_ref>)) add(r, tau::wff);
	// collect all bf typed refs
	for (tref r : t.select_all(is<node, tau::bf_ref>))  add(r, tau::bf);
	return errors_.empty();
}

template <NodeType node>
bool ref_types<node>::get_ref_types(const rr<node>& nso_rr) {
	// get types from relations' heads if any
	for (const auto& r : nso_rr.rec_relations)
		get_types(r.first->get(), true); // true since these are defs
	// from relations' bodies
	for (const auto& r : nso_rr.rec_relations)
		get_types(r.second->get());
	if (nso_rr.main) get_types(nso_rr.main->get()); // from main if any
	return errors_.empty();
}

//------------------------------------------------------------------------------
// rec relations ref type checking and inference

template <NodeType node>
std::optional<rr<node>> infer_ref_types(const rr<node>& nso_rr) {
	ref_types<node> ts(nso_rr);
	return infer_ref_types<node>(nso_rr, ts);
}

template <NodeType node>
std::optional<rr<node>> infer_ref_types(const rr<node>& nso_rr,
	ref_types<node>& ts)
{
	using tau = tree<node>;
	LOG_DEBUG << "-- Begin ref type inferrence --";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	static auto update_ref = [](tref r, node::type nt) {
		LOG_TRACE << "updating ref: " << LOG_FM(r)
			<< " to " << LOG_NT(nt);
		const auto& rt = tau::get(r);
		r = tau::get(nt, tau::get(nt == tau::wff ? tau::wff_ref
                                                : tau::bf_ref,
				bintree<node>::get(
					node(rt.value.nt, 0,
						nt == tau::wff ? 0 : 1),
					rt.left_child(), rt.right_sibling())));
		LOG_TRACE << "updated ref: " << LOG_FM(r);
		return r;
	};
	rr<node> nn = nso_rr;
	// inference loop
	bool changed;
	do {
		changed = false;
		for (auto& r : nn.rec_relations) {
                        tref head = r.first->get();
                        tref body = r.second->get();
			// check type of the right side
			typename node::type
				nt_head = tau::get(head).get_type(),
				nt_body = tau::get(body).get_type();
			LOG_TRACE << LOG_RULE(r) << " is " << LOG_NT(nt_body);
			if (nt_body == tau::ref) {
				// right side is unresolved ref
				if (auto topt = ts.get(get_rr_sig<node>(body));
					topt.has_value())
				{
					nt_body = topt.value();
					LOG_TRACE << "updating right side: "
							<< LOG_FM(r.second);
					r.second = tau::geth(
						update_ref(body, nt_body));
					changed = true;
				}
			}
			// update left side if right side is known
			if (nt_body == tau::bf || nt_body == tau::wff) {
				if (nt_head == tau::ref) {
					// left side is unresolved ref
					LOG_TRACE << "updating left side: "
							<< LOG_FM(r.first);
					ts.add(head, nt_body);
					r.first = tau::geth(
						update_ref(head, nt_body));
					changed = true;
				}
			}
			// infer capture's type from the left side if known
			if (nt_body == tau::capture) {
				// left side is an unresolved ref
				if (nt_head == tau::ref) {
					auto topt = ts.get(
							get_rr_sig<node>(head));
					if (topt.has_value()) { // if we know
						nt_head = topt.value(); // update
						LOG_TRACE<<"updating left side: "
							<< LOG_FM(r.first);
						ts.done(get_rr_sig<node>(head));
						r.first = tau::geth(
							update_ref(head, nt_head));
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (nt_head == tau::bf || nt_head == tau::wff) {
					LOG_TRACE << "updating capture"
						<< LOG_FM(r.second);
					r.second = tau::geth(
						tau::get(nt_head, body));
					changed = true;
				}
			}
		}
	} while (changed); // inference fixed point

	// infer main if unresolved ref
	if (nn.main) {
		tref main = nn.main->get();
		auto nt = tau::get(main).get_type();
		LOG_TRACE << "main (" << LOG_FM(main) << ") is " << LOG_NT(nt);
		if (nt == tau::ref) {
			// main is an unresolved ref
			if (auto topt = ts.get(get_rr_sig<node>(main)); topt) {
				nt = topt.value();
				LOG_TRACE << "updating main: " << LOG_FM(main);
				nn.main = tau::geth(update_ref(main, nt));
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
	if (nso_rr != nn) LOG_DEBUG << "Result: " << LOG_RR(nn);
	LOG_DEBUG << "-- End ref type inferrence --";
	return { nn };
}

inline std::ostream& operator<<(std::ostream& os, const rr_sig& s) {
	return os << rr_dict(s.name)
				<< "[" << s.offset_arity << "]/" << s.arg_arity;
}

} // namespace idni::tau_lang
