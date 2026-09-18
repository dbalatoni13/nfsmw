#!/usr/bin/env python3
"""dwarf1.py -- el oraculo del .debug DWARF-1 del ELF de GameCube (r70b).

`orig/GOWE69/NFSMWRELEASE.ELF` lleva 92 MB de `.debug` DWARF-1 emitido por el
compilador EXACTO del build (verificado en el DIE de cada unidad):
    XGNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube
Es la fuente primaria para nombres/tipos/ubicaciones de locales, parametros y
arbol de inlines del ORIGINAL -- mas rico que dwbody.py (PS2 mdebug, alpha).

EL FORMATO (verificado contra el productor, NO supuesto)
--------------------------------------------------------
Las constantes y la maquinaria de emision estan en el arbol:
`orig/prodg/NGC_GNU_SRC/NGC/gcc/dwarfout.c` + `dwarf.h` (el GCC de SN que
compilo el ELF). Lectura corroborada byte a byte en los anclajes de r70b
('vert_comp' .debug+0x9E82CC, 'SignedMod' +0x5D7FFA, CU zAI.cpp +0x0):

  DIE   = [u32 length BIG-ENDIAN, INCLUYE los 4 bytes del length]
          [u16 tag]
          [attrs...]        -- hasta agotar `length`
  attr  = [u16 nombre] -- el nible BAJO codifica la CLASE del valor:
          1=ADDR(u4) 2=REF(u4, offset dentro de .debug) 3=BLOCK2(u2+n)
          4=BLOCK4(u4+n) 5=DATA2 6=DATA4 7=DATA8 8=STRING(nul)
  marker= DIE con length==4 y sin tag: `end_sibling_chain` (dwarfout.c:5239),
          termina la cadena de hijos de un padre.

TAGs (dwarf.h): 0x0006 global_subroutine, 0x0014 subroutine, 0x0005
formal_parameter, 0x000c local_variable, 0x000b lexical_block, 0x0011
compile_unit, 0x001d inlined_subroutine, 0x0013 structure_type, 0x0002
class_type, 0x0016 typedef, 0x0004 enumeration_type, 0x000f pointer_type,
0x0001 array_type, 0x000a label, 0x000d member, 0x0007 global_variable,
0x0017 union_type, 0x001c inheritance, 0x0021 subrange_type.
(Ojo: la hipotesis a mano de r70b decia 0x000e=TAG_variable y 0x000d=
formal_parameter -- es al reves: local_variable=0x000c, member=0x000d,
formal_parameter=0x0005, y 0x000e esta RESERVADO.)

ATs (dwarf.h, nombre|forma): sibling 0x0012, location 0x0023, name 0x0038,
fund_type 0x0055, mod_fund_type 0x0063, user_def_type 0x0072, mod_u_d_type
0x0083, byte_size 0x00b6, low_pc 0x0111, high_pc 0x0121, language 0x0136,
comp_dir 0x01b8, const_value 0x01c?, inline 0x0208 (string; vacio =
declarado inline), abstract_origin 0x02b2, producer 0x0258, stmt_list
0x0106, upper_bound 0x02f5/0x02f6; ext GNU: sf_names 0x8006, src_info
0x8016, src_coords 0x8036 (fileno:u16 << 16 | lineno:u16).

Tipos: AT_fund_type = u16 codigo FT_* (0x000e float, 0x000f double
precision, 0xa108 real64 GNU, 0x0007/8 integer, 0x000d pointer(void*), ...).
AT_mod_fund_type = BLOCK2 [bytes MOD_* (1=*, 2=&, 3=const, 4=volatile)] +
[u16 FT]. AT_user_def_type = REF al DIE del tipo; AT_mod_u_d_type = BLOCK2
[MODs] + [u32 REF].

Ubicaciones (BLOCK2 de AT_location; dwarfout.c:2853 `output_reg_number`
emite el numero de registro como .unaligned INT -> operandos de 4 BYTES):
  OP_REG(01)+u4 reg | OP_BASEREG(02)+u4 reg | OP_ADDR(03)+u4 dir |
  OP_CONST(04)+u4 | OP_DEREF2/4(05/06) | OP_ADD(07) | OP_MULT(80)
Numeros de registro = DBX_REGISTER_NUMBER de GCC/PPC: 0-31 = r0..r31
(basereg 1 = SP), 32-63 = f0..f31.
  Ejemplos decodificados: 'vert_comp' = [01 0000003e] = f30 (float, u16
  0x000e); 'hcomp' = [02 00000001][04 00000028][07] = *(r1+0x28).

Arbol: AT_sibling (REF) apunta al final del SUBARBOL del DIE (donde empieza
su siguiente hermano); los hijos de X son los DIE contenidos en (X, sib(X)).
Validado: sib(CU zAI) = offset de la CU siguiente. El arbol que sale para
locales consecutivos esta a veces re-anidado (emision del compilador, ya
visto en r69 §6 con GetKeySize) -- para atribuir funcion/inline no importa.

Uso:
    python scripts/dwarf1.py cus              unidades con fichero y rango
    python scripts/dwarf1.py fn <nombre>      ficha de una funcion
        <nombre> = 'Update', 'Clase::Metodo', nombre mangleado del .symtab
        o una direccion 0x800XXXXX. Imprime: CU (ruta D:/mw real), parametros
        y locales con nombre+tipo+ubicacion, bloques lexicos y arbol de
        TAG_inlined_subroutine con nombres.
    python scripts/dwarf1.py at 0x9E82CC      que funcion/CU contiene ese
                                               offset de .debug (anclas)
    python scripts/dwarf1.py die 0x9E82BE     dump crudo de un DIE
    python scripts/dwarf1.py scan             validacion global del walk
"""
import bisect
import os
import pickle
import struct
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')

