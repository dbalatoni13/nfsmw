#!/usr/bin/env python3
"""pubnames.py -- atribuye CADA SIMBOLO del original a SU UNIDAD DE COMPILACION,
leyendo `.debug_pubnames` y `.debug` de `orig/GOWE69/NFSMWRELEASE.ELF`.

Es la version por SIMBOLO de lo que `config/GOWE69/splits.txt` intenta adivinar
por RANGO.

EL FORMATO, confirmado en el fuente de GCC que esta en el arbol
(`orig/prodg/NGC_GNU_SRC/NGC/gcc/dwarfout.c`):
  - :7156-7160  al abrir cada unidad, UNA palabra: la direccion de
    `DEBUG_BEGIN_LABEL`, o sea el offset en `.debug` del DIE `TAG_compile_unit`.
  - :6452-6462 (funciones) y :6490-6499 (variables): por cada simbolo publico un
    par (palabra = offset de un DIE en `.debug`, cadena terminada en nul).
  - :7336-7341  terminador de unidad: palabra 0 mas cadena vacia (5 bytes).

DOS TRAMPAS, las dos MEDIDAS (no supuestas):

 1. **La cadena NO esta decorada.** Es `IDENTIFIER_POINTER(DECL_NAME(decl))`
    (:6459, :6497), no `DECL_ASSEMBLER_NAME`: pone `Construct`, no
    `Construct__16AITrafficManagerGQ23Sim5Param`. Emparejar por cadena contra
    `symbols.txt` es exactamente la trampa de `nfsmw-el-cero-que-miente` al
    reves. Aqui NO se empareja por nombre en ningun sitio: todo va por
    DIRECCION.

 2. **El offset del par esta DESFASADO en 5.806 de 27.446 entradas (21 %).**
    `dwarfout_file_scope_decl` escribe la entrada con `next_pubname_number` SIN
    incrementar (:6457, :6495) y es `output_decl` quien define la etiqueta
    `.L_P<n>` e incrementa (:6081, :6319). Pero `output_decl` recursa: emite
    antes el tipo contenedor, y los miembros publicos de esa clase consumen
    numeros. Resultado medido: el pubname "Construct" de zAI apunta al DIE cuyo
    `AT_name` es "mStagger". El offset del par es, por tanto, INUTILIZABLE.

LA VIA BUENA, y la que usa esta herramienta: los DIE de cada unidad son
CONTIGUOS en `.debug`, asi que el OFFSET DEL DIE ya dice de que unidad es. Se
recorre `.debug` entero (3.312.735 DIE, 11 s), se coge cada DIE con direccion
(`AT_low_pc` en funciones, `AT_location` = OP_ADDR en variables; OJO: en DWARF-1
OP_ADDR es 0x03, dwarf.h:200, y `AT_location` es 0x0023, dwarf.h:121) y se
atribuye a la unidad cuyo rango de offsets lo contiene.

Salen 42.145 DIE con direccion; 20.600 la tienen a 0 o a 0xFFFFFFFF (copias
`linkonce` que el enlazador tiro) y quedan 21.545 direcciones. De esas, 450
aparecen en mas de una unidad -- son globales declaradas en cabeceras, que
sacan DIE en toda unidad que las referencia (`UMath::Vector3::kZero` sale en
33). Se desempatan con la lista de pubnames, que SOLO emite la unidad que
define (`! DECL_EXTERNAL`, :6481-6483): 336 de las 450 quedan resueltas.

RESULTADO: 21.431 direcciones con dueno unico. Contra `splits.txt`: 20.609
coinciden, 61 son alias de nombre de fichero, 128 son DISCREPANCIAS reales
(casi todas fronteras de `.bss` mal puestas por unos pocos bytes) y 633 caen
fuera de todo rango (469 en `.over`, el resto en los comodines `auto_*`).
Control: 100 simbolos de cinco SourceLists ya promocionadas, 100 aciertos.

Uso:
    python scripts/pubnames.py units             censo de unidades
    python scripts/pubnames.py syms  <unidad>    simbolos publicos de una unidad
    python scripts/pubnames.py where <sym|0xADDR>...   dueno de cada simbolo
    python scripts/pubnames.py check             control masivo contra splits.txt
    python scripts/pubnames.py control <unidad> [n]   control de n simbolos
    python scripts/pubnames.py auto              que dice de los comodines auto_*
    python scripts/pubnames.py dump              TSV: addr, unidad, simbolo, publico
"""
import bisect
import collections
import glob
import os
import pickle
import re
import struct
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')

