// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__REF_TYPES_INFERENCE_H__
#define __IDNI__TAU__REF_TYPES_INFERENCE_H__

#include <functional>

#include "tau_tree.h"

namespace idni::tau_lang {

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

inline std::ostream& operator<<(std::ostream& os, const rr_sig& s) {
	return os 
		<< rr_dict(s.name) 
		<< "[" << s.offset_arity << "]/" 
		<< s.arg_arity;
}

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



#endif // __IDNI__TAU__REF_TYPES_INFERENCE_H__