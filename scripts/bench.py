#!/usr/bin/env python3
"""bench.py -- el ciclo de trabajo en 1,4 s en vez de 34.

El coste real de una iteracion, cronometrado sobre zFe2:

    build_direct  22.880 ms      <-- el 67% del bucle
    measure        4.118 ms
    fndiff         7.329 ms

zFe2 mete **71 ficheros** en una sola unidad de traduccion, asi que cambiar UNA
linea recompila los 71. A 200 iteraciones son casi dos horas solo de compilador,
que es exactamente lo que tardan los agentes.

Este banco compila el prologo de la SourceList mas **el minimo prefijo de
ficheros que hace falta**, con los cflags EXACTOS de la unidad. La trampa que
descubrio un agente: algunos ficheros necesitan a un hermano anterior porque
alli viven los inlines (`FnDeltaQFast` necesita `FnDeltaQ`, 2.732 contra
2.800 B). Por eso el banco **verifica la fidelidad** comparando el tamano de
cada funcion contra el `.o` real de la unidad, y si no cuadra **anade ficheros
anteriores hacia atras hasta que cuadre**.

MEDIDO de punta a punta sobre `eagl4supportdlopen.cpp` de zEagl4Anim:

    compilar el banco    564 ms
    medir con objdiff    856 ms
    ---------------------------
    ciclo completo     1.420 ms   contra 34.300 ms   ->  **24 veces mas rapido**

La clave de la medida: **objdiff empareja por NOMBRE**, asi que le da igual que
el banco tenga 20 simbolos y la unidad 1.100. El porcentaje coincide con el de
la unidad entera **al quinto decimal**.

    python scripts/bench.py zFe2 FeHudElement.cpp           # prepara y verifica
    python scripts/bench.py zFe2 FeHudElement.cpp --medir   # + %, tamano y diffs
    python scripts/bench.py zFe2 FeHudElement.cpp --medir --fn Render   # una sola
    python scripts/bench.py zFe2 FeHudElement.cpp --asm     # y volcar el .s

Edita el `.cpp` REAL del arbol; el banco lo incluye, no lo copia. **Y las
CABECERAS se barren copiandolas al scratchpad y sustituyendo el `#include` del
banco: cero riesgo para los demas agentes**, que es lo que antes obligaba a
editar el arbol en bucle y tiraba las tandas de los vecinos.

Aviso: `__static_initialization_and_destruction_0` casi nunca es fiable en el
banco (depende de TODOS los ficheros de la unidad). El script lo nombra.
"""
import os
import re
import struct
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
SCR = os.environ.get(
    "SCRATCH",
    "C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/"
    "3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad")

sys.path.insert(0, os.path.join(ROOT, "scripts"))
import build_direct  # noqa: E402  -- reutiliza su lectura de build.ninja


def prologo_e_includes(unit):
    """(prologo, [rutas incluidas en orden], epilogo).

    El epilogo importa: en `zPlatform` los `#include` van **dentro de un
    `#ifdef EA_PLATFORM_GAMECUBE`**, asi que el prologo arrastra el condicional
    abierto y el banco no compilaba. Devolviendo tambien la cola —todo lo que hay
    tras el ULTIMO `#include`, que es donde vive el `#endif`— el banco queda
    balanceado sea cual sea la estructura de la SourceList.
    """
    p = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists", unit + ".cpp")
    txt = open(p, encoding="utf-8", errors="replace").read()
    incs = re.findall(r'^\s*#include\s+"([^"]+)"', txt, re.M)
    i = txt.find('#include "')
    j = txt.rfind('#include "')
    cola = ""
    if j != -1:
        fin = txt.find("\n", j)
        if fin != -1:
            cola = txt[fin + 1:]
    return (txt[:i] if i > 0 else ""), incs, cola


def funcs_de_o(path):
    """{nombre: tamano} de las funciones de un ELF."""
    try:
        d = open(path, "rb").read()
    except IOError:
        return {}
    if d[:4] != b"\x7fELF":
        return {}
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    sts = [x for x in secs if x[1] == 2]
    if not sts:
        return {}
    st = sts[0]
    strt = secs[st[6]]
    out = {}
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        if (info & 0xF) != 2 or not size:      # STT_FUNC con tamano
            continue
        o2 = strt[4] + nameo
        out[d[o2:d.index(bytes([0]), o2)].decode("utf-8", "replace")] = size
    return out


