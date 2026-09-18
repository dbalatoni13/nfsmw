#!/usr/bin/env python3
"""nombra_por_codigo.py <version> [unidad...] [--aplicar] -- recupera nombres de funciones de un ejecutable sin
simbolos (Xbox 360 o PC) comparando, unidad a unidad, el objeto del original troceado (build/<V>/obj) con nuestro
objeto compilado (build/<V>/src). Una funcion del original sin nombre (fn_/sub_...) recibe el nombre de una nuestra
cuando sus bytes coinciden entera y unicamente (en las palabras/bytes con reubicacion en cualquiera de los dos lados
solo se exige el codigo de operacion en PowerPC y se ignoran los 4 bytes en x86).

Solo informa; con --aplicar escribe los nombres en config/<V>/symbols.txt (Xbox 360). Formato COFF (MSVC)."""
import collections
import glob
import os
import re
import struct
import sys

args = [a for a in sys.argv[1:] if not a.startswith('--')]
VERSION = args[0]
UNIDADES = args[1:]
APLICAR = '--aplicar' in sys.argv
MIN_PALABRAS = 5
SIN_NOMBRE = re.compile(r'^(fn|sub|func|lbl|zz|j_sub|nullsub)_[0-9A-Fa-f_]+$')


class Coff:
    def __init__(self, ruta):
        d = open(ruta, 'rb').read()
        self.d = d
        maquina, nsec, _, psym, nsym, opt, _ = struct.unpack_from('<HHIIIHH', d, 0)
        self.maquina = maquina
        self.ppc = maquina == 0x01F2
        self.secciones = []
        for i in range(nsec):
            o = 20 + opt + i * 40
            nombre, vsz, va, rsz, praw, prel, plin, nrel, nlin, car = struct.unpack_from('<8sIIIIIIHHI', d, o)
            relocs = []
            for k in range(nrel):
                ra, rsym, rtipo = struct.unpack_from('<IIH', d, prel + k * 10)
                relocs.append((ra, rsym, rtipo))
            self.secciones.append({'nombre': nombre.rstrip(b'\0').decode('latin-1'), 'tam': rsz, 'praw': praw,
                                   'relocs': relocs, 'car': car})
        cad_off = psym + nsym * 18
        self.simbolos = []
        i = 0
        while i < nsym:
            o = psym + i * 18
            nombre = d[o:o + 8]
            if nombre[:4] == b'\0\0\0\0':
                off = struct.unpack_from('<I', nombre, 4)[0]
                fin = d.index(b'\0', cad_off + off)
                nombre = d[cad_off + off:fin]
            else:
                nombre = nombre.rstrip(b'\0')
            valor, sec, tipo, clase, naux = struct.unpack_from('<IhHBB', d, o + 8)
            self.simbolos.append((i, nombre.decode('latin-1'), valor, sec, tipo, clase))
            i += 1 + naux
        self.por_indice = {s[0]: s for s in self.simbolos}

    def datos(self, sec):
        s = self.secciones[sec - 1]
        return self.d[s['praw']:s['praw'] + s['tam']]

    def funciones(self):
        """[(nombre, bytes, offsets reubicados relativos)] de las funciones con codigo."""
        por_sec = collections.defaultdict(list)
        for idx, nombre, valor, sec, tipo, clase in self.simbolos:
            if sec <= 0 or clase not in (2, 3):
                continue
            s = self.secciones[sec - 1]
            if not (s['car'] & 0x20):  # IMAGE_SCN_CNT_CODE
                continue
            if clase == 3 and valor == 0 and nombre.startswith('.'):
                continue  # simbolo de seccion
            por_sec[sec].append((valor, nombre))
        out = []
        for sec, lista in por_sec.items():
            lista.sort()
            datos = self.datos(sec)
            relocs = sorted((r[0], self.por_indice.get(r[1], (0, ''))[1]) for r in self.secciones[sec - 1]['relocs'])
            for k, (valor, nombre) in enumerate(lista):
                fin = lista[k + 1][0] if k + 1 < len(lista) else len(datos)
                if fin <= valor:
                    continue
                rel = [(r - valor, destino) for r, destino in relocs if valor <= r < fin]
                out.append((nombre, datos[valor:fin], rel, (sec, valor)))
        return out


