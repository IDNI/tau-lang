"""Wasm32 instruction tariff, LLVM disassembler fully byte-audited.
This tool does not instrument host work; its linker must price or deny imports.
Unaudited opcodes are refused. Result/generation are staged outside this tool."""
import argparse,hashlib,pathlib,re,subprocess
def u(n):
 b=bytearray()
 while n>=128: b.append((n&127)|128); n>>=7
 b.append(n);return bytes(b)
def si(n):
 b=bytearray()
 while n>=64:
  b.append((n&127)|128);n>>=7
 return bytes(b)+bytes((n,))
def pure(m):
 # No control joins, call-outs, global/linear-memory writes, or trapping math.
 return m.startswith("local.") or m in ("global.get","drop") or m.endswith(".select") or (
  m.startswith(("i32.","i64.","f32.","f64.")) and not any(k in m for k in (".load",".store","atomic")) and
  (not m.startswith(("i32.div","i64.div","i32.rem","i64.rem","i32.trunc_","i64.trunc_")) or "trunc_sat" in m))
def val(b,i):
 n=0
 for sh in range(0,70,7):
   x=b[i]; i+=1
   n|=(x&127)<<sh
   if x<128:return n,i
 raise ValueError('LEB too long')
def text(b,i):
 n,i=val(b,i); j=i+n; return b[i:j].decode(),j
def section(k,b):return bytes((k,))+u(len(b))+b
def index(op,n):return bytes((op,))+u(n)
def name(s):return u(len(s))+s
def vec(b,extras):
 n,i=val(b,0);return u(n+len(extras))+b[i:]+b''.join(extras)
def body(b):
 b=b'\x00'+b+b'\x0b';return u(len(b))+b
def instructions(cmd,p):
 pat=re.compile(r'^\s+([0-9A-Fa-f]+):\s+((?:[0-9A-Fa-f]{2}\s+)+)(\S+)')
 with subprocess.Popen([cmd,'-d',str(p)],stdout=subprocess.PIPE,text=True) as proc:
  for line in proc.stdout:
   m=pat.match(line)
   if m:
    yield int(m[1],16),bytes.fromhex(m[2]),m[3]
  if proc.wait():raise ValueError('disassembler failed')
