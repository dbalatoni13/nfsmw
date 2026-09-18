#!/usr/bin/env python3
"""vtable_audit.py -- audita el orden/conteo de metodos virtuales.

Tres fuentes:
  A) ORIGINAL  build/GOWE69/asm/**/*.s   ".obj _vt.<mangled>, global" ... ".endobj"
                -> verdad absoluta: tamano de la vtable y simbolo por ranura.
  B) NUESTRO   build/GOWE69/obj/**/*.o   objdump -t (addr/size) + objdump -r (simbolo)
  C) PS2       symbols/PS2/PS2_types.nothpp   "/* vtable[N] */ virtual ..."
                -> nombre legible de cada ranura, incluidas las __pure_virtual.

Ranura i -> offset 8*i dentro del objeto _vt.; el puntero esta en 8*i+4.
Ranura 0 = cabecera (delta/index), ranura 1 = destructor, ultima = terminador.
"""
import os, re, sys, json, subprocess, glob, collections, argparse

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
ASM = os.path.join(ROOT, "build", "GOWE69", "asm")
OBJ = os.path.join(ROOT, "build", "GOWE69", "src")   # base = compilado de nuestro src/
PS2 = os.path.join(ROOT, "symbols", "PS2", "PS2_types.nothpp")
OBJDUMP = os.path.join(ROOT, "build", "ppc_binutils", "powerpc-eabi-objdump.exe")
REPORT = os.path.join(ROOT, "build", "GOWE69", "report.json")
CACHE = os.path.join(ROOT, "build", "vtable_audit_cache.json")


# ---------------------------------------------------------------- demangle
def demangle_class(m):
    """GNU v2: 8FEWidget -> FEWidget ; Q24Path9PathToSnd -> Path::PathToSnd"""

    def one(s, i):
        if i < len(s) and s[i] == 'Q':
            i += 1
            if s[i] == '_':
                j = s.index('_', i + 1)
                n = int(s[i + 1:j])
                i = j + 1
            else:
                n = int(s[i])
                i += 1
            parts = []
            for _ in range(n):
                p, i = one(s, i)
                parts.append(p)
            return "::".join(parts), i
        if i < len(s) and s[i] == 't':
            i += 1
            j = i
            while j < len(s) and s[j].isdigit():
                j += 1
            n = int(s[i:j])
            name = s[j:j + n]
            return name + "<>", len(s)
        j = i
        while j < len(s) and s[j].isdigit():
            j += 1
        if j == i:
            return s[i:], len(s)
        n = int(s[i:j])
        return s[j:j + n], j + n

    try:
        name, _ = one(m, 0)
        return name
    except Exception:
        return m


def fn_class(sym):
    """Act__8FEWidget -> FEWidget ; _._8FEWidget -> FEWidget"""
    if sym.startswith("_._"):
        return demangle_class(sym[3:])
    i = sym.find("__")
    while i != -1:
        rest = sym[i + 2:]
        if rest and (rest[0].isdigit() or rest[0] in "Qt"):
            return demangle_class(rest)
        i = sym.find("__", i + 1)
    return None


# ---------------------------------------------------------- A) original asm
OBJ_RE = re.compile(r'^\.obj\s+(_vt\.[^,\s]+)\s*,')
B4_RE = re.compile(r'^\s*\.4byte\s+(\S+)')


def load_original():
    out = {}
    for path in glob.glob(os.path.join(ASM, "**", "*.s"), recursive=True):
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
        i = 0
        while i < len(lines):
            m = OBJ_RE.match(lines[i])
            if not m:
                i += 1
                continue
            name = m.group(1)
            vals = []
            j = i + 1
            while j < len(lines) and not lines[j].startswith(".endobj"):
                mb = B4_RE.match(lines[j])
                if mb:
                    vals.append(mb.group(1))
                j += 1
            slots = []
            for k in range(0, len(vals) - 1, 2):
                v = vals[k + 1]
                slots.append(None if v in ("0x00000000", "0") else v)
            out[name] = {"size": len(vals) * 4, "slots": slots,
                         "file": os.path.relpath(path, ROOT).replace("\\", "/")}
            i = j + 1
    return out