def destinos(rel, ppc):
    """{indice de palabra (PPC) o byte (x86): nombre del destino} para los destinos con nombre real."""
    out = {}
    for r, destino in rel:
        if not destino or destino.startswith(('.', '$', 'lbl_')) or SIN_NOMBRE.match(destino):
            continue
        out[r // 4 if ppc else r] = destino
    return out


def normaliza(b, rel, ppc):
    """Tupla comparable: en PPC, palabras con las reubicadas reducidas a su codigo de operacion; en x86, bytes con los
    4 de cada reubicacion puestos a None."""
    if ppc:
        n = len(b) // 4
        palabras = list(struct.unpack('>%dI' % n, b[:n * 4]))
        marcadas = {r // 4 for r, _ in rel}
        return tuple((w >> 26, None) if i in marcadas else (w, 0) for i, w in enumerate(palabras)), marcadas
    lista = list(b)
    marcadas = set()
    for r, _ in rel:
        for j in range(r, min(r + 4, len(lista))):
            marcadas.add(j)
    return tuple(None if i in marcadas else x for i, x in enumerate(lista)), marcadas


def coinciden(a, b, ma, mb, ppc, da=None, db=None):
    if len(a) != len(b):
        return False
    if da and db:
        for k, nombre in da.items():
            if k in db and db[k] != nombre:
                return False
    if ppc:
        for i, (x, y) in enumerate(zip(a, b)):
            if i in ma or i in mb:
                if (x[0] if i in ma else x[0] >> 26) != (y[0] if i in mb else y[0] >> 26):
                    return False
            elif x != y:
                return False
        return True
    for i, (x, y) in enumerate(zip(a, b)):
        if i in ma or i in mb:
            continue
        if x != y:
            return False
    return True


def unidades_de(version):
    base = 'build/%s/obj' % version
    for p in glob.glob(base + '/**/*.obj', recursive=True):
        rel = os.path.relpath(p, base).replace(os.sep, '/')
        yield rel


propuestas = {}
evidencia = {}
en_orden = set()
resumen = []
ya_nombrados = set()
if os.path.exists('config/%s/symbols.json' % VERSION):
    import json
    for f in json.load(open('config/%s/symbols.json' % VERSION, encoding='utf-8'))['functions']:
        ya_nombrados.add(f['name'])
if os.path.exists('config/%s/symbols.txt' % VERSION):
    for l in open('config/%s/symbols.txt' % VERSION, encoding='utf-8', errors='replace'):
        m = re.match(r'(\S+) = ', l)
        if m:
            ya_nombrados.add(m.group(1))
for rel in sorted(unidades_de(VERSION)):
    if UNIDADES and not any(u in rel for u in UNIDADES):
        continue
    objetivo = 'build/%s/obj/%s' % (VERSION, rel)
    nuestro = 'build/%s/src/%s' % (VERSION, rel)
    if not os.path.exists(nuestro):
        continue
    t = Coff(objetivo)
    n = Coff(nuestro)
    ppc = t.ppc
    nuestras = []
    for nombre, b, rel_, orden in n.funciones():
        # `$L121558` y `.L22802` no son nombres de funcion: son etiquetas internas que emite el
        # compilador (tablas de saltos, destinos de bucle) y que acaban en la tabla de simbolos del
        # objeto. Ponerle una a una funcion del objetivo ROMPE el emparejamiento de objdiff: el
        # informe entero de la version falla con "Failed to find right side symbol for paired left
        # side symbol", y falla en silencio, porque el objeto solo cambia al volver a trocear.
        if nombre.startswith(('$L', '.L', '$LN', '__LINE__')):
            continue
        clave, marcas = normaliza(b, rel_, ppc)
        if len(clave) >= MIN_PALABRAS * (1 if ppc else 4):
            nuestras.append((nombre, clave, marcas, destinos(rel_, ppc), orden))
    por_tam = collections.defaultdict(list)
    for x in nuestras:
        por_tam[len(x[1])].append(x)
    nombres_objetivo = {s[1] for s in t.simbolos}
    candidatas = 0
    halladas = 0
    en_unidad = []
    orden_nuestro = {x[0]: x[4] for x in nuestras}
    for nombre, b, rel_, orden_t in t.funciones():
        if not SIN_NOMBRE.match(nombre):
            # ancla: funcion que ya tiene el mismo nombre en los dos lados
            if nombre in orden_nuestro:
                en_unidad.append((orden_t, orden_nuestro[nombre], None))
            continue
        clave, marcas = normaliza(b, rel_, ppc)
        if len(clave) < MIN_PALABRAS * (1 if ppc else 4):
            continue
        candidatas += 1
        dt = destinos(rel_, ppc)
        iguales = [x[0] for x in por_tam.get(len(clave), []) if coinciden(clave, x[1], marcas, x[2], ppc, dt, x[3])]
        iguales = [x for x in set(iguales) if x not in nombres_objetivo and x not in ya_nombrados]
        if len(iguales) == 1:
            halladas += 1
            propuestas.setdefault(nombre, set()).add(iguales[0])
            suya = [x for x in por_tam.get(len(clave), []) if x[0] == iguales[0]][0]
            comunes = sum(1 for k, v in dt.items() if suya[3].get(k) == v)
            evidencia[nombre] = (len(clave), comunes)
            en_unidad.append((orden_t, suya[4], nombre))
    # orden: las propuestas buenas crecen a la vez en el original y en nuestro objeto (subsecuencia creciente
    # mas larga); las que quedan fuera solo valen si ademas coinciden en un destino con nombre
    en_unidad.sort()
    colas, padre, idx_colas = [], [None] * len(en_unidad), []
    import bisect
    for i, (_, o, _) in enumerate(en_unidad):
        j = bisect.bisect_left(colas, o)
        if j == len(colas):
            colas.append(o)
            idx_colas.append(i)
        else:
            colas[j] = o
            idx_colas[j] = i
        padre[i] = idx_colas[j - 1] if j > 0 else None
    k = idx_colas[-1] if idx_colas else None
    while k is not None:
        if en_unidad[k][2] is not None:
            en_orden.add(en_unidad[k][2])
        k = padre[k]
    resumen.append((rel, candidatas, halladas, len(nuestras)))

for rel, c, h, nn in resumen:
    print('%-55s sin nombre %5d  nombradas %5d  (nuestras %d)' % (rel, c, h, nn))
# un mismo nombre propuesto para varias direcciones, o varias propuestas para una: se descartan
uso = collections.Counter(next(iter(v)) for v in propuestas.values() if len(v) == 1)
finales = {k: next(iter(v)) for k, v in propuestas.items() if len(v) == 1 and uso[next(iter(v))] == 1}
fuera = [k for k in finales if k not in en_orden and evidencia[k][1] == 0]
print('fuera de orden y sin destino coincidente (descartadas):', len(fuera))
for k in fuera:
    del finales[k]
print('propuestas unicas:', len(finales), 'de', len(propuestas))
con_destino = sum(1 for k in finales if evidencia[k][1] > 0)
print('  con al menos un destino con nombre coincidente:', con_destino)
tams = sorted(evidencia[k][0] for k in finales)
if tams:
    print('  tamano (palabras/bytes): min %d, mediana %d, max %d' % (tams[0], tams[len(tams) // 2], tams[-1]))
for k in list(finales)[:15]:
    print('  ', k, '->', finales[k])

if APLICAR and finales and os.path.exists('config/%s/symbols.json' % VERSION):
    # PC: exportacion de IDA; se cambia el campo name de cada funcion, con sustitucion exacta de texto
    ruta = 'config/%s/symbols.json' % VERSION
    b = open(ruta, 'rb').read().decode('utf-8')
    cambios = 0
    for viejo, nuevo in finales.items():
        antes = '"name": "%s",' % viejo
        if b.count(antes) == 1:
            b = b.replace(antes, '"name": "%s",' % nuevo)
            cambios += 1
    open(ruta, 'wb').write(b.encode('utf-8'))
    print('aplicados', cambios)
elif APLICAR and finales:
    ruta = 'config/%s/symbols.txt' % VERSION
    t = open(ruta, encoding='utf-8').read()
    cambios = 0
    lineas = t.split('\n')
    for i, l in enumerate(lineas):
        m = re.match(r'(\S+)( = .*)$', l)
        if m and m.group(1) in finales:
            lineas[i] = finales[m.group(1)] + m.group(2)
            cambios += 1
    open(ruta, 'w', encoding='utf-8', newline='\n').write('\n'.join(lineas))
    print('aplicados', cambios)
