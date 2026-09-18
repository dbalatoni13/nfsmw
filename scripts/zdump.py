#!/usr/bin/env python3
"""Vuelca el desensamblado original de varios simbolos de zLua fundido con la linea de fuente."""
import os,re,sys
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
RE_INSN=re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\s*(.*?)\s*$')
RE_LINE=re.compile(r'^0x([0-9A-Fa-f]+): (\S+) \(line (\d+)\)')
lines={}
for l in open(os.path.join(ROOT,'symbols','debug_lines.txt'),encoding='utf-8',errors='replace'):
    m=RE_LINE.match(l)
    if m: lines[int(m.group(1),16)]=(m.group(2),int(m.group(3)))
p=os.path.join(ROOT,'build','GOWE69','asm','Speed','Indep','SourceLists','zLua.s')
txt=open(p,encoding='utf-8',errors='replace').read().split('\n')
syms=sys.argv[1:]
for sym in syms:
    inside=False; cur=None
    print('=== %s ==='%sym)
    for l in txt:
        if l.startswith('.fn %s,'%sym): inside=True; continue
        if inside and l.startswith('.endfn'): break
        if inside:
            m=RE_INSN.match(l.rstrip())
            if m:
                a=int(m.group(1),16)
                if a in lines:
                    f,n=lines[a]
                    if (f,n)!=cur:
                        cur=(f,n); print('  --- %s:%d ---'%(f,n))
                print('    %s'%m.group(2))
            elif l.strip().endswith(':') and not l.startswith('/*') and l.strip().startswith('.L'):
                print('  %s'%l.strip())
