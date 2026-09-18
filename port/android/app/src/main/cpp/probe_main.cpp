#include <cstdio>
#include "UMath.h"
int main() {
    printf("paso0: inicio\n"); fflush(stdout);
    UMath::Vector3 a; a.x = 3.0f; a.y = 4.0f; a.z = 0.0f;
    printf("paso1: vector creado\n"); fflush(stdout);
    UMath::Vector3 b; b.x = 0.0f; b.y = 0.0f; b.z = 1.0f;
    UMath::Vector3 c;
    UMath::Cross(a, b, c);
    printf("paso2: cross=(%.4f,%.4f,%.4f)\n", c.x, c.y, c.z); fflush(stdout);
    float len = UMath::Length(a);
    printf("paso3: len=%.6f\n", len); fflush(stdout);
    printf("RESULTADO: %s\n", (len > 4.999f && len < 5.001f) ? "OK" : "FALLO");
    return (len > 4.999f && len < 5.001f) ? 0 : 1;
}
