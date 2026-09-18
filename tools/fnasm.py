#!/usr/bin/env python3
"""Dump original (left) disassembly for named functions of a unit."""
import subprocess
import json
import sys
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def fetch(unit):
    out = subprocess.run(
        [os.path.join(ROOT, 'build/tools/objdiff-cli'), 'diff',
         '-c', 'functionRelocDiffs=none',
         '-c', 'ppc.calculatePoolRelocations=false',
         '-u', unit, '-o', '-', '--format', 'json'],
        capture_output=True, text=True, cwd=ROOT)
    return json.loads(out.stdout)


def main():
    unit = sys.argv[1]
    pat = sys.argv[2]
    sides = ('left', 'right')
    j = fetch(unit)
    fns = {}
    for side in sides:
        for f in j[side]['symbols']:
            name = f.get('demangled_name') or f['name']
            if pat.lower() in name.lower():
                fns.setdefault(name, {})[side] = f
    for name, d in fns.items():
        for side in sides:
            f = d.get(side)
            if not f:
                continue
            print('=== %s %s addr=%s size=%s' % (side, name, f['address'], f['size']))
            for i in f.get('instructions', []):
                if 'instruction' not in i:
                    continue
                ins = i['instruction']
                print('%6x  %s' % (int(ins['address']), ins['formatted']))
            print()


if __name__ == '__main__':
    main()