# --------------------------------------------- constantes (ver cabecera/dwarf.h)
TAG_array, TAG_class, TAG_entry, TAG_enum = 0x0001, 0x0002, 0x0003, 0x0004
TAG_formal_parameter, TAG_global_subroutine = 0x0005, 0x0006
TAG_global_variable, TAG_label, TAG_lexical_block = 0x0007, 0x000a, 0x000b
TAG_local_variable, TAG_member, TAG_pointer_type = 0x000c, 0x000d, 0x000f
TAG_reference_type, TAG_compile_unit, TAG_string_type = 0x0010, 0x0011, 0x0012
TAG_structure, TAG_subroutine, TAG_subroutine_type = 0x0013, 0x0014, 0x0015
TAG_typedef, TAG_union, TAG_unspec_params = 0x0016, 0x0017, 0x0018
TAG_inheritance, TAG_inlined_subroutine = 0x001c, 0x001d
TAG_subrange = 0x0021

TAG_NAMES = {
    0x0001: 'array_type', 0x0002: 'class_type', 0x0003: 'entry_point',
    0x0004: 'enumeration_type', 0x0005: 'formal_parameter',
    0x0006: 'global_subroutine', 0x0007: 'global_variable', 0x000a: 'label',
    0x000b: 'lexical_block', 0x000c: 'local_variable', 0x000d: 'member',
    0x000f: 'pointer_type', 0x0010: 'reference_type',
    0x0011: 'compile_unit', 0x0012: 'string_type', 0x0013: 'structure_type',
    0x0014: 'subroutine', 0x0015: 'subroutine_type', 0x0016: 'typedef',
    0x0017: 'union_type', 0x0018: 'unspecified_parameters',
    0x001c: 'inheritance', 0x001d: 'inlined_subroutine',
    0x0021: 'subrange_type',
}

AT_sibling, AT_location, AT_name = 0x0012, 0x0023, 0x0038
AT_fund_type, AT_mod_fund_type = 0x0055, 0x0063
AT_user_def_type, AT_mod_u_d_type = 0x0072, 0x0083
AT_byte_size, AT_stmt_list, AT_low_pc, AT_high_pc = 0x00b6, 0x0106, 0x0111, 0x0121
AT_language, AT_comp_dir, AT_inline = 0x0136, 0x01b8, 0x0208
AT_abstract_origin, AT_producer = 0x02b2, 0x0258
AT_src_coords = 0x8036

AT_NAMES = {
    0x0012: 'sibling', 0x0023: 'location', 0x0038: 'name',
    0x0055: 'fund_type', 0x0063: 'mod_fund_type', 0x0072: 'user_def_type',
    0x0083: 'mod_u_d_type', 0x00b6: 'byte_size', 0x0106: 'stmt_list',
    0x0111: 'low_pc', 0x0121: 'high_pc', 0x0136: 'language',
    0x01b8: 'comp_dir', 0x0208: 'inline', 0x02b2: 'abstract_origin',
    0x0258: 'producer', 0x8036: 'src_coords', 0x8006: 'sf_names',
    0x8016: 'src_info',
}

