#!/usr/bin/env python3

from __future__ import annotations

import argparse
import struct
import sys
from dataclasses import dataclass
from pathlib import Path


def p32(value: int) -> bytes:
    return struct.pack("<I", value & 0xFFFFFFFF)


def p32s(value: int) -> bytes:
    return struct.pack("<i", value)


@dataclass(frozen=True)
class Section:
    name: str
    virtual_size: int
    virtual_address: int
    raw_size: int
    raw_pointer: int

    @property
    def mapped_size(self) -> int:
        return max(self.virtual_size, self.raw_size)


class PEImage:
    def __init__(self, path: Path):
        self.path = path
        self.data = bytearray(path.read_bytes())

        pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise RuntimeError(f"{path} is not a PE executable")

        magic = struct.unpack_from("<H", self.data, pe_offset + 24)[0]
        if magic != 0x10B:
            raise RuntimeError(f"{path} has unsupported PE magic 0x{magic:x}")

        self.image_base = struct.unpack_from("<I", self.data, pe_offset + 24 + 28)[0]
        section_count = struct.unpack_from("<H", self.data, pe_offset + 6)[0]
        optional_size = struct.unpack_from("<H", self.data, pe_offset + 20)[0]
        section_offset = pe_offset + 24 + optional_size

        self.sections: dict[str, Section] = {}
        self.section_offsets: dict[str, int] = {}
        for index in range(section_count):
            offset = section_offset + 40 * index
            name = (
                self.data[offset : offset + 8].rstrip(b"\0").decode("ascii", errors="ignore")
            )
            virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections[name] = Section(
                name=name,
                virtual_size=virtual_size,
                virtual_address=virtual_address,
                raw_size=raw_size,
                raw_pointer=raw_pointer,
            )
            self.section_offsets[name] = offset

    def va_to_offset(self, va: int) -> int:
        rva = va - self.image_base
        for section in self.sections.values():
            if section.virtual_address <= rva < section.virtual_address + section.mapped_size:
                return section.raw_pointer + (rva - section.virtual_address)
        raise RuntimeError(f"RVA 0x{rva:x} is not mapped in {self.path}")

    def read(self, va: int, size: int) -> bytes:
        offset = self.va_to_offset(va)
        return bytes(self.data[offset : offset + size])

    def write(self, va: int, payload: bytes) -> None:
        offset = self.va_to_offset(va)
        self.data[offset : offset + len(payload)] = payload

    def read_u32(self, va: int) -> int:
        return struct.unpack("<I", self.read(va, 4))[0]

    def text_cave(self, pad: int = 0x20) -> tuple[int, int]:
        text = self.sections[".text"]
        next_rva = min(
            section.virtual_address
            for section in self.sections.values()
            if section.virtual_address > text.virtual_address
        )
        cave_rva = text.virtual_address + text.virtual_size + pad
        cave_limit = min(text.virtual_address + text.mapped_size, next_rva)
        if cave_rva >= cave_limit:
            raise RuntimeError(f"no executable cave available in {self.path}")
        return self.image_base + cave_rva, cave_limit - cave_rva

    def write_out(self, path: Path) -> None:
        path.write_bytes(self.data)

    def set_virtual_size(self, section_name: str, virtual_size: int) -> None:
        struct.pack_into("<I", self.data, self.section_offsets[section_name] + 8, virtual_size)


