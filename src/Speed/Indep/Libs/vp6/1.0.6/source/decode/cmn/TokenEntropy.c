#include "../../../include/vp6_pbdll.h"

extern int VP6_DcNodeEqs[5][3][2];

void VP6_ConfigureContexts(struct PB_INSTANCE *pbi) {
    unsigned int i;
    unsigned int Node;
    unsigned int Plane;
    int Temp;

    for (Plane = 0; Plane < 2; Plane++) {
        for (i = 0; i < 3; i++) {
            for (Node = 0; Node < 5; Node++) {
                Temp = (pbi->DcProbs[Plane * 11 + Node] * VP6_DcNodeEqs[Node][i][0] + 128) >> 8;
                Temp += VP6_DcNodeEqs[Node][i][1];
                if (Temp > 255) {
                    Temp = 255;
                }
                Temp = Temp < 1 ? 1 : Temp;
                pbi->DcNodeContexts[Plane][i][Node] = Temp;
            }
        }
    }
}