def rewrite(argv=None):
 a=argparse.ArgumentParser()
 a.add_argument('--group',action='store_true')
 a.add_argument('objdump');a.add_argument('module',type=pathlib.Path);a.add_argument('output',type=pathlib.Path)
 a=a.parse_args(argv)
 b=a.module.read_bytes(); assert b[:8]==b'\0asm\1\0\0\0'
 pos=8; sections=[]; data={}
 while pos<len(b):
  k=b[pos]; n,j=val(b,pos+1);end=j+n
  assert end<=len(b)
  sections.append((k,j,end))
  if k!=0: assert k not in data;data[k]=(j,end)
  pos=end
 assert 8 not in data, 'start functions cannot be staged by this prototype'
 def contents(k):
    j,end=data[k];return b[j:end]
 imports=0
 if 2 in data:
  z=contents(2); n,i=val(z,0)
  for _ in range(n):
    host,i=text(z,i); field,i=text(z,i)
    assert z[i]==0,(host,field,'only function imports permitted')
    _,i=val(z,i+1);imports+=1
  assert i==len(z)
 assert 5 in data and val(contents(5),0)[0]==1, 'one private linear memory required'
 mem=contents(5)
 _,i=val(mem,0); flags,i=val(mem,i)
 assert flags==1, 'bounded wasm32 nonshared linear memory required'
 _,i=val(mem,i); maxpages,i=val(mem,i); assert i==len(mem) and maxpages<=4096
 types,_=val(contents(1),0)
 fs,_=val(contents(3),0)
 gs,_=val(contents(6),0)
 cap,stop,site=gs,gs+1,gs+2
 charge=imports+fs
 # Charge routine: if cost > remaining, mark deterministic logical exhaustion and trap;
 # otherwise remaining -= cost. Wasm unreachable is not a catchable language exception.
 get=lambda i:index(0x23,i)
 setg=lambda i:index(0x24,i)
 loc=lambda i:index(0x20,i)
 call=lambda i:index(0x10,i)
 failure=b'\x41\x01'+setg(stop)+b'\x00'
 entry=loc(0)+get(cap)+b'\x56\x04\x40'
 debit=get(cap)+loc(0)+b'\x7d'+setg(cap)
 exact=body(entry+loc(1)+setg(site)+failure+b'\x0b'+debit)
 partial=body(entry+loc(1)+get(cap)+b'\xa7\x6a'+setg(site)+b'\x42\x00'+setg(cap)+failure+b'\x0b'+debit)
 ts=[b'\x60\x02\x7e\x7f\x00']; ft=[u(types),u(types)]; extra=[exact,partial]
 bulk={}
 special={'memory.copy','memory.fill','memory.init','memory.grow'}
 markers={'else','end','catch','catch_all'}
 i32=lambda n:b'\x41'+si(n)
 def get_wrapper(op,raw):
  if raw in bulk:return bulk[raw]
  k=imports+fs+len(extra)
  bulk[raw]=k
  # Descriptor base + ceil(bytes/32), or pages times 2048 base units.
  # Operands are already on stack; a call with the exact operand signature wraps originals.
  if op=='memory.grow':
   ts.append(b'\x60\x02\x7f\x7f\x01\x7f')
   prefix=loc(0)+b'\xad\x42'+u(2048)+b'\x7e\x42\x01\x7c'
   args=loc(0); ordinal=loc(1)
  else:
   ts.append(b'\x60\x04\x7f\x7f\x7f\x7f\x00')
   prefix=loc(2)+b'\xad\x42\x1f\x7c\x42\x05\x88\x42\x01\x7c'
   args=loc(0)+loc(1)+loc(2); ordinal=loc(3)
  ft.append(u(types+len(ts)-1))
  extra.append(body(prefix+ordinal+call(charge)+args+raw))
  return k
 j,end=data[10];count,p=val(b,j);assert count==fs
 ops=iter(instructions(a.objdump,a.module)); op=next(ops,None)
 out=[]; events=0; dynamic=0; elided=0
 for fn in range(count):
   sz,p=val(b,p); last=p+sz
   assert last<=end
   nlocals,q=val(b,p)
   for _ in range(nlocals):
    _,q=val(b,q);t=b[q];q+=1
    if t in (0x63,0x64):_,q=val(b,q)
   code=bytearray(b[p:q])
   run=bytearray(); run_len=0; run_start=0
   def flush():
    nonlocal run_len,elided
    if run_len:
     code.extend(b'\x42'+si(run_len)+i32(run_start)+call(charge+1)+run)
     elided+=run_len-1;run.clear();run_len=0
   while q<last:
    assert op is not None and op[0]==q,(fn,q,op)
    _,raw,mnemonic=op; assert raw and b[q:q+len(raw)]==raw,(fn,q,op)
    assert q+len(raw)<=last and mnemonic not in ('<unknown>','unknown'),(fn,q,op)
    if a.group and pure(mnemonic):
     if not run_len:run_start=events+1
     run.extend(raw);run_len+=1
     if run_len==63:flush()
     events+=1;q+=len(raw);op=next(ops,None)
     continue
    flush()
    if mnemonic in markers:
      code+=raw
    elif mnemonic in special:
      dynamic+=1; code+=i32(events+1)+call(get_wrapper(mnemonic,raw))
    else:
      # These have operand-dependent work not covered by the one-instruction base.
      # Atomic/shared/V128/GC/table ops are outside the initial solver profile.
      assert not mnemonic.startswith(('table.','atomic.','memory.atomic','i8x','i16x','i32x','i64x','f32x','f64x','v128','struct.','array.')),(fn,q,mnemonic)
      code+=b'\x42\x01'+i32(events+1)+call(charge)+raw
    events+=1;q+=len(raw);op=next(ops,None)
   assert q==last and b[last-1]==0x0b
   out.append(u(len(code))+code);p=last
 assert p==end and op is None
 cn=[b'\x7e\x01\x42\x00\x0b']+[b'\x7f\x01\x41\x00\x0b']*2
 en=[name(b'tau_meter_remaining')+b'\x03'+u(cap),name(b'tau_meter_stopped')+b'\x03'+u(stop),name(b'tau_meter_stop_site')+b'\x03'+u(site)]
 exp=contents(7)
 assert all(x not in exp for x in [b'tau_meter_remaining',b'tau_meter_stopped'])
 replacements={1:vec(contents(1),ts),3:vec(contents(3),ft),6:vec(contents(6),cn),7:vec(exp,en),10:u(count+len(extra))+b''.join(out+extra)}
 # Custom sections (debug/name/producers) have no operational behavior; do not preserve stale offsets.
 result=b[:8]+b''.join(section(k,replacements.get(k,b[j:end])) for k,j,end in sections if k)
 a.output.write_bytes(result)
 print(dict(original_sha=hashlib.sha256(b).hexdigest(),metered_sha=hashlib.sha256(result).hexdigest(),sizes=(len(b),len(result)),functions=count,original_ops=events,dynamic_sites=dynamic,meter_wrappers=len(extra),maxpages=maxpages,elided=elided))
if __name__=='__main__':rewrite()
