#!/usr/bin/env python3
"""Buscador automatico de la forma de fuente que casa (un *permuter*).

Idea: en vez de que un humano lea el diff, escriba una variante, compile y
repita, se generan **automaticamente** variantes semanticamente equivalentes de
la fuente de UNA funcion, se compilan a ensamblador, se puntuan contra el
desensamblado del original y se conserva la mejor (ascenso de colina).

  # el diagnostico de regmap.py y el plan que sale de el, sin compilar nada
  python scripts/permuter.py zAI "AICopManager::UpdatePursuits" --diag

  # BUSQUEDA GUIADA: regmap elige las transformaciones y las locales objetivo
  python scripts/permuter.py zCamera "TrackCarCameraMover::Init" --guided -j 12

  # recocido: 2 o 3 cambios simultaneos y pasos laterales
  python scripts/permuter.py zSim "OBB::SphereVsBox" --guided --depth 2       --combos 250 --anneal 0.02

  # toda la unidad, un proceso por funcion (sigue aunque una reviente)
  python scripts/permuter.py zTrack --sweep --lo 99 --hi 100 -j 10

  # ver que catalogo hay para una funcion, sin compilar
  python scripts/permuter.py zSim "OBB::SphereVsBox" --guided --dry

  # probar UN cuerpo escrito a mano (o seguir desde el mejor de otra tanda)
  python scripts/permuter.py zFe "PauseMenu::NotificationMessage"       --seed cuerpo.cpp --rounds 0

  # a que apuntar: los near-miss de una unidad, los mas cerca primero
  python scripts/permuter.py zSim --near

La busqueda guiada
------------------
El ascenso de colina ciego se para en la primera ronda: desde el 99,7% casi
todo empeora. Pero **el diagnostico existe**: `regmap.py` enfrenta local a
local el registro que el compilador de EA asigno contra el nuestro, alinea los
bloques anonimos por solapamiento de rangos y nombra la causa. Con `--guided`:

  * el VEREDICTO elige el plan de transformaciones (CAUSE_PLAN),
  * las LOCALES que regmap nombra (la que sobra, la que falta, la que cambia
    de registro) ordenan el catalogo: esas variantes van primero,
  * la PRIMERA DIVERGENCIA ordena el resto: lo que hay antes ya casa
    instruccion a instruccion, asi que mutar ahi rinde menos.

Si el plan sale con menos de `--floor` variantes se abre el catalogo entero:
un plan de una sola variante mata la busqueda antes de empezar.

Las transformaciones (ver TRANSFORMS) salen del catalogo destilado por el
proyecto. Ademas de las de forma (rotar stores, invertir un if, partir un &&,
orden de los case, while contra for, const, operandos conmutativos, temporal
anonimo contra nombrado, copiar el bloque de un goto...), estan las que ataca
el triaje de regmap, que dice que el 60% es ESTRUCTURA y solo el 4% techo del
asignador:

  for_counter   sacar el contador del `for` a nivel de funcion (y unificarlo:
                el original de EA declara UNO y lo reutiliza en todos)
  hoist_decl    subir una declaracion un nivel lexico
  sink_decl     bajarla al punto donde se usa  -> ACORTA el rango de vida
  split_decl    `T x = e;` <-> `T x; ... x = e;` -> ALARGA / acorta el rango
  init_decl     `T x;` -> `T x = false/0/nullptr;`: el inicializador muerto
                NO emite instruccion pero cambia el reparto de registros (la
                local nace viva en la declaracion). Cerro EnforceMinGapToWalls
  add_local     atar una subexpresion repetida a una local nueva
  ghost_local   escribir la local que el DWARF declara aunque nadie la lea:
                un contador muerto no es inocuo, alarga el rango de vida de
                la condicion y empuja un miembro a la pila
  split_arg     sacar a una local el argumento que es una LLAMADA (sin abrir
                bloque: hoist_temp abria uno y cambiaba el arbol de ambitos)
  move_stmt     mover una sentencia varias posiciones, no solo a la de al
                lado: en el grupo IDENTICO lo que baila son los registros de
                direcciones y constantes, y el mecanismo es que el original
                ENTRELAZA las sentencias que tocan un mismo global
  block_wrap    envolver una tirada en un bloque anonimo (o quitarlo,
                block_unwrap) cuando el arbol de bloques no cuadra

Y las de la LISTA DE INICIALIZACION de un constructor (INIT_TRANSFORMS), que
no vive dentro del `{...}`: `Sim::Entity::Entity` no ofrecia NINGUNA variante
porque sus 231 instrucciones salen integras de ella.

  init_swap / init_rot   GCC 2.9 emite en el orden ESCRITO, no en el de
                         declaracion de los miembros
  init2body / body2init  no es lo mismo: la lista usa el copy-ctor implicito y
                         emite copia de bloque; el cuerpo, si el tipo declara
                         operator=, emite la copia miembro a miembro

Cada una se aplica en todas las posiciones posibles del cuerpo; el compilador
hace de validador (lo que no compila se descarta, y lo que compila y da el asm
del original es correcto por construccion).

Como compila
------------
Las unidades son SourceLists que hacen #include de decenas de .cpp. Se genera
un **prefijo**: las lineas del SourceList hasta el #include del fichero que
contiene la funcion, con ese include sustituido por una copia mutada que vive
en el scratchpad. Se anade `-I <dir del .cpp original>` AL FINAL de la lista de
-I, para que sus includes entre comillas ("QuickGame.h", "../OBB.h") sigan
resolviendo igual; al ir al final solo actua de ultimo recurso y no puede
cambiar ninguna resolucion que ya funcionaba. **No se toca src/ en ningun
momento.**

Como puntua
-----------
El comparador de texto miente: `$LCnnn` contra `lbl_XXXXXXXX` y las `.L`
normalizadas ocultan destinos de salto distintos. Aqui **cada salto se resuelve
a un desplazamiento relativo en instrucciones** antes de comparar, en los dos
lados (en el original la etiqueta es `.L_<direccion>`, asi que el indice sale
de la direccion). Las referencias a rodata anonima si se colapsan a un token
comun, pero se cuentan y se avisan.

  score = instrucciones alineadas / max(mias, originales)   -> 1.0 = identico

La verificacion final SIEMPRE es objdiff:
  python scripts/build_direct.py <unidad>
  ./objdiff-cli-windows-x86_64.exe report generate -o build/GOWE69/report.json -f json
"""
import argparse
import difflib
import hashlib
import os
import re
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor

import builtins
import functools

# las busquedas duran minutos: sin esto no se ve nada hasta que terminan
print = functools.partial(builtins.print, flush=True)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))

# un directorio por proceso: varias busquedas a la vez (y el --sweep, que lanza
# un proceso por funcion) se pisaban el p_w00.s y morian con WinError 32.
SCRATCH = os.environ.get('PERMUTER_TMP') or os.path.join(
    os.environ.get('TEMP', os.path.join(ROOT, 'build')), 'permuter',
    'p%d' % os.getpid())
ASM_ROOT = os.path.join(ROOT, 'build', 'GOWE69', 'asm', 'Speed', 'Indep', 'SourceLists')


# --------------------------------------------------------------------------
# 1. ENSAMBLADOR: extraccion y normalizacion
# --------------------------------------------------------------------------

RE_ORIG_INSN = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\s*(.*?)\s*$')
RE_ORIG_LBL = re.compile(r'^\.L_([0-9A-F]{8}):')

CRBIT = {'lt': 0, 'gt': 1, 'eq': 2, 'so': 3, 'un': 3}

BC_NAME = {(12, 0): 'blt', (12, 1): 'bgt', (12, 2): 'beq', (12, 3): 'bso',
           (4, 0): 'bge', (4, 1): 'ble', (4, 2): 'bne', (4, 3): 'bns'}


