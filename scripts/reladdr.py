#!/usr/bin/env python3
"""reladdr.py <unidad> -- la DIRECCION a la que apunta cada reubicacion HA16/LO16.

`fncmp` normaliza los nombres `$LCnnn`, asi que una referencia que apunta a OTRA
copia --misma cadena, otra direccion-- pasa su control y rompe el DOL en
silencio. Esto compara, para cada offset de cada funcion emparejada, el
SIMBOLO CONTENEDOR + delta en los dos objetos. Los `$LCnnn` cortos son el pool
que cc1plus internaliza y se cuentan aparte (`pendientes`).
"""
import sys,struct,re,collections
sys.path.insert(0,'scripts')
from rodata import load
def info(p):
    d,E,secs=load(p)
    st=[s for s in secs if s['typ']==2][0]; strt=secs[st['link']]
    syms=[]
    for i in range(st['size']//16):
        o=st['off']+i*16
        no,v,sz,inf,oth,sh=struct.unpack(E+'IIIBBH',d[o:o+16])
        e=d.index(b'\0',strt['off']+no)
        syms.append((d[strt['off']+no:e].decode('latin-1'),v,sz,inf,sh))
    bysec=collections.defaultdict(list)
    for nm,v,sz,inf,sh in syms:
        if sh in (0,0xfff1,0xfff2) or not nm: continue
        if nm.startswith('gcc2_compiled') or nm.endswith('.c') or nm.endswith('.cpp'): continue
        bysec[sh].append((v,sz,nm))
    for k in bysec: bysec[k].sort()
    def owner(sh,addr):
        best=None
        for v,sz,nm in bysec.get(sh,()):
            if v<=addr: best=(nm,addr-v)
            else: break
        return best
    ti=[i for i,s in enumerate(secs) if s['sname']=='.text'][0]
    fns=sorted([(v,sz,nm) for nm,v,sz,inf,sh in syms if sh==ti and (inf&0xf)==2])
    rel=[]
    for s in secs:
        if s['typ']!=4 or s['info']!=ti: continue
        for i in range(s['size']//s['entsize']):
            o=s['off']+i*s['entsize']
            r_off,r_info,add=struct.unpack(E+'IIi',d[o:o+12])
            if (r_info&0xff) not in (4,5,6): continue
            si=r_info>>8
            nm,v,sz,inf,sh=syms[si]
            rel.append((r_off,r_info&0xff,nm,owner(sh,v+add),(secs[sh]['sname'] if sh<len(secs) else '?')+'@%#x'%(v+add)))
    return fns,rel
u=sys.argv[1]
base='build/GOWE69/%s/Speed/Indep/SourceLists/'+u+'.o'
fa,ra=info(base%'obj'); fb,rb=info(base%'src')
ma={nm:(v,sz) for v,sz,nm in fa}; mb={nm:(v,sz) for v,sz,nm in fb}
def rd(rel,base_,size): return {o-base_:r for o,*r in ((o,t,n,ow,sn) for o,t,n,ow,sn in rel) if base_<=o<base_+size}
small=re.compile(r'^\$LC\d{1,6}$')
err=0; ok=0; pend=0; lines=[]
for nm,(vb,sb) in mb.items():
    if nm not in ma: continue
    va,sa=ma[nm]
    A=rd(ra,va,sa); B=rd(rb,vb,sb)
    for o in sorted(B):
        if o not in A: continue
        t,n,ow,sn = B[o]; ta,na,owa,sna = A[o]
        if small.match(n): pend+=1; continue
        if ow is None or owa is None: continue
        b=(re.sub(r'\.\d+$','',ow[0]),ow[1]); a=(re.sub(r'\.\d+$','',owa[0]),owa[1])
        if b==a: ok+=1
        else:
            err+=1
            if len(lines)<20: lines.append('  %-42s +%#06x  nue %s+%d [%s]  obj %s+%d [%s]'%(nm[:42],o,b[0][:26],b[1],sn,a[0][:26],a[1],sna))
print(u,': coinciden %d, DIRECCION DISTINTA %d, pendientes %d'%(ok,err,pend))
for l in lines: print(l)
