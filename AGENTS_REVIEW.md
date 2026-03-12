# Need for Speed Most Wanted Decompilation

Matching decompilation of Need for Speed Most Wanted 2005 (GameCube) targeting the USA Release build (`GOWE69`).
The goal is to produce C++ source that compiles to byte-identical and dwarf-identical object code against the
original retail binary using the ProDG GC 3.9.3 compiler.

You should heavily rely on your skills. An AI agent recently decompiled the translation unit zAnim,
but the result is not as clean as human written code. The dwarf in Symbols/Dwarf unfortunately also didn't
contain the GameCube dwarf, so the agent had to rely on the PS2 ones.

You should do the following tasks:

1. Get rid of `using namespace` and qualify the full names instead.
2. Compare the coding style of the functions in zAnim (AI written) with zAI (human written) and sum up a generic style guide that
   tells later agents how to write code that conforms to the human style. It should be an .MD file in the root of the project.
3. Apply these style guides to zAnim.
4. Look at all the functions and struct/classes that were created in all the latest Copilot commits and make sure that the dwarf matches the original (dtk dump + lookup skill). If it looks like the struct is based on the PS2 version, then completely replace it with that from the dwarf (including offset comments), but keep the bodies of the inlines that you figured out and the virtual functions too.  
5. Look at all the structs/classes that were created in all the latest Copilot commits and depending on the line info decide whether they are defined in the right place (lookup skill + line_lookup skill). Move them if they aren't.
6. Make sure there were no regressions in the matching progress due to the cleanup.
