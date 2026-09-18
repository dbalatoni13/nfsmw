#include "./sndcmn.h"

// sqrt(1 + x) por su serie de Taylor: cada termino es el anterior por x y por
// el cociente de coeficientes. Nombres de parametro y locales del DWARF.
float SNDI_rootof1plusx(float input) {
    float p1 = input * 0.5f;
    float p2 = p1 * (input * -0.25f);
    float p3 = p2 * (input * -0.5f);
    float p4 = p3 * (input * -0.625f);
    float p5 = p4 * (input * -0.7f);
    float p6 = p5 * (input * -0.75f);
    float p7 = p6 * (input * -0.7857f);

    return 1.0f + p1 + p2 + p3 + p4 + p5 + p6 + p7;
}