def compila(unit, banco, solo_s=False):
    """Compila el banco con los cflags EXACTOS de la unidad. -> (ok, salida, ms)."""
    units = build_direct.parse_units()
    if unit not in units:
        return False, "unidad %s no esta en build.ninja" % unit, 0
    src, out, cflags, toolchain, rename = units[unit][:5]
    # misma invocacion que build_direct: ngccc con SN_NGC_PATH apuntando a su carpeta
    cc = os.path.join(ROOT, "build", "compilers", toolchain, "ngccc.exe")
    env = dict(os.environ)
    env["SN_NGC_PATH"] = os.path.join(ROOT, "build", "compilers", toolchain)
    dest = os.path.join(SCR, "bench_%s.s" % unit) if solo_s else \
        os.path.join(SCR, "bench_%s.o" % unit)
    cmd = [cc] + list(cflags) + (["-S"] if solo_s else ["-c"]) + ["-o", dest, banco]
    t0 = time.time()
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT, env=env)
    ms = int((time.time() - t0) * 1000)
    if r.returncode != 0:
        return False, (r.stderr or r.stdout)[-1500:], ms
    # zFeOverlay y zOnline viven en .over, no en .text. Sin renombrar la seccion
    # objdiff no empareja NADA y el banco entero mide 0,0000% con 0 diffs, que es
    # exactamente la trampa que el playbook ya documenta para build_direct.
    if not solo_s and rename:
        rr = subprocess.run([sys.executable, os.path.join("tools", "rename_section.py"),
                             "-q", dest, rename],
                            capture_output=True, text=True, cwd=ROOT)
        if rr.returncode != 0:
            return False, "rename_section: " + (rr.stderr or rr.stdout), ms
    return True, dest, ms