FORM_ADDR, FORM_REF, FORM_BLOCK2, FORM_BLOCK4 = 1, 2, 3, 4
FORM_DATA2, FORM_DATA4, FORM_DATA8, FORM_STRING = 5, 6, 7, 8
AT_NAME, AT_LOW_PC, AT_LOCATION = 0x0038, 0x0111, 0x0023
OP_REG, OP_BASEREG, OP_ADDR = 0x01, 0x02, 0x03
# los TAG que pueden llevar direccion (dwarf.h del arbol, :56-80)
TAG_ADDR = frozenset((0x0004, 0x0006, 0x0007, 0x000C, 0x0014))
TAG_NAME = {0x0004: 'local_variable?', 0x0006: 'global_subroutine',
            0x0007: 'global_variable', 0x000C: 'local_variable',
            0x0011: 'compile_unit', 0x0014: 'subroutine'}


# --------------------------------------------------------------------- ELF

class Elf(object):
    def __init__(self, path=ELF):
        self.path = path
        self.d = open(path, 'rb').read()
        d = self.d
        assert d[:4] == b'\x7fELF' and d[5] == 2, 'no es ELF32 big-endian'
        e_shoff, = struct.unpack_from('>I', d, 0x20)
        e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('>HHH', d, 0x2E)
        raw = []
        for i in range(e_shnum):
            o = e_shoff + i * e_shentsize
            n, t, fl, ad, of, sz, lk, inf, al, es = struct.unpack_from('>10I', d, o)
            raw.append(dict(name=n, typ=t, addr=ad, off=of, size=sz, link=lk))
        so = raw[e_shstrndx]['off']
        for s in raw:
            e = d.index(b'\0', so + s['name'])
            s['n'] = d[so + s['name']:e].decode('latin1')
        self.sh = raw
        self.by = dict((s['n'], s) for s in raw)

    def sec(self, name):
        s = self.by[name]
        return self.d[s['off']:s['off'] + s['size']]

    def symbols(self):
        """[(addr, name, size, shndx, bind)] de .symtab, sin SECTION ni FILE."""
        d = self.d
        st, sr = self.by['.symtab'], self.by['.strtab']
        out = []
        for o in range(st['off'], st['off'] + st['size'], 16):
            nm, val, sz, info, other, shndx = struct.unpack_from('>IIIBBH', d, o)
            if (info & 0xF) in (3, 4):
                continue
            base = sr['off'] + nm
            e = d.index(b'\0', base)
            name = d[base:e].decode('latin1')
            if name:
                out.append((val, name, sz, shndx, info >> 4))
        return out


# --------------------------------------------------------------------- DIE

