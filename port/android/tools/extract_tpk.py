#!/usr/bin/env python3
"""Extract PC TPK textures (DXT1/3/5/RGBA) to TGA for the Android port."""
import struct
from pathlib import Path

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


def rgb565(c):
    r = ((c >> 11) & 31) * 255 // 31
    g = ((c >> 5) & 63) * 255 // 63
    b = (c & 31) * 255 // 31
    return r, g, b


def dxt_colors(c0, c1, punch):
    r0, g0, b0 = rgb565(c0)
    r1, g1, b1 = rgb565(c1)
    cols = [(r0, g0, b0, 255), (r1, g1, b1, 255)]
    if punch and c0 <= c1:
        cols.append(((r0 + r1) // 2, (g0 + g1) // 2, (b0 + b1) // 2, 255))
        cols.append((0, 0, 0, 0))
    else:
        cols.append(((2 * r0 + r1) // 3, (2 * g0 + g1) // 3, (2 * b0 + b1) // 3, 255))
        cols.append(((r0 + 2 * r1) // 3, (g0 + 2 * g1) // 3, (b0 + 2 * b1) // 3, 255))
    return cols


def decode_dxt1(src, w, h, punch=True):
    out = bytearray(w * h * 4)
    i = 0
    for by in range(0, h, 4):
        for bx in range(0, w, 4):
            c0, c1, bits = struct.unpack_from("<HHI", src, i)
            i += 8
            cols = dxt_colors(c0, c1, punch)
            for py in range(4):
                for px in range(4):
                    if bx + px >= w or by + py >= h:
                        bits >>= 2
                        continue
                    c = cols[bits & 3]
                    o = ((by + py) * w + (bx + px)) * 4
                    out[o : o + 4] = bytes(c)
                    bits >>= 2
    return bytes(out)


def decode_dxt3(src, w, h):
    out = bytearray(w * h * 4)
    i = 0
    for by in range(0, h, 4):
        for bx in range(0, w, 4):
            a0, a1 = struct.unpack_from("<II", src, i)
            alpha = a0 | (a1 << 32)
            i += 8
            c0, c1, bits = struct.unpack_from("<HHI", src, i)
            i += 8
            cols = dxt_colors(c0, c1, False)
            for py in range(4):
                for px in range(4):
                    if bx + px >= w or by + py >= h:
                        bits >>= 2
                        alpha >>= 4
                        continue
                    c = cols[bits & 3]
                    a = (alpha & 15) * 17
                    o = ((by + py) * w + (bx + px)) * 4
                    out[o : o + 4] = bytes((c[0], c[1], c[2], a))
                    bits >>= 2
                    alpha >>= 4
    return bytes(out)


def decode_dxt5(src, w, h):
    out = bytearray(w * h * 4)
    i = 0
    for by in range(0, h, 4):
        for bx in range(0, w, 4):
            a0 = src[i]
            a1 = src[i + 1]
            abits = int.from_bytes(src[i + 2 : i + 8], "little")
            i += 8
            alphas = [a0, a1]
            if a0 > a1:
                for n in range(1, 7):
                    alphas.append(((7 - n) * a0 + n * a1) // 7)
            else:
                for n in range(1, 5):
                    alphas.append(((5 - n) * a0 + n * a1) // 5)
                alphas.extend((0, 255))
            c0, c1, bits = struct.unpack_from("<HHI", src, i)
            i += 8
            cols = dxt_colors(c0, c1, False)
            for py in range(4):
                for px in range(4):
                    if bx + px >= w or by + py >= h:
                        bits >>= 2
                        abits >>= 3
                        continue
                    c = cols[bits & 3]
                    a = alphas[abits & 7]
                    o = ((by + py) * w + (bx + px)) * 4
                    out[o : o + 4] = bytes((c[0], c[1], c[2], a))
                    bits >>= 2
                    abits >>= 3
    return bytes(out)


def decode_bgra32(src, w, h):
    out = bytearray(w * h * 4)
    n = w * h
    for i in range(n):
        b, g, r, a = src[i * 4 : i * 4 + 4]
        out[i * 4 : i * 4 + 4] = bytes((r, g, b, a))
    return bytes(out)


def write_tga(path, rgba, w, h):
    hdr = bytearray(18)
    hdr[2] = 2
    hdr[12] = w & 255
    hdr[13] = (w >> 8) & 255
    hdr[14] = h & 255
    hdr[15] = (h >> 8) & 255
    hdr[16] = 32
    hdr[17] = 0x28
    bgra = bytearray(w * h * 4)
    for i in range(w * h):
        r, g, b, a = rgba[i * 4 : i * 4 + 4]
        bgra[i * 4 : i * 4 + 4] = bytes((b, g, r, a))
    path.write_bytes(bytes(hdr) + bytes(bgra))


def scale_nn(rgba, w, h, nw, nh):
    out = bytearray(nw * nh * 4)
    for y in range(nh):
        sy = y * h // nh
        for x in range(nw):
            sx = x * w // nw
            out[(y * nw + x) * 4 : (y * nw + x) * 4 + 4] = rgba[(sy * w + sx) * 4 : (sy * w + sx) * 4 + 4]
    return bytes(out)


def iter_tpk(data):
    pos, n = 0, len(data)
    while pos + 8 <= n:
        cid, sz = struct.unpack_from("<Ii", data, pos)
        if sz < 0 or pos + 8 + sz > n:
            pos += 4
            continue
        if cid == TPK:
            infos = None
            vram = None
            for c in children(data, pos):
                cc = struct.unpack_from("<I", data, c)[0]
                if cc == INFO:
                    for g in children(data, c):
                        if struct.unpack_from("<I", data, g)[0] == TEXS:
                            infos = g
                elif cc == DATA:
                    for g in children(data, c):
                        if struct.unpack_from("<I", data, g)[0] == DARR:
                            payload = g + 8
                            vram = (payload + 127) & ~127
            if infos is not None and vram is not None:
                yield data, infos, vram
        pos += 8 + sz


def textures(data, infos, vram):
    gs = struct.unpack_from("<i", data, infos + 4)[0]
    count = gs // 0x7C
    for t in range(count):
        base = infos + 8 + t * 0x7C
        name = data[base + 0x0C : base + 0x24].split(b"\x00")[0].decode("latin1", "ignore")
        place = struct.unpack_from("<i", data, base + 0x30)[0]
        imsz = struct.unpack_from("<i", data, base + 0x38)[0]
        w, h = struct.unpack_from("<hh", data, base + 0x44)
        fmt = data[base + 0x4A]
        src = data[vram + place : vram + place + max(imsz, 0)]
        yield name, w, h, fmt, src


def decode(fmt, src, w, h):
    if fmt == 34:
        return decode_dxt1(src, w, h)
    if fmt == 36:
        return decode_dxt3(src, w, h)
    if fmt == 38:
        return decode_dxt5(src, w, h)
    if fmt == 32:
        return decode_bgra32(src, w, h)
    return None


WANT = {
    "CIRCLE_2": "U2_loading_Circle.tga",
    "GREYCIRCLE": "GREYCIRCLE.tga",
    "WHITE16X16": "White16x16.tga",
    "FE_WHITE": "FE_WHITE.tga",
    "DEMO_SPLASH": "MwSplashBack.tga",
    "ROUNDED_CORNER": "45Corner.tga",
    "TRAX_BASE": "trax_base.tga",
    "TRAX_TAB": "trax_tab.tga",
    "INNER_STRIPES": "Inner_Stripes.tga",
    "BLACK_RING": "black_ring.tga",
    "BLACK_CIRCLE_BASE": "black_circle_base.tga",
}


def main():
    out = Path(__file__).resolve().parents[1] / "app" / "src" / "main" / "assets" / "tpk"
    out.mkdir(parents=True, exist_ok=True)
    files = [
        Path(r"F:\Need for Speed Most Wanted 2005\FRONTEND\FrontB.lzc"),
        Path(r"F:\Need for Speed Most Wanted 2005\GLOBAL\GLOBALB.BUN"),
    ]
    saved = set()
    for path in files:
        data = path.read_bytes()
        for blob, infos, vram in iter_tpk(data):
            for name, w, h, fmt, src in textures(blob, infos, vram):
                key = name.upper()
                if key not in WANT or WANT[key] in saved:
                    continue
                rgba = decode(fmt, src, w, h)
                if not rgba:
                    print("skip", name, "fmt", fmt)
                    continue
                dest_name = WANT[key]
                dw, dh = w, h
                if dest_name == "MwSplashBack.tga" and (w > 640 or h > 480):
                    rgba = scale_nn(rgba, w, h, 640, 480)
                    dw, dh = 640, 480
                dest = out / dest_name
                write_tga(dest, rgba, dw, dh)
                saved.add(dest_name)
                print("wrote", dest.name, dw, dh, "from", name, "fmt", fmt, dest.stat().st_size)

    print("saved", sorted(saved))


if __name__ == "__main__":
    main()
