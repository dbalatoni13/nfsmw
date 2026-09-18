#ifndef FEINTERPOLATORS_H_
#define FEINTERPOLATORS_H_

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEScript.h"

void FEKeyInterp(FEScript *pScript, u8 TrackNum, i32 tTime, FEObject *pOutObj);
void FEKeyInterp(FEKeyTrack *pTrack, i32 tTime, void *pOutData);
void FEKeyInterpFast(FEKeyTrack *pTrack, i32 tTime, void *pOutData);

void FEInterpLinear(FEKeyTrack *pTrack, i32 tTime, void *pOutData);
void FEInterpLinear(FEScript *pScript, u8 TrackNum, i32 tTime, void *pOutData);
void FELerpInteger(i32 n1, i32 n2, float t, i32 *pOffset, i32 *pDest);
void FELerpFloat(float n1, float n2, float t, float *pOffset, float *pDest);
void FELerpVector2(FEVector2 &v1, FEVector2 &v2, float t, FEVector2 *pOffset, FEVector2 *pDest);
void FELerpVector3(FEVector3 &v1, FEVector3 &v2, float t, FEVector3 *pOffset, FEVector3 *pDest);
void FELerpQuaternion(FEQuaternion &q1, FEQuaternion &q2, float t, FEQuaternion *pOffset, FEQuaternion *pDest);
void FELerpColor(FEColor &c1, FEColor &c2, float t, FEColor *pOffset, FEColor *pDest);
bool Close(float x, float y, float epsilon);
bool Close(i32 x, i32 y, i32 epsilon);
void FEInterpLinear(FEKeyTrack *pTrack, i32 tTime, void *pOutDataPtr);

void FEInterpNone(FEKeyTrack *pTrack, i32 tTime, void *pOutDataPtr);
void FEInterpNone(FEScript *pScript, u8 TrackNum, i32 tTime, void *pOutData);

#endif
