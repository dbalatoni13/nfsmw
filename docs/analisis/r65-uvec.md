# r65 — uvec: los 103 pines de `UVectorMath.cpp`, retirados

**Resultado: 103 → 0.** Y de propina la única barrera `asm("")` del fichero,
104 → 0. **Cero irreducibles.** Las 41 funciones `VU0_*` del objeto salen
**byte a byte idénticas** y las ocho secciones ALLOC de `zFoundation.o` no se
mueven.

| | antes | después |
|---|---:|---:|
| pines `register T x asm("rN")` | **103** | **0** |
| barreras `asm("" : …)` | 1 | 0 |
| funciones reescritas | — | 33 |
| irreducibles | — | **0** |

Sello ALLOC de `build/GOWE69/src/Speed/Indep/SourceLists/zFoundation.o`
(`.text`, `.rodata`, `.data`, `.bss`, `.ctors` y sus `.rela.*`; ignorando
`.line`, `.debug*`, `.comment` y `.stab*`):

```
                  ANTES                       DESPUES
.bss            228   da39a3ee5e6b4b0d       228   da39a3ee5e6b4b0d
.ctors            4   9069ca78e7450a28         4   9069ca78e7450a28
.data           248   260f0a47384997de       248   260f0a47384997de
.rela.ctors      12   c391c43bc674d2fe        12   c391c43bc674d2fe
.rela.rodata     72   f41e944d3f4285c5        72   f41e944d3f4285c5
.rela.text    23376   025a45cd25662e92     23376   025a45cd25662e92
.rodata        2328   86e81eb73835a166      2328   86e81eb73835a166
.text         36184   d9db593e2f103afd     36184   d9db593e2f103afd
TOTAL-ALLOC          8015d53355947702            8015d53355947702
```

**Control de que la medida está viva** (la trampa nº 3 del encargo:
`build_direct` puede dejar el `.o` viejo y decir «1 ok»). Metiendo un cambio
real —`float s = e[0]+e[5]+e[10]` → `… + e[15]` en `VU0_m4toquat`— el sello **sí**
se mueve, a `.text 36192 / 48bb3a75b94f2a7b`, `TOTAL 135438cbc906591f`; al
revertirlo vuelve exactamente a `d9db593e2f103afd`. La cadena mide de verdad.

**Aviso lateral que me costó una medida falsa: `build_direct.py` escribe en
`build/GOWE69/src/…/zFoundation.o`, no en `build/GOWE69/obj/…`.** El de `obj/` es
el objeto **extraído del objetivo** y llevaba ahí desde agosto; sellarlo da
«idéntico» pase lo que pase. Sello del bueno o no hay medida.

---

## 1. El diagnóstico: no eran pines de reparto, era una transcripción

Los 103 no eran el andamio clásico sobre código C. Eran las ataduras de operando
de bloques `asm` monolíticos en los que **los números de registro del objetivo
estaban escritos a pelo**:

```c
void VU0_v3crossprod(const UMath::Vector3 &a, …) {
    register const UMath::Vector3 *pa asm("r3") = &a;      // ← andamio
    register const UMath::Vector3 *pb asm("r4") = &b;      // ← andamio
    register UMath::Vector3 *pd asm("r5") = &dest;         // ← andamio
    asm ("psq_l 9, 0(4), 0, 0\n\t"
         "lfs 10, 8(3)\n\t" … : : "r"(pa), "r"(pb), "r"(pd));
}
```

Los pines existían **sólo** porque el cuerpo nombraba `3`, `4`, `5` y `1`
literalmente. Es una transcripción del desensamblado, no una decompilación.

## 2. El oráculo: el DWARF nombra las locales, una por registro

`symbols/mw_dwarfdump.nothpp` lista, para **cada** `VU0_*`, las locales del
original con su registro, y los rangos coinciden con `config/GOWE69/symbols.txt`
(`VU0_v3crossprod` = `0x80193010`, tamaño `0x3C`):

```
void VU0_v3crossprod(const struct Vector3 & a /* r3 */, …) {
    float t0; // f8      float t3; // f11     float t6; // f0
    float t1; // f9      float t4; // f12
    float t2; // f10     float t5; // f13
}
```

