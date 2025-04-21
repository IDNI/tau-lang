// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
ref_types<node>::ref_types(const rr& nso_rr) { get_ref_types(nso_rr); }

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
std::optional<typename node::type> ref_types<node>::get_type(
	const rr_sig& sig)
{
	rr_sig s(sig);
	auto fpopt = fpcall(s);
	if (fpopt) s = fpopt.value();
	if (auto it = types_.find(s); it != types_.end()) {
		BOOST_LOG_TRIVIAL(trace)
			<< "(I) -- Looking for type of " << s
			<< " found " << node::name(it->second);
		return { it->second };
	}
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Looking for type of "
					<< sig << " failed";
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
	BOOST_LOG_TRIVIAL(trace)
		<< "(I) -- ref type done " << sig;
	todo_.erase(sig), done_.insert(sig);
}

template <NodeType node>
void ref_types<node>::todo(const rr_sig& sig) {
	if (done_.contains(sig) || todo_.contains(sig)) return;
	BOOST_LOG_TRIVIAL(trace)
		<< "(I) -- ref type todo " << sig;
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
bool ref_types<node>::add(tref n, const node::type& t) {
	auto r = n;
	if (auto r_as_child = tt(n) | tau::ref; r_as_child)
		r = r_as_child.value();
	auto sig = tau::get_rr_sig(r);
	if (auto fp_sig = fpcall(sig); fp_sig) { // if fp_call
		BOOST_LOG_TRIVIAL(trace) << "(I) -- FP call "
			<< fp_sig.value() << " for "
			<< sig << "() : " << node::name(t);
		sig = fp_sig.value(); // use actual relation's sig
	}
	typename node::type new_type = t;
	auto it = types_.find(sig);
	if (it != types_.end()) {
		auto& rt = it->second;
		std::stringstream err;
		if (rt != t) err << "Type mismatch. ";
		if (err.tellp()) return
			err << sig << "() : "
				<< node::name(new_type) << " declared as "
				<< node::name(rt),
			errors_.insert(err.str()), false;
	} else {
		types_[sig] = new_type, done(sig);
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Found type of "
			<< sig << "() : "
			<< node::name(types_[sig]);
		return true;
	}
	return false;
};

template <NodeType node>
bool ref_types<node>::get_types(tref n, bool def) {
	const auto& t = tau::get(n);
	for (tref r : t.select_all(// collect all refs to do
			is_non_terminal<tau::ref, node>))
	{
		auto sig = tau::get_rr_sig(r);
		todo(sig);
		if (def	&& sig.offset_arity > 0) add_fpcall(sig);
	}
	for (tref r : t.select_all(// collect all wff typed refs
			is_non_terminal<tau::wff_ref, node>))
		add(r, tau::wff);
	for (tref r : t.select_all(// collect all bf typed refs
			is_non_terminal<tau::bf_ref, node>))
		add(r, tau::bf);
	return errors_.empty();
}

template <NodeType node>
bool ref_types<node>::get_ref_types(const rr& nso_rr) {
	// get types from relations' heads if any
	for (const auto& r : nso_rr.rec_relations)
		get_types(r.first->get(), true); // true since these are defs
	// from relations' bodies
	for (const auto& r : nso_rr.rec_relations)
		get_types(r.second->get());
	if (nso_rr.main) get_types(nso_rr.main->get()); // from main if any
	return errors_.empty();
}

} // namespace idni::tau_lang