def orig_function(unit, sym):
    """-> lista de instrucciones normalizadas del original, saltos resueltos."""
    # Las unidades de middleware NO son SourceLists: su .s cuelga de la ruta
    # completa (egami/..., Speed/Indep/Libs/...). Sin esto el permutador solo
    # veia las SourceLists.
    path = os.path.join(ASM_ROOT, unit + '.s')
    if not os.path.exists(path):
        cands = [os.path.join(ROOT, 'build', 'GOWE69', 'asm',
                              *(unit + '.s').split('/'))]
        try:
            import build_direct
            u = build_direct.parse_units().get(unit)
            if u:
                out = u[1].replace(chr(92), '/')
                cands.append(os.path.join(
                    ROOT, *out.replace('build/GOWE69/src/',
                                       'build/GOWE69/asm/')[:-2].split('/')) + '.s')
        except Exception:
            pass
        for alt in cands:
            if os.path.exists(alt):
                path = alt
                break
    insns, labels = [], {}
    on = False
    for line in open(path, encoding='utf-8', errors='replace'):
        if line.startswith('.fn '):
            on = line[4:].split(',')[0].strip() == sym
            continue
        if line.startswith('.endfn'):
            if on:
                break
            continue
        if not on:
            continue
        m = RE_ORIG_LBL.match(line)
        if m:
            labels[int(m.group(1), 16)] = len(insns)
            continue
        m = RE_ORIG_INSN.match(line)
        if m:
            insns.append((int(m.group(1), 16), m.group(2)))
    if not insns:
        return None
    base = insns[0][0]
    for a, _ in insns:
        labels.setdefault(a, (a - base) // 4)
    out = []
    for i, (addr, text) in enumerate(insns):
        out.append(normalize(text, i, lambda l: labels.get(_lblval(l))))
    return out


def _lblval(lbl):
    m = re.match(r'^\.L_([0-9A-Fa-f]{8})$', lbl)
    return int(m.group(1), 16) if m else None


def mine_function(asm_path, sym):
    """-> lista de instrucciones normalizadas del .s que hemos generado."""
    try:
        lines = open(asm_path, encoding='utf-8', errors='replace').read().split('\n')
    except OSError:
        return None
    start = None
    for i, l in enumerate(lines):
        if l.rstrip() == sym + ':':
            start = i + 1
            break
    if start is None:
        return None
    raw, labels = [], {}
    for l in lines[start:]:
        s = l.strip()
        if not s:
            continue
        if re.match(r'^\.Lfe\d+:', s):
            break
        if re.match(r'^[A-Za-z_$][\w$.]*:$', s):     # empieza otra funcion
            break
        if s.endswith(':'):
            labels[s[:-1]] = len(raw)
            continue
        if s.startswith('#'):
            continue
        if s.startswith('.'):
            if re.match(r'^\.(size|type|globl|section|align|previous|ident)\b', s):
                if raw:
                    break
            continue
        raw.append(s)
    if not raw:
        return None
    return [normalize(t, i, labels.get) for i, t in enumerate(raw)]


def normalize(text, idx, label_index):
    """Normaliza una instruccion; resuelve el destino de salto a offset relativo.

    label_index(nombre_de_etiqueta) -> indice de instruccion, o None.
    """
    t = text.split('#')[0].strip()
    t = t.replace(',', ', ')
    t = re.sub(r'\s+', ' ', t).strip()
    # bits de CR simbolicos (solo el original los usa): cr7un -> 31, eq -> 2
    if t.startswith('cr'):
        if t == 'crclr cr1eq':
            t = 'crxor 6, 6, 6'
        t = re.sub(r'\b(?:cr(\d))?(lt|gt|eq|so|un)\b',
                   lambda m: str(4 * int(m.group(1) or 0) + CRBIT[m.group(2)]), t)
    # bc BO,BI,destino -> nombre simbolico (el original ya viene simbolico)
    m = re.match(r'^bc (\d+), (\d+), (\S+)$', t)
    if m:
        bo, bi = int(m.group(1)), int(m.group(2))
        cr, bit = bi // 4, bi % 4
        name = BC_NAME.get((bo, bit))
        if name is None:
            t = 'bc%d_%d %s' % (bo, bi, m.group(3))
        elif cr == 0:
            t = '%s %s' % (name, m.group(3))
        else:
            t = '%s cr%d, %s' % (name, cr, m.group(3))
    # resolver saltos a offset relativo
    m = re.match(r'^(b|ba|bl|blt|bgt|beq|bso|bge|ble|bne|bns|bdnz|bdz|bc\d+_\d+)'
                 r'((?: cr\d,)?) (\S+)$', t)
    if m and (m.group(3).startswith('.L') or m.group(3).startswith('L_')):
        tgt = label_index(m.group(3))
        dest = ('%+d' % (tgt - idx)) if tgt is not None else '?'
        t = '%s%s %s' % (m.group(1), m.group(2), dest)
    # hexadecimales a decimal
    t = re.sub(r'-0x([0-9A-Fa-f]+)', lambda m: str(-int(m.group(1), 16)), t)
    t = re.sub(r'0x([0-9A-Fa-f]+)', lambda m: str(int(m.group(1), 16)), t)
    # registros: quitar el prefijo en los dos lados
    t = re.sub(r'\bqr(\d+)\b', r'\1', t)
    t = re.sub(r'\bcr(\d+)\b', r'C\1', t)
    t = re.sub(r'\br(\d+)\b', r'\1', t)
    t = re.sub(r'\bf(\d+)\b', r'\1', t)
    t = re.sub(r'@ha\b', '@h', t)
    # subic./subi/subis == addic./addi/addis con la constante negada
    m = re.match(r'^sub(i|ic|ic\.|is)\s+(\d+), (\d+), (-?\d+)$', t)
    if m:
        t = 'add%s %s, %s, %d' % (m.group(1), m.group(2), m.group(3),
                                  -int(m.group(4)))
    # campo de CR explicito en las comparaciones: 7 -> C7
    m = re.match(r'^(cmpw|cmpwi|cmplw|cmplwi|fcmpu|fcmpo) (\d), (\S+, \S+)$', t)
    if m:
        t = '%s C%s, %s' % (m.group(1), m.group(2), m.group(3))
    # slwi/srwi/clrlwi/rotlwi son rlwinm: los dos lados usan las dos formas y
    # sin esto un `slwi 26, 26, 28` cuenta como diferencia de un `rlwinm`.
    m = re.match(r'^(slwi|srwi|clrlwi|clrrwi|rotlwi) (\d+), (\d+), (\d+)$', t)
    if m:
        op, rd, ra, n = m.group(1), m.group(2), m.group(3), int(m.group(4))
        if op == 'slwi':
            sh, mask = n, (0xFFFFFFFF << n) & 0xFFFFFFFF
        elif op == 'srwi':
            sh, mask = (32 - n) % 32, 0xFFFFFFFF >> n
        elif op == 'clrlwi':
            sh, mask = 0, 0xFFFFFFFF >> n
        elif op == 'clrrwi':
            sh, mask = 0, (0xFFFFFFFF << n) & 0xFFFFFFFF
        else:
            sh, mask = n, 0xFFFFFFFF
        t = 'rlwinm %s, %s, %d, %d' % (rd, ra, sh, mask)
    m = re.match(r'^rlwinm(\.?) (\d+), (\d+), (\d+), (\d+), (\d+)$', t)
    if m:
        mb, me = int(m.group(5)), int(m.group(6))
        mask = 0
        for bit in range(32):
            if (mb <= me and mb <= bit <= me) or (mb > me and (bit >= mb or bit <= me)):
                mask |= 1 << (31 - bit)
        t = 'rlwinm%s %s, %s, %s, %d' % (m.group(1), m.group(2), m.group(3),
                                         m.group(4), mask)
    m = re.match(r'^rlwinm(\.?) (\d+), (\d+), (\d+), (-\d+)$', t)
    if m:
        t = 'rlwinm%s %s, %s, %s, %d' % (m.group(1), m.group(2), m.group(3),
                                         m.group(4), int(m.group(5)) & 0xFFFFFFFF)
    # la rd,X(rb) == addi rd,rb,X
    m = re.match(r'^la (\d+), (\S+)\((\d+)\)$', t)
    if m:
        t = 'addi %s, %s, %s' % (m.group(1), m.group(3), m.group(2))
    # rodata anonima: $LC132 y lbl_80403FC0 son lo mismo (ojo: colapsa el valor)
    t = re.sub(r'\$LC\d+', 'LC', t)
    t = re.sub(r'\blbl_[0-9A-F]{8}\b', 'LC', t)
    # sufijos de estaticas locales
    t = re.sub(r'\.\d+\b', '', t)
    t = re.sub(r'^(cmpw|cmpwi|cmplw|cmplwi|fcmpu|fcmpo) C?0, (\S+, \S+)$', r'\1 \2', t)
    return t


def score(mine, orig):
    """-> (fraccion alineada, fraccion de opcodes, primera divergencia)."""
    if not mine:
        return (0.0, 0.0, 0)
    sm = difflib.SequenceMatcher(None, mine, orig, autojunk=False)
    matched = sum(b.size for b in sm.get_matching_blocks())
    n = max(len(mine), len(orig))
    ops_a = [x.split(' ')[0] for x in mine]
    ops_b = [x.split(' ')[0] for x in orig]
    sm2 = difflib.SequenceMatcher(None, ops_a, ops_b, autojunk=False)
    matched2 = sum(b.size for b in sm2.get_matching_blocks())
    first = n
    for i, (a, b) in enumerate(zip(mine, orig)):
        if a != b:
            first = i
            break
    return (matched / n, matched2 / n, first)


def rank(mine, orig):
    """Clave de ordenacion: mas alto es mejor."""
    if mine is None:
        return (-1.0, -1.0, -1.0, 0, 0)
    f, o, first = score(mine, orig)
    # la suma guia mejor que la fraccion exacta sola: una variante que arregla
    # la secuencia de opcodes pero baraja registros va por buen camino.
    return (f + o, f, o, first, -abs(len(mine) - len(orig)))


# --------------------------------------------------------------------------
# 2. HARNESS DE COMPILACION
# --------------------------------------------------------------------------

class Harness(object):
    def __init__(self, unit, target_cpp, quiet=True):
        import build_direct
        units = build_direct.parse_units()
        if unit not in units:
            sys.exit('unidad desconocida: %s' % unit)
        src, out, cflags, tc, rn = units[unit]
        self.unit = unit
        self.cc = os.path.join(ROOT, 'build', 'compilers', tc, 'ngccc.exe')
        self.env = dict(os.environ)
        self.env['SN_NGC_PATH'] = os.path.join(ROOT, 'build', 'compilers', tc)
        self.cflags = [c for c in cflags if c != '-gdwarf+']
        self.cflags += ['-I', os.path.dirname(target_cpp).replace('\\', '/')]
        self.target_cpp = target_cpp.replace('\\', '/')
        rel = self.target_cpp
        if rel.startswith(ROOT.replace('\\', '/') + '/'):
            rel = rel[len(ROOT) + 1:]
        key = rel.split('/')[-1]
        # prefijo del SourceList hasta el include del fichero objetivo
        lines = open(os.path.join(ROOT, src), encoding='utf-8',
                     errors='replace').read().split('\n')
        self.prefix, hit = [], False
        for l in lines:
            if l.startswith('#include') and key in l:
                self.prefix.append('@@TARGET@@')
                hit = True
                break
            self.prefix.append(l)
        if not hit:
            # unidad directa: el .cpp de la unidad ES el fichero objetivo, no
            # hay SourceList que lo incluya.
            if os.path.normcase(os.path.abspath(os.path.join(ROOT, src))) ==                os.path.normcase(os.path.abspath(self.target_cpp)):
                self.prefix = ['@@TARGET@@']
            else:
                sys.exit('%s no aparece en %s' % (key, src))
        self.nincludes = len(self.prefix)
        os.makedirs(SCRATCH, exist_ok=True)
        self.quiet = quiet

    def build(self, source_text, tag):
        """Compila `source_text` como el .cpp objetivo. -> (ok, ruta .s, err)."""
        cpp = os.path.join(SCRATCH, 'p_%s_body.cpp' % tag)
        wrap = os.path.join(SCRATCH, 'p_%s_wrap.cpp' % tag)
        asm = os.path.join(SCRATCH, 'p_%s.s' % tag)
        with open(cpp, 'w', encoding='utf-8', errors='replace') as fh:
            fh.write(source_text)
        body = [('#include "%s"' % cpp.replace('\\', '/')) if l == '@@TARGET@@' else l
                for l in self.prefix]
        # El prefijo corta en el #include del objetivo: si queda algun #if/#ifdef
        # abierto (p.ej. zPlatform.cpp empieza con #ifdef EA_PLATFORM_GAMECUBE),
        # hay que cerrarlo o el wrap no compila.
        open_if = 0
        for l in body:
            s = l.lstrip()
            if s.startswith(('#if', '#ifdef', '#ifndef')):
                open_if += 1
            elif s.startswith('#endif'):
                open_if -= 1
        if open_if > 0:
            body = body + ['#endif'] * open_if
        with open(wrap, 'w', encoding='utf-8', errors='replace') as fh:
            fh.write('\n'.join(body) + '\n')
        if os.path.exists(asm):
            try:
                os.remove(asm)
            except OSError:
                pass
        cmd = [self.cc] + self.cflags + ['-S', '-o', asm, wrap]
        p = subprocess.run(cmd, cwd=ROOT, env=self.env, capture_output=True, text=True)
        err = (p.stdout or '') + (p.stderr or '')
        return p.returncode == 0, asm, err


# --------------------------------------------------------------------------
# 3. TROCEADO DE C++ (consciente de llaves, parentesis, cadenas y comentarios)
# --------------------------------------------------------------------------

def scan(text):
    """-> lista con el nivel de anidamiento (llaves, parentesis) y si es codigo."""
    n = len(text)
    depth_b = [0] * n
    depth_p = [0] * n
    code = [True] * n
    i, db, dp = 0, 0, 0
    while i < n:
        c = text[i]
        if c == '/' and i + 1 < n and text[i + 1] == '/':
            j = text.find('\n', i)
            j = n if j < 0 else j
            for k in range(i, j):
                code[k] = False
                depth_b[k], depth_p[k] = db, dp
            i = j
            continue
        if c == '/' and i + 1 < n and text[i + 1] == '*':
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                code[k] = False
                depth_b[k], depth_p[k] = db, dp
            i = j
            continue
        if c in '"\'':
            q = c
            j = i + 1
            while j < n:
                if text[j] == '\\':
                    j += 2
                    continue
                if text[j] == q:
                    j += 1
                    break
                j += 1
            for k in range(i, min(j, n)):
                code[k] = False
                depth_b[k], depth_p[k] = db, dp
            i = j
            continue
        if c == '#' and (i == 0 or text[i - 1] == '\n'):
            j = text.find('\n', i)
            j = n if j < 0 else j
            for k in range(i, j):
                code[k] = False
                depth_b[k], depth_p[k] = db, dp
            i = j
            continue
        if c == '{':
            db += 1
        elif c == '(' or c == '[':
            dp += 1
        depth_b[i], depth_p[i] = db, dp
        if c == '}':
            db -= 1
            depth_b[i] = db
        elif c == ')' or c == ']':
            dp -= 1
            depth_p[i] = dp
        i += 1
    return depth_b, depth_p, code


KEYWORDS = ('if', 'for', 'while', 'switch', 'do', 'else', 'case', 'default', 'return')


def split_block(text, lo, hi):
    """Trocea el contenido de un bloque [lo,hi) en sentencias de primer nivel.

    -> [(inicio, fin)] con fin exclusivo, incluyendo los cuerpos compuestos.
    """
    db, dp, code = scan(text)
    base_b = db[lo] if lo < len(db) else 0
    out = []
    i = lo
    start = None
    while i < hi:
        c = text[i]
        if not code[i] or c.isspace():
            i += 1
            continue
        if start is None:
            start = i
        if c == ';' and db[i] == base_b and dp[i] == 0:
            out.append((start, i + 1))
            start = None
            i += 1
            continue
        if c == '{' and dp[i] == 0:
            # saltar hasta la llave que lo cierra
            j = i + 1
            while j < hi and not (code[j] and text[j] == '}' and db[j] == db[i] - 1):
                j += 1
            j += 1
            # un else / while(do) puede continuar la sentencia
            k = j
            while k < hi and (text[k].isspace() or not code[k]):
                k += 1
            if text[k:k + 4] == 'else' and not (text[k + 4:k + 5] or ' ').isalnum():
                i = k + 4
                continue
            if text[k:k + 5] == 'while':
                m = re.compile(r'while\s*\([^;]*\)\s*;').match(text, k)
                if m:
                    j = m.end()
            if text[j:j + 1] == ';':
                j += 1
            out.append((start, j))
            start = None
            i = j
            continue
        i += 1
    if start is not None:
        s = text[start:hi].strip()
        if s:
            out.append((start, hi))
    return out


def all_blocks(text, lo=0, hi=None):
    """-> [(inicio_contenido, fin_contenido)] de todos los `{...}` de `text`."""
    hi = len(text) if hi is None else hi
    db, dp, code = scan(text)
    stack, out = [], []
    for i in range(lo, hi):
        if not code[i]:
            continue
        if text[i] == '{':
            stack.append(i)
        elif text[i] == '}' and stack:
            s = stack.pop()
            out.append((s + 1, i))
    out.sort()
    return out


def stmt_kind(s):
    s = s.strip()
    w = re.match(r'^(\w+)', s)
    w = w.group(1) if w else ''
    if w in ('if', 'for', 'while', 'switch', 'do', 'else', 'case', 'default'):
        return w
    if w == 'return':
        return 'return'
    if '{' in s or '}' in s:
        return 'compound'
    return 'simple'


DECL_RE = re.compile(
    r'^(?:const\s+|static\s+|volatile\s+|register\s+|unsigned\s+|signed\s+)*'
    r'(?:struct\s+|class\s+|enum\s+)?'
    r'[A-Za-z_][\w:]*(?:\s*<[^;]*>)?(?:\s*(?:\*|&))*\s+'
    r'(\*?\s*[A-Za-z_]\w*)\s*(=|\(|\[|;)')


def is_decl(s):
    s = s.strip()
    if stmt_kind(s) != 'simple':
        return None
    m = DECL_RE.match(s)
    if not m:
        return None
    name = m.group(1).replace('*', '').strip()
    if name in ('return', 'delete', 'new', 'else'):
        return None
    return name


# --------------------------------------------------------------------------
# 4. TRANSFORMACIONES
# --------------------------------------------------------------------------

def _splice(text, spans, pieces):
    """Sustituye varias regiones disjuntas (ordenadas) de una vez."""
    out, prev = [], 0
    for (a, b), p in zip(spans, pieces):
        out.append(text[prev:a])
        out.append(p)
        prev = b
    out.append(text[prev:])
    return ''.join(out)


def t_swap_adjacent(body):
    """Intercambia dos sentencias contiguas (declaraciones incluidas)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for i in range(len(st) - 1):
            a, b = st[i], st[i + 1]
            ka, kb = stmt_kind(body[a[0]:a[1]]), stmt_kind(body[b[0]:b[1]])
            if ka in ('case', 'default', 'else') or kb in ('case', 'default', 'else'):
                continue
            if 'return' in (ka, kb):
                continue
            ta, tb = body[a[0]:a[1]], body[b[0]:b[1]]
            na, nb = is_decl(ta), is_decl(tb)
            if na and re.search(r'\b%s\b' % re.escape(na), tb):
                continue
            if nb and re.search(r'\b%s\b' % re.escape(nb), ta):
                continue
            new = _splice(body, [a, b], [tb, ta])
            out.append(('swap@%d' % a[0], new))
    return out


def t_rotate_run(body):
    """Rota una tirada de sentencias simples 1 o 2 posiciones (el planificador
    rota exactamente 2, a veces 1): [S(n-1), S(n), S(1)..S(n-2)]."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        runs, cur = [], []
        for s in st:
            if stmt_kind(body[s[0]:s[1]]) == 'simple':
                cur.append(s)
            else:
                if len(cur) >= 3:
                    runs.append(cur)
                cur = []
        if len(cur) >= 3:
            runs.append(cur)
        for run in runs:
            texts = [body[a:b] for a, b in run]
            # una tirada con declaraciones usadas por sus vecinas no se puede
            # rotar: solo gasta compilados (era el 14% del presupuesto)
            decls = [is_decl(x) for x in texts]
            if any(d and any(re.search(r'\b%s\b' % re.escape(d), t)
                             for j, t in enumerate(texts) if j != i)
                   for i, d in enumerate(decls)):
                continue
            for k in (1, 2, -1, -2):
                rot = texts[-k:] + texts[:-k] if k > 0 else texts[-k:] + texts[:-k]
                rot = texts[len(texts) - (k % len(texts)):] + \
                    texts[:len(texts) - (k % len(texts))]
                if rot == texts:
                    continue
                out.append(('rot%+d@%d' % (k, run[0][0]),
                            _splice(body, run, rot)))
    return out


def t_inline_temp(body):
    """`T x = expr; ... f(x)` -> `f(expr)` cuando x se usa una sola vez."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for i, (a, b) in enumerate(st):
            s = body[a:b]
            name = is_decl(s)
            if not name:
                continue
            m = re.match(r'^[^=;]*=\s*(.*);\s*$', s.strip(), re.S)
            if not m:
                continue
            expr = m.group(1).strip()
            if not expr or expr.startswith('{'):
                continue
            rest_lo = b
            rest = body[rest_lo:hi]
            uses = list(re.finditer(r'\b%s\b' % re.escape(name), rest))
            if len(uses) != 1:
                continue
            # y que no reaparezca fuera del bloque
            if re.search(r'\b%s\b' % re.escape(name), body[hi:]):
                continue
            u = uses[0]
            new = body[:a] + body[b:rest_lo + u.start()] + '(' + expr + ')' + \
                body[rest_lo + u.end():]
            out.append(('inline_temp:%s' % name, new))
    return out


def t_hoist_temp(body):
    """Inverso: mete una variable intermedia para el argumento de una llamada.

    `f(a->b(c))` -> `{ __typeof__(a->b(c)) __t = a->b(c); f(__t); }`
    Vale un `mr` de mas o de menos."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b]
            if stmt_kind(s) != 'simple' or '=' in s:
                continue
            m = re.match(r'^\s*([\w:.\->\[\]]+)\s*\((.*)\)\s*;\s*$', s, re.S)
            if not m:
                continue
            args = split_args(m.group(2))
            for k, arg in enumerate(args):
                arg = arg.strip()
                if not re.search(r'[(\->.]', arg) or len(arg) > 90:
                    continue
                na = list(args)
                na[k] = '__pt%d' % k
                stmt = ('{ __typeof__(%s) __pt%d = %s; %s(%s); }'
                        % (arg, k, arg, m.group(1), ', '.join(x.strip() for x in na)))
                out.append(('hoist@%d#%d' % (a, k), body[:a] + stmt + body[b:]))
    return out


def t_bind_ref(body):
    """Liga el resultado de una llamada a un `const T&` en la sentencia anterior
    (mueve la carga detras de la llamada)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b].strip()
            name = is_decl(s)
            if not name or 'const' in s.split('=')[0]:
                continue
            m = re.match(r'^(.*?)\s+(\*?\s*%s)\s*=\s*(.*);$' % re.escape(name), s, re.S)
            if not m or '*' in m.group(2):
                continue
            new_s = 'const %s &%s = %s;' % (m.group(1).strip(), name, m.group(3).strip())
            out.append(('bind_ref:%s' % name, body[:a] + new_s + body[b:]))
    return out


def split_args(text):
    parts, depth, cur = [], 0, []
    for c in text:
        if c in '([<':
            depth += 1
        elif c in ')]>':
            depth -= 1
        if c == ',' and depth == 0:
            parts.append(''.join(cur))
            cur = []
            continue
        cur.append(c)
    parts.append(''.join(cur))
    return [p for p in parts if p.strip()]


def negate(cond):
    c = cond.strip()
    forms = ['!(%s)' % c]
    m = re.match(r'^!\s*\((.*)\)$', c, re.S)
    if m:
        forms.append(m.group(1))
    m = re.match(r'^!\s*([\w:.\->\[\]()]+)$', c)
    if m:
        forms.append(m.group(1))
    for a, b in (('==', '!='), ('!=', '=='), ('<=', '>'), ('>=', '<'),
                 ('<', '>='), ('>', '<=')):
        parts = split_top(c, a)
        if len(parts) == 2:
            forms.append('%s %s %s' % (parts[0].strip(), b, parts[1].strip()))
            break
    return list(dict.fromkeys(forms))


def split_top(text, op):
    """Parte por `op` cuando aparece a profundidad 0 (una sola vez)."""
    depth, i, n = 0, 0, len(text)
    while i < n:
        c = text[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif depth == 0 and text.startswith(op, i):
            if op in ('<', '>') and text[i:i + 2] in ('<<', '>>', '<=', '>='):
                i += 1
                continue
            if op in ('==', '!=', '<=', '>=', '&&', '||') or \
               (text[i - 1:i] not in ('=', '<', '>', '!') and
                    text[i + len(op):i + len(op) + 1] not in ('=',)):
                return [text[:i], text[i + len(op):]]
        i += 1
    return [text]


IF_RE = re.compile(r'\bif\s*\(')


def find_if(body):
    """-> [(inicio_if, cond, cuerpo_span, else_span_or_None, fin)]"""
    db, dp, code = scan(body)
    res = []
    for m in IF_RE.finditer(body):
        i = m.start()
        if not code[i]:
            continue
        if body[max(0, i - 5):i].rstrip().endswith('else'):
            pass
        p = m.end() - 1
        depth, j = 0, p
        while j < len(body):
            if code[j] and body[j] == '(':
                depth += 1
            elif code[j] and body[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        cond = body[p + 1:j]
        k = j + 1
        while k < len(body) and (body[k].isspace() or not code[k]):
            k += 1
        then_end = _stmt_end(body, k, code, db)
        if then_end is None:
            continue
        e = then_end
        while e < len(body) and (body[e].isspace() or not code[e]):
            e += 1
        els = None
        if body[e:e + 4] == 'else' and not body[e + 4:e + 5].isalnum() \
                and body[e + 4:e + 5] != '_':
            f = e + 4
            while f < len(body) and (body[f].isspace() or not code[f]):
                f += 1
            ee = _stmt_end(body, f, code, db)
            if ee is not None:
                els = (f, ee)
        res.append((i, p + 1, j, k, then_end, els))
    return res


def _stmt_end(body, k, code, db):
    if k >= len(body):
        return None
    if body[k] == '{':
        depth, j = 0, k
        while j < len(body):
            if code[j] and body[j] == '{':
                depth += 1
            elif code[j] and body[j] == '}':
                depth -= 1
                if depth == 0:
                    return j + 1
            j += 1
        return None
    j = k
    depth = 0
    while j < len(body):
        if code[j] and body[j] in '([':
            depth += 1
        elif code[j] and body[j] in ')]':
            depth -= 1
        elif code[j] and body[j] == ';' and depth == 0:
            return j + 1
        j += 1
    return None


def t_invert_if(body):
    """`if (C) A else B` -> `if (!C) B else A`. GCC deja la caida en la rama
    que NO es el cuerpo, asi que esto voltea la polaridad del salto."""
    out = []
    for (i, cs, ce, ts, te, els) in find_if(body):
        if not els:
            continue
        cond = body[cs:ce]
        then = body[ts:te]
        oth = body[els[0]:els[1]]
        if oth.lstrip().startswith('if'):
            continue
        for nc in negate(cond)[:2]:
            new = (body[:cs] + nc + body[ce:ts] + _blockify(oth) +
                   ' else ' + _blockify(then) + body[els[1]:])
            out.append(('invert_if@%d' % i, new))
    return out


def _blockify(s):
    s = s.strip()
    return s if s.startswith('{') else '{ %s }' % s


def t_split_and(body):
    """`if (A && B) X` -> `if (A) { if (B) X }`  y el inverso."""
    out = []
    for (i, cs, ce, ts, te, els) in find_if(body):
        cond = body[cs:ce]
        parts = split_top(cond, '&&')
        if len(parts) == 2 and not els:
            new = (body[:i] + 'if (%s) { if (%s) %s }'
                   % (parts[0].strip(), parts[1].strip(), _blockify(body[ts:te]))
                   + body[te:])
            out.append(('split_&&@%d' % i, new))
        # inverso: if (A) { if (B) X }  ->  if (A && B) X
        inner = body[ts:te].strip()
        if not els and inner.startswith('{'):
            core = inner[1:-1].strip()
            m = re.match(r'^if\s*\((.*)\)\s*(.*)$', core, re.S)
            if m and core.count('if') == 1:
                new = (body[:i] + 'if (%s && %s) %s'
                       % (cond.strip(), m.group(1).strip(), m.group(2).strip())
                       + body[te:])
                out.append(('merge_&&@%d' % i, new))
    return out


def t_zero_if(body):
    """`x = 0; if (A) x = 1;`  <->  `if (A) x = 1; else x = 0;`"""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for k in range(len(st) - 1):
            a, b = st[k], st[k + 1]
            s1 = body[a[0]:a[1]].strip()
            s2 = body[b[0]:b[1]].strip()
            m1 = re.match(r'^([\w:.\->\[\]]+)\s*=\s*([^;]+);$', s1)
            if not m1 or not s2.startswith('if'):
                continue
            ifs = find_if(s2)
            if not ifs:
                continue
            (i2, cs, ce, ts, te, els) = ifs[0]
            if els or te != len(s2.rstrip()):
                continue
            inner = s2[ts:te].strip().strip('{}').strip()
            m2 = re.match(r'^%s\s*=\s*([^;]+);$' % re.escape(m1.group(1)), inner)
            if not m2:
                continue
            new_s = ('if (%s) { %s = %s; } else { %s = %s; }'
                     % (s2[cs:ce].strip(), m1.group(1), m2.group(1).strip(),
                        m1.group(1), m1.group(2).strip()))
            out.append(('zero_if@%d' % a[0], body[:a[0]] + new_s + body[b[1]:]))
    return out


def t_while_for(body):
    """`while (c) {}` <-> `for (;;) { if (!c) break; }`"""
    out = []
    db, dp, code = scan(body)
    for m in re.finditer(r'\bwhile\s*\(', body):
        i = m.start()
        if not code[i]:
            continue
        j, depth = m.end() - 1, 0
        while j < len(body):
            if code[j] and body[j] == '(':
                depth += 1
            elif code[j] and body[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        cond = body[m.end():j]
        k = j + 1
        while k < len(body) and body[k].isspace():
            k += 1
        if body[k:k + 1] != '{':
            continue
        end = _stmt_end(body, k, code, db)
        if end is None:
            continue
        inner = body[k + 1:end - 1]
        nc = negate(cond)[0]
        new = body[:i] + 'for (;;) { if (%s) break;\n%s }' % (nc, inner) + body[end:]
        out.append(('while2for@%d' % i, new))
    for m in re.finditer(r'\bfor\s*\(\s*;\s*;\s*\)\s*\{', body):
        i = m.start()
        if not code[i]:
            continue
        k = m.end() - 1
        end = _stmt_end(body, k, code, db)
        if end is None:
            continue
        inner = body[k + 1:end - 1].strip()
        m2 = re.match(r'^if\s*\((.*?)\)\s*break\s*;(.*)$', inner, re.S)
        if not m2:
            continue
        nc = negate(m2.group(1))[0]
        new = body[:i] + 'while (%s) {%s}' % (nc, m2.group(2)) + body[end:]
        out.append(('for2while@%d' % i, new))
    return out


def t_const(body):
    """El `const` de una local es un dial: funciona en los dos sentidos."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b]
            name = is_decl(s)
            if not name:
                continue
            t = s.lstrip()
            pad = s[:len(s) - len(t)]
            if t.startswith('const '):
                out.append(('unconst@%d' % a, body[:a] + pad + t[6:] + body[b:]))
                continue
            # solo si no se reasigna (si no, ni compila y gastamos un compilado)
            rest = body[b:]
            if re.search(r'\b%s\s*(\+\+|--|[-+*/|&^]?=[^=])' % re.escape(name), rest):
                continue
            if re.search(r'[&(]\s*%s\b' % re.escape(name), rest):
                continue
            if '=' not in t.split(';')[0]:
                continue
            out.append(('const@%d' % a, body[:a] + pad + 'const ' + t + body[b:]))
    return out


COMMUTE = ('||', '&&', '|', '^', '+', '*', '==', '!=')


def t_commute(body):
    """Orden de los operandos: `a | b` emite primero `b`; el primero que entra
    en el registro resultado de Min/Max es `a`."""
    out = []
    # condiciones de if/while: partir por && / || de primer nivel
    for (i, cs, ce, ts, te, els) in find_if(body):
        cond = body[cs:ce]
        for op in ('&&', '||'):
            parts = split_top(cond, op)
            if len(parts) != 2:
                continue
            l, r = parts[0].strip(), parts[1].strip()
            if not l or not r:
                continue
            out.append(('commute%s@%d' % (op, i),
                        body[:cs] + '%s %s %s' % (r, op, l) + body[ce:]))
    # RHS de una asignacion simple: lhs = A op B;
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            st = body[a:b].strip()
            if stmt_kind(st) != 'simple':
                continue
            m = re.match(r'^(.*?=\s*)(.+);$', st, re.S)
            if not m or '==' in st or '!=' in st:
                continue
            rhs = m.group(2).strip()
            for op in ('|', '^', '+', '*', '&'):
                parts = split_top(rhs, op)
                if len(parts) != 2:
                    continue
                l, r = parts[0].strip(), parts[1].strip()
                if not l or not r or l.endswith(('(', ',')) or r.startswith(')'):
                    continue
                if l.count('(') != l.count(')') or r.count('(') != r.count(')'):
                    continue
                ns = '%s%s %s %s;' % (m.group(1), r, op, l)
                out.append(('commute%s@%d' % (op, a), body[:a] + ns + body[b:]))
    # argumentos de Min/Max (el primero es el que entra en el registro destino)
    for m in re.finditer(r'\b(Min|Max)\s*\(([^()]*)\)', body):
        args = split_args(m.group(2))
        if len(args) != 2:
            continue
        ns = '%s(%s, %s)' % (m.group(1), args[1].strip(), args[0].strip())
        out.append(('swapargs:%s@%d' % (m.group(1), m.start()),
                    body[:m.start()] + ns + body[m.end():]))
    return out


def t_case_order(body):
    """El orden de los `case` es el orden de emision de los cuerpos."""
    out = []
    for (lo, hi) in all_blocks(body):
        txt = body[lo:hi]
        if 'case' not in txt:
            continue
        db, dp, code = scan(body)
        marks = [m.start() for m in re.finditer(r'\bcase\b|\bdefault\b', body)
                 if lo <= m.start() < hi and code[m.start()]
                 and db[m.start()] == db[lo]]
        if len(marks) < 2:
            continue
        groups = []
        for i, s in enumerate(marks):
            e = marks[i + 1] if i + 1 < len(marks) else hi
            groups.append((s, e))
        for i in range(len(groups) - 1):
            g1, g2 = groups[i], groups[i + 1]
            t1, t2 = body[g1[0]:g1[1]], body[g2[0]:g2[1]]
            if not re.search(r'\b(break|return|goto)\b', t1):
                continue
            new = _splice(body, [g1, g2], [t2, t1])
            out.append(('caseswap@%d' % g1[0], new))
    return out


def t_empty_case(body):
    """Un `case` vacio invisible mueve el pivote del arbol de decision."""
    out = []
    vals = set()
    for m in re.finditer(r'\bcase\s+(-?\d+|0[xX][0-9a-fA-F]+)\s*:', body):
        try:
            vals.add(int(m.group(1), 0))
        except ValueError:
            pass
    if not vals:
        return out
    for m in re.finditer(r'\bcase\s+(-?\d+|0[xX][0-9a-fA-F]+)\s*:', body):
        try:
            v = int(m.group(1), 0)
        except ValueError:
            continue
        for d in (-1, 1):
            if v + d in vals:
                continue
            new = body[:m.start()] + 'case %d:\n' % (v + d) + body[m.start():]
            out.append(('empty_case%d@%d' % (v + d, m.start()), new))
            vals.add(v + d)
    return out


def t_elseif(body):
    """Cadena de `if` sueltos contra `else if` (cuando el cuerpo retorna)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for k in range(len(st) - 1):
            a, b = st[k], st[k + 1]
            s1, s2 = body[a[0]:a[1]].strip(), body[b[0]:b[1]].strip()
            if not s1.startswith('if') or not s2.startswith('if'):
                continue
            if 'else' in s1:
                continue
            new = _splice(body, [a, b], [s1 + ' else ' + s2, ''])
            out.append(('elseif@%d' % a[0], new))
        # inverso: separar un else-if
    for (i, cs, ce, ts, te, els) in find_if(body):
        if els and body[els[0]:els[1]].lstrip().startswith('if') \
                and re.search(r'\breturn\b|\bbreak\b', body[ts:te]):
            new = body[:els[0] - 4] + '\n' + body[els[0]:]
            if 'else' in body[max(0, els[0] - 6):els[0]]:
                j = body.rfind('else', 0, els[0])
                new = body[:j] + '\n' + body[j + 4:]
                out.append(('split_elseif@%d' % j, new))
    return out


def t_split_assign(body):
    """Mete una variable intermedia: `lhs = expr;` -> `T __t = expr; lhs = __t;`
    Una variable intermedia vale un `mr` (y el inverso tambien)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for n, (a, b) in enumerate(split_block(body, lo, hi)):
            s = body[a:b].strip()
            if stmt_kind(s) != 'simple' or is_decl(s):
                continue
            m = re.match(r'^([\w:.\->\[\]() ]+?)\s*=\s*(.+);$', s, re.S)
            if not m or '==' in s or '=' in m.group(1):
                continue
            lhs, expr = m.group(1).strip(), m.group(2).strip()
            if not expr or expr.startswith('{'):
                continue
            tmp = '__pv%d' % n
            stmt = '__typeof__(%s) %s = %s; %s = %s;' % (lhs, tmp, expr, lhs, tmp)
            out.append(('split_assign@%d' % a, body[:a] + stmt + body[b:]))
    return out


def t_cse_temp(body):
    """`x = expr; ... g(x)` -> `T t = expr; x = t; ... g(t)`.

    Liga el valor a un temporal y usa el temporal en el resto del bloque: mueve
    la carga detras de la llamada y suele quitar (o poner) un `mr`."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for n, (a, b) in enumerate(st):
            s = body[a:b].strip()
            if stmt_kind(s) != 'simple':
                continue
            m = re.match(r'^([\w:.\->\[\]() ]+?)\s*=\s*(.+);$', s, re.S)
            if not m or '==' in s:
                continue
            lhs, expr = m.group(1).strip(), m.group(2).strip()
            decl = is_decl(s)
            if decl:
                lhs = decl
            if not re.match(r'^[\w:.\->\[\]]+$', lhs):
                continue
            rest = body[b:hi]
            if not re.search(re.escape(lhs), rest):
                continue
            if re.search(re.escape(lhs), body[hi:]):
                continue
            tmp = '__ct%d' % n
            head = ('__typeof__(%s) %s = %s; %s = %s;' % (lhs, tmp, expr, lhs, tmp)
                    if not decl else s[:-1] + '; __typeof__(%s) %s = %s;' % (lhs, tmp, lhs))
            nrest = rest.replace(lhs, tmp)
            out.append(('cse_temp:%s@%d' % (lhs[:14], a),
                        body[:a] + head + nrest + body[hi:]))
    return out


def t_expand_use(body):
    """Sustituye TODOS los usos de un temporal por su expresion (escribir la
    misma expresion dos veces contra `b = a;`: la primera produce un `fmr`)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b]
            name = is_decl(s)
            if not name:
                continue
            m = re.match(r'^[^=;]*=\s*(.*);\s*$', s.strip(), re.S)
            if not m:
                continue
            expr = m.group(1).strip()
            if not expr or len(expr) > 70 or expr.startswith('{'):
                continue
            rest = body[b:hi]
            uses = list(re.finditer(r'\b%s\b' % re.escape(name), rest))
            if len(uses) < 2 or re.search(r'\b%s\b' % re.escape(name), body[hi:]):
                continue
            if re.search(r'\b%s\s*[-+*/|&^]?=[^=]' % re.escape(name), rest):
                continue
            nrest = re.sub(r'\b%s\b' % re.escape(name), '(' + expr + ')', rest)
            out.append(('expand:%s' % name, body[:a] + body[b:b] + nrest + body[hi:]))
    return out


