extern unsigned int vblticks;

typedef void (*VBLTIMERFUNC)();
extern VBLTIMERFUNC vbltmrsub[8];

void ttDoVTimerMsg() {
    vblticks++;

    for (int i = 0; i < 8; i++) {
        if (vbltmrsub[i] != 0) {
            vbltmrsub[i]();
        }
    }
}
