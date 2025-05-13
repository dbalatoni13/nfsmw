echo off
set asm_file=%1
set ctx=%2
set extra=%3
set extra2=%4
python .\tools\decompctx2.py %ctx% --m2c
python ..\m2c\m2c.py -t ppc-mwcc-c++ --unk-underscore --no-casts --reg-vars r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,f30,f31 --knr %extra% %extra2% --context ctx.cpp %asm_file% > decompiled.cpp
@REM python ..\m2c\m2c.py -t ppc-mwcc-c --unk-underscore --no-casts --knr %extra% %extra2% --context ctx.c %asm_file% > decompiled.c

REM python ..\m2c\m2c.py -t ppc-mwcc-c --unk-underscore --no-casts --knr %extra% %extra2% --context ctx.c %asm_file% > decompiled.c

REM python ..\m2c\m2c.py -t ppc-mwcc-c --reg-vars r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,f30,f31 --knr %extra% %extra2% --context ctx.c %asm_file% > decompiled.c

REM python ..\m2c\m2c.py --visualize=asm -t ppc-mwcc-c %extra% %extra2% --context ctx.c %asm_file% > graph.svg
@REM python .\tools\replace_rodata.py %asm_file% %cd%\decompiled.c
