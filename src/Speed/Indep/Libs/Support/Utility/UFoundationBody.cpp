static int gMasterVideoMode = 0; // .data:0x8041D1DC

// .data:0x8041D1E0, valor 0. Lo asigna Main.cpp (zMisc) y el zMisc extraido lo
// referencia como UND: sin la definicion, zFoundation no enlaza.
void (*UFoundation_AssertMessage)(const char *, ...) = 0;

int GetFoundationVideoMode() {
    return gMasterVideoMode;
}
