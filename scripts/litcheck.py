#!/usr/bin/env python3
"""litcheck.py -- el multiconjunto de VALORES del pool, funcion a funcion.

Otra cara del «100% que miente»: con `function_reloc_diffs=none`, **objdiff no ve
QUE constante carga un `lfs`**. Un literal equivocado es invisible al porcentaje,
y aun asi rompe el DOL. Este barrido enfrenta los valores del pool del objetivo
(los `lbl_` del `.s` resueltos contra el ELF) con los `$LC` de nuestro `.o`.

**Cuatro aciertos de cuatro en su primera pasada:**

    ComputeSkill        `if (difficulty > 1.0f)` era `> 0.0f` -- UN CARACTER,
                        99,44% -> 100%, 540 B. Y de paso desaparecio una
                        permutacion f29/f31: con 0.0f el pool tiene UNA entrada
                        en vez de dos y sobra un allocno flotante.
    AIVehicle::Update   el objetivo materializa -PI/2 DOS veces y nosotros una
                        (CSE la funde porque el primer sitio domina al segundo).
                        95,05% -> 99,33% con tamano exacto.
    GetAdjustedCopCounts  el campo era `unsigned`, asi que el producto salia con
                        la conversion SIN signo (sesgo 0x4330000000000000, sin
                        `xoris`) y el objetivo usa la CON signo: un FPR salvado
                        de mas y 8 B de marco. 94,36% -> 98,45%.
    RoadblockFlow ctor  `mLoDist2RB(0.0f)` era **32767.0f**. No mueve
                        `matched_code` -- el ctor ya media 100% -- **pero si el DOL**.

Cuesta segundos y encuentra lo que ningun diff ensena. Pasalo por toda unidad
antes de pelear registros.
"""
import json,struct,sys,os,re,collections
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
SC = os.environ.get(
    "SCRATCH",
    "C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/"
    "3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad")
unit=sys.argv[1]
E='>'
# --- ELF del objetivo ---
d=open(ROOT+'/orig/GOWE69/NFSMWRELEASE.ELF','rb').read()
shoff,=struct.unpack(E+'I',d[0x20:0x24]); shentsize,shnum,_=struct.unpack(E+'HHH',d[0x2e:0x34])
secs=[]
for i in range(shnum):
    o=shoff+i*shentsize
    nm,typ,fl,addr,off,size=struct.unpack(E+'IIIIII',d[o:o+24]); secs.append((addr,off,size))
def rdva(va):
    for addr,off,size in secs:
        if addr and addr<=va<addr+size: return struct.unpack(E+'I',d[off+(va-addr):off+(va-addr)+4])[0]
    return None
# --- nuestro .o: valores de cada $LC ---
p=ROOT+'/build/GOWE69/src/Speed/Indep/SourceLists/%s.o'%unit
o=open(p,'rb').read()
shoff,=struct.unpack(E+'I',o[0x20:0x24]); shentsize,shnum,shstrndx=struct.unpack(E+'HHH',o[0x2e:0x34])
S=[]
for i in range(shnum):
    q=shoff+i*shentsize
    nm,typ,fl,addr,off,size,link,info,align,entsz=struct.unpack(E+'IIIIIIIIII',o[q:q+40])
    S.append(dict(nm=nm,typ=typ,off=off,size=size,link=link))
