"""Typed description has a narrower contract than interpreter admission."""

import sys
import tau_loader as tau

if not any(n.startswith("bv-") for n in tau.option_names()):
    sys.exit(77)

def bv(n, bits):
    return "{ #x%0*x }:bv[%d]" % (bits // 4, n, bits)

def issue_spec(k, w=16):
    ids = dict(zip((18, 19, 20), (3*k+1, 3*k+2, 3*k+3)))
    blocked = " || ".join(
        "(i1[t]:bv[24] > %s && !(i%d[t]:bv[%d] = %s))" % (bv(limit, 24), slot, w, bv(ids[slot], w))
        for limit, slot in ((100000,20),(10000,19),(1000,18)))
    rule = "( %s ) ? (o%d[t]:bv[24] = %s) : (o%d[t]:bv[24] = %s)" % (
        blocked, 5, bv(0,24), 5, bv(1,24))
    return "always ( %s )." % rule

def verify(source, reads, output=("o5", 24)):
    r = tau.describe(source)
    tag = r.pop("report")
    assert not tag.has_error and r["status"] == "Described", (source, r, tag.errors)
    assert r["functional_shape"] == "proved_for_subset" and r["required_history"] == max([0]+[lag for _,lag in reads.values()])
    assert r["reads"] == {n:{"width":width,"deepest_lookback":lag,"domain":"bv"} for n,(width,lag) in reads.items()}, (source,r)
    assert r["writes"] == {output[0]:{"width":output[1],"domain":"bv"}}
    return r

# This is the single-rule example printed in the body of IDNI/tau-lang #211.
ISSUE_211_PRINTED = """always ( ( (i1[t]:bv[24] > { #x0186a0 }:bv[24] && !(i20[t]:bv[16] = { #x0003 }:bv[16]))
        || (i1[t]:bv[24] > { #x002710 }:bv[24] && !(i19[t]:bv[16] = { #x0002 }:bv[16]))
        || (i1[t]:bv[24] > { #x0003e8 }:bv[24] && !(i18[t]:bv[16] = { #x0001 }:bv[16])) )
         ? (o5[t]:bv[24] = { #x000000 }:bv[24])
         : (o5[t]:bv[24] = { #x000001 }:bv[24]) )."""

holds = {
 "issue_211_printed":(ISSUE_211_PRINTED,{"i1":(24,0),"i18":(16,0),"i19":(16,0),"i20":(16,0)}),
 "issue_211_generator_16":(issue_spec(1),{"i1":(24,0),"i18":(16,0),"i19":(16,0),"i20":(16,0)}),
 "translated_384":(issue_spec(1,384),{"i1":(24,0),"i18":(384,0),"i19":(384,0),"i20":(384,0)}),
 "lookback":"always ((i1[t-2]:bv[24] > i1[t-4]:bv[24]) ? (o5[t]:bv[24] = i2[t-5]:bv[24]) : (o5[t]:bv[24] = i2[t]:bv[24])).",
 "inferred_input":"always (o5[t]:bv[24] = i1[t]).",
 "inferred_output":"always (o5[t] = i1[t]:bv[24]).",
 "alias":"type amt_primary is bv[24]. always (o5[t]:amt_primary = i1[t]:bv[24]).",
 "constant":"always (o5[t]:bv[24] = { #x000001 }:bv[24]).",
}
for name in ("inferred_input","inferred_output","alias"):
    holds[name]=(holds[name],{"i1":(24,0)})
holds["lookback"]=(holds["lookback"],{"i1":(24,4),"i2":(24,5)})
holds["constant"]=(holds["constant"],{})
refuse={
 "future":"always (o5[t]:bv[24] = i1[t+1]:bv[24]).",
 "absolute":"always (o5[t]:bv[24] = i1[0]:bv[24]).",
 "eventually":"sometimes (o5[t]:bv[24] = i1[t]:bv[24]).",
 "relational":"always ((i1[t]:bv[24] = { 1 }:bv[24] && o5[t]:bv[24] = { 1 }:bv[24]) || (i1[t]:bv[24] != { 1 }:bv[24] && o5[t]:bv[24] = { 0 }:bv[24])).",
 "multi_output":"always ((o5[t]:bv[24] = i1[t]:bv[24]) && (o6[t]:bv[24] = i1[t]:bv[24])).",
 "mixed_compare_width":"always (o5[t]:bv[24] = i1[t]:bv[16]).",
 "invalid":"always (o5[t]:bv[24] =",
 "future_after_other_call":"always (o5[t]:bv[24] = i1[t+1]).",
}
def test_metadata():
    seen={}
    for name,(src,reads) in holds.items():
        seen[name] = verify(src, reads)
    for name,src in refuse.items():
        r=tau.describe(src)
        del r["report"]
        assert r["status"]=="Unsupported" and r["functional_shape"]=="unknown" and "reads" not in r and "writes" not in r,(name,r)
        seen[name]=r

if __name__ == "__main__":
    test_metadata()
