#!/usr/bin/env python3
"""audit.py -- PRUEBA que una funcion casa, en vez de creerse el porcentaje.

Portado del arnes que jferr tiene en `scratchpad/phase13_*_audit.py`. Resuelve
el problema conocido como "el 100% que miente": objdiff puede dar 100% mientras
las reubicaciones apuntan a otro sitio, porque compara la GRAFIA de la
instruccion y no su destino.

Lo que comprueba, funcion a funcion:

  bytes        reconstruye los bytes de nuestra funcion y los compara con los
               del ELF original en la direccion que le da symbols.txt.
  ramas        cada rama local tiene el MISMO destino relativo y la misma
               condicion/operando de CR, no solo el mismo mnemonico.
  reubicaciones cada una tiene el mismo tipo y el mismo addend, y su simbolo
               resuelve a la misma direccion.
  literales    cuando el nuestro es un `$LC` y el del objetivo un `lbl_`, se
               compara **el valor de 4 bytes**, no que "haya un literal".

    python scripts/audit.py <unidad> [Simbolo ...]

Sin simbolos audita todas las funciones de la unidad que objdiff da al 100%,
que es justo donde el porcentaje puede estar mintiendo.
"""
import json
import os
import pickle
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rodata

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
SYMS = os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt')
SCR = os.environ.get('SCRATCH', os.path.join(ROOT, 'build'))
XDEF = os.path.join(SCR, 'audit_xdef.pkl')


def xdef():
    """nombre de simbolo -> ruta del .o que lo DEFINE.

    Un simbolo UNDEF en la unidad auditada (dato o literal que vive en otra
    unidad) hacia que el cotejo de literales leyera la CABECERA del .o local
    (basura '\\x7fELF') y cantara un 'literal distinto' falso: medido en
    LoadGlobalAChunks, cuyo marcador define zPlatform. El indice se construye
    una vez y se cachea en build/.
    """
    if os.path.exists(XDEF):
        try:
            with open(XDEF, 'rb') as fh:
                return pickle.load(fh)
        except Exception:
            pass
    idx = {}
    base = os.path.join(ROOT, 'build', 'GOWE69', 'src')
    for dirpath, _, files in os.walk(base):
        for f in sorted(files):
            if not f.endswith('.o'):
                continue
            p = os.path.join(dirpath, f)
            try:
                _, _, _, ss = rodata.syms(p)
            except Exception:
                continue
            for n, v, sz, ix in ss:
                if n and ix and n not in idx:
                    idx[n] = p
    try:
        with open(XDEF, 'wb') as fh:
            pickle.dump(idx, fh)
    except Exception:
        pass
    return idx


def diff_json(unit):
    a = os.path.join(ROOT, 'build', 'GOWE69', 'obj', unit.replace('/', os.sep) + '.o')
    b = os.path.join(ROOT, 'build', 'GOWE69', 'src', unit.replace('/', os.sep) + '.o')
    for p in (a, b):
        if not os.path.exists(p):
            raise SystemExit('falta el objeto: %s' % p)
    out = os.path.join(SCR, 'audit_%s.json' % unit.replace('/', '_'))
    r = subprocess.run([CLI, 'diff', '-1', a, '-2', b,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false',
                        '--format', 'json', '-o', out], cwd=ROOT, capture_output=True)
    if r.returncode != 0 or not os.path.exists(out):
        raise SystemExit('objdiff falla en %s' % unit)
    d = json.load(open(out, encoding='utf-8'))
    os.remove(out)
    return d, b