symtab=[s for s in S if s['typ']==2][0]; strtab=S[symtab['link']]
byname={}
for i in range(symtab['size']//16):
    q=symtab['off']+i*16
    no,val,sz,info,other,shndx=struct.unpack(E+'IIIBBH',o[q:q+16])
    e=o.index(b'\0',strtab['off']+no); byname[o[strtab['off']+no:e].decode()]=(val,shndx)
def ourval(name):
    if name not in byname: return None
    v,sh=byname[name]
    if sh>=len(S): return None
    b=o[S[sh]['off']+v:S[sh]['off']+v+4]
    return struct.unpack(E+'I',b)[0] if len(b)==4 else None
# --- objetivo: por funcion, del .s del troceador ---
# OJO: el troceador referencia las constantes del objetivo en DOS formatos:
#   lbl_<hex>   -- entradas de pool (.sdata2): el valor se lee en esa direccion
#   $LC<dec>    -- literales de CADENA en .rodata: el decimal ES la direccion
#                  de la cadena (>=0x80000000). Ignorarlos deja ciego al barrido
#                  exactamente donde mas duele: los tags de gFastMem ("STL")
#                  y cualquier printf. Medido: 129 falsos "STL\\0 tgt=0" en
#                  23 unidades venian de aqui y del fold de plantillas debil.
asmf=ROOT+'/build/GOWE69/asm/Speed/Indep/SourceLists/%s.s'%unit
tgt=collections.defaultdict(collections.Counter)
fns_tgt=set()
cur=None
for line in open(asmf,encoding='utf-8',errors='replace'):
    if line.startswith('.fn '):
        cur=line[4:].split(',')[0].strip(); fns_tgt.add(cur)
    elif line.startswith('.endfn'): cur=None
    elif cur:
        for m in re.finditer(r'lbl_([0-9A-F]{8})',line):
            tgt[cur][int(m.group(1),16)]+=1
        for m in re.finditer(r'\$LC(\d{9,})',line):
            va=int(m.group(1))
            if va>=0x80000000: tgt[cur][va]+=1
# --- nuestro: por funcion, del JSON de objdiff (auto-generado si falta) ---
import os, subprocess
dj='%s/d_%s.json'%(SC,unit)
if not os.path.exists(dj):
    os.makedirs(SC, exist_ok=True)
    import ntpath; q=subprocess.run([os.path.join(ROOT,'build','tools','objdiff-cli.exe'),'diff',
        '-c','functionRelocDiffs=none','-u','main/Speed/Indep/SourceLists/'+unit,
        '-o','-','--format','json'],capture_output=True,text=True,cwd=ROOT)
    open(dj,'w').write(q.stdout)
D=json.load(open(dj))
ours=collections.defaultdict(collections.Counter)
for s in D['right']['symbols']:
    n=s.get('name')
    if not n: continue
    for ins in s.get('instructions') or []:
        f=(ins.get('instruction') or {}).get('formatted','')
        for m in re.finditer(r'\$LC\d+',f):
            ours[n][m.group(0)]+=1
def f32(u):
    return struct.unpack(E+'f',struct.pack(E+'I',u))[0]
tot=0
extra=[]
for fn in sorted(set(tgt)|set(ours)):
    A=collections.Counter(); B=collections.Counter()
    for va,c in tgt[fn].items():
        u=rdva(va)
        if u is not None: A[u]+=c//2 if c%2==0 else c
    for lc,c in ours[fn].items():
        u=ourval(lc)
        if u is not None: B[u]+=c//2 if c%2==0 else c
    if not A and not B: continue
    if fn not in fns_tgt:
        # la funcion NO esta en el .s del objetivo: casi siempre una
        # instanciacion plantilla debil nuestra que el linker original plego
        # en otra unidad (p.ej. los _M_erase/_M_insert/reserve en zAttribSys).
        # Comparar literales contra la nada solo genera falsos positivos.
        if B: extra.append(fn)
        continue
    dif=[(v,A.get(v,0),B.get(v,0)) for v in set(A)|set(B) if A.get(v,0)!=B.get(v,0)]
    if dif:
        tot+=1
        print('### %s'%fn[:90])
        for v,a,b in sorted(dif):
            print('    0x%08X %-14g  tgt=%d  ours=%d'%(v,f32(v),a,b))
print('# %d funciones con el multiconjunto de constantes distinto'%tot)
if extra:
    print('# %d funciones SOLO NUESTRAS (ausentes del .s del objetivo; sin comparacion de literales):'%(len(extra)))
    for fn in extra: print('#   %s'%fn[:100])
