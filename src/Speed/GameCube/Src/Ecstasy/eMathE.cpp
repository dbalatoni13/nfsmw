#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/mtx.h"

#include <math.h>

bMatrix4 eMathIdentityMatrix;
bMatrix4 eMathZeroMatrix;

void eMathInit(void) {
    bMatrix4 *identity = eGetIdentityMatrix();
    bMatrix4 *zero = eGetZeroMatrix();
    MTX44Identity(*reinterpret_cast<Mtx44 *>(identity));
    bMemSet(zero, 0, sizeof(*zero));
}

inline float eRecip(float x) {
    float val = x;
    float recip;

    asm("fres %0, %1" : "=f"(recip) : "f"(val));
    return recip;
}

void eCopyMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bCopy(dest, src);
}

void eMulMatrix(bMatrix4 *ab, bMatrix4 *a, bMatrix4 *b) {
    MTX44Concat(*reinterpret_cast<Mtx44 *>(a), *reinterpret_cast<Mtx44 *>(b), *reinterpret_cast<Mtx44 *>(ab));
}

// TODO the variables might be unused in these two
void eMulVector(bVector4 *vm, const bMatrix4 *m, const bVector4 *v) {
    {
        double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm volatile("psq_l %0, 0(%6), 0, 0\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x10\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds1 %2, %2, %0, %4\n"
                     "addi %6, %6, 8\n"
                     "psq_l %1, 0(%6), 0, 0\n"
                     "addi 9, %5, 0x20\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds0 %3, %3, %1, %2\n"
                     "addi 9, %5, 0x30\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %3\n"
                     "psq_st %4, 0(%7), 0, 0\n"
                     "addi 9, %5, 8\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x18\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds1 %3, %3, %0, %4\n"
                     "addi 9, %5, 0x28\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds0 %2, %2, %1, %3\n"
                     "addi %5, %5, 0x38\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %2\n"
                     "psq_st %4, 8(%7), 0, 0"
                     : "=&f"(FP0), "=&f"(FP1), "=&f"(FP2), "=&f"(FP3), "=&f"(FP4)
                     : "b"(m), "b"(v), "b"(vm)
                     : "r9", "memory");
    }
}

void eMulVector(bVector3 *vm, const bMatrix4 *m, const bVector3 *v) {
    {
        double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm volatile("psq_l %0, 0(%6), 0, 0\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x10\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds1 %2, %2, %0, %4\n"
                     "addi 5, 5, 8\n"
                     "psq_l %1, 0(5), 1, 0\n"
                     "addi 9, %5, 0x20\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds0 %3, %3, %1, %2\n"
                     "addi 9, %5, 0x30\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %3\n"
                     "psq_st %4, 0(%7), 0, 0\n"
                     "addi 9, %5, 8\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x18\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds1 %3, %3, %0, %4\n"
                     "addi 9, %5, 0x28\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds0 %2, %2, %1, %3\n"
                     "addi 4, 4, 0x38\n"
                     "psq_l %4, 0(4), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %2\n"
                     "psq_st %4, 8(%7), 1, 0"
                     : "=&f"(FP0), "=&f"(FP1), "=&f"(FP2), "=&f"(FP3), "=&f"(FP4)
                     : "r"(m), "r"(v), "r"(vm)
                     : "r9", "memory");
    }
}

