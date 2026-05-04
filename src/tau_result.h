// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TAU_RESULT_H__
#define __IDNI__TAU__TAU_RESULT_H__

#include <cstdint>
#include <iosfwd>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "utility/tree.h"

namespace idni::tau_lang {

// ---------------------------------------------------------------------------
// measuring - timing transport
// ---------------------------------------------------------------------------

struct measuring {
	std::string name;
	double ms = 0;
	std::vector<measuring> parts{};
	measuring& part();
	std::ostream& operator()(std::ostream& os, size_t level = 0) const;
	std::ostream& to_json(std::ostream& os, size_t level = 0) const;
};

std::ostream& operator<<(std::ostream& os, const measuring& m);

// ---------------------------------------------------------------------------
// diagnostic types
// ---------------------------------------------------------------------------

enum class level : uint8_t {
	error = 0,
	warning = 1,
	info = 2
};

// Stable, machine-readable diagnostic codes.
// Values are append-only. Existing numeric values must not be reused.
enum class code : uint32_t {
	unknown = 0,
	invalid_argument = 1,
	internal_error = 2,
	solver_error = 3,
	io_error = 4,
	type_error = 5,
	unsupported_operation = 6,
	invalid_state = 7,
	not_found = 8,
	unsat = 9,
	missing_type_information = 10,
	invalid_input_stream = 11,
	invalid_output_stream = 12,
	invalid_memory_access = 13,
	out_of_range = 14
};

struct location {
	std::string file{};
	std::string function{};
	uint32_t line = 0;
	uint32_t column = 0;
};

using context_value = std::variant<
	std::monostate,
	bool,
	int64_t,
	uint64_t,
	double,
	std::string
>;

using context_entry = std::pair<std::string, context_value>;
using context_entries = std::vector<context_entry>;

struct tau_diagnostic {
	idni::tau_lang::level level = idni::tau_lang::level::error;
	idni::tau_lang::code code   = idni::tau_lang::code::unknown;
	std::string message{};
	context_entries context{};
	std::optional<location> source{};

	constexpr bool is_error() const {
		return level == idni::tau_lang::level::error;
	}
	constexpr bool is_warning() const {
		return level == idni::tau_lang::level::warning;
	}
	constexpr bool is_info() const {
		return level == idni::tau_lang::level::info;
	}
};

// ---------------------------------------------------------------------------
// short diagnostic factories
// ---------------------------------------------------------------------------

inline tau_diagnostic log_error(code c, std::string msg,
	context_entries ctx = {})
{
	return tau_diagnostic{level::error, c, std::move(msg), std::move(ctx)};
}

inline tau_diagnostic log_warning(code c, std::string msg,
	context_entries ctx = {})
{
	return tau_diagnostic{level::warning, c, std::move(msg), std::move(ctx)};
}

inline tau_diagnostic log_info(code c, std::string msg,
	context_entries ctx = {})
{
	return tau_diagnostic{level::info, c, std::move(msg), std::move(ctx)};
}

// ---------------------------------------------------------------------------
// tau_result<T>
// ---------------------------------------------------------------------------

template <typename T>
class tau_result {
public:
	using value_type = T;
	using diagnostics_type = std::vector<tau_diagnostic>;

	// Allow absorb() to access private members of other tau_result<U>.
	template <typename U> friend class tau_result;

	tau_result() = default;
	explicit tau_result(T value)
		: value_(std::move(value)) {}
	tau_result(T value, diagnostics_type diagnostics)
		: value_(std::move(value)), diagnostics_(std::move(diagnostics))
	{
		enforce_error_no_value_invariant();
	}
	explicit tau_result(diagnostics_type diagnostics)
		: diagnostics_(std::move(diagnostics))
	{
		enforce_error_no_value_invariant();
	}

	[[nodiscard]] bool has_value() const {
		return value_.has_value();
	}

	explicit operator bool() const {
		return has_value();
	}

	operator T() const requires std::is_pointer_v<T> {
		return value_.has_value() ? value_.value() : nullptr;
	}

	bool operator==(std::nullptr_t) const requires std::is_pointer_v<T> {
		return !value_.has_value();
	}
	bool operator!=(std::nullptr_t) const requires std::is_pointer_v<T> {
		return value_.has_value();
	}

	T& value() & {
		return value_.value();
	}
	const T& value() const & {
		return value_.value();
	}
	T&& value() && {
		return std::move(value_.value());
	}

	void set_value(T value) {
		value_ = std::move(value);
		enforce_error_no_value_invariant();
	}

	void clear_value() {
		value_.reset();
	}

