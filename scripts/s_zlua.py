#!/usr/bin/env python3
"""Compila el PREFIJO de zLua.cpp hasta el .cpp indicado, a ensamblador (-S).
   uso: python scripts/s_zlua.py <substring del include> [tag]
"""
import os,re,subprocess,sys,time
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
SCR=r'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad'
sys.path.insert(0,os.path.join(ROOT,'scripts'))
import build_direct as b
os.chdir(ROOT)
units=b.parse_units(); src,out,cflags,tc,rn=units['zLua']
cc=os.path.join(ROOT,'build','compilers',tc,'ngccc.exe')
env=dict(os.environ); env['SN_NGC_PATH']=os.path.join(ROOT,'build','compilers',tc)
key=sys.argv[1] if len(sys.argv)>1 else 'LuaGameHooks'
tag=sys.argv[2] if len(sys.argv)>2 else 'a'
lines=open(src).read().split('\n')
outl=[];hit=False
for l in lines:
    outl.append(l)
    if l.startswith('#include') and key in l:
        hit=True; break
if not hit:
    print('no match for',key); sys.exit(1)
os.makedirs(SCR,exist_ok=True)
w=os.path.join(SCR,'zlua_%s.cpp'%tag)
open(w,'w').write('\n'.join(outl)+'\n')
asm=os.path.join(SCR,'zlua_%s.s'%tag)
cmd=[cc]+[c for c in cflags if c!='-gdwarf+']+['-S','-o',asm,w]
t=time.time()
p=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True)
el=time.time()-t
errs=[x for x in (p.stdout+p.stderr).split('\n') if ' error' in x or 'error:' in x or 'parse error' in x or 'syntax error' in x or 'no matching' in x or 'has no member' in x or 'undeclared' in x]
if p.returncode!=0:
    print('\n'.join(errs[:40])); print('FAIL %.1fs'%el); sys.exit(1)
print('OK %.1fs -> %s  (%d includes)'%(el,asm,len(outl)))
