"""Comprueba que cada entrada `objeto:simbolo` de keep.lst existe de verdad.
Una entrada rancia no da error: -strip-unused-data se lleva sus bytes en
silencio y el DOL sale corto."""
import io, os, struct, sys, glob

def objsyms(p):
    f = open(p, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff+i*shentsize:shoff+i*shentsize+40]) for i in range(shnum)]
    def nm(x, b):
        e = f.index(b'\0', b + x); return f[b+x:e].decode()
    st = [i for i, s in enumerate(S) if s[1] == 2]
    if not st: return set()
    st = st[0]; strt = S[S[st][6]][4]
    out = set()
    for o in range(S[st][4], S[st][4] + S[st][5], 16):
        nameo = struct.unpack('>I', f[o:o+4])[0]
        n = nm(nameo, strt)
        if n: out.add(n)
    return out

# Los objetos EXTRAIDOS primero: son los que dan fe de los nombres de dtk
# (lbl_, gap_, pad_). Pero una entrada puede apuntar a un simbolo que solo
# existe en NUESTRO objeto -- los $LC que emite cc1plus para una cadena muerta
# que queremos forzar. Sin este respaldo salian como RANCIA siendo correctas.
idx = {}
for p in glob.glob('build/GOWE69/obj/**/*.o', recursive=True):
    idx.setdefault(os.path.basename(p), p)
idx_src = {}
for p in glob.glob('build/GOWE69/src/**/*.o', recursive=True):
    idx_src.setdefault(os.path.basename(p), p)
cache = {}
falta_obj, rancias, ok = [], [], 0
for l in io.open('config/GOWE69/keep.lst', encoding='utf-8'):
    t = l.strip()
    if not t or t.startswith('#') or ':' not in t: continue
    obj, sym = t.split(':', 1)
    p = idx.get(obj)
    if p is None:
        falta_obj.append(t); continue
    if p not in cache: cache[p] = objsyms(p)
    if sym in cache[p]:
        ok += 1
        continue
    q = idx_src.get(obj)
    if q:
        if q not in cache: cache[q] = objsyms(q)
        if sym in cache[q]:
            ok += 1
            continue
    rancias.append(t)
print('entradas con objeto     : %d ok, %d RANCIAS, %d sin objeto' % (ok, len(rancias), len(falta_obj)))
for t in rancias: print('  RANCIA   ', t)
for t in falta_obj: print('  SIN OBJ  ', t)