// NON_MATCHING: 85,82 % (r24). El 88,85 % anterior era MENOS fiel: el DWARF del
// original no da registro a clipY (combine funde el neg en el fmadds y mata el
// pseudo de la declaracion) y declara oneOverW ANTES que clipX, con halfVP2/3
// detras de clipZ. Las dos cosas estan aqui; el porcentaje baja 3,03 pp y cero
// bytes. A partir de aqui la linea del original = la nuestra + 87 (clipX 157
// -> 244, halfVP2 164 -> 251, *sx 167 -> 254). Lo unico que falta: halfVP2
// tiene que asignarse ANTES que clipX (objetivo f0/f13, nosotros f13/f0):
// prioridad 3/len(halfVP2) > 2/len(clipX) en local-alloc; hoy 5000 vs 6666.
// r30: ESE EMPATE SE ROMPE, y baja de 25 diffs a 15. La palanca es la del
// brief 1 aplicada a LOCAL-alloc (primera vez en el proyecto): un asm no
// volatil cuenta como REFERENCIA sin emitir un byte. Datos del .lreg:
//   Register 137  3 refs / 6 insns  -> halfVP2   pri 5000
//   Register 107  2 refs / 3 insns  -> clipX     pri 6666
//   Register 143  3 refs / 5 insns  -> halfVP3   pri 6000
// Con UNA `__asm__("" : "+f"(halfVP2));` detras de la declaracion de halfVP3:
// halfVP2 pasa a 4/7 = 5714 y clipX a 2/4 = 5000, y halfVP2 GANA ->
// 93,67164 % / 16 diffs; con `*sy = vp[1] + halfVP3 + -clipY * halfVP3 *
// oneOverW;` encima, 93,82089 % / 15. REVERTIDO: es un asm y el DWARF de
// eProject no lo tiene.
// Lo que queda es el MISMO empate un piso mas abajo: halfVP3 (3/5 = 6000)
// contra el temporal de clipY (objetivo halfVP3 en f13 y clipY en f12,
// nosotros al reves). Y ahi el asm NO ENTRA: seis formas y sitios sobre
// halfVP3 dan objeto IDENTICO, porque su definicion es la sentencia
// inmediatamente anterior y GCC absorbe el asm en la copia que ya habia
// (regla nueva: el asm solo anade una insn sobre una variable de vida larga).
// Vedas: orden de clipX/clipY/clipZ (5 formas) 16-32 diffs; halfVP3 declarada
// detras del *sx 22; asm sobre clipX, clipY u oneOverW 24-33.
// Detalle y las 31 formas medidas en docs/analisis/r30-bw.md.
// r36d: APLICADO. La veda de la r30 ("es un asm y el DWARF de eProject no lo
// tiene") CADUCA: la barrera selectiva es una de las cinco palancas legitimas
// desde el brief de la r36, y no emite un byte (268/268 antes y despues). Y no
// va sola: con la barrera de la r30 MAS una reasociacion de *sx la funcion pasa
// de 85,82089 % / 25 diffs a 93,970146 % / 14 diffs, y sube tambien el recuento
// exacto (46 -> 54 instrucciones). Las tres piezas son inseparables:
//     barrera sola                      93,82089 % / 15
//     *sx reasociado sin barrera        86,1194 %  / 23
//     las dos                           93,970146 %/ 14   <- lo que hay aqui
// Lo que queda son DOS ciclos de dos, y estan medidos contra las cinco palancas:
//   (a) halfVP2 <-> clipX en *sx: el objetivo f0/f13, nosotros f13/f0.
//   (b) halfVP3 <-> el temporal de clipY en *sy: objetivo f13/f12, al reves.
// `register float halfVP3 asm("fr13")` CIERRA (b) y deja 12 diffs con eq=57,
// pero baja el fuzzy a 92,955 % porque rompe el registro del resultado de *sy
// (el pin reserva f13 toda la vida de halfVP3 y el objetivo REUTILIZA f13 para
// el producto). No aplicado: pctsnap solo ve el fuzzy. Anotado por si alguien
// mide por instrucciones exactas -- ahi es +3.
// Vedas nuevas de la r36d, todas sobre la base de 14 diffs:
//   40 pines (halfVP2/halfVP3/clipX/clipY/clipZ x fr0..fr13): mejor 13 (h3 fr13)
//   72 destinatarios de barrera (6 variables x N=1..3 x 2 sitios x 2 bases): 13
//   48 formas de *sx y *sy (6 x 4 reasociaciones x 2 de clipX): 12 el mejor
//   36 combinaciones pin x forma x barrera y 26 de dos destinatarios a la vez
// El unico eje que aun no toca nadie es por que local_alloc reparte f0/f13 al
// reves en (a): las dos tienen ya el mismo n_refs y el ciclo no se rompe.
// r36f: 33 medidas mas, con la receta que cerro EmitterSystem::Render (arreglar
// PRIMERO la diferencia que no es de registro). Aqui esa diferencia esta en las
// filas 33-38: el objetivo hace `lfs vp[0]` + `fadds` (el `vp[0] + halfVP2` de
// *sx) ANTES del `fmadds` de clipY y nosotros al reves. Nada la mueve:
//   - segundo operando en el `asm` QUE YA ESTA (la forma ganadora en Render):
//     `"+f"(halfVP2), "+f"(V)` y `"+f"(halfVP2) : "f"(V)` sobre halfVP3, clipX,
//     clipY, clipZ y oneOverW, x1 y x2 -- 16 medidas, de 20 a 44 filas, y cuatro
//     ademas engordan a 272 B. La mejor (entrada oneOverW) da 20.
//   - BARRERA DE RANURA (`"+m"`, la cuarta familia, que la r36d no probo aqui) y
//     barreras sueltas ENTRE `*sx` y `*sy` y DELANTE de `*sx` -- 14 medidas:
//     `"+m"(*sx)` 18/42, `"+m"(*sy)` 24/47, `"+f"(oneOverW)` 26, `"+f"(clipY)`
//     24 (272 B); `"+f"(halfVP3)`, `"+f"(clipZ)` y el clobber de r0 salen
//     IDENTICOS en los dos sitios (absorbidos).
//   - local propia para vp[0]/vp[1] con barrera: 19 y IDENTICO.
// r48: los NUMEROS de local_alloc, que hasta ahora se daban por supuestos.
// QTY_CMP_PRI (local-alloc.c:1568) = floor_log2(n_refs)*n_refs*size /
// (death-birth) * 10000, y el .lreg de eMathE.cpp compilado SUELTO (1,4 s) da:
//     107 clipX    2 refs /  3 insns  ->  6666   -> f0   (objetivo f13)
//     137 halfVP2  5 refs /  8 insns  -> 12500   -> f13  (objetivo f0)
//     143 halfVP3  3 refs /  5 insns  ->  6000   -> f12  (objetivo f13)
// O sea que la prioridad NO explica el ciclo (a): halfVP2 ya la tiene mas alta
// y aun asi se lleva f13. Lo que SI lo mueve es el numero de barreras:
//     sin asm ...... 17 filas   137 (3/6)   p5000    107=f0  137=f13
//     asm x1 (hoy) . 13 filas   137 (5/8)   p12500   107=f0  137=f13
//     asm x2 ....... 25 filas   137 (7/11)  p12727   107=f13 137=f0   <- (a) CERRADO
//     asm x3 ....... 31 filas ; asm x5 ..... 20 filas
// Con dos barreras el ciclo (a) casa EXACTAMENTE (107=f13, 137=f0) pero el
// resto del reparto se desmonta (25 filas). Quien retome esto tiene ahi el
// unico eje vivo: cerrar (a) con x2 y arreglar lo que rompe.
// Negativo nuevo: la forma `__asm__("" : : "f"(x))` de SOLO ENTRADA del brief
// de la r47 AQUI SI EMITE (67 -> 68 insns) porque un asm sin salidas es
// volatil, y las cinco colocaciones dan 47-51 filas.
// r49: `lmap.py` SOBRE EL ORIGINAL DEMUESTRA QUE EL `asm` DE ABAJO NO ESTA EN
// LA FUENTE ORIGINAL, y de paso da el mapa de sentencias entero. Lineas del
// original -> lineas nuestras: 225 = la cabecera, 231/232/233 = local.x/y/z,
// 234 = MTXMultVec, 236 = el `if`, 238 = los dos `*sy=0;*sx=0`, 239 = `*sz`,
// 240 = el `return`, 244/245/246 = clipX/clipY/clipZ, 248 = oneOverW,
// 251/252 = halfVP2/halfVP3, 254/255/256 = *sx/*sy/*sz, 257 = la llave.
// El desplazamiento vale +31 desde la 207 hasta la 221 y pasa a +30 en la 224:
// o sea que entre `halfVP3` y `*sx` el original tiene UNA linea (la 253, sin
// codigo) y nosotros DOS (el `__asm__` y el blanco). No hay hueco para ninguna
// sentencia ahi: el asm es NUESTRO. Cuesta 4 filas quitarlo (13 -> 17), asi que
// no lo he retirado, pero quien vuelva a esta funcion debe partir de que la
// forma correcta no lo lleva.
// r49: y el mismo mapa nombra las 13 filas. No son "dos ciclos de dos": son UNA
// transposicion de dos cadenas independientes mas su domino de registros.
// El objetivo emite, por lineas: [254 lfs vp[0]] [251 fmuls halfVP2]
// [244 lfs eye.x] [254 fadds] [246 lfs pm[6]] [244 fmadds clipX]; nosotros
// adelantamos la cadena de 244 y atrasamos la de 254. Detras van f0<->f13 y
// f12<->f13, que son consecuencia. Y el `fres` sale de eMathE.cpp:56 (eRecip
// inline), no de la 248.
// r63: EL EJE QUE LA r48 DEJO VIVO ESTA EJECUTADO, Y ES NEGATIVO.
// La r48 escribio: <<Con dos barreras el ciclo (a) casa EXACTAMENTE (107=f13,
// 137=f0) pero el resto del reparto se desmonta (25 filas). Quien retome esto
// tiene ahi el unico eje vivo: cerrar (a) con x2 y arreglar lo que rompe>>.
// Reproducido (banco: eMathE.cpp suelto con los cflags de zEcstasy, 1,5 s):
//   base (asm x1, lo que hay) ....... 14 filas  93,970146 %  268 B
//   asm x2 .......................... 26 filas  87,701490 %  268 B
// Y con x2 el diff NO es <<(a) cerrado y el resto roto>>: se desmonta la cadena
// entera de *sx (filas 29-31 desaparecen y 39/43/44 aparecen), o sea que lo que
// rompe NO es un domino de registros que se pueda repinchar.
// SEIS PINES SOBRE LA BASE x2, uno por medida, para arreglar lo que rompe:
//   halfVP3 fr13 26 | halfVP3 fr12 26 | clipY fr12 26 | clipY fr13 26 |
//   halfVP2 fr0 26  | clipX fr0 22. Todas 268 B. Ninguna se acerca a las 14.
// VEREDICTO: el eje <<x2 + arreglar>> queda CERRADO. Lo unico que sigue sin
// probar en esta funcion es la forma SIN el asm (17 filas, y la r49 demostro con
// lmap que el asm no esta en la fuente original): quien vuelva tiene que buscar
// la forma de fuente que da el horario del objetivo con CERO asm, no una barrera
// mas.
void eProject(float x, float y, float z, Mtx mtx, float *pm, float *vp, float *sx, float *sy, float *sz) {
    Vec local;
    Vec eye;

    local.x = x;
    local.y = y;
    local.z = z;
    MTXMultVec(mtx, &local, &eye);

    if (eye.z == 0.0f) {
        *sy = 0.0f; *sx = 0.0f;
        *sz = -2.0f;
        return;
    }

    float oneOverW;
    float clipX = eye.x * pm[1] + eye.z * pm[2];
    float clipY = eye.y * pm[3] + eye.z * pm[4];
    float clipZ = eye.z * pm[5] + pm[6];

    oneOverW = eRecip(-eye.z);

    float halfVP2 = vp[2] * 0.5f;
    float halfVP3 = vp[3] * 0.5f;

    *sx = vp[0] + halfVP2 + clipX * halfVP2 * oneOverW;
    *sy = vp[1] + halfVP3 + -clipY * halfVP3 * oneOverW;
    *sz = (vp[5] - vp[4]) * clipZ * oneOverW + vp[5];
}