def die_at(dbg, off):
    """DIE en `off` del buffer `.debug`. -> (tag, {attr: valor}, longitud)."""
    if off + 6 > len(dbg):
        return None, {}, 0
    ln, = struct.unpack_from('>I', dbg, off)
    if ln < 6 or off + ln > len(dbg):
        return None, {}, ln
    tag, = struct.unpack_from('>H', dbg, off + 4)
    p, end, at = off + 6, off + ln, {}
    while p < end:
        a, = struct.unpack_from('>H', dbg, p)
        p += 2
        f = a & 0xF
        if f in (FORM_ADDR, FORM_REF, FORM_DATA4):
            v, = struct.unpack_from('>I', dbg, p)
            p += 4
        elif f == FORM_DATA2:
            v, = struct.unpack_from('>H', dbg, p)
            p += 2
        elif f == FORM_DATA8:
            v, = struct.unpack_from('>Q', dbg, p)
            p += 8
        elif f == FORM_BLOCK2:
            n, = struct.unpack_from('>H', dbg, p)
            p += 2
            v = dbg[p:p + n]
            p += n
        elif f == FORM_BLOCK4:
            n, = struct.unpack_from('>I', dbg, p)
            p += 4
            v = dbg[p:p + n]
            p += n
        elif f == FORM_STRING:
            e = dbg.index(b'\0', p)
            v = dbg[p:e].decode('latin1')
            p = e + 1
        else:
            break
        at.setdefault(a, v)
    return tag, at, ln


def die_addr(at):
    """Direccion del simbolo que describe el DIE, o None."""
    if AT_LOW_PC in at:
        return at[AT_LOW_PC]
    loc = at.get(AT_LOCATION)
    # DWARF-1: OP_ADDR es 0x03 (dwarf.h del arbol, :200), NO 0x01 (que es OP_REG)
    if loc and len(loc) >= 5 and loc[0] == OP_ADDR:
        return struct.unpack_from('>I', loc, 1)[0]
    return None


# ---------------------------------------------------------------- pubnames

def parse_pubnames(elf):
    """-> ([unidades], buffer .debug), en orden de emision.

    Cada unidad: {off, file, entries:[(die_off, nombre_SIN_decorar)]}.
    OJO: `die_off` esta desfasado en el 21 % de los casos (ver cabecera); solo
    se usa para saber QUE NOMBRES son publicos, nunca para sacar direcciones.
    """
    b = elf.sec('.debug_pubnames')
    dbg = elf.sec('.debug')
    units, p, n = [], 0, len(b)
    while p < n:
        uoff, = struct.unpack_from('>I', b, p)
        p += 4
        ents = []
        while p < n:
            doff, = struct.unpack_from('>I', b, p)
            p += 4
            e = b.index(b'\0', p)
            name = b[p:e].decode('latin1')
            p = e + 1
            if doff == 0 and name == '':
                break
            ents.append((doff, name))
        tag, at, _ = die_at(dbg, uoff)
        units.append(dict(off=uoff, tag=tag, file=at.get(AT_NAME, '?'), entries=ents))
    return units, dbg


def norm(path):
    """'D:/mw/Speed/Indep/SourceLists/zAI.cpp' -> 'Speed/Indep/SourceLists/zAI.cpp'."""
    q = path.replace('\\', '/')
    i = q.lower().find('/mw/')
    return q[i + 4:] if i >= 0 else q


# ------------------------------------------------------- el recorrido de .debug

CACHE = os.path.join(tempfile.gettempdir(), 'nfsmw_pubnames_cache.pkl')


def walk(elf=None, force=False):
    """-> dict con:
        files    [nombre de fuente por unidad, en orden]
        bounds   [offset de inicio de cada unidad] + [fin]
        dies     [(offset_die, tag, nombre, addr)] de TODO DIE con direccion
        pub      {indice_de_unidad: set(nombres publicos)}
    """
    key = None
    if os.path.exists(ELF):
        key = (os.path.getsize(ELF), int(os.path.getmtime(ELF)))
    if not force and os.path.exists(CACHE):
        try:
            c = pickle.load(open(CACHE, 'rb'))
            if c.get('key') == key:
                return c
        except Exception:
            pass
    elf = elf or Elf()
    units, dbg = parse_pubnames(elf)
    unp4 = struct.Struct('>I').unpack_from
    unp2 = struct.Struct('>H').unpack_from
    dies, p, n = [], 0, len(dbg)
    while p < n:
        ln = unp4(dbg, p)[0]
        if ln < 4:
            break
        if ln >= 6:
            tag = unp2(dbg, p + 4)[0]
            if tag in TAG_ADDR:
                t, at, _ = die_at(dbg, p)
                a = die_addr(at)
                if a is not None:
                    dies.append((p, tag, at.get(AT_NAME), a))
        p += ln
    c = dict(key=key,
             files=[norm(u['file']) for u in units],
             bounds=[u['off'] for u in units] + [n],
             dies=dies,
             pub=dict((i, set(nm for _, nm in u['entries']))
                      for i, u in enumerate(units)),
             nent=[len(u['entries']) for u in units])
    try:
        pickle.dump(c, open(CACHE, 'wb'), 2)
    except Exception:
        pass
    return c


