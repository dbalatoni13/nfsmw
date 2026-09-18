import json, subprocess, sys, os, time
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SC='C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad'
unit=sys.argv[1]; pats=sys.argv[2:]
out=f'{SC}/p_{unit}_{os.getpid()}.json'
r=subprocess.run([f'{ROOT}/objdiff-cli-windows-x86_64.exe','diff',
  '-1',f'{ROOT}/build/GOWE69/obj/Speed/Indep/SourceLists/{unit}.o',
  '-2',f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o',
  '-c','function_reloc_diffs=none','-c','ppc.calculatePoolRelocations=false','-o',out,'--format','json'],capture_output=True,text=True)
if r.returncode!=0:
    print(r.stdout[-1500:]); print(r.stderr[-1500:]); sys.exit(1)
d=json.load(open(out))
L={s.get('name'):s for s in d['left']['symbols']}
R={s.get('name'):s for s in d['right']['symbols']}
for n,s in L.items():
    if not n: continue
    if pats and not any(p in n for p in pats): continue
    o=R.get(n)
    if o is None:
        print('%-70s MISSING size=%s'%(n[:70], s.get('size'))); continue
    ndiff=sum(1 for a,b in zip(s.get('instructions',[]),o.get('instructions',[])) if (a.get('diff_kind') or 'DIFF_NONE')!='DIFF_NONE' or (b.get('diff_kind') or 'DIFF_NONE')!='DIFF_NONE')
    print('%-70s %10.5f%%  size=%s/%s  diffs=%d'%(n[:70], o.get('match_percent') or 0, s.get('size'), o.get('size'), ndiff))
os.remove(out)
