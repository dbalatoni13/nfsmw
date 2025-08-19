# -------------------------------------------------------------------------------
# Generates empty source and header files from a text file that lists them
# -------------------------------------------------------------------------------
import sys
import os

HEADER_CONTENT = """#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif



#endif
"""


def process_file(filenames_file):
    print("Processing file:", filenames_file)

    with open(filenames_file, "r") as f:
        for line in f.readlines():
            if "speed/indep/src" not in line:
                continue
            file_path = line.replace("d:/mw", "src").strip()

            if os.path.exists(file_path):
                continue
            os.makedirs(os.path.dirname(file_path), exist_ok=True)
            print(file_path)
            with open(file_path, "w") as out_f:
                # .cpp files should be empty
                if file_path.endswith(".h") or file_path.endswith(".hpp"):
                    header_name = (
                        file_path.split("src/speed/indep/src/")[1]
                        .split(".h")[0]
                        .replace("/", "_")
                        .replace(".", "_")
                        .upper()
                    )
                    out_f.writelines(HEADER_CONTENT.replace("HEADER_NAME", header_name))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Expected usage: {0} <file with filenames>".format(sys.argv[0]))
        sys.exit(1)
    process_file(sys.argv[1])
