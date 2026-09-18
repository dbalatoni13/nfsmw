# Pruebas de reparto de `sndmix`

Comprueban con `static_assert` que `smixport.h` --la definicion unica que usa el
port de 64 bits-- sigue siendo fiel al original y sigue coincidiendo con la de
`smixer.c`. No hacen falta ni aparato ni Gradle: son compilaciones de sintaxis.

    NDK=$LOCALAPPDATA/Android/Sdk/ndk/27.0.12077973/toolchains/llvm/prebuilt/windows-x86_64/bin
    I="-Isrc/Speed/Indep/Libs/snd/9/include -Isrc/Speed/Indep/Libs/snd/9/source/library/mix"

    # en arm64 tiene que repartir igual que smixer.c
    $NDK/clang++ --target=aarch64-linux-android24 -fsyntax-only -std=c++17 $I \
        port/android/tests/smixport_arm64.cpp

    # en 32 bits tiene que dar los offsets medidos contra el asm de GameCube
    $NDK/clang++ --target=armv7a-linux-androideabi24 -fsyntax-only -std=c++17 $I \
        port/android/tests/smixport_gc32.cpp

Si alguna falla, el mensaje dice **que campo** se ha movido. La referencia buena
es siempre `smixer.c`, que es la que salio del asm.
