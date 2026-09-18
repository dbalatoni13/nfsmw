#!/usr/bin/env python3
"""unitmap.py -- porcentaje por unidad + total, desde build/GOWE69/report.json.
   python scripts/unitmap.py [guardar_como.json]   # imprime y opcionalmente compara
   python scripts/unitmap.py --cmp base.json       # diff contra un snapshot
"""
import json, os, sys
ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
def load(p):
    d = json.load(open(p, encoding="utf-8"))
    out = {}
    for u in d["units"]:
        m = u["measures"]
        out[u["name"]] = (int(m.get("matched_code", 0)), int(m.get("total_code", 0)),
                          float(m.get("matched_code_percent", 0.0)))
    m = d["measures"]
    out["#TOTAL#"] = (int(m.get("matched_code",0)), int(m.get("total_code",0)), float(m.get("matched_code_percent",0.0)))
    return out
if __name__ == "__main__":
    cur = load(os.path.join(ROOT, "build", "GOWE69", "report.json"))
    if len(sys.argv) > 2 and sys.argv[1] == "--cmp":
        old = load(sys.argv[2])
        reg = []
        for k, v in sorted(cur.items()):
            o = old.get(k)
            if o and abs(v[2]-o[2]) > 1e-9:
                reg.append((v[2]-o[2], k, o[2], v[2], v[0]-o[0]))
        reg.sort()
        for d, k, a, b, db in reg:
            print("%-46s %8.4f%% -> %8.4f%%  (%+.4f, %+d B)" % (k, a, b, d, db))
        if not reg: print("sin cambios")
    else:
        for k in sorted(cur):
            if "SourceLists" in k or k == "#TOTAL#":
                print("%-46s %8.4f%%  %d/%d" % (k, cur[k][2], cur[k][0], cur[k][1]))
