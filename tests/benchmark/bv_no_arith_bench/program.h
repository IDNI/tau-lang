// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// A Tau program over bv[n] streams, unrolled into one closed formula over the
// steps 0..T. Each `always` part has its own lookback d, the largest k of the
// `s[t-k]` its body reads: its instances run from t = d to t = T, so no index
// is negative and no value before step 0 is assumed. The prefix is in time
// order, per step `all` over its inputs then `ex` over its outputs, binding
// the variables some instance mentions. The result is the text `tau::get`
// parses, ending in `.`.

#ifndef __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__
#define __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace bv_bench {

using stream_id = size_t;

class program;

/// What a part's body writes with while one instance, at step t, is built.
class instance_writer {
public:
	/// `s[t - back]` as a typed variable, e.g. `i1_3:bv[8]`.
	std::string at(stream_id s, unsigned back);
	/// A binder name for this instance, `x_3`, unused anywhere else in the
	/// formula. A quantifier inside a rule needs one per instance.
	std::string fresh(const std::string& base);
	/// An occurrence of a binder from fresh(), typed.
	std::string typed(const std::string& binder) const;

private:
	friend class program;
	instance_writer(const program& p, unsigned t,
		std::vector<std::vector<char>>& used,
		std::unordered_map<std::string, unsigned>& binders);
	const program& p;
	const unsigned t;
	std::vector<std::vector<char>>& used;
	std::unordered_map<std::string, unsigned>& binders;
	unsigned deepest = 0;
};

using rule_body = std::function<std::string(instance_writer&)>;

class program {
public:
	explicit program(unsigned width);
	/// Stream and binder names: a letter other than `T` or `F`, then letters
	/// and digits; the step is appended after `_`.
	stream_id input(const std::string& name);
	stream_id output(const std::string& name);
	/// `always body`, whose body reads exactly back to `s[t - lookback]`.
	void always(unsigned lookback, rule_body body);
	/// The closed formula over steps 0..T; throws if no part has an instance.
	std::string formula(unsigned T) const;

private:
	friend class instance_writer;
	struct stream { std::string name; bool is_input; };
	struct part { unsigned lookback; rule_body body; };
	stream_id add(const std::string& name, bool is_input);
	const std::string type;
	std::vector<stream> streams;
	std::vector<part> parts;
};

} // namespace bv_bench

#endif // __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__