def direcciones():
    elf, _, secs, syms = rodata.syms(ELF)
    d = {n: v for n, v, _, _ in syms if n}
    tam = {n: sz for n, _, sz, _ in syms if n and sz}
    if os.path.exists(SYMS):
        txt = open(SYMS, encoding='utf-8', errors='replace').read()
        for n, v in re.findall(r'^(\S+) = \.\S+:(0x[0-9A-Fa-f]+);', txt, re.M):
            d[n] = int(v, 16)
        # `lbl_804FF838 = .sdata:0x804FF838; // type:object size:0x8 ...`
        for n, sz in re.findall(r'^(\S+) = \.\S+:0x[0-9A-Fa-f]+;.*?\bsize:(0x[0-9A-Fa-f]+)',
                                txt, re.M):
            tam[n] = int(sz, 16)
    # El troceador nombra los literales anonimos de las unidades Metrowerks
    # `@N_ADDR` y NO los declara en symbols.txt: sin esto `na not in addr` corta
    # el cotejo por valor antes de empezar. La direccion esta en el propio
    # nombre, que es de donde la saca el troceador.
    dirs = DirsMW(d)
    # Rangos cargables del ELF original: un `X_8xxxxxxx` solo se acepta como
    # nombre del troceador si la direccion que lleva dentro existe de verdad.
    dirs.rangos = tuple(sorted((s['addr'], s['addr'] + s['size'])
                               for s in secs if s.get('addr')))
    return elf, secs, dirs, tam


class DirsMW(dict):
    """El mapa de direcciones. Resuelve ademas los nombres que el troceador
    INVENTA y no declara en symbols.txt.

    El troceador tiene UNA forma para eso: pegarle al nombre la direccion,
    `<nombre>_8xxxxxxx`. `@N_ADDR` de Metrowerks y `gcc2_compiled._ADDR` --que
    NO es un simbolo aparte sino LA PRIMERA FUNCION de esa unidad-- son casos
    particulares de la misma forma, y cada uno costo un FALLA falso: el de vp6
    en la r22 y el de `timerthread` en la r23, donde el objetivo referencia
    `systemtasksubs_804D63F8` y nosotros `TimerThreadStack+0x1000`, que es LA
    MISMA DIRECCION (symbols.txt: TimerThreadStack = 0x804D53F8, tamano
    0x1000). Aqui se resuelve la forma general, con dos cautelas: la direccion
    tiene que caer en una seccion cargable del ELF original, y el nombre no
    puede estar ya en el mapa (`__missing__` solo entra si falta).
    """

    rangos = ()

    def _troc(self, k):
        m = re.match(r'^[^\s]*?_([0-9A-Fa-f]{8})$', k or '')
        if not m:
            return None
        v = int(m.group(1), 16)
        for a, b in self.rangos:
            if a <= v < b:
                return v
        return None

    def __missing__(self, k):
        v = self._troc(k)
        if v is None:
            raise KeyError(k)
        return v

    def __contains__(self, k):
        return dict.__contains__(self, k) or self._troc(k) is not None

    def get(self, k, dflt=None):
        try:
            return self[k]
        except KeyError:
            return dflt


# El troceador inventa este nombre para un dato ANONIMO del objetivo (una
# entrada del pool de constantes del compilador, que no tiene nombre propio).
# Su identidad ES su valor: cotejarlo por nombre es imposible y cotejarlo por
# bytes es la prueba de verdad.
RE_ANON = re.compile(r'^(?:lbl_|@\d+_|gcc2_compiled\._)[0-9A-Fa-f]{8}$')
# La forma `@N_ADDR` es la del troceador para las unidades METROWERKS, donde
# el pool no se llama `lbl_`. Sin ella el cotejo por valor no se intentaba y
# salia un FALLA con la reubicacion correcta: medido en EXI2_ReadN, donde
# `@28_8044F610` y nuestro `@225` son la misma cadena de EXI2.
# tipos de reubicacion que apuntan a un DATO (no a codigo): sin esto la
# comparacion por valor compararia instrucciones de dos funciones distintas.
TIPOS_DATO = (1, 4, 5, 6, 109, 110, 111, 112)
# Cuantos bytes lee/escribe de verdad la instruccion que lleva la reubicacion.
# El `size:` de symbols.txt es una ESTIMACION del troceador y a veces se come
# el relleno: `lbl_804FF208` es un float y symbols.txt le pone `size:0x8`.
ANCHO = {}
for _m, _n in (('lbz', 1), ('stb', 1), ('lha', 2), ('lhz', 2), ('sth', 2),
               ('lwz', 4), ('stw', 4), ('lfs', 4), ('stfs', 4), ('lwa', 4),
               ('lfd', 8), ('stfd', 8), ('ld', 8), ('std', 8)):
    for _suf in ('', 'u', 'x', 'ux'):
        ANCHO[_m + _suf] = _n


