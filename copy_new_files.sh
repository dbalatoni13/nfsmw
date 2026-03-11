#!/bin/bash

# Try to make files writable
echo "Attempting to change permissions..."
chmod u+w /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp 2>&1

if [ $? -ne 0 ]; then
    echo "chmod failed, removing files first..."
    rm -f /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp
fi

echo "Copying .new files..."
cp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp

cp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp

cp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp

echo "=== LINE COUNTS ==="
wc -l /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp

echo "=== VERIFICATION ==="
diff /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.cpp && echo "File 1: MATCH" || echo "File 1: MISMATCH"

diff /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp && echo "File 2: MATCH" || echo "File 2: MISMATCH"

diff /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp.new /Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp && echo "File 3: MATCH" || echo "File 3: MISMATCH"
