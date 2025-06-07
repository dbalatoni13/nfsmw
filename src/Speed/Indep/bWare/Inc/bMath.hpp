#pragma once

#include "./bVector.hpp"

float bSin(unsigned short angle);
float bCos(unsigned short angle);

inline float bTan(unsigned short angle) {
  return bSin(angle) / bCos(angle);
}

inline float bSqrt(float x) {
  const float bSqrtEPS = 5e-11f;

  float y0;
  float y1;
  float t0;
  float t1;
  float t2;

  float half = 0.5f;
  float one = 1.0f;

  if (!(x <= bSqrtEPS)) {
    asm("frsqrte %0, %1" : "=f"(y0) : "f"(x));
    t0 = y0 * y0; // f0
    t1 = x * t0;
    t2 = t1 - one;
    y1 = y0 * half;
    y0 += (-t2 * y1);

    t0 = y0 * y0;
    t1 = x * t0;
    t2 = t1 - one;
    y1 = y0 * half;
    y0 += -t2 * y1;

    y0 *= x; // f1 = f1 * f12
  } else {
    y0 = 0.0f;
  }

  return y0;
}

inline int bMax(int a, int b) {
  return a > b ? a : b;
}
