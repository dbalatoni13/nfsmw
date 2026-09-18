#!/usr/bin/env python3
"""Escanea los volumenes ZZDATA de la ISO GC y mapea los contenedores EA.

Formatos conocidos (del analisis de la ISO GOWE69):
  MVhd  - MultiVolume header: indice de archivos virtuales dentro del volumen
  SCHl  - stream comprimido (audio/video)
Los offsets/volumenes estan en orig/GOWE69 (extraidos de la ISO).

Uso: python zzdata_scan.py <ruta-a-volumen-zzdata.bin>
"""
import struct
import sys

VOLUMES = {
    # nombre: (offset ISO, tamano) — del FST de la ISO GOWE69
    "ZZDIR.BIN":   (0x04780000, 0x00007FC8),
    "ZZDATA0.BIN": (0x047FFFC8, 0x12CA6800),
    "ZZDATA1.BIN": (0x131267C8, 0x1447A800),
    "ZZDATA2.BIN": (0x275A0FC8, 0x13BA6000),
    "ZZDATA3.BIN": (0x3B146FC8, 0x1447A800),
}

def scan_chunk_headers(data, base=0):
    """Encuentra cabeceras de 4 letras conocidas y reporta offset+version."""
    magicos = {b"MVhd": "MultiVolume", b"SCHl": "StreamCH", b"MVhd": "MV"}
    pos = 0
    encontrados = []
    while True:
        pos = data.find(b"MVhd", pos)
        if pos < 0: break
        ver, = struct.unpack(">H", data[pos+4:pos+6])
        encontrados.append((pos, "MVhd", ver))
        pos += 4
    pos = 0
    while True:
        pos = data.find(b"SCHl", pos)
        if pos < 0: break
        size, = struct.unpack(">I", data[pos+4:pos+8])
        encontrados.append((pos, "SCHl", size))
        pos += 4
    return sorted(encontrados)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(1)
    data = open(sys.argv[1], "rb").read()
    print(f"volumen: {sys.argv[1]} ({len(data)} bytes)")
    for off, magic, info in scan_chunk_headers(data)[:20]:
        print(f"  {off:#010x}  {magic}  {info}")


def parse_chunks(data, start, end):
    """Parser de chunks EA: magic(4) + size_LE(4) + payload. Devuelve lista."""
    pos = start
    out = []
    while pos < end - 8:
        magic = data[pos:pos+4]
        size, = struct.unpack("<I", data[pos+4:pos+8])
        if not all(65 <= c <= 122 for c in magic) or size > 0x1000000 or size == 0:
            pos += 1
            continue
        out.append((magic.decode(errors="replace"), pos, size))
        pos += 8 + size
    return out


def extract_stream(iso, schl_off, out_path):
    """Extrae un stream SCHl completo (audio/canal) a un archivo."""
    size, = struct.unpack("<I", iso[schl_off+4:schl_off+8])
    with open(out_path, "wb") as f:
        f.write(iso[schl_off:schl_off+8+size])
    return size
