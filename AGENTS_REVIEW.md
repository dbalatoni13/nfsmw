# Need for Speed Most Wanted Decompilation

Matching decompilation of Need for Speed Most Wanted 2005 (GameCube) targeting the USA Release build (`GOWE69`).
The goal is to produce C++ source that compiles to byte-identical and dwarf-identical object code against the
original retail binary using the ProDG GC 3.9.3 compiler.

You should heavily rely on your skills. An AI agent recently decompiled the translation unit zPhysics,
but the result is not as clean as human written code.

You should do the following tasks:

1. Make sure the dol builds using `ninja`.
2. Get rid of `using namespace` and qualify the full names instead.
3. Compare the coding style of zPhysics (AI written) with zAI (human written) and sum up a generic style guide that
   tells later agents how to write code that conforms to the human style. It should be an .MD file in the root of the project.
4. Apply these style guides to zPhysics.
5. Look at all the functions and struct/classes that were created in all the latest Copilot commits and make sure that the dwarf matches the original (dtk dump + lookup skill).
6. Look at all the structs/classes that were created in all the latest Copilot commits and depending on the line info decide whether they are defined in the right place (lookup skill + line_lookup skill). Move them if they aren't.
