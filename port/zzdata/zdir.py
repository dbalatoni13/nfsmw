#!/usr/bin/env python3
"""Lector del ZDIR.BIN (tabla de assets del NFS: Most Wanted GC).

Formato: 1363 entradas de 24 bytes, little-endian, ordenadas por hash:
  Hash(4) - bStringHash del nombre (mayusculas, \ como separador, h*33+c)
  FileNumber(4) - indice del ZZDATAn.BIN (0-3)
  LocalSectorOffset(4) - sector relativo dentro del volumen
  TotalSectorOffset(4) - sector global (para el seek del DVD)
  Size(4) - tamano en bytes
  Checksum(4) - CRC/suma del contenido

Uso: python zdir.py <iso> [nombre-de-archivo]
"""
import struct
import sys

ZDIR_ISO_OFFSET = 0x478000
ZDIR_ISO_SIZE = 0x7FC8
ZZDATA_BASES = [0x47FFFC8, 0x131267C8, 0x275A0FC8, 0x3B146FC8]  # ISO abs


def bstring_hash(text):
    h = 0xFFFFFFFF
    for c in text.encode():
        h = ((h << 5) + h + c) & 0xFFFFFFFF
    return h


def file_hash(path):
    return bstring_hash(path.upper().replace("/", "\\"))


def load(iso):
    zz = iso[ZDIR_ISO_OFFSET:ZDIR_ISO_OFFSET + ZDIR_ISO_SIZE]
    n = len(zz) // 24
    entries = []
    for i in range(n):
        h, fn, lso, tso, sz, chk = struct.unpack("<IIIIII", zz[i*24:(i+1)*24])
        entries.append({
            "hash": h, "file": fn, "local_sector": lso,
            "total_sector": tso, "size": sz, "checksum": chk,
            "iso_offset": ZZDATA_BASES[fn] + lso * 0x800,
        })
    entries.sort(key=lambda e: e["hash"])
    return entries


def find(entries, name):
    h = file_hash(name)
    lo, hi = 0, len(entries)
    while lo < hi:
        mid = (lo + hi) // 2
        if entries[mid]["hash"] < h:
            lo = mid + 1
        elif entries[mid]["hash"] > h:
            hi = mid
        else:
            return entries[mid]
    return None


if __name__ == "__main__":
    iso = open(sys.argv[1], "rb").read()
    entries = load(iso)
    print(f"{len(entries)} entradas cargadas")
    if len(sys.argv) > 2:
        e = find(entries, sys.argv[2])
        if e:
            print(f"{sys.argv[2]}: ZZDATA{e['file']}.BIN @ sector {e['local_sector']} "
                  f"(ISO {e['iso_offset']:#x}), {e['size']} bytes")
        else:
            print(f"no encontrado (hash={file_hash(sys.argv[2]):#010x})")
    else:
        for e in entries[:5]:
            print(f"  hash={e['hash']:#010x} ZZDATA{e['file']} sector={e['local_sector']} size={e['size']}")
