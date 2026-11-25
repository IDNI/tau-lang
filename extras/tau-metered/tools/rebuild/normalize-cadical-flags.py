"""Normalize a diagnostic-only compiler root reported by CaDiCaL."""
from pathlib import Path
import sys
root=Path(sys.argv[1]).resolve()
s=root/'src/cadical-rel-2.1.3-elevate/scripts/make-build-header.sh'
t=s.read_text()
p=t.splitlines(keepends=True)
i=[j for j,line in enumerate(p) if line.startswith('FLAGS=') and 'CXXFLAGS' in line]
assert len(i)==1
addition='FLAGS="$(printf \'%s\' "$FLAGS" | sed -e "s@${CVC_SOURCE_ROOT:?}@source-root@g")"\n'
if addition not in t:p.insert(i[0]+1,addition);s.write_text(''.join(p))
