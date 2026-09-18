# r36c — zCamera: las 7 funciones (11.928 B)

**Estado del arbol al terminar: UN solo fichero tocado**,
`src/Speed/Indep/Libs/Support/Utility/UTLVector.h` (3 lineas). Todo lo demas
—`ICEMover.cpp`, `ICEMath.hpp`, `TrackCop.cpp`— queda **exactamente como estaba**.
`lcfix.py --check zCamera`: *todas las entradas @lc estan al dia*.

## 0. Verificacion del encargo

`triaje.py zCamera` reproduce el encargo fila por fila. Siguen siendo **7**, no 6:
el brief se deja `LoadCameraShakes` (168 B), ya barrido en la r36b.

| B | funcion | antes | **despues** |
|---|---|---|---|
| 3868 | `Update__8ICEMoverf` | 99,907 | 99,907 |
| 3604 | `__static_initialization_and_destruction_0` | 97,112 | 97,112 |
| 1192 | `TerrainVelocityNoise` | 99,077 | 99,077 |
| 1156 | `_Storage<CameraAI::Director*,2>` | 97,834 | **98,699 y TAMANO EXACTO** |
| 992 | `Update__19TrackCarCameraMoverf` | 99,629 | 99,629 |
| 948 | `Update__19TrackCopCameraMoverf` | 99,156 | 99,156 |
| 168 | `LoadCameraShakes` | 96,667 | 96,667 |

---

## 1. CIERRA EL FRENTE DE `_Storage`: 1.160 -> **1.156 B, el tamano del objetivo**

La r36b dejo el diagnostico cerrado y un negativo medido; le faltaba **la segunda
mitad de la palanca**.

### El diagnostico (r36b, confirmado)

`combine` funde `mr`+`cmpwi` en `mr.` **solo si entre la copia y la comparacion
no hay ningun uso del registro copiado** (`try_combine` mueve el computo al sitio
de `i3` y borra `i2`; un uso intermedio lo prohibe). En el nuestro el `subf` leia
`r27` —la COPIA— en medio:

    objetivo:  add r24,r0,r9 / mr. r27,r0 / subf r9,r0,r24     <- resta desde srcBeg
    nuestro:   mr r27,r9 / add r24,r9,r0 / subf r0,r27,r24 / cmpwi r27,0

### Por que el intercambio obvio no bastaba

Declarar `minSize` antes que `srcIt` **si** consigue el `mr.`, pero de propina GCC
**pliega** `(srcBeg + n*4) - srcBeg` a `n*4` y se lleva el `add`+`subf` que el
objetivo si tiene: 97,834 -> 96,606 % y **1.152 B** (4 B de MENOS que el objetivo).
Eso es lo que midio la r36b y por eso lo dio por cerrado.

### Lo que faltaba: reordenar **y** una barrera de cero bytes que impida el plegado

```cpp
void assign(const_iterator srcBeg, const_iterator srcEnd) {
    asm("" : "+r"(srcEnd));                 // <- impide que CSE vea srcEnd == srcBeg + n*4
    size_type minSize = srcEnd - srcBeg;
    const_iterator srcIt = srcBeg;
    if (srcIt == 0) {
```

**97,8339 % -> 98,6990 %, y el tamano pasa de 1.160 a 1.156 B: el del objetivo,
exacto.** Sale el `mr. r27, r9` y sobreviven el `add` y el `subf`.

Esto es un **tercer uso de la barrera selectiva**, distinto de los dos del brief
(frenar un adelanto del planificador; impedir el coalescing): aqui **impide un
PLEGADO de CSE** que borra instrucciones que el objetivo tiene. La firma es
«reordenar da la forma buena pero pierde instrucciones»: entonces la barrera va
sobre **el operando que hace visible la identidad algebraica**, no sobre el que
cambia de registro.

### Lo que queda en `_Storage` (98,699 %, 22 filas, tamano ya exacto)

1. **r24 <-> r25 intercambiados** (el 0 constante contra `srcEnd`): permutacion
   limpia del asignador. Ningun local con nombre -> el pin no llega.
2. **r0 <-> r9** en las dos `lwz` de `mBegin`/`mSize`: mismo orden, otro nombre.
3. El objetivo **guarda `size()` en r4 y lo reutiliza** (indices 66-74 y 130-132);
   nosotros lo recargamos en un sitio y lo ahorramos en el otro, **y se compensan
   exactamente**: por eso el tamano ya cuadra. Reproducirlo pide una local con
   nombre en `assign` (`size_type curSize = size();`), que es cabecera compartida.

