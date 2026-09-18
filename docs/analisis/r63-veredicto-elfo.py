import struct, sys

def u32(b,o): return struct.unpack_from('>I',b,o)[0]
def u16(b,o): return struct.unpack_from('>H',b,o)[0]

class Obj:
    def __init__(self, path):
        b = open(path,'rb').read(); self.b=b
        shoff=u32(b,0x20); shentsize=u16(b,0x2e); shnum=u16(b,0x30); shstrndx=u16(b,0x32)
        secs=[]
        for i in range(shnum):
            o=shoff+i*shentsize
            name,typ,flags,addr,off,size,link,info,align,entsz = struct.unpack_from('>10I',b,o)
            secs.append(dict(name=name,typ=typ,flags=flags,addr=addr,off=off,size=size,link=link,info=info,entsz=entsz,idx=i))
        strtaboff=secs[shstrndx]['off']
        def nm(x):
            e=b.index(b'\0',strtaboff+x); return b[strtaboff+x:e].decode('utf8','replace')
        for s in secs: s['sname']=nm(s['name'])
        self.secs=secs
        # symtab
        self.syms=[]
        for s in secs:
            if s['typ']==2:  # SYMTAB
                stro=secs[s['link']]['off']
                n=s['size']//16
                for i in range(n):
                    o=s['off']+i*16
                    nameoff,value,size,info,other,shndx = struct.unpack_from('>IIIBBH',b,o)
                    e=b.index(b'\0',stro+nameoff)
                    name=b[stro+nameoff:stro+nameoff+ (e-(stro+nameoff))].decode('utf8','replace')
                    self.syms.append(dict(name=name,value=value,size=size,info=info,shndx=shndx,idx=i,
                                          typ=info&0xf, bind=info>>4))
        self.symtab=[s for s in secs if s['typ']==2]
    def sec(self,name):
        for s in self.secs:
            if s['sname']==name: return s
        return None
    def funcs(self, secname='.text'):
        s=self.sec(secname)
        if s is None: return []
        out=[x for x in self.syms if x['shndx']==s['idx'] and x['typ']==2]
        out.sort(key=lambda x:(x['value'],x['name']))
        return out
    def relocs(self, secname='.text'):
        s=self.sec(secname)
        out=[]
        for r in self.secs:
            if r['typ']==4 and r['info']==s['idx']:  # RELA
                n=r['size']//12
                for i in range(n):
                    off,info,add=struct.unpack_from('>IIi',self.b,r['off']+i*12)
                    out.append((off, info>>8, info&0xff, add))
            if r['typ']==9 and r['info']==s['idx']:  # REL
                n=r['size']//8
                for i in range(n):
                    off,info=struct.unpack_from('>II',self.b,r['off']+i*8)
                    out.append((off, info>>8, info&0xff, 0))
        return out
