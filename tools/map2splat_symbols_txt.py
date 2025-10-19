import sys
import csv

# (Please don't ever look at this anyone)


def process_file(map_path, output_path):
    with open(map_path) as map_file:
        with open(output_path, "w") as output_file:
            map_reader = csv.reader(map_file, delimiter=",")
            current_section = None
            i = 0
            for row in map_reader:
                if i == 0:
                    # jump over header
                    i += 1
                    continue
                if len(row) > 3 and row[3]:
                    current_section = row[3]
                if current_section == ".text":
                    if len(row) > 5 and row[5] and "sourcelists" in row[5]:
                        file_name = row[5]
                        output_file.write(
                            "///////////////////////////////////////////////////////////////////\n"
                        )
                        output_file.write(f"// {file_name}\n")
                        output_file.write(
                            "///////////////////////////////////////////////////////////////////\n"
                        )
                    if len(row) > 6 and row[6]:
                        address = int(row[0], 16)
                        symbol_name = row[6]
                        str_to_write = f"{symbol_name} = {hex(address)}; // type:func"
                        if len(symbol_name) > 250:
                            str_to_write += f" filename:func_{hex(address)[2:]}"
                        output_file.write(str_to_write + "\n")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Expected usage: {0} <map path> <output path>".format(sys.argv[0]))
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2])
