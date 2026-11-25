"""Verify or locally provision the pinned work-budget reactor images."""
import argparse,hashlib,json,shutil,subprocess,sys,tempfile
from pathlib import Path
here=Path(__file__).resolve().parent
manifest=json.loads((here/'profile.manifest.json').read_text())
raw=json.loads((here/'raw-inputs.json').read_text())
def digest(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def verify(d):
    for name, rec in manifest['images'].items():
        f=d/name
        if not f.is_file() or f.stat().st_size!=rec['length'] or digest(f)!=rec['sha256']:
            raise RuntimeError('unqualified profile image: '+name)
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory',type=Path)
    parser.add_argument('--check',action='store_true')
    parser.add_argument('--objdump',type=Path)
    for role in raw:parser.add_argument('--'+role,type=Path)
    args=parser.parse_args()
    dest=args.directory.resolve()
    if args.check:
        verify(dest);print('WORK_BUDGET_PROFILE_EXACT');return
    if not args.objdump or any(getattr(args,k) is None for k in raw):
        parser.error('provisioning needs the pinned disassembler and all four raw modules')
    if dest.exists():parser.error('destination already exists')
    for r,h in raw.items():
        if digest(getattr(args,r))!=h:parser.error('unqualified raw '+r)
    with tempfile.TemporaryDirectory(dir=dest.parent) as work:
        w=Path(work)
        stage=w/'images';stage.mkdir()
        shutil.copyfile(args.tau,stage/'tau.raw.wasm')
        for role in raw:
            p=w/'grouped.wasm'
            subprocess.run([sys.executable,str(here/'meter_ordinal.py'),'--group',
                            str(args.objdump.resolve()),str(getattr(args,role).resolve()),str(p)],check=True)
            subprocess.run([sys.executable,str(here/'guard_growth.py'),str(p),
                            str(stage/(role+'.guarded.wasm'))],check=True)
            p.unlink()
        verify(stage)
        stage.rename(dest)
    print('WORK_BUDGET_PROFILE_EXACT')
if __name__=='__main__':main()
