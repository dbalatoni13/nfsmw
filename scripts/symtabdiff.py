#!/usr/bin/env python3
"""symtabdiff.py -- diff de TABLAS DE SIMBOLOS ELF entre el .o objetivo y el nuestro.

Los simbolos DEFINIDOS que le faltan al nuestro senalan la raiz de las
instanciaciones de plantilla apagadas: si Listable<T,N>::_mTable queda UNDEF
(o en COMMON), GCC 2.9 no emite ni la vtable ni ~List/GetGrowSize/
GetMaxCapacity/AllocVectorSpace de esa instanciacion.

  python scripts/symtabdiff.py                 # mapa de TODAS las unidades
  python scripts/symtabdiff.py zAI             # detalle de una unidad
  python scripts/symtabdiff.py zAI --all       # incluye funciones, no solo objetos
  python scripts/symtabdiff.py --csv           # resumen en csv
"""
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
TGT = os.path.join(ROOT, "build", "GOWE69", "obj", "Speed", "Indep", "SourceLists")
OURS = os.path.join(ROOT, "build", "GOWE69", "src", "Speed", "Indep", "SourceLists")

STT = {0: "NOTYPE", 1: "OBJECT", 2: "FUNC", 3: "SECTION", 4: "FILE", 5: "COMMON"}
STB = {0: "LOCAL", 1: "GLOBAL", 2: "WEAK"}
SHN_UNDEF, SHN_COMMON = 0, 0xFFF2


def load(path):
    d = open(path, "rb").read()
    assert d[:4] == b"\x7fELF", path
    E = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(E + "I", d[0x20:0x24])
    shentsize, shnum, shstrndx = struct.unpack(E + "HHH", d[0x2E:0x34])
    secs = []
    for i in range(shnum):
        o = shoff + i * shentsize
        name, typ, flags, addr, off, size, link, info, align, entsize = struct.unpack(E + "10I", d[o:o + 40])
        secs.append(dict(name=name, typ=typ, off=off, size=size, link=link, addr=addr))
    shstr = secs[shstrndx]

    def nm(tbl, off):
        e = d.index(b"\0", tbl["off"] + off)
        return d[tbl["off"] + off:e].decode("utf-8", "replace")

    for s in secs:
        s["sname"] = nm(shstr, s["name"])
    return d, E, secs


