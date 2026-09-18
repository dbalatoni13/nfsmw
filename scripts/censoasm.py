#!/usr/bin/env python3
"""censoasm.py -- el censo EXACTO de todo el asm de GCC que queda en el arbol.

`grep asm` miente por dos motivos, y los dos falsean cualquier inventario:

  1. Hay CIENTOS de lineas de diagnostico en comentarios que mencionan pines
     ("// el pin register float x asm(\"fr11\") era un parche"). Aqui los
     comentarios se quitan ANTES de contar, con una maquina de estados que
     respeta cadenas y caracteres.
  2. `asm(...)` no es una cosa: un pin de registro, una barrera del
     planificador, un bloque con instrucciones PPC de verdad, un pool de
     `.section` y un ALIAS de nombre manglado son cinco animales distintos,
     con cinco costes distintos. El alias NO es andamio.

Clases:
    PIN       `register T x asm("rN"/"frN")`      -> ata una local a un registro
    BARRERA   `asm("" : ...)` / `__asm__("")`     -> ata al planificador
    INSTR     bloque con mnemonicos PPC reales
    DATOS     `asm(".section ...")` / `.byte`     -> coloca rodata/data a mano
    ALIAS     `__asm__("simbolo")` en una decl    -> renombra, NO es andamio

    python scripts/censoasm.py                # tabla por fichero + totales
    python scripts/censoasm.py --json S       # vuelca el censo crudo a S
    python scripts/censoasm.py --clase PIN    # solo una clase, con detalle
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

EXTS = ('.c', '.cpp', '.cc', '.h', '.hpp', '.inl', '.cxx')

# Mnemonicos PPC que de verdad EMITEN codigo. No hace falta el set completo:
# basta con que ninguna directiva ni ningun nombre de simbolo entre por error.
MNEM = set("""
li lis mr mr. addi addis add add. addc adde subf subf. subfic subi subis neg
mulli mullw mulhw divw divwu and and. andi. andis. or or. ori oris xor xori
nand nor eqv extsb extsh extsw slw srw sraw srawi rlwinm rlwimi rlwnm cntlzw
lwz lwzu lwzx lhz lhzu lhzx lha lhax lbz lbzu lbzx lmw lwarx
stw stwu stwux stwx sth sthu sthx stb stbu stbx stmw stwcx.
lfs lfsu lfsx lfd lfdu lfdx stfs stfsu stfsx stfd stfdu stfdx stfiwx
psq_l psq_lu psq_lx psq_st psq_stu psq_stx ps_add ps_sub ps_mul ps_madd
ps_merge00 ps_merge01 ps_merge10 ps_merge11 ps_mr ps_div ps_msub ps_nmadd
fadd fadds fsub fsubs fmul fmuls fdiv fdivs fmadd fmadds fmsub fmsubs
fnmadd fnmadds fnmsub fnmsubs fabs fnabs fneg fmr fres frsqrte frsp
fctiw fctiwz fsel fcmpo fcmpu cmpw cmpwi cmplw cmplwi cmp cmpi cmpl cmpli
tlbie tlbsync tlbia lwbrx stwbrx lhbrx sthbrx mcrf mcrxr
b ba bl bla blr bctr bctrl beq bne blt bgt ble bge bdnz bso bns
beq- bne- blt- bgt- ble- bge- beq+ bne+ blt+ bgt+ ble+ bge+
mflr mtlr mfctr mtctr mfcr mtcrf mfmsr mtmsr mfspr mtspr mffs mtfsf isync
sync eieio icbi dcbf dcbi dcbst dcbt dcbz dcbz_l nop crclr crxor creqv
sc rfi twi tw trap stswi lswi mftb mfdec mtdec
""".split())

RE_ASM = re.compile(r'(?<![A-Za-z0-9_$])(?:__asm__|__asm|asm)\s*'
                    r'(?:__volatile__|volatile|__volatile)?\s*\(')
RE_REG = re.compile(r'^(?:r|f|fr|v|cr|sp|lr|ctr)[0-9]*$')
RE_INC = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', re.M)
RE_DEF = re.compile(r'^[A-Za-z_~][A-Za-z0-9_:<>,*&\[\] \t]*[A-Za-z0-9_:~\]]\s*\(')


# ----------------------------------------------------------------- comentarios
def despoja(txt):
    """Devuelve el texto con los comentarios sustituidos por espacios.

    Conserva LONGITUD y saltos de linea, asi que los numeros de linea y los
    offsets siguen siendo los del fichero original. Respeta cadenas "..." y
    caracteres '...' (un `"/* no es comentario */"` dentro de un asm() de
    .section se queda), y las continuaciones de linea con contrabarra.
    """
    out = list(txt)
    i, n = 0, len(txt)
    while i < n:
        c = txt[i]
        if c == '/' and i + 1 < n and txt[i + 1] == '/':
            while i < n and txt[i] != '\n':
                if txt[i] == '\\' and i + 1 < n and txt[i + 1] == '\n':
                    out[i] = out[i + 1] = ' '
                    i += 2
                    continue
                out[i] = ' '
                i += 1
        elif c == '/' and i + 1 < n and txt[i + 1] == '*':
            out[i] = out[i + 1] = ' '
            i += 2
            while i < n and not (txt[i] == '*' and i + 1 < n and txt[i + 1] == '/'):
                if txt[i] != '\n':
                    out[i] = ' '
                i += 1
            if i < n:
                out[i] = ' '
                if i + 1 < n:
                    out[i + 1] = ' '
                i += 2
        elif c in '"\'':
            q = c
            i += 1
            while i < n and txt[i] != q:
                if txt[i] == '\\':
                    i += 2
                    continue
                if txt[i] == '\n':      # cadena sin cerrar: corta
                    break
                i += 1
            i += 1
        else:
            i += 1
    return ''.join(out)


def cadenas_de(bloque):
    """Concatena los literales de cadena de un bloque asm(...)."""
    partes, i, n = [], 0, len(bloque)
    while i < n:
        if bloque[i] == '"':
            j, buf = i + 1, []
            while j < n and bloque[j] != '"':
                if bloque[j] == '\\' and j + 1 < n:
                    buf.append({'n': '\n', 't': '\t', '"': '"',
                                '\\': '\\'}.get(bloque[j + 1], bloque[j + 1]))
                    j += 2
                    continue
                buf.append(bloque[j])
                j += 1
            partes.append(''.join(buf))
            i = j + 1
        elif bloque[i] == "'":
            i += 1
            while i < n and bloque[i] != "'":
                i += 2 if bloque[i] == '\\' else 1
            i += 1
        else:
            i += 1
    return ''.join(partes)


def bloque_de(txt, abre):
    """Desde el '(' en `abre`, devuelve (contenido, indice del ')')."""
    d, i, n = 0, abre, len(txt)
    while i < n:
        c = txt[i]
        if c == '"' or c == "'":
            q, i = c, i + 1
            while i < n and txt[i] != q:
                i += 2 if txt[i] == '\\' else 1
            i += 1
            continue
        if c == '(':
            d += 1
        elif c == ')':
            d -= 1
            if d == 0:
                return txt[abre + 1:i], i
        i += 1
    return txt[abre + 1:], n - 1


def tiene_mnemonicos(s):
    """Que mnemonicos PPC reales aparecen como primera palabra de una linea."""
    v = []
    for ln in re.split(r'[\n;]', s):
        t = ln.strip()
        if not t or t.startswith(('.', '#', '/')) or t.endswith(':'):
            continue
        w = t.split()[0].rstrip(',')
        if w in MNEM:
            v.append(w)
    return v


# MIPS/VU0 (PS2), SPARC (STLport): instrucciones que NO son de GameCube. No son
# andamio del puerto GC; son ramas de otra plataforma o de terceros.
RE_OTRA = re.compile(r'^\s*('
                     r'lqc2|sqc2|qmtc2[a-z.]*|qmfc2[a-z.]*|v[a-z]+\b|lq|sq|'
                     r'mfc0|mtc0|sync\.[lp]|ei\b|di\b|break|min\.s|max\.s|abs\.s|'
                     r'casx?\b|swap\b|membar|pex[a-z]+|por\b|lui\b|addiu|daddu|jr\b)',
                     re.M)


def tiene_instrucciones_otra_arq(s):
    m = RE_OTRA.findall(s)
    return ','.join(sorted(set(m))[:6]) if m else ''


# --------------------------------------------------------------- clasificacion
def clasifica(pre, cuerpo, plantilla, cruda=''):
    """(clase, detalle). `pre` = los ~300 chars de fuente ANTES del asm.

    `cruda` es el TEXTO de la plantilla antes de extraerle las cadenas: hace
    falta porque las macros con `#name` (fdlibm.h, libioP.h de la STL) no
    tienen ningun literal y sin esto pasaban por barreras secas.
    """
    ops = cuerpo[len(re.match(r'\s*(?:"[^"]*"\s*)*', cuerpo).group(0)):]
    hay_ops = ops.lstrip().startswith(':')
    t = plantilla.strip()

    if '"' not in cruda and cruda.strip():
        # plantilla construida por el preprocesador: `__asm__(#name)`
        return 'ALIAS', 'macro ' + re.sub(r'\s+', ' ', cruda.strip())[:46]

    # PIN: `register T x asm("rN")`. La palabra `register` tiene que estar en la
    # MISMA declaracion (se corta en el ultimo ; { } o , de separacion).
    if RE_REG.match(t) and re.search(r'(?:^|[^A-Za-z0-9_])register(?![A-Za-z0-9_])',
                                     pre.rsplit(';', 1)[-1].rsplit('{', 1)[-1]
                                     .rsplit('}', 1)[-1]):
        return 'PIN', t
    if RE_REG.match(t) and not hay_ops:
        # `asm("r3")` sin register: el alias de un global a un registro no
        # existe en GCC 2.9; es un pin cuyo `register` esta en otra linea.
        return 'PIN', t

    solo_comentario = t != '' and all(
        (not x.strip()) or x.strip().startswith('#') or x.strip().startswith(';')
        for x in t.split('\n'))
    if t == '' or solo_comentario:
        kind = 'con operandos' if hay_ops else 'seca'
        if solo_comentario:
            kind += ' (comentario)'
        clob = re.findall(r'"([^"]+)"', ops.split(':')[-1]) if ops.count(':') >= 3 else []
        return 'BARRERA', kind + (' clobber=' + ','.join(clob) if clob else '')

    mn = tiene_mnemonicos(t)
    if mn:
        return 'INSTR', ','.join(sorted(set(mn))[:6])
    otra = tiene_instrucciones_otra_arq(t)
    if otra:
        return 'INSTR-OTRA', otra

    if t.startswith('.') or re.match(r'^\s*\.', t):
        d = sorted({m for m in re.findall(r'^\s*(\.[a-z_0-9]+)', t, re.M)})
        return 'DATOS', ','.join(d[:6])

    if not hay_ops and re.match(r'^[A-Za-z_$][A-Za-z0-9_$.]*$', t):
        return 'ALIAS', t

    return 'OTRO', t[:60].replace('\n', ' ')


# ---------------------------------------------------------- contexto del fuente
def guardas(txt, pos):
    """Pila de #if/#ifdef vigente en `pos` (para detectar ramas muertas)."""
    pila = []
    for m in re.finditer(r'^[ \t]*#[ \t]*(ifdef|ifndef|if|elif|else|endif)\b([^\n]*)',
                         txt[:pos], re.M):
        d, arg = m.group(1), m.group(2).strip()
        if d in ('ifdef', 'ifndef', 'if'):
            pila.append(('#%s %s' % (d, arg))[:48])
        elif d == 'elif':
            if pila:
                pila[-1] = ('#elif %s' % arg)[:48]
        elif d == 'else':
            if pila:
                pila[-1] = '#else of ' + pila[-1]
        elif d == 'endif':
            if pila:
                pila.pop()
    return pila


