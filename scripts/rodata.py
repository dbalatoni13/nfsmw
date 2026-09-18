import struct,sys,re
def load(path):
    d=open(path,'rb').read()
    assert d[:4]==b'\x7fELF'
    be = d[5]==2
    E='>' if be else '<'
    shoff,=struct.unpack(E+'I',d[0x20:0x24])
    shentsize,shnum,shstrndx=struct.unpack(E+'HHH',d[0x2e:0x34])
    secs=[]
    for i in range(shnum):
        o=shoff+i*shentsize
        name,typ,flags,addr,off,size,link,info,align,entsize=struct.unpack(E+'10I',d[o:o+40])
        secs.append(dict(name=name,typ=typ,off=off,size=size,link=link,info=info,entsize=entsize,addr=addr))
    shstr=secs[shstrndx]
    def nm(tbl,off):
        e=d.index(b'\0',tbl['off']+off); return d[tbl['off']+off:e].decode()
    for s in secs: s['sname']=nm(shstr,s['name'])
    return d,E,secs
def syms(path):
    d,E,secs=load(path)
    out=[]
    for s in secs:
        if s['typ']!=2: continue
        strt=secs[s['link']]
        n=s['size']//16
        for i in range(n):
            o=s['off']+i*16
            nameoff,value,size,info,other,shndx=struct.unpack(E+'IIIBBH',d[o:o+16])
            e=d.index(b'\0',strt['off']+nameoff)
            name=d[strt['off']+nameoff:e].decode()
            out.append((name,value,size,shndx))
    return d,E,secs,out
if __name__=='__main__':
    path=sys.argv[1]; pat=sys.argv[2]
    d,E,secs,ss=syms(path)
    rx=re.compile(pat)
    for name,value,size,shndx in ss:
        if rx.search(name) and shndx<len(secs):
            sec=secs[shndx]
            b=d[sec['off']+value:sec['off']+value+4]
            if len(b)==4:
                f=struct.unpack(E+'f',b)[0]; u=struct.unpack(E+'I',b)[0]
                print('%-24s %-16s +0x%-6x 0x%08X  %g'%(name,sec['sname'],value,u,f))