def bytes_orig(elf, secs, addr, size):
    for s in secs:
        if s['addr'] and s['addr'] <= addr and addr + size <= s['addr'] + s['size']:
            o = s['off'] + addr - s['addr']
            return elf[o:o + size]
    return None


def dir_compatible(elf, secs, contenido, tipo, crudo, esperado, conocidas):
    """Cierto si ALGUN dato del ELF con ese CONTENIDO vive en una direccion cuyo
    campo de reubicacion coincide con el inmediato crudo del objetivo.

    Hace falta cuando el objetivo trae el inmediato crudo --`lis r11, 0x803e` y
    luego `lfs f12, -0xb8c(r7)`-- porque APARCA EL @ha EN LR y el troceador no
    puede emparejar las dos mitades, mientras nosotros traemos `$LC980@ha` y
    `$LC980@l` con reubicaciones sin resolver. Los nombres no se pueden cotejar
    (el nuestro es un `$LC` local) y nuestras direcciones no existen hasta
    enlazar: lo unico probable es que nuestro literal caiga en una direccion
    cuyo campo coincida. Medido en zEcstasy: $LC980 = 80,0f y 0x803DF474 =
    80,0f, con ha(0x803DF474) = 0x803E y lo(0x803DF474) = -0xB8C.
    """
    if not contenido or crudo is None:
        return False
    for s in secs:
        if not s['addr'] or s.get('typ') == 8:
            continue
        blob = elf[s['off']:s['off'] + s['size']]
        j = blob.find(contenido)
        while j >= 0:
            a = s['addr'] + j
            # EXIGIR QUE SEA UN SIMBOLO CONOCIDO. Sin esto la prueba es
            # demasiado laxa para valores comunes: hay millones de palabras a
            # cero en el DOL y alguna casaria el campo por casualidad.
            if a in conocidas and esperado(tipo, a) == (crudo & 0xFFFF):
                return True
            j = blob.find(contenido, j + 1)
    return False


RE_IMM = re.compile(r'(-?0x[0-9a-fA-F]+|-?\d+)\s*(?:\(r\d+\))?\s*$')


def relocadas(path):
    """-> {indice de seccion: set(desplazamientos relocados)} de NUESTRO .o.

    Sin esto un literal que CONTIENE punteros se lee como "literal distinto":
    en nuestro objeto sin enlazar esas 4 palabras valen 0 y en el ELF ya estan
    resueltas. Medido: 4 de los 5 "literal distinto" del lote de zAI eran esto.
    """
    d, E, secs = rodata.load(path)
    out = {}
    for sec in secs:
        if sec['typ'] not in (4, 9):          # SHT_RELA / SHT_REL
            continue
        esz = sec['entsize'] or (12 if sec['typ'] == 4 else 8)
        st = out.setdefault(sec['info'], set())
        for o in range(sec['off'], sec['off'] + sec['size'], esz):
            st.add(struct.unpack(E + 'I', d[o:o + 4])[0])
    return out


def imm_de(txt):
    m = RE_IMM.search(txt)
    return int(m.group(1), 0) if m else None


def esperado(tipo, a):
    """El inmediato de 16 bits que corresponde a la direccion `a`."""
    if tipo == 6:                              # R_PPC_ADDR16_HA
        return ((a >> 16) + (1 if (a & 0x8000) else 0)) & 0xFFFF
    if tipo == 5:                              # R_PPC_ADDR16_HI
        return (a >> 16) & 0xFFFF
    if tipo == 4:                              # R_PPC_ADDR16_LO
        return a & 0xFFFF
    return None


