// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#pragma once

#include <istream>
#include <ostream>

#include "plan.h"

namespace idni::tau_lang::ga1 {

/// Each worker owns its held plans. A caller serializes calls to one worker;
/// use a separate worker per thread. Persistence and the trusted digest
/// ledger are the caller's responsibility.
class worker {
public:
	json request(const json& req) {
		const auto* call = req.get("call");
		const auto* save = req.get("save");
		const auto* drop = req.get("drop");
		if (int(call != nullptr) + int(save != nullptr) + int(drop != nullptr) != 1)
			return request_error("expected_one_call_save_or_drop", nullptr);
		if (!call) {
			std::string key;
			const auto* k = save ? save : drop;
			if (!hex64(k)) return request_error("invalid_plan_digest", nullptr);
			k->ascii(key);
			json result = json::object();
			result.set("status", json::str("Completed"));
			if (drop) {
				result.set("dropped", json::boolean(held.by_digest.erase(key) != 0));
			} else {
				auto it = held.by_digest.find(key);
				if (it == held.by_digest.end()) {
					result.set("status", json::str("Operational"));
					result.set("kind", json::str("plan_not_held"));
				} else result.set("plan", saved_of(*it->second));
			}
			result.set("held", json::integer((int64_t)held.by_digest.size()));
			return result;
		}
		const json* op = call->get("op");
		std::string tag;
		if (!op || !op->ascii(tag) ||
				(tag != "get_interpreter" && tag != "load" && tag != "step"))
			return request_error("unsupported_worker_operation", call->get("state"));
		if (tag == "load" && !hex64(call->get("bind")))
			return request_error("expected_saved_plan_digest", call->get("state"));
		const auto state = held.attach(call->get("state"));
		result_t r;
		if (tag == "get_interpreter" && !call->get("compiler")) {
			json build = *call;
			build.set("compiler", json::str(MULTILINE_COMPILER));
			r = transition(build, state);
		} else r = transition(*call, state);
		json reply = json::object();
		reply.set("outcome", std::move(r.outcome));
		const auto* pre = call->get("state");
		reply.set("post", r.post ? std::move(*r.post) : pre ? *pre : json{});
		// A completed binary load has already verified that these exact
		// lowercase hex code units encode the committed saved bytes.
		const auto* wire = tag == "load" && r.prepared
			&& (r.prepared->format == "packed" || r.prepared->format == "varint")
			? call->get("bytes") : nullptr;
		reply.set("plan", r.prepared ? saved_of(*r.prepared, wire) : json{});
		held.keep(std::move(r.prepared));
		return reply;
	}

	/// For callers using typed C++ ownership rather than the JSON protocol.
	std::size_t held_count() const { return held.by_digest.size(); }
	void clear() { held.by_digest.clear(); }

private:
	plan_table held;

	static json request_error(const char* reason, const json* pre) {
		json j = json::object();
		j.set("outcome", ga1_wire::stopped_json("Unsupported", reason, "worker.admit", {}, 0));
		j.set("post", pre ? *pre : json{});
		j.set("plan", json{});
		return j;
	}

	static json saved_of(const plan& p, const json* wire = nullptr) {
		static const char digits[] = "0123456789abcdef";
		std::string hex;
		if (!wire) {
			hex.reserve(p.saved.size() * 2);
			for (unsigned char c : p.saved) {
				hex.push_back(digits[c >> 4]);
				hex.push_back(digits[c & 15]);
			}
		}
		json j = json::object();
		j.set("format", json::str(p.format));
		j.set("digest", json::str(p.digest));
		j.set("saved", wire ? *wire : json::str(hex));
		return j;
	}
};

/// Read at most 1 MiB per request, enough for a hex-encoded maximum-size
/// plan. Discard an oversize line before processing the next one.
inline int run_worker(std::istream& input, std::ostream& output) {
	worker w;
	static constexpr std::size_t max_line_bytes = 1 << 20;
	auto* buffer = input.rdbuf();
	using traits = std::char_traits<char>;
	for (;;) {
		std::string line;
		bool oversize = false;
		traits::int_type c;
		while ((c = buffer->sbumpc()) != traits::eof() && c != '\n') {
			if (line.size() < max_line_bytes) line.push_back(traits::to_char_type(c));
			else oversize = true;
		}
		if (c == traits::eof() && line.empty()) break;
		auto req = oversize ? std::optional<json>{} : ga1_wire::parse_json(line, 32);
		output << ga1_wire::dump(w.request(req ? *req : json{})) << '\n' << std::flush;
		if (!output) return 1;
	}
	return 0;
}

} // namespace idni::tau_lang::ga1