def funcion_de(lineas, i):
    """Firma de la funcion que contiene la linea i (0-based), o '' (ambito de fichero)."""
    llaves = 0
    for k in range(i, -1, -1):
        t = lineas[k]
        llaves += t.count('}') - t.count('{')
        s = t.rstrip()
        if not s or s[0] in ' \t#':
            continue
        if s.lstrip().startswith(('*', '/')):
            continue
        if RE_DEF.match(s) and ('{' in s or (k + 1 < len(lineas))):
            if s.rstrip().endswith(';'):
                continue
            if re.match(r'^\s*(?:if|for|while|switch|return|else|case)\b', s):
                continue
            return s.strip()[:110]
    return ''


# -------------------------------------------------------------------- unidades
def unidades():
    """{ruta de fuente sin extension -> [unidades de enlace]}.

    build.ninja NO sirve: las SourceLists compilan UN .cpp que hace #include de
    los demas. Y una cabecera (UTLVector.h, ENVIRO_AEMS.h) la consumen VARIAS
    unidades a la vez: por eso el valor es una lista, no un nombre.
    """
    out = {}
    def anade(k, u):
        out.setdefault(k, [])
        if u not in out[k]:
            out[k].append(u)
    sl = os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists')
    if os.path.isdir(sl):
        for f in sorted(os.listdir(sl)):
            if not f.endswith('.cpp'):
                continue
            u = 'Speed/Indep/SourceLists/' + f[:-4]
            txt = open(os.path.join(sl, f), encoding='utf-8', errors='replace').read()
            for m in RE_INC.finditer(txt):
                anade(os.path.splitext(m.group(1))[0], u)
    p = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')
    if os.path.exists(p):
        for u in json.load(open(p, encoding='utf-8'))['units']:
            sp = (u.get('metadata') or {}).get('source_path') or ''
            n = u['name'][5:] if u['name'].startswith('main/') else u['name']
            if sp.startswith('src/'):
                anade(os.path.splitext(sp[4:])[0], n)
    asmd = os.path.join(ROOT, 'build', 'GOWE69', 'asm')
    for dp, _, fs in os.walk(asmd):
        for f in fs:
            if not f.endswith('.s'):
                continue
            rel = os.path.relpath(os.path.join(dp, f), asmd).replace(os.sep, '/')[:-2]
            anade(rel, rel)
    return out


