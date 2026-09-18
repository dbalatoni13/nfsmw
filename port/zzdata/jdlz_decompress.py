#!/usr/bin/env python3
"""Descompresor JDLZ de EA, portado directamente del JLZDecompress de LZCompress.cpp.

Header (16 bytes, LE):
  ID(4)='JDLZ' Version(1)=2 HeaderSize(1)=0x10 Flags(2)
  UncompressedSize(4) CompressedSize(4)
Tras el header: Control(1) RunType(1) + datos.
"""
import struct


def jdlz_decompress(data):
    if data[:4] != b'JDLZ':
        raise ValueError("no es JDLZ")
    ver = data[4]
    uncomp_size, comp_size = struct.unpack("<II", data[8:16])
    if ver != 2:
        raise ValueError(f"version {ver} no soportada")

    size = comp_size - 0x12
    out = bytearray()
    control = data[0x10] | 0x100
    runtype = data[0x11] | 0x100
    src = 0x12

    while size != 0:
        if control & 1:
            if runtype & 1:
                # run corto: offset 4bits + len 12bits
                run = ((data[src] >> 4) << 8 | data[src+1]) + 3
                offset = (data[src] & 0xF) + 1
            else:
                # run largo: offset 11bits + len 5bits
                offset = ((data[src] >> 5) << 8 | data[src+1]) + 0x11
                run = (data[src] & 0x1F) + 3
            # copiar del back-reference
            for _ in range(run):
                out.append(out[-offset])
            src += 2
            size -= 2
            runtype >>= 1
        else:
            if len(out) < uncomp_size:
                out.append(data[src])
                src += 1
            size -= 1
        control >>= 1
        if control == 1:
            size -= 1
            control = data[src] | 0x100
            src += 1
        if runtype == 1:
            size -= 1
            runtype = data[src] | 0x100
            src += 1
    return bytes(out[:uncomp_size])


if __name__ == "__main__":
    import sys
    d = open(sys.argv[1] if len(sys.argv) > 1 else "build/GAMEPLAY.BIN", "rb").read()
    pos = d.find(b"JDLZ")
    result = jdlz_decompress(d[pos:pos+0x1002])
    print(f"descomprimido: {len(result)} bytes")
    print("preview:", result[:32].hex(" ", 8))
