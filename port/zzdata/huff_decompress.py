#!/usr/bin/env python3
"""Descompresor HUFF de EA, portado del LZCompress.cpp del decomp.

El formato (BE en stream):
  [0] flag(1bit) -> 16bit type; si bit15: cabecera larga (8+32 bits),
  sino corta (8+24 bits). Bit8 = hay campo extra de 16/8 bits.
  Tras type: clue(8), tabla de codigos canonica, datos Huffman.
"""
import struct


class BitReaderBE:
    def __init__(self, data):
        self.d = data
        self.pos = 0
        self.bits = 0
        self.nbits = 0

    def get(self, n):
        while self.nbits < n:
            if self.pos >= len(self.d):
                return 0
            self.bits = (self.bits << 8) | self.d[self.pos]
            self.pos += 1
            self.nbits += 8
        v = (self.bits >> (self.nbits - n)) & ((1 << n) - 1)
        self.nbits -= n
        return v


def huff_decompress(data):
    br = BitReaderBE(data)
    typ = br.get(16)
    if typ & 0x8000:
        if typ & 0x100:
            br.get(16); br.get(16)
        br.get(16)
        ulen = br.get(16) | (br.get(16) << 16)
    else:
        if typ & 0x100:
            br.get(8); br.get(16)
        br.get(8)
        ulen = br.get(16) | (br.get(8) << 16)

    clue = br.get(8)
    # tabla canonica
    bitnumtbl = {}
    numchars = 0
    numbits = 1
    while True:
        bitnum = br.get(4)  # SQgetnum lee 4 bits
        bitnumtbl[numbits] = bitnum
        numchars += bitnum
        numbits += 1
        if bitnum == 0:
            break
    mostbits = numbits - 1

    # leap-decode de los caracteres
    leap = [0] * 256
    codetbl = []
    nextchar = 255  # empieza en -1 (unsigned wrap)
    for _ in range(numchars):
        leapdelta = br.get(4) + 1
        while leapdelta:
            nextchar = (nextchar + 1) & 0xFF
            if not leap[nextchar]:
                leapdelta -= 1
        leap[nextchar] = 1
        codetbl.append(nextchar)

    # tabla rapida (8 bits lookahead)
    quicklen = [64] * 256
    quickcode = [0] * 256
    ci = 0
    for bits in range(1, min(mostbits + 1, 9)):
        n = bitnumtbl.get(bits, 0)
        entries = 1 << (8 - bits)
        for _ in range(n):
            c = codetbl[ci]; ci += 1
            ln = bits
            if c == clue:
                cluelen = bits
                ln = 96
            for _ in range(entries):
                idx = len([x for x in quicklen if x == 64])  # proxima libre
                # simplificado: llenar secuencial
                pass
            break  # simplificado: un solo nivel por ahora

    # descompresion bit a bit (lenta pero correcta)
    out = bytearray()
    codes = {}
    code = 0
    for bits in range(1, mostbits + 1):
        n = bitnumtbl.get(bits, 0)
        for _ in range(n):
            codes[(code, bits)] = codetbl[len(out) if False else 0]  # placeholder
            code += 1
        code <<= 1

    # version simple bit-a-bit:
    out = bytearray()
    cur = 0
    curlen = 0
    code = 0
    assigned = 0
    # construir el diccionario codigo->char correctamente (canonico):
    codemap = {}
    code = 0
    for bits in range(1, mostbits + 1):
        for _ in range(bitnumtbl.get(bits, 0)):
            codemap[(code, bits)] = codetbl[assigned]
            assigned += 1
            code += 1
        code <<= 1

    while len(out) < ulen:
        cur = (cur << 1) | br.get(1)
        curlen += 1
        if (cur, curlen) in codemap:
            c = codemap[(cur, curlen)]
            if c == clue:
                # RLE: los siguientes bits son la longitud
                rep = 0
                while True:
                    b = br.get(1)
                    if not b: break
                    rep += 1
                cnt = (1 << rep) + br.get(rep if rep else 0) if rep else 1
                out.extend(bytes([out[-1] if out else 0]) * cnt)
            else:
                out.append(c)
            cur = 0
            curlen = 0
        if curlen > mostbits + 1:
            break
    return bytes(out[:ulen])


if __name__ == "__main__":
    d = open("build/GAMEPLAY.BIN", "rb").read()
    # probar con el chunk HUFF @0x14cda0
    p = d.find(b"HUFF\x01\x00")
    if p < 0: p = d.find(b"HUFF")
    sz, = struct.unpack("<I", d[p+4:p+8])
    result = huff_decompress(d[p+8:p+8+sz])
    print(f"descomprimido: {len(result)} bytes")
    print("preview:", result[:32].hex(" ", 8))