void eRotTransPers(bVector3 *dest, const bVector3 *src, bMatrix4 *wv, bMatrix4 *vs, float xOrig, float yOrig, float width, float height, float zNear,
                   float zFar) {
    float eproj[7] = {0.0f, vs->v0.x, vs->v0.z, vs->v1.y, vs->v1.z, vs->v2.z, vs->v2.w};
    float eviewport[6] = {xOrig, yOrig, width, height, zNear, zFar};
    float mhW2V[3][4];

    eConvertToGX34(mhW2V, *wv);
    eProject(src->x, src->y, src->z, mhW2V, eproj, eviewport, &dest->x, &dest->y, &dest->z);
}

void eCreateAxisRotationMatrix(bMatrix4 *dest, bVector3 &axis, bAngle angle) {
    float c = bCos(angle);
    float s = bSin(angle);
    float t = 1.0f - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float tx = t * x;
    float ty = t * y;

    dest->v0.x = tx * x + c;
    dest->v0.y = tx * y - s * z;
    dest->v0.z = tx * z + s * y;
    dest->v0.w = 0.0f;
    dest->v1.x = tx * y + s * z;
    dest->v1.y = ty * y + c;
    dest->v1.z = ty * z - x * s;
    dest->v1.w = 0.0f;
    dest->v2.x = tx * z - s * y;
    dest->v2.y = ty * z + x * s;
    dest->v2.z = t * z * z + c;
    dest->v2.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    dest->v3.w = 1.0f;
}