FORM_ADDR, FORM_REF, FORM_BLOCK2, FORM_BLOCK4 = 1, 2, 3, 4
FORM_DATA2, FORM_DATA4, FORM_DATA8, FORM_STRING = 5, 6, 7, 8

# tipos fundamentales FT_* (dwarf.h)
FT = {
    0x0001: 'char', 0x0002: 'signed char', 0x0003: 'unsigned char',
    0x0004: 'short', 0x0005: 'signed short', 0x0006: 'unsigned short',
    0x0007: 'int', 0x0008: 'signed int', 0x0009: 'unsigned int',
    0x000a: 'long', 0x000b: 'signed long', 0x000c: 'unsigned long',
    0x000d: 'void *', 0x000e: 'float', 0x000f: 'double',
    0x0010: 'long double', 0x0011: 'complex', 0x0012: 'double complex',
    0x0014: 'void', 0x0015: 'boolean', 0x0017: 'label',
    0x8008: 'long long', 0x8108: 'signed long long',
    0x8208: 'unsigned long long',
    0x9001: 'int8', 0x9101: 'signed int8', 0x9201: 'unsigned int8',
    0x9302: 'int16', 0x9402: 'signed int16', 0x9502: 'unsigned int16',
    0x9604: 'int32', 0x9704: 'signed int32', 0x9804: 'unsigned int32',
    0x9908: 'int64', 0x9a08: 'signed int64', 0x9b08: 'unsigned int64',
    0xa004: 'real32', 0xa108: 'real64', 0xa20c: 'real96', 0xa310: 'real128',
}
# modificadores de tipo MOD_* (dwarf.h); los de sufijo van al final
MOD_PTR = {1: '*', 2: '&'}
MOD_PRE = {3: 'const ', 4: 'volatile '}

OP_REG, OP_BASEREG, OP_ADDR, OP_CONST = 0x01, 0x02, 0x03, 0x04
OP_DEREF2, OP_DEREF4, OP_ADD, OP_MULT = 0x05, 0x06, 0x07, 0x80

U4 = struct.Struct('>I')
U2 = struct.Struct('>H')


# --------------------------------------------------------------------- ELF

class Elf(object):
    def __init__(self, path=ELF):
        self.path = path
        self.d = open(path, 'rb').read()
        d = self.d
        assert d[:4] == b'\x7fELF' and d[5] == 2, 'no es ELF32 big-endian'
        e_shoff, = U4.unpack_from(d, 0x20)
        e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('>HHH', d, 0x2E)
        raw = []
        for i in range(e_shnum):
            o = e_shoff + i * e_shentsize
            n, t, fl, ad, of, sz, lk, inf, al, es = struct.unpack_from('>10I', d, o)
            raw.append(dict(name=n, typ=t, addr=ad, off=of, size=sz))
        so = raw[e_shstrndx]['off']
        for s in raw:
            e = d.index(b'\0', so + s['name'])
            s['n'] = d[so + s['name']:e].decode('latin1')
        self.by = dict((s['n'], s) for s in raw)

    def sec(self, name):
        s = self.by[name]
        return self.d[s['off']:s['off'] + s['size']]

    def symbols(self):
        """[(addr, name, size)] de .symtab, sin SECTION ni FILE ni locales."""
        d = self.d
        st, sr = self.by['.symtab'], self.by['.strtab']
        out = []
        for o in range(st['off'], st['off'] + st['size'], 16):
            nm, val, sz, info, other, shndx = struct.unpack_from('>IIIBBH', d, o)
            if (info & 0xF) in (3, 4) or not nm:
                continue
            base = sr['off'] + nm
            e = d.index(b'\0', base)
            name = d[base:e].decode('latin1')
            if name and name != 'gcc2_compiled.':
                out.append((val, name, sz))
        return out


# ------------------------------------------------------------------ un DIE

