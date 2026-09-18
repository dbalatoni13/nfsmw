import re
with open('symbols/debug_lines.txt','r') as f:
    lines = f.readlines()
mapping={}
pat=re.compile(r'^0x([0-9A-Fa-f]+): d:/mw/speed/indep/src/Camera/([^ ]+) \(line (\d+)\)')
for L in lines:
    L=L.strip()
    m=pat.match(L)
    if not m: continue
    addr=int(m.group(1),16)
    if addr < 0x80064DB4 or addr > 0x80083604: continue
    fname = m.group(2)
    if '../' in fname: continue
    # only consider .cpp/.hpp source parts:
    if fname not in mapping:
        mapping[fname]=addr
for k,v in sorted(mapping.items(), key=lambda kv: kv[1]):
    print(hex(v), k)
