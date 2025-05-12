// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__REF_TYPES_INFERENCE_H__
#define __IDNI__TAU__REF_TYPES_INFERENCE_H__

#include <functional>

#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType N>
struct ref_types;

template <NodeType node>
std::optional<rr> infer_ref_types(const rr& nso_rr);
template <NodeType node>
std::optional<rr> infer_ref_types(const rr& nso_rr, ref_types<node>& ts);

// rr name-id dict
static inline std::vector<std::string> rr_v{ "dummy" };
static inline std::map<std::string, size_t> rr_m{};
inline size_t rr_dict(const std::string& s) {
	if (auto it = rr_m.find(s); it != rr_m.end()) return it->second;
	return rr_m.emplace(s, rr_v.size()), rr_v.push_back(s), rr_v.size() - 1;
};
inline const std::string& rr_dict(size_t i) { return rr_v[i]; };

// rel's signature = name id (rr_dict), offset arity and argument arity
struct rr_sig {
	size_t name = 0, offset_arity = 0, arg_arity = 0;
	auto operator<=>(const rr_sig&) const = default;
};

} // namespace idni::tau_lang

template<>
struct std::hash<idni::tau_lang::rr_sig> {
	size_t operator()(const idni::tau_lang::rr_sig& s) const noexcept {
		size_t seed = 0;
		idni::hash_combine(seed, s.name);
		idni::hash_combine(seed, s.offset_arity);
		idni::hash_combine(seed, s.arg_arity);
		return seed;
	}
};

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

#include "ref_types_inference.tmpl.h"

#endif // __IDNI__TAU__REF_TYPES_INFERENCE_H__