def die_attrs(dbg, p, end):
    """Attrs de un DIE entre p y end. -> ({attr: valor}, truncados)"""
    at, bad = {}, 0
    while p + 2 <= end:
        a, = U2.unpack_from(dbg, p)
        p += 2
        f = a & 0xF
        if f == FORM_ADDR or f == FORM_REF:
            v, = U4.unpack_from(dbg, p); p += 4
        elif f == FORM_DATA2:
            v, = U2.unpack_from(dbg, p); p += 2
        elif f == FORM_DATA4:
            v, = U4.unpack_from(dbg, p); p += 4
        elif f == FORM_DATA8:
            v = dbg[p:p + 8]; p += 8
        elif f == FORM_BLOCK2:
            n, = U2.unpack_from(dbg, p); p += 2
            v = dbg[p:p + n]; p += n
        elif f == FORM_BLOCK4:
            n, = U4.unpack_from(dbg, p); p += 4
            v = dbg[p:p + n]; p += n
        elif f == FORM_STRING:
            e = dbg.index(b'\0', p, end + 1) if b'\0' in dbg[p:end + 1] else end
            v = dbg[p:e].decode('latin1'); p = e + 1
        else:                       # niblo 0 = fin prematuro (corrupto)
            bad += 1
            break
        if p > end:
            bad += 1
            break
        at.setdefault(a, v)
    return at, bad


def die_at(dbg, off):
    """-> dict del DIE en `off` (tag, at, end, sub) o None si no parsea."""
    if off + 6 > len(dbg):
        return None
    ln, = U4.unpack_from(dbg, off)
    if ln < 6 or off + ln > len(dbg):
        return None
    tag, = U2.unpack_from(dbg, off + 4)
    at, _ = die_attrs(dbg, off + 6, off + ln)
    return dict(off=off, len=ln, tag=tag, at=at,
                end=off + ln, sub=at.get(AT_sibling, off + ln))


# ---------------------------------------------------------- walk de .debug

CACHE = os.path.join(tempfile.gettempdir(), 'nfsmw_dwarf1_cache.pkl')
ROUTINE_TAGS = (TAG_global_subroutine, TAG_subroutine)


def walk(force=False):
    """Recorre .debug entero. -> dict cache con:
        cus  [(off, end, fichero, productor, comp_dir)]   en orden
        fns  [(off, sub, low, high, nombre, cu, tag, inline)] de rutinas
        stats
    El arbol fino de una funcion se re-parsea on demand (ver fn_subtree)."""
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

    dbg = Elf().sec('.debug')
    n = len(dbg)
    cus, fns = [], []
    nd, nmark, nodd = 0, 0, 0
    p = 0
    up4, up2 = U4.unpack_from, U2.unpack_from
    while p < n:
        ln, = up4(dbg, p)
        if ln == 4:                            # end_sibling_chain
            nmark += 1
            p += 4
            continue
        if ln < 6 or p + ln > n:
            # DIE truncado por el enlazador (secciones condicionales):
            # saltarlo y resincronizar (medido: 1 caso, len=5, en 0x3FE803)
            nodd += 1
            p += ln if ln >= 1 else 4
            continue
        tag, = up2(dbg, p + 4)
        # attrs que interesan del tirón (sin construir dict completo por DIE)
        q, end = p + 6, p + ln
        name = low = high = sib = None
        inl = False
        while q + 2 <= end:
            a, = up2(dbg, q)
            q += 2
            f = a & 0xF
            if f == FORM_ADDR or f == FORM_REF or f == FORM_DATA4:
                v, = up4(dbg, q)
                q += 4
                if a == AT_sibling:
                    sib = v
                elif a == AT_low_pc:
                    low = v
                elif a == AT_high_pc:
                    high = v
            elif f == FORM_DATA2:
                q += 2
            elif f == FORM_DATA8:
                q += 8
            elif f == FORM_BLOCK2:
                nb, = up2(dbg, q)
                q += 2 + nb
            elif f == FORM_BLOCK4:
                nb, = up4(dbg, q)
                q += 4 + nb
            elif f == FORM_STRING:
                if a == AT_name:
                    e = dbg.index(b'\0', q, end + 1)
                    name = dbg[q:e].decode('latin1')
                    q = e + 1
                elif a == AT_inline:
                    e = dbg.index(b'\0', q, end + 1)
                    inl = True                 # cualquier AT_inline = inline
                    q = e + 1
                else:
                    e = dbg.index(b'\0', q, end + 1)
                    q = e + 1
            else:
                break
            if q > end:
                break
        sub = sib if sib is not None else p + ln
        if tag == TAG_compile_unit:
            a2, _ = die_attrs(dbg, p + 6, p + ln)   # comp_dir/producer del DIE
            cus.append((p, sub, name or '?',
                        a2.get(AT_producer, ''), a2.get(AT_comp_dir, '')))
        elif tag in ROUTINE_TAGS:
            fns.append((p, sub, low, high, name, -1, tag, inl))
        nd += 1
        p += ln

    # atribuye cada rutina a su CU por rango de offsets
    starts = [c[0] for c in cus]
    ends = [c[1] for c in cus]
    for i, r in enumerate(fns):
        j = bisect.bisect_right(starts, r[0]) - 1
        if j >= 0 and r[0] >= ends[j]:         # entre subarbol de CU j y CU j+1
            j += 1
        cu = min(max(j, 0), len(cus) - 1)
        fns[i] = (r[0], r[1], r[2], r[3], r[4], cu, r[6], r[7])

    c = dict(key=key, cus=cus, fns=fns,
             stats=dict(n=nd, marks=nmark, odd=nodd, size=n))
    try:
        pickle.dump(c, open(CACHE, 'wb'), 2)
    except Exception:
        pass
    return c


