# -------------------------------------------------------------------------------
# Export debug line info from an ELF that contains GCC DWARF1 data
# Credits: Brawltendo
# -------------------------------------------------------------------------------
from __future__ import print_function
import io
import os
import struct
import sys

from elftools.elf.elffile import ELFFile
from typing import NamedTuple
from dataclasses import dataclass


def formatted_read(srcdata, format, numbytes):
    return struct.unpack(format, srcdata.read(numbytes))[0]


@dataclass
class LineEntry:
    # the line number in the source file that corresponds to the current instruction
    lineNum: int
    # position on the line at which a statement occurs, value is 0xffff if the position doesn't need to be determined
    statementPos: int
    # offset from this compile unit's base address to the current instruction
    addrDelta: int


@dataclass
class LineSectionChunk:
    # the size of this compile unit's .line section
    sectionSize: int
    # the virtual address that this compile unit starts at
    cuStartAddress: int


class SrcinfoEntry(NamedTuple):
    # offset into the .line section
    lineEntryOffset: int
    # offset into the .debug_sfnames section
    sfNameOffset: int


class SrcinfoSectionChunk(NamedTuple):
    # offset to a LineSectionChunk from the beginning of the .line section
    cuLineOffset: int
    # offset into the .debug_sfnames section for this compile unit
    cuSfnamesOffset: int
    # starting offset for this compile unit's .text section
    cuTextStartOffset: int
    # ending offset for this compile unit's .text section
    cuTextEndOffset: int
    # if specified by the compiler, describes the date/time at which this compile unit was compiled
    # defaults to 0xffffffff otherwise
    compileDateTime: int
    # a list of SrcinfoEntry data for this compile unit
    entries: list
    finalLineEntryOffset: int


def process_file(filename, outdirectory):
    print("Processing file:", filename)
    with open(filename, "rb") as f:
        elffile = ELFFile(f)
        srcinfoSection = elffile.get_section_by_name(".debug_srcinfo")
        srcinfoChunks = []

        with io.BytesIO(srcinfoSection.data()) as srcinfoData:
            # read each compile unit chunk in the .debug_srcinfo section
            while srcinfoData.tell() < srcinfoSection.data_size:
                cuLineOffset = formatted_read(srcinfoData, ">I", 4)
                cuSfnamesOffset = formatted_read(srcinfoData, ">I", 4)
                cuTextStartOffset = formatted_read(srcinfoData, ">I", 4)
                cuTextEndOffset = formatted_read(srcinfoData, ">I", 4)
                compileDateTime = formatted_read(srcinfoData, ">I", 4)
                srcInfoEntries = []

                # get all srcinfo entries for the current chunk
                while True:
                    # if this is the final entry, the value will be the offset to the final line entry for this compile unit's .line section
                    lineEntryOffset = formatted_read(srcinfoData, ">I", 4)
                    # acts as a terminator with the value 0xffffffff if this is the final entry
                    sfNameOffset = formatted_read(srcinfoData, ">I", 4)

                    if sfNameOffset == 0xFFFFFFFF:
                        srcinfoChunks.append(
                            SrcinfoSectionChunk(
                                cuLineOffset,
                                cuSfnamesOffset,
                                cuTextStartOffset,
                                cuTextEndOffset,
                                compileDateTime,
                                srcInfoEntries,
                                lineEntryOffset,
                            )
                        )
                        # break out and start reading the next srcinfo chunk
                        break
                    else:
                        srcInfoEntries.append(
                            SrcinfoEntry(lineEntryOffset, sfNameOffset)
                        )

        with open(os.path.join(outdirectory, "debug_lines.txt"), "w") as outfile:
            # get the line data for each srcinfo chunk
            lineSection = elffile.get_section_by_name(".line")
            with io.BytesIO(lineSection.data()) as lineData:
                sfnamesSection = elffile.get_section_by_name(".debug_sfnames")
                sfnamesData = io.BytesIO(sfnamesSection.data())

                for srcinfoChunk in srcinfoChunks:
                    lineData.seek(srcinfoChunk.cuLineOffset)

                    lineChunk = LineSectionChunk(0, 0)
                    lineChunk.sectionSize = formatted_read(lineData, ">I", 4)
                    lineChunk.cuStartAddress = formatted_read(lineData, ">I", 4)

                    for idx, entry in enumerate(srcinfoChunk.entries):
                        # jump to the entry and name offsets relative to this chunk's sections
                        # comments in GCC's source code say otherwise, but this is the correct way
                        lineData.seek(srcinfoChunk.cuLineOffset + entry.lineEntryOffset)
                        sfnamesData.seek(
                            srcinfoChunk.cuSfnamesOffset + entry.sfNameOffset
                        )
                        sourceFile = "".join(
                            iter(lambda: sfnamesData.read(1).decode("utf-8"), "\x00")
                        )

                        # one srcinfo entry can (implicitly) point to multiple line entries for that source file
                        # so keep reading line entries until we're at the one the next srcinfo entry points to (or we've run out of entries)
                        while True:
                            lineEntry = LineEntry(0, 0, 0)
                            lineEntry.lineNum = formatted_read(lineData, ">I", 4)
                            lineEntry.statementPos = formatted_read(lineData, ">H", 2)
                            lineEntry.addrDelta = formatted_read(lineData, ">I", 4)

                            addrString = format(
                                lineChunk.cuStartAddress + lineEntry.addrDelta, "08X"
                            )
                            lineNumString = str
                            if lineEntry.statementPos != 0xFFFF:
                                lineNumString = f"(line {str(lineEntry.lineNum)}, column {str(lineEntry.statementPos)})"
                            else:
                                lineNumString = f"(line {str(lineEntry.lineNum)})"

                            outLine = f"{addrString}: {sourceFile} {lineNumString}"
                            outfile.write(outLine + "\n")

                            maxEntry = len(srcinfoChunk.entries) - 1
                            if (
                                idx == maxEntry
                                or lineData.tell()
                                >= srcinfoChunk.cuLineOffset
                                + srcinfoChunk.entries[idx + 1].lineEntryOffset
                            ):
                                break


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Expected usage: {0} <input file> <output directory>".format(sys.argv[0]))
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2])