Siete locales `float` en siete FPR distintos y sin marco. Eso **no** lo produce
un `asm` con registros a pelo: lo produce **un solo `asm` con siete operandos de
salida `"=f"`**, porque GCC obliga a que los operandos de un mismo `asm` vivan en
registros distintos. Y delata el detalle que ninguna otra pista daba: **`t5`
(f13) no aparece en el cuerpo** y aun así hay que declararlo, porque lo que fija
el reparto es el NÚMERO de operandos, no los que se usen.

La forma correcta, con los registros elegidos por GCC:

```c
void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "lfs %2, 8(%7)\n\t"
        …
        "psq_st %1, 4(%9), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&a), "b"(&b), "b"(&dest));
}
```

Sale **idéntica a la primera compilación**, y el mismo molde vale para 31
funciones. Ojo al efecto colateral bueno: con operandos, el `addi 9,3,8` +
`lfs 13,0(9)` que `-fforce-addr` mete cuando se escribe `a.z` en C **no aparece**
(la plantilla lleva el `8(%7)`), y tampoco aparece el `stwu 1,-16(1)` que sale al
intentarlo con los intrínsecos `__builtin_ps_*`.

## 3. Las tres reglas, medidas

**(1) El reparto de los FPR de salida es determinista, y se calcula.**
`REG_ALLOC_ORDER` (`orig/prodg/NGC_GNU_SRC/NGC/gcc/config/rs6000/rs6000.h:932`)
empieza `{32, 45,44,43,42,41,40,39,…}`, o sea **f0, f13, f12, f11, f10, f9, f8…**.
Con *N* salidas, la **última** se lleva f0 y las anteriores van hacia atrás. De
ahí la fórmula que usé para traducir cada plantilla sin adivinar:

> operando de `fk` = `N - 15 + k` (y `f0` → `N - 1`), con `N = 15 - min(fk usado)`.

Comprobada contra el DWARF en las 31: `crossprod` N=7 (f8→`%0` … f0→`%6`),
`v3add` N=6 (f9→`%0` … f0→`%5`), `v3dotprod` N=4 (f11→`%0` … f0→`%3`),
`v3scale(float)` N=2 (f13→`%0`, f0→`%1`). Cuatro de cuatro, y las otras 27 con
ellas.

**(2) El valor de retorno va SIEMPRE de operando 0, y se llama `result`.**
Se lleva f1 por la sugerencia de la copia del `return`, o sea **sale del reparto**
y no consume posición. Medido: con `result` al final —que es como lo **declara**
el DWARF de `VU0_v3dotprod`— los otros cuatro registros se desplazan
(`t0→f0, t1→f11, t2→f12, t3→f13` en vez de `f11, f12, f13, f0`) y las tres
`*dotprod*` no casan. Moviéndolo a operando 0, casan.
**El orden de DECLARACIÓN y el orden de OPERANDOS son cosas distintas**:
`VU0_v3distancesquare` declara `result` primero y `VU0_v3dotprod` lo declara
último, y las dos quieren `result` de operando 0.

**(3) Con una entrada `"f"(escalar)` en fr1, las salidas necesitan earlyclobber
`"=&f"`.** Sin la `&`, GCC le da f1 a la primera salida y **pisa el argumento**:
`ps_muls0 1, 1, 1`. Ocho funciones (`v3scale`, `v4scalexyz`, `v3addscale`,
`v4addscale`, `v4addscalexyz`, `v3scaleadd`, `v4scaleaddxyz`, `v4scaleadd`)
pasaron de «distinto» a idénticas con ese único carácter. Y es además lo
*correcto*: sin `&` el asm está mal escrito, no sólo desalineado.

`volatile` va **sólo** donde ninguna salida se lee después; si no, el `asm` es
código muerto y GCC lo borra entero.

## 4. `VU0_sqrt`: dos pines, y era C de verdad

El DWARF dice: parámetro `a` en **f12** y **un bloque anónimo cuya única local es
`guess`, en f1** (rango `0x80193434`, que es justo la dirección del `frsqrte`).
Eso no es un bloque de ensamblador: es un `if` con cuerpo.

```c
float VU0_sqrt(const float a) {
    if (a != lbl_803EB518) {                 // 0.0f
        float guess;
        asm volatile ("frsqrte %0, %1" : "=f"(guess) : "f"(a));
        return (a * guess + lbl_803EB51C / guess) * lbl_803EB520;   // 1.0f, 0.5f
    }
    return a;
}
```

Banco de 9 formas. Las que fallan y por qué, porque vale para otras funciones:

