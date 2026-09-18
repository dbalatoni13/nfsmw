import struct, sys
f = open(sys.argv[1],'rb').read()
e_shoff = struct.unpack('>I', f[32:36])[0]
e_shentsize = struct.unpack('>H', f[46:48])[0]
e_shnum = struct.unpack('>H', f[48:50])[0]
e_shstrndx = struct.unpack('>H', f[50:52])[0]
secs = []
for i in range(e_shnum):
    off = e_shoff + i*e_shentsize
    name, typ, flags, addr, offset, size, link, info, align, entsize = struct.unpack('>10I', f[off:off+40])
    secs.append((name,typ,flags,offset,size,link,entsize))
shstr = secs[e_shstrndx]
def sname(n):
    s = f[shstr[3]+n:]
    return s[:s.index(b'\x00')].decode()
for i,s in enumerate(secs):
    if sname(s[0]) in ('.text','.rodata','.data','.bss','.ctors','.sdata','.sdata2','.ctors','.dtors'):
        print(f'sec {sname(s[0]):10s} size={s[4]:#x}')
for i,s in enumerate(secs):
    if sname(s[0]) == '.symtab':
        strtab = secs[s[5]]
        n = s[4]//16
        for j in range(n):
            off = s[3]+j*16
            nm, val, sz, info, other, shndx = struct.unpack('>IIIBBH', f[off:off+16])
            st = f[strtab[3]+nm:]
            name = st[:st.index(b'\x00')].decode(errors='replace')
            if not name or name.startswith('.'): continue
            bind = info>>4; typ = info&0xf
            sn = sname(secs[shndx][0]) if 0 < shndx < len(secs) else str(shndx)
            print(f'{name!r:60s} val={val:#x} sz={sz} bind={bind} sec={sn}')
