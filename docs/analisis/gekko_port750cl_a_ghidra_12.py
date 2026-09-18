# Port ONLY the 750CL-guarded hunks from Ghidra 11.4's ppc_common.sinc into
# Ghidra 12.1.2's ppc_common.sinc.  Everything not related to 750CL is left
# alone (12.1.2 has genuine fixes to EVUIMM_*_RAt, CC_TF and dqPlusRaOrZero
# that must NOT be reverted).
import io, sys, os, shutil

P = r"C:\Users\jferr\Desktop\ghidra_12.1.2_PUBLIC_20260605\ghidra_12.1.2_PUBLIC\Ghidra\Processors\PowerPC\data\languages\ppc_common.sinc"
BK = r"C:\Users\jferr\Desktop\nfsdecompiled\scratchpad\ghidra63\gekko\backup\ppc_common.sinc.12.1.2.ORIG"

s = io.open(P, encoding='utf-8', newline='').read()
if '750CL' in s:
    print('ALREADY PATCHED'); sys.exit(0)
if not os.path.exists(BK):
    shutil.copy2(P, BK); print('backed up ->', BK)

n = 0
def sub(old, new, label):
    global s, n
    c = s.count(old)
    if c != 1:
        print('FAIL anchor %-28s count=%d' % (label, c)); sys.exit(2)
    s = s.replace(old, new); n += 1
    print('ok   %s' % label)

# 1. guard the VSX register bank (750CL has no VSX, and ps regs overlay f regs)
sub("define register offset=0x4000 size=16 \n",
    "@ifndef 750CL\ndefine register offset=0x4000 size=16 \n", "guard-vsx-open")
sub("      vs48  vs49  vs50  vs51  vs52  vs53  vs54  vs55  vs56  vs57  vs58  vs59  vs60  vs61  vs62  vs63 \n    ];\n",
    "      vs48  vs49  vs50  vs51  vs52  vs53  vs54  vs55  vs56  vs57  vs58  vs59  vs60  vs61  vs62  vs63 \n    ];\n@endif\n\n",
    "guard-vsx-close")

# 2. the ps<N>_1 register bank (the second slot of each paired single)
anchor2 = "\t_ f16 _ f17 _ f18 _ f19 _ f20 _ f21 _ f22 _ f23 _ f24 _ f25 _ f26 _ f27 _ f28 _ f29 _ f30 _ f31 ];\n@endif\n"
sub(anchor2, anchor2 + """
@ifdef 750CL
define register offset=0x11000 size=8 [
         ps0_1 ps1_1  ps2_1  ps3_1  ps4_1  ps5_1  ps6_1  ps7_1
         ps8_1 ps9_1  ps10_1 ps11_1 ps12_1 ps13_1 ps14_1 ps15_1
        ps16_1 ps17_1 ps18_1 ps19_1 ps20_1 ps21_1 ps22_1 ps23_1
        ps24_1 ps25_1 ps26_1 ps27_1 ps28_1 ps29_1 ps30_1 ps31_1];
@endif
""", "ps1-regbank")

# 3. token fields
sub("\tfD=(21,25)\n",
    "\tfD=(21,25)\n\tps0A=(16,20)\n\tps1A=(16,20)\n\tps0B=(11,15)\n\tps1B=(11,15)\n"
    "\tps0C=(6,10)\n\tps1C=(6,10)\n\tps0D=(21,25)\n\tps1D=(21,25)\n", "tok-psX")
sub("\tfS=(21,25)\n", "\tfS=(21,25)\n\tps0S=(21,25)\n\tps1S=(21,25)\n\tps1T=(21,25)\n", "tok-psS")
sub("\tIMM=(11,15)\n", "\tIMM=(11,15)\n\n\tI=(12,14)\n\tIX=(7,9)\n", "tok-I-IX")
sub("\tSIMM_DS=(2,15) signed\n", "\tSIMM_PS=(0,11) signed\n\tSIMM_DS=(2,15) signed\n", "tok-SIMM_PS")
sub("\tWC=(21,22)\n", "\tW=(15,15)\n\tWX=(10,10)\n\tWC=(21,22)\n", "tok-W-WX")

