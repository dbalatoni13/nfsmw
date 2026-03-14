#!/bin/sh
if [ "$#" -eq 0 ]; then
  echo "Usage: .comms/reply_now/testComms.sh \"your real reply here\""
  exit 1
fi
cd /Users/johannberger/nfsmw-zFE2 || exit 1
/Users/johannberger/.pyenv/versions/3.12.1/bin/python tools/comms.py reply testComms --to commsManager "$@"