| forma | resultado |
|---|---|
| `if (a == 0) return a;` + bloque anónimo detrás | 16 insns: aparece `fmr 1,12` + `blr` en una cola aparte y se pierde el `beqlr` |
| lo mismo con `guess` declarada arriba | 16 insns, igual |
| `float ret = a; if (…) ret = …; return ret;` | 13 insns, estructura distinta |
| `return lbl_803EB518;` en la salida temprana | 14 insns pero `lfs 1,…` en vez de `lfs 0,…` |
| **`if (a != 0) { … return expr; } return a;`** | **14 insns, casi** |
| **la anterior + `asm volatile`** | **IDÉNTICA** |
| `+ "memory"` en el clobber | idéntica también (el `volatile` ya basta) |
| `lbl_803EB520 * (…)` | mueve `fmuls 1,13,1` |
| `(1.0f/guess + a*guess)` | idéntica también |

Lo que cierra la función es **el `volatile`**: sin él el planificador sube uno de
los dos `lis` del pool por encima del `frsqrte` y de paso intercambia r9/r11
entre las dos constantes. Con él, el `frsqrte` se queda pegado al `beqlr` como en
el objetivo.

*Nota de lectura que casi me cuesta un falso negativo:* **`bclr 12,2` en la
salida del ensamblador ES `beqlr`.** Mismo encoding. Un comparador textual dice
«distinto» y no lo es.

## 5. `VU0_m4toquat`: los cinco pines eran una variable mal partida

Era el único candidato de verdad al veredicto ESTRUCTURA, y lo fue. Tenía cinco
pines —`t`(fr10), `tr`(fr11), `s`(fr11), `i`(r9), `d`(fr0)— y **el DWARF sólo
declara dos locales**:

```
void VU0_m4toquat(const struct Matrix4 & mat /* r31 */, struct Vector4 & result /* r30 */) {
    float s; // f11
    int i;   // r9
}
```

O sea: `t`, `tr`, `d` y `m00/m11/m22` **no existen** en el original; son
subexpresiones que CSE comparte (el DWARF además lista ~20 `inline const float *
Matrix4::GetElements() const`, uno por uso). Quitándolas todas y dejando `s` e
`i` sueltos, el cuerpo sale **143 instrucciones contra las 147 del objetivo**:
faltan exactamente los cuatro `fmr 11,1`, porque GCC le da f1 a `s` por la
sugerencia del `return` de `VU0_sqrt`.

Ocho intentos de mover `s` a f11 por forma de fuente —declarar `i` antes,
inicializar `s` a 0, `C*s` en vez de `s*C`, quitar la local `e`,
`mat.GetElements()[k]` en cada uso, `m00/m11/m22` como locales, doble
asignación— **fallan todos**: la sugerencia de la copia del retorno es demasiado
fuerte. Y **pinchar `s` a fr11 tampoco vale**: el pin bloquea f11 para todo lo
demás y entonces `tr` ya no puede compartirlo (por eso la fuente vieja
necesitaba pinchar `tr` *y* `s` al mismo registro; con sólo el pin de `s` salen
147 instrucciones pero con `t`/`tr` en f11/f0 en vez de f10/f11).

La salida no era mover `s`: era darse cuenta de que **`tr` y `s` son la misma
variable**. El original guarda la traza en `s` y luego reutiliza `s` para la
raíz. Eso explica el f11 compartido *y* la única local del DWARF:

```c
const float *e = mat.GetElements();
float s = e[0] + e[5] + e[10];      // la traza
int i;
if (s > lbl_803EB53C) {
    s = VU0_sqrt(s + lbl_803EB540); // y ahora la raíz, misma variable
    …
```

**147 instrucciones, idéntica, cero pines**, y con `t`/`d` como subexpresiones
(`(e[0] + e[5])`, `e[10] - (e[0] + e[5])`), que es lo que el DWARF pedía.

## 6. La barrera de regalo

`ps_to_f` —`static inline float ps_to_f(sn_ps v) { float r; asm("" : "=f"(r) : "f"(v)); return r; }`—
y el macro `PS_ST1` estaban **muertos**: una sola aparición cada uno, la suya.
Retirados; sello ALLOC idéntico. Era la única barrera `asm("")` del fichero.

## 7. Lo que esto abre en el resto del frente

