#!/usr/bin/env python3
"""x360ref.py — referencia cruzada GC↔X360 sobre el basefile descifrado.

El build X360 (EUROPEGERMILESTONE, Oct-21-2005) comparte base C++ con la
versión GC. Cuando una función GC resiste, su gemela X360 (otro compilador,
mismas decisiones de fuente) revela la estructura: constantes exactas,
condiciones, orden de sentencias.

Uso:
  python scripts/x360ref.py str "literal"        # VA(s) del string en .text X360
  python scripts/x360ref.py fn "literal"         # función X360 que contiene el string (escanea prólogos hacia atrás) + asm
  python scripts/x360ref.py fnat <va> [n]        # asm de la función que empieza en VA
  python scripts/x360ref.py around <va> [n]      # contexto ±n

Requiere tools/scratch/NFS.exe y x360_text.bin (docs/X360_EXTRACTION.md).
"""
import sys, os, struct, re, subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SC = os.path.join(ROOT, 'tools/scratch')
TEXT_BIN = os.path.join(SC, 'x360_text.bin')
BASEFILE = os.path.join(SC, 'NFS.exe')
TEXT_VA = 0x4F0000
IMG = 0x400000
TEXT_RAW = 0xE7200

_text = None
_secs = None

def text():
    global _text
    if _text is None:
        _text = open(TEXT_BIN, 'rb').read()
    return _text

def secs():
    global _secs
    if _secs is None:
        d = open(BASEFILE, 'rb').read()
        e, = struct.unpack('<I', d[0x3c:0x40])
        nsec, = struct.unpack('<H', d[e+6:e+8])
        optsz, = struct.unpack('<H', d[e+20:e+22])
        so = e + 24 + optsz
        _secs = []
        for i in range(nsec):
            s = d[so+i*40:so+(i+1)*40]
            vsz, va, rsz, roff = struct.unpack('<IIII', s[8:24])
            _secs.append((roff, rsz, va + IMG))
    return _secs

def off2va(off):
    for roff, rsz, va in secs():
        if roff <= off < roff + rsz:
            return va + (off - roff)
    return None

def find_str(pat):
    d = open(BASEFILE, 'rb').read()
    needle = pat.encode('latin1', errors='ignore')
    hits, start = [], 0
    while len(hits) < 20:
        i = d.find(needle, start)
        if i < 0: break
        va = off2va(i)
        if va: hits.append(va)
        start = i + 1
    return hits

def refs_to(va):
    """lis/addi en .text que cargan va → direcciones de las funciones que lo usan."""
    t = text()
    hi = ((va >> 16) + (1 if (va & 0xFFFF) >= 0x8000 else 0)) & 0xFFFF
    lo = va & 0xFFFF
    w = struct.unpack(f'>{len(t)//4}I', t[:len(t)//4*4])
    out = []
    for k, x in enumerate(w):
        if (x >> 26) == 15 and (x & 0xFFFF) == hi:
            rt = (x >> 21) & 31
            for j in (k+1, k+2, k+3):
                if j < len(w):
                    y = w[j]
                    if (y >> 26) == 14 and ((y >> 21) & 31) == rt and (y & 0xFFFF) == lo:
                        out.append(TEXT_VA + k*4)
                        break
        if len(out) >= 8: break
    return out

def fn_start(va):
    """prólogo de función más cercano hacia atrás (stwu r1,-X(r1) o stmw)."""
    t = text()
    off = va - TEXT_VA
    off &= ~3
    for k in range(off, max(0, off - 0x4000), -4):
        w, = struct.unpack('>I', t[k:k+4])
        # stwu r1, -X(r1): 0x9421 FFxx (negativo) o mflr r0 (7C0802A6) seguido de stwu
        if (w >> 16) == 0x9421 and (w & 0x8000):
            return TEXT_VA + k
        if (w >> 26) == 47:  # stmw
            return TEXT_VA + k
    return None

OBJDUMP = None
def objdump():
    global OBJDUMP
    if OBJDUMP is None:
        for c in ('build/ppc_binutils/powerpc-eabi-objdump.exe',):
            p = os.path.join(ROOT, c)
            if os.path.exists(p):
                OBJDUMP = p; break
        else:
            OBJDUMP = 'powerpc-eabi-objdump'
    return OBJDUMP

def dump(va, n=80):
    t = text()
    off = va - TEXT_VA
    if not (0 <= off < len(t)):
        print(f'VA {va:#x} fuera de .text')
        return
    blob = t[off:off+n*4+64]
    p = os.path.join(SC, '_chunk.bin')
    open(p, 'wb').write(blob)
    r = subprocess.run([objdump(), '-b', 'binary', '-m', 'powerpc:common64', '-D', p],
                       capture_output=True, text=True)
    cnt = 0
    for l in r.stdout.split('\n'):
        m = re.match(r'\s+([0-9a-f]+):\s+(?:[0-9a-f]{2}\s+)+\s*(\S+.*)', l)
        if m:
            print(f'{va + cnt*4:#010x}  {m.group(2)[:52]}')
            cnt += 1
            if cnt >= n: break

def cmd_str(pat):
    hits = find_str(pat)
    if not hits:
        print(f'"{pat}" no encontrado en el basefile X360')
        return
    for h in hits[:8]:
        print(f'string @{h:#x}')
        for r in refs_to(h)[:4]:
            fs = fn_start(r)
            print(f'  ← referenciado @ {r:#x}  (función: {fs:#x})' if fs else f'  ← referenciado @ {r:#x}')

def cmd_fn(pat, n=80):
    hits = find_str(pat)
    if not hits:
        print(f'"{pat}" no encontrado'); return
    # función cuya REFERENCIA al string exista; si no, la que lo contiene
    for h in hits[:4]:
        for r in refs_to(h)[:2]:
            fs = fn_start(r)
            if fs:
                print(f'=== función X360 @ {fs:#x} (ref string @{h:#x}) ===')
                dump(fs, n)
                return
    fs = fn_start(hits[0])
    if not fs:
        print('sin prólogo'); return
    print(f'=== función X360 @ {fs:#x} (string @{hits[0]:#x}) ===')
    dump(fs, n)

def cmd_fnat(va, n=80):
    dump(int(va, 16), n)

def cmd_around(va, n=24):
    v = int(va, 16)
    dump(v - n*4, n*2)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(__doc__); sys.exit(1)
    c = sys.argv[1]
    if c == 'str': cmd_str(sys.argv[2])
    elif c == 'fn': cmd_fn(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 80)
    elif c == 'fnat': cmd_fnat(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 80)
    elif c == 'around': cmd_around(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 24)