### Efecto en las otras 12 unidades que usan `UTL::Vector` — MEDIDO

`pctsnap --cmp` sobre las **13** unidades que llevan simbolos de `UTL::Vector`
(zAI, zCamera, zEAXSound, zEAXSound2, zFe2, zFoundation, zMain, zMiscSmall,
zPhysics, zSim, zSpeech, zWorld, zWorld2), reconstruidas las 13 antes y despues:

```
MEJORAN: 2
  +0.865 pp  zCamera   _Storage<CameraAI::Director*,2>   97.8339 -> 98.6990  1156 B (-4)
  +0.147 pp  zWorld2   InitAtSegment__8WRoadNavscf       99.0049 -> 99.1520   816 B
EMPEORAN: 1
  -0.357 pp  zEAXSound SetupNextLoad__14EAXAemsManager   98.1151 -> 97.7579  1008 B (+4)
```

**Solo se mueven 3 funciones de 8.831.** Ninguna cruza el 100 %, asi que
`matched_code` **no cambia hoy** en ninguna direccion. **Quien integre debe saber
que hay 4 B de coste en `SetupNextLoad` (zEAXSound)**; lo dejo aplicado porque el
tamano exacto de `_Storage` es una ganancia estructural y las otras dos filas van
a favor, pero es una decision que le corresponde a quien monte la ronda.

**Alternativa medida con el signo contrario** (por si se prefiere): la misma
reordenacion con la barrera sobre **`srcBeg`** en vez de `srcEnd` da
`SetupNextLoad` **98,671 %** (mejor que la base) pero `_Storage` se queda en
98,484 % y **1.160 B** (la barrera materializa un `mr r0, r11` de mas).

---

## 2. `ICEMover::Update` (3.868 B): las 15 diferencias EXPLICADAS y reproducidas — falta UNA instruccion

La r36b quemo aqui el permutador guiado (393 variantes de un cambio) y lo dejo en
«reparto». **No era reparto: era la forma de `ICE::SignedMod`.**

### Lo que pasa

`SignedMod` tiene **un solo llamante en todo el arbol** (`ICEMover.cpp:675`) y
esta en mi territorio (`Camera/ICE/ICEMath.hpp`). El objetivo reparte asi:

    r8  = el valor entrante (FloatToInt) Y el resultado
    r11 = la copia de trabajo del bucle
    lwz r8,0x15c(r1) / mr r11,r8 / cmpwi r8,0 / add. r11,r11,r10 / subf r8,r0,r11

Nosotros, con `SignedMod` mutando su propio parametro, fundimos el resultado con
la copia de trabajo: todo cae en r11 y el valor entrante se queda en r9.

### El cambio que cierra las 15 (ensayo 10/12, MEDIDO)

```cpp
inline int SignedMod(int a, int b) {
    int t = a;                       // <- copia de trabajo explicita, ANTES del if
    if (b > 0) {
        while (t < 0) { t += b; }
        int c = t / b;
        c = c * b;
        a = t - c;                   // <- el resultado va al parametro
    } else {
        a = 0;
    }
    return a;
}
```

y en `ICEMover.cpp:675`, partir la sentencia:

```cpp
int frame = ICE::FloatToInt(fParam * length * current_sec);
frame = ICE::SignedMod(frame, pShake->GetNumKeys());
```

**Resultado: de 15 filas de diff a 3.** Todo el racimo 596-617 —`cmpwi r8`,
`add. r11,r11,r10`, `divw r0,r11,r10`, `subf r8`, `li r8`, `extsh r9,r7`,
`subi r0`, `li r11`, `mr r11,r8`, `mr r10,r11`, `cmpw r0,r11`, `cmpw r8,r10`,
`mulli r9,r8,0x18`— **casa instruccion a instruccion**.

Lo unico que queda son tres filas:

    objetivo:  lwz r8,0x15c(r1) / mr r11,r8
    nuestro:   lwz r9,0x15c(r1) / mr r8,r9 / mr r11,r8      <- UN `mr` de mas

Ese `mr r8, r9` es **la ligadura del parametro inline**: `expand_inline_function`
copia el argumento a un pseudo nuevo **porque el cuerpo asigna a `a`**.

### El callejon: las dos condiciones se excluyen en nuestro compilador