void eCreateLookAtMatrix(bMatrix4 *mat, bVector3 &eye, bVector3 &center, bVector3 &up) {
    bVector3 c = center - eye;
    bNormalize(&c, &c);
    bVector3 b = -up;
    bVector3 a;
    bMatrix4 tl;

    bCross(&a, &b, &c);
    bCross(&b, &c, &a);
    bNormalize(&a, &a);
    bNormalize(&b, &b);

    mat->v0.x = a.x;
    mat->v0.y = b.x;
    mat->v0.z = c.x;
    mat->v0.w = 0.0f;
    mat->v1.x = a.y;
    mat->v1.y = b.y;
    mat->v1.z = c.y;
    mat->v1.w = 0.0f;
    mat->v2.x = a.z;
    mat->v2.y = b.z;
    mat->v2.z = c.z;
    mat->v2.w = 0.0f;
    mat->v3.x = 0.0f;
    mat->v3.y = 0.0f;
    mat->v3.z = 0.0f;
    mat->v3.w = 1.0f;

    tl.v0.x = 1.0f;
    tl.v0.y = 0.0f;
    tl.v0.z = 0.0f;
    tl.v0.w = 0.0f;
    tl.v1.x = 0.0f;
    tl.v1.y = 1.0f;
    tl.v1.z = 0.0f;
    tl.v1.w = 0.0f;
    tl.v2.x = 0.0f;
    tl.v2.y = 0.0f;
    tl.v2.z = 1.0f;
    tl.v2.w = 0.0f;
    tl.v3.x = -eye.x;
    tl.v3.y = -eye.y;
    tl.v3.z = -eye.z;
    tl.v3.w = 1.0f;
    eMulMatrix(mat, &tl, mat);
}

