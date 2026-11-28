#include "metered_tau.h"
#include <wasmtime.h>
static_assert(WASMTIME_VERSION_MAJOR==51 && WASMTIME_VERSION_MINOR==0 && WASMTIME_VERSION_PATCH==0,
              "This experimental profile requires Wasmtime 51.0.0");
#include <openssl/sha.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <expected>
#include <fstream>
#include <limits>
#include <mutex>
#include <regex>
#include <set>
#include <stdexcept>
#include <tuple>

namespace idni::tau_lang::metered {
namespace {
using bytes = std::vector<uint8_t>;
using view = std::span<const uint8_t>;
constexpr uint32_t packet_limit = 65536, child_limit = 262144;
constexpr uint64_t blocks(uint64_t n) { return (n+31)/32; }
struct stop { status code; std::string why; bool trap = false; };
template<class T> using outcome = std::expected<T,stop>;
auto fail(status s, std::string why, bool trap=false) {
    return std::unexpected(stop{s,std::move(why),trap});
}
#define M_REQUIRE(condition, reason) \
    do { if (!(condition)) return fail(status::operational, reason); } while (false)
#define M_TRY(expression) \
    do { if (auto result = (expression); !result) return std::unexpected(std::move(result.error())); } while (false)
#define M_GET(name, expression) \
    auto name##_result = (expression); \
    if (!name##_result) return std::unexpected(std::move(name##_result.error())); \
    auto name = std::move(*name##_result)
std::string digest(view b) {
    std::array<unsigned char,SHA256_DIGEST_LENGTH> d;
    SHA256(b.data(),b.size(),d.data());
    std::string k; constexpr char hex[]="0123456789abcdef";
    for (auto n:d) { k.push_back(hex[n>>4]); k.push_back(hex[n&15]); }
    return k;
}
outcome<bytes> load(const std::filesystem::path& path, std::string_view sum, size_t length) {
    // The component only loads its five immutable local files, before any
    // call. Check the exact size before allocation, then authenticate bytes.
    std::error_code ec;
    const auto physical=std::filesystem::file_size(path,ec);
    M_REQUIRE(!ec,"image not found");
    M_REQUIRE(physical==length,"unqualified image length");
    std::ifstream f(path,std::ios::binary);
    M_REQUIRE(bool(f),"image unavailable");
    bytes out(length);
    f.read(reinterpret_cast<char*>(out.data()),out.size());
    M_REQUIRE(bool(f)&&f.peek()==std::char_traits<char>::eof()&&!f.bad(),"image read");
    M_REQUIRE(digest(out)==sum,"unqualified image digest");
    return out;
}
outcome<void> err(wasmtime_error_t* e) {
    if (!e) return {};
    const std::unique_ptr<wasmtime_error_t,decltype(&wasmtime_error_delete)> owner(e,&wasmtime_error_delete);
    wasm_name_t msg{};
    wasmtime_error_message(e,&msg);
    struct cleanup { wasm_name_t* n; ~cleanup() { wasm_name_delete(n); } } release{&msg};
    return fail(status::operational,std::string(msg.data,msg.size));
}
outcome<void> trapping(wasm_trap_t* t) {
    if (!t) return {};
    const std::unique_ptr<wasm_trap_t,decltype(&wasm_trap_delete)> owner(t,&wasm_trap_delete);
    wasm_message_t name{};
    wasm_trap_message(t,&name);
    struct cleanup { wasm_message_t* n; ~cleanup() { wasm_name_delete(n); } } release{&name};
    return fail(status::operational,std::string(name.data,name.size),true);
}
void append(bytes& out,uint32_t n) { for(int i=0;i<4;i++) out.push_back(uint8_t(n>>(8*i))); }
void append(bytes& out,std::string_view v) {
    append(out,uint32_t(v.size())); out.insert(out.end(),v.begin(),v.end());
}
uint32_t parse32(view v) {
    return uint32_t(v[0]) | uint32_t(v[1])<<8 | uint32_t(v[2])<<16 | uint32_t(v[3])<<24;
}
std::string as_text(view b) { return std::string(b.begin(),b.end()); }
wasmtime_val_t i32(uint32_t n) {
    wasmtime_val_t v{}; v.kind=WASMTIME_I32; v.of.i32=n; return v;
}
wasmtime_val_t i64(uint64_t n) {
    wasmtime_val_t v{}; v.kind=WASMTIME_I64; v.of.i64=std::bit_cast<int64_t>(n); return v;
}
struct module {
    wasmtime_module_t* m=nullptr;
    wasmtime_linker_t* l=nullptr;
    ~module() { if(l) wasmtime_linker_delete(l); if(m) wasmtime_module_delete(m); }
};
using native_store=std::unique_ptr<wasmtime_store_t,decltype(&wasmtime_store_delete)>;
struct snapshot {
    // The store is only a memory carrier. It is never executed again.
    native_store store;
    wasmtime_context_t* ctx;
    wasmtime_memory_t memory;
    uint32_t stack;
    view memory_bytes() const {
        return {wasmtime_memory_data(ctx,&memory),wasmtime_memory_data_size(ctx,&memory)};
    }
};
enum class op:int { build, update, step, inspect };
constexpr std::array phases{"boot","decode","parse","prepare","solve","render","embedded_update",
    "update_plan","update_metadata","update_install","report","inspect","bv_translate","bv_decision",
    "bv_model_check","bv_model_value","hsb_decision","hsb_model_value"};
struct spot_query {
    uint8_t op,flags;
    std::string source,argument,algo,decompose,simplify;
    std::vector<std::string> inputs,outputs,batch;
};
outcome<spot_query> request(view b) {
    M_REQUIRE(b.size()>=16&&b.size()<=packet_limit&&b[0]==1&&b[1]<=5&&b[2]<=7&&!b[3],"Spot header");
    size_t p=16;
    auto get=[&]() -> outcome<std::string> {
        M_REQUIRE(p+4<=b.size(),"missing Spot string length");
        const auto n=parse32(b.subspan(p)); p+=4;
        M_REQUIRE(n<=b.size()-p,"missing Spot string bytes");
        auto text=as_text(b.subspan(p,n)); p+=n;
        M_REQUIRE(text.find('\0')==std::string::npos,"Spot NUL");
        return text;
    };
    auto vec=[&](uint32_t n) -> outcome<std::vector<std::string>> {
        M_REQUIRE(n<=64,"Spot list bound");
        std::vector<std::string> v;
        for(uint32_t i=0;i<n;i++){ M_GET(text,get());v.push_back(std::move(text)); }
        return v;
    };
    M_GET(source,get()); M_GET(argument,get()); M_GET(algo,get());
    M_GET(decompose,get()); M_GET(simplify,get());
    M_GET(inputs,vec(parse32(b.subspan(4))));
    M_GET(outputs,vec(parse32(b.subspan(8))));
    M_GET(batch,vec(parse32(b.subspan(12))));
    M_REQUIRE(p==b.size(),"Spot trailing fields");
    return spot_query{b[1],b[2],std::move(source),std::move(argument),std::move(algo),
        std::move(decompose),std::move(simplify),std::move(inputs),std::move(outputs),std::move(batch)};
}
struct answer {
    status code;
    std::string why;
    uint64_t used;
    bytes packet={};
};
}

struct profile {
    std::unique_ptr<wasm_engine_t,decltype(&wasm_engine_delete)> engine{nullptr,&wasm_engine_delete};
    module core,child,ltlfilt,autfilt;
    uint64_t boot=0;
    answer solve(const spot_query&,uint64_t);
    answer filters(const spot_query&,uint64_t);
    outcome<void> load_filters(const std::filesystem::path&);
    outcome<void> open(const std::filesystem::path&);
    receipt invoke(op,view,uint64_t,std::optional<snapshot>&);
};

namespace {
struct machine {
    profile& owner;
    native_store store{nullptr,&wasmtime_store_delete};
    wasmtime_context_t* ctx=nullptr;
    wasmtime_instance_t instance{};
    wasmtime_memory_t memory{};
    wasmtime_global_t meter{};
    bool live=false, is_child;
    bool virtual_fstat=false;
    uint64_t remaining,cap;
    receipt r;
    std::optional<stop> stopped;
    std::string stage="instantiate";
    std::optional<int> exited;
    std::vector<std::string> argv;
    bytes stdin_bytes,stdout_bytes;
    size_t read_at=0,room=child_limit;
    machine(profile& p,uint64_t budget,bool child):owner(p),is_child(child),remaining(budget),cap(budget) {}
    uint64_t left() {
        if (!live) return remaining;
        wasmtime_val_t v{}; wasmtime_global_get(ctx,&meter,&v);
        return uint64_t(v.of.i64);
    }
    outcome<void> spend(uint64_t n) {
        uint64_t v=left();
        if (v<n) return fail(status::budget,"host charge");
        if (!live) { remaining=v-n; return {}; }
        auto out=i64(v-n);
        return err(wasmtime_global_set(ctx,&meter,&out));
    }
    std::optional<wasmtime_extern_t> lookup(std::string_view name) {
        wasmtime_extern_t x{};
        if(wasmtime_instance_export_get(ctx,&instance,name.data(),name.size(),&x)) return x;
        return {};
    }
    outcome<wasmtime_extern_t> get(std::string_view key) {
        auto x=lookup(key); M_REQUIRE(x.has_value(),"missing guest export");return *x;
    }
    outcome<uint32_t> glob(std::string_view key) {
        M_GET(x,get(key)); wasmtime_val_t v{};
        wasmtime_global_get(ctx,&x.of.global,&v); return uint32_t(v.of.i32);
    }
    bool capacity_failed() { return live&&glob("tau_capacity_failed").value_or(0)==1; }
    size_t size() { return wasmtime_memory_data_size(ctx,&memory); }
    outcome<std::span<uint8_t>> slice(uint32_t at,uint32_t n) {
        M_REQUIRE(uint64_t(at)+n<=size(),"Wasm range");
        return std::span{wasmtime_memory_data(ctx,&memory)+at,n};
    }
    outcome<void> write(uint32_t at,view data) {
        M_GET(target,slice(at,data.size()));std::copy(data.begin(),data.end(),target.begin());
        return {};
    }
    outcome<void> put(uint32_t at,uint64_t n,uint32_t width=4) {
        M_GET(v,slice(at,width));for(auto& b:v) { b=uint8_t(n);n>>=8; }
        return {};
    }
    outcome<void> attach(module& mod) {
        store.reset(wasmtime_store_new(owner.engine.get(),this,nullptr));
        M_REQUIRE(bool(store),"store unavailable");
        ctx=wasmtime_store_context(store.get());
        wasm_trap_t* t=nullptr;
        M_TRY(err(wasmtime_linker_instantiate(mod.l,ctx,mod.m,&instance,&t))); M_TRY(trapping(t));
        M_GET(m,get("memory")); memory=m.of.memory;
        M_GET(g,get("tau_meter_remaining")); meter=g.of.global;
        auto v=i64(remaining); M_TRY(err(wasmtime_global_set(ctx,&meter,&v)));
        live=true;
        return {};
    }
    outcome<int32_t> wasm(std::string_view name,std::initializer_list<wasmtime_val_t> args={},bool output=true) {
        M_GET(f,get(name));
        wasm_trap_t* t=nullptr; wasmtime_val_t v{};
        auto* failure=wasmtime_func_call(ctx,&f.of.func,args.begin(),args.size(),output?&v:nullptr,output?1:0,&t);
        // Only the typed callback state classifies a host termination.
        if(failure&&(exited.has_value()||stopped.has_value())){
            wasmtime_error_delete(failure);
            if(t)wasm_trap_delete(t);
            return fail(status::operational,"typed host termination",true);
        }
        M_TRY(err(failure));
        M_TRY(trapping(t)); return output?v.of.i32:0;
    }
    outcome<void> restore(const snapshot& prev) {
        const auto mem=prev.memory_bytes();
        M_TRY(spend(2+blocks(mem.size())));
        auto n=size();
        M_REQUIRE(mem.size()>=n&&((mem.size()-n)%65536)==0,"restore length");
        if (mem.size()>n) {
            uint64_t old;
            if(auto e=wasmtime_memory_grow(ctx,&memory,(mem.size()-n)/65536,&old)) {
                wasmtime_error_delete(e);
                return fail(status::operational,"restore runtime capacity");
            }
        }
        M_TRY(write(0,mem));
        M_GET(stack,get("__stack_pointer"));auto v=i32(prev.stack);
        return err(wasmtime_global_set(ctx,&stack.of.global,&v));
    }
    outcome<uint32_t> bridge(uint32_t p,uint32_t n,uint32_t out,uint32_t limit) {
        M_TRY(spend(1));
        auto stop_spot=[&](status s,std::string why,uint32_t wire){
            stopped=stop{s,std::move(why)}; stage="spot"; return wire;
        };
        if(!p&&!n&&!out&&!limit)return stop_spot(status::unsupported,"guest request bound",uint32_t(-2));
        if(n>packet_limit||limit>child_limit||!n||uint64_t(p)+n>size()||
            uint64_t(out)+limit>size()||(p<uint64_t(out)+limit&&out<uint64_t(p)+n))
            return stop_spot(status::operational,"request/reply ranges",uint32_t(-4));
        M_TRY(spend(blocks(n)));
        M_GET(request_bytes,slice(p,n));
        M_GET(q,request(request_bytes));
        const auto budget=left();
        if(!budget)return stop_spot(status::budget,"no affordable child dispatch",uint32_t(-1));
        const auto parent_prefix=cap-budget;
        auto a=owner.solve(q,budget);
        M_REQUIRE(a.used<=budget,"child receipt outside ledger");
        M_TRY(spend(a.used));
        r.solvers.push_back({q.op,q.source,q.outputs,parent_prefix,a.used,a.code,
            a.code==status::finished?std::optional(digest(a.packet)):std::nullopt});
        if(a.code!=status::finished)
            return stop_spot(a.code,a.why,uint32_t(a.code==status::budget?-1:a.code==status::unsupported?-2:-3));
        M_REQUIRE(a.used,"zero-cost child success");
        if(a.packet.size()>limit)return stop_spot(status::unsupported,"guest reply bound",uint32_t(-2));
        M_TRY(spend(1+blocks(a.packet.size())));
        M_TRY(write(out,a.packet));
        return a.packet.size();
    }
    outcome<uint32_t> invoke_import(std::string_view ns,std::string_view fn,const wasmtime_val_t* a) {
        auto arg=[&](size_t i){return uint32_t(a[i].of.i32);};
        if(ns=="tau"&&fn=="spot_v1") { ++r.attempts["spot"];
            auto bridged=bridge(arg(0),arg(1),arg(2),arg(3));
            if(bridged)return *bridged;
            const auto& s=bridged.error();
            stopped=stop{s.code==status::budget?status::budget:status::operational,s.why};
            stage="spot";return uint32_t(s.code==status::budget?-1:-4);
        }
        std::string label(fn);
        if(!is_child) {
            if(fn=="clock_time_get")label="clock";
            else if(fn=="environ_sizes_get")label="env_sizes";
            else if(fn=="environ_get")label="env_get";
            else if(fn=="emscripten_notify_memory_growth")label="memory_notify";
            else if(fn=="__syscall_faccessat")label="faccessat";
            else if(fn=="__syscall_dup3")label="dup3";
            else if((fn=="__syscall_pipe"||fn=="__syscall_pipe2"))label="pipe";
            else if(fn=="_msync_js")label="msync";
            else if(fn=="__syscall_unlinkat")label="unlink";
            else if(fn=="random_get")label="random_get";
        }
        M_TRY(spend(1)); ++r.attempts[label];
        if(ns=="tau") {
            if(fn=="phase") {
                stage=arg(0)<phases.size()?phases[arg(0)]:"guest";
                r.trace.push_back({stage,cap-left()}); return 0;
            }
            if(fn=="unsupported") {
                constexpr std::array reasons{"input_file","output_file","spot","console_input"};
                return fail(status::unsupported,arg(0)<reasons.size()?reasons[arg(0)]:"external");
            }
        }
        if(ns=="env") {
            if(fn=="emscripten_notify_memory_growth")return 0;
            const auto reason=fn=="__syscall_faccessat"?"filesystem_probe":
                fn=="__syscall_dup3"?"external_file_descriptor":(fn=="__syscall_pipe"||fn=="__syscall_pipe2")?"process_pipe":
                fn=="_msync_js"?"file_sync":"file_unlink";
            return fail(status::unsupported,reason);
        }
        if(fn=="clock_time_get"){M_TRY(spend(1));M_TRY(put(arg(2),0,8));return 0;}
        if(fn=="environ_sizes_get"){M_TRY(spend(2));M_TRY(put(arg(0),0));M_TRY(put(arg(1),0));return 0;}
        if(fn=="environ_get")return 0;
        if(fn=="fd_close")return is_child&&arg(0)<=2?0:8;
        if(ns=="wasi_snapshot_preview1"&&fn=="fd_filestat_get"&&is_child&&virtual_fstat){
            if(arg(0)>2)return 8;
            M_TRY(spend(2)); M_GET(dest,slice(arg(1),64));
            std::fill(dest.begin(),dest.end(),0); dest[16]=2; return 0;
        }
        if(fn=="fd_fdstat_get"){
            if(arg(0)>2)return 8;
            M_TRY(spend(2));M_GET(dest,slice(arg(1),24));std::fill(dest.begin(),dest.end(),0);
            dest[0]=6;dest[8]=arg(0)==0?2:64;return 0;
        }
        if(!is_child&&fn=="fd_seek")return fail(status::unsupported,"file_seek");
        if(!is_child&&fn=="fd_read")return fail(status::unsupported,"console_input");
        if(fn=="random_get")return fail(status::unsupported,"ambient_randomness");
        if(fn=="proc_exit"){
            if(!is_child)return fail(status::operational,"guest exited");
            exited=arg(0);return fail(status::operational,"virtual process exit",true);
        }
        if(is_child&&fn=="fd_seek")return arg(0)<=2?70:8;
        if(is_child&&(fn=="args_sizes_get"||fn=="args_get")){
            size_t total=0;for(const auto& s:argv)total+=s.size()+1;
            if(fn=="args_sizes_get"){M_TRY(spend(2));M_TRY(put(arg(0),argv.size()));M_TRY(put(arg(1),total));return 0;}
            M_TRY(spend(argv.size()+blocks(total)));
            uint32_t d=arg(0),p=arg(1);
            for(const auto& s:argv){
                M_TRY(put(d,p));M_GET(dst,slice(p,s.size()+1));
                std::copy(s.begin(),s.end(),dst.begin());dst.back()=0;d+=4;p+=s.size()+1;
            }
            return 0;
        }
        if(fn=="fd_write"||(is_child&&fn=="fd_read")){
            const bool reading=fn=="fd_read"; const auto fd=arg(0), n=arg(2);
            if(reading?fd!=0:(fd!=1&&fd!=2)){
                if(is_child)return 8;
                return fail(status::unsupported,"file_output");
            }
            if(n>(is_child?64u:8192u))return fail(status::unsupported,is_child?"iov limit":"vector bound");
            M_TRY(spend(n+(!is_child)));
            M_GET(vec,slice(arg(1),8*n));uint64_t total=0;
            for(uint32_t i=0;i<n;i++){
                const auto a=parse32(vec.subspan(8*i)); auto length=parse32(vec.subspan(8*i+4));
                if(!is_child) {M_TRY(slice(a,length));total+=length;M_REQUIRE(total<=UINT32_MAX,"console size");continue;}
                if(reading){
                    length=std::min<size_t>(length,stdin_bytes.size()-read_at);
                    M_TRY(spend(blocks(length)));M_TRY(write(a,view(stdin_bytes).subspan(read_at,length)));read_at+=length;
                }else{
                    if(length>room)return fail(status::unsupported,"output bound");
                    M_TRY(spend(blocks(length)));M_GET(b,slice(a,length));room-=length;
                    if(fd==1)stdout_bytes.insert(stdout_bytes.end(),b.begin(),b.end());
                }
                total+=length;
            }
            if(is_child)M_TRY(spend(1));
            M_TRY(put(arg(3),total));return 0;
        }
        return fail(status::unsupported,"import:"+std::string(fn));
    }
};
struct binding { std::string module,name; };
wasm_trap_t* callback(void* ptr,wasmtime_caller_t* caller,const wasmtime_val_t* in,size_t,
        wasmtime_val_t* out,size_t nr) noexcept {
    auto& bind=*static_cast<binding*>(ptr);
    auto* live=static_cast<machine*>(wasmtime_context_get_data(wasmtime_caller_context(caller)));
    const char* immediate="sandboxed host stop";
    if(!live)return wasmtime_trap_new(immediate,std::strlen(immediate));
    auto& g=*live;
    try {
        auto res=g.invoke_import(bind.module,bind.name,in);
        if(res) {
            if(nr)out[0]=i32(*res);
            return nullptr;
        }
        if(g.exited) immediate="virtual process exit";
        else g.stopped=std::move(res.error());
    } catch(...) {}
    return wasmtime_trap_new(immediate,std::strlen(immediate));
}
outcome<void> link(wasm_engine_t* e,module& m,view image) {
    M_TRY(err(wasmtime_module_new(e,image.data(),image.size(),&m.m)));
    m.l=wasmtime_linker_new(e);M_REQUIRE(m.l,"linker unavailable");
    wasm_importtype_vec_t list{};wasmtime_module_imports(m.m,&list);
    struct cleanup { wasm_importtype_vec_t* v; ~cleanup() { wasm_importtype_vec_delete(v); } } release{&list};
    for(size_t j=0;j<list.size;j++){
        auto p=list.data[j]; auto a=wasm_importtype_module(p),b=wasm_importtype_name(p);
        auto k=wasm_importtype_type(p);M_REQUIRE(wasm_externtype_kind(k)==WASM_EXTERN_FUNC,"non-function import");
        auto bind=std::make_unique<binding>(binding{{a->data,a->size},{b->data,b->size}});
        M_REQUIRE(bind->module=="env"||bind->module=="tau"||bind->module=="wasi_snapshot_preview1","invalid import namespace");
        // The linker owns each callback binding.
        binding* c=bind.release();
        M_TRY(err(wasmtime_linker_define_func(m.l,a->data,a->size,b->data,b->size,
            wasm_externtype_as_functype_const(k),callback,c,[](void* x){delete static_cast<binding*>(x);})));
    }
    return {};
}
std::string join(const std::vector<std::string>& parts) {
    std::string q;for(auto& s:parts){if(!q.empty())q+=',';q+=s;}return q;
}
}

namespace {
std::optional<answer> run_spot(machine& s, module& image,uint64_t cap) {
    const auto refused=[&](const stop& why){
        return answer{why.code,why.code==status::budget?"budget":why.why,cap-s.left()};
    };
    try {
        if(auto attached=s.attach(image);!attached)return refused(attached.error());
        if(auto result=s.wasm("_start",{},false);result)s.exited=0;
        else {
            if(!result.error().trap)return refused(result.error());
            if(s.glob("tau_meter_stopped").value_or(0)==1)return answer{status::budget,"budget",cap-s.left()};
            if(s.capacity_failed())return answer{status::operational,"runtime capacity",cap-s.left()};
            if(s.stopped)return refused(*s.stopped);
            if(!s.exited)return answer{status::operational,"unexpected Spot runtime failure",0};
        }
        if(auto spent=s.spend(1+blocks(child_limit-s.room));!spent)return refused(spent.error());
    }catch(...){return answer{status::operational,"unexpected Spot runtime failure",0};}
    return {};
}
bool formula_line(std::string_view s) {
    return !s.empty()&&s.size()<=4096&&s.find_first_of(std::string_view("\0\n\r",3))==s.npos;
}
std::string_view trim_output(std::string_view s,bool newline=true) {
    const auto i=s.find_last_not_of(newline?" \r\n\t\v\f":" \r\t\v\f");
    return s.substr(0,i==s.npos?0:i+1);
}
}
answer profile::filters(const spot_query& q,uint64_t cap) {
    const auto unsupported=[](){return answer{status::unsupported,"Spot filter profile",0};};
    if(q.op<2||q.op>5||q.flags||!q.algo.empty()||!q.decompose.empty()||!q.simplify.empty()||
        !q.inputs.empty()||!q.outputs.empty())return unsupported();
    machine s(*this,cap,true);
    std::string source;
    s.argv={"ltlfilt","-r","-F","-"};
    if(q.op==2){
        if(!q.argument.empty()||!q.batch.empty()||!q.source.starts_with("HOA:")||
           q.source.size()>packet_limit||q.source.find('\0')!=std::string::npos)return unsupported();
        source=q.source; s.argv={"autfilt","--dot","-"}; s.virtual_fstat=true;
    }else if(q.op==5){
        if(!q.argument.empty()||!q.source.empty()||q.batch.empty()||q.batch.size()>64)return unsupported();
        for(const auto& row:q.batch){
            if(!formula_line(row)||row.size()+1>packet_limit-source.size())return unsupported();
            source+=row; source+='\n';
        }
    }else{
        if(!q.batch.empty()||!formula_line(q.source))return unsupported();
        if(q.op==3){if(!q.argument.empty())return unsupported();}
        else {
            static const std::regex atom("[A-Za-z_][A-Za-z_0-9]{0,31}");
            if(!std::regex_match(q.argument,atom))return unsupported();
            s.argv[1]="--from-ltlf="+q.argument;
        }
        source=q.source+'\n';
    }
    s.stdin_bytes.assign(source.begin(),source.end());
    if(auto bad=run_spot(s,q.op==2?autfilt:ltlfilt,cap))return *bad;
    const uint64_t used=cap-s.left();
    if(s.exited!=0)return {status::operational,"stock Spot exit without admitted result",used};
    const auto text=as_text(s.stdout_bytes);
    const std::string_view out=text;
    bytes payload;
    if(q.op==2)append(payload,out);
    else if(q.op==3)payload.push_back(trim_output(out)=="1");
    else if(q.op==4){
        const auto value=trim_output(out);
        if(value.empty())return {status::operational,"ltlfilt produced no formula",used};
        append(payload,value);
    }else {
        size_t pos=0;
        append(payload,uint32_t(q.batch.size()));
        while(pos<out.size()){
            const auto nl=out.find('\n',pos);
            payload.push_back(trim_output(out.substr(pos,nl==out.npos?nl:nl-pos),false)=="1");
            if(nl==out.npos)break;
            pos=nl+1;
        }
        if(payload.size()!=4+q.batch.size())
            return {status::operational,"ltlfilt printed a different number of rows",used};
    }
    return {status::finished,"",used,std::move(payload)};
}
outcome<void> profile::load_filters(const std::filesystem::path& root) {
    M_GET(lt,load(root/"ltlfilt.guarded.wasm", "f6536f44c3ef060f115fc4f19a10c517dad3c03e27f65b8a8d093bf6d94fd48e", 11872722));
    M_GET(au,load(root/"autfilt.guarded.wasm", "362bf52647dd25e79f12c32ff9a6aeafd46980530971de4a2a941d175a783a49", 12081918));
    M_TRY(link(engine.get(),ltlfilt,lt));
    return link(engine.get(),autfilt,au);
}

answer profile::solve(const spot_query& q,uint64_t cap) {
    if(q.op>1)return filters(q,cap);
    const bool game=q.op==1;
    if (q.op>1||q.flags||!q.batch.empty()||q.outputs.empty()||q.inputs.size()+q.outputs.size()>24||
        !q.algo.empty()||!q.decompose.empty()||!q.simplify.empty()||
        (game?(q.argument!="acd"&&q.argument!="sd"):!q.argument.empty())||
        q.source.size()>4096||q.source.empty()||q.source.find_first_of("\r\n")!=std::string::npos)
        return {status::unsupported," stock Spot operation/subprofile",0};
    static const std::regex port("[A-Za-z_][A-Za-z_0-9]{0,31}");
    std::set<std::string> used;
    for (const auto* p : {&q.inputs,&q.outputs})
        for (auto& x : *p)
            if(!std::regex_match(x,port)||!used.insert(x).second)
                return {status::unsupported,"stock Spot atomic proposition list",0};
    machine s(*this,cap,true);
    s.argv={"ltlsynt","-F","-"};
    if(!q.inputs.empty())s.argv.push_back("--ins="+join(q.inputs));
    s.argv.push_back("--outs="+join(q.outputs));
    if(game){
        for(auto x:{"--polarity=no","--decompose=no","--global-equivalence=no","--bypass=no","--print-game-hoa"})
            s.argv.push_back(x);
        s.argv.push_back("--algo="+q.argument);
    }
    s.stdin_bytes.assign(q.source.begin(),q.source.end());s.stdin_bytes.push_back('\n');
    if(auto bad=run_spot(s,child,cap))return *bad;
    const std::string out=as_text(s.stdout_bytes);
    bytes payload;
    if(game&&s.exited==0&&out.starts_with("HOA:")){
        append(payload,out);return {status::finished,"",cap-s.left(),std::move(payload)};
    }
    if(!game&&s.exited==0&&out.starts_with("REALIZABLE\nHOA:")){
        payload.push_back(1);append(payload,std::string_view(out).substr(11));
        return {status::finished,"",cap-s.left(),std::move(payload)};
    }
    if(!game&&s.exited==1&&out=="UNREALIZABLE\n"){
        payload.resize(5);return {status::finished,"",cap-s.left(),std::move(payload)};
    }
    return {status::operational,"stock Spot exit without admitted result",cap-s.left()};
}

namespace {
outcome<uint64_t> parent_boot(module& parent,uint64_t raw_size) {
    std::optional<uint64_t> pages,slots;
    wasm_exporttype_vec_t exports{};
    wasmtime_module_exports(parent.m,&exports);
    for(size_t i=0;i<exports.size;i++){
        const auto name=wasm_exporttype_name(exports.data[i]);
        const std::string_view n{name->data,name->size};
        const auto t=wasm_exporttype_type(exports.data[i]);
        if(n=="memory"&&wasm_externtype_kind(t)==WASM_EXTERN_MEMORY)
            pages=wasmtime_memorytype_minimum(wasm_externtype_as_memorytype_const(t));
        if(n=="__indirect_function_table"&&wasm_externtype_kind(t)==WASM_EXTERN_TABLE)
            slots=wasmtime_tabletype_minimum(wasm_externtype_as_tabletype_const(t));
    }
    wasm_exporttype_vec_delete(&exports);
    M_REQUIRE(pages&&slots&&*pages<=1024&&*slots<=65536,"unqualified QLT boot descriptors");
    return 1+blocks(raw_size)+blocks(65536 * (*pages))+*slots;
}
}

outcome<void> profile::open(const std::filesystem::path& root) {
    auto cfg=wasm_config_new();
    M_REQUIRE(cfg,"runtime configuration unavailable");
    wasmtime_config_parallel_compilation_set(cfg,false);
    wasmtime_config_cranelift_opt_level_set(cfg,WASMTIME_OPT_LEVEL_NONE);
    wasmtime_config_wasm_exceptions_set(cfg,true);
    engine.reset(wasm_engine_new_with_config(cfg));
    M_REQUIRE(bool(engine),"runtime engine unavailable");
    {
        M_GET(raw,load(root/"tau.raw.wasm", "6cda21644e326bac9b2108c7590f059585583339d42c93dee26dec0693012f50", 31395770));
        M_GET(grouped,load(root/"tau.guarded.wasm", "cbec8b45c1d01790ca986fd2836ee676f295566b8f9fbe936f0fcf0d5fafdc76", 106447147));
        M_TRY(link(engine.get(),core,grouped));
        M_GET(charge,parent_boot(core,raw.size()));
        boot=charge;
    }
    {
        M_GET(stock,load(root/"ltlsynt.guarded.wasm", "fdcb835f6fb21fd4bdb3f7db1c7a90091d0d2846a89f77dabdcfcd2207a04713", 14300447));
        M_TRY(link(engine.get(),child,stock));
    }
    return load_filters(root);
}

receipt profile::invoke(op command,view args,uint64_t budget,std::optional<snapshot>& active) {
    if(args.size()>packet_limit||(command==op::build)==active.has_value()){
        receipt r; r.completion=status::invalid;return r;
    }
    machine g(*this,budget,false);
    const auto refused=[&](status s,std::string why){
        g.r.completion=s;
        g.r.used=budget-g.left();
        if(s==status::operational) {g.r.diagnostic_used=g.r.used;g.r.used=0;}
        g.r.phase=g.stage; g.r.why=std::move(why);
        return std::move(g.r);
    };
    const auto attempt=[&]() -> outcome<void> {
        M_TRY(g.spend(boot));
        M_TRY(g.attach(core));
        if(active){
            g.stage="restore";M_TRY(g.restore(*active));
        }else {
            g.stage="reactor_init";M_TRY(g.wasm("_initialize",{},false));
        }
        g.stage="ingress";
        M_TRY(g.spend(1+blocks(args.size())));
        M_GET(ptr,g.wasm("tau_request_ptr"));
        M_TRY(g.write(ptr,args));
        g.stage="decode";
        M_GET(r,g.wasm("tau_dispatch",{i32(int(command)),i32(args.size())}));
        g.stage="egress";
        M_GET(raw_ptr,g.wasm("tau_reply_ptr"));
        M_GET(raw_size,g.wasm("tau_reply_size"));
        const uint32_t p=raw_ptr,n=raw_size;
        if(n>packet_limit)return fail(status::unsupported,"reply bound");
        M_TRY(g.spend(1+blocks(n)));
        M_GET(body,g.slice(p,n));
        bytes staged(body.begin(),body.end());
        bool commit=r==0&&command!=op::inspect;
        if(commit){
            g.stage="publish";M_TRY(g.spend(1));
            if(command==op::update){
                M_REQUIRE(n>=8&&parse32(staged)==n-8,"update reply shape");
                const auto v=parse32(view(staged).last(4));
                M_REQUIRE(v<=1,"update reply shape");commit=v==1;
            }
        }
        g.r.used=budget-g.left();
        if(commit){
            M_GET(stack,g.glob("__stack_pointer"));
            // Clear the ephemeral callback binding before retaining inert memory.
            wasmtime_context_set_data(g.ctx,nullptr);
            active.emplace(snapshot{std::move(g.store),g.ctx,g.memory,stack});
        }
        g.r.completion=status::finished;
        g.r.tau_code=r;
        g.r.value=std::move(staged);
        return {};
    };
    try {
        auto result=attempt();
        if(result)return std::move(g.r);
        const auto& why=result.error();
        if(why.trap) {
            if(g.stopped)return refused(g.stopped->code,g.stopped->why);
            if(g.live&&g.glob("tau_meter_stopped").value_or(0)==1) {
                g.r.site=g.glob("tau_meter_stop_site").value_or(0);
                return refused(status::budget,"guest charge");
            }
            if(g.capacity_failed())return refused(status::operational,"runtime capacity");
        }
        return refused(why.code,why.why);
    }catch(const std::bad_alloc&){return refused(status::operational,"host capacity");}
}

struct interpreter::core {
    std::shared_ptr<profile> p;
    std::optional<snapshot> state;
    mutable std::mutex guard;
    explicit core(std::shared_ptr<profile> p):p(std::move(p)){}
};
interpreter::interpreter(std::unique_ptr<core> p):private_(std::move(p)){}
interpreter::~interpreter()=default;
interpreter::interpreter(interpreter&&) noexcept=default;
interpreter& interpreter::operator=(interpreter&&) noexcept=default;
static receipt invalid() { receipt ret; ret.completion=status::invalid;return ret; }
static view view_of(std::string_view s) {return {reinterpret_cast<const uint8_t*>(s.data()),s.size()};}
receipt interpreter::update(std::string_view s,uint64_t cap) {
    if(!private_)return invalid();
    std::lock_guard lock(private_->guard);
    return private_->p->invoke(op::update,view_of(s),cap,private_->state);
}
receipt interpreter::inspect(uint64_t cap) const {
    if(!private_)return invalid();
    std::lock_guard lock(private_->guard);
    return private_->p->invoke(op::inspect,{},cap,private_->state);
}
receipt interpreter::step(std::span<const input> in,uint64_t cap) {
    if(!private_)return invalid();
    std::lock_guard lock(private_->guard);
    uint64_t prefix=0;
    std::string_view phase="typed.admit";
    const auto spend=[&](uint64_t n){
        if(n>cap-prefix)return false;
        prefix+=n;return true;
    };
    const auto staged_invalid=[&]{
        auto r=invalid();r.used=prefix;r.phase=phase;return r;
    };
    const auto capped=[&]{
        receipt r; r.completion=status::budget;r.used=prefix;r.phase=phase;r.why="typed input charge";return r;
    };
    try {
        if(!spend(1))return capped();
        if(in.size()>(packet_limit-4)/12)return staged_invalid();
        phase="typed.describe";
        size_t length=4;
        for(const auto& i:in){
            if(!spend(1))return capped();
            if(i.name.size()>packet_limit||i.value.size()>packet_limit)return staged_invalid();
            length+=12+i.name.size()+i.value.size();
            if(length>packet_limit)return staged_invalid();
        }
        phase="typed.order";
        if(!spend(1+blocks(in.size()*16)))return capped(); // Two logical arrays of 64-bit indices.
        std::vector<size_t> from(in.size()), to(in.size());
        for(size_t i=0;i<in.size();i++)from[i]=i;
        const auto compare=[&](size_t a,size_t b) -> std::optional<int> {
            const auto& x=in[a]; const auto& y=in[b];
            const auto width=std::min(x.name.size(),y.name.size());
            if(!spend(1+blocks(width)))return {}; // deterministic bound before comparing bytes
            for(size_t i=0;i<width;i++){
                if(uint8_t(x.name[i])<uint8_t(y.name[i]))return -1;
                if(uint8_t(x.name[i])>uint8_t(y.name[i]))return 1;
            }
            if(x.name.size()!=y.name.size())return x.name.size()<y.name.size()?-1:1;
            if(x.time!=y.time)return x.time<y.time?-1:1;
            return 0;
        };
        for(size_t span=1;span<in.size();span*=2){
            for(size_t base=0;base<in.size();base+=2*span){
                size_t i=base, middle=std::min(base+span,in.size()), j=middle;
                const size_t end=std::min(base+2*span,in.size());
                for(size_t k=base;k<end;k++){
                    if(!spend(1))return capped();
                    bool use_left=j==end;
                    if(i<middle&&j<end){
                        const auto order=compare(from[i],from[j]);
                        if(!order)return capped();
                        if(!*order)return staged_invalid();
                        use_left=*order<0;
                    }
                    to[k]=use_left?from[i++]:from[j++];
                }
            }
            from.swap(to);
        }
        phase="typed.write";
        if(!spend(1+blocks(length)))return capped();
        bytes wire; wire.reserve(length); append(wire,uint32_t(in.size()));
        for(size_t i:from){append(wire,in[i].name);append(wire,in[i].time);append(wire,in[i].value);}
        auto r=private_->p->invoke(op::step,wire,cap-prefix,private_->state);
        if(r.completion!=status::operational)r.used+=prefix;
        else if(r.diagnostic_used)*r.diagnostic_used+=prefix;
        for(auto& m:r.trace)m.used+=prefix;
        for(auto& a:r.solvers)a.charged_before+=prefix;
        return r;
    }catch(const std::bad_alloc&){
        receipt r;r.completion=status::operational;r.diagnostic_used=prefix;
        r.phase=phase;r.why="host typed-input capacity";return r;
    }
}
engine::engine(const std::filesystem::path& root) {
    receipt startup;
    startup.phase="profile";
    startup.completion=status::operational;
    try {
        auto preparing=std::make_shared<profile>();
        if(auto result=preparing->open(root);result)profile_=std::move(preparing);
        else startup.why=std::move(result.error().why);
    }catch(const std::bad_alloc&){startup.why="host capacity";}
    if(!profile_)startup_error_=std::move(startup);
}
admission engine::get_interpreter(std::string_view s,uint64_t cap) const {
    if(!profile_)return {startup_error_.value_or(invalid()),std::nullopt};
    if(s.size()>packet_limit)return {invalid(),std::nullopt};
    if(cap<profile_->boot) {
        receipt r;r.completion=status::budget;r.phase="instantiate";r.why="host charge";
        return {std::move(r),std::nullopt};
    }
    try {
        auto x=std::make_unique<interpreter::core>(profile_);
        auto r=profile_->invoke(op::build,view_of(s),cap,x->state);
        if(!x->state)return {std::move(r),std::nullopt};
        return {std::move(r),interpreter(std::move(x))};
    } catch(const std::bad_alloc&) {
        receipt r;r.completion=status::operational;r.diagnostic_used=0;
        r.phase="instantiate";r.why="host capacity";
        return {std::move(r),std::nullopt};
    }
}
}