# 4. guard the VSX 'T' attach
old4 = "attach variables [ T ]\n"
sub(old4, "@ifndef 750CL\n" + old4, "guard-T-open")
sub("      vs16  vs17  vs18  vs19  vs20  vs21  vs22  vs23  vs24  vs25  vs26  vs27  vs28  vs29  vs30  vs31 \n    ];\n",
    "      vs16  vs17  vs18  vs19  vs20  vs21  vs22  vs23  vs24  vs25  vs26  vs27  vs28  vs29  vs30  vs31 \n    ];\n@endif\n",
    "guard-T-close")

# 5. the 750CL attach-variables block, right after the fA/fB/fC/fD/fS/fT attach
anchor5 = ("                   f16 f17 f18 f19 f20 f21 f22 f23\n"
           "                   f24 f25 f26 f27 f28 f29 f30 f31 ];\n")
sub(anchor5, anchor5 + """
@ifdef 750CL

# fX are mirrored to psX, so we simplify and just use the fX registers as the psX_0 registers.
# This is also how the actual hardware behaves.
attach variables [ ps0D ps0B ps0A ps0C ps0S ]
                 [ f0  f1  f2  f3  f4  f5  f6  f7
                   f8  f9  f10 f11 f12 f13 f14 f15
                   f16 f17 f18 f19 f20 f21 f22 f23
                   f24 f25 f26 f27 f28 f29 f30 f31 ];

attach variables [ ps1D ps1B ps1A ps1C ps1S ps1T ]
                 [  ps0_1  ps1_1  ps2_1  ps3_1  ps4_1  ps5_1  ps6_1  ps7_1
                    ps8_1  ps9_1 ps10_1 ps11_1 ps12_1 ps13_1 ps14_1 ps15_1
                   ps16_1 ps17_1 ps18_1 ps19_1 ps20_1 ps21_1 ps22_1 ps23_1
                   ps24_1 ps25_1 ps26_1 ps27_1 ps28_1 ps29_1 ps30_1 ps31_1 ];

# GQR0-GQR7
attach variables [I IX]
\t[spr390 spr391 spr392 spr393 spr394 spr395 spr396 spr397];
@endif
""", "attach-750cl")

# 6. guard the whole Altivec / VSX attach region
sub("attach variables [vrDR vrAR vrBR vrSR vrCR]\n",
    "@ifndef 750CL\nattach variables [vrDR vrAR vrBR vrSR vrCR]\n", "guard-altivec-open")
i = s.find("attach variables vrC_8_15 [")
assert i > 0, "no vrC_8_15 attach"
j = s.find("\n", i)
s = s[:j+1] + "@endif # @ifndef 750CL\n" + s[j+1:]
n += 1; print('ok   guard-altivec-close')

# 7. dcbz_l pcodeop
sub("define pcodeop dataCacheBlockClearToZero;\n",
    "define pcodeop dataCacheBlockClearToZero;\ndefine pcodeop dataCacheBlockSetToZeroLocked;\n",
    "pcodeop-dcbz_l")

# 8. the 12-bit-displacement address subtables used by psq_l / psq_st
sub("dPlusRaAddress: SIMM(A)					is SIMM & A				{tmp:$(REGISTER_SIZE) = A+SIMM; export tmp;  }\n",
    "dPlusRaAddress: SIMM(A)					is SIMM & A				{tmp:$(REGISTER_SIZE) = A+SIMM; export tmp;  }\n"
    "dPlusRaOrZeroAddressPS: SIMM_PS(RA_OR_ZERO) is SIMM_PS & RA_OR_ZERO { tmp:4 = RA_OR_ZERO+SIMM_PS; export tmp; }\n"
    "dPlusRaAddressPS:       SIMM_PS(A)          is SIMM_PS & A          { tmp:4 = A+SIMM_PS; export tmp; }\n",
    "subtable-PS-addr")

io.open(P, 'w', encoding='utf-8', newline='').write(s)
print('WROTE %s  (%d hunks)' % (P, n))
