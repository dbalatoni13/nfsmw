#!/usr/bin/env python3
"""c25w2_alloc.py -- la tabla de allocnos REAL, contrastada contra la verdad
del propio compilador.

Diferencias con scripts/lreg.py (las tres son errores medidos):

 1. lreg.py lista TODOS los pseudos del `.lreg`. A `global_alloc` solo llegan
    los que `local_alloc` NO asigno. El `.greg` los da literalmente en la linea
    `;; N regs to allocate: ...`, y ademas YA ORDENADOS por allocno_compare.
 2. lreg.py omite el factor `size` de allocno_compare:
        pri = floor_log2(n_refs)*n_refs/live_length * 10000 * size
    size = ceil(bytes/4). El `.lreg` lo imprime como `; N bytes` cuando no es
    una palabra (o sea: los `double` valen 2 y duplican su prioridad).
 3. lreg.py casa el nombre por SUBCADENA: "Smackable::Simplify" tambien casa
    "Smackable::SimplifySort". Aqui el nombre es exacto salvo que se pida --sub.

Uso:
    python scripts/alloc.py <dump-base> "<firma exacta>" [--sub] [--list PATRON]
"""
import re
import sys


def floor_log2(n):
    r = -1
    while n:
        n >>= 1
        r += 1
    return r


RE_REG = re.compile(r'^Register (\d+) used (\d+) times? across (-?\d+) insns?')


def sections(path):
    out, cur = {}, None
    for line in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'^;; Function (.+?)\s*$', line)
        if m:
            cur = m.group(1)
            out.setdefault(cur, [])
            continue
        if cur is not None:
            out[cur].append(line)
    return out


def main():
    argv = [a for a in sys.argv[1:] if not a.startswith('--')]
    base, want = argv[0], (argv[1] if len(argv) > 1 else None)
    lreg, greg = base + '.lreg', base + '.greg'
    L, G = sections(lreg), sections(greg)

    if '--list' in sys.argv:
        pat = argv[-1]
        for n in sorted(L):
            if pat in n:
                print(n)
        return

    if '--sub' in sys.argv:
        names = [n for n in sorted(L) if want in n]
    else:
        names = [n for n in sorted(L) if n == want]
        if not names:
            names = [n for n in sorted(L) if want in n]
            if len(names) > 1:
                print('AMBIGUO, %d candidatos:' % len(names))
                for n in names:
                    print('   ', n)
                return
    if not names:
        sys.exit('no encuentro: ' + str(want))

    for name in names:
        # --- .lreg: flow info -------------------------------------------
        info = {}
        localalloc = {}
        for line in L[name]:
            s = line.strip()
            m = RE_REG.match(s)
            if m:
                pseudo = int(m.group(1))
                refs = int(m.group(2))
                live = int(m.group(3))
                mb = re.search(r'; (\d+) bytes', s)
                nbytes = int(mb.group(1)) if mb else 4
                size = max(1, (nbytes + 3) // 4)
                deaths = re.search(r'dies in (\d+) places', s)
                calls = re.search(r'crosses (\d+) calls?', s)
                if 'crosses 1 call' in s:
                    ncalls = 1
                else:
                    ncalls = int(calls.group(1)) if calls else 0
                mp = re.search(r'pref ([\w ]+?)(?:, else ([\w ]+?))?[.;]', s)
                pref = mp.group(1).strip() if mp else (
                    'FLOAT_REGS' if 'FLOAT_REGS' in s else '?')
                info[pseudo] = dict(refs=refs, live=live, size=size,
                                    nbytes=nbytes, pref=pref,
                                    uservar='user var' in s,
                                    deaths=int(deaths.group(1)) if deaths else 1,
                                    calls=ncalls)
                continue
            m = re.match(r'^;; Register (\d+) in (-?\d+)\.', s)
            if m:
                localalloc[int(m.group(1))] = int(m.group(2))

        # --- .greg: orden real + dispositions ---------------------------
        order, disp, confl = [], {}, {}
        gsec = G.get(name, [])
        for line in gsec:
            m = re.match(r'^;; (\d+) regs to allocate:(.*)$', line)
            if m:
                order = [int(x) for x in re.findall(r'(?<![+\d])(\d+)', m.group(2))]
                raw = m.group(2)
                continue
            m = re.match(r'^;; (\d+) conflicts:(.*)$', line)
            if m:
                confl[int(m.group(1))] = len(m.group(2).split())
                continue
            if line.startswith(';; Register dispositions'):
                continue
            for a, b in re.findall(r'(\d+) in (-?\d+)', line):
                disp.setdefault(int(a), int(b))

        print(';; %s' % name)
        print(';; pseudos en .lreg: %d | asignados por LOCAL-alloc: %d | '
              'llegan a GLOBAL-alloc: %d'
              % (len(info), len(localalloc), len(order)))
        print()
        print('   %-4s %-7s %-7s %-9s %-5s %-12s %-12s %-6s %-6s %s'
              % ('#', 'pseudo', 'n_refs', 'live_len', 'size', 'pri(CON size)',
                 'pri(SIN size)', 'confl', 'reg', 'pref'))
        prev = None
        bad = 0
        for idx, p in enumerate(order):
            d = info.get(p)
            if d is None:
                print('   %-4d %-7d  <sin flow info>' % (idx, p))
                continue
            live = max(1, d['live'])
            pri_s = int((float(floor_log2(d['refs']) * d['refs']) / live)
                        * 10000 * d['size'])
            pri_n = int((float(floor_log2(d['refs']) * d['refs']) / live) * 10000)
            hard = disp.get(p)
            reg = ('r%d' % hard) if hard is not None and hard >= 0 else (
                '-' if hard is None else 'MEM')
            flag = ''
            if prev is not None and pri_s > prev:
                flag = '  <-- ORDEN ROTO'
                bad += 1
            prev = pri_s
            print('   %-4d %-7d %-7d %-9d %-5d %-12d %-12d %-6s %-6s %s%s%s'
                  % (idx, p, d['refs'], d['live'], d['size'], pri_s, pri_n,
                     confl.get(p, '-'), reg, d['pref'],
                     '  (var)' if d['uservar'] else '', flag))
        print()
        print(';; formula CON size reproduce el orden del compilador: %s'
              % ('SI' if bad == 0 else 'NO (%d roturas)' % bad))
        # comprobar tambien sin size
        prev, badn = None, 0
        for p in order:
            d = info.get(p)
            if d is None:
                continue
            live = max(1, d['live'])
            pri_n = int((float(floor_log2(d['refs']) * d['refs']) / live) * 10000)
            if prev is not None and pri_n > prev:
                badn += 1
            prev = pri_n
        print(';; formula SIN size: %s'
              % ('SI' if badn == 0 else 'NO (%d roturas)' % badn))
        print()


main()
