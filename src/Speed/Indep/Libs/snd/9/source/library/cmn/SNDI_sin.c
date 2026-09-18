#include "./sndcmn.h"

// Serie de Taylor hasta x^13; nombres de parametro y locales del DWARF.
float SNDI_sin(float input) {
    while (input > 6.2831855f) {
        input -= 6.2831855f;
    }

    float x2 = input * input;
    float x3 = x2 * input;
    float x5 = x2 * x3;
    float x7 = x2 * x5;
    float x9 = x2 * x7;
    float x11 = x2 * x9;
    float x13 = x2 * x11;

    return input - x3 * 0.16666667f + x5 * 0.00833333f - x7 * 0.0001984127f +
           x9 * 2.7557319e-06f - x11 * 2.5052108e-08f +
           x13 * 1.6059e-10f;
}