# --------------------------------------------------------------- ubicaciones

def regname(r):
    if r < 32:
        return 'r%d' % r
    if r < 64:
        return 'f%d' % (r - 32)
    return 'gcc%d' % r


def loc_str(block, syms=None):
    """BLOCK2 de AT_location -> texto. Operandos de los opcodes: 4 bytes."""
    if block is None:
        return '(sin AT_location)'
    if len(block) == 0:
        return '(loc vacia: optimizado fuera)'
    p, out, stack = 0, [], []
    n = len(block)
    while p < n:
        op = block[p]
        p += 1
        if op == OP_REG:
            v, = U4.unpack_from(block, p); p += 4
            return regname(v)
        if op == OP_BASEREG:
            v, = U4.unpack_from(block, p); p += 4
            stack.append(regname(v))
        elif op == OP_ADDR:
            v, = U4.unpack_from(block, p); p += 4
            nm = ''
            if syms:
                for a, s, _ in syms.get(v, ()):
                    nm = ' <%s>' % s
                    break
            return '=0x%08X%s' % (v, nm)
        elif op == OP_CONST:
            v, = U4.unpack_from(block, p); p += 4
            sv = v - (1 << 32) if v >= (1 << 31) else v
            stack.append('%+d' % sv if stack else str(sv))
        elif op == OP_DEREF4:
            out.append('deref4')
        elif op == OP_DEREF2:
            out.append('deref2')
        elif op == OP_ADD:
            if len(stack) >= 2:
                b = stack.pop()
                stack[-1] = stack[-1] + b
        elif op == OP_MULT:
            if len(stack) >= 2:
                b = stack.pop()
                stack[-1] = '(%s*%s)' % (stack[-1], b)
        else:
            return 'op?0x%02X %s' % (op, block.hex())
    if stack and out:
        return '*(' + ' '.join(stack) + ') ' + ' '.join(out)
    if stack:
        return '[' + ' '.join(stack) + ']'
    return ' '.join(out) or block.hex()


# ------------------------------------------------------------------- tipos

def ft_str(code):
    return FT.get(code, 'FT_0x%04X' % code)


def mods_str(mods):
    pre = ''.join(MOD_PRE.get(m, 'MOD%d ' % m) for m in mods if m in MOD_PRE)
    suf = ''.join(MOD_PTR.get(m, '') for m in mods if m in MOD_PTR)
    return pre, suf


def type_str(dbg, at, seen=None):
    """Tipo de un DIE a partir de sus attrs de tipo. -> texto."""
    seen = seen or set()
    if AT_fund_type in at:
        return ft_str(at[AT_fund_type])
    if AT_mod_fund_type in at:
        b = at[AT_mod_fund_type]
        ft, = U2.unpack_from(b, len(b) - 2)
        pre, suf = mods_str(list(b[:-2]))
        return pre + ft_str(ft) + suf
    if AT_user_def_type in at:
        return ud_str(dbg, at[AT_user_def_type], seen)
    if AT_mod_u_d_type in at:
        b = at[AT_mod_u_d_type]
        ref, = U4.unpack_from(b, len(b) - 4)
        pre, suf = mods_str(list(b[:-4]))
        return pre + ud_str(dbg, ref, seen) + suf
    return '?'