def t_named_temp(body):
    """`Tipo(args).metodo(...)` -> `Tipo __r(args); __r.metodo(...)`.

    El temporal anonimo y el nombrado no reparten los registros igual: el
    nombrado suele materializar la direccion directamente en r3."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for n, (a, b) in enumerate(split_block(body, lo, hi)):
            s = body[a:b]
            for m in re.finditer(r'\b([A-Z]\w+)\s*\(([^()]*)\)\s*\.', s):
                if m.group(1) in ('UMath', 'if', 'while', 'switch', 'for'):
                    continue
                tmp = '__nt%d' % n
                decl = '%s %s(%s); ' % (m.group(1), tmp, m.group(2))
                ns = s[:m.start()] + tmp + '.' + s[m.end():]
                out.append(('named_temp:%s@%d' % (m.group(1)[:12], a),
                            body[:a] + decl + ns + body[b:]))
    return out


def t_anon_temp(body):
    """Inverso de named_temp: `Tipo t(args); ... t.m()` -> `Tipo(args).m()`."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for i, (a, b) in enumerate(st):
            s = body[a:b].strip()
            m = re.match(r'^([A-Z]\w+)\s+(\w+)\s*\(([^()]*)\)\s*;$', s)
            if not m:
                continue
            name = m.group(2)
            rest = body[b:hi]
            uses = list(re.finditer(r'\b%s\b' % re.escape(name), rest))
            if len(uses) != 1 or re.search(r'\b%s\b' % re.escape(name), body[hi:]):
                continue
            u = uses[0]
            new = (body[:a] + rest[:u.start()] + '%s(%s)' % (m.group(1), m.group(3))
                   + rest[u.end():] + body[hi:])
            out.append(('anon_temp:%s' % name, new))
    return out