- Si `a` **no** se asigna, no hay copia de ligadura (bien) **pero GCC funde
  tambien `t` con `a`** y desaparece el `mr r11,r8` que el objetivo si tiene
  -> 3.864 B, una instruccion de MENOS. Cuatro formas medidas, las cuatro igual
  (ensayos 9, 11, 15, 16, 17), incluida la que escribe el peel a mano
  (`if (a<0) { do {...} while (t<0); }`) para que la guarda lea el original.
- Si `a` **si** se asigna, sale el `mr r11,r8` bueno **y** la copia de ligadura
  -> 3.872 B, una instruccion de MAS.

Por eso **NO lo dejo aplicado**: 99,881 % con 3.872 B no aporta un byte
(`matched_code` es todo-o-nada) y ademas rompe el tamano exacto que la base ya
tiene. Pero el diagnostico pasa de «15 diferencias de registro, permutador» a
**«una copia de ligadura de parametro inline»**, que es un problema con nombre.

**Para quien lo retome:** el resto del brief no ayuda aqui. `register int frame
asm("r8")` esta medido **tres veces y las tres negativo** (sobre la base:
99,907 -> 99,809 y +4 B; partido: -> 99,762; sobre el ensayo 12: -> 99,674 y
+8 B): el pin aterriza en el DESTINO y añade `mr r8, r11` al final.

---

## 3. `TerrainVelocityNoise` (1.192 B) y `TrackCar::Update` (992 B): TECHO de forma de fuente

`regmap` dice **IDENTICO** en las dos (mismas locales, mismo arbol, mismo
reparto), asi que segun la regla del proyecto toca **permutador CIEGO con el
catalogo entero**. Hecho, y es la primera vez en las dos:

| funcion | variantes | resultado |
|---|---|---|
| `CameraMover::TerrainVelocityNoise` | **234** de un cambio | **ninguna mejora; para en la ronda 1** |
| `TrackCarCameraMover::Update` | **212** de un cambio | **ninguna mejora; para en la ronda 1** |

- **TerrainVelocityNoise**: las 14 filas son el `bDot` inline de
  `bClamp(bDot(GetAcceleration(), GetForwardVector()), 0, 20) * 0.05f`. Los cinco
  literales **casan uno a uno** ($LC455/456/462/463/464/465/466 contra
  0x803D1D6C/70/84/88/8C/90/94): **el pool esta bien**. Lo que baila es (a) f0 y
  f12 intercambiados en la cadena `fmuls`/`fmadds` y (b) que nuestro `lis` del
  literal 0.0f se adelanta a la 2ª posicion y el del objetivo va en la 5ª. Los dos
  temporales son **anonimos** -> el pin de registro **no tiene donde agarrarse**.
- **TrackCar**: tres subracimos. El tercero (134-143) tiene **el mismo orden y
  solo cambia el nombre del registro flotante**, o sea que cuelga de los dos
  primeros, que son el orden de los tres `stfs` de `Look = *GetGeometryPosition()`
  y de `displacement /= distance` (el objetivo emite z,x,y; nosotros x,y,z). Eso
  sale de `bCopy`/`operator/=` de **`bMath.hpp`, fuera de territorio**.

---

## 4. `__static_initialization_and_destruction_0` (3.604 B): racimo A acotado

Las 54 diferencias siguen siendo dos racimos (r36b). El **racimo A** son **6
filas** alrededor del indice 549 y ahora estan leidas:

    objetivo:  lis r11,$LC1125 ... li r9,0x38e ... lis r10,HydraulicsLookAngle
               lfs f24,$LC1125@l(r11) / lis r11,NOSFov / sth r0,NOSFov@l(r11)
               sth r9,HydraulicsLookAngle@l(r10)
    nuestro:   lis r11,HydraulicsLookAngle ... lis r9,$LC1125 ... li r10,0x38e
               lfs f24,$LC1125@l(r9) / lis r9,NOSFov / sth r0,NOSFov@l(r9)
               sth r10,HydraulicsLookAngle@l(r11)

O sea: para `HydraulicsLookAngle = bDegToAng(5.0f)` (`Cubic.cpp:357`) **el
objetivo materializa antes el VALOR (`li 0x38e`) y despues la DIRECCION; nosotros
al reves**, y el `lis` del literal flotante $LC1125 se cuela en medio. **Mismo
numero de instrucciones**: es un empate de `sched2` dentro de un bloque basico de
~900 instrucciones, donde todos los candidatos estan listos a la vez y el
desempate final es el LUID, que aqui lo fija el orden de expansion de los
inicializadores estaticos de **toda la SourceList**.