def ud_str(dbg, ref, seen=()):
    if ref in seen:
        return '...'
    d = die_at(dbg, ref)
    if not d:
        return 'DIE?0x%X' % ref
    at = d['at']
    nm = at.get(AT_name) or TAG_NAMES.get(d['tag'], '?')
    sz = at.get(AT_byte_size)
    inner = ''
    if d['tag'] in (TAG_pointer_type, TAG_reference_type):
        inner = ' -> ' + type_str(dbg, at, seen | {ref})
    if d['tag'] == TAG_array:
        inner = ' ' + type_str(dbg, at, seen | {ref})
    s = nm + ('[%d]' % sz if sz else '') + inner
    return s


# --------------------------------------------------- subarbol de una rutina

class Node(object):
    __slots__ = ('die', 'kids')

    def __init__(self, die):
        self.die = die
        self.kids = []


def fn_subtree(dbg, off):
    """-> Node raiz (la rutina en `off`) con su arbol de hijos por contencion."""
    root = die_at(dbg, off)
    if not root:
        return None
    root = Node(root)
    stack = [root]
    p, end = root.die['end'], root.die['sub']
    while p < end:
        ln, = U4.unpack_from(dbg, p)
        if ln == 4:
            p += 4
            continue
        if ln < 6 or p + ln > len(dbg):
            p += ln if ln >= 1 else 4        # DIE truncado: resincroniza
            continue
        d = die_at(dbg, p)
        if not d:
            break
        nd = Node(d)
        while len(stack) > 1 and stack[-1].die['sub'] <= d['off']:
            stack.pop()
        stack[-1].kids.append(nd)
        stack.append(nd)
        p = d['end']
    return root


def resolve_origin(dbg, at):
    """Si el DIE referencia AT_abstract_origin, trae nombre/tipo de alla."""
    if AT_abstract_origin not in at:
        return None
    return die_at(dbg, at[AT_abstract_origin])


def die_name(dbg, at):
    if AT_name in at:
        return at[AT_name]
    o = resolve_origin(dbg, at)
    if o:
        return o['at'].get(AT_name)
    return None


def die_type(dbg, at):
    t = type_str(dbg, at)
    if t == '?':
        o = resolve_origin(dbg, at)
        if o:
            return type_str(dbg, o['at'])
    return t


# ------------------------------------------------------------------ comandos

def sym_by_addr(elf):
    m = {}
    for a, s, z in elf.symbols():
        m.setdefault(a, []).append((a, s, z))
    return m


def cu_of(c, off):
    starts = [x[0] for x in c['cus']]
    i = bisect.bisect_right(starts, off) - 1
    return max(i, 0)