def t_empty_then(body):
    """`if (C) X` (sin else) -> `if (!C) {} else X`.

    GCC deja la caida en la rama que NO es el cuerpo, asi que esto voltea la
    polaridad del salto sin cambiar el numero de instrucciones."""
    out = []
    for (i, cs, ce, ts, te, els) in find_if(body):
        if els:
            continue
        cond = body[cs:ce]
        for nc in negate(cond)[:2]:
            new = (body[:cs] + nc + body[ce:ts] + '{ } else ' +
                   _blockify(body[ts:te]) + body[te:])
            out.append(('empty_then@%d' % i, new))
    return out


def t_case_rotate(body):
    """Rota la lista de `case` completa: el orden de los case es el orden de
    emision de los cuerpos y mueve el pivote del arbol de decision."""
    out = []
    for (lo, hi) in all_blocks(body):
        txt = body[lo:hi]
        if 'case' not in txt:
            continue
        db, dp, code = scan(body)
        marks = [m.start() for m in re.finditer(r'\bcase\b|\bdefault\b', body)
                 if lo <= m.start() < hi and code[m.start()]
                 and db[m.start()] == db[lo]]
        if len(marks) < 3:
            continue
        groups = [(s, marks[i + 1] if i + 1 < len(marks) else hi)
                  for i, s in enumerate(marks)]
        texts = [body[a:b] for a, b in groups]
        if any(not re.search(r'\b(break|return|goto)\b', t) for t in texts[:-1]):
            continue
        n = len(texts)
        for k in (1, 2, n - 1):
            rot = texts[k:] + texts[:k]
            if rot == texts:
                continue
            out.append(('case_rot%d@%d' % (k, lo), _splice(body, groups, rot)))
    return out


def t_goto_inline(body):
    """`goto L;` con `L:` a un bloque corto que acaba en return -> copiar el
    bloque en el sitio del goto. Cambia la colocacion de los bloques y con ella
    la polaridad de los saltos del arbol de decision."""
    out = []
    db, dp, code = scan(body)
    labels = {}
    for m in re.finditer(r'^[ 	]*(\w+):[ 	]*$', body, re.M):
        if m.group(1) in ('default', 'public', 'private', 'protected'):
            continue
        labels[m.group(1)] = m.end()
    for m in re.finditer(r'\bgoto\s+(\w+)\s*;', body):
        name = m.group(1)
        if name not in labels or not code[m.start()]:
            continue
        if len(re.findall(r'\bgoto\s+%s\s*;' % re.escape(name), body)) != 1:
            continue
        pos = labels[name]
        # bloque enclosante de la etiqueta
        block = None
        for (lo, hi) in sorted(all_blocks(body), key=lambda x: x[1] - x[0]):
            if lo <= pos < hi:
                block = (lo, hi)
                break
        if block is None:
            block = (0, len(body))
        piece, n = [], 0
        for (a, b) in split_block(body, pos, block[1]):
            if a < pos:
                continue
            txt = body[a:b]
            piece.append(txt)
            n += 1
            if re.match(r'^\s*(return|break)\b', txt) or n >= 5:
                break
        if not piece or not re.match(r'^\s*(return|break)\b', piece[-1]):
            continue
        rep = ' '.join(x.strip() for x in piece)
        out.append(('goto_inline:%s' % name,
                    body[:m.start()] + rep + body[m.end():]))
    return out