def mide(unit, banco_o, filtro=None):
    """%, tamano y diffs de cada funcion del banco, con objdiff contra el objetivo.

    La clave, medida: **objdiff empareja por NOMBRE**, asi que le da igual que el
    banco tenga menos simbolos que la unidad. Da el porcentaje real y coincide con
    la unidad entera **al quinto decimal**, en 2-3 s en vez de los 34 s del ciclo
    build+measure+fndiff. Y como el banco vive en el scratchpad, las cabeceras se
    barren copiandolas y sustituyendo el `#include`: **cero riesgo para los demas
    agentes**.
    """
    tgt = os.path.join(ROOT, "build", "GOWE69", "obj", "Speed", "Indep",
                       "SourceLists", unit + ".o")
    if not os.path.exists(tgt):
        return None
    out = os.path.join(SCR, "bench_%s_diff.json" % unit)
    cli = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")
    r = subprocess.run([cli, "diff", "-1", tgt, "-2", banco_o,
                        "-c", "function_reloc_diffs=none",
                        "-c", "ppc.calculatePoolRelocations=false",
                        "-o", out, "--format", "json", "zz"],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0 or not os.path.exists(out):
        return None
    import json
    d = json.load(open(out))
    try:
        os.remove(out)                    # borrar YA: estos volcados llenan el disco
    except OSError:
        pass
    der = {s["name"]: s for s in d["right"]["symbols"]}
    filas = []
    for s in d["left"]["symbols"]:
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        n = s["name"]
        if n not in der:
            continue                       # no esta en el banco: normal
        if filtro and filtro not in n:
            continue
        ndiff = sum(1 for e in (der[n].get("instructions") or [])
                    if (e.get("diff_kind") or "DIFF_NONE") != "DIFF_NONE")
        filas.append((int(s.get("size") or 0), s.get("match_percent") or 0.0, ndiff, n))
    filas.sort(reverse=True)
    return filas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    if len(args) < 2:
        print(__doc__)
        return
    unit, objetivo = args[0], args[1]
    solo_s = "--asm" in sys.argv
    rapido = "-c" in sys.argv
    medir = "--medir" in sys.argv
    filtro = None
    if "--fn" in sys.argv:
        filtro = sys.argv[sys.argv.index("--fn") + 1]

    pro, incs, cola = prologo_e_includes(unit)
    idx = [i for i, x in enumerate(incs) if x.endswith("/" + objetivo) or x == objetivo]
    if not idx:
        print("no encuentro %s entre los %d includes de %s" % (objetivo, len(incs), unit))
        cand = [x for x in incs if objetivo.split("/")[-1].lower() in x.lower()]
        for c in cand[:8]:
            print("   parecido:", c)
        return
    n = idx[0]
    print("%s es el fichero %d de %d en %s" % (objetivo, n + 1, len(incs), unit))

    real = funcs_de_o(os.path.join(ROOT, "build", "GOWE69", "src", "Speed", "Indep",
                                   "SourceLists", unit + ".o"))
    if not real:
        print("aviso: no puedo leer el .o de la unidad; no habra verificacion")

    banco = os.path.join(SCR, "bench_%s.cpp" % unit)
    # arranca con SOLO el fichero objetivo y va anadiendo anteriores hasta que
    # los tamanos de funcion cuadren con el .o real
    # La COLA (todo lo que va tras el ULTIMO #include) a veces depende de
    # ficheros POSTERIORES al objetivo: en zAI son IMPLEMENT_SINGLETON(Gps),
    # AIPerpVehicle::mStagger o gHeliVehicle, que viven en los ficheros 21..37.
    # Con ella puesta NINGUN prefijo compila y el banco se daba por imposible
    # para los 37 ficheros de la unidad. Se reintenta SIN ella: no aporta nada
    # a la medida de las funciones del fichero objetivo.
    intentos = [(a, cola) for a in (0, 1, 2, 4, 8, 16, n)]
    if cola.strip():
        intentos += [(a, "") for a in (0, 1, 2, 4, 8, 16, n)]
    culpa_cola = [False]
    for k, (atras, cola_i) in enumerate(intentos):
        if cola_i and culpa_cola[0]:
            continue          # ya sabemos que el culpable es la cola: no gastes compilados
        desde = max(0, n - atras)
        with open(banco, "w", encoding="utf-8", newline="\n") as f:
            f.write(pro)
            for x in incs[desde:n + 1]:
                f.write('#include "%s"\n' % x)
            f.write(cola_i)    # el #endif de las SourceLists con los include en un #ifdef
        ok, res, ms = compila(unit, banco, solo_s=False)
        if not ok:
            # Si el error apunta al PROPIO banco --y no a un .cpp del arbol-- el
            # culpable es la cola de la SourceList, no el prefijo: no tiene
            # sentido probar los otros seis prefijos CON ella.
            if cola_i and os.path.basename(banco) in res:
                culpa_cola[0] = True
            if k == len(intentos) - 1:
                print("no compila ni con el prefijo entero:")
                print(res)
                return
            continue
        if cola_i != cola:
            print("   (la cola de la SourceList necesita ficheros posteriores:"
                  " banco montado SIN ella)")
        mias = funcs_de_o(res)
        comunes = [k for k in mias if k in real]
        malas = [k for k in comunes if mias[k] != real[k]]
        print("   prefijo de %d fichero(s): compila en %d ms, %d funciones, %d con tamano distinto"
              % (n + 1 - desde, ms, len(comunes), len(malas)))
        # Criterio: el banco es util si el 95% de las funciones cuadra. Exigir
        # CERO desajustes lo rechazaba siempre por una sola funcion --normalmente
        # un `static` o un inline que depende de un fichero posterior-- y hacia
        # caer al `build_direct` de 23 s sin necesidad. Las que no cuadran se
        # nombran para que no se midan en el banco.
        if comunes and len(malas) * 20 <= len(comunes):
            if malas:
                print("   NO fiables en el banco (%d): %s"
                      % (len(malas), ", ".join(m[:60] for m in malas[:4])))
            print()
            print("BANCO FIEL: %s" % banco)
            print("   %d ficheros en vez de %d   ->   %d ms por variante" % (n + 1 - desde, len(incs), ms))
            if solo_s:
                ok2, res2, ms2 = compila(unit, banco, solo_s=True)
                print("   asm en %s (%d ms)" % (res2, ms2))
            if medir:
                t0 = time.time()
                filas = mide(unit, res, filtro)
                dt = int((time.time() - t0) * 1000)
                if filas is None:
                    print("   (no pude medir con objdiff)")
                else:
                    print()
                    print("   medido con objdiff en %d ms  ->  ciclo completo %d ms"
                          % (dt, ms + dt))
                    print("   %8s %9s %7s  %s" % ("bytes", "%", "diffs", "funcion"))
                    for sz, pct, nd, nm in filas[:25]:
                        print("   %8d %8.4f%% %7d  %s" % (sz, pct, nd, nm[:78]))
            return
        if rapido:
            return
    print("no consegui un prefijo fiel; usa build_direct para esta unidad")


if __name__ == "__main__":
    main()
