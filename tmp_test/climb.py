import os, re, subprocess, sys, itertools

SRC = 'src/Speed/Indep/Libs/snd/9/source/library/cmn/splysdef.c'
OUT = 'tmp_test/splysdef.s'
line = open(os.path.join(os.environ['TEMP'], 'cmd.txt')).read().strip().replace('cmd /c ', '', 1)
sp, c = line.split('&&', 1)
BS = chr(92)
c2 = c.replace('-o build' + BS + 'GOWE69' + BS + 'src' + BS + 'Speed' + BS + 'Indep' + BS + 'Libs' + BS + 'snd' + BS + '9' + BS + 'source' + BS + 'library' + BS + 'cmn' + BS + 'splysdef.o', '-S -o tmp_test' + BS + 'splysdef.s')
c2 = re.sub(r'-MF "?[^ "]+\.d"? ', '', c2).replace('-MD', '')
open(os.path.join(os.environ['TEMP'], 's.bat'), 'w').write('@echo off\n' + sp.strip() + '\n' + c2.strip() + '\n')


def compile_src():
    r = subprocess.run(['cmd', '/c', os.path.join(os.environ['TEMP'], 's.bat')], capture_output=True)
    if not os.path.exists(OUT):
        print(r.stdout.decode(errors='replace')[-500:], r.stderr.decode(errors='replace')[-500:])
        sys.exit(1)
    return open(OUT).read()


def make_source(order):
    lines = []
    for (field, val, kind) in order:
        lines.append("    pspo->%s = %s;" % (field, val))
    return '#include "./sndcmn.h"\n\nint SNDplaysetdef(SNDPLAYOPTS *pspo) {\n' + '\n'.join(lines) + '\n\n    return 0;\n}\n'


def get_emission(asm):
    lis = {}
    stores = []
    for m in re.finditer(r'^\t(li|sth|stb)\s+(\w+),(-?\w+)(?:\((\w+)\))?', asm, re.M):
        op, reg, val, base = m.groups()
        if op == 'li':
            lis[reg] = val
        elif base == '9':
            stores.append((op, lis.get(reg, '?'), int(val, 0)))
    return stores


def parse_target():
    orig = open('build/GOWE69/asm/Speed/Indep/Libs/snd/9/source/library/cmn/splysdef.s').read()
    lis = {}
    out = []
    for m in re.finditer(r'\*/\t(li|sth|stb)\s+(\w+),\s*(-?0x[0-9a-f]+|-?\d+)(?:\((\w+)\))?', orig):
        op, reg, val, base = m.groups()
        if op == 'li':
            lis[reg] = val
        elif base == 'r9':
            v = lis.get(reg, reg).replace('0x', '')
            iv = int(v, 16) if re.match(r'^-?[0-9a-f]+$', v) else int(v)
            off = int(val.replace('0x', ''), 16) if val.startswith(('0x', '-0x')) else int(val)
            out.append((op, iv, off))
    return out


def norm_stores(stores):
    res = []
    for op, v, off in stores:
        try:
            vi = int(v, 0)
        except Exception:
            vi = v
        res.append((op, vi, off))
    return res


target = norm_stores(parse_target())
print("TARGET:", target)

fields = [
    ('azimuth', '0', 'h'), ('keynum', '60', 'b'), ('timemult', '4096', 'h'),
    ('tempomult', '4096', 'h'), ('highpasscutoff', '0', 'h'), ('drylevel', '127', 'b'),
    ('vol', '127', 'b'), ('lowpasscutoff', '0xFFFF', 'h'), ('fxlevel0', '0', 'b'),
    ('bend', '64', 'b'), ('velocity', '127', 'b'), ('elevation', '0', 'h'),
    ('pitchmult', '4096', 'h'),
]


def lcs(a, b):
    n, m = len(a), len(b)
    dp = [[0] * (m + 1) for _ in range(n + 1)]
    for i in range(n):
        for j in range(m):
            if a[i] == b[j]:
                dp[i + 1][j + 1] = dp[i][j] + 1
            else:
                dp[i + 1][j + 1] = max(dp[i + 1][j], dp[i][j + 1])
    return dp[n][m]


def score(order):
    open(SRC, 'w').write(make_source(order))
    asm = compile_src()
    em = norm_stores(get_emission(asm))
    return lcs(em, target), em


import random
overall_best = None
overall_sc = -1
random.seed(1234)
for trial in range(12):
    if trial == 0:
        cand0 = list(fields)
    else:
        cand0 = list(fields)
        random.shuffle(cand0)
    best_order = cand0
    best_sc, em = score(best_order)
    rounds = 0
    improved = True
    while improved and rounds < 25:
        improved = False
        rounds += 1
        for i in range(len(best_order)):
            for j in range(len(best_order)):
                if i == j:
                    continue
                cand = list(best_order)
                cand[i], cand[j] = cand[j], cand[i]
                sc, em = score(cand)
                if sc > best_sc:
                    best_sc, best_order = sc, cand
                    improved = True
        for i in range(len(best_order)):
            for j in range(len(best_order)):
                if i == j:
                    continue
                cand = list(best_order)
                x = cand.pop(i)
                cand.insert(j, x)
                sc, em = score(cand)
                if sc > best_sc:
                    best_sc, best_order = sc, cand
                    improved = True
    print("trial", trial, "->", best_sc)
    if best_sc > overall_sc:
        overall_sc, overall_best = best_sc, best_order
best_order = overall_best
best_sc = overall_sc
print("start score", best_sc, "/", len(target))
rounds = 0
improved = True
while improved and rounds < 25:
    improved = False
    rounds += 1
    for i in range(len(best_order)):
        for j in range(len(best_order)):
            if i == j:
                continue
            cand = list(best_order)
            cand[i], cand[j] = cand[j], cand[i]
            sc, em = score(cand)
            if sc > best_sc:
                best_sc, best_order = sc, cand
                improved = True
                print("round", rounds, "swap", i, j, "->", best_sc)
    for i in range(len(best_order)):
        for j in range(len(best_order)):
            if i == j:
                continue
            cand = list(best_order)
            x = cand.pop(i)
            cand.insert(j, x)
            sc, em = score(cand)
            if sc > best_sc:
                best_sc, best_order = sc, cand
                improved = True
                print("round", rounds, "move", i, j, "->", best_sc)
print("FINAL", best_sc)
print(best_order)
open(SRC, 'w').write(make_source(best_order))