def t_dead_local(body):
    """Quitar (o poner) una local muerta: gasta un registro salvado de mas.

    La firma es un `stmw` que empieza un registro antes que el del original."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b]
            name = is_decl(s)
            if not name:
                continue
            rest = body[:a] + body[b:]
            if re.search(r'\bNAME\b'.replace('NAME', re.escape(name)), rest):
                continue
            out.append(('dead_local:%s' % name, body[:a] + body[b:]))
    return out


# --------------------------------------------------------------------------
# 4b. TRANSFORMACIONES DE ESTRUCTURA LEXICA
#
# El triaje de regmap.py dice que el 60% de los near-miss entre el 90% y el
# 100% es ESTRUCTURA (una local que falta o sobra, o vive en otro ambito) y
# solo el 4% es techo del asignador. Estas son las que atacan ese 60%.
# --------------------------------------------------------------------------

FOR_RE = re.compile(r'\bfor\s*\(')


def _for_heads(body):
    """-> [(inicio_for, ini_parentesis, fin_parentesis, (init,cond,step), cortes)]"""
    db, dp, code = scan(body)
    out = []
    for m in FOR_RE.finditer(body):
        i = m.start()
        if not code[i]:
            continue
        p = m.end() - 1
        depth, j = 0, p
        while j < len(body):
            if code[j] and body[j] == '(':
                depth += 1
            elif code[j] and body[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        if j >= len(body):
            continue
        inner = body[p + 1:j]
        d, cuts = 0, []
        for k, c in enumerate(inner):
            if c in '([':
                d += 1
            elif c in ')]':
                d -= 1
            elif c == ';' and d == 0:
                cuts.append(k)
        if len(cuts) != 2:
            continue
        out.append((i, p + 1, j,
                    (inner[:cuts[0]], inner[cuts[0] + 1:cuts[1]], inner[cuts[1] + 1:]),
                    cuts))
    return out


DECL_INIT_RE = re.compile(
    r'^\s*((?:const\s+|unsigned\s+|signed\s+)*'
    r'(?:int|short|long|char|float|double|bool|size_t|unsigned|s32|u32|s16|u16|'
    r'[A-Z][\w:]*)\s*\**)\s+([A-Za-z_]\w*)\s*=\s*(.+?)\s*$')


def t_for_counter(body):
    """Saca el contador de un `for` a nivel de funcion (y lo reutiliza).

    El original de NFSMW declara **un solo** contador por funcion y lo usa en
    todos los bucles; nosotros escribimos `for (int i = 0; ...)` en cada uno.
    Un contador declarado en el `for` abre un ambito propio y ocupa un registro
    salvado distinto: es el subpatron dominante del veredicto ESTRUCTURA.
    """
    out = []
    decls = []
    for (i, p0, p1, parts, cuts) in _for_heads(body):
        m = DECL_INIT_RE.match(parts[0])
        if m:
            decls.append((i, p0, p1, m.group(1).strip(), m.group(2),
                          m.group(3).strip(), cuts))

    # 1) cada for por separado: el contador sube a nivel de funcion
    for (i, p0, p1, ty, name, init, cuts) in decls:
        new = body[:p0] + ('%s = %s' % (name, init)) + body[p0 + cuts[0]:]
        out.append(('for_hoist:%s@%d' % (name, i),
                    '%s %s;\n' % (ty, name) + new))

    # 2) todos a la vez, y ademas unificando el nombre (el patron de EA)
    if len(decls) > 1:
        for unify in (False, True):
            tys = set(d[3] for d in decls)
            if unify and len(tys) != 1:
                continue
            new, names, ok = body, [], True
            for (i, p0, p1, ty, name, init, cuts) in sorted(decls, key=lambda d: -d[1]):
                nm = decls[0][4] if unify else name
                if nm != name:
                    dbn, _, cdn = scan(new)
                    e = _stmt_end(new, _skipws(new, p1 + 1), cdn, dbn)
                    if e is None:
                        ok = False
                        break
                    seg = re.sub(r'\b%s\b' % re.escape(name), nm, new[p1:e])
                    new = new[:p1] + seg + new[e:]
                new = new[:p0] + ('%s = %s' % (nm, init)) + new[p0 + cuts[0]:]
                names.append((ty, nm))
            if not ok:
                continue
            pre = ''.join('%s %s;\n' % (a, b) for a, b in dict.fromkeys(names))
            out.append(('for_hoist_all%s' % ('_unif' if unify else ''), pre + new))

    # 3) inverso: `int i;` suelto y `for (i = 0; ...)` -> meterlo dentro del for
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            s = body[a:b].strip()
            m = re.match(r'^((?:unsigned\s+|signed\s+)*'
                         r'(?:int|short|long|char|unsigned))\s+(\w+)\s*;$', s)
            if not m:
                continue
            ty, name = m.group(1), m.group(2)
            hits = [h for h in _for_heads(body)
                    if re.match(r'^\s*%s\s*=' % re.escape(name), h[3][0])]
            if len(hits) != 1:
                continue
            h = hits[0]
            dbn, _, cdn = scan(body)
            e = _stmt_end(body, _skipws(body, h[2] + 1), cdn, dbn)
            if e is None or re.search(r'\b%s\b' % re.escape(name), body[e:]):
                continue
            new = body[:h[1]] + ty + ' ' + body[h[1]:]
            new = new[:a] + new[b:] if a < h[1] else new
            out.append(('for_sink:%s' % name, new))
    return out


def _skipws(body, k):
    while k < len(body) and body[k].isspace():
        k += 1
    return k


def _owner_start(body, brace_content_lo):
    """Inicio de la sentencia cuyo cuerpo `{` empieza en brace_content_lo-1."""
    db, dp, code = scan(body)
    i = brace_content_lo - 2
    depth = 0
    while i >= 0:
        c = body[i]
        if code[i]:
            if c in ')]':
                depth += 1
            elif c in '([':
                depth -= 1
            elif depth == 0 and c in ';{}':
                return i + 1
        i -= 1
    return 0


def t_hoist_decl(body):
    """Sube una declaracion un nivel lexico: la saca delante del bloque.

    regmap dice `esta local esta a otra profundidad` o `el bloque bN SOBRA`;
    esta es la forma exacta de arreglarlo desde la fuente."""
    out = []
    for (lo, hi) in all_blocks(body):
        start = _owner_start(body, lo)
        for (a, b) in split_block(body, lo, hi):
            name = is_decl(body[a:b])
            if not name:
                continue
            txt = body[a:b].strip()
            if not txt.endswith(';') or start is None or start >= a:
                continue
            # sacar la declaracion del bloque la ejecuta aunque el bloque no se
            # ejecute: solo vale si el inicializador no tiene efectos (si no,
            # el permuter generaria codigo que no significa lo mismo).
            init = txt.split('=', 1)[1] if '=' in txt else ''
            if re.search(r'\w\s*\(', init) or '++' in init or '--' in init:
                continue
            out.append(('hoist_decl:%s' % name,
                        body[:start] + txt + '\n' + body[start:a] + body[b:]))
    return out


def t_sink_decl(body):
    """Baja una declaracion al punto donde de verdad se usa.

    **Acorta el rango de vida**, que es lo que decide que registro salvado
    toca: el arreglo de las permutaciones puras que el asignador no da
    (`vehicleRemoved` robandole r31 al iterador, `leftDistHigh` robandole f28).
    """
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for k, (a, b) in enumerate(st):
            name = is_decl(body[a:b])
            if not name:
                continue
            txt = body[a:b].strip()
            if not txt.endswith(';'):
                continue
            first = None
            for j in range(k + 1, len(st)):
                if re.search(r'\b%s\b' % re.escape(name), body[st[j][0]:st[j][1]]):
                    first = j
                    break
            if first is None or first == k + 1:
                continue
            tgt = st[first][0]
            out.append(('sink_decl:%s@%d' % (name, first),
                        body[:a] + body[b:tgt] + txt + '\n' + body[tgt:]))
    return out


def t_split_decl(body):
    """`T x = e;` <-> `T x; ... x = e;`  (alarga / acorta el rango de vida).

    Con las mismas locales y el mismo arbol de bloques, este es el unico dial
    que le queda a una PERMUTACION pura."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        for k, (a, b) in enumerate(st):
            name = is_decl(body[a:b])
            if not name:
                continue
            txt = body[a:b].strip()
            m = re.match(r'^(.*?)\b%s\s*=\s*(.+);$' % re.escape(name), txt, re.S)
            if m and '(' not in m.group(1) and m.group(1).strip():
                ty, val = m.group(1).strip(), m.group(2).strip()
                out.append(('split_decl:%s' % name,
                            body[:a] + '%s %s;\n%s = %s;\n' % (ty, name, name, val)
                            + body[b:]))
                out.append(('split_decl_top:%s' % name,
                            '%s %s;\n' % (ty, name) + body[:a]
                            + '%s = %s;\n' % (name, val) + body[b:]))
                continue
            m = re.match(r'^(.*?)\b%s\s*;$' % re.escape(name), txt, re.S)
            if not m or '(' in m.group(1) or not m.group(1).strip():
                continue
            for j in range(k + 1, len(st)):
                seg = body[st[j][0]:st[j][1]].strip()
                ma = re.match(r'^%s\s*=\s*(.+);$' % re.escape(name), seg, re.S)
                if ma:
                    out.append(('merge_decl:%s' % name,
                                body[:a] + body[b:st[j][0]] +
                                '%s %s = %s;\n' % (m.group(1).strip(), name,
                                                   ma.group(1).strip()) +
                                body[st[j][1]:]))
                    break
                if re.search(r'\b%s\b' % re.escape(name), seg):
                    break
    return out


_ZERO_OF = [
    (re.compile(r'^(?:const\s+)?bool$'), ['false', 'true']),
    (re.compile(r'^(?:const\s+)?(?:unsigned\s+|signed\s+)?'
                r'(?:char|short|int|long|long\s+long)$'), ['0']),
    (re.compile(r'^(?:const\s+)?(?:float|double)$'), ['0.0f']),
    (re.compile(r'^.*\*$'), ['nullptr']),
]


def t_init_decl(body):
    """`T x;` -> `T x = <cero>;` sin tocar nada mas.

    Parece inocuo y NO lo es: el inicializador muerto no emite instruccion
    (GCC lo borra) pero **cambia el reparto de registros**, porque la local
    nace viva en la declaracion en vez de en su primera asignacion real. Es
    la hermana de `ghost_local`: alli se anade una local que no existe, aqui
    se le da valor inicial a una que ya esta. Cerro
    `CameraMover::EnforceMinGapToWalls` (1.224 B) de 99,837% a 100% con las
    mismas 306 instrucciones: solo bailaban r26 y r27."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for (a, b) in split_block(body, lo, hi):
            name = is_decl(body[a:b])
            if not name:
                continue
            txt = body[a:b].strip()
            m = re.match(r'^(.*?)\b%s\s*;$' % re.escape(name), txt, re.S)
            if not m:
                continue
            ty = m.group(1).strip()
            # `T x;` de un escalar: ni llamadas, ni arrays, ni ctores
            if not ty or '(' in ty or '[' in txt or '=' in txt:
                continue
            for rx, vals in _ZERO_OF:
                if rx.match(ty):
                    for v in vals:
                        out.append(('init_decl:%s=%s' % (name, v),
                                    body[:a] + '%s %s = %s;\n' % (ty, name, v)
                                    + body[b:]))
                    break
    return out


def t_add_local(body):
    """Introduce una local declarada que hoy no existe.

    Cuando regmap dice `falta esta local`: el original la declaraba y nosotros
    la escribimos como subexpresion repetida."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        if len(st) < 2:
            continue
        seg = body[st[0][0]:st[-1][1]]
        cnt = {}
        for m in re.finditer(r'\b[A-Za-z_]\w*(?:(?:->|\.)\w+)+(?:\(\))?', seg):
            e = m.group(0)
            if 6 <= len(e) <= 60:
                cnt[e] = cnt.get(e, 0) + 1
        for n, (e, c) in enumerate(sorted(cnt.items(), key=lambda kv: -kv[1])[:6]):
            if c < 2:
                continue
            tmp = '__al%d' % n
            nseg = re.sub(r'(?<![\w>.])%s(?![\w(])' % re.escape(e), tmp, seg)
            if nseg == seg:
                continue
            out.append(('add_local:%s' % e[:20],
                        body[:st[0][0]] + '__typeof__(%s) %s = %s;\n' % (e, tmp, e)
                        + nseg + body[st[-1][1]:]))
    return out


