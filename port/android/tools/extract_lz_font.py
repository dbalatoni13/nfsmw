#!/usr/bin/env python3
"""Decompress JDLZ FEng packages and extract RealFont + FONT_MW_TITLE DXT5."""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "zzdata"))
from jdlz_decompress import jdlz_decompress

ROOT = Path(__file__).resolve().parents[1] / "app" / "src" / "main" / "assets"
FNG = ROOT / "fng"
FONT = ROOT / "font"
FNG.mkdir(parents=True, exist_ok=True)
FONT.mkdir(parents=True, exist_ok=True)

TPK = 0xB3300000
INFO = 0xB3310000
DATA = 0xB3320000
TEXS = 0x33310004
DARR = 0x33320002


def children(data, off):
    _cid, sz = struct.unpack_from("<Ii", data, off)
    cur, end, out = off + 8, off + 8 + sz, []
    while cur + 8 <= end:
        _cc, cs = struct.unpack_from("<Ii", data, cur)
        if cs < 0 or cur + 8 + cs > end:
            break
        out.append(cur)
        cur += 8 + cs
    return out


def walk_bun(path):
    data = Path(path).read_bytes()
    n, i = len(data), 0
    while i + 8 <= n:
        cid, sz = struct.unpack_from("<Ii", data, i)
        if sz < 0 or i + 8 + sz > n:
            i += 4
            continue
        yield i, cid, sz, data[i + 8 : i + 8 + sz]
        i += 8 + sz


def main():
    globalb = Path(r"F:\Need for Speed Most Wanted 2005\GLOBAL\GLOBALB.BUN")
    frontb = Path(r"F:\Need for Speed Most Wanted 2005\FRONTEND\FrontB.lzc")
    for off, cid, sz, payload in walk_bun(globalb):
        if cid != 0x30210:
            continue
        lz = payload[4:]
        if lz[:4] != b"JDLZ":
            print("skip non-JDLZ", hex(struct.unpack_from("<I", payload, 0)[0]))
            continue
        out = jdlz_decompress(lz)
        name = out[0x28:0x48].split(b"\x00")[0].decode("latin1", "ignore") or "unk.fng"
        dest = FNG / name
        dest.write_bytes(out)
        print("lz", name, len(out))

    data = frontb.read_bytes()
    for off, cid, sz, payload in walk_bun(frontb):
        if cid == 0x30201:
            name = payload[:64].split(b"\x00")[0].decode("latin1", "ignore")
            dest = FONT / (name + ".bin")
            dest.write_bytes(payload)
            print("font", name, sz)
        if cid == 0x30203:
            name = payload[0x28:0x48].split(b"\x00")[0].decode("latin1", "ignore")
            if name and not (FNG / name).exists():
                (FNG / name).write_bytes(payload)
                print("fng", name, sz)

    # FONT_MW_TITLE mip0 DXT5
    n = len(data)
    i = 0
    while i + 8 <= n:
        cid, sz = struct.unpack_from("<Ii", data, i)
        if sz < 0 or i + 8 + sz > n:
            i += 4
            continue
        if cid == TPK:
            infos = vram = None
            for c in children(data, i):
                cc = struct.unpack_from("<I", data, c)[0]
                if cc == INFO:
                    for g in children(data, c):
                        if struct.unpack_from("<I", data, g)[0] == TEXS:
                            infos = g
                elif cc == DATA:
                    for g in children(data, c):
                        if struct.unpack_from("<I", data, g)[0] == DARR:
                            vram = (g + 8 + 127) & ~127
            if infos is not None and vram is not None:
                gs = struct.unpack_from("<i", data, infos + 4)[0]
                for t in range(gs // 0x7C):
                    base = infos + 8 + t * 0x7C
                    name = data[base + 0x0C : base + 0x24].split(b"\x00")[0].decode("latin1", "ignore")
                    if name.upper() != "FONT_MW_TITLE":
                        continue
                    place = struct.unpack_from("<i", data, base + 0x30)[0]
                    w, h = struct.unpack_from("<hh", data, base + 0x44)
                    mip0 = (w // 4) * (h // 4) * 16
                    blob = data[vram + place : vram + place + mip0]
                    hdr = struct.pack("<4sHH", b"DXT5", w, h) + blob
                    dest = FONT / "FONT_MW_TITLE.dxt5"
                    dest.write_bytes(hdr)
                    print("atlas", dest, w, h, len(hdr))
        i += 8 + sz


if __name__ == "__main__":
    main()
