#!/usr/bin/env python3
"""datadiff.py -- diff BYTE a BYTE, con resincronizacion, de una seccion de datos.

`datacmp.py` dice CUANTO falta y donde esta la primera diferencia; esto dice QUE
falta: alinea los dos blobs con difflib y lista los trozos ausentes y sobrantes
con su desplazamiento en cada lado, en hex y en texto. Es lo que convierte un
`rodata-648` en una lista de cadenas y constantes concretas.

    python scripts/datadiff.py zFEng            # .rodata
    python scripts/datadiff.py zFEng .data
"""
import difflib, os, struct, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def blob(p, sname):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, sn, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]
    for s in S:
        e = d.index(b'\0', stro + s[0])
        if d[stro + s[0]:e].decode() == sname:
            return d[s[4]:s[4] + s[5]]
    return b''


def txt(b):
    return ''.join(chr(c) if 32 <= c < 127 else '.' for c in b)


u = sys.argv[1]
sname = sys.argv[2] if len(sys.argv) > 2 else '.rodata'
rel = 'Speed/Indep/SourceLists/' + u
A = blob(os.path.join(ROOT, 'build', 'GOWE69', 'obj', *rel.split('/')) + '.o', sname)
B = blob(os.path.join(ROOT, 'build', 'GOWE69', 'src', *rel.split('/')) + '.o', sname)
print('objetivo %d B   nuestro %d B   (%+d)' % (len(A), len(B), len(B) - len(A)))
sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
falta = sobra = 0
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == 'equal':
        if i2 - i1 >= 64:
            print('  == %5d B  obj +0x%04X  src +0x%04X' % (i2 - i1, i1, j1))
        continue
    if tag in ('delete', 'replace'):
        falta += i2 - i1
        for k in range(i1, i2, 32):
            ch = A[k:min(k + 32, i2)]
            print('  FALTA obj +0x%04X %3d  %-64s |%s|' % (k, len(ch), ch.hex(), txt(ch)))
    if tag in ('insert', 'replace'):
        sobra += j2 - j1
        for k in range(j1, j2, 32):
            ch = B[k:min(k + 32, j2)]
            print('  SOBRA src +0x%04X %3d  %-64s |%s|' % (k, len(ch), ch.hex(), txt(ch)))
print('falta %d B, sobra %d B (neto %+d)' % (falta, sobra, sobra - falta))
