#!/bin/bash
# Usage: ./zbuild_libsn.sh <object-path-relative> e.g. build/GOWE69/src/LibSN/vm.o
cd "C:/Users/jferr/Desktop/nfsdecompiled" || exit 1
OBJ="$1"
python -m ninja -t commands "$OBJ" 2>&1 | grep ngccc | head -1 > "$TEMP/cmd.txt"
python - "$OBJ" <<'EOF'
import os, sys
line = open(os.path.join(os.environ['TEMP'], 'cmd.txt')).read().strip().replace('cmd /c ', '', 1)
sp, c = line.split('&&', 1)
open(os.path.join(os.environ['TEMP'], 'b_libsn.bat'), 'w').write('@echo off\n' + sp.strip() + '\n' + c.strip() + '\n')
EOF
cmd //c "%TEMP%\\b_libsn.bat"
