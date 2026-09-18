#include "FnEventBlender.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

extern float qt0[7];

namespace EAGL4Anim {

void FnEventBlender::Eval(float previousTime, float currentTime, float *data) {
    if (currentTime <= mStartTransTime) {

        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);

    } else if (currentTime >= mEndTransTime) {

        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);

    } else {

        switch (mTriggerType) {

        case FIRST_ONLY:

            mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
            break;

        case SECOND_ONLY:

            mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
            break;

        default:

            mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);

            mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
            break;
        }
    }
}

inline void EulF3(float *&data, float *output) {
    float eul[3];

    eul[0] = DegToRad(*data++);
    eul[1] = DegToRad(*data++);
    eul[2] = DegToRad(*data++);

    EulToQuat(eul, output);
}

// TODO inline and move
inline void QuatF4(float *&data, float *output) {
    output[0] = *data++;
    output[1] = *data++;
    output[2] = *data++;
    output[3] = *data++;
}

// TODO inline and move
inline void TranF3(float *&data, float *output) {
    output[4] = *data++;
    output[5] = *data++;
    output[6] = *data++;
}

// TODO inline and move
// r62 (eagl) -- EL `inline` DE ESTOS TRES VALE 7.917 B, y es la palanca mas
// barata que le queda a la unidad.  Sin el, GCC 2.9 los emite en el punto de
// parseo (posicion 130 de 318) y el objetivo los tiene en la COLA de inlines
// diferidas (posicion 364), pegados a EulF3/QuatF4/TranF3 -- que ya eran
// inline.  Se les toma la direccion en RawPoseChannel.cpp:70-76, asi que GCC
// los emite igual, pero desde finish_file: con `inline` caen en su sitio.
// Medido (bytes distintos en los rangos de splits.txt, contra el enlace base):
//   sin inline .. 93.279 B   textorder: 273 descolocadas / 75 saltos
//   con inline .. 85.362 B   textorder: 251 descolocadas / 73 saltos
// Es la receta de memory/nfsmw-decl-comdat-inline aplicada al reves: aqui no
// hay que retrasar una inline, hay que CONVERTIR en inline una que no lo era.
inline void EulF3Interp(float w, float *&data0, float *&data1, float *output) {
    EulF3(data0, qt0);
    EulF3(data1, output);
    FastQuatBlendF4(w, qt0, output, output);
}

// TODO inline and move
inline void QuatF4Interp(float w, float *&data0, float *&data1, float *output) {
    QuatF4(data0, qt0);
    QuatF4(data1, output);
    FastQuatBlendF4(w, qt0, output, output);
}

// TODO inline and move
inline void TranF3Interp(float w, float *&data0, float *&data1, float *output) {
    TranF3(data0, qt0);
    TranF3(data1, output);
    LinearBlendF3(w, &qt0[4], &output[4], &output[4]);
}

}; // namespace EAGL4Anim