Es decir: **la misma familia que el racimo B**, que la r36b ya probo que no es
culpa de la fuente (repro minimo identico al objetivo; lo decide cuanto codigo de
inicializacion estatica hay delante, via `cse2` sobre `-fforce-addr`). **No hay
palanca de fuente aqui**: ni el pin (no hay locales; es una funcion sintetica sin
DWARF), ni la etiqueta de tipo (no hay bloques que abrir: `regmap` no reporta
ningun `lexical_block` de mas ni de menos), ni la barrera (no hay llamada de por
medio ni valor en registro preservado).

---

## 5. La etiqueta de tipo (`enum { _tag };`) NO aplica en zCamera

El brief la pide «cada vez que `regmap`/`dwbody` diga que el original abre un
bloque que nosotros no». **Lo comprobe en las seis**: `regmap --all` dice
`0 de ambito equivocado` y mismo arbol de bloques en `ICEMover` (b0..b6 y b3/b0),
`TrackCop`, `TrackCar`, `TerrainVelocityNoise` y `_Storage`. **En zCamera no hay
ni un bloque vacio que reproducir**; la palanca 4 no tiene diana aqui.

---

## 6. Vedas nuevas de esta ronda (10, todas medidas)

| # | funcion | cambio | resultado |
|---|---|---|---|
| 1 | `ICEMover` | `register int frame asm("r8")` sobre la base | 99,907 -> **99,809**, +4 B (mete `mr r8,r11` al final) |
| 2 | `ICEMover` | partir `frame = FloatToInt(...)` / `= SignedMod(frame,...)` | **binario identico** (GCC las funde) |
| 3 | `ICEMover` | lo anterior + pin a `r8` | 99,907 -> **99,762**, +4 B |
| 4 | `ICEMover` | pin sobre el ensayo 12 (el bueno) | 99,881 -> **99,674**, +8 B |
| 5 | `SignedMod` | resultado en local nueva `r`, `a` mutado en el bucle | 99,907 -> **99,767**, −4 B (borra el `mr` bueno) |
| 6 | `SignedMod` | `int t=a` + dos `return` (parametro sin mutar) | -> **99,783**, −4 B |
| 7 | `SignedMod` | `int t=a` fuera + `int r` (parametro sin mutar) | -> **99,767**, −4 B |
| 8 | `SignedMod` | `int t=a` **dentro** del `if` + `int r` | -> **99,710**, −4 B |
| 9 | `SignedMod` | peel escrito a mano `if (a<0) do{...}while(t<0)` | -> **99,767**, −4 B |
| 10 | `TrackCop` | `bVector3 *p_look=&look_offset; asm("" : "+r"(p_look));` como 3er argumento de `eMulVector` | 99,156 -> **98,823** |
| 11 | `Vector::assign` | mover `minSize` **dentro** de cada rama del `if` | `_Storage` 97,834 -> **21,073 %** (¡y `SetupNextLoad` sube a 98,671!) |
| 12 | `Vector::assign` | `if (srcBeg == 0)` en vez de `if (srcIt == 0)` | **binario identico** (GCC canonicaliza los dos) |

Y dos **techos** nuevos con cifra: permutador ciego de un cambio, **234 variantes
en `TerrainVelocityNoise`** y **212 en `TrackCar`**, cero mejoras las dos.

---

## 7. Lo que dejo abierto, por orden de premio

1. **`ICEMover::Update` (3.868 B) esta a UNA instruccion.** Aplicar el parche de
   la seccion 2 y atacar la copia de ligadura del parametro inline: hace falta
   que `a` se asigne (para que exista `mr r11,r8`) **sin** que
   `expand_inline_function` copie el argumento. Ideas no probadas: que el
   argumento sea una expresion que ya viva en un pseudo muerto, o mirar
   `integrate.c` para la condicion exacta con `-dR`.
2. **`_Storage` (1.156 B)**: ya tiene el tamano exacto. Quedan el intercambio
   r24/r25 y la cache de `size()`. Lo segundo pide `size_type curSize = size();`
   en `assign` — cabecera compartida, hay que volver a medir las 13 unidades.
3. **`bMath.hpp` `bCopy`/`operator/=`**: el objetivo emite los tres `stfs` en
   orden z,x,y y nosotros x,y,z. Toca `TrackCar` y probablemente medio proyecto.
   Fuera de mi territorio; sigue senalado desde la r36b.
4. **`SetupNextLoad` (zEAXSound)**: si a alguien le importan esos 4 B, la variante
   con la barrera sobre `srcBeg` los devuelve a cambio de 4 B en `_Storage`.