def print_fn(c, elf, syms, rec, dbg):
    off, sub, low, high, name, cu, tag, inl = rec
    co, ce, cfile, prod, cdir = c['cus'][cu]
    symlist = syms.get(low, []) if low is not None else []
    print('=' * 78)
    print('FUNCION   %s   [%s%s]' % (
        name or '(sin nombre)', TAG_NAMES.get(tag, hex(tag)),
        ' inline' if inl else ''))
    print('CU        %s   (.debug+0x%06X..0x%06X)' % (cfile, co, ce))
    print('compilador %s' % (prod or '?'))
    print('dir       %s' % (cdir or '?'))
    if low is not None:
        sz = (high - low) if high else 0
        extra = ', '.join('%s (%d B)' % (s, z) for _, s, z in symlist)
        print('rango     0x%08X..0x%08X  (%d B)%s' % (
            low, high or 0, sz, '   [symtab: %s]' % extra if extra else ''))
    print('DIE       .debug+0x%06X  subarbol 0x%06X (%d B)' % (
        off, sub, sub - off))
    print('-' * 78)

    root = fn_subtree(dbg, off)
    if not root:
        print('  (subarbol no parsea)')
        return

    def loc_of(at):
        blk = at.get(AT_location)
        if blk is None and AT_abstract_origin in at:
            o = resolve_origin(dbg, at)
            if o:
                blk = o['at'].get(AT_location)
        return loc_str(blk, syms)

    def walk_tree(nd, depth):
        d = nd.die
        at = d['at']
        t = d['tag']
        pad = '  ' * depth
        off_s = '.debug+0x%06X' % d['off']
        if t == TAG_formal_parameter:
            print('%sparam     %-24s %-28s %-14s %s' % (
                pad, die_name(dbg, at) or '(anon)', die_type(dbg, at),
                loc_of(at), off_s))
        elif t in (TAG_local_variable, TAG_global_variable):
            print('%slocal     %-24s %-28s %-14s %s' % (
                pad, die_name(dbg, at) or '(anon)', die_type(dbg, at),
                loc_of(at), off_s))
        elif t == TAG_lexical_block:
            lo, hi = at.get(AT_low_pc), at.get(AT_high_pc)
            src = at.get(AT_src_coords)
            line = (' linea %d' % (src & 0xFFFF)) if src is not None else ''
            print('%sbloque    {0x%08X..0x%08X}%s  %s' % (
                pad, lo or 0, hi or 0, line, off_s))
        elif t == TAG_inlined_subroutine:
            o = resolve_origin(dbg, at)
            onm = o['at'].get(AT_name) if o else None
            oinl = ' inline' if (o and o['at'].get(AT_inline)) else ''
            ocur = ''
            if o is not None:
                j = cu_of(c, o['off'])
                ocur = '  [%s]' % c['cus'][j][2].split('/')[-1]
            lo, hi = at.get(AT_low_pc), at.get(AT_high_pc)
            src = at.get(AT_src_coords)
            line = (' linea %d' % (src & 0xFFFF)) if src is not None else ''
            print('%sINLINE    %s%s%s  0x%08X..0x%08X (%d B)%s  %s' % (
                pad, onm or '(?)', oinl, '', lo or 0, hi or 0,
                (hi - lo) if (hi and lo) else 0, line, off_s) + ocur)
        elif t in ROUTINE_TAGS and depth > 0:
            print('%srutina    %s  %s' % (
                pad, die_name(dbg, at) or '(anon)', off_s))
        elif t == TAG_label:
            print('%setiqueta  %s  0x%08X  %s' % (
                pad, at.get(AT_name) or '?', at.get(AT_low_pc, 0), off_s))
        elif t in (TAG_structure, TAG_class, TAG_typedef, TAG_enum,
                   TAG_union, TAG_array, TAG_pointer_type, TAG_subroutine_type,
                   TAG_string_type, TAG_entry):
            pass                       # ruido de tipos dentro de la rutina
        else:
            print('%s%-9s %-24s %s' % (
                pad, TAG_NAMES.get(t, 'tag0x%04x' % t),
                die_name(dbg, at) or '', off_s))
        for k in nd.kids:
            walk_tree(k, depth + 1)

    walk_tree(root, 0)
    # resumen
    nl = ni = nb = 0

    def count(nd):
        nonlocal nl, ni, nb
        t = nd.die['tag']
        if t in (TAG_local_variable, TAG_formal_parameter):
            nl += 1
        elif t == TAG_inlined_subroutine:
            ni += 1
        elif t == TAG_lexical_block:
            nb += 1
        for k in nd.kids:
            count(k)
    count(root)
    print('-' * 78)
    print('resumen: %d params+locales, %d inlines, %d bloques lexicos'
          % (nl, ni, nb))


def cmd_fn(query):
    c = walk()
    elf = Elf()
    dbg = elf.sec('.debug')
    syms = sym_by_addr(elf)
    q = query.strip()
    addr = None
    if q.lower().startswith('0x'):
        addr = int(q, 16)
    cls = None
    if '::' in q:
        cls, q = q.rsplit('::', 1)

    recs = []
    if addr is not None:
        recs = [r for r in c['fns'] if r[2] == addr]
    else:
        # por nombre de DIE ...
        cand = [r for r in c['fns'] if r[4] == q]
        if not cand:                   # ... o mangleado del symtab
            for a, ss in syms.items():
                if any(q == s for _, s, _ in ss):
                    cand.extend(r for r in c['fns'] if r[2] == a)
            cand = list({r[0]: r for r in cand}.values())
        if cls:
            def keep(r):
                if r[2] is None:
                    return cls in (c['cus'][r[5]][2] or '')
                return any(cls in s for _, s, _ in syms.get(r[2], ()))
            cand = [r for r in cand if keep(r)]
        recs = cand
    if not recs:
        # ultimo recurso: subcadena en symtab (filtrando por clase si la hay)
        hits = []
        for a, ss in syms.items():
            for _, s, _ in ss:
                if q in s and (not cls or cls in s):
                    hits.extend(r for r in c['fns'] if r[2] == a)
        recs = list({r[0]: r for r in hits}.values())
    if not recs:
        sys.exit('nada en .debug case con %r' % query)
    for r in sorted(recs, key=lambda r: (r[5], r[0])):
        print_fn(c, elf, syms, r, dbg)


