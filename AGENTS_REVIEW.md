# Need for Speed Most Wanted Decompilation

Matching decompilation of Need for Speed Most Wanted 2005 (GameCube) targeting the USA Release build (`GOWE69`).
The goal is to produce C++ source that compiles to byte-identical and dwarf-identical object code against the
original retail binary using the ProDG GC 3.9.3 compiler.

You should heavily rely on your skills. An AI agent recently decompiled the translation unit zPhysics,
but the result is not as clean as human written code. And there was a fatal error, the agent used the PS2
structs, because the GameCube ones were missing.

You should do the following tasks:

1. Make sure the dol builds using `ninja`.
2. Get rid of `using` directives and qualify the full names instead.
3. Apply STYLE_GUIDE.md to zPhysics.
4. Look at all the functions and struct/classes that were created in all the latest Copilot commits and make sure that the dwarf matches the original (dtk dump + lookup skill). Ignore the fact that some inlines are duplicated inside struct bodies in the original dwarf. Keep in mind that the dwarf only ever shows "struct", so don't change classes to structs according to that.
5. Look at all the structs/classes that were created in all the latest Copilot commits and depending on the line info decide whether they are defined in the right place (lookup skill + line_lookup skill). Move them if they aren't.
