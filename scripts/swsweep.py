#!/usr/bin/env python3
"""fe_swsweep.py -- barrido global buscando los DOS patrones de switch.

Patron 1: `case A: case B:` con cuerpo compartido -> en el original son DOS
          cases con el cuerpo duplicado; un solo code_label impide que el
          cross-jumping funda las colas de despacho.
Patron 2: la ultima hoja del arbol de switch se INVIERTE si el cuerpo de su
          case va pegado al despacho (o sea, si ese case es el PRIMERO de la
          fuente), y esa inversion impide la fusion.

Discriminante: delta de mnemonicos con SALTOS de signo opuesto (inversion de
polaridad) y/o `b` con delta, mas presencia de un arbol de switch en el asm.
"""
import collections
import json
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "..", ".."))
ROOT = r"C:\Users\jferr\Desktop\nfsdecompiled"
SCR = r"C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad"
CLI = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")

COND = ("beq", "bne", "bgt", "blt", "bge", "ble", "beqlr", "bnelr", "bgtlr",
        "bltlr", "bgelr", "blelr")
INV = {"beq": "bne", "bne": "beq", "bgt": "ble", "ble": "bgt",
       "blt": "bge", "bge": "blt"}


def texto(e):
    return ((e.get("instruction") or {}).get("formatted", "") or "").strip()


def mnem(ins):
    c = collections.Counter()
    for e in ins or []:
        f = texto(e).split()
        if f:
            c[f[0]] += 1
    return c


def arbol(ins):
    """(n_cmp_const, tiene_tabla): pinta de switch en el asm."""
    txt = [texto(e) for e in ins or []]
    ncmp = sum(1 for t in txt if re.match(r"^(cmpwi|cmplwi)\s", t))
    tabla = any(t.startswith("bctr") or t.startswith("mtctr") for t in txt)
    return ncmp, tabla


def audita(unit):
    base = "Speed/Indep/SourceLists/" + unit
    a = os.path.join(ROOT, "build", "GOWE69", "obj", base.replace("/", os.sep) + ".o")
    b = os.path.join(ROOT, "build", "GOWE69", "src", base.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None
    out = os.path.join(SCR, "swsweep_%s.json" % unit)
    r = subprocess.run([CLI, "diff", "-1", a, "-2", b,
                        "-c", "function_reloc_diffs=none",
                        "-c", "ppc.calculatePoolRelocations=false",
                        "-o", out, "--format", "json", "zz"],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0 or not os.path.exists(out):
        return None
    try:
        d = json.load(open(out))
    except Exception:
        return None
    finally:
        try:
            os.remove(out)
        except OSError:
            pass
    der = {s.get("name"): s for s in d["right"]["symbols"]}
    filas = []
    for s in d["left"]["symbols"]:
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        n = s.get("name")
        o = der.get(n)
        if not n or o is None:
            continue
        pct = o.get("match_percent") or 0
        if pct >= 100.0:
            continue
        ia, ib = s.get("instructions"), o.get("instructions")
        ma, mb = mnem(ia), mnem(ib)
        delta = {k: ma.get(k, 0) - mb.get(k, 0)
                 for k in set(ma) | set(mb) if ma.get(k, 0) != mb.get(k, 0)}
        if not delta:
            continue
        # polaridad: hay un salto que sube y su inverso que baja
        swaps = []
        for k, v in delta.items():
            j = INV.get(k)
            if j and delta.get(j, 0) * v < 0:
                if (k, j) not in swaps and (j, k) not in swaps:
                    swaps.append((k, j))
        db = delta.get("b", 0)
        ncmp_t, tab_t = arbol(ia)
        ncmp_n, tab_n = arbol(ib)
        filas.append(dict(unit=unit, sz=int(s.get("size") or 0), pct=pct, n=n,
                          delta=delta, swaps=swaps, db=db,
                          ncmp_t=ncmp_t, tab_t=tab_t, ncmp_n=ncmp_n, tab_n=tab_n))
    return filas


def main():
    args = sys.argv[1:]
    if not args or args == ["--todas"]:
        sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
        args = sorted(f[:-4] for f in os.listdir(sl) if f.endswith(".cpp"))
    todas = []
    for u in args:
        f = audita(u)
        if f:
            todas.extend(f)
    todas.sort(key=lambda r: -r["sz"])
    print("=== TODAS las funciones con delta (%d) ===" % len(todas))
    for r in todas:
        top = sorted(r["delta"].items(), key=lambda x: -abs(x[1]))
        marca = ""
        if r["swaps"]:
            marca += " [SWAP:%s]" % ",".join("%s/%s" % s for s in r["swaps"])
        if r["db"]:
            marca += " [b%+d]" % r["db"]
        if r["ncmp_t"] >= 3 or r["tab_t"]:
            marca += " [sw?cmp=%d/%d%s]" % (r["ncmp_t"], r["ncmp_n"],
                                            ",TABLA" if r["tab_t"] else "")
        print("%-14s %7dB %8.4f%%  %-52s %-40s%s"
              % (r["unit"], r["sz"], r["pct"], r["n"][:52],
                 ", ".join("%s%+d" % (k, v) for k, v in top[:6]), marca))
    print()
    cand = [r for r in todas if r["swaps"] or r["db"]]
    cand.sort(key=lambda r: -r["sz"])
    print("=== CANDIDATAS (inversion de polaridad y/o delta de `b`): %d, %d B ==="
          % (len(cand), sum(r["sz"] for r in cand)))
    for r in cand:
        top = sorted(r["delta"].items(), key=lambda x: -abs(x[1]))
        print("%-14s %7dB %8.4f%%  %-52s %s | swaps=%s b%+d cmp=%d/%d %s"
              % (r["unit"], r["sz"], r["pct"], r["n"][:52],
                 ", ".join("%s%+d" % (k, v) for k, v in top[:6]),
                 ",".join("%s/%s" % s for s in r["swaps"]) or "-", r["db"],
                 r["ncmp_t"], r["ncmp_n"], "TABLA" if r["tab_t"] else ""))


if __name__ == "__main__":
    main()