def audita(unit, quiere):
    d, nuestro_o = diff_json(unit)
    izq = {s['name']: s for s in d['left']['symbols'] if s.get('name')}
    der = {s['name']: s for s in d['right']['symbols'] if s.get('name')}
    elf, secs, addr, tam_sym = direcciones()
    conocidas = set(addr.values())
    raw, E, osecs, osyms = rodata.syms(nuestro_o)
    relmap = relocadas(nuestro_o)
    named = {n: (v, sz, ix) for n, v, sz, ix in osyms if n}
    # Simbolos LOCALES por (seccion, valor), de los DOS objetos de la unidad.
    # El cotejo por direccion del ELF no vale para nombres que se repiten en
    # cada unidad (`__static_initialization_and_destruction_0`, marcadores de
    # seccion): en zOnline, `_overlay_start` y el static-init viven los DOS en
    # .over+0 y el troceador/el compilador eligen distinto nombre para la
    # MISMA direccion. Sin esto, `_GLOBAL_.I.__OSBusClock` cantaba FALLA.
    loc_our = {n: (osecs[ix].get('sname'), v) for n, v, sz, ix in osyms
               if n and ix and ix < len(osecs)}
    loc_tgt = {}
    tgt_o = nuestro_o.replace(os.sep + 'src' + os.sep, os.sep + 'obj' + os.sep, 1)
    if os.path.exists(tgt_o):
        try:
            _, _, tsecs, tsyms = rodata.syms(tgt_o)
            loc_tgt = {n: (tsecs[ix].get('sname'), v) for n, v, sz, ix in tsyms
                       if n and ix and ix < len(tsecs)}
        except Exception:
            pass

    # Nuestros literales se llaman `$LCn` y los del objetivo `lbl_ADDR`, asi
    # que un `$LC` sin pareja no se puede resolver por su nombre. Este mapa lo
    # saca de las OTRAS apariciones del mismo `$LC` en la unidad, donde objdiff
    # si reconstruyo la reubicacion en los dos lados.
    pareja = {}
    for n, Ls in izq.items():
        Rs = der.get(n)
        if Rs is None:
            continue
        for a, b in zip(Ls.get('instructions') or [], Rs.get('instructions') or []):
            ia0, ib0 = a.get('instruction'), b.get('instruction')
            if not ia0 or not ib0:
                continue
            ra, rb = ia0.get('relocation'), ib0.get('relocation')
            if not ra or not rb:
                continue
            na = d['left']['symbols'][ra['target_symbol']].get('name')
            nb = d['right']['symbols'][rb['target_symbol']].get('name')
            if na and nb and nb not in pareja:
                pareja[nb] = na

    nombres = quiere or [n for n, s in der.items()
                         if s.get('kind') == 'SYMBOL_FUNCTION'
                         and float(s.get('match_percent') or 0) >= 100.0]
    if not nombres:
        print('  nada que auditar en %s' % unit)
        return 0

    malas = 0
    for fn in sorted(nombres):
        if fn not in izq or fn not in der:
            print('  %-52s NO ESTA EN EL DIFF' % fn[:52])
            malas += 1
            continue
        # `__tcf_N` aparece 7 veces en symbols.txt --uno por unidad de
        # traduccion-- asi que la direccion que devuelve es la de OTRA. El
        # cotejo de bytes contra el ELF no vale para esos nombres repetidos.
        AMBIGUO = re.match(r'__tcf_\d+$|gcc2_compiled\.$'
                           r'|__static_initialization_and_destruction_\d+$', fn)
        if fn not in addr and not AMBIGUO:
            print('  %-52s sin direccion en el ELF/symbols.txt' % fn[:52])
            continue
        if fn not in named:
            print('  %-52s no esta en nuestro .o' % fn[:52])
            malas += 1
            continue
        L, R = izq[fn], der[fn]
        li, ri = L.get('instructions') or [], R.get('instructions') or []
        if len(li) != len(ri):
            print('  %-52s DISTINTO NUMERO DE INSTRUCCIONES (%d/%d)' % (fn[:52], len(li), len(ri)))
            malas += 1
            continue

        start, size, tix = named[fn]
        sec = osecs[tix]
        nuestro = bytes(raw[sec['off'] + start:sec['off'] + start + size])
        a_fn = None if AMBIGUO else addr.get(fn)
        orig = bytes_orig(elf, secs, a_fn, size) if a_fn is not None else None

        ramas = relocs = lits = 0
        fallo = None
        for a, b in zip(li, ri):
            ia, ib = a['instruction'], b['instruction']
            ba, bb = ia.get('branch_dest'), ib.get('branch_dest')
            if ba is not None or bb is not None:
                if ba is None or bb is None:
                    fallo = 'una rama sin pareja'
                    break
                # objdiff OMITE `address` cuando vale 0: sin el .get, la primera
                # funcion de cada seccion revienta con KeyError y aborta la unidad.
                if int(ba) - int(L.get('address', 0)) != int(bb) - int(R.get('address', 0)):
                    fallo = 'destino de rama distinto'
                    break
                if ia['formatted'].rsplit(' ', 1)[0] != ib['formatted'].rsplit(' ', 1)[0]:
                    fallo = 'condicion/CR de rama distinta: %s | %s' % (ia['formatted'], ib['formatted'])
                    break
                ramas += 1
                if ia.get('relocation') or ib.get('relocation'):
                    # el campo de desplazamiento aun no esta resuelto en
                    # nuestro .o: el cotejo de bytes crudos no vale aqui
                    relocs += 1
                continue
            ra, rb = ia.get('relocation'), ib.get('relocation')
            if bool(ra) != bool(rb):
                # objdiff NO reconstruye la reubicacion en el objeto objetivo
                # cuando el `lis` queda sin su `@l` emparejado (por ejemplo un
                # `lis` muerto que el siguiente sobreescribe). Ahi el objetivo
                # trae el inmediato crudo y esto no es un fallo: hay que
                # comparar el VALOR, que es lo que se puede probar.
                r0 = ra or rb
                crudo = imm_de(ib['formatted'] if ra else ia['formatted'])
                nsym = (d['left']['symbols'] if ra else d['right']['symbols'])[
                    r0['target_symbol']].get('name')
                base = addr.get(nsym)
                if base is None and not ra:
                    base = addr.get(pareja.get(nsym))
                esp = (esperado(r0['type'], base + int(r0.get('addend', 0)))
                       if base is not None else None)
                if esp is None or crudo is None or (crudo & 0xFFFF) != esp:
                    # Ultimo recurso: el objetivo trae el inmediato crudo porque aparca
                    # el @ha en LR y el troceador no puede emparejarlo con su @l. Ahi el
                    # nombre no se puede cotejar --el nuestro es un `$LC` local sin
                    # direccion hasta enlazar-- y lo unico probable es que nuestro
                    # literal caiga en una direccion del DOL cuyo @ha sea ese inmediato.
                    cont = None
                    if not ra and nsym in named:
                        vv, ssz, iix = named[nsym]
                        if iix and iix < len(osecs) and osecs[iix].get('typ') != 8:
                            off = osecs[iix]['off'] + vv
                            cont = raw[off:off + 4]
                    if not (crudo is not None and cont
                            and dir_compatible(elf, secs, cont, r0['type'],
                                              crudo, esperado, conocidas)):
                        fallo = 'reubicacion en un solo lado: %s | %s' % (ia['formatted'], ib['formatted'])
                        break
                relocs += 1
                continue
            if not ra:
                if ia['formatted'] != ib['formatted']:
                    fallo = 'instruccion distinta: %s | %s' % (ia['formatted'], ib['formatted'])
                    break
                continue
            na = d['left']['symbols'][ra['target_symbol']].get('name')
            nb = d['right']['symbols'][rb['target_symbol']].get('name')
            # Lo que tiene que coincidir es la DIRECCION FINAL, no el addend
            # suelto: `SceneryDrawInfoTable+0xa410` y `m_RequiredSlots+0`
            # resuelven al mismo sitio y no son un fallo. Sin esto salian FALLA
            # falsas en unidades intactas.
            ea = addr.get(na, 0) + int(ra.get('addend', 0)) if na in addr else None
            eb = addr.get(nb, 0) + int(rb.get('addend', 0)) if nb in addr else None
            mismo_destino = ea is not None and ea == eb
            # Dato ANONIMO del objetivo (`lbl_ADDR`) contra un simbolo nuestro
            # con otro nombre: el addend no tiene por que coincidir --el pool
            # del compilador vive en UNA seccion y se referencia con
            # `[.sdata]+0x38`, mientras el troceador le puso nombre propio a
            # cada entrada-- asi que aqui la prueba es el VALOR, mas abajo.
            secsym = bool(nb) and nb.startswith('[') and nb.endswith(']')
            # `$LCn` y `_.tmp_N.M` ya los resolvia el camino de siempre, con su
            # propia regla de desplazamiento: no se toca.
            # `@N` a secas es como llama MWCC a sus propios literales, igual
            # que `$LCn` en GCC: el numero es un contador y no significa nada.
            viejo = ((nb or '').startswith('$LC')
                     or bool(re.match(r'@\d+$', nb or ''))
                     or bool(re.match(r'_\.tmp_\d+\.\d+$', na or '') and
                             re.match(r'_\.tmp_\d+\.\d+$', nb or '')))
            anon = (nb != na and na is not None and RE_ANON.match(na)
                    and ra['type'] in TIPOS_DATO and na in addr
                    and (secsym or not viejo
                         or int(ra.get('addend', 0)) == int(rb.get('addend', 0))))
            if ra['type'] != rb['type'] or (
                    not anon
                    and int(ra.get('addend', 0)) != int(rb.get('addend', 0))
                    and not mismo_destino):
                fallo = 'tipo/addend de reubicacion distinto en %s' % na
                break
            # `unlockType.33568` contra `unlockType.38626_8041CF8C`: el numero de
            # serie lo pone el compilador y el sufijo `_ADDR` lo pone symbols.txt.
            # El `.NNN` lo pone el compilador y el `_ADDR` lo pone symbols.txt
            # cuando el dato es anonimo. Los dos sufijos pueden venir sueltos:
            # medido, `exi` contra `exi_804FF598` daba 18 FALLA falsas en
            # steering y DebuggerDriver, unidades intactas.
            RE_SER = r'(\.\d+)?(_[0-9A-Fa-f]{6,8})?$'
            if nb != na and re.sub(RE_SER, '', na or '') == re.sub(RE_SER, '', nb or ''):
                relocs += 1
                continue
            if na == 'gcc2_compiled.' and nb in named and named[nb][0] == 0                     and int(ra.get('addend', 0)) == 0:
                # `gcc2_compiled.` es un marcador de tamano 0 en .text+0. objdiff
                # lo elige en un lado y la funcion real (misma direccion) en el
                # otro: es un empate de nombres, no un destino distinto.
                relocs += 1
                continue
            if nb != na and mismo_destino:
                # `PostCollisionSteerReductionData+0x18` contra
                # `SteerInputRemapTables-0x14`: el troceador nombra la direccion
                # por el simbolo QUE LA CONTIENE y nosotros por el simbolo del
                # que partimos; los dos suman 0x80489F64. La prueba es la
                # DIRECCION FINAL, que ya esta calculada arriba en
                # `mismo_destino`, no el par (nombre, addend). Sin esto,
                # `DoHumanSteering` daba FALLA con la reubicacion ya correcta.
                relocs += 1
                continue
            if (nb != na and loc_tgt.get(na) is not None
                    and loc_tgt.get(na) == loc_our.get(nb)
                    and int(ra.get('addend', 0)) == int(rb.get('addend', 0))):
                # empate de nombres LOCALES: misma (seccion, valor) en los dos
                # objetos de la unidad -- el destino es el mismo y solo cambia
                # cual de los dos simbolos gemelos elige cada lado.
                relocs += 1
                continue
            if nb != na:
                # nuestro literal se llama distinto: hay que comparar el VALOR.
                # Vale para `$LCn` y para los temporales `_.tmp_N.M`, donde el N
                # tambien es un contador del compilador y no significa nada.
                if not (viejo or anon) or na not in addr:
                    fallo = 'reubicacion a otro simbolo: %s contra %s' % (na, nb)
                    break
                v, lsz, lix = named.get(nb, (None, 0, 0))
                draw, dsecs = raw, osecs
                if v is not None and lix == 0:
                    # UNDEF en esta unidad: el dato lo define otra. Sin esto
                    # se lee la cabecera del .o local como si fuera el dato.
                    ext = xdef().get(nb)
                    if ext:
                        try:
                            eraw, eE, esecs, essyms = rodata.syms(ext)
                            for n2, v2, sz2, ix2 in essyms:
                                if n2 == nb and ix2 and ix2 < len(esecs):
                                    v, lsz, lix = v2, sz2, ix2
                                    draw, dsecs = eraw, esecs
                                    break
                        except Exception:
                            pass
                if v is not None and anon and not viejo:
                    # `zero+4` contra `lbl_804FEEFC`: el troceador le pone nombre
                    # propio a la palabra baja de un double, asi que el addend es
                    # NUESTRO y el ancho lo manda la instruccion.
                    v += int(rb.get('addend', 0))
                    lsz = (ANCHO.get(ia['formatted'].split(' ', 1)[0])
                           or (lsz - int(rb.get('addend', 0)) if lsz else 0)
                           or tam_sym.get(na, 0))
                if v is None and secsym:
                    # objdiff nombra `[.sdata]` al SIMBOLO DE SECCION, que en el
                    # symtab va con nombre vacio: se resuelve por el nombre de
                    # la seccion, y entonces el desplazamiento del dato lo lleva
                    # ENTERO el addend, y su tamano lo dice el ancho de la
                    # instruccion (el `size:` de symbols.txt es una estimacion
                    # del troceador: a `lbl_804FF208`, un float, le pone 0x8).
                    for k, sec in enumerate(osecs):
                        if sec.get('sname') == nb[1:-1]:
                            v = int(rb.get('addend', 0))
                            lsz = (ANCHO.get(ia['formatted'].split(' ', 1)[0])
                                   or tam_sym.get(na, 0))
                            lix = k
                            break
                if v is None:
                    fallo = 'literal %s no esta en nuestro .o' % nb
                    break
                ls = dsecs[lix]
                if ls['typ'] == 8:
                    # SHT_NOBITS: el simbolo vive en .bss y NO tiene contenido en
                    # el fichero. Leerlo devuelve basura del hueco del ELF, que es
                    # como se colaron 60 "literal distinto" falsos: los `_.tmp_N`
                    # de `_GetKind__*` son UCrc32 estaticos que rellena
                    # `__static_initialization_and_destruction_0` en ejecucion.
                    relocs += 1
                    continue
                # un literal de CADENA no mide 4 bytes: se compara su contenido
                # hasta el NUL, no un flotante.
                n = lsz if lsz else 0
                if n == 0:
                    e = draw.index(0, ls['off'] + v)
                    n = e - (ls['off'] + v) + 1
                mio = bytes(draw[ls['off'] + v:ls['off'] + v + n])
                suyo = bytes_orig(elf, secs,
                                  addr[na] + (int(ra.get('addend', 0)) if anon else 0), n)
                # Un literal que contiene PUNTEROS vale 0 en nuestro objeto sin
                # enlazar y ya esta resuelto en el ELF. Comparar esas palabras
                # da un falso "literal distinto" -- 4 de los 5 de zAI eran esto.
                dif = suyo is None or mio != suyo
                if dif and suyo is not None and n % 4 == 0:
                    rel = relmap.get(lix, ())
                    dif = any(mio[k:k + 4] != suyo[k:k + 4]
                              for k in range(0, n, 4) if (v + k) not in rel)
                if dif:
                    if n == 4:
                        f1 = struct.unpack('>f', mio)[0]
                        f2 = struct.unpack('>f', suyo)[0] if suyo else None
                        fallo = 'literal distinto: %s=%r contra %s=%r' % (nb, f1, na, f2)
                    else:
                        fallo = 'literal distinto (%d B): %r contra %r' % (n, mio[:40], (suyo or b'')[:40])
                    break
                lits += 1
            relocs += 1

        if fallo is None and orig is not None and nuestro != orig:
            n = sum(1 for i in range(0, min(len(nuestro), len(orig)), 4)
                    if nuestro[i:i + 4] != orig[i:i + 4])
            # los bytes crudos difieren en los campos que la reubicacion rellena
            if relocs == 0:
                fallo = '%d palabras distintas del ELF original' % n
        if fallo:
            print('  %-52s FALLA: %s' % (fn[:52], fallo))
            malas += 1
        else:
            print('  %-52s ok  %4d B, %2d ramas, %2d relocs, %d literales' %
                  (fn[:52], size, ramas, relocs, lits))
    return malas


def main():
    a = sys.argv[1:]
    if not a:
        print(__doc__)
        return 0
    return 1 if audita(a[0], a[1:]) else 0


if __name__ == '__main__':
    sys.exit(main())
