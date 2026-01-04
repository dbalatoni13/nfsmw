#!/usr/bin/env python3

import argparse
from platform import uname


def main() -> None:
    parser = argparse.ArgumentParser(description="""Creates an ok file""")
    parser.add_argument(
        "ok_file",
        help="""Dependency file in""",
    )
    args = parser.parse_args()

    with open(args.ok_file, "w", encoding="UTF-8") as f:
        f.write("")


if __name__ == "__main__":
    main()
