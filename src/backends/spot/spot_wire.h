// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
#pragma once

#include <array>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

namespace idni::tau_lang::spot_wire {
enum class op : uint8_t { strategy, game, dot, tautology, ltlf, batch_tautology };
inline constexpr uint32_t request_bound=65536, reply_bound=262144, list_bound=64;

extern "C" __attribute__((import_module("tau"), import_name("spot_v1")))
int32_t spot_v1(const uint8_t*, uint32_t, uint8_t*, uint32_t);

[[noreturn]] inline void trap() { __builtin_trap(); }
[[noreturn]] inline void local_refusal() {
	(void)spot_v1(nullptr,0,nullptr,0); // transport's typed Unsupported sentinel
	trap();
}

struct request {
	op method;
	std::string_view source = {};
	std::span<const std::string> inputs = {}, outputs = {}, batch = {};
	std::string_view argument = {};
	const synthesis_options* options = nullptr;
};
inline void word(std::vector<uint8_t>& b,uint32_t n) {
	for(unsigned s=0;s!=32;s+=8)b.push_back((n>>s)&255);
}
inline void text(std::vector<uint8_t>& b,std::string_view v) {
	if(v.size()>request_bound-4 || b.size()>request_bound-4-v.size())local_refusal();
	word(b,(uint32_t)v.size());
	b.insert(b.end(),v.begin(),v.end());
}
inline std::vector<uint8_t> encode(const request& r) {
	if(r.inputs.size()>list_bound||r.outputs.size()>list_bound||r.batch.size()>list_bound)local_refusal();
	const auto* opt=r.options;
	uint8_t flags=opt?uint8_t(opt->moore|opt->aiger<<1|opt->verify<<2):0;
	std::vector<uint8_t> b{1,static_cast<uint8_t>(r.method),flags,0};
	word(b,r.inputs.size());word(b,r.outputs.size());word(b,r.batch.size());
	for(auto s:{r.source,r.argument,opt?std::string_view(opt->algo):std::string_view{},
	    opt?std::string_view(opt->decompose):std::string_view{},
	    opt?std::string_view(opt->simplify):std::string_view{}})text(b,s);
	for(auto list:{r.inputs,r.outputs,r.batch})for(const auto& s:list)text(b,s);
	return b;
}
struct read {
	std::span<const uint8_t> b;
	size_t at=0;
	uint8_t byte(){if(at>=b.size())trap();return b[at++];}
	bool boolean(){auto v=byte();if(v>1)trap();return v!=0;}
	uint32_t word(){
		uint32_t n=0;
		for(unsigned s=0;s!=32;s+=8)n|=uint32_t(byte())<<s;
		return n;
	}
	std::string text(){
		const auto len=word();if(len>b.size()-at)trap();
		std::string s(reinterpret_cast<const char*>(b.data()+at),len);
		at+=len;return s;
	}
};

template<class T> result<T> invoke(const request& r) {
	auto send=encode(r);
	std::vector<uint8_t> recv(reply_bound);
	int32_t n=spot_v1(send.data(),send.size(),recv.data(),recv.size());
	// No throwing a C++ exception: terminate this entire detached guest
	// immediately; an ordinary native fallback must never see this result.
	if(n<0 || uint32_t(n)>recv.size())trap();
	read in{std::span<const uint8_t>(recv.data(),uint32_t(n))};
	T value;
	if constexpr (std::is_same_v<T,synthesis_verdict>) {
		value={in.boolean(),in.text()};
		if(!value.realizable&&!value.hoa.empty())trap();
	} else if constexpr (std::is_same_v<T,std::string>) value=in.text();
	else if constexpr (std::is_same_v<T,bool>) value=in.boolean();
	else {
		static_assert(std::is_same_v<T,std::vector<bool>>);
		const auto count=in.word();if(count!=r.batch.size())trap();
		for(uint32_t j=0;j<count;++j)value.push_back(in.boolean());
	}
	if(in.at!=in.b.size())trap();
	return result<T>().with_value(std::move(value));
}
}
