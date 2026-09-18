# Second pass: wrap the additions that port750cl.py left UNGUARDED inside
# @ifdef 750CL, so the other 18 PowerPC languages compile byte-for-byte
# identically to stock Ghidra 12.1.2.
import io, sys

P = sys.argv[1] if len(sys.argv) > 1 else \
    r"C:\Users\jferr\Desktop\ghidra_12.1.2_PUBLIC_20260605\ghidra_12.1.2_PUBLIC\Ghidra\Processors\PowerPC\data\languages\ppc_common.sinc"

s = io.open(P, encoding='utf-8', newline='').read()
if '@ifdef 750CL\n\tps0A=(16,20)' in s:
    print('ALREADY GUARDED'); sys.exit(0)

n = 0
def sub(old, new, label):
    global s, n
    c = s.count(old)
    if c != 1:
        print('FAIL %-20s count=%d' % (label, c)); sys.exit(2)
    s = s.replace(old, new); n += 1
    print('ok   %s' % label)

def guard(block, label):
    sub(block, "@ifdef 750CL\n" + block + "@endif\n", label)

guard("\tps0A=(16,20)\n\tps1A=(16,20)\n\tps0B=(11,15)\n\tps1B=(11,15)\n"
      "\tps0C=(6,10)\n\tps1C=(6,10)\n\tps0D=(21,25)\n\tps1D=(21,25)\n", "tok-psX")
guard("\tps0S=(21,25)\n\tps1S=(21,25)\n\tps1T=(21,25)\n", "tok-psS")
guard("\n\tI=(12,14)\n\tIX=(7,9)\n", "tok-I-IX")
guard("\tSIMM_PS=(0,11) signed\n", "tok-SIMM_PS")
guard("\tW=(15,15)\n\tWX=(10,10)\n", "tok-W-WX")
guard("\tXOP_1_6=(1,6)\n", "tok-XOP_1_6")
guard("define pcodeop dataCacheBlockSetToZeroLocked;\n", "pcodeop-dcbz_l")
guard("dPlusRaOrZeroAddressPS: SIMM_PS(RA_OR_ZERO) is SIMM_PS & RA_OR_ZERO { tmp:4 = RA_OR_ZERO+SIMM_PS; export tmp; }\n"
      "dPlusRaAddressPS:       SIMM_PS(A)          is SIMM_PS & A          { tmp:4 = A+SIMM_PS; export tmp; }\n",
      "subtable-PS-addr")

io.open(P, 'w', encoding='utf-8', newline='').write(s)
print('WROTE %s  (%d guards)' % (P, n))