float eSin(float a) {
    const float twopi = UMath::PI * 2;
    float flip_sign;

    while (a > twopi) {
        a -= twopi;
    }

    while (a < 0.0f) {
        a += twopi;
    }

    flip_sign = 1.0f;

    const float pi = UMath::PI;
    const float piover2 = pi / 2.0f;
    float result;

    if (a >= pi) {
        a -= pi;
        flip_sign = -1.0f;
    }

    if (a >= piover2) {
        a = pi - a;
    }

    float a2 = a * a;
    float a3 = a * a2;
    result = a + a3 * -0.16666657f;
    float a5 = a3 * a2;
    result += a5 * 0.0083330255f;
    float a7 = a5 * a2;
    result += a7 * -0.00019807414f;
    float a9 = a7 * a2;
    result += a9 * 2.601887e-06f;

    return result * flip_sign;
}

void eCreateRotationZ(bMatrix4 *dest, bAngle angle) {
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    dest->v0.x = cn;
    dest->v0.y = sn;
    dest->v0.z = 0.0f;
    dest->v0.w = 0.0f;
    dest->v1.x = -sn;
    dest->v1.y = cn;
    dest->v1.z = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.x = 0.0f;
    dest->v2.y = 0.0f;
    dest->v2.z = 1.0f;
    dest->v2.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    dest->v3.w = 1.0f;
}

void eRotateX(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 r;
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    r.v0.x = 1.0f;
    r.v0.y = 0.0f;
    r.v0.z = 0.0f;
    r.v0.w = 0.0f;
    r.v1.x = 0.0f;
    r.v1.y = cn;
    r.v1.z = sn;
    r.v1.w = 0.0f;
    r.v2.x = 0.0f;
    r.v2.y = -sn;
    r.v2.z = cn;
    r.v2.w = 0.0f;
    r.v3.x = 0.0f;
    r.v3.y = 0.0f;
    r.v3.z = 0.0f;
    r.v3.w = 1.0f;
    eMulMatrix(dest, a, &r);
}

void eRotateY(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 r;
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    r.v0.x = cn;
    r.v0.y = 0.0f;
    r.v0.z = -sn;
    r.v0.w = 0.0f;
    r.v1.x = 0.0f;
    r.v1.y = 1.0f;
    r.v1.z = 0.0f;
    r.v1.w = 0.0f;
    r.v2.x = sn;
    r.v2.y = 0.0f;
    r.v2.z = cn;
    r.v2.w = 0.0f;
    r.v3.x = 0.0f;
    r.v3.y = 0.0f;
    r.v3.z = 0.0f;
    r.v3.w = 1.0f;
    eMulMatrix(dest, a, &r);
}

