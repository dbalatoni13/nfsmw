#!/usr/bin/env python3
"""Compara los RANGOS de bloque/inline del DWARF NORMALIZADOS al inicio de la
funcion. dwbody imprime direcciones absolutas y no se pueden comparar; restando
el low_pc de la funcion (la linea `// Range:` que va JUSTO ENCIMA de la firma)
los dos lados quedan en la misma escala y un bloque que abre 3 instrucciones
antes salta a la vista."""
import glob,os,re,sys
ROOT='C:/Users/jferr/Desktop/nfsdecompiled'
ORIG=os.path.join(ROOT,'symbols','mw_dwarfdump.nothpp')

def bodies(path):
    txt=open(path,encoding='utf-8',errors='replace').read().split('\n')
    out={}; i=0
    while i<len(txt):
        line=txt[i]
        if (line and not line.startswith((' ','\t','}','/','#'))
                and line.rstrip().endswith('{') and '(' in line):
            lo=None
            for k in range(max(0,i-4),i):
                m=re.match(r'//\s*Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)',txt[k].strip())
                if m: lo=int(m.group(1),16)
            j,depth,buf=i,0,[]
            while j<len(txt):
                buf.append(txt[j]); depth+=txt[j].count('{')-txt[j].count('}')
                if depth<=0 and j>i: break
                j+=1
            out.setdefault(line.strip(),[]).append((lo,'\n'.join(buf)))
            i=j+1; continue
        i+=1
    return out

def norm(lo,body):
    res=[]
    for ln in body.split('\n'):
        m=re.match(r'\s*//\s*Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)',ln)
        ind=len(ln)-len(ln.lstrip())
        if m and lo is not None:
            a=int(m.group(1),16)-lo; b=int(m.group(2),16)-lo
            res.append('%s// Range: +0x%x -> +0x%x  (ins %d..%d)'%(' '*ind,a,b,a//4,b//4))
        else:
            res.append(ln)
    return res

def main():
    unit,name=sys.argv[1],sys.argv[2]
    o=bodies(ORIG)
    cands=sorted(glob.glob(os.path.join(ROOT,'build','regmap','our_%s_*.nothpp'%unit)))
    n=bodies(cands[-1])
    ho=[k for k in o if name in k]; hn=[k for k in n if name in k]
    if not ho or not hn:
        print('no encontrada:',ho,hn); return 1
    lo,bo=o[ho[0]][0]; ln_,bn=n[hn[0]][0]
    A=norm(lo,bo); B=norm(ln_,bn)
    import difflib
    d=list(difflib.unified_diff(A,B,'ORIGINAL','NUESTRO',n=2,lineterm=''))
    if not d: print('rangos normalizados IDENTICOS')
    else: print('\n'.join(d))
    return 0
sys.exit(main())
