# -*- coding: utf-8 -*-
# Se ejecuta desde la RAIZ del repo:  python docs/analisis/<este fichero>
import sys, os, struct, collections, bisect, random
exec(open('docs/analisis/r63-veredicto-elfo.py', encoding='utf8').read())
# -*- coding: utf-8 -*-
"""orden.py -- prueba: el grafo de llamadas predice el orden del objetivo?"""

unit = sys.argv[1] if len(sys.argv)>1 else 'Speed/Indep/SourceLists/zPhysics'
tgt = Obj('build/GOWE69/obj/%s.o'%unit)
our = Obj('build/GOWE69/src/%s.o'%unit)

def order(o):
    fs=o.funcs()
    seen={}; seq=[]
    for f in fs:
        if f['name'] in seen: continue
        seen[f['name']]=len(seq); seq.append(f['name'])
    return seq, {f['name']:f for f in fs}

tseq,tmap = order(tgt)
oseq,omap = order(our)
print('objetivo %d funciones | nuestro %d' % (len(tseq), len(oseq)))

# --- grafo de llamadas del OBJETIVO: reloc R_PPC_REL24 (=10) en .text
def calls(o):
    fs=o.funcs()
    starts=[(f['value'], f['value']+f['size'], f['name']) for f in fs]
    starts.sort()
    symtab=o.syms
    edges=[]   # (offset_del_sitio, caller, callee)
    import bisect
    lo=[s[0] for s in starts]
    for off,symi,typ,add in o.relocs('.text'):
        if typ!=10: continue
        callee=symtab[symi]['name']
        i=bisect.bisect_right(lo,off)-1
        caller = starts[i][2] if i>=0 and off < starts[i][1] else None
        edges.append((off,caller,callee))
    return edges

te=calls(tgt); oe=calls(our)
print('sitios de llamada (REL24): objetivo %d | nuestro %d' % (len(te),len(oe)))

tset=set(tseq)
edg=set((c,e) for _,c,e in te if c in tset and e in tset and c!=e)
print('aristas internas distintas en el objetivo: %d' % len(edg))

# --- TEST A: adyacencia. pares consecutivos con arista de llamada
def adjacency(seq, edges):
    n=0
    for a,b in zip(seq,seq[1:]):
        if (a,b) in edges or (b,a) in edges: n+=1
    return n, len(seq)-1

na,da = adjacency(tseq, edg)
print('TEST A  objetivo : %d de %d pares consecutivos unidos por llamada (%.1f%%)'%(na,da,100.0*na/da))
common=[f for f in oseq if f in tset]
nb,db = adjacency(common, edg)
print('TEST A  nuestro  : %d de %d (%.1f%%)'%(nb,db,100.0*nb/db))
random.seed(7); acc=[]
for _ in range(200):
    s=tseq[:]; random.shuffle(s); acc.append(adjacency(s,edg)[0])
print('TEST A  azar     : %.1f de %d (%.2f%%)  [200 barajas]'%(sum(acc)/len(acc),da,100.0*(sum(acc)/len(acc))/da))

# --- TEST B: tau de Kendall contra el orden del objetivo
def tau(pred, ref):
    """pred, ref: listas del MISMO conjunto. tau-b sencillo O(n^2)."""
    rp={n:i for i,n in enumerate(pred)}; rr={n:i for i,n in enumerate(ref)}
    ns=list(ref); c=d=0
    for i in range(len(ns)):
        for j in range(i+1,len(ns)):
            a=ns[i]; b=ns[j]
            s1=rr[a]-rr[b]; s2=rp[a]-rp[b]
            if s1*s2>0: c+=1
            elif s1*s2<0: d+=1
    return (c-d)/float(c+d) if c+d else 0.0

comunes=[f for f in tseq if f in set(oseq)]
print('comunes: %d'%len(comunes))
ref=comunes
parse=[f for f in oseq if f in set(comunes)]

# predictor "primer uso": ordenar por el offset del primer sitio de llamada en el OBJETIVO
firstuse={}
for off,c,e in te:
    if e in set(comunes):
        if e not in firstuse or off<firstuse[e]: firstuse[e]=off
sinllam=[f for f in comunes if f not in firstuse]
print('sin ningun sitio de llamada REL24 en el objetivo: %d de %d (%.1f%%)'%(len(sinllam),len(comunes),100.0*len(sinllam)/len(comunes)))
# a las que no tienen llamador, se les deja su posicion del objetivo (regalo al predictor)
# HONESTO: a las que no tienen llamador se les deja su puesto de PARSE (lo unico
# que sabriamos sin el grafo), NO su puesto del objetivo (eso seria filtrar la respuesta).
rpar={n:i for i,n in enumerate([f for f in oseq if f in set(comunes)])}
key={}
for f in comunes:
    key[f]= firstuse[f] if f in firstuse else 10**9+rpar[f]
fu=sorted(comunes,key=lambda f:(key[f],rpar[f]))
fu_solo=[f for f in comunes if f in firstuse]

print('TAU  parse (nuestro orden)      vs objetivo: %+.3f'%tau(parse,ref))
print('TAU  primer uso (grafo Ghidra)  vs objetivo: %+.3f'%tau(fu,ref))
random.seed(11); ts=[]
for _ in range(20):
    s=comunes[:]; random.shuffle(s); ts.append(tau(s,ref))
print('TAU  azar                       vs objetivo: %+.3f (media de 20)'%(sum(ts)/len(ts)))

# --- TEST C: solo sobre las DESCOLOCADAS (rango de parse != rango objetivo en >5 puestos)
rp={n:i for i,n in enumerate(parse)}; rr={n:i for i,n in enumerate(ref)}
desc=[f for f in ref if abs(rp[f]-rr[f])>5]
print('descolocadas (>5 puestos): %d'%len(desc))
if len(desc)>10:
    p2=[f for f in parse if f in set(desc)]
    f2=[f for f in fu if f in set(desc)]
    r2=[f for f in ref if f in set(desc)]
    sinll=len([f for f in desc if f not in firstuse])
    print('   de esas, sin llamador REL24: %d (%.1f%%)'%(sinll,100.0*sinll/len(desc)))
    print('   TAU parse      %+.3f'%tau(p2,r2))
    print('   TAU primer uso %+.3f'%tau(f2,r2))
    con=[f for f in desc if f in firstuse]
    if len(con)>10:
        print('   -- solo las %d descolocadas CON llamador:'%len(con))
        print('      TAU parse      %+.3f'%tau([f for f in parse if f in set(con)],[f for f in ref if f in set(con)]))
        print('      TAU primer uso %+.3f'%tau([f for f in fu if f in set(con)],[f for f in ref if f in set(con)]))