- **Donde haya un `asm` con registros a pelo, el DWARF dice cuántos operandos
  hacen falta**, y el reparto no se adivina: se calcula con `REG_ALLOC_ORDER`.
  Un operando de salida que la plantilla no referencia puede ser imprescindible.
- **La regla del earlyclobber es un bug latente, no una preferencia.** Merece
  barrido propio: cualquier `asm` del árbol con salidas `"=f"`/`"=r"` y entradas
  del mismo banco está mal escrito sin `&`, y es muy probable que haya pines
  tapándolo.
- **Una variable reutilizada para dos cosas parece dos locales y no lo es.** En
  `m4toquat` costó cinco pines. El delator es doble: el DWARF con MENOS locales
  que la fuente, y **dos pines al MISMO registro**. Ese segundo síntoma es
  grepeable en todo el árbol.
- **El `volatile` de un `asm` es una palanca de planificación**, no sólo un
  seguro contra el borrado: en `VU0_sqrt` es lo único que separa 14 instrucciones
  bien colocadas de 14 mal colocadas.
- **`build/GOWE69/obj/` no es la salida del build**, es el objetivo extraído.

## 8. Evidencia por función

Las 41 funciones `VU0_*` del objeto, con su sha1 de `.text` antes y después
(41/41 idénticas, 3.172 B):

