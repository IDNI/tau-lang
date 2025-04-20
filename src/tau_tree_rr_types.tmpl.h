// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

//------------------------------------------------------------------------------
// rec relations type checking and inference

static auto sig2str = [](const rr_sig& s) {
	std::stringstream ss;
	return (ss << rr_dict(s.name) << "[" << s.offset_arity
		<< "]/" << s.arg_arity), ss.str();
};

// manages information about refs resolved/unresolved types and fp calls
template <NodeType N>
struct rr_types {
        using node = N;
        using type = node::type;
        using tau = tree<node>;
        using tt = tau::traverser;
	friend tau;
	rr_types(const rr& nso_rr) { get_ref_types(nso_rr); }
	// returns false if any error or unresolved ref
	bool ok() const { return errors_.empty() && unresolved().empty(); }
	// returns set of errors
	const std::set<std::string>& errors() const { return errors_; }
	// returns set of unresolved refs
	std::set<rr_sig> unresolved() const {
		std::set<rr_sig> unres(todo_);
		std::erase_if(unres, [this](const rr_sig& x) {
			return fpcalls_.contains(x);
		});
		return unres;
	}
	// returns known type of a ref, or no value
	std::optional<type> get_type(const rr_sig& sig) const {
		rr_sig s(sig);
		auto fpopt = fpcall(s);
		if (fpopt) s = fpopt.value();
		if (auto it = types_.find(s); it != types_.end()) {
			BOOST_LOG_TRIVIAL(trace)
				<< "(I) -- Looking for type of " << sig2str(s)
				<< " found " << node::name(it->second);
			return { it->second };
		}
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Looking for type of "
						<< sig2str(sig) << " failed";
		return {};
	}
	// returns ref to calculate fp by provided by fp call sig, or no value
	std::optional<rr_sig> fpcall(const rr_sig& fp_sig) const {
		if (auto it = fpcalls_.find(fp_sig); it != fpcalls_.end())
			return { it->second };
		return {};
	}
	// std::ostream& print(std::ostream& os) const {
	// 	os << "Types:\n";
	// 	for (const auto& [sig, t] : types_)
	// 		os << "\t" << sig2str(sig) << " : " << node::name(t) << "\n";
	// 	if (auto unres = unresolved(); !unres.empty()) {
	// 		os << "Unresolved:\n";
	// 		for (const auto& sig : unres)
	// 			os << "\t" << sig2str(sig) << "\n";
	// 	}
	// 	if (errors_.size()) {
	// 		os << "Errors:\n";
	// 		for (const auto& err : errors_)
	// 			os << "\t" << err << "\n";
	// 	}
	// 	return os;
	// }
private:
	void done(const rr_sig& sig) {
		BOOST_LOG_TRIVIAL(trace)
			<< "(I) -- ref type done " << sig2str(sig);
		todo_.erase(sig), done_.insert(sig);
	}
	void todo(const rr_sig& sig) {
		if (done_.contains(sig) || todo_.contains(sig)) return;
		BOOST_LOG_TRIVIAL(trace)
			<< "(I) -- ref type todo " << sig2str(sig);
		todo_.insert(sig);
	}
	void add_fpcall(const rr_sig& sig) {
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
	bool add(tref n, const node::type& t) {
		auto r = n;
		if (auto r_as_child = tt(n) | tau::ref; r_as_child)
			r = r_as_child.value();
		auto sig = tau::get_rr_sig(r);
		if (auto fp_sig = fpcall(sig); fp_sig) { // if fp_call
			BOOST_LOG_TRIVIAL(trace) << "(I) -- FP call "
				<< sig2str(fp_sig.value()) << " for "
				<< sig2str(sig) << "() : " << node::name(t);
			sig = fp_sig.value(); // use actual relation's sig
		}
		type new_type = t;
		auto it = types_.find(sig);
		if (it != types_.end()) {
			auto& rt = it->second;
			std::stringstream err;
			if (rt != t) err << "Type mismatch. ";
			if (err.tellp()) return
				err << sig2str(sig) << "() : "
					<< node::name(new_type) << " declared as "
					<< node::name(rt),
				errors_.insert(err.str()), false;
		} else {
			types_[sig] = new_type, done(sig);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- Found type of "
				<< sig2str(sig) << "() : "
				<< node::name(types_[sig]);
			return true;
		}
		return false;
	};
	bool get_types(tref n, bool def = false) {
		for (tref r : select_all(n, // collect all refs to do
				is_non_terminal<tau::ref, node>))
		{
			auto sig = tau::get_rr_sig(r);
			todo(sig);
			if (def	&& sig.offset_arity > 0) add_fpcall(sig);
		}
		for (tref r : select_all(n, // collect all wff typed refs
				is_non_terminal<tau::wff_ref, node>))
			add(r, tau::wff);
		for (tref r : select_all(n, // collect all bf typed refs
				is_non_terminal<tau::bf_ref, node>))
			add(r, tau::bf);
		return errors_.empty();
	}
	bool get_ref_types(const rr& nso_rr) {
		// get types from relations' heads if any
		for (const auto& r : nso_rr.rec_relations)
			get_types(r.first->get(), true); // true since these are defs
		// from relations' bodies
		for (const auto& r : nso_rr.rec_relations)
                        get_types(r.second->get());
		if (nso_rr.main) get_types(nso_rr.main->get()); // from main if any
		return errors_.empty();
	}
	std::unordered_map<rr_sig, rr_sig> fpcalls_;
	std::unordered_map<rr_sig, type> types_;
	std::set<rr_sig> done_, todo_;
	std::set<std::string> errors_;
};

template <NodeType node>
std::optional<rr> tree<node>::infer_ref_types(const rr& nso_rr) {
	rr_types<node> ts(nso_rr);
	return infer_ref_types(nso_rr, ts);
}

template <NodeType node>
std::optional<rr> tree<node>::infer_ref_types(const rr& nso_rr,
	rr_types<node>& ts)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin ref type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	static auto get_nt_type = [](tref r) -> node::type {
                return tree<node>::get(r).get_type();
	};
	static auto update_ref = [](tref r, const node::type& t) {
		//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
		r = get(t, get(t == tau::wff ? tau::wff_ref
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
			auto t = get_nt_type(body);
			// BOOST_LOG_TRIVIAL(trace) << "(T) " << r.second
			// 			<< " is " << (node::name(t));
			if (t == ref) {
				// right side is unresolved ref
				if (auto topt = ts.get_type(get_rr_sig(body));
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
			if (t == bf || t == wff) {
				if (get_nt_type(head) == ref) {
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
			if (t == capture) {
				t = get_nt_type(head);
				// left side is an unresolved ref
				if (t == ref) {
					auto topt = ts.get_type(get_rr_sig(head));
					if (topt.has_value()) { // if we know
						t = topt.value(); // update
						// BOOST_LOG_TRIVIAL(trace)
						// 	<< "(T) updating left "
						// 	"side: " << r.first;
						ts.done(get_rr_sig(head));
						update_ref(head, t);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (t == bf || t == wff) {
					// BOOST_LOG_TRIVIAL(trace)
					// 	<< "(T) updating capture: "
					// 	<< r.second;
					//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
					body = get(t, body);
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
		if (t == ref) {
			// main is an unresolved ref
			if (auto topt = ts.get_type(get_rr_sig(nn.main->get())); topt) {
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
		for (auto& sig : unresolved) ss << " " << sig2str(sig);
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unknown ref type for:" << ss.str();
		return {};
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End ref type inferrence"; // << ": " << nn;
	return { nn };
}

} // namespace idni::tau_lang