def informe():
    """{unidad -> (pct_unidad, {demangled/mangled -> (pct, size)})}."""
    p = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')
    if not os.path.exists(p):
        return {}
    d = json.load(open(p, encoding='utf-8'))
    out = {}
    for u in d['units']:
        n = u['name']
        n = n[5:] if n.startswith('main/') else n
        fn = {}
        for f in (u.get('functions') or []):
            pct = f.get('fuzzy_match_percent', 0.0)
            sz = int(f.get('size', '0') or 0)
            fn[f['name']] = (pct, sz, f['name'])
            dm = (f.get('metadata') or {}).get('demangled_name')
            if dm:
                fn.setdefault('D:' + dm, (pct, sz, f['name']))
        out[n] = (u['measures'].get('fuzzy_match_percent', 0.0), fn)
    return out


comentadas = [0, 0]     # [asm(...) en comentario, pines en comentario]


def escanea(raiz):
    """Lista de apariciones crudas."""
    filas = []
    for dp, dirs, fs in os.walk(raiz):
        dirs[:] = [d for d in dirs if d not in ('.git', '__pycache__')]
        for f in sorted(fs):
            if not f.endswith(EXTS):
                continue
            fp = os.path.join(dp, f)
            try:
                txt = open(fp, encoding='utf-8', errors='replace').read()
            except IOError:
                continue
            if 'asm' not in txt:
                continue
            limpio = despoja(txt)
            lineas = limpio.split('\n')
            comentadas[0] += len(RE_ASM.findall(txt)) - len(RE_ASM.findall(limpio))
            rp = re.compile(r'register[^;\n]*?asm\s*\(\s*"(?:f?r|f)[0-9]+"')
            comentadas[1] += len(rp.findall(txt)) - len(rp.findall(limpio))
            rel = os.path.relpath(fp, ROOT).replace(os.sep, '/')
            for m in RE_ASM.finditer(limpio):
                abre = m.end() - 1
                cuerpo, cierra = bloque_de(limpio, abre)
                # la plantilla es lo que va antes del primer ':' de nivel 0
                d, k = 0, 0
                while k < len(cuerpo):
                    c = cuerpo[k]
                    if c in '"\'':
                        q, k = c, k + 1
                        while k < len(cuerpo) and cuerpo[k] != q:
                            k += 2 if cuerpo[k] == '\\' else 1
                    elif c == '(':
                        d += 1
                    elif c == ')':
                        d -= 1
                    elif c == ':' and d == 0:
                        break
                    k += 1
                cruda = cuerpo[:k]
                pl = cadenas_de(cruda)
                ini = limpio.count('\n', 0, m.start()) + 1
                fin = limpio.count('\n', 0, cierra) + 1
                pre = limpio[max(0, m.start() - 300):m.start()]
                clase, det = clasifica(pre, cuerpo, pl, cruda)
                filas.append({
                    'fichero': rel, 'linea': ini, 'lineas': fin - ini + 1,
                    'clase': clase, 'detalle': det,
                    'texto': re.sub(r'\s+', ' ', txt.split('\n')[ini - 1].strip())[:150],
                    'funcion': funcion_de(lineas, ini - 1),
                    'guardas': guardas(limpio, m.start()),
                })
    return filas