def t_block_wrap(body):
    """Envuelve una tirada de sentencias en un bloque anonimo `{ ... }`.

    Cuando regmap dice `el bloque bN FALTA en el nuestro`: el original abria un
    ambito que nosotros no abrimos, y eso recoloca el reparto entero."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        n = len(st)
        if n < 2:
            continue
        for i in range(n):
            for j in set([i + 1, i + 2, n - 1]):
                if j <= i or j >= n:
                    continue
                a, b = st[i][0], st[j][1]
                names = [x for x in (is_decl(body[p:q]) for p, q in st[i:j + 1]) if x]
                if not names and j != i + 1:
                    # un bloque anonimo que no declara nada tambien cuenta: es
                    # justo lo que regmap ve como "el bloque bN FALTA (declara:
                    # nada)". Pero solo la version corta, o son cientos.
                    continue
                after = body[b:hi] + body[hi:]
                if any(re.search(r'\b%s\b' % re.escape(x), after) for x in names):
                    continue
                out.append(('block_wrap@%d-%d' % (i, j),
                            body[:a] + '{\n' + body[a:b] + '\n}\n' + body[b:]))
    return out


def t_block_unwrap(body):
    """Quita un bloque anonimo `{ ... }` (el inverso de block_wrap)."""
    out = []
    db, dp, code = scan(body)
    for (lo, hi) in all_blocks(body):
        j = lo - 2
        while j >= 0 and (body[j].isspace() or not code[j]):
            j -= 1
        if j >= 0 and body[j] not in ';{}':
            continue                      # cuelga de un if/for/while/switch
        names = [x for x in (is_decl(body[a:b])
                             for a, b in split_block(body, lo, hi)) if x]
        outside = body[:lo - 1] + body[hi + 1:]
        if any(re.search(r'\b%s\b' % re.escape(x), outside) for x in names):
            continue
        out.append(('block_unwrap@%d' % lo,
                    body[:lo - 1] + body[lo:hi] + body[hi + 1:]))
    return out


# --------------------------------------------------------------------------
# 4c. LISTA DE INICIALIZACION DE UN CONSTRUCTOR
#
# `Sim::Entity::Entity` no ofrecia NINGUNA variante: sus 231 instrucciones
# salen integras de la lista de init, que no vive dentro del `{...}`.
# --------------------------------------------------------------------------

def split_initlist(init):
    """`: a(x), b(y)` -> [(nombre, args)] respetando parentesis y plantillas."""
    s = init.strip()
    if not s.startswith(':'):
        return []
    s = s[1:]
    parts, depth, cur = [], 0, []
    for c in s:
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        if c == ',' and depth == 0:
            parts.append(''.join(cur))
            cur = []
            continue
        cur.append(c)
    parts.append(''.join(cur))
    out = []
    for p in parts:
        p = p.strip()
        if not p:
            continue
        if not p.endswith(')'):
            return []
        d, cut = 0, None
        for k in range(len(p) - 1, -1, -1):
            if p[k] == ')':
                d += 1
            elif p[k] == '(':
                d -= 1
                if d == 0:
                    cut = k
                    break
        if cut is None or cut == 0:
            return []
        out.append((p[:cut].strip(), p[cut + 1:-1]))
    return out


def join_initlist(items):
    if not items:
        return ' '
    return ' : ' + ', '.join('%s(%s)' % (n, a) for n, a in items) + ' '


def ti_swap(init, body):
    """Intercambia dos entradas contiguas de la lista de init.

    GCC 2.9 **emite en el orden en que estan escritas** (avisa con -Wreorder
    pero obedece la lista), asi que el orden es un dial de verdad."""
    items = split_initlist(init)
    out = []
    for i in range(len(items) - 1):
        ni = list(items)
        ni[i], ni[i + 1] = ni[i + 1], ni[i]
        out.append(('init_swap@%d' % i, join_initlist(ni), body))
    return out


def ti_rotate(init, body):
    items = split_initlist(init)
    n = len(items)
    out = []
    for k in (1, 2, n - 1, n - 2):
        if n < 3 or k <= 0 or k >= n:
            continue
        r = items[k:] + items[:k]
        if r != items:
            out.append(('init_rot%d' % k, join_initlist(r), body))
    return out


def ti_to_body(init, body):
    """Mueve un miembro de la lista de init al cuerpo.

    **No es lo mismo**: en la lista se usa el copy-ctor implicito y GCC emite
    una copia de bloque; en el cuerpo, si el tipo declara `operator=`, emite la
    copia miembro a miembro."""
    items = split_initlist(init)
    out = []
    for i, (n, a) in enumerate(items):
        if not re.match(r'^[A-Za-z_]\w*$', n) or not a.strip():
            continue
        if len(split_args(a)) != 1:
            continue
        ni = items[:i] + items[i + 1:]
        stmt = '\n%s = %s;\n' % (n, a.strip())
        out.append(('init2body:%s' % n, join_initlist(ni), stmt + body))
        out.append(('init2body_end:%s' % n, join_initlist(ni), body + stmt))
    return out


def ti_from_body(init, body):
    """Sube `m = x;` del principio del cuerpo a la lista de inicializacion."""
    items = split_initlist(init)
    out = []
    st = split_block(body, 0, len(body))
    for (a, b) in st[:4]:
        s = body[a:b].strip()
        m = re.match(r'^(?:this\s*->\s*)?([A-Za-z_]\w*)\s*=\s*([^=].*);$', s, re.S)
        if not m:
            continue
        name, val = m.group(1), m.group(2).strip()
        if any(n == name for n, _ in items):
            continue
        nb = body[:a] + body[b:]
        for pos in set([len(items), 0]):
            ni = items[:pos] + [(name, val)] + items[pos:]
            out.append(('body2init:%s@%d' % (name, pos), join_initlist(ni), nb))
    return out


INIT_TRANSFORMS = [
    ('init_swap', ti_swap),
    ('init_rot', ti_rotate),
    ('init2body', ti_to_body),
    ('body2init', ti_from_body),
]


def gen_init_variants(init, body, only=None, skip=None):
    """Variantes que tocan la lista de inicializacion -> [(tag, init, body)]."""
    if not init.strip().startswith(':'):
        return []
    seen, out = set(), []
    for name, fn in INIT_TRANSFORMS:
        if only and name not in only:
            continue
        if skip and name in skip:
            continue
        try:
            cands = fn(init, body)
        except Exception as exc:
            cands = []
            sys.stderr.write('  (transform %s fallo: %s)\n' % (name, exc))
        for tag, ni, nb in cands:
            if (ni, nb) == (init, body):
                continue
            h = hashlib.md5((ni + '\x00' + nb).encode('utf-8', 'replace')).hexdigest()
            if h in seen:
                continue
            seen.add(h)
            out.append(('%s:%s' % (name, tag), ni, nb))
    return out


# --------------------------------------------------------------------------
# 4e. LO QUE EL DWARF DECLARA Y NOSOTROS NO ESCRIBIMOS
# --------------------------------------------------------------------------

# Locales que el volcado del original declara y a nosotros nos faltan, con su
# tipo. Lo rellena la guia (`plan_from`) desde regmap: las transformaciones de
# abajo las leen de aqui porque el cuerpo no tiene forma de adivinar el tipo.
HINT_LOCALS = []


def t_ghost_local(body):
    """Escribe la local que el DWARF declara aunque parezca que nadie la lee.

    **Un contador muerto NO es inocuo.** `if (!isDone) numIncomplete++;` con
    nadie leyendo `numIncomplete`: GCC borra el store, pero el `if` alarga el
    rango de vida de `isDone` hasta un registro salvado y empuja un miembro a
    la pila. Eso solo llevo `LocalPlayer::UpdateHud` de 99,855% a 100%.
    Regla: una local del DWARF que parece sin usar hay que escribirla igual.
    """
    out = []
    if not HINT_LOCALS:
        return out
    db, dp, code = scan(body)
    for (ty, name) in HINT_LOCALS[:4]:
        if re.search(r'\b%s\b' % re.escape(name), body):
            continue
        ty = (ty or 'int').strip()
        if not re.match(r'^[\w:<>,\s*&]+$', ty) or '[' in ty:
            continue
        decl = '%s %s = 0;\n' % (ty, name)
        # el uso muerto va dentro de cada cuerpo de `if`: es donde estaba en
        # los casos vistos, y es lo que alarga el rango de vida de la condicion
        sites = [(ts, te) for (i, cs, ce, ts, te, els) in find_if(body)]
        for k, (ts, te) in enumerate(sites[:6]):
            seg = body[ts:te]
            ins = ts + 1 if seg.startswith('{') else ts
            use = '%s++;\n' % name if seg.startswith('{') else ''
            if not use:
                out.append(('ghost_local:%s@if%d' % (name, k),
                            decl + body[:ts] + '{ %s++; %s }' % (name, seg)
                            + body[te:]))
            else:
                out.append(('ghost_local:%s@if%d' % (name, k),
                            decl + body[:ins] + '\n' + use + body[ins:]))
        # y la version sin uso: solo la declaracion
        out.append(('ghost_local:%s@decl' % name, decl + body))
    return out


def t_split_arg(body):
    """Saca a una local el argumento que es una LLAMADA.

    Un argumento que es una llamada inline con ramas obliga a precalcular el
    `this` y deja un `mr` de mas: `SetTarget(t, Clamp(...), ang)` no casa y
    `range = Clamp(...); SetTarget(t, range, ang)` si. Es `split_assign`, pero
    sobre el argumento, y **sin abrir bloque** (hoist_temp abria uno y con el
    cambiaba el arbol de ambitos)."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        for n, (a, b) in enumerate(split_block(body, lo, hi)):
            s = body[a:b]
            if stmt_kind(s) not in ('simple',):
                continue
            for m in re.finditer(r'\b([A-Za-z_][\w:.\->]*)\s*\(', s):
                if m.group(1) in ('if', 'while', 'for', 'switch', 'return',
                                  'sizeof', 'return'):
                    continue
                # localizar el parentesis que cierra ESTA llamada
                j, depth = m.end() - 1, 0
                while j < len(s):
                    if s[j] == '(':
                        depth += 1
                    elif s[j] == ')':
                        depth -= 1
                        if depth == 0:
                            break
                    j += 1
                if j >= len(s):
                    continue
                args = split_args(s[m.end():j])
                for k, arg in enumerate(args):
                    arg = arg.strip()
                    # solo argumentos que a su vez son una llamada
                    if not re.match(r'^[A-Za-z_][\w:.\->]*\s*\(.*\)$', arg, re.S):
                        continue
                    if len(arg) > 90:
                        continue
                    tmp = '__sa%d_%d' % (n, k)
                    na = list(args)
                    na[k] = tmp
                    ns = (s[:m.end()] + ', '.join(x.strip() for x in na)
                          + s[j:])
                    out.append(('split_arg:%s#%d' % (m.group(1)[:14], k),
                                body[:a] + '__typeof__(%s) %s = %s;\n'
                                % (arg, tmp, arg) + ns + body[b:]))
    return out


def t_move_stmt(body):
    """Mueve una sentencia varias posiciones (no solo a la de al lado).

    En el grupo IDENTICO todas las locales casan registro a registro y los
    offsets de marco tambien; lo que baila son los registros que sostienen
    direcciones y constantes hoisteadas. El mecanismo es el ORDEN: el original
    **entrelaza** las sentencias que tocan un mismo global y nosotros las
    agrupamos. `swap` solo llega a la vecina; esto llega a la de mas alla."""
    out = []
    for (lo, hi) in [(0, len(body))] + all_blocks(body):
        st = split_block(body, lo, hi)
        n = len(st)
        if n < 3:
            continue
        kinds = [stmt_kind(body[a:b]) for a, b in st]
        for i in range(n):
            if kinds[i] not in ('simple',):
                continue
            si = body[st[i][0]:st[i][1]]
            name = is_decl(si)
            for d in (-3, -2, 2, 3):
                j = i + d
                if not (0 <= j < n) or kinds[j] in ('case', 'default', 'else'):
                    continue
                lo_, hi_ = (min(i, j), max(i, j))
                between = ''.join(body[st[k][0]:st[k][1]]
                                  for k in range(lo_, hi_ + 1) if k != i)
                if name and re.search(r'\b%s\b' % re.escape(name), between):
                    continue
                # ninguna de las de en medio puede declarar algo que use `si`
                bad = False
                for k in range(lo_, hi_ + 1):
                    if k == i:
                        continue
                    nk = is_decl(body[st[k][0]:st[k][1]])
                    if nk and re.search(r'\b%s\b' % re.escape(nk), si):
                        bad = True
                        break
                if bad or 'return' in (kinds[i], kinds[j]):
                    continue
                order = list(range(n))
                order.remove(i)
                order.insert(j, i)
                pieces = [body[st[k][0]:st[k][1]] for k in order]
                out.append(('move%+d@%d' % (d, i),
                            _splice(body, st, pieces)))
    return out


TRANSFORMS = [
    ('swap', t_swap_adjacent),
    ('rot', t_rotate_run),
    ('inline_temp', t_inline_temp),
    ('dead_local', t_dead_local),
    ('split_assign', t_split_assign),
    ('cse_temp', t_cse_temp),
    ('expand_use', t_expand_use),
    ('hoist_temp', t_hoist_temp),
    ('bind_ref', t_bind_ref),
    ('named_temp', t_named_temp),
    ('anon_temp', t_anon_temp),
    ('invert_if', t_invert_if),
    ('empty_then', t_empty_then),
    ('case_rotate', t_case_rotate),
    ('split_and', t_split_and),
    ('zero_if', t_zero_if),
    ('while_for', t_while_for),
    ('const', t_const),
    ('commute', t_commute),
    ('case_order', t_case_order),
    ('empty_case', t_empty_case),
    ('elseif', t_elseif),
    ('goto_inline', t_goto_inline),
    # --- estructura lexica (el 60% del triaje de regmap) ---
    ('for_counter', t_for_counter),
    ('hoist_decl', t_hoist_decl),
    ('sink_decl', t_sink_decl),
    ('split_decl', t_split_decl),
    ('init_decl', t_init_decl),
    ('add_local', t_add_local),
    ('block_wrap', t_block_wrap),
    ('block_unwrap', t_block_unwrap),
    # --- lo que el DWARF declara y no escribimos, y el orden de sentencias ---
    ('ghost_local', t_ghost_local),
    ('split_arg', t_split_arg),
    ('move_stmt', t_move_stmt),
]


# --------------------------------------------------------------------------
# 4d. GUIA POR DIAGNOSTICO (regmap.py)
#
# El ascenso de colina ciego se para en la primera ronda: desde el 99,7% casi
# todo empeora. Pero el diagnostico existe: `regmap.py` enfrenta local a local
# el registro que el compilador de EA asigno contra el nuestro, alinea los
# bloques anonimos por solapamiento de rangos y **nombra la causa**. Su triaje
# de las 371 funciones entre el 90% y el 100%:
#
#     ESTRUCTURA 224 (60%)   REPARTO 65 (18%)
#     IDENTICO    68 (18%)   PERMUTACION 14 (4%)
#
# Elegir transformaciones a ciegas cuando hay un diagnostico disponible es lo
# que estanca la busqueda. Aqui el diagnostico decide QUE transformaciones se
# prueban y SOBRE QUE LOCALES.
# --------------------------------------------------------------------------

# Cada causa nombrada -> las transformaciones que la atacan, en orden.
CAUSE_PLAN = {
    # sobra una local nuestra: el original la tenia como temporal o subexpresion
    'only_ours': ['anon_temp', 'inline_temp', 'dead_local', 'expand_use',
                  'block_unwrap', 'for_counter', 'hoist_decl'],
    # falta una local del original: la escribimos como subexpresion
    'only_orig': ['ghost_local', 'named_temp', 'add_local', 'split_arg',
                  'split_assign', 'cse_temp', 'hoist_temp', 'split_decl',
                  'init_decl', 'for_counter', 'block_wrap'],
    # la misma local pero a otra profundidad lexica
    'moved': ['hoist_decl', 'sink_decl', 'block_wrap', 'block_unwrap',
              'for_counter', 'split_decl'],
    # el original abre un bloque anonimo que nosotros no abrimos
    'block_missing': ['block_wrap', 'sink_decl', 'for_counter', 'split_decl'],
    # nosotros abrimos uno de mas
    'block_extra': ['block_unwrap', 'hoist_decl', 'for_counter', 'dead_local'],
    # mismas locales, mismo arbol, registros en ciclo: rango de vida
    'PERMUTACION': ['sink_decl', 'split_decl', 'init_decl', 'swap', 'move_stmt',
                    'rot', 'cse_temp', 'bind_ref', 'inline_temp', 'expand_use',
                    'commute', 'split_arg', 'hoist_temp', 'const'],
    # mismas locales pero los registros se mueven sin cerrar ciclo
    'REPARTO': ['split_decl', 'init_decl', 'sink_decl', 'dead_local',
                'ghost_local', 'add_local', 'split_arg', 'cse_temp',
                'split_assign', 'hoist_temp', 'const', 'bind_ref', 'move_stmt'],
    # mismas locales y mismo reparto: no es una local, es forma de codigo
    # el grupo IDENTICO (68 funciones) no es un problema de locales: todas
    # casan registro a registro y los offsets de marco tambien. Lo que baila
    # son los registros que sostienen direcciones y constantes hoisteadas, y
    # el mecanismo es el ORDEN DE SENTENCIAS: el original entrelaza las que
    # tocan un mismo global y nosotros las agrupamos.
    'IDENTICO': ['move_stmt', 'swap', 'rot', 'invert_if', 'empty_then',
                 'commute', 'case_rotate', 'case_order', 'empty_case',
                 'named_temp', 'anon_temp', 'split_arg', 'goto_inline',
                 'split_and', 'elseif', 'zero_if', 'while_for', 'const'],
}


def diagnose(unit, func, quiet=True):
    """-> dict con el veredicto de regmap y las locales culpables, o None."""
    try:
        import regmap
    except Exception:
        return None
    try:
        index = regmap.build_index(regmap.ORIG_DUMP, not quiet)
    except Exception as exc:
        return {'error': 'indice del volcado original: %s' % exc}
    key = None
    for nm in (func, func.split('::', 1)[-1]):
        if nm in index:
            key = nm
            break
    if key is None:
        tail = func.split('::')[-1]
        hits = [nm for nm in index if nm.split('::')[-1] == tail]
        if len(hits) == 1:
            key = hits[0]
    if key is None:
        return {'error': 'no esta en el volcado DWARF del original'}
    cands = index[key]
    same = [c for c in cands if c[0] == unit] or cands
    o_cu, o_off, o_sig_line = same[0]
    o_lines = regmap.read_block_at(regmap.ORIG_DUMP, o_off)
    m = regmap.RE_OPEN.match(o_sig_line)
    if not m:
        return {'error': 'firma del original no parseable'}
    o_sig = m.group('sig')
    try:
        nd = regmap.our_dump(unit, False, not quiet, None)
        nindex = regmap.build_index(nd, not quiet)
    except SystemExit as exc:
        return {'error': 'nuestro volcado: %s' % exc}
    nc = nindex.get(key)
    if not nc:
        return {'error': 'no aparece en NUESTRO volcado (inlineada o sin escribir)'}
    n_lines = regmap.read_block_at(nd, nc[0][1])
    m = regmap.RE_OPEN.match(nc[0][2])
    if not m:
        return {'error': 'firma nuestra no parseable'}
    a = regmap.analyze(o_sig, o_lines, m.group('sig'), n_lines)
    if a is None:
        return {'error': 'no he podido parsear uno de los dos bloques'}
    lab, det = regmap.verdict_of(a)
    only_o = [(sc, v) for (sc, v, w) in a['rows'] if v and not w]
    only_n = [(sc, w) for (sc, v, w) in a['rows'] if w and not v]
    ok, diff, _, _, mapping = regmap.tally(a)
    # las locales cuyo registro NO coincide: son el foco de una PERMUTACION o
    # un REPARTO (regmap ya sabe cual es cual, solo hay que preguntarselo)
    shifted = [(v[1], v[2] or '-', w[2] or '-')
               for (sc, v, w) in a['rows']
               if v and w and (v[2] or '-') != (w[2] or '-')]
    return {'verdict': lab, 'detail': det, 'key': key,
            'only_orig': only_o, 'only_ours': only_n, 'shifted': shifted,
            'moved': a['moved'], 'notes': a['notes'],
            'cycles': regmap.find_cycles(mapping), 'mapping': mapping,
            'ok': ok, 'diff': diff}


