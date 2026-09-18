#!/bin/bash
# usage: mkbat.sh <cmdfile> <batname>
python - "$1" "$2" << 'PYEOF'
import sys
line = open(sys.argv[1]).read().splitlines()[-1]
line = line.replace('cmd /c ', '', 1)
setpart, comp = line.split('&&', 1)
open(sys.argv[2], 'w').write('@echo off\n' + setpart.strip() + '\n' + comp.strip() + '\n')
PYEOF