def main():
    filas = escanea(os.path.join(ROOT, 'src'))
    for extra in ('include', 'tools/scratch'):
        d = os.path.join(ROOT, extra)
        if os.path.isdir(d) and '--todo' in sys.argv:
            filas += escanea(d)

    umap = unidades()
    rep = informe()
    for r in filas:
        rel = r['fichero'][4:] if r['fichero'].startswith('src/') else r['fichero']
        us = umap.get(os.path.splitext(rel)[0], [])
        r['unidades'] = us
        u = us[0] if us else '?'
        r['unidad'] = u
        pu, fns = rep.get(u, (None, {}))
        for v in us[1:]:                       # cabecera compartida: la peor manda
            p2, f2 = rep.get(v, (None, {}))
            if p2 is not None and (pu is None or p2 < pu):
                pu = p2
            for k, val in f2.items():
                fns.setdefault(k, val)
        r['pct_unidad'] = pu
        r['pct_fn'] = None
        r['tam_fn'] = None
        r['fn_sym'] = None
        r['fn_mangled'] = None
        nom = r['funcion']
        mm = re.search(r'([A-Za-z_][A-Za-z0-9_]*(?:::\s*~?[A-Za-z_][A-Za-z0-9_]*)?)\s*\(', nom)
        if mm and fns:
            base = re.sub(r'\s+', '', mm.group(1))
            cands = []
            for k, (p, s, mg) in fns.items():
                if k.startswith('D:'):
                    dn = k[2:]
                    d0 = dn.split('(')[0].strip()
                    if d0 == base or d0.endswith('::' + base) or d0.split('::')[-1] == base:
                        cands.append((p, s, dn, mg))
                elif '::' not in base and (k.startswith(base + '__') or k == base):
                    cands.append((p, s, k, mg))
            if cands:
                cands.sort()
                (r['pct_fn'], r['tam_fn'], r['fn_sym'], r['fn_mangled']) = cands[0]

    if '--json' in sys.argv:
        dest = sys.argv[sys.argv.index('--json') + 1]
        json.dump(filas, open(dest, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        print('%d apariciones -> %s' % (len(filas), dest))

    sel = None
    if '--clase' in sys.argv:
        sel = sys.argv[sys.argv.index('--clase') + 1].upper()

    orden = ['PIN', 'BARRERA', 'INSTR', 'INSTR-OTRA', 'DATOS', 'ALIAS', 'OTRO']
    tot = {c: 0 for c in orden}
    for r in filas:
        tot[r['clase']] = tot.get(r['clase'], 0) + 1
    print('== TOTALES (comentarios YA quitados) ==')
    print('  (en comentarios, NO contadas: %d menciones de asm(...) '
          'y %d de register X asm("rN"))' % (comentadas[0], comentadas[1]))
    for c in orden:
        nf = len({r['fichero'] for r in filas if r['clase'] == c})
        print('  %-8s %5d apariciones en %3d ficheros' % (c, tot[c], nf))
    print('  %-8s %5d en %d ficheros' % ('TOTAL', len(filas),
                                         len({r['fichero'] for r in filas})))
    andamio = [r for r in filas if r['clase'] in ('PIN', 'BARRERA')]
    print('  ANDAMIO REAL (PIN+BARRERA): %d en %d ficheros' %
          (len(andamio), len({r['fichero'] for r in andamio})))
    print()

    print('== POR FICHERO ==')
    porf = {}
    for r in filas:
        porf.setdefault(r['fichero'], []).append(r)
    def clave(it):
        rs = it[1]
        return (-sum(1 for r in rs if r['clase'] in ('PIN', 'BARRERA')), it[0])
    for f, rs in sorted(porf.items(), key=clave):
        cs = {}
        for r in rs:
            cs[r['clase']] = cs.get(r['clase'], 0) + 1
        if sel and sel not in cs:
            continue
        u = rs[0]['unidad']
        pu = rs[0]['pct_unidad']
        print('%-78s %-34s %s  %s' % (
            f, u, ('%6.2f%%' % pu) if pu is not None else '   ?  ',
            ' '.join('%s=%d' % (k, v) for k, v in sorted(cs.items()))))
        if sel:
            for r in rs:
                if r['clase'] != sel:
                    continue
                print('     L%-6d %-9s %-14s fn=%s %s' % (
                    r['linea'], r['clase'], r['detalle'][:14],
                    (r['fn_sym'] or r['funcion'] or '<fichero>')[:58],
                    ('%.2f%%' % r['pct_fn']) if r['pct_fn'] is not None else '?'))
                if r['guardas']:
                    print('              guardas: %s' % ' | '.join(r['guardas']))


if __name__ == '__main__':
    main()
