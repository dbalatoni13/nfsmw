#pragma once

float bSin(unsigned short angle);
float bCos(unsigned short angle);

inline float bTan(unsigned short angle) { return bSin(angle) / bCos(angle); }
