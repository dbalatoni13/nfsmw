#!/usr/bin/env python3
"""audit_zeros.py — auditoría Fase 1: símbolos ausentes y near-misses por unidad.

Para cada unidad: objdiff-cli diff (target vs nuestro .o) y clasifica:
  MISS  : símbolo del original que no existe (o tiene size 0) en el nuestro.
          Son los "bytes más baratos": dtors sin definir, externs, stubs.
  NEAR90: 90-100% — candidatos a cierre con los patrones documentados.
  NEAR70: 70-90%.

Uso:
  python scripts/audit_zeros.py zFe2 zAI ...        # una o varias unidades
  python scripts/audit_zeros.py --top12             # las 12 grandes

Los JSON de objdiff se cachean en .tmpwork/audit/ (no se recalculan si existen).
Resumen acumulado en .tmpwork/phase1_summary.txt
"""
import json
import os
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
OBJDIFF = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")
OUT = os.path.join(ROOT, ".tmpwork", "audit")
SUMMARY = os.path.join(ROOT, ".tmpwork", "phase1_summary.txt")

TOP12 = ["zFe2", "zAI", "zCamera", "zPhysics", "zEcstasy", "zGameplay",
         "zEagl4Anim", "zFe", "zFeOverlay", "zEAXSound", "zSpeech", "zLua"]

# unidad SourceLists vive en Speed/Indep/SourceLists; admite rutas main/... completas
def paths(unit):
    if unit.startswith("main/"):
        rel = unit[len("main/"):]
    else:
        rel = "Speed/Indep/SourceLists/" + unit
    tgt = os.path.join(ROOT, "build", "GOWE69", "obj", *rel.split("/")) + ".o"
    our = os.path.join(ROOT, "build", "GOWE69", "src", *rel.split("/")) + ".o"
    return tgt, our


def diff(unit):
    os.makedirs(OUT, exist_ok=True)
    safe = unit.replace("/", "_")
    out = os.path.join(OUT, "d_%s.json" % safe)
    if os.path.exists(out):
        return out
    tgt, our = paths(unit)
    r = subprocess.run(
        [OBJDIFF, "diff", "-1", tgt, "-2", our,
         "-c", "function_reloc_diffs=none", "-c", "ppc.calculatePoolRelocations=false", "-o", out, "--format", "json"],
        capture_output=True, text=True)
    if r.returncode != 0:
        sys.stderr.write("objdiff fallo en %s:\n%s\n%s\n" % (unit, r.stdout, r.stderr))
        return None
    return out


def analyze(unit, path):
    d = json.load(open(path, encoding="utf-8"))
    L = {s["name"]: s for s in d["left"]["symbols"] if s.get("kind") == "SYMBOL_FUNCTION"}
    R = {s["name"]: s for s in d["right"]["symbols"]}
    miss, near90, near70 = [], [], []
    matched_b = 0
    total_b = 0
    for n, s in L.items():
        sz = int(s["size"])
        total_b += sz
        r = R.get(n)
        mp = s.get("match_percent") or 0.0
        if r is None or int(r.get("size") or 0) == 0:
            miss.append((sz, n))
            continue
        matched_b += int(sz * mp / 100.0)
        if 90 <= mp < 100:
            near90.append((sz, mp, n))
        elif 70 <= mp < 90:
            near70.append((sz, mp, n))
    miss.sort(reverse=True)
    near90.sort(key=lambda x: x[0], reverse=True)
    return miss, near90, near70, matched_b, total_b


def main():
    units = sys.argv[1:]
    if units == ["--top12"]:
        units = TOP12
    if not units:
        sys.exit(__doc__)
    lines = []
    for u in units:
        p = diff(u)
        if not p:
            continue
        miss, near90, near70, mb, tb = analyze(u, p)
        hdr = ("== %s: %.2f%% aprox | MISS %d fns / %d B | near90 %d fns / %d B | near70 %d fns"
               % (u, 100.0 * mb / max(tb, 1), len(miss), sum(m[0] for m in miss),
                  len(near90), sum(n[0] for n in near90), len(near70)))
        print(hdr)
        lines.append(hdr)
        for sz, n in miss[:25]:
            line = "  MISS %7d B  %s" % (sz, n[:100])
            print(line)
            lines.append(line)
        if len(miss) > 25:
            line = "  ... y %d MISS mas" % (len(miss) - 25)
            print(line)
            lines.append(line)
        for sz, mp, n in near90[:10]:
            line = "  N90  %7d B %6.2f%%  %s" % (sz, mp, n[:95])
            print(line)
            lines.append(line)
        lines.append("")
    with open(SUMMARY, "a", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
    print("resumen -> %s" % SUMMARY)


if __name__ == "__main__":
    main()
