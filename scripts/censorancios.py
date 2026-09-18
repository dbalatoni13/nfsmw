#!/usr/bin/env python3
"""censorancios.py -- recompila cada unidad PROMOCIONADA a un temporal y compara las
secciones ALLOC (mas sus reubicaciones y el symtab) con el .o que hay en disco.

NO toca el arbol: el compilado va a un directorio del scratchpad.
"""
import hashlib, json, os, re, shlex, shutil, struct, subprocess, sys, time
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCRATCH = os.path.join(os.environ.get('SCRATCH') or os.environ.get('TEMP') or ROOT,
                       'censorancios_%d' % os.getpid())
os.makedirs(SCRATCH, exist_ok=True)
import atexit
atexit.register(lambda: shutil.rmtree(SCRATCH, ignore_errors=True))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
os.chdir(ROOT)
import build_direct as BD
import trypromo as TP

SHF_ALLOC = 0x2


def secciones(path):
    """-> {nombre: sha1(contenido)} de TODAS las secciones, mas la marca ALLOC."""
    d = open(path, 'rb').read()
    assert d[:4] == b'\x7fELF', path
    # ELF32 big endian
    e_shoff, = struct.unpack_from('>I', d, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('>HHH', d, 0x2E)
    hdrs = []
    for i in range(e_shnum):
        o = e_shoff + i * e_shentsize
        name, typ, flags, addr, off, size = struct.unpack_from('>IIIIII', d, o)
        hdrs.append((name, typ, flags, addr, off, size))
    stro = hdrs[e_shstrndx][4]
    out = {}
    for name, typ, flags, addr, off, size in hdrs:
        end = d.index(b'\0', stro + name)
        nm = d[stro + name:end].decode('latin1')
        if not nm:
            continue
        body = b'' if typ == 8 else d[off:off + size]  # NOBITS
        out[nm] = (hashlib.sha1(body).hexdigest(), bool(flags & SHF_ALLOC), size)
    return out


# OJO: `.rela.debug*` es la tabla de reubicaciones DE la depuracion y tampoco
# llega al DOL. Sin el `(rela\.)?` el censo daba RANCIO a unidades que solo
# diferian ahi (r66: pathaction/pathevent/pathnode/pathsnd/saemstimupdt tras
# tocar pathi.h). `.rela.text`, `.rela.sdata`... SIGUEN contando.
DEBUGISH = re.compile(r'^\.(rela\.)?(line|debug|comment|stab|note)')


def interesante(nm):
    return not DEBUGISH.match(nm)


def compila_a_temp(name, spec, dest_dir):
    src, out, cflags, toolchain, rename = spec
    rule = BD.RULE.get(name, 'prodg')
    env = dict(os.environ)
    os.makedirs(dest_dir, exist_ok=True)
    base = os.path.basename(out)
    tmp_out = os.path.join(dest_dir, base)
    if rule.startswith('mwcc'):
        cc = os.path.join(ROOT, 'build', 'compilers', toolchain, 'mwcceppc.exe')
        wrap = os.path.join(ROOT, 'build', 'tools', 'sjiswrap.exe')
        cf = shlex.split(' '.join(cflags))
        head = [wrap, cc] if rule.endswith('sjis') else [cc]
        cmd = head + cf + ['-c', src, '-o', dest_dir]
        tmp_out = os.path.join(dest_dir, os.path.basename(src).rsplit('.', 1)[0] + '.o')
    else:
        cc = os.path.join(ROOT, 'build', 'compilers', toolchain, 'ngccc.exe')
        env['SN_NGC_PATH'] = os.path.join(ROOT, 'build', 'compilers', toolchain)
        cmd = [cc] + cflags + ['-c', '-o', tmp_out, src]
    if os.path.exists(tmp_out):
        os.remove(tmp_out)
    p = subprocess.run(cmd, cwd=ROOT, env=env, capture_output=True, text=True)
    if p.returncode != 0 or not os.path.exists(tmp_out):
        return None, 'rc=%d %s' % (p.returncode, ((p.stderr or '') + (p.stdout or '')).strip()[:200])
    if rename:
        r = subprocess.run([sys.executable, os.path.join('tools', 'rename_section.py'),
                            '-q', tmp_out, rename], cwd=ROOT, capture_output=True, text=True)
        if r.returncode != 0:
            return None, 'rename: ' + (r.stderr or r.stdout)[:150]
    return tmp_out, None


def una(name, spec, idx):
    disco = spec[1]
    if not os.path.exists(disco):
        return name, 'SIN_O', {}, ''
    dest = os.path.join(SCRATCH, 'w%d' % (idx % 8), name.replace('/', '_').replace(chr(92), '_'))
    t0 = time.time()
    tmp, err = compila_a_temp(name, spec, dest)
    dt = time.time() - t0
    if tmp is None:
        shutil.rmtree(dest, ignore_errors=True)
        return name, 'NOCOMPILA', {'err': err}, ''
    try:
        a = secciones(disco)
        b = secciones(tmp)
    finally:
        pass
    difs = {}
    for nm in sorted(set(a) | set(b)):
        va, vb = a.get(nm), b.get(nm)
        if va is None or vb is None:
            difs[nm] = ('AUSENTE' if va is None else 'SOBRA', va[2] if va else None, vb[2] if vb else None)
        elif va[0] != vb[0]:
            difs[nm] = ('DISTINTA', va[2], vb[2])
    rel = {k: v for k, v in difs.items() if interesante(k)}
    estado = 'RANCIO' if rel else ('ok_debug' if difs else 'ok')
    if estado != 'RANCIO':
        shutil.rmtree(dest, ignore_errors=True)
    return name, estado, rel, '%.1fs' % dt



ASM = {'build/GOWE69/src/LibSN/metrotrk.o': 'src/LibSN/metrotrk.s',
       'build/GOWE69/src/LibSN/crt0.o': 'src/LibSN/crt0.s',
       'build/GOWE69/src/pro2D4.o': 'src/LibSN/pro2D4.s'}
ASFLAGS = ('-mgekko --strip-local-absolute -I include -I build/GOWE69/include '
           '--defsym BUILD_VERSION=0').split()


def una_asm(out, src, idx):
    """Las tres unidades de la regla `as`: powerpc-eabi-as + dtk elf fixup."""
    AS = os.path.join('build', 'ppc_binutils', 'powerpc-eabi-as.exe')
    DTK = os.path.join('build', 'tools', 'dtk.exe')
    name = os.path.basename(out)[:-2]
    dest = os.path.join(SCRATCH, 'as', name)
    os.makedirs(dest, exist_ok=True)
    tmp = os.path.join(dest, name + '.o')
    t0 = time.time()
    p = subprocess.run([AS] + ASFLAGS + ['-o', tmp, src, '-MD', tmp + '.d'],
                       cwd=ROOT, capture_output=True, text=True)
    if p.returncode != 0 or not os.path.exists(tmp):
        return name, 'NOCOMPILA', {'err': (p.stderr or '')[:200]}, ''
    subprocess.run([DTK, 'elf', 'fixup', tmp, tmp], cwd=ROOT, capture_output=True, text=True)
    a, b = secciones(out), secciones(tmp)
    difs = {nm: ('DISTINTA', a.get(nm, (0, 0, None))[2], b.get(nm, (0, 0, None))[2])
            for nm in sorted(set(a) | set(b)) if a.get(nm, ('x',))[0] != b.get(nm, ('y',))[0]}
    rel = {k: v for k, v in difs.items() if interesante(k)}
    return name, ('RANCIO' if rel else 'ok'), rel, '%.1fs' % (time.time() - t0)


def main():
    units = BD.parse_units()
    base = TP.objetos_del_enlace()
    prom = set(x.replace(chr(92), '/') for x in base if 'GOWE69/src' in x.replace(chr(92), '/'))
    sel = {k: v for k, v in units.items() if v[1].replace(chr(92), '/') in prom}
    filtro = [a for a in sys.argv[1:] if not a.startswith('--')]
    if filtro:
        if filtro[0] == 'SL':
            sel = {k: v for k, v in sel.items() if 'SourceLists' in v[1]}
        elif filtro[0] == 'NOSL':
            sel = {k: v for k, v in sel.items() if 'SourceLists' not in v[1]}
        else:
            sel = {k: v for k, v in sel.items() if k in filtro}
    print('promocionadas en el enlace: %d ; con edge en build.ninja: %d' % (len(prom), len(sel)))
    faltan = prom - set(v[1].replace(chr(92), '/') for v in sel.values())
    res = []
    if not filtro:
        for i, (o, sr) in enumerate(sorted(ASM.items())):
            r = una_asm(o, sr, i)
            res.append(r)
            print('  asm  %-34s %-10s %-7s %s' % (r[0], r[1], r[3],
                  ','.join(sorted(r[2])) if r[2] else ''), flush=True)
        faltan = set()
    with ThreadPoolExecutor(max_workers=6) as ex:
        futs = [ex.submit(una, n, s, i) for i, (n, s) in enumerate(sorted(sel.items()))]
        for j, f in enumerate(futs):
            name, estado, difs, dt = f.result()
            res.append((name, estado, difs, dt))
            marca = '' if estado.startswith('ok') else '   <<<'
            print('%4d/%d  %-34s %-10s %-7s %s%s' % (j + 1, len(futs), name, estado, dt,
                                                     ','.join(sorted(difs)) if difs else '', marca), flush=True)
    out = os.path.join(ROOT, 'build', 'GOWE69',
                       'censorancios_%s.json' % (filtro[0] if filtro else 'all'))
    json.dump({'res': [(a, b, c, d) for a, b, c, d in res], 'sin_edge': sorted(faltan)},
              open(out, 'w'), indent=1)
    print('\n--- resumen ---')
    for e in ('RANCIO', 'NOCOMPILA', 'SIN_O', 'ok_debug', 'ok'):
        n = [r for r in res if r[1] == e]
        print('%-10s %d' % (e, len(n)))
    print('json:', out)


main()
