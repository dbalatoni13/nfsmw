"""Compara el ARBOL de un switch: la secuencia de constantes comparadas.
Mismo conjunto, distinto orden -> pivote movido (hay `case` ocultos).
El objetivo compara valores que nosotros no -> nos faltan `case`."""
import json,sys,re
SCR=r'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad'
def tree(sym):
    ins=[((e.get('instruction') or {}).get('formatted','')).strip() for e in sym.get('instructions',[])]
    ins=[t for t in ins if t]
    cur={}; seq=[]
    for i,t in enumerate(ins):
        m=re.match(r'lis (r\d+), (0x[0-9a-f]+)$',t)
        if m: cur[m.group(1)]=int(m.group(2),16)<<16; continue
        m=re.match(r'ori (r\d+), (r\d+), (0x[0-9a-f]+)$',t)
        if m and m.group(1)==m.group(2) and m.group(1) in cur:
            cur[m.group(1)]|=int(m.group(3),16); continue
        m=re.match(r'(cmpw|cmplw) (r\d+), (r\d+)$',t)
        if m and m.group(3) in cur: seq.append(cur[m.group(3)]); continue
        m=re.match(r'(cmpwi|cmplwi) (r\d+), (-?0x[0-9a-f]+|-?\d+)$',t)
        if m:
            nx=ins[i+1].split()[0] if i+1<len(ins) else ''
            if nx in ('beq','bne','bgt','blt','bge','ble'): seq.append(int(m.group(3),0))
        if t.startswith('bl '): cur={}
    return seq
for unit in sys.argv[1:]:
    d=json.load(open(f'{SCR}/fe2_d_{unit}.json'))
    R={s['name']:s for s in d['right']['symbols'] if s.get('kind')=='SYMBOL_FUNCTION'}
    print('===',unit)
    tot=0
    for s in d['left']['symbols']:
        if s.get('kind')!='SYMBOL_FUNCTION': continue
        if (s.get('match_percent') or 0)>=100.0: continue
        o=R.get(s['name'])
        if not o: continue
        a=tree(s); b=tree(o)
        if a==b or len(a)<3: continue
        sa,sb=set(a),set(b)
        if sa==sb: kind='ORDEN (pivote movido)'
        elif sa-sb: kind='FALTAN valores: %s'%[hex(x) for x in sorted(sa-sb)][:5]
        elif sb-sa: kind='SOBRAN nuestros: %s'%[hex(x) for x in sorted(sb-sa)][:5]
        else: kind='orden'
        tot+=int(s['size'])
        print('  %-64s %6.2f%% %5dB  %s'%(s['name'][:64],s.get('match_percent') or 0,int(s['size']),kind))
        if kind.startswith('ORDEN'):
            print('       tgt: %s'%[hex(x) for x in a[:9]])
            print('       ns : %s'%[hex(x) for x in b[:9]])
    print('  --- %d B'%tot)
