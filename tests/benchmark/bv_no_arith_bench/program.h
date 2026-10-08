// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// A Tau program over bv[n] streams, unrolled into one closed formula over the
// steps 0..T. Each `always` part has its own lookback d, the largest k of the
// `s[t-k]` its body reads: its instances run from t = d to t = T, so no index
// is negative and no value before step 0 is assumed. A constraint at one step
// has that single instance. The prefix binds the parameters `all` up front,
// then is in time order, per step `all` over its inputs then `ex` over its
// outputs, binding the variables some instance mentions. The result is the
// text `tau::get` parses, ending in `.`.

#ifndef __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__
#define __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace bv_bench {

using stream_id = size_t;

class program;

/// What a part's body writes with while one instance, at step t, is built.
class instance_writer {
public:
	/// `s[t - back]` as a typed variable, e.g. `i1_3:bv[8]`; a parameter
	/// has no step, `p1:bv[8]`, and is read with back 0.
	std::string at(stream_id s, unsigned back);
	/// A binder name for this instance, `x_3`, unused anywhere else in the
	/// formula. A quantifier inside a rule needs one per instance.
	std::string fresh(const std::string& base);
	/// An occurrence of a binder from fresh(), typed.
	std::string typed(const std::string& binder) const;
	/// The constant `value` of the program's type, e.g. `{ 0 }:bv[8]`.
	std::string constant(unsigned long long value) const;

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

/// `h` holds at most one element, for a variable `h` from at():
/// `all x (x & h' = 0 -> (x = 0 || x = h))` with a fresh x.
std::string at_most_one(instance_writer& w, const std::string& h);

class program {
public:
	explicit program(unsigned width);
	/// Stream and binder names: a letter other than `T` or `F`, then letters
	/// and digits; the step is appended after `_`.
	stream_id input(const std::string& name);
	stream_id output(const std::string& name);
	/// One value for all steps, bound `all` in front of the whole ladder.
	stream_id parameter(const std::string& name);
	/// `always body`, whose body reads exactly back to `s[t - lookback]`.
	void always(unsigned lookback, rule_body body);
	/// `body` at the single step `step` only; it may read back to step 0.
	void at_step(unsigned step, rule_body body);
	/// The closed formula over steps 0..T; throws if no part has an instance
	/// or a constraint lies beyond step T.
	std::string formula(unsigned T) const;

private:
	friend class instance_writer;
	enum class role { input, output, parameter };
	struct stream { std::string name; role r; };
	// An `always` part when `step` is empty.
	struct part {
		unsigned lookback;
		rule_body body;
		std::optional<unsigned> step;
	};
	stream_id add(const std::string& name, role r);
	const std::string type;
	std::vector<stream> streams;
	std::vector<part> parts;
};

} // namespace bv_bench

#endif // __IDNI__TAU__BV_NO_ARITH_BENCH__PROGRAM_H__
