import csv
import sys


CODE_SECTIONS = {".text", ".noover", ".over", ".vutext"}
DATA_SECTIONS = {".rodata", ".data", ".bss"}
TARGET_SECTIONS = CODE_SECTIONS | DATA_SECTIONS
SOURCE_LISTS_MARKER = "sourcelists"


def is_sourcelists_obj(file_name):
    return SOURCE_LISTS_MARKER in file_name.lower() and file_name.lower().endswith(".obj")


def get_row_file(row):
    for value in row[4:]:
        value = value.strip()
        if value.lower().endswith(".obj"):
            return value
    return None


def format_func_line(symbol_name, address):
    line = f"{symbol_name} = {hex(address)}; // type:func"
    if len(symbol_name) > 250:
        line += f" filename:func_{hex(address)[2:]}"
    return line


def format_data_line(symbol_name, address, size):
    return f"{symbol_name} = {hex(address)}; // size:{hex(size)}"


def process_file(map_path, output_path):
    symbols_by_file = {}
    file_order = []

    with open(map_path, newline="") as map_file:
        map_reader = csv.reader(map_file, delimiter=",")
        current_section = None
        current_file = None

        for i, row in enumerate(map_reader):
            if i == 0:
                continue

            if len(row) > 3 and row[3]:
                current_section = row[3].strip()

            row_file = get_row_file(row)
            if row_file:
                current_file = row_file

            if current_section not in TARGET_SECTIONS or not current_file:
                continue

            if not is_sourcelists_obj(current_file):
                continue

            if current_file not in symbols_by_file:
                symbols_by_file[current_file] = []
                file_order.append(current_file)

            if len(row) <= 6 or not row[6]:
                continue

            symbol_name = row[6].strip()
            if not symbol_name:
                continue

            address = int(row[0], 16)
            size = int(row[1], 16) if len(row) > 1 and row[1] else 0
            if size == 0:
                continue

            if current_section in CODE_SECTIONS:
                symbols_by_file[current_file].append(format_func_line(symbol_name, address))
            else:
                symbols_by_file[current_file].append(
                    format_data_line(symbol_name, address, size)
                )

    with open(output_path, "w") as output_file:
        for file_name in file_order:
            file_symbols = symbols_by_file[file_name]
            if not file_symbols:
                continue

            output_file.write("///////////////////////////////////////////////////////////////////\n")
            output_file.write(f"// {file_name}\n")
            output_file.write("///////////////////////////////////////////////////////////////////\n")

            for symbol_line in file_symbols:
                output_file.write(symbol_line + "\n")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Expected usage: {0} <map path> <output path>".format(sys.argv[0]))
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2])