| funcion (simbolo) | bytes | sha1 `.text` ANTES | sha1 `.text` DESPUES |
|---|---:|---|---|
| `VU0_m4toquat__FRCQ25UMath7Matrix4RQ25UMath7Vector4` | 588 | `5a26f505db51` | `5a26f505db51` |
| `VU0_Matrix4ToEuler__FRCQ25UMath7Matrix4RQ25UMath7Vector3` | 340 | `1d71daf65ba0` | `1d71daf65ba0` |
| `VU0_Atan2__Fff` | 192 | `72832b715878` | `72832b715878` |
| `VU0_v3quatrotate_xlate__FRCQ25UMath7Vector4RCQ25UMath7Vector3T1RQ25UMath7Vector3` | 180 | `c75486f7b24b` | `c75486f7b24b` |
| `VU0_v3quatrotate__FRCQ25UMath7Vector4RCQ25UMath7Vector3RQ25UMath7Vector3` | 160 | `ab42e0415140` | `ab42e0415140` |
| `VU0_MATRIX4setyrot__FRQ25UMath7Matrix4f` | 148 | `911e89567e6e` | `911e89567e6e` |
| `VU0_MATRIX4_vect4mult__FPCQ25UMath7Vector4RCQ25UMath7Matrix4PQ25UMath7Vector4i` | 100 | `d68574d99a62` | `d68574d99a62` |
| `VU0_MATRIX4_vect3mult__FRCQ25UMath7Vector3RCQ25UMath7Matrix4RQ25UMath7Vector3` | 84 | `9a7846240a7b` | `9a7846240a7b` |
| `VU0_MATRIX4_vect4mult__FRCQ25UMath7Vector4RCQ25UMath7Matrix4RQ25UMath7Vector4` | 84 | `71a56bff07e7` | `71a56bff07e7` |
| `VU0_MATRIX3x4_vect3mult__FRCQ25UMath7Vector3RCQ25UMath7Matrix4RQ25UMath7Vector3` | 68 | `1e9fa9412944` | `1e9fa9412944` |
| `VU0_MATRIX3x4_vect4mult__FRCQ25UMath7Vector4RCQ25UMath7Matrix4RQ25UMath7Vector4` | 68 | `1e9fa9412944` | `1e9fa9412944` |
| `VU0_rsqrt__Ff` | 64 | `5b626ac5260d` | `5b626ac5260d` |
| `VU0_v3crossprod__FRCQ25UMath7Vector3T0RQ25UMath7Vector3` | 60 | `8bcc615604a4` | `8bcc615604a4` |
| `VU0_v4crossprodxyz__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 60 | `8bcc615604a4` | `8bcc615604a4` |
| `VU0_sqrt__Ff` | 56 | `c93f8863edea` | `c93f8863edea` |
| `VU0_v3addscale__FRCQ25UMath7Vector3T0fRQ25UMath7Vector3` | 44 | `9b19f294834e` | `9b19f294834e` |
| `VU0_v3scaleadd__FRCQ25UMath7Vector3fT0RQ25UMath7Vector3` | 44 | `d95740f29989` | `d95740f29989` |
| `VU0_v4addscale__FRCQ25UMath7Vector4T0fRQ25UMath7Vector4` | 44 | `b428af14b1a5` | `b428af14b1a5` |
| `VU0_v4addscalexyz__FRCQ25UMath7Vector4T0fRQ25UMath7Vector4` | 44 | `9b19f294834e` | `9b19f294834e` |
| `VU0_v4scaleadd__FRCQ25UMath7Vector4fT0RQ25UMath7Vector4` | 44 | `1a2963bdc76d` | `1a2963bdc76d` |
| `VU0_v4scaleaddxyz__FRCQ25UMath7Vector4fT0RQ25UMath7Vector4` | 44 | `d95740f29989` | `d95740f29989` |
| `VU0_v3distancesquare__FRCQ25UMath7Vector3T0` | 40 | `27e99da43069` | `27e99da43069` |
| `VU0_v4distancesquarexyz__FRCQ25UMath7Vector4T0` | 40 | `27e99da43069` | `27e99da43069` |
| `VU0_v3add__FRCQ25UMath7Vector3T0RQ25UMath7Vector3` | 36 | `bca127121bc2` | `bca127121bc2` |
| `VU0_v3distancesquarexz__FRCQ25UMath7Vector3T0` | 36 | `7c49f0ebaa3c` | `7c49f0ebaa3c` |
| `VU0_v3scale__FRCQ25UMath7Vector3T0RQ25UMath7Vector3` | 36 | `554b4b847db4` | `554b4b847db4` |
| `VU0_v3sub__FRCQ25UMath7Vector3T0RQ25UMath7Vector3` | 36 | `0b6901997f03` | `0b6901997f03` |
| `VU0_v4add__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 36 | `8af46fa48051` | `8af46fa48051` |
| `VU0_v4addxyz__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 36 | `bca127121bc2` | `bca127121bc2` |
| `VU0_v4scalexyz__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 36 | `554b4b847db4` | `554b4b847db4` |
| `VU0_v4sub__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 36 | `a9431058bfb5` | `a9431058bfb5` |
| `VU0_v4subxyz__FRCQ25UMath7Vector4T0RQ25UMath7Vector4` | 36 | `0b6901997f03` | `0b6901997f03` |
| `VU0_v3dotprod__FRCQ25UMath7Vector3T0` | 32 | `0f4061fc9393` | `0f4061fc9393` |
| `VU0_v4dotprod__FRCQ25UMath7Vector4T0` | 32 | `f833b63d16b2` | `f833b63d16b2` |
| `VU0_v4dotprodxyz__FRCQ25UMath7Vector4T0` | 32 | `0f4061fc9393` | `0f4061fc9393` |
| `VU0_v3scale__FRCQ25UMath7Vector3fRQ25UMath7Vector3` | 28 | `e0dc8909e57f` | `e0dc8909e57f` |
| `VU0_v4scale__FRCQ25UMath7Vector4fRQ25UMath7Vector4` | 28 | `e79562c9be7c` | `e79562c9be7c` |
| `VU0_v4scalexyz__FRCQ25UMath7Vector4fRQ25UMath7Vector4` | 28 | `e0dc8909e57f` | `e0dc8909e57f` |
| `VU0_v3lengthsquare__FRCQ25UMath7Vector3` | 24 | `dc26ed463fea` | `dc26ed463fea` |
| `VU0_v4lengthsquare__FRCQ25UMath7Vector4` | 24 | `2f57b05d2401` | `2f57b05d2401` |
| `VU0_v4lengthsquarexyz__FRCQ25UMath7Vector4` | 24 | `dc26ed463fea` | `dc26ed463fea` |

## 9. Qué NO se tocó

- `UVectorMath.hpp` (la cabecera con los bloques `#if defined(__ANDROID__)`): **sin
  tocar**. Todo el trabajo es del `.cpp`, que sólo entra por
  `zFoundation.cpp:85`, así que ninguna otra unidad puede verse afectada
  (comprobado: es el único `#include` del `.cpp` en todo el árbol). Aun así se
  recompilaron las 33 unidades: `33 ok, 0 fallidas`.
- `configure.py`, `config/GOWE69/*`, `splits.txt`: sin tocar. Sin commit. Sin
  `lcfix.py`. Sin re-extraer.
- El bloque `asm()` de ámbito de fichero con el pool de `lbl_803EB518…568` sigue
  igual: no es un andamio, es el pool de `rodata` del objetivo.
