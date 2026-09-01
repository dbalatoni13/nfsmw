#include "../../../include/vp6_pbdll.h"

extern int VP6_DcNodeEqs[5][3][2];

void VP6_ConfigureContexts(struct PB_INSTANCE *pbi) {
    unsigned int i;
    unsigned int Node;
    unsigned int Plane;
    int Temp;

    for (Node = 0; Node < 2; Node++) {
        for (Plane = 0; Plane < 3; Plane++) {
            for (i = 0; i < 5; i++) {
                Temp = (pbi->DcProbs[Node * 11 + i] * VP6_DcNodeEqs[i][Plane][0] + 128) >> 8;
                Temp += VP6_DcNodeEqs[i][Plane][1];
                if (Temp > 255) {
                    Temp = 255;
                }
                if (Temp < 1) {
                    Temp = 1;
                }
                pbi->DcNodeContexts[Node][Plane][i] = Temp;
            }
        }
    }
}
