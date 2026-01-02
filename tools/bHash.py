from ctypes import c_uint32
import sys


def bHash(string: str):
    if not string:
        return 0

    result = c_uint32(0xFFFFFFFF)

    for c in string:
        result = c_uint32(result.value * 33 + ord(c))

    return result.value


if __name__ == "__main__":
    if len(sys.argv) == 2:
        filename = sys.argv[1]
        with open(filename, "r") as file:
            lines = file.readlines()
            for line in lines:
                string = line.strip()
                chars = ", ".join([f"""'{c}'""" for c in string])
                templated = f"BinHash<{chars}>::value"
                print(f"{templated} = {hex(bHash(string))}")
    else:
        while True:
            string = input("Input string: ")
            print(hex(bHash(string)))
