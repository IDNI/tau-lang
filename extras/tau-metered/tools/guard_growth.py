"""Diagnostic memory-capacity shim: preserve the original published work tariff."""
import argparse
import hashlib
import pathlib

def enc(n):
    b=bytearray()
    while n>=128:
        b.append((n&127)|128)
        n >>= 7
    return bytes(b)+bytes([n])

def si(n):
    assert n>=0
    b=bytearray()
    while n>=64:
        b.append((n&127)|128)
        n >>= 7
    return bytes(b)+bytes([n])

def dec(b,i=0):
    n=0
    for sh in range(0,70,7):
        x=b[i]; i+=1
        n|=(x&127)<<sh
        if x<128:
            return n,i
    raise ValueError('bad ULEB')

def section(k,b): return bytes((k,))+enc(len(b))+b
def vector_append(v,s):
    size,start=dec(v)
    return enc(size+1)+v[start:]+s

def guarded(src):
    assert src[:8]==b'\0asm\x01\0\0\0'
    sections={}
    p=8
    while p<len(src):
        k=src[p]
        n,start=dec(src,p+1)
        assert k and k not in sections and start+n<=len(src)
        sections[k]=src[start:start+n]
        p=start+n
    b=sections[5]
    count,p=dec(b); flags,p=dec(b,p)
    low,p=dec(b,p); high,p=dec(b,p)
    assert count==1 and flags==1 and p==len(b) and 0<low<=high<=4096
    global_num=dec(sections[6])[0]
    export=b'tau_capacity_failed'
    assert export not in sections[7]
    sections[6]=vector_append(sections[6],b'\x7f\x01\x41\x00\x0b')
    sections[7]=vector_append(sections[7],enc(len(export))+export+b'\x03'+enc(global_num))
    b=sections[10]; count,p=dec(b)
    new=bytearray(enc(count))
    hit=[]
    kprefix=b'\x00\x20\x00\xad\x42'+enc(2048)+b'\x7e\x42\x01\x7c\x20\x01\x10'
    for i in range(count):
        n,p=dec(b,p); body=b[p:p+n]; p+=n
        # Fully match ONLY the previously audited generated wrapper.
        if body.startswith(kprefix):
            _,end=dec(body,len(kprefix))
            if body[end:]==b'\x20\x00\x40\x00\x0b':
                # Add local result. If grow returned -1 though delta was in
                # [0, max-current] unsigned, the runtime—not guest policy—
                # denied it. Trap uncatchably after setting the typed flag.
                trailer=(b'\x22\x02\x41\x7f\x46\x04\x40'
                         + b'\x20\x00\x41'+si(high)+b'\x3f\x00\x6b\x4d\x04\x40'
                         + b'\x41\x01\x24'+enc(global_num)+b'\x00\x0b\x0b\x20\x02\x0b')
                body=b'\x01\x01\x7f'+body[1:-1]+trailer
                hit.append(i)
        new+=enc(len(body))+body
    assert p==len(b) and len(hit)==1 and hit[0]>=count-7,hit
    sections[10]=bytes(new)
    result=src[:8]+b''.join(section(k,v) for k,v in sections.items())
    return result, (low,high,hit[0],global_num)

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('source',type=pathlib.Path)
    p.add_argument('target',type=pathlib.Path)
    a=p.parse_args()
    old=a.source.read_bytes()
    new,proof=guarded(old)
    a.target.write_bytes(new)
    print(dict(baseline=hashlib.sha256(old).hexdigest(),guarded=hashlib.sha256(new).hexdigest(),guard=proof))