class Asm:
    def __init__(self, base_va: int):
        self.base_va = base_va
        self.buf = bytearray()

    @property
    def here(self) -> int:
        return self.base_va + len(self.buf)

    def raw(self, payload: bytes) -> None:
        self.buf.extend(payload)

    def push_imm(self, imm: int) -> None:
        self.buf.extend(b"\x68")
        self.buf.extend(p32(imm))

    def push_membp(self, disp: int) -> None:
        self.buf.extend((0xFF, 0x75, disp & 0xFF))

    def push_eax(self) -> None:
        self.buf.extend(b"\x50")

    def push_ecx(self) -> None:
        self.buf.extend(b"\x51")

    def push_edx(self) -> None:
        self.buf.extend(b"\x52")

    def mov_eax_moffs(self, addr: int) -> None:
        self.buf.extend(b"\xA1")
        self.buf.extend(p32(addr))

    def mov_eax_ptr_edi(self, disp: int) -> None:
        self.buf.extend((0x8B, 0x47, disp & 0xFF))

    def mov_edx_ptr_edi(self, disp: int) -> None:
        self.buf.extend((0x8B, 0x57, disp & 0xFF))

    def mov_ecx_indexed(self, base: int) -> None:
        self.buf.extend((0x8B, 0x0C, 0x85))
        self.buf.extend(p32(base))

    def call(self, target: int) -> None:
        rel = target - (self.here + 5)
        self.buf.extend(b"\xE8")
        self.buf.extend(p32s(rel))

    def add_esp(self, amount: int) -> None:
        self.buf.extend((0x83, 0xC4, amount))

    def prolog(self) -> None:
        self.buf.extend(b"\x55\x8B\xEC\x53\x56\x57")

    def epilog(self) -> None:
        self.buf.extend(b"\x5F\x5E\x5B\x5D\xC3")


@dataclass(frozen=True)
class CallPatch:
    site: int
    original_target: int
    helper_name: str


@dataclass(frozen=True)
class PushPatch:
    site: int
    original_imm: int
    replacement_name: str


@dataclass(frozen=True)
class Manifest:
    profile: str
    style: str
    description: str
    format_out: int
    asm_out_file_ptr: int
    call_patches: tuple[CallPatch, ...]
    output_constant: int | None = None
    pool_real: int | None = None
    pool_int: int | None = None
    label_prefix: int | None = None
    mode_size_table: int | None = None
    label_left: int | None = None
    label_right: int | None = None
    plain_label: bool = False
    push_patches: tuple[PushPatch, ...] = ()


def build_gamecube_layout(manifest: Manifest, cave_base: int) -> tuple[bytes, dict[str, int]]:
    if (
        manifest.output_constant is None
        or manifest.pool_real is None
        or manifest.pool_int is None
        or manifest.label_prefix is None
        or manifest.mode_size_table is None
    ):
        raise RuntimeError(f"profile {manifest.profile} is missing GameCube fields")

    strings_base = cave_base + 0x200
    string_addrs = {
        "label_fmt": strings_base,
        "type_fmt": strings_base + len(b"$%s%d:\n\0"),
        "size_fmt": strings_base + len(b"$%s%d:\n\0") + len(b"\t.type\t$%s%d,@object\n\0"),
        "star_fmt": strings_base
        + len(b"$%s%d:\n\0")
        + len(b"\t.type\t$%s%d,@object\n\0")
        + len(b"\t.size\t$%s%d,%d\n\0"),
    }

    asm = Asm(cave_base)

    helpers: dict[str, int] = {}

    helpers["label"] = asm.here
    asm.prolog()
    asm.push_membp(0x14)
    asm.push_membp(0x10)
    asm.push_imm(string_addrs["type_fmt"])
    asm.push_membp(0x08)
    asm.call(manifest.format_out)
    asm.add_esp(0x10)
    asm.push_membp(0x14)
    asm.push_membp(0x10)
    asm.push_imm(string_addrs["label_fmt"])
    asm.push_membp(0x08)
    asm.call(manifest.format_out)
    asm.add_esp(0x10)
    asm.epilog()

    helpers["output_constant"] = asm.here
    asm.prolog()
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.output_constant)
    asm.add_esp(0x08)
    asm.raw(b"\x8B\x45\x28")
    asm.push_membp(0x0C)
    asm.push_eax()
    asm.push_imm(manifest.label_prefix)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x14)
    asm.epilog()

    helpers["pool_real"] = asm.here
    asm.prolog()
    asm.push_membp(0x10)
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.pool_real)
    asm.add_esp(0x0C)
    asm.mov_eax_ptr_edi(0x08)
    asm.mov_ecx_indexed(manifest.mode_size_table)
    asm.mov_edx_ptr_edi(0x10)
    asm.push_ecx()
    asm.push_edx()
    asm.push_imm(manifest.label_prefix)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x14)
    asm.epilog()

    helpers["pool_int"] = asm.here
    asm.prolog()
    asm.push_membp(0x10)
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.pool_int)
    asm.add_esp(0x0C)
    asm.mov_eax_ptr_edi(0x08)
    asm.mov_ecx_indexed(manifest.mode_size_table)
    asm.mov_edx_ptr_edi(0x10)
    asm.push_ecx()
    asm.push_edx()
    asm.push_imm(manifest.label_prefix)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x14)
    asm.epilog()

    if len(asm.buf) > 0x200:
        raise RuntimeError(f"profile {manifest.profile} helper code exceeds reserved cave size")

    code_and_strings = bytearray(0x200)
    code_and_strings[: len(asm.buf)] = asm.buf

    string_payloads = {
        "label_fmt": b"$%s%d:\n\0",
        "type_fmt": b"\t.type\t$%s%d,@object\n\0",
        "size_fmt": b"\t.size\t$%s%d,%d\n\0",
        "star_fmt": b"*$%s%d\0",
    }
    for name, payload in string_payloads.items():
        start = string_addrs[name] - cave_base
        end = start + len(payload)
        if end > len(code_and_strings):
            code_and_strings.extend(b"\0" * (end - len(code_and_strings)))
        code_and_strings[start:end] = payload

    helpers["star_fmt"] = string_addrs["star_fmt"]
    return bytes(code_and_strings), helpers


