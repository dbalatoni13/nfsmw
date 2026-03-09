#!/usr/bin/env python3

import sys


def filter_lines(input_file, output_file, search_string):
    with open(input_file, "r") as infile, open(output_file, "w") as outfile:
        for line in infile:
            if search_string in line:
                outfile.write(line)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python filter_file.py <input_file> <output_file> <search_string>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    search_string = sys.argv[3]

    filter_lines(input_file, output_file, search_string)