def plan_from(diag):
    """-> (lista ordenada de transformaciones, conjunto de nombres a enfocar).

    Las causas concretas mandan sobre el veredicto: si regmap nombra una local
    que sobra, la primera transformacion que se prueba es la que la quita."""
    if not diag or diag.get('error'):
        return None, set()
    plan, focus = [], set()

    def add(k):
        for t in CAUSE_PLAN.get(k, []):
            if t not in plan:
                plan.append(t)

    if diag['only_ours']:
        add('only_ours')
        focus |= set(v[1] for _, v in diag['only_ours'])
    if diag['only_orig']:
        add('only_orig')
        focus |= set(v[1] for _, v in diag['only_orig'])
        # el cuerpo no puede adivinar el tipo: se lo da el DWARF del original
        del HINT_LOCALS[:]
        HINT_LOCALS.extend((v[0], v[1]) for _, v in diag['only_orig'])
    for note in diag['notes']:
        if 'FALTA' in note:
            add('block_missing')
        elif 'SOBRA' in note:
            add('block_extra')
        else:
            add('block_missing')
            add('block_extra')
    if diag['moved']:
        add('moved')
        focus |= set(mv[1][1] for mv in diag['moved'])
    add(diag['verdict'])
    if not focus and diag.get('shifted'):
        # en PERMUTACION/REPARTO el objetivo son las locales cuyo registro baila:
        # acortar su rango de vida es el unico dial que queda
        focus |= set(n for n, _, _ in diag['shifted'])
    return plan, focus


def changed_region(a, b):
    """-> (trozo de a que cambia, trozo de b que cambia)."""
    i, n = 0, min(len(a), len(b))
    while i < n and a[i] == b[i]:
        i += 1
    j = 0
    while j < n - i and a[len(a) - 1 - j] == b[len(b) - 1 - j]:
        j += 1
    return a[i:len(a) - j], b[i:len(b) - j]


def touches(base, new, names):
    """True si la variante toca alguno de esos identificadores."""
    if not names:
        return True
    ra, rb = changed_region(base, new)
    seg = ra + '\x00' + rb
    return any(re.search(r'\b%s\b' % re.escape(x), seg) for x in names)


def gen_variants(body, only=None, skip=None):
    seen, out = set(), []
    for name, fn in TRANSFORMS:
        if only and name not in only:
            continue
        if skip and name in skip:
            continue
        try:
            cands = fn(body)
        except Exception as exc:                       # una forma rara no debe matar la busqueda
            cands = []
            sys.stderr.write('  (transform %s fallo: %s)\n' % (name, exc))
        for tag, new in cands:
            if new == body:
                continue
            h = hashlib.md5(new.encode('utf-8', 'replace')).hexdigest()
            if h in seen:
                continue
            seen.add(h)
            out.append(('%s:%s' % (name, tag), new))
    return out


# --------------------------------------------------------------------------
# 5. LOCALIZAR LA FUNCION EN LA FUENTE
# --------------------------------------------------------------------------