def build_old_layout(manifest: Manifest, cave_base: int) -> tuple[bytes, dict[str, int]]:
    if (
        manifest.output_constant is None
        or manifest.pool_real is None
        or manifest.pool_int is None
        or manifest.label_left is None
        or manifest.label_right is None
    ):
        raise RuntimeError(f"profile {manifest.profile} is missing old-layout fields")

    strings_base = cave_base + 0x200
    string_addrs = {
        "label_fmt": strings_base,
        "type_fmt": strings_base + len(b"%s%s%d:\n\0"),
        "size_fmt": strings_base + len(b"%s%s%d:\n\0") + len(b"\t.type\t%s%s%d,@object\n\0"),
        "dollar": strings_base
        + len(b"%s%s%d:\n\0")
        + len(b"\t.type\t%s%s%d,@object\n\0")
        + len(b"\t.size\t%s%s%d,%d\n\0"),
    }
    left_addr = string_addrs["dollar"] if manifest.plain_label else manifest.label_left
    right_addr = manifest.label_right

    asm = Asm(cave_base)
    helpers: dict[str, int] = {}

    helpers["label"] = asm.here
    asm.prolog()
    asm.push_membp(0x18)
    asm.push_imm(right_addr)
    asm.push_imm(left_addr)
    asm.push_imm(string_addrs["type_fmt"])
    asm.push_membp(0x08)
    asm.call(manifest.format_out)
    asm.add_esp(0x14)
    asm.push_membp(0x18)
    asm.push_imm(right_addr)
    asm.push_imm(left_addr)
    asm.push_imm(string_addrs["label_fmt"])
    asm.push_membp(0x08)
    asm.call(manifest.format_out)
    asm.add_esp(0x14)
    asm.epilog()

    helpers["output_constant"] = asm.here
    asm.prolog()
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.output_constant)
    asm.add_esp(0x08)
    asm.raw(b"\x8B\x45\x28")
    asm.push_membp(0x0C)
    asm.push_eax()
    asm.push_imm(right_addr)
    asm.push_imm(left_addr)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x18)
    asm.epilog()

    helpers["pool_real"] = asm.here
    asm.prolog()
    asm.push_membp(0x10)
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.pool_real)
    asm.add_esp(0x0C)
    asm.mov_eax_ptr_edi(0x14)
    asm.mov_edx_ptr_edi(0x10)
    asm.push_eax()
    asm.push_edx()
    asm.push_imm(right_addr)
    asm.push_imm(left_addr)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x18)
    asm.epilog()

    helpers["pool_int"] = asm.here
    asm.prolog()
    asm.push_membp(0x10)
    asm.push_membp(0x0C)
    asm.push_membp(0x08)
    asm.call(manifest.pool_int)
    asm.add_esp(0x0C)
    asm.mov_eax_ptr_edi(0x14)
    asm.mov_edx_ptr_edi(0x10)
    asm.push_eax()
    asm.push_edx()
    asm.push_imm(right_addr)
    asm.push_imm(left_addr)
    asm.push_imm(string_addrs["size_fmt"])
    asm.mov_eax_moffs(manifest.asm_out_file_ptr)
    asm.push_eax()
    asm.call(manifest.format_out)
    asm.add_esp(0x18)
    asm.epilog()

    if len(asm.buf) > 0x200:
        raise RuntimeError(f"profile {manifest.profile} helper code exceeds reserved cave size")

    code_and_strings = bytearray(0x200)
    code_and_strings[: len(asm.buf)] = asm.buf

    string_payloads = {
        "label_fmt": b"%s%s%d:\n\0",
        "type_fmt": b"\t.type\t%s%s%d,@object\n\0",
        "size_fmt": b"\t.size\t%s%s%d,%d\n\0",
        "dollar": b"$\0",
    }
    for name, payload in string_payloads.items():
        start = string_addrs[name] - cave_base
        end = start + len(payload)
        if end > len(code_and_strings):
            code_and_strings.extend(b"\0" * (end - len(code_and_strings)))
        code_and_strings[start:end] = payload

    helpers["left_prefix"] = left_addr
    return bytes(code_and_strings), helpers


