// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <array>
#include <cstdint>
#include <sstream>
#include <string_view>

#include "tau.h"
#include "work_budget_guest.h"

#ifdef TAU_WORK_BUDGET_GUEST
#include <cstdlib>
#include <emscripten/emscripten.h>
#define TAU_GUEST_EXPORT EMSCRIPTEN_KEEPALIVE
namespace boost {
BOOST_NORETURN void throw_exception(const std::exception&) { std::abort(); }
BOOST_NORETURN void throw_exception(const std::exception&, const source_location&) { std::abort(); }
}
#else
#define TAU_GUEST_EXPORT
#endif

namespace {

using namespace idni::tau_lang;
using node_t = tau_pack::node_t;
using api_t = api<node_t>;
using phase = work_budget_guest::phase;

enum class operation : uint32_t { build, update, step, inspect };
enum class status : uint32_t { finished, native_error, bad_packet, no_state };

std::array<char, 65536> request;
std::string reply_bytes;
std::optional<interpreter<node_t>> state;

void put_u32(uint32_t value) {
	for (unsigned shift = 0; shift != 32; shift += 8)
		reply_bytes.push_back(static_cast<char>(value >> shift));
}

void put_text(std::string_view text) {
	put_u32(static_cast<uint32_t>(text.size()));
	reply_bytes.append(text);
}

struct reader {
	std::string_view bytes;

	std::optional<uint32_t> number() {
		if (bytes.size() < 4) return {};
		uint32_t result = 0;
		for (unsigned n = 0; n < 4; ++n)
			result |= uint32_t(static_cast<unsigned char>(bytes[n])) << (8 * n);
		bytes.remove_prefix(4);
		return result;
	}

	std::optional<std::string_view> string() {
		auto size = number();
		if (!size || bytes.size() < *size) return {};
		auto v = bytes.substr(0, *size);
		bytes.remove_prefix(*size);
		return v;
	}
};

template <typename T>
status report_result(const result<T>& result) {
	work_budget_guest::enter(phase::report);
	std::ostringstream text;
	result.print(text);
	put_text(text.str());
	return result.has_value() && !result.has_error()
		? status::finished : status::native_error;
}

status build(std::string_view source) {
	work_budget_guest::enter(phase::boot);
	// A build runs in a fresh reactor. Time limits are disabled for this image.
	api_t::set_ltl_timeout_sec(0);
	api_t::set_ltl_closed_regions_timeout(0);
	auto ready = tau_init<node_t>();
	if (ready.has_error()) return report_result(ready);
	auto built = api_t::get_interpreter(std::string(source));
	const auto result = report_result(built);
	if (result == status::finished) state.emplace(std::move(built.value()));
	return result;
}

status update(std::string_view source) {
	auto updated = api_t::update(*state, std::string(source));
	const auto result = report_result(updated);
	if (result == status::finished) put_u32(updated.value() ? 1 : 0);
	return result;
}

status step(reader& wire) {
	auto count = wire.number();
	// Each row has at least two length words and its time word.
	if (!count || *count > wire.bytes.size() / 12) return status::bad_packet;
	std::map<stream_at, std::string> inputs;
	std::optional<stream_at> previous;
	for (uint32_t i = 0; i < *count; ++i) {
		auto name = wire.string();
		auto time = wire.number();
		auto value = wire.string();
		if (!name || !time || !value || *time > INT32_MAX)
			return status::bad_packet;
		stream_at key{std::string(*name), *time};
		if (previous && key <= *previous) return status::bad_packet;
		previous = key;
		inputs.emplace(std::move(key), std::string(*value));
	}
	if (!wire.bytes.empty()) return status::bad_packet;
	auto stepped = api_t::step(*state, std::move(inputs), false);
	const auto result = report_result(stepped);
	if (result == status::finished) {
		put_u32(static_cast<uint32_t>(stepped.value().size()));
		for (const auto& [key, value] : stepped.value()) {
			put_text(key.name);
			put_u32(static_cast<uint32_t>(key.time_point));
			put_text(value);
		}
	}
	return result;
}

status inspect() {
	work_budget_guest::enter(phase::inspect);
	using tau = tree<node_t>;
	put_u32(static_cast<uint32_t>(state->time_point));
	put_u32(static_cast<uint32_t>(api_t::spec_revision(*state)));
	put_text(api_t::current_spec(*state));
	std::vector<std::pair<std::string, std::string>> memory;
	for (const auto& [key, value] : state->memory)
		memory.emplace_back(tau::get(key).to_str(), tau::get(value).to_str());
	std::ranges::sort(memory);
	put_u32(static_cast<uint32_t>(memory.size()));
	for (const auto& [key, value] : memory) {
		put_text(key);
		put_text(value);
	}
	// Run on a detached inspection instance. This native query can prepare state.
	auto reads = api_t::get_inputs_for_step(*state);
	const auto result = report_result(reads);
	if (result == status::finished) {
		auto sorted = std::move(reads.value());
		std::ranges::sort(sorted);
		put_u32(static_cast<uint32_t>(sorted.size()));
		for (const auto& stream : sorted) {
			put_text(stream.name);
			put_u32(static_cast<uint32_t>(stream.time_point));
		}
	}
	return result;
}

status run(operation op, std::string_view payload) {
	work_budget_guest::enter(phase::decode);
	if (op != operation::build && !state) return status::no_state;
	reader wire{payload};
	switch (op) {
	case operation::build:
	case operation::update:
		return op == operation::build ? build(payload) : update(payload);
	case operation::step: return step(wire);
	case operation::inspect:
		return wire.bytes.empty() ? inspect() : status::bad_packet;
	}
	return status::bad_packet;
}
}

extern "C" {
TAU_GUEST_EXPORT uintptr_t tau_request_ptr() {
	return reinterpret_cast<uintptr_t>(request.data());
}

TAU_GUEST_EXPORT uint32_t tau_dispatch(uint32_t op, uint32_t size) {
	reply_bytes.clear();
	if (size > request.size()) return uint32_t(status::bad_packet);
	return uint32_t(run(static_cast<operation>(op), {request.data(), size}));
}

TAU_GUEST_EXPORT const char* tau_reply_ptr() { return reply_bytes.data(); }

TAU_GUEST_EXPORT uint32_t tau_reply_size() {
	return static_cast<uint32_t>(reply_bytes.size());
}
}

#ifndef TAU_WORK_BUDGET_GUEST
#include <iostream>

// Reference transport for the same bridge over native Tau. Console stream
// bytes use stdout only internally. The response goes to the original buffer.
int main() {
	std::ostream response(std::cout.rdbuf());
	std::ostringstream muted;
	std::cout.rdbuf(muted.rdbuf());
	char op;
	while (std::cin.get(op)) {
		std::array<unsigned char, 4> length;
		if (!std::cin.read(reinterpret_cast<char*>(length.data()), 4)) return 1;
		uint32_t size = 0;
		for (unsigned n = 0; n < 4; ++n) size |= uint32_t(length[n]) << (8 * n);
		if (size > request.size() || !std::cin.read(request.data(), size)) return 1;
		if (op == char(operation::build) && state) return 2;
		muted.str({});
		const auto code = tau_dispatch(static_cast<unsigned char>(op), size);
		response.put(static_cast<char>(code));
		size = tau_reply_size();
		for (unsigned shift = 0; shift != 32; shift += 8)
			response.put(static_cast<char>(size >> shift));
		response.write(tau_reply_ptr(), size).flush();
	}
	std::cout.rdbuf(response.rdbuf());
}
#endif
