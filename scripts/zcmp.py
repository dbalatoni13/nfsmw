#!/usr/bin/env python3
"""Compara nuestro .s (de s_zlua.py) contra el .s original, funcion a funcion."""
import os,re,sys
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
SCR=r'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad'
sym=sys.argv[1]; tag=sys.argv[2] if len(sys.argv)>2 else 'a'
RE_INSN=re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\s*(.*?)\s*$')
def orig(sym):
    p=os.path.join(ROOT,'build','GOWE69','asm','Speed','Indep','SourceLists','zLua.s')
    out=[];inside=False
    for l in open(p,encoding='utf-8',errors='replace'):
        if l.startswith('.fn %s,'%sym): inside=True; continue
        if inside and l.startswith('.endfn'): break
        if inside:
            m=RE_INSN.match(l.rstrip('\n'))
            if m: out.append(m.group(2))
    return out
def ours(sym,tag):
    p=os.path.join(SCR,'zlua_%s.s'%tag)
    out=[];inside=False
    for l in open(p,encoding='utf-8',errors='replace'):
        s=l.rstrip('\n')
        if re.match(r'^%s:\s*$'%re.escape(sym),s): inside=True; continue
        if inside and (s.startswith('\t.size') or (re.match(r'^\S+:\s*$',s) and not s.strip().startswith('.L'))): break
        if inside:
            t=s.strip()
            if not t or t.startswith('.') and not t.startswith('.L'): continue
            if t.startswith('.L'): continue
            out.append(t)
    return out
BC={('12','2'):'beq',('4','2'):'bne',('12','0'):'blt',('4','0'):'bge',('12','1'):'bgt',('4','1'):'ble',('12','3'):'bso',('4','3'):'bns'}
def norm(t):
    t=t.split('#')[0].strip()
    t=re.sub(r'\s+',' ',t.replace('\t',' '))
    m=re.match(r'^bc (\d+),(\d+),(.*)$',t)
    if m: t='%s %s'%(BC.get((m.group(1),m.group(2)),'bc?'),m.group(3))
    m=re.match(r'^bclr (\d+),(\d+)$',t)
    if m: t='%slr'%BC.get((m.group(1),m.group(2)),'bc?')
    t=re.sub(r'\.L[A-Za-z0-9_]+','LBL',t)
    t=re.sub(r'\br(\d+)\b',r'\1',t)
    t=re.sub(r'\bf(\d+)\b',r'F\1',t)
    t=re.sub(r'-0x([0-9a-fA-F]+)',lambda m:str(-int(m.group(1),16)),t)
    t=re.sub(r'0x([0-9a-fA-F]+)',lambda m:str(int(m.group(1),16)),t)
    t=re.sub(r'^la ','addi ',t)
    t=re.sub(r'^subi (\S+), (\S+), (\d+)',lambda m:'addi %s, %s, -%s'%(m.group(1),m.group(2),m.group(3)),t)
    t=re.sub(r'^subic\. (\S+), (\S+), (\d+)',lambda m:'addic. %s, %s, -%s'%(m.group(1),m.group(2),m.group(3)),t)
    t=re.sub(r'(-?\d+)\((\d+)\)',lambda m:'['+m.group(2)+'+'+m.group(1)+']',t)
    t=re.sub(r'[ ,]+',' ',t)
    t=re.sub(r'^(cmp\w*) 0 ',lambda m:m.group(1)+' ',t)
    t=re.sub(r'^addi (\S+) (\S+)\((\d+)\)$',lambda m:'addi '+m.group(1)+' '+m.group(3)+' '+m.group(2),t)
    return t
A=[norm(x) for x in orig(sym)]; B=[norm(x) for x in ours(sym,tag)]
bad=sum(1 for i in range(max(len(A),len(B))) if (A[i] if i<len(A) else '')!=(B[i] if i<len(B) else ''))
print('# %s   orig=%d  ours=%d  diff=%d'%(sym,len(A),len(B),bad))
for i in range(max(len(A),len(B))):
    a=A[i] if i<len(A) else ''
    b=B[i] if i<len(B) else ''
    print('%s %-46s %s'%(' ' if a==b else '|',a,b))