def find_function(text, qualified, which=1):
    """-> (inicio_cuerpo, fin_cuerpo) del `{...}` de la definicion."""
    cls, _, meth = qualified.rpartition('::')
    pat = re.compile(r'(^|[\s*&:~])%s\s*\(' % re.escape(qualified), re.M)
    db, dp, code = scan(text)
    for m in pat.finditer(text):
        i = m.end() - 1
        if not code[i] or db[i] > 2:      # 0 = fichero, 1-2 = namespace anidado
            continue
        depth, j = 0, i
        while j < len(text):
            if code[j] and text[j] == '(':
                depth += 1
            elif code[j] and text[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        k = j + 1
        while k < len(text) and (text[k].isspace() or not code[k] or
                                 text[k] in ':,' or text[k].isalnum() or
                                 text[k] in '_()'):
            if text[k] == '{':
                break
            if text[k] == ';':
                k = -1
                break
            k += 1
        if k < 0 or k >= len(text) or text[k] != '{':
            continue
        e = _stmt_end(text, k, code, db)
        if e:
            # un .cpp puede llevar un talon vacio ANTES de la definicion de
            # verdad (`void TrackStreamer::CheckLoadingBar() {}` y mas abajo la
            # buena): quedarse con el talon dejaba la busqueda con 0 variantes
            # y el mensaje "0 B de cuerpo".
            if text[k + 1:e - 1].strip():
                which -= 1
                if which <= 0:
                    return (k + 1, e - 1)
            empty = (k + 1, e - 1)
    return locals().get('empty')


def resolve_symbol(unit, name):
    """demangled -> simbolo mangled, usando el report de objdiff."""
    import fuse
    names = fuse.demangled(unit)
    if name in names:
        return name, names[name][1]
    cands = [(s, d, p) for s, (d, p) in names.items() if name in d]
    if not cands:
        cands = [(s, s, 0.0) for s in names if name in s]
    if not cands:
        sys.exit('no encuentro %s en el report de %s' % (name, unit))
    cands.sort(key=lambda c: len(c[1]))
    exact = [c for c in cands if c[1].split('(')[0] == name]
    c = (exact or cands)[0]
    return c[0], c[2]


# --------------------------------------------------------------------------
# 6. BUSQUEDA
# --------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('unit')
    ap.add_argument('function', nargs='?')
    ap.add_argument('--file', help='.cpp que contiene la funcion (por defecto se busca)')
    ap.add_argument('--symbol', help='simbolo mangled (por defecto se resuelve)')
    ap.add_argument('--overload', type=int, default=1,
                    help='la enesima definicion con ese nombre (sobrecargas)')
    ap.add_argument('-j', type=int, default=max(2, (os.cpu_count() or 4) - 2))
    ap.add_argument('--rounds', type=int, default=3)
    ap.add_argument('--only', help='lista de transformaciones separada por comas')
    ap.add_argument('--skip', help='lista de transformaciones a saltar')
    ap.add_argument('--pairs', action='store_true', help='probar combinaciones de dos')
    ap.add_argument('--limit', type=int, default=0, help='maximo de variantes por ronda')
    ap.add_argument('--dry', action='store_true', help='solo enumerar variantes')
    ap.add_argument('--score-only', action='store_true')
    ap.add_argument('--out', help='escribir el mejor cuerpo a este fichero')
    ap.add_argument('--seed', help='partir de este cuerpo en vez del de la fuente')
    ap.add_argument('--near', action='store_true',
                    help='listar los near-miss de la unidad y salir')
    # --- guia por diagnostico (regmap.py) ---
    ap.add_argument('--guided', action='store_true',
                    help='que regmap.py elija las transformaciones y el foco')
    ap.add_argument('--diag', action='store_true',
                    help='solo el diagnostico de regmap.py, sin compilar')
    ap.add_argument('--focus', help='solo variantes que toquen estos identificadores')
    # --- recocido ---
    ap.add_argument('--depth', type=int, default=1,
                    help='cambios simultaneos por variante (2 o 3 = recocido)')
    ap.add_argument('--combos', type=int, default=200,
                    help='con --depth>1, cuantas combinaciones sortear por ronda')
    ap.add_argument('--anneal', type=float, default=0.0, metavar='EPS',
                    help='aceptar pasos laterales que empeoren menos de EPS')
    ap.add_argument('--patience', type=int, default=4,
                    help='pasos laterales seguidos antes de rendirse')
    ap.add_argument('--floor', type=int, default=15,
                    help='si el plan de regmap da menos variantes, abrir el '
                         'catalogo entero (el foco las sigue ordenando)')
    ap.add_argument('--shuffle', action='store_true',
                    help='con --limit, sortear en vez de coger los primeros')
    ap.add_argument('--seed-rng', type=int, default=1234, dest='seed_rng')
    ap.add_argument('--sweep', action='store_true',
                    help='recorrer los near-miss de la unidad, uno por proceso')
    ap.add_argument('--lo', type=float, default=99.0, help='con --sweep, %% minimo')
    ap.add_argument('--hi', type=float, default=100.0, help='con --sweep, %% maximo')
    ap.add_argument('--timeout', type=int, default=900,
                    help='con --sweep, segundos por funcion')
    args = ap.parse_args()

    os.chdir(ROOT)
    if args.sweep:
        return sweep(args)
    if args.near or not args.function:
        import fuse
        rows = [(p, len(orig_function(args.unit, s) or []) * 4, d)
                for s, (d, p) in fuse.demangled(args.unit).items() if 0 < p < 100]
        rows.sort(key=lambda r: (-r[0], -r[1]))
        print('near-miss de %s (los mas cerca primero):' % args.unit)
        for p, b, d in rows[:40]:
            print('  %7.3f%%  %6d B  %s' % (p, b, d[:78]))
        return 0
    sym, pct = resolve_symbol(args.unit, args.function)
    if args.symbol:
        import fuse as _fuse
        _d = _fuse.demangled(args.unit).get(args.symbol)
        sym = args.symbol
        if _d:
            pct = _d[1]
    orig = orig_function(args.unit, sym)
    if orig is None:
        sys.exit('no encuentro %s en el asm de %s' % (sym, args.unit))

    # el nombre en la fuente puede llevar menos calificacion que el demangled
    # (namespace Speech { void Cache::FlushLRU() } -> Speech::Cache::FlushLRU)
    parts = args.function.split('::')
    names = ['::'.join(parts[i:]) for i in range(len(parts))]

    def locate(txt):
        for nm in names:
            sp = find_function(txt, nm, args.overload)
            if sp:
                return sp, nm
        return None, None

    path, srcname = args.file, args.function
    if not path:
        hits = subprocess.run(['git', 'grep', '-l', '-e', parts[-1], '--', 'src'],
                              cwd=ROOT, capture_output=True, text=True).stdout.split()
        hits = [h for h in hits if h.endswith('.cpp')]
        cache = {}
        # el nombre MAS calificado manda: si no, PauseMenu::NotificationMessage
        # acaba encontrando UIProfileManager::NotificationMessage.
        for nm in names:
            for h in hits:
                txt = cache.get(h)
                if txt is None:
                    txt = cache[h] = open(os.path.join(ROOT, h), encoding='utf-8',
                                          errors='replace').read()
                if find_function(txt, nm):
                    path, srcname = h, nm
                    break
            if path:
                break
    if not path:
        sys.exit('no encuentro la definicion de %s; usa --file' % args.function)
    path = os.path.join(ROOT, path)
    text = open(path, encoding='utf-8', errors='replace').read()
    span, srcname = locate(text)
    if not span:
        sys.exit('no encuentro el cuerpo de %s en %s' % (args.function, path))
    body = text[span[0]:span[1]]
    # la lista de inicializacion de un constructor NO vive dentro del `{...}`:
    # Sim::Entity::Entity no ofrecia ninguna variante porque sus 231
    # instrucciones salen integras de ella. Aqui se hace mutable tambien.
    init, pre_lo = '', span[0] - 1
    ilo = find_initlist(text, span[0])
    if ilo is not None:
        init, pre_lo = text[ilo:span[0] - 1], ilo
    if args.seed:
        blob = open(args.seed, encoding='utf-8', errors='replace').read()
        if blob.lstrip().startswith('@@INIT@@'):
            _, init, body = blob.split('@@INIT@@', 1)[1].split('@@BODY@@', 1)[0], \
                blob.split('@@INIT@@', 1)[1].split('@@BODY@@', 1)[0], \
                blob.split('@@BODY@@', 1)[1]
        else:
            body = blob

    print('%s  ->  %s' % (args.function, sym))
    print('  fichero : %s (%d B de cuerpo)' % (os.path.relpath(path, ROOT), len(body)))
    print('  original: %d instrucciones   objdiff: %.3f%%' % (len(orig), pct))
    if init.strip().startswith(':'):
        print('  init    : %d miembros en la lista de inicializacion'
              % len(split_initlist(init)))

    only, skip = _list(args.only), _list(args.skip)
    focus = set(_list(args.focus) or [])

    # --- el diagnostico manda ---------------------------------------------
    if args.guided or args.diag:
        diag = diagnose(args.unit, args.function, quiet=True) or {}
        if diag.get('error'):
            print('  regmap  : sin diagnostico (%s)' % diag['error'])
        else:
            print('  regmap  : %s -- %s' % (diag['verdict'], diag['detail']))
            for sc, v in diag['only_ours'][:6]:
                print('            sobra NUESTRA local %-18s (%s) %s'
                      % (v[1], v[2] or '-', sc))
            for sc, v in diag['only_orig'][:6]:
                print('            falta la local del ORIGINAL %-12s (%s) %s'
                      % (v[1], v[2] or '-', sc))
            for mv in diag['moved'][:6]:
                print('            %-18s cambia de ambito: %s -> %s'
                      % (mv[1][1], mv[0], mv[2]))
            for nm, ro, rn in diag.get('shifted', [])[:8]:
                print('            %-18s original=%-5s nuestro=%s' % (nm, ro, rn))
            for n in diag['notes'][:6]:
                print('            %s' % n[:88])
            plan, f2 = plan_from(diag)
            if plan and args.guided:
                only = plan if not only else [x for x in plan if x in only] or plan
                focus |= f2
                print('  plan    : %s' % ', '.join(only[:10]))
                if focus:
                    print('  foco    : %s' % ', '.join(sorted(focus)))
        if args.diag:
            return 0

    harness = Harness(args.unit, path)
    print('  prefijo : %d lineas del SourceList' % harness.nincludes)

    def make_source(new_init, new_body):
        return (text[:pre_lo] + new_init + '{' + new_body + '}'
                + text[span[1] + 1:])

    def evaluate(job):
        idx, tag, ni, nb = job
        ok, asm, err = harness.build(make_source(ni, nb), 'w%02d' % (idx % (args.j * 4)))
        if not ok:
            return (tag, None, err)
        return (tag, mine_function(asm, sym), '')

    t0 = time.time()
    _, base_asm, err = evaluate((0, 'BASE', init, body))
    if base_asm is None:
        print('\nLA BASE NO COMPILA:')
        lines = [l for l in err.split('\n') if 'error' in l.lower()]
        # ngccc no siempre dice "error": si no hay ninguna, ensena el final
        print('\n'.join((lines or [l for l in err.split('\n') if l.strip()][-15:])[:15]))
        return 1
    base_time = time.time() - t0
    best_rank = rank(base_asm, orig)
    f, o, first = score(base_asm, orig)
    print('  base    : %d instrucciones  score=%.6f  opcodes=%.6f  1a divergencia=%d'
          % (len(base_asm), f, o, first))
    print('  compilar: %.2f s por variante' % base_time)

    if args.score_only:
        show_diff(base_asm, orig)
        return 0

    def catalog(ci, cb):
        """-> [(tag, init, body)], las que tocan el foco delante.

        El foco ORDENA, no filtra: filtrar dejaba catalogos de 2 variantes y la
        busqueda se moria antes de empezar. Lo que si recorta de verdad es el
        plan de transformaciones (`only`), que sale del veredicto de regmap."""
        out = [(t, ci, nb) for t, nb in gen_variants(cb, only, skip)]
        out += gen_init_variants(ci, cb, only, skip)
        if only and len(out) < args.floor:
            # el plan de regmap ha salido demasiado estrecho (a veces 1 sola
            # variante): mejor abrir el catalogo entero que quedarse sin nada.
            extra = [(t, ci, nb) for t, nb in gen_variants(cb, None, skip)]
            extra += gen_init_variants(ci, cb, None, skip)
            have = set(t for t, _, _ in out)
            out += [x for x in extra if x[0] not in have]
        # PRIOR POSICIONAL: todo lo que hay antes de la primera divergencia ya
        # casa instruccion a instruccion. Una variante que solo toca esa parte
        # tiene mucho menos que aportar que una que toca de la divergencia en
        # adelante, asi que estas van primero. Es gratis y ordena bien cuando
        # regmap no nombra ninguna local (veredicto IDENTICO).
        cut = int(len(cb) * catalog.frac)

        def key(x):
            hot = bool(focus) and (touches(cb, x[2], focus)
                                   or touches(ci, x[1], focus))
            i, n = 0, min(len(cb), len(x[2]))
            while i < n and cb[i] == x[2][i]:
                i += 1
            return (0 if hot else 1, 0 if i >= cut else 1, i)

        out.sort(key=key)
        catalog.hot = sum(1 for x in out if key(x)[0] == 0)
        return out

    # la fraccion del cuerpo a partir de la cual conviene mutar: todo lo que
    # hay antes de la primera divergencia ya casa instruccion a instruccion
    catalog.frac = max(0.0, min(0.95, first / float(max(1, len(orig))) - 0.05))
    variants = catalog(init, body)
    print('  catalogo: %d variantes de un solo cambio%s'
          % (len(variants), (' (%d tocan el foco)' % catalog.hot)
             if getattr(catalog, 'hot', 0) else ''))
    if args.dry:
        from collections import Counter
        for k, v in Counter(t.split(':')[0] for t, _, _ in variants).most_common():
            print('     %-14s %d' % (k, v))
        return 0

    import random
    rng = random.Random(args.seed_rng)
    best_init, best_body, best_asm = init, body, base_asm
    cur_init, cur_body, cur_rank = init, body, best_rank
    total, t_start, stall = 0, time.time(), 0

    def run(cands, label):
        """Compila un lote y devuelve (mejor_de_la_tanda, r, asm) y cuenta."""
        nonlocal total
        print('\n--- %s: %d variantes ---' % (label, len(cands)))
        jobs = [(i, t, a, b) for i, (t, a, b) in enumerate(cands)]
        by_tag = {t: (a, b) for t, a, b in cands}
        top, bad = None, {}
        with ThreadPoolExecutor(max_workers=args.j) as ex:
            for tag, asm, err in ex.map(evaluate, jobs):
                total += 1
                if asm is None:
                    k = tag.split(':')[0].split('@')[0]
                    bad[k] = bad.get(k, 0) + 1
                    continue
                r = rank(asm, orig)
                if top is None or r > top[1]:
                    top = (tag, r, asm, by_tag[tag])
                    f_, o_, fi_ = score(asm, orig)
                    if r > best_rank:
                        # sin esto una ronda de 350 variantes no dice nada
                        # durante 15 minutos
                        print('  + %-44s score=%.6f (%d insn, div=%d)'
                              % (tag[:44], f_, len(asm), fi_))
                    if f_ >= 1.0:
                        break
        rate = total / max(1e-9, (time.time() - t_start)) * 60
        print('  %d variantes probadas  (%.0f/min)  %d no compilan %s'
              % (total, rate, sum(bad.values()),
                 ' '.join('%s=%d' % kv for kv in sorted(bad.items()))))
        return top

    for rnd in range(args.rounds):
        cands = catalog(cur_init, cur_body)
        if args.depth > 1:
            # RECOCIDO: variantes de 2 o 3 cambios simultaneos. El ascenso de
            # colina puro se para porque desde el 99,7% ningun cambio suelto
            # mejora; dos a la vez si pueden (uno paga lo que el otro cuesta).
            # Se combinan EMPALMANDO ediciones disjuntas del mismo cuerpo, no
            # regenerando el catalogo por combinacion: eso era O(combos x
            # catalogo) y en un cuerpo de 6 KB la primera ronda no llegaba a
            # arrancar. Aqui cada combinacion cuesta un empalme de cadenas.
            edits = []
            for (t, ci, cb) in cands:
                e = _edit_span(cur_body, cb)
                if e is not None and ci == cur_init:
                    edits.append((t, e))
            extra, seen = [], set()
            for _ in range(args.combos * 4):
                if len(extra) >= args.combos or len(edits) < 2:
                    break
                k = rng.randint(2, max(2, args.depth))
                pick = rng.sample(edits, min(k, len(edits)))
                pick.sort(key=lambda x: x[1][0])
                if any(pick[i][1][1] > pick[i + 1][1][0]
                       for i in range(len(pick) - 1)):
                    continue                      # se solapan: no son disjuntas
                nb, prev, tags = [], 0, []
                for t, (a, b, mid) in pick:
                    nb.append(cur_body[prev:a])
                    nb.append(mid)
                    prev = b
                    tags.append(t)
                nb.append(cur_body[prev:])
                nb = ''.join(nb)
                h = hashlib.md5(nb.encode('utf-8', 'replace')).hexdigest()
                if h in seen:
                    continue
                seen.add(h)
                extra.append(('+'.join(t[:20] for t in tags), cur_init, nb))
            cands = cands + extra
        elif args.pairs:
            extra = []
            for tag, ci, cb in cands[:60]:
                for t2, i2, b2 in catalog(ci, cb)[:12]:
                    extra.append((tag + '+' + t2, i2, b2))
            cands += extra
        if args.limit and len(cands) > args.limit:
            cands = rng.sample(cands, args.limit) if args.shuffle else cands[:args.limit]
        if not cands:
            print('  catalogo vacio; paro')
            break
        top = run(cands, 'ronda %d' % (rnd + 1))
        if top is None:
            print('  nada compila; paro')
            break
        tag, r, asm, (ni, nb) = top
        f, o, fi = score(asm, orig)
        if r > best_rank:
            best_rank, best_init, best_body, best_asm = r, ni, nb, asm
            cur_init, cur_body, cur_rank, stall = ni, nb, r, 0
            print('  = me quedo con %s' % tag[:56])
            if f >= 1.0:
                print('\n*** IDENTICO ***')
                break
        # rank() devuelve una TUPLA, asi que el margen se compara sobre su
        # primer componente (la suma alineadas+opcodes), no sobre la tupla.
        elif args.anneal and r[0] > cur_rank[0] - args.anneal:
            # aceptar un paso lateral (o levemente peor) para salir del valle
            stall += 1
            cur_init, cur_body, cur_rank = ni, nb, r
            print('  ~ paso lateral %-32s score=%.6f (mejor sigue %.6f)'
                  % (tag[:32], f, score(best_asm, orig)[0]))
            if stall >= args.patience:
                print('  %d pasos laterales sin mejorar; paro' % stall)
                break
        else:
            print('  ninguna mejora; paro')
            break

    f, o, first = score(best_asm, orig)
    print('\nresultado: score=%.6f opcodes=%.6f 1a divergencia=%d  (%d insn vs %d)'
          % (f, o, first, len(best_asm), len(orig)))
    nlc = sum(1 for x in best_asm if 'LC' in x)
    if nlc:
        print('AVISO: %d instrucciones referencian rodata anonima ($LCnnn contra'
              ' lbl_XXXXXXXX);\n       el valor NO se compara: verifica siempre'
              ' con objdiff report generate.' % nlc)
    if (best_body, best_init) != (body, init):
        outp = args.out or os.path.join(SCRATCH, 'best_%s.cpp' % sym[:40])
        open(outp, 'w', encoding='utf-8').write(
            '@@INIT@@' + best_init + '@@BODY@@' + best_body)
        print('mejor cuerpo -> %s' % outp)
        if best_init != init:
            print('  lista de init: %s' % best_init.strip()[:200])
    show_diff(best_asm, orig)
    return 0


def _edit_span(base, new):
    """-> (a, b, texto) tal que base[:a] + texto + base[b:] == new, o None.

    Es la region que cambia una transformacion. Dos transformaciones cuyas
    regiones no se solapan se pueden aplicar A LA VEZ empalmando: eso da los
    cambios simultaneos del recocido sin regenerar el catalogo por combinacion.
    """
    if base == new:
        return None
    i, n = 0, min(len(base), len(new))
    while i < n and base[i] == new[i]:
        i += 1
    j = 0
    while j < n - i and base[len(base) - 1 - j] == new[len(new) - 1 - j]:
        j += 1
    return (i, len(base) - j, new[i:len(new) - j])


def sweep(args):
    """Recorre los near-miss de una unidad con la busqueda guiada.

    Un proceso por funcion: si una revienta o se cuelga, la tanda sigue. Al
    final lista las que llegaron a score 1.0, que son las que hay que aplicar
    (y verificar SIEMPRE con `objdiff report generate`)."""
    import fuse
    rows = [(p, d) for s, (d, p) in fuse.demangled(args.unit).items()
            if args.lo <= p < args.hi]
    rows.sort(reverse=True)
    print('%s: %d funciones entre %.1f%% y %.1f%%' % (args.unit, len(rows),
                                                      args.lo, args.hi))
    won, res = [], []
    for k, (p, dem) in enumerate(rows):
        name = dem.split('(')[0].strip()
        cmd = [sys.executable, os.path.join(ROOT, 'scripts', 'permuter.py'),
               args.unit, name, '--guided', '-j', str(args.j),
               '--rounds', str(args.rounds), '--depth', str(args.depth)]
        if args.anneal:
            cmd += ['--anneal', str(args.anneal)]
        if args.limit:
            cmd += ['--limit', str(args.limit)]
        print('\n%s\n[%d/%d] %7.3f%%  %s' % ('=' * 74, k + 1, len(rows), p, name))
        t0 = time.time()
        try:
            q = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                               timeout=args.timeout)
            out = q.stdout or ''
        except subprocess.TimeoutExpired as exc:
            out = (exc.stdout or b'').decode('utf-8', 'replace') \
                if isinstance(exc.stdout, bytes) else (exc.stdout or '')
            out += '\n[TIMEOUT %ds]' % args.timeout
        keep = [l for l in out.split('\n')
                if l.startswith(('  regmap', '  plan', '  foco', '  base',
                                 '  catalogo', '  + ', '  = ', '  ~ ',
                                 'resultado', '*** IDENT', 'mejor cuerpo',
                                 '[TIMEOUT', 'LA BASE NO COMPILA',
                                 'no encuentro', 'unidad desconocida'))]
        print('\n'.join(keep[:18]))
        fin = [l for l in out.split('\n') if l.startswith('resultado:')]
        sc = 0.0
        if fin:
            m = re.search(r'score=([\d.]+)', fin[0])
            sc = float(m.group(1)) if m else 0.0
        res.append((sc, p, name, time.time() - t0))
        if sc >= 1.0:
            won.append(name)
            print('  >>> CIERRA: %s' % name)
    print('\n%s\nRESUMEN %s' % ('=' * 74, args.unit))
    for sc, p, name, dt in sorted(res, reverse=True):
        print('  %s %.6f  (objdiff %7.3f%%)  %5.0fs  %s'
              % ('OK ' if sc >= 1.0 else '   ', sc, p, dt, name[:60]))
    print('  %d de %d llegan a 1.0' % (len(won), len(res)))
    return 0


def find_initlist(text, body_lo):
    """-> indice del `:` que abre la lista de inicializacion, o None.

    `body_lo` es el primer caracter DENTRO del `{`, asi que text[body_lo-1]=='{'.
    """
    db, dp, code = scan(text)
    i = body_lo - 2
    depth = 0
    while i >= 0:
        c = text[i]
        if not code[i]:
            i -= 1
            continue
        if c in ')]':
            depth += 1
        elif c in '([':
            depth -= 1
            if depth < 0:
                return None
        elif depth == 0 and c == ':' and text[i - 1:i] != ':' and text[i + 1:i + 2] != ':':
            return i
        elif depth == 0 and c in ';}{':
            return None
        i -= 1
    return None


def _list(s):
    return [x.strip() for x in s.split(',')] if s else None


def show_diff(mine, orig, ctx=3):
    if mine == orig:
        print('  *** IDENTICO (score 1.0) ***')
        return
    n = 0
    for l in difflib.unified_diff(mine, orig, 'mias', 'original', lineterm='', n=ctx):
        print('  ' + l)
        n += 1
        if n > 80:
            print('  ...')
            break


if __name__ == '__main__':
    sys.exit(main())