void eRotateZ(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 rot;

    eCreateRotationZ(&rot, angle);
    eMulMatrix(dest, a, &rot);
}

void eTranslate(bMatrix4 *dest, bMatrix4 *a, bVector3 *tran) {
    bMatrix4 t;

    t.v0.x = 1.0f;
    t.v0.y = 0.0f;
    t.v0.z = 0.0f;
    t.v0.w = 0.0f;
    t.v1.x = 0.0f;
    t.v1.y = 1.0f;
    t.v1.z = 0.0f;
    t.v1.w = 0.0f;
    t.v2.x = 0.0f;
    t.v2.y = 0.0f;
    t.v2.z = 1.0f;
    t.v2.w = 0.0f;
    t.v3.x = tran->x;
    t.v3.y = tran->y;
    t.v3.z = tran->z;
    t.v3.w = 1.0f;
    eMulMatrix(dest, a, &t);
}

void eCreateTranslationMatrix(bMatrix4 *dest, bVector3 &tran) {
    dest->v0.x = 1.0f;
    dest->v0.y = 0.0f;
    dest->v0.z = 0.0f;
    dest->v0.w = 0.0f;
    dest->v1.x = 0.0f;
    dest->v1.y = 1.0f;
    dest->v1.z = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.x = 0.0f;
    dest->v2.y = 0.0f;
    dest->v2.z = 1.0f;
    dest->v2.w = 0.0f;
    dest->v3.x = tran.x;
    dest->v3.y = tran.y;
    dest->v3.z = tran.z;
    dest->v3.w = 1.0f;
}

bMatrix4 *eInvertMatrix(bMatrix4 *dest, bMatrix4 *m) {
    MTX44Inverse(*reinterpret_cast<Mtx44 *>(m), *reinterpret_cast<Mtx44 *>(dest));
    return dest;
}

bMatrix4 *eInvertTransformationMatrix(bMatrix4 *dest, const bMatrix4 *tm) {
    {
        bMatrix4 m1;
        bMatrix4 m2;
        float x0 = tm->v0.x;
        float y0 = tm->v0.y;
        float z0 = tm->v0.z;
        float x1 = tm->v1.x;
        float y1 = tm->v1.y;
        float z1 = tm->v1.z;
        float x2 = tm->v2.x;
        float y2 = tm->v2.y;
        float z2 = tm->v2.z;
        float x3 = tm->v3.x;
        float y3 = tm->v3.y;
        float z3 = tm->v3.z;

        m1.v0.x = x0;
        m1.v0.y = x1;
        m1.v0.z = x2;
        m1.v0.w = 0.0f;
        m1.v1.x = y0;
        m1.v1.y = y1;
        m1.v1.z = y2;
        m1.v1.w = 0.0f;
        m1.v2.x = z0;
        m1.v2.y = z1;
        m1.v2.z = z2;
        m1.v2.w = 0.0f;
        m1.v3.x = 0.0f;
        m1.v3.y = 0.0f;
        m1.v3.z = 0.0f;
        m1.v3.w = 1.0f;

        m2.v0.x = 1.0f;
        m2.v0.y = 0.0f;
        m2.v0.z = 0.0f;
        m2.v0.w = 0.0f;
        m2.v1.x = 0.0f;
        m2.v1.y = 1.0f;
        m2.v1.z = 0.0f;
        m2.v1.w = 0.0f;
        m2.v2.x = 0.0f;
        m2.v2.y = 0.0f;
        m2.v2.z = 1.0f;
        m2.v2.w = 0.0f;
        m2.v3.x = -x3;
        m2.v3.y = -y3;
        m2.v3.z = -z3;
        m2.v3.w = 1.0f;

        eMulMatrix(dest, &m2, &m1);
    }
    return dest;
}

bMatrix4 *eInvertRotationMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bTransposeMatrix(dest, src);
    dest->v0.w = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.w = 0.0f;
    dest->v3.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    return dest;
}

float ePowf(float x, float y) {
    return powf(x, y);
}