# ------------------------------------------------------------- B) nuestro
def load_ours(objs=None):
    if objs is None:
        objs = sorted(glob.glob(os.path.join(OBJ, "**", "*.o"), recursive=True))
    out = {}
    for o in objs:
        try:
            t = subprocess.run([OBJDUMP, "-t", o], capture_output=True, text=True,
                               errors="replace").stdout
        except Exception:
            continue
        vts = []
        for ln in t.splitlines():
            if "_vt." not in ln:
                continue
            parts = ln.split()
            if len(parts) < 5 or not parts[-1].startswith("_vt."):
                continue
            try:
                addr = int(parts[0], 16)
                size = int(parts[-2], 16)
            except ValueError:
                continue
            sec = None
            for p in parts:
                if p.startswith("."):
                    sec = p
                    break
            if sec is None or size == 0:
                continue
            vts.append((parts[-1], sec, addr, size))
        if not vts:
            continue
        r = subprocess.run([OBJDUMP, "-r", o], capture_output=True, text=True,
                           errors="replace").stdout
        relocs = collections.defaultdict(dict)
        cur = None
        for ln in r.splitlines():
            m = re.match(r'RELOCATION RECORDS FOR \[(\S+)\]', ln)
            if m:
                cur = m.group(1)
                continue
            m = re.match(r'^([0-9a-f]{8})\s+\S+\s+(\S+)', ln)
            if m and cur:
                relocs[cur][int(m.group(1), 16)] = m.group(2)
        for name, sec, addr, size in vts:
            slots = []
            for i in range(size // 8):
                slots.append(relocs[sec].get(addr + 8 * i + 4))
            out[name] = {"size": size, "slots": slots,
                         "obj": os.path.relpath(o, ROOT).replace("\\", "/")}
    return out


# ----------------------------------------------------------------- C) PS2
CLS_RE = re.compile(r'^(?:struct|class|union)\s+([A-Za-z_]\w*(?:<[^>]*>)?)\s*(?::[^{]*)?\{')
VT_RE = re.compile(r'^\s*/\* vtable\[(\d+)\] \*/\s*virtual\s+(.*?)\s*;\s*$')


def load_ps2():
    out = {}
    cur = None
    with open(PS2, "r", encoding="utf-8", errors="replace") as f:
        for ln in f:
            if ln and not ln[0].isspace():
                m = CLS_RE.match(ln)
                cur = m.group(1) if m else None
                if cur and cur not in out:
                    out[cur] = {}
                continue
            if cur is None:
                continue
            m = VT_RE.match(ln)
            if m:
                idx = int(m.group(1))
                sig = m.group(2)
                nm = re.search(r'(~?\w+)\s*\(', sig)
                out[cur][idx] = (nm.group(1) if nm else sig, sig)
    return {k: v for k, v in out.items() if v}


# ------------------------------------------------------------ D) report.json
def load_report():
    if not os.path.exists(REPORT):
        return {}
    with open(REPORT, "r", encoding="utf-8", errors="replace") as f:
        rep = json.load(f)
    out = {}
    for u in rep.get("units", []):
        for fn in (u.get("functions") or []):
            nm = fn.get("name") or ""
            m = fn.get("measures") or {}
            pct = float(m.get("fuzzy_match_percent",
                              fn.get("fuzzy_match_percent", 0.0)) or 0.0)
            sz = int(m.get("total_code", fn.get("size", 0)) or 0)
            out[nm] = (sz, pct, u.get("name", ""))
    return out


# ------------------------------------------------------------------ main
def norm(sym):
    """GCC numera las clases locales con un sufijo .NNNNN que no es comparable
    entre binarios; sin quitarlo cada callback local sale como falso positivo."""
    return re.sub(r'\.\d+$', '', sym) if sym else sym


def build_rows(orig, ours, cost):
    rows = []
    for name, o in sorted(orig.items()):
        u = ours.get(name)
        cls = demangle_class(name[4:].split(".")[0])
        if u is None:
            rows.append((cls, name, "AUSENTE", o["size"], 0, [], o))
            continue
        diffs = []
        n = max(len(o["slots"]), len(u["slots"]))
        for i in range(n):
            a = o["slots"][i] if i < len(o["slots"]) else "<fuera>"
            b = u["slots"][i] if i < len(u["slots"]) else "<fuera>"
            if norm(a) != norm(b):
                diffs.append((i, a, b))
        if o["size"] != u["size"] or diffs:
            kind = "TAMANO" if o["size"] != u["size"] else "ORDEN"
            rows.append((cls, name, kind, o["size"], u["size"], diffs, o))
    rows.sort(key=lambda r: -cost[r[0]])
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--top", type=int, default=60)
    ap.add_argument("--klass", default=None)
    ap.add_argument("--refresh", action="store_true")
    ap.add_argument("--no-absent", action="store_true",
                    help="oculta las vtables que ni siquiera emitimos")
    args = ap.parse_args()

    if os.path.exists(CACHE) and not args.refresh:
        cache = json.load(open(CACHE, "r", encoding="utf-8"))
        orig, ours = cache["orig"], cache["ours"]
    else:
        sys.stderr.write("[*] leyendo asm original...\n")
        orig = load_original()
        sys.stderr.write("[*] leyendo objetos propios...\n")
        ours = load_ours()
        json.dump({"orig": orig, "ours": ours}, open(CACHE, "w", encoding="utf-8"))
    ps2 = load_ps2()
    rep = load_report()

    cost = collections.Counter()
    nfun = collections.Counter()
    for fn, (sz, pct, unit) in rep.items():
        if pct >= 100.0:
            continue
        c = fn_class(fn)
        if c:
            cost[c] += int(sz * (1.0 - pct / 100.0))
            nfun[c] += 1

    if args.klass:
        detail(args.klass, orig, ours, ps2)
        return

    rows = build_rows(orig, ours, cost)
    if args.no_absent:
        rows = [r for r in rows if r[2] != "AUSENTE"]
    def slot_gain(diffs):
        g = 0
        for i, a, b in diffs:
            if not a or a in ("__pure_virtual", "<fuera>"):
                continue
            sz, pct, _ = rep.get(a, (0, 0.0, ""))
            g += int(sz * (1.0 - pct / 100.0)) if sz else 0
        return g

    scored = [(slot_gain(r[5]), r) for r in rows]
    scored.sort(key=lambda t: (-t[0], -cost[t[1][0]]))
    tot = sum(cost[r[0]] for r in rows)
    totslot = sum(s for s, _ in scored)
    print("=" * 78)
    print("CLASES CON DISCREPANCIA DE VTABLE: %d de %d vtables del original"
          % (len(rows), len(orig)))
    print("BYTES EN JUEGO (metodos no casados de esas clases): %d" % tot)
    print("BYTES EN LAS PROPIAS RANURAS mal puestas:           %d" % totslot)
    print("=" * 78)
    for gain, (cls, name, kind, so, su, diffs, o) in scored[:args.top]:
        p = ps2.get(cls, {})
        print("\n%-38s %-7s orig=0x%-4x ours=0x%-4x ranuras=%dB clase=%dB (%dfn) [%s]"
              % (cls, kind, so, su, gain, cost[cls], nfun[cls],
                 os.path.basename(o["file"])))
        if p:
            print("     PS2: %d ranuras conocidas (max %d)" % (len(p), max(p)))
        for i, a, b in diffs[:16]:
            pn = p.get(i, ("?", ""))[0]
            sz, pct, _ = rep.get(a, (0, -1.0, ""))
            tag = ("  <-- %dB @%.1f%%" % (sz, pct)) if pct >= 0 else                   ("  <-- NO EXISTE" if a and a != "__pure_virtual" else "")
            print("      [%2d] 0x%02x PS2=%-20s orig=%-38s ours=%s%s"
                  % (i, 8 * i, pn, a or "0", b or "0", tag))
        if len(diffs) > 16:
            print("      ... %d ranuras mas" % (len(diffs) - 16))


def detail(cls, orig, ours, ps2):
    cand = [n for n in orig if demangle_class(n[4:].split(".")[0]) == cls]
    if not cand:
        cand = [n for n in orig if cls in n]
    for name in cand:
        o = orig[name]
        u = ours.get(name)
        p = ps2.get(demangle_class(name[4:].split(".")[0]), {})
        print("\n=== %s (%s) orig=0x%x nuestro=%s" %
              (name, demangle_class(name[4:].split(".")[0]), o["size"],
               ("0x%x" % u["size"]) if u else "AUSENTE"))
        n = max(len(o["slots"]), len(u["slots"]) if u else 0,
                (max(p) + 1) if p else 0)
        for i in range(n):
            a = o["slots"][i] if i < len(o["slots"]) else "-"
            b = (u["slots"][i] if u and i < len(u["slots"]) else "-")
            pn = p.get(i, ("", ""))[0]
            flag = "  <<<" if norm(a) != norm(b) else ""
            print("  [%2d] 0x%02x  PS2=%-28s orig=%-42s ours=%s%s"
                  % (i, 8 * i, pn, a or "0", b or "0", flag))


if __name__ == "__main__":
    main()