def cmd_cus():
    c = walk()
    cus, fns = c['cus'], c['fns']
    nf = [0] * len(cus)
    for r in fns:
        nf[r[5]] += 1
    print('%d unidades, %d rutinas, %d DIE, %d marks, stats %s' % (
        len(cus), len(fns), c['stats']['n'], c['stats']['marks'],
        {k: v for k, v in c['stats'].items() if k in ('odd', 'size')}))
    print('%-7s %-9s %-6s %s' % ('offset', 'fin', 'ruti', 'fichero'))
    for i, (co, ce, f, prod, cd) in enumerate(cus):
        print('0x%05X 0x%07X %-6d %s' % (co, ce, nf[i], f))


def cmd_at(off_s):
    c = walk()
    off = int(off_s, 16)
    elf = Elf()
    syms = sym_by_addr(elf)
    hits = [r for r in c['fns'] if r[0] <= off < r[1]]
    if not hits:
        i = cu_of(c, off)
        print('.debug+0x%X cae en la CU %s (0x%X..0x%X), fuera de toda rutina'
              % (off, c['cus'][i][2], c['cus'][i][0], c['cus'][i][1]))
        return
    for r in sorted(hits, key=lambda r: r[1] - r[0]):
        co, ce, cfile, prod, cd = c['cus'][r[5]]
        symlist = ', '.join(s for _, s, _ in syms.get(r[2], ())) if r[2] else ''
        print('.debug+0x%X -> %s  (CU %s, rutina .debug+0x%X..0x%X, 0x%08X %s)'
              % (off, r[4] or '(?)', cfile, r[0], r[1], r[2] or 0, symlist))
        d = die_at(elf.sec('.debug'), off)
        if d:
            print('   DIE: tag=%s name=%s' % (
                TAG_NAMES.get(d['tag'], hex(d['tag'])), d['at'].get(AT_name)))
            for k, v in sorted(d['at'].items()):
                print('     %-18s %s' % (AT_NAMES.get(k, hex(k)), v))


def cmd_die(off_s):
    elf = Elf()
    d = die_at(elf.sec('.debug'), int(off_s, 16))
    if not d:
        sys.exit('no parsea un DIE en 0x%s' % off_s)
    print('.debug+0x%X  len=%d tag=%s sub=0x%X' % (
        d['off'], d['len'], TAG_NAMES.get(d['tag'], hex(d['tag'])), d['sub']))
    for k, v in d['at'].items():
        if isinstance(v, bytes):
            v = v.hex()
        print('  %-18s %s' % (AT_NAMES.get(k, 'at_0x%04x' % k), v))
    print('  tipo : %s' % type_str(elf.sec('.debug'), d['at']))


def cmd_scan():
    c = walk()
    s = c['stats']
    print('seccion .debug : %d B' % s['size'])
    print('DIEs parseados: %d  + %d marks end_sibling_chain' % (s['n'], s['marks']))
    print('DIE truncados : %d' % s['odd'])
    print('CUs           : %d' % len(c['cus']))
    print('rutinas       : %d (con low_pc: %d)' % (
        len(c['fns']), sum(1 for r in c['fns'] if r[2] is not None)))
    # control: ninguna rutina con low_pc empieza dentro del subarbol de otra
    fns = sorted(r for r in c['fns'] if r[2] is not None)
    bad = 0
    for a, b in zip(fns, fns[1:]):
        if b[0] < a[1]:
            bad += 1
            if bad <= 10:
                print('  SWALLOW: %s (0x%X..0x%X) contiene a %s (0x%X)'
                      % (a[4], a[0], a[1], b[4], b[0]))
    print('rutinas contenidas en el subarbol de otra (re-anidado): %d' % bad)
    # control: subarbol de CU termina donde empieza la siguiente
    cus = c['cus']
    ok = sum(1 for a, b in zip(cus, cus[1:]) if a[1] == b[0])
    print('CUs encadenadas por sibling exacto: %d/%d' % (ok, len(cus) - 1))


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd = sys.argv[1]
    if cmd == 'cus':
        cmd_cus()
    elif cmd == 'fn':
        if len(sys.argv) < 3:
            sys.exit('uso: dwarf1.py fn <nombre|Clase::Metodo|0xADDR>')
        cmd_fn(sys.argv[2])
    elif cmd == 'at':
        cmd_at(sys.argv[2])
    elif cmd == 'die':
        cmd_die(sys.argv[2])
    elif cmd == 'scan':
        cmd_scan()
    elif cmd == 'walk!':
        walk(force=True)
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main()