	[[nodiscard]] const diagnostics_type& diagnostics() const {
		return diagnostics_;
	}
	[[nodiscard]] diagnostics_type& diagnostics() {
		return diagnostics_;
	}

	void add_diagnostic(const tau_diagnostic& diagnostic) {
		diagnostics_.push_back(diagnostic);
		enforce_error_no_value_invariant();
	}
	void add_diagnostic(tau_diagnostic&& diagnostic) {
		diagnostics_.push_back(std::move(diagnostic));
		enforce_error_no_value_invariant();
	}

	[[nodiscard]] diagnostics_type errors() const {
		return filter_by_level(level::error);
	}
	[[nodiscard]] diagnostics_type warnings() const {
		return filter_by_level(level::warning);
	}
	[[nodiscard]] diagnostics_type infos() const {
		return filter_by_level(level::info);
	}

	// A result is well-formed iff it carries either a value or at least one
	// error diagnostic. Catches the "silent failure" state #4 where neither
	// has_value() nor any error is present - usually a forgotten branch that
	// returned `r` without populating it.
	[[nodiscard]] bool is_well_formed() const {
		return value_.has_value() || contains_level(diagnostics_, level::error);
	}

	measuring&       measured()       { return measured_; }
	const measuring& measured() const { return measured_; }

	void log_error(code c, std::string msg, context_entries ctx = {}) {
		add_diagnostic(
			tau_diagnostic{level::error, c, std::move(msg), std::move(ctx)});
	}
	void log_warning(code c, std::string msg, context_entries ctx = {}) {
		add_diagnostic(
			tau_diagnostic{level::warning, c, std::move(msg), std::move(ctx)});
	}
	void log_info(code c, std::string msg, context_entries ctx = {}) {
		add_diagnostic(
			tau_diagnostic{level::info, c, std::move(msg), std::move(ctx)});
	}

	// Move child's diagnostics into *this, append child's measured as a
	// sub-part of this->measured_. Returns the child's value (empty if failed).
	template <typename U>
	std::optional<U> absorb(tau_result<U>&& child) {
		diagnostics_.insert(diagnostics_.end(),
			std::make_move_iterator(child.diagnostics_.begin()),
			std::make_move_iterator(child.diagnostics_.end()));
		measured_.parts.push_back(std::move(child.measured_));
		enforce_error_no_value_invariant();
		return std::move(child.value_);
	}

	// absorb without extracting the child's value.
	template <typename U>
	void absorb_into(tau_result<U>&& child) {
		diagnostics_.insert(diagnostics_.end(),
			std::make_move_iterator(child.diagnostics_.begin()),
			std::make_move_iterator(child.diagnostics_.end()));
		measured_.parts.push_back(std::move(child.measured_));
		enforce_error_no_value_invariant();
	}

private:
	static bool contains_level(
		const diagnostics_type& diagnostics,
		level diagnostic_level)
	{
		for (const auto& d : diagnostics)
			if (d.level == diagnostic_level) return true;
		return false;
	}

	void enforce_error_no_value_invariant() {
		if (value_.has_value() && contains_level(diagnostics_, level::error))
			value_.reset();
	}

	[[nodiscard]] diagnostics_type filter_by_level(level diagnostic_level) const {
		diagnostics_type out;
		out.reserve(diagnostics_.size());
		for (const auto& d : diagnostics_)
			if (d.level == diagnostic_level) out.push_back(d);
		return out;
	}

	std::optional<T> value_{};
	diagnostics_type diagnostics_{};
	measuring measured_{};
};

// ---------------------------------------------------------------------------
// free helpers
// ---------------------------------------------------------------------------

inline void merge_diagnostics(
	std::vector<tau_diagnostic>& dst,
	const std::vector<tau_diagnostic>& src)
{
	dst.insert(dst.end(), src.begin(), src.end());
}

inline void merge_diagnostics(
	std::vector<tau_diagnostic>& dst,
	std::vector<tau_diagnostic>&& src)
{
	dst.insert(dst.end(),
		std::make_move_iterator(src.begin()),
		std::make_move_iterator(src.end()));
}

template <typename T>
[[nodiscard]] inline tau_result<T> fail(
	std::vector<tau_diagnostic> diagnostics)
{
	return tau_result<T>(std::move(diagnostics));
}

template <typename T>
[[nodiscard]] inline tau_result<T> tau_error(
	code c, std::string msg, context_entries ctx = {})
{
	return fail<T>({log_error(c, std::move(msg), std::move(ctx))});
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_RESULT_H__