def read_call_target(image: PEImage, site: int) -> int:
    op = image.read(site, 1)
    if op != b"\xE8":
        raise RuntimeError(f"0x{site:x} does not contain a call instruction")
    rel = struct.unpack("<i", image.read(site + 1, 4))[0]
    return site + 5 + rel


def patch_call(image: PEImage, site: int, expected_target: int, new_target: int) -> None:
    current_target = read_call_target(image, site)
    if current_target == new_target:
        return
    if current_target != expected_target:
        raise RuntimeError(
            f"0x{site:x} points to 0x{current_target:x}, expected 0x{expected_target:x}"
        )
    rel = new_target - (site + 5)
    image.write(site + 1, p32s(rel))


def patch_push_imm(image: PEImage, site: int, expected_imm: int, new_imm: int) -> None:
    op = image.read(site, 1)
    if op != b"\x68":
        raise RuntimeError(f"0x{site:x} does not contain a push imm32 instruction")
    current_imm = image.read_u32(site + 1)
    if current_imm == new_imm:
        return
    if current_imm != expected_imm:
        raise RuntimeError(
            f"0x{site:x} pushes 0x{current_imm:x}, expected 0x{expected_imm:x}"
        )
    image.write(site + 1, p32(new_imm))


MANIFESTS: dict[str, Manifest] = {
    "prodg393-gc-cc1": Manifest(
        profile="prodg393-gc-cc1",
        style="gamecube",
        description="ProDG 3.9.3 GameCube stock cc1.exe",
        format_out=0x005669FE,
        asm_out_file_ptr=0x005D04E4,
        output_constant=0x00565A20,
        pool_real=0x005630B0,
        pool_int=0x00562BA0,
        label_prefix=0x00598D00,
        mode_size_table=0x005B28F4,
        call_patches=(
            CallPatch(0x00564A0E, 0x005669FE, "label"),
            CallPatch(0x00564A22, 0x00565A20, "output_constant"),
            CallPatch(0x00564A3C, 0x00565A20, "output_constant"),
            CallPatch(0x005655C2, 0x005669FE, "label"),
            CallPatch(0x00565642, 0x005630B0, "pool_real"),
            CallPatch(0x00565656, 0x00562BA0, "pool_int"),
        ),
        push_patches=(
            PushPatch(0x0056357A, 0x0059871C, "star_fmt"),
            PushPatch(0x0056367F, 0x0059871C, "star_fmt"),
            PushPatch(0x00564FA1, 0x0059871C, "star_fmt"),
        ),
    ),
    "prodg393-gc-cc1plus": Manifest(
        profile="prodg393-gc-cc1plus",
        style="gamecube",
        description="ProDG 3.9.3 GameCube stock cc1plus.exe",
        format_out=0x005C3048,
        asm_out_file_ptr=0x0064A1A0,
        output_constant=0x005C1EF0,
        pool_real=0x005BF560,
        pool_int=0x005BF050,
        label_prefix=0x00603DEC,
        mode_size_table=0x0062AB50,
        call_patches=(
            CallPatch(0x005C0EBE, 0x005C3048, "label"),
            CallPatch(0x005C0ED2, 0x005C1EF0, "output_constant"),
            CallPatch(0x005C0EEC, 0x005C1EF0, "output_constant"),
            CallPatch(0x005C1A92, 0x005C3048, "label"),
            CallPatch(0x005C1B12, 0x005BF560, "pool_real"),
            CallPatch(0x005C1B26, 0x005BF050, "pool_int"),
        ),
        push_patches=(
            PushPatch(0x005BFA2A, 0x0060371C, "star_fmt"),
            PushPatch(0x005BFB2F, 0x0060371C, "star_fmt"),
            PushPatch(0x005C1451, 0x0060371C, "star_fmt"),
        ),
    ),
    "ps2-ee-991111-cc1": Manifest(
        profile="ps2-ee-991111-cc1",
        style="old-ps2",
        description="PS2 EE 2.9-ee-991111 stock cc1.exe",
        format_out=0x00570E18,
        asm_out_file_ptr=0x005E9F28,
        output_constant=0x00453140,
        pool_real=0x00450850,
        pool_int=0x004504B0,
        label_left=0x0059D670,
        label_right=0x0059D66C,
        plain_label=True,
        call_patches=(
            CallPatch(0x004522DB, 0x00570E18, "label"),
            CallPatch(0x004522F4, 0x00453140, "output_constant"),
            CallPatch(0x00452313, 0x00453140, "output_constant"),
            CallPatch(0x00452DB8, 0x00570E18, "label"),
            CallPatch(0x00452DDE, 0x004504B0, "pool_int"),
            CallPatch(0x00452E22, 0x00450850, "pool_real"),
        ),
        push_patches=(
            PushPatch(0x00450D28, 0x0059D62C, "left_prefix"),
            PushPatch(0x00450E2E, 0x0059D640, "left_prefix"),
        ),
    ),
    "ps2-ee-991111-cc1plus": Manifest(
        profile="ps2-ee-991111-cc1plus",
        style="old-ps2",
        description="PS2 EE 2.9-ee-991111 stock cc1plus.exe",
        format_out=0x005F2528,
        asm_out_file_ptr=0x00685284,
        output_constant=0x00453140,
        pool_real=0x00450850,
        pool_int=0x004504B0,
        label_left=0x0061E670,
        label_right=0x0061E66C,
        plain_label=True,
        call_patches=(
            CallPatch(0x004522DB, 0x005F2528, "label"),
            CallPatch(0x004522F4, 0x00453140, "output_constant"),
            CallPatch(0x00452313, 0x00453140, "output_constant"),
            CallPatch(0x00452DB8, 0x005F2528, "label"),
            CallPatch(0x00452DDE, 0x004504B0, "pool_int"),
            CallPatch(0x00452E22, 0x00450850, "pool_real"),
        ),
        push_patches=(
            PushPatch(0x00450D28, 0x0061E62C, "left_prefix"),
            PushPatch(0x00450E2E, 0x0061E640, "left_prefix"),
        ),
    ),
}


