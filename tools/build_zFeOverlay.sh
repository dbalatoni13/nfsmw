#!/bin/bash
set -e
cd "$(dirname "$0")/.."
ninja build/GOWE69/src/Speed/Indep/SourceLists/zFeOverlay.o
python3 tools/rename_section.py build/GOWE69/src/Speed/Indep/SourceLists/zFeOverlay.o .text .over
