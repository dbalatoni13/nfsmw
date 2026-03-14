#!/bin/sh
if [ "$#" -eq 0 ]; then
  echo "Usage: .comms/reply_now/zFe2.sh \"your real reply here\""
  exit 1
fi
cd /Users/johannberger/nfsmw-zFE2 || exit 1
/Users/johannberger/.pyenv/versions/3.12.1/bin/python tools/comms.py reply zFe2 --to commsManager --thread live-checkin "$@"