def patch_binary(manifest: Manifest, input_path: Path, output_path: Path) -> None:
    image = PEImage(input_path)
    cave_base, cave_capacity = image.text_cave()
    if manifest.style == "gamecube":
        cave_payload, helpers = build_gamecube_layout(manifest, cave_base)
    elif manifest.style == "old-ps2":
        cave_payload, helpers = build_old_layout(manifest, cave_base)
    else:
        raise RuntimeError(f"unknown profile style {manifest.style!r}")

    if len(cave_payload) > cave_capacity:
        raise RuntimeError(
            f"profile {manifest.profile} needs 0x{len(cave_payload):x} cave bytes, "
            f"but only 0x{cave_capacity:x} are available"
        )

    image.write(cave_base, cave_payload)

    for patch in manifest.call_patches:
        patch_call(image, patch.site, patch.original_target, helpers[patch.helper_name])

    for patch in manifest.push_patches:
        patch_push_imm(image, patch.site, patch.original_imm, helpers[patch.replacement_name])

    image.write_out(output_path)


def patch_ngcld(input_path: Path, output_path: Path) -> None:
    """Patch ProDG's linker to accept host-style paths in response files."""
    image = PEImage(input_path)
    cave_base, cave_capacity = image.text_cave(pad=0)
    if cave_base != 0x004267F0:
        raise RuntimeError(f"unexpected ngcld .text layout (cave at 0x{cave_base:x})")

    payload = bytes.fromhex(
        "8b460450e82800000083c40485c0741650e86ae0fdff83c40485c0740950"
        "e85de7fdff83c4048b3685f675d4e98e8bfeff535657558b5c241431ed89da"
        "8a0284c074093c3a750289d542ebf185ed741139dd740b807d010074058d7d"
        "01eb0431ed89df8b355414430085f674608b4e1485c9745389fa8a023a0175"
        "4b84c074044241ebf285ed743b8b461885c074388b401c85c0743189c189c2"
        "8a0284c0740e3c5c74043c2f75038d4a0142ebec89da39ea740a8a023a0175"
        "0d4241ebf28039007504c64611018b3685f675a085ed750489d8eb0231c05d"
        "5f5e5bc3"
    )
    if len(payload) > cave_capacity:
        raise RuntimeError(f"ngcld patch needs 0x{len(payload):x} cave bytes")

    hook_site = 0x0040F390
    original_hook = bytes.fromhex(
        "8b460450e8d754ffff83c40485c0740950e8ca5bffff83c4048b3685f675e1"
    )
    replacement_hook = b"\xe9" + p32s(cave_base - (hook_site + 5)) + b"\x90" * (len(original_hook) - 5)
    current_hook = image.read(hook_site, len(original_hook))
    if current_hook == original_hook:
        image.write(cave_base, payload)
        image.write(hook_site, replacement_hook)
        image.set_virtual_size(".text", 0x258CC)
    elif current_hook != replacement_hook:
        raise RuntimeError(f"0x{hook_site:x} does not contain the expected ngcld hook")
    image.write_out(output_path)