def addr2unit(c=None, con_ambiguas=False):
    """{direccion: (indice_unidad, nombre_sin_decorar, tag)}.

    Dos filtros, los dos medidos:

    - Se descartan las direcciones 0 y 0xFFFFFFFF: son las copias `linkonce`
      que el enlazador tiro (20.600 de 42.145 DIE con direccion).

    - Una variable global declarada en una cabecera saca un DIE en CADA unidad
      que la referencia, no solo en la que la define (`UMath::Vector3::kZero`
      sale en 33 unidades). Se desempata con la lista de pubnames: la entrada
      en `.debug_pubnames` solo la emite la unidad que DEFINE el simbolo
      (dwarfout.c:6481-6483 exige `! DECL_EXTERNAL`). Eso resuelve 336 de las
      450 direcciones repetidas; las otras 114 quedan fuera salvo que se pida
      `con_ambiguas`.
    """
    c = c or walk()
    b = c['bounds']
    per = {}
    for off, tag, nm, a in c['dies']:
        if a in (0, 0xFFFFFFFF):
            continue
        i = bisect.bisect_right(b, off) - 1
        per.setdefault(a, []).append((i, nm, tag))
    out = {}
    for a, v in per.items():
        us = set(i for i, nm, tag in v)
        if len(us) > 1:
            defs = [(i, nm, tag) for i, nm, tag in v if nm in c['pub'][i]]
            us = set(i for i, nm, tag in defs)
            if len(us) != 1:
                if not con_ambiguas:
                    continue
                out[a] = (-1, v[0][1], v[0][2])
                continue
            v = defs
        out[a] = v[0]
    return out


def symmap(elf):
    by_addr, by_name = {}, {}
    for val, name, sz, shndx, bind in elf.symbols():
        if name == 'gcc2_compiled.':
            continue
        by_addr.setdefault(val, []).append(name)
        by_name.setdefault(name, []).append(val)
    return by_addr, by_name


# ------------------------------------------------------------------ config

