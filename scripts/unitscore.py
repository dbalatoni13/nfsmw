import json,subprocess,sys,os
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
SCR='C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad'
unit=sys.argv[1]
filt=sys.argv[2] if len(sys.argv)>2 else None
out=f'{SCR}/d_{unit}.json'
r=subprocess.run([f'{ROOT}/objdiff-cli-windows-x86_64.exe','diff',
  '-1',f'{ROOT}/build/GOWE69/obj/Speed/Indep/SourceLists/{unit}.o',
  '-2',f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o',
  '-c','function_reloc_diffs=none','-c','ppc.calculatePoolRelocations=false','-o',out,'--format','json'],capture_output=True,text=True)
d=json.load(open(out))
syms=[s for s in d['left']['symbols'] if s.get('kind')=='SYMBOL_FUNCTION']
tot=sum(int(s['size']) for s in syms)
m100=[s for s in syms if (s.get('match_percent') or 0)>=100.0]
near=[s for s in syms if 70<=(s.get('match_percent') or 0)<100]
print('%s: fns=%d totcode=%d matched=%d (%d fns)  near70=%d (%dB)'%(unit,len(syms),tot,sum(int(s['size']) for s in m100),len(m100),len(near),sum(int(s['size']) for s in near)))
if filt:
    for s in syms:
        if filt in s.get('name','') or filt in (s.get('demangled_name') or ''):
            print('   %7sB %7.3f%% %s'%(s['size'],s.get('match_percent') or 0,s['name'][:80]))
