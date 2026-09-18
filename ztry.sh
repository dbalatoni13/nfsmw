#!/bin/bash
# Brute-force source variants: ztry.sh <srcfile> <mangled-name> <target-instr-file>
SRC="$1"; FN="$2"; TGT="$3"
cd "C:/Users/jferr/Desktop/nfsdecompiled" || exit 1
cmd //c "set SN_NGC_PATH=C:\Users\jferr\Desktop\nfsdecompiled\build\compilers\ProDG\3.9.3&& build\compilers\ProDG\3.9.3\ngccc.exe -S -O1 -gdwarf+ -Wno-ctor-dtor-privacy -Woverloaded-virtual -Wno-multichar -I src/Speed/Indep/Libs/Support/stlgc -I src/Speed/GameCube/Libs/stl/STLport-4.5/stlport -I src/Speed/GameCube/bWare/GameCube/SN/include -I src/Packages/eathread/1.1.0/include -I src/Speed/GameCube/bWare/GameCube/dolphinsdk/include -I src/Packages -I src/Speed/GameCube/bWare/GameCube/SN -I src -DEA_PLATFORM_GAMECUBE -DEA_REGION_AMERICA -DGEKKO -D_USE_MATH_DEFINES -I build/GOWE69/include -DBUILD_VERSION=0 -DVERSION_GOWE69 -DNDEBUG=1 -G0 -O2 -fno-strength-reduce -fno-strict-aliasing -ffast-math -x c++ -I src/Speed/Indep/Libs/allocator/1.5.0 -I src/Speed/Indep/Libs/csis/dev/include -I src/Speed/Indep/Libs/snd/9/include -I src/Speed/Indep/Libs/spch/dev/include -o tmp_zexp\\try.s $(cygpath -w "$SRC" | sed 's|\\\\|/|g')" 2>&1 | grep -iE "error" && exit 1
awk -v fn="$FN" '$0 ~ "^"fn":" {p=1} p && /blr/ {print; exit} p' tmp_zexp/try.s \
  | grep -oE "[a-z][a-z0-9.]*[ \t]+[-0-9]+[^#]*" \
  | sed -e 's/[ \t][ \t]*/ /g;s/ $//' \
        -e 's/^cmpw 0,\([0-9]*\),\([0-9]*\)/cmpw \1,\2/' \
        -e 's/^cmpwi 0,\([0-9]*\),/cmpwi \1,/' \
        -e 's/^cmplw 0,\([0-9]*\),\([0-9]*\)/cmplw \1,\2/' \
        -e 's/^cmplwi 0,\([0-9]*\),/cmplwi \1,/' \
        -e 's/^rlwinm \([0-9]*\),\([0-9]*\),0,25,31$/clrlwi \1,\2,25/' \
        -e 's/^\(bc\|bge\|bgelr\|blt\|ble\|bgt\|bne\|beq\|b\) .*/B/' \
  > tmp_zexp/got.txt; echo B >> tmp_zexp/got.txt
# branch cond: raw 'bc 4,0' = bge (cr0); map: 0->lt? ProDG numbering: 0=lt,1=gt,2=eq... use objdiff semantics: bge = bc 4,0
if diff -q tmp_zexp/got.txt "$TGT" > /dev/null 2>&1; then echo "MATCH: $SRC"; exit 0; fi
exit 2