def patch_all(root: Path) -> None:
    prodg = root / "ProDG" / "3.9.3"
    for profile, filename in (
        ("prodg393-gc-cc1", "cc1.exe"),
        ("prodg393-gc-cc1plus", "cc1plus.exe"),
    ):
        path = prodg / filename
        if path.exists():
            patch_binary(MANIFESTS[profile], path, path)
            print(f"patched {path} with profile {profile}")
    ngcld = prodg / "ngcld.exe"
    if ngcld.exists():
        patch_ngcld(ngcld, ngcld)
        print(f"patched {ngcld} with profile prodg393-ngcld")


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Patch stock SN compiler and linker binaries for decompilation."
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="list supported binary profiles and exit",
    )
    parser.add_argument(
        "--all",
        type=Path,
        metavar="TOOLCHAIN",
        help="patch all supported ProDG binaries below TOOLCHAIN",
    )
    parser.add_argument("profile", nargs="?", help="profile name to patch")
    parser.add_argument("input", nargs="?", help="input executable")
    parser.add_argument(
        "output",
        nargs="?",
        help="output executable (defaults to patching the input file in place)",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    if args.list:
        for profile in sorted(MANIFESTS):
            manifest = MANIFESTS[profile]
            print(f"{profile}: {manifest.description}")
        print("prodg393-ngcld: ProDG 3.9.3 GameCube stock ngcld.exe")
        return 0

    if args.all:
        patch_all(args.all)
        return 0

    if not args.profile or not args.input:
        print("usage: patch-toolchain.py <profile> <input> [output]", file=sys.stderr)
        print("       patch-toolchain.py --list", file=sys.stderr)
        return 2

    try:
        manifest = MANIFESTS[args.profile]
    except KeyError:
        print(f"unknown profile {args.profile!r}", file=sys.stderr)
        print("use --list to see supported profiles", file=sys.stderr)
        return 2

    input_path = Path(args.input)
    output_path = Path(args.output) if args.output else input_path

    patch_binary(manifest, input_path, output_path)
    print(f"patched {output_path} with profile {manifest.profile}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
