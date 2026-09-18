#!/bin/bash
# usage: tmpb/build.sh <source-list-unit> <batname>
set -e
ROOT="C:/Users/jferr/Desktop/nfsdecompiled"
cd "$ROOT"
python -m ninja -t commands "build/GOWE69/src/$1.o" > tmpb/cmd.txt 2>/dev/null
line=$(grep ngccc tmpb/cmd.txt | tail -1)
python - "$line" "tmpb/$2.bat" << 'PYEOF'
import sys
line = sys.argv[1].strip()
line = line.replace('cmd /c ', '', 1)
setpart, comp = line.split('&&', 1)
open(sys.argv[2], 'w').write('@echo off\n' + setpart.strip() + '\n' + comp.strip() + '\n')
PYEOF
cmd //c 'C:\Users\jferr\Desktop\nfsdecompiled\tmpb\'"$2"'.bat' > tmpb/build.log 2>&1 || { tail -30 tmpb/build.log; exit 1; }
echo BUILD_OK
