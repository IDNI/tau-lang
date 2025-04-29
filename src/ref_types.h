// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__REF_TYPES_H__
#define __IDNI__TAU__REF_TYPES_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// manages information about refs resolved/unresolved types and fp calls
template <NodeType N>
struct ref_types {
	using node = N;
	using tau = tree<node>;
	using tt = tau::traverser;
	template <NodeType NT>
	friend std::optional<rr> infer_ref_types(const rr& nso_rr);
	template <NodeType NT>
	friend std::optional<rr> infer_ref_types(const rr& nso_rr,
						ref_types<NT>& ts);


	ref_types(const rr& nso_rr);
	// returns false if any error or unresolved ref
	bool ok() const;
	// returns set of errors
	const std::set<std::string>& errors() const;
	// returns set of unresolved refs
	std::set<rr_sig> unresolved() const;
	// returns known type of a ref, or no value
	std::optional<typename node::type> get(const rr_sig& sig);
	// returns ref to calculate fp by provided by fp call sig, or no value
	std::optional<rr_sig> fpcall(const rr_sig& fp_sig) const;
private:
	void done(const rr_sig& sig);
	void todo(const rr_sig& sig);
	void add_fpcall(const rr_sig& sig);
	// add sig with type t, and if it's already typed, check it equals to t
	bool add(tref n, typename node::type t);
	bool get_types(tref n, bool def = false);
	bool get_ref_types(const rr& nso_rr);
	std::unordered_map<rr_sig, rr_sig> fpcalls_;
	std::unordered_map<rr_sig, typename node::type> types_;
	std::set<rr_sig> done_, todo_;
	std::set<std::string> errors_;
};

} // namespace idni::tau_lang

#include "ref_types.tmpl.h"

#endif // __IDNI__TAU__REF_TYPES_H__