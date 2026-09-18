#!/usr/bin/env python3
"""Pack RealFont glyphs into a 64x64 cell grid TGA + metrics bin."""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[0]))
from extract_tpk import decode_dxt5, write_tga

CELL = 64
COLS = 16
ROWS = 16
OUT = Path(__file__).resolve().parents[1] / "app" / "src" / "main" / "assets" / "font"


def main():
    raw = (OUT / "FONT_MW_TITLE.dxt5").read_bytes()
    aw, ah = struct.unpack_from("<HH", raw, 4)
    atlas = decode_dxt5(raw[8:], aw, ah)
    fbin = (OUT / "font_mw_title.bin").read_bytes()
    rf = fbin[0x200:]
    num = struct.unpack_from("<H", rf, 10)[0]
    gt = struct.unpack_from("<i", rf, 0x14)[0]
    grid = bytearray(COLS * CELL * ROWS * CELL * 4)
    mets = []
    n = min(num, COLS * ROWS)
    for i in range(n):
        o = gt + i * 16
        uni, w, h = struct.unpack_from("<HBB", rf, o)
        u, v = struct.unpack_from("<HH", rf, o + 4)
        offx, offy = struct.unpack_from("bb", rf, o + 9)
        advx = struct.unpack_from("<h", rf, o + 14)[0]
        cx, cy = (i % COLS) * CELL, (i // COLS) * CELL
        cw, ch = min(w, CELL), min(h, CELL)
        for yy in range(ch):
            for xx in range(cw):
                sx, sy = u + xx, v + yy
                if 0 <= sx < aw and 0 <= sy < ah:
                    a = atlas[(sy * aw + sx) * 4 + 3]
                else:
                    a = 0
                di = ((cy + yy) * COLS * CELL + (cx + xx)) * 4
                grid[di : di + 4] = bytes((255, 255, 255, a))
        mets.append((uni, w, h, offx, offy, advx, i))
    write_tga(OUT / "font_grid.tga", bytes(grid), COLS * CELL, ROWS * CELL)
    blob = struct.pack("<I", len(mets))
    for uni, w, h, offx, offy, advx, idx in mets:
        blob += struct.pack("<HbbBBhhI", uni, offx, offy, w, h, advx, 0, idx)
    (OUT / "font_grid.bin").write_bytes(blob)
    print("glyphs", len(mets), "grid", COLS * CELL, ROWS * CELL)


if __name__ == "__main__":
    main()