def symbols(path):
    """-> {name: dict(defined, sec, size, typ, bind)}"""
    d, E, secs = load(path)
    out = {}
    for s in secs:
        if s["typ"] != 2:  # SHT_SYMTAB
            continue
        strt = secs[s["link"]]
        for i in range(s["size"] // 16):
            o = s["off"] + i * 16
            nameoff, value, size, info, other, shndx = struct.unpack(E + "IIIBBH", d[o:o + 16])
            e = d.index(b"\0", strt["off"] + nameoff)
            name = d[strt["off"] + nameoff:e].decode("utf-8", "replace")
            if not name:
                continue
            typ = STT.get(info & 0xF, str(info & 0xF))
            bind = STB.get(info >> 4, str(info >> 4))
            if typ in ("SECTION", "FILE"):
                continue
            defined = shndx != SHN_UNDEF and shndx != SHN_COMMON
            sec = ""
            if defined and shndx < len(secs):
                sec = secs[shndx]["sname"]
            elif shndx == SHN_COMMON:
                sec = "*COM*"
            else:
                sec = "*UND*"
            prev = out.get(name)
            if prev is None or (defined and not prev["defined"]):
                out[name] = dict(defined=defined, sec=sec, size=size, typ=typ, bind=bind)
    return out


# raices de instanciacion: los estaticos de plantilla que apagan todo el resto
ROOT_PATTERNS = [
    # GCC 2.9 mangla el estatico de una especializacion como _<clase-manglada>.<miembro>
    (re.compile(r"\._mTable$"), "Listable::_mTable"),
    (re.compile(r"\._mLists$"), "ListableSet::_mLists"),
    (re.compile(r"\._mList$"), "Instanceable::_mList"),
    (re.compile(r"\._mCollector$"), "GarbageNode::_mCollector"),
    (re.compile(r"\._mInstance$|\._mSingleton$|\.mInstance$"), "Singleton::_mInstance"),
    (re.compile(r"^_vt\.|^__vt_|^_vt__"), "vtable"),
    # OJO: _13cFEngJoyInput.mInstance es un estatico NORMAL de clase, no una
    # plantilla. Solo los que llevan el marcador de plantilla (t<N><nombre>)
    # antes del punto son raices de instanciacion.
    (re.compile(r"t\d+[A-Za-z_]\w*\d*Z.*\.[A-Za-z_]\w*$"), "estatico-PLANTILLA"),
    (re.compile(r"\.[A-Za-z_][A-Za-z0-9_]*$"), "estatico-de-clase"),
]


NOISE = re.compile(r"^(lbl_|gap_|pad_|jumptable_|jtbl_|@)|\.[0-9]+$|^r\.[0-9]+|^\$")


def is_noise(name):
    """Etiquetas anonimas del volcado (pools de literales, huecos): no son trabajo."""
    return bool(NOISE.search(name))


def classify(name):
    for rx, tag in ROOT_PATTERNS:
        if rx.search(name):
            return tag
    return ""


def units():
    out = []
    for f in sorted(os.listdir(TGT)):
        if f.endswith(".o") and os.path.exists(os.path.join(OURS, f)):
            out.append(f[:-2])
    return out


def analyze(unit):
    L = symbols(os.path.join(TGT, unit + ".o"))
    R = symbols(os.path.join(OURS, unit + ".o"))
    missing = []
    for n, s in L.items():
        if not s["defined"]:
            continue
        r = R.get(n)
        if r is None:
            missing.append((n, s, "AUSENTE"))
        elif not r["defined"]:
            missing.append((n, s, "UND" if r["sec"] == "*UND*" else "COMMON"))
    return L, R, missing


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    flags = set(a for a in sys.argv[1:] if a.startswith("--"))
    if "--roots" in flags:
        # MEDIDO: solo pagan los estaticos cuyo tipo es un CONTENEDOR con vtable
        # (Listable/ListableSet/Instanceable/GarbageNode). Arrastran vtable +
        # destructor + GetGrowSize/GetMaxCapacity/AllocVectorSpace. Los escalares
        # (Singleton/Countable/Factory/SAP::Grid) hay que definirlos igual para
        # que el simbolo case, pero aportan 0 B de codigo.
        PAGAN = ("Listable::_mTable", "ListableSet::_mLists",
                 "Instanceable::_mList", "GarbageNode::_mCollector")
        for u in (args or units()):
            L, R, miss = analyze(u)
            hot = [m for m in miss if classify(m[0]) in PAGAN]
            cold = [m for m in miss if classify(m[0]) in
                    ("Singleton::_mInstance", "estatico-PLANTILLA")]
            if not hot and not cold:
                continue
            print("=== %s ===" % u)
            for n, s_, w in sorted(hot, key=lambda m: -m[1]["size"]):
                print("  PAGA  %-8s %6dB %-26s %s" % (w, s_["size"], classify(n), n))
            for n, s_, w in sorted(cold, key=lambda m: -m[1]["size"]):
                print("  cero  %-8s %6dB %-26s %s" % (w, s_["size"], classify(n), n))
        return
    if not args:
        rows = []
        for u in units():
            try:
                L, R, miss = analyze(u)
            except Exception as ex:
                print("%-18s ERROR %s" % (u, ex))
                continue
            sig = [m for m in miss if not is_noise(m[0])]
            und = [m for m in sig if m[2] in ("UND", "COMMON")]
            roots = [m for m in sig if classify(m[0])]
            vt = [m for m in sig if classify(m[0]) == "vtable"]
            tmpl = [m for m in roots if classify(m[0]) != "vtable"]
            rows.append((len(und), u, len(sig), len(roots), len(vt), len(tmpl),
                         sum(m[1]["size"] for m in sig)))
        rows.sort(reverse=True)
        print("%-18s %6s %6s %6s %6s %7s %9s" % (
            "UNIDAD", "UND", "SENAL", "RAICES", "VTBL", "ESTATIC", "BYTES"))
        for n, u, sg, r, v, t, b in rows:
            print("%-18s %6d %6d %6d %6d %7d %9d" % (u, n, sg, r, v, t, b))
        print("")
        print(" UND = definido en objetivo, sin definir en el nuestro (la senal accionable).")
        print(" ESTATIC = _mTable/_mLists/_mList/_mCollector/_mInstance ausentes: las raices de plantilla.")
        return

    unit = args[0]
    L, R, miss = analyze(unit)
    print("=== %s : %d definidos en objetivo, %d en el nuestro, %d definidos ausentes ===" % (
        unit, sum(1 for s in L.values() if s["defined"]),
        sum(1 for s in R.values() if s["defined"]), len(miss)))
    if "--noise" not in flags:
        miss = [m for m in miss if not is_noise(m[0])]
    if "--und" in flags:
        miss = [m for m in miss if m[2] in ("UND", "COMMON")]
    show = miss if "--all" in flags else [m for m in miss if m[1]["typ"] == "OBJECT" or classify(m[0])]
    show.sort(key=lambda m: (-m[1]["size"], m[0]))
    for n, s, why in show:
        print("  %-8s %-6s %-14s %7dB  %-24s %s" % (why, s["typ"], s["sec"], s["size"], classify(n), n))
    if "--all" not in flags:
        nf = sum(1 for m in miss if m[1]["typ"] == "FUNC" and not classify(m[0]))
        print("  (+%d funciones sin clasificar; --all para verlas)" % nf)


if __name__ == "__main__":
    main()
