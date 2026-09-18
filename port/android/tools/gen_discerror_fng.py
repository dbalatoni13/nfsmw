#!/usr/bin/env python3
"""Write a little-endian FEng package matching FEPackageChunks / FEPackageReader."""
import struct
from pathlib import Path

NESTED = 0x80000000


def fe_id(a, b, c, d):
    return ord(a) | (ord(b) << 8) | (ord(c) << 16) | (ord(d) << 24)


def fe_tag(a, b):
    return ord(a) | (ord(b) << 8)


CH_FENG = fe_id("F", "E", "n", "g") | NESTED
CH_PKHD = fe_id("P", "k", "H", "d")
CH_TYPS = fe_id("T", "y", "p", "S")
CH_RESL = fe_id("R", "e", "s", "L") | NESTED
CH_RSNM = fe_id("R", "s", "N", "m")
CH_RSRQ = fe_id("R", "s", "R", "q")
CH_OBJL = fe_id("O", "b", "j", "L") | NESTED
CH_FOBJ = fe_id("F", "O", "b", "j") | NESTED
CH_OBJD = fe_id("O", "b", "j", "D")
TG_OT = fe_tag("O", "t")
TG_ON = fe_tag("O", "n")
TG_OH = fe_tag("O", "h")
TG_OP = fe_tag("O", "P")
TG_SA = fe_tag("S", "A")

FE_IMAGE = 1
RT_IMAGE = 1


def chunk(cid, payload):
    return struct.pack("<II", cid, len(payload)) + payload


def tag(tid, payload):
    return struct.pack("<HH", tid, len(payload)) + payload


def pad4(b):
    return b + (b"\x00" * ((4 - (len(b) & 3)) & 3))


def sa_image(col_b, col_g, col_r, col_a, px, py, sx, sy, u0=0.0, v0=0.0, u1=1.0, v1=1.0):
    vals = [
        ("i", col_b),
        ("i", col_g),
        ("i", col_r),
        ("i", col_a),
        ("f", 0.0),
        ("f", 0.0),
        ("f", 0.0),
        ("f", px),
        ("f", py),
        ("f", 0.0),
        ("f", 0.0),
        ("f", 0.0),
        ("f", 0.0),
        ("f", 1.0),
        ("f", sx),
        ("f", sy),
        ("f", 1.0),
        ("f", u0),
        ("f", v0),
        ("f", u1),
        ("f", v1),
    ]
    return b"".join(struct.pack("<" + t, v) for t, v in vals)


def obj_image(name, guid, flags, res_index, sa):
    name_b = pad4(name.encode("ascii") + b"\x00")
    tags = b"".join(
        [
            tag(TG_OT, struct.pack("<I", FE_IMAGE)),
            tag(TG_ON, name_b),
            tag(TG_OH, struct.pack("<I", guid)),
            tag(TG_OP, struct.pack("<IIII", guid, guid, flags, res_index)),
            tag(TG_SA, sa),
        ]
    )
    return chunk(CH_FOBJ, chunk(CH_OBJD, tags))


def main():
    short = b"DiscError\x00"
    fname = b"DiscError.fng\x00"
    hdr = struct.pack("<IIIIII", 0x00020000, 0, 1, 4, len(short), 0) + short + fname
    typs = struct.pack("<II", FE_IMAGE, 0x54)
    names = b"logo.tga\x00"
    req = struct.pack("<I", 1) + struct.pack("<IIIIII", 1, 0, RT_IMAGE, 0, 0, 0)
    resl = chunk(CH_RSNM, names) + chunk(CH_RSRQ, req)
    objs = b"".join(
        [
            obj_image("BACKDROP", 1, 0, 0xFFFF, sa_image(0, 0, 0, 160, 0, 0, 640, 480)),
            obj_image("PANEL", 2, 0, 0xFFFF, sa_image(16, 16, 12, 230, 0, 16, 368, 200)),
            obj_image("TITLE", 3, 0, 0xFFFF, sa_image(10, 106, 255, 255, 0, -68, 368, 32)),
            obj_image("BADGE", 4, 0, 0, sa_image(255, 255, 255, 255, 0, 24, 192, 64)),
        ]
    )
    root = b"".join(
        [
            chunk(CH_PKHD, hdr),
            chunk(CH_TYPS, typs),
            chunk(CH_RESL, resl),
            chunk(CH_OBJL, objs),
        ]
    )
    blob = chunk(CH_FENG, root)
    out = Path(__file__).resolve().parents[1] / "app" / "src" / "main" / "assets" / "DiscError.fng"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(blob)
    print("wrote", out, "bytes", len(blob))


if __name__ == "__main__":
    main()