def load_splits():
    """-> {unidad: {sec: (start, end)}} de config/GOWE69/splits.txt."""
    out, cur = {}, None
    with open(os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt')) as fh:
        for line in fh:
            s = line.strip()
            if not s or s.startswith('#'):
                continue
            if s.endswith(':') and not line[0].isspace():
                cur = s[:-1]
                out[cur] = {}
            elif cur and s.startswith('.'):
                f = s.split()
                d = dict(x.split(':', 1) for x in f[1:] if ':' in x)
                if 'start' in d and 'end' in d:
                    out[cur][f[0]] = (int(d['start'], 16), int(d['end'], 16))
    return out


def splits_index():
    sp = load_splits()
    items = []
    for k, r in sp.items():
        for sec, (s, e) in r.items():
            items.append((s, e, k))
    items.sort()
    starts = [x[0] for x in items]

    def owner(a):
        i = bisect.bisect_right(starts, a) - 1
        while i >= 0 and items[i][0] > a - 0x200000:
            s, e, k = items[i]
            if s <= a < e:
                return k
            i -= 1
        return None
    return sp, owner


def load_symbols_txt():
    """-> {nombre: [(sec, addr)]} de config/GOWE69/symbols.txt."""
    out = {}
    with open(os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt')) as fh:
        for line in fh:
            s = line.strip()
            if '=' not in s or s.startswith('//'):
                continue
            name, rest = s.split('=', 1)
            loc = rest.split(';')[0].strip()
            if ':' not in loc:
                continue
            sec, addr = loc.rsplit(':', 1)
            try:
                out.setdefault(name.strip(), []).append((sec, int(addr, 16)))
            except ValueError:
                pass
    return out


# ---------------------------------------------------------------- comandos

def cmd_units():
    c = walk()
    print('unidades en .debug_pubnames : %d' % len(c['files']))
    print('entradas (simbolos publicos): %d' % sum(c['nent']))
    print('DIE con direccion util      : %d' % len(addr2unit(c)))
    for i, f in enumerate(c['files']):
        print('%6d  %-70s off=0x%07X' % (c['nent'][i], f, c['bounds'][i]))


def cmd_syms(pat):
    c = walk()
    elf = Elf()
    by_addr, _ = symmap(elf)
    a2u = addr2unit(c)
    per = collections.defaultdict(list)
    for a, (i, nm, tag) in a2u.items():
        per[i].append((a, nm, tag))
    for i, f in enumerate(c['files']):
        if pat.lower() not in f.lower():
            continue
        print('== %s  (off 0x%X, %d publicos)' % (f, c['bounds'][i], c['nent'][i]))
        for a, nm, tag in sorted(per[i]):
            pubm = 'pub' if nm in c['pub'][i] else '   '
            print('  0x%08X %s %-16s %-24s %s' % (
                a, pubm, TAG_NAME.get(tag, hex(tag)), nm or '-',
                ' | '.join(by_addr.get(a, []))))


def cmd_where(args):
    c = walk()
    elf = Elf()
    by_addr, by_name = symmap(elf)
    a2u = addr2unit(c)
    sp, owner = splits_index()
    for q in args:
        if q.lower().startswith('0x'):
            addrs = [int(q, 16)]
        elif q in by_name:
            addrs = by_name[q]
        else:
            print('%-52s -> no esta en .symtab del original' % q[:52])
            continue
        for a in addrs:
            r = a2u.get(a)
            if not r:
                print('%-52s 0x%08X -> pubnames NO lo cubre   (splits: %s)'
                      % (q[:52], a, owner(a)))
                continue
            i, nm, tag = r
            print('%-52s 0x%08X -> %s   [%s "%s"]   (splits: %s)'
                  % (q[:52], a, c['files'][i], TAG_NAME.get(tag, hex(tag)),
                     nm, owner(a)))


def cmd_check():
    """Control masivo: toda direccion que pubnames atribuye, contra splits.txt."""
    c = walk()
    a2u = addr2unit(c)
    elf = Elf()
    by_addr, _ = symmap(elf)
    sp, owner = splits_index()
    ok = alias = miss = 0
    pares = collections.Counter()
    for a, (i, nm, tag) in a2u.items():
        u = c['files'][i]
        k = owner(a)
        if k is None:
            miss += 1
            continue
        if u.split('/')[-1].lower() == k.split('/')[-1].lower():
            ok += 1
        else:
            alias += 1
            pares[(u.split('/')[-1], k.split('/')[-1])] += 1
    print('direcciones atribuidas por pubnames : %d' % len(a2u))
    print('  mismo fichero que splits.txt      : %d' % ok)
    print('  fichero DISTINTO                  : %d' % alias)
    print('  fuera de todo rango de splits.txt : %d' % miss)
    unidades = set(f.split('/')[-1].lower() for f in c['files'])
    for (u, k), v in pares.most_common():
        # si splits.txt tiene un nombre que NO es unidad de pubnames, es un
        # ALIAS de nombre; si los dos son unidades reales, es DISCREPANCIA.
        marca = 'DISCREPA' if k.lower() in unidades else 'alias   '
        print('    %s %4d  pubnames=%-24s splits=%s' % (marca, v, u, k))


def cmd_control(pat, n=20):
    """Control por unidad: coge `n` simbolos de una unidad de splits.txt (por
    direccion, no por nombre) y comprueba a quien los atribuye pubnames."""
    c = walk()
    a2u = addr2unit(c)
    elf = Elf()
    by_addr, _ = symmap(elf)
    sp, owner = splits_index()
    keys = [k for k in sp if pat.lower() in k.lower()]
    if not keys:
        sys.exit('splits.txt no tiene ninguna unidad que case con %r' % pat)
    k = keys[0]
    st = load_symbols_txt()
    cand = []
    for name, v in st.items():
        for sec, a in v:
            if any(s <= a < e for s, e in sp[k].values()) and a in a2u:
                cand.append((a, name))
    cand.sort()
    step = max(1, len(cand) // n)
    sel = cand[::step][:n]
    print('unidad de control: %s   (%d simbolos con DIE, se prueban %d)'
          % (k, len(cand), len(sel)))
    ok = bad = 0
    for a, name in sel:
        i, nm, tag = a2u[a]
        u = c['files'][i]
        good = u.split('/')[-1].lower() == k.split('/')[-1].lower()
        ok, bad = (ok + 1, bad) if good else (ok, bad + 1)
        print('  %s 0x%08X %-56s -> %s' % ('OK  ' if good else 'FALLA', a,
                                           name[:56], u))
    print('aciertos: %d de %d   fallos: %d' % (ok, len(sel), bad))


def cmd_auto():
    """Que dicen pubnames de los comodines build/GOWE69/obj/auto_*.o."""
    c = walk()
    a2u = addr2unit(c)
    elf = Elf()
    by_addr, _ = symmap(elf)
    fs = sorted(glob.glob(os.path.join(ROOT, 'build', 'GOWE69', 'obj', 'auto_*.o')))
    addrs = sorted(a2u)
    tot = collections.Counter()
    cubiertos = 0
    for f in fs:
        m = re.search(r'auto_(\d+)_([0-9A-Fa-f]{8})_(\w+)\.o$', os.path.basename(f))
        if not m:
            continue
        base = int(m.group(2), 16)
        o = Elf(f)
        sec = '.' + m.group(3)
        if sec not in o.by:
            continue
        size = o.by[sec]['size']
        lo = bisect.bisect_left(addrs, base)
        hi = bisect.bisect_left(addrs, base + size)
        hits = collections.Counter(c['files'][a2u[a][0]] for a in addrs[lo:hi])
        if not hits:
            continue
        cubiertos += 1
        tot.update(hits)
        print('%-40s 0x%08X+0x%X  %s' % (
            os.path.basename(f), base, size,
            ', '.join('%s x%d' % (k, v) for k, v in hits.most_common())))
    print()
    print('comodines auto_*: %d, con atribucion de pubnames: %d, simbolos: %d'
          % (len(fs), cubiertos, sum(tot.values())))


def cmd_dump():
    c = walk()
    a2u = addr2unit(c)
    elf = Elf()
    by_addr, _ = symmap(elf)
    print('addr\tunidad\tpubname\ttag\tpublico\telf_symbols')
    for a in sorted(a2u):
        i, nm, tag = a2u[a]
        print('0x%08X\t%s\t%s\t%s\t%s\t%s' % (
            a, c['files'][i], nm or '', TAG_NAME.get(tag, hex(tag)),
            'si' if nm in c['pub'][i] else 'no', ','.join(by_addr.get(a, []))))


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd = sys.argv[1]
    if cmd == 'units':
        cmd_units()
    elif cmd == 'syms':
        cmd_syms(sys.argv[2])
    elif cmd == 'where':
        cmd_where(sys.argv[2:])
    elif cmd == 'check':
        cmd_check()
    elif cmd == 'control':
        cmd_control(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 20)
    elif cmd == 'auto':
        cmd_auto()
    elif cmd == 'dump':
        cmd_dump()
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main()
