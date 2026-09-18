# Ronda 46 --- territorio `game`

`zAI` + `zPhysicsBehaviors` + `zPlatform` + `zFe2` + `zFeOverlay`.
Base: 7.664 B en 6 funciones. **Cerrado: 384 B en 1 funcion, y `zFe2` queda a CERO
funciones con codigo distinto.**

| unidad | funcion | antes | despues |
|---|---|---|---|
| `zFe2` | `__12IconScrollerPCcN31f` | 384 B, 97,7708 %, 11 filas | **100,0 %, 0 filas** |
| `zAI` | `UpdateAllAvoidables__11AIAvoidablef` | 2.908 B, 99,6011 % | igual (veda ajena vigente) |
| `zPhysicsBehaviors` | `Add__6RBGrid...` | 1.488 B, 99,3011 % | igual |
| `zPhysicsBehaviors` | `UpdateLoaded__Q217SuspensionTraffic4Tireffff` | 856 B, 97,2103 % | igual |
| `zPlatform` | `ActualReadJoystickData__Fv` | 1.588 B, 99,2443 %, 19 filas | igual (5 formas nuevas medidas) |
| `zFeOverlay` | `SetScreenNames__13CustomizeMain` | 440 B, 91,7273 %, 23 filas | igual (veda medida) |

---

## 1. CERRADA: `__12IconScrollerPCcN31f` --- 384 B

```
# __12IconScrollerPCcN31f  target=100.0%  ours=100.0%  size=384/384
```

`fncmp` de `zFe2`: **0 de 1307 funciones con el codigo distinto, 0 B**.

### La palanca: sched1 parte los stores en DOS GRUPOS por `INSN_REG_WEIGHT`

Esta es la aportacion util de la ronda y **no esta en el brief**. La veda de la
r36f decia «la fuente ya es la buena y lo que reordena es el planificador», y
tenia razon en la primera mitad y no en la segunda: el planificador es
predecible y **la fuente lo controla**.

`rank_for_schedule` de GCC 2.9 ordena la lista de listos por, en este orden:

1. `INSN_PRIORITY` (camino de dependencias mas largo hasta el final del bloque);
2. **`INSN_REG_WEIGHT`** = (registros que NACEN) − (registros que MUEREN, o sea
   las notas `REG_DEAD`/`REG_UNUSED`) --- **gana el MENOR**; este nivel solo
   existe antes de reload, es decir **solo en sched1**;
3. ... y al final `INSN_LUID`, que es **el orden de la fuente**.

En una tirada de N escrituras de miembros que leen todas el MISMO registro
constante (aqui `f0` = 0.0f, `f12` = 9.0f, `r0` = 4, `r9`, `r11`, `r10`), todos
los stores empatan en prioridad, asi que manda el peso:

- el store que **mata** el registro de su valor pesa **−1**;
- los demas pesan **0**;
- la nota `REG_DEAD` de un registro compartido esta en **el ultimo uso EN LA
  FUENTE**.

Resultado: **todos los stores de peso −1 salen primero, en orden de fuente, y
detras todos los de peso 0, tambien en orden de fuente.** Los dos grupos se
intercalan con los del constructor de `AnimateCubic`, que ya venian bien.

### Como se aplico

El objetivo emite `...0xd4, 0xf8, 0xfc, 0x100, 0x114, 0x118 | 0xd0, 0xdc, 0xe4,
0xe8, 0xec, 0xf4, 0xe0...`; nosotros emitiamos `0xd4, 0xfc, 0x100, 0x114, 0x118,
0xf4 | 0xd0, 0xdc, 0xe4, 0xf8, 0xe8, 0xec, 0xe0`. Quitando `0xf4` y `0xf8` las dos
secuencias son **identicas**: solo habia que meter `fCurFadeTime` (0xF8) en el
grupo de peso −1 y dejar `fCurrentAddPos` (0xF4) en el de peso 0, y ademas
colocarlo antes de `fMaxFadeTime` dentro de su grupo. Eso se consigue **solo
moviendo sentencias**, sin `asm`, sin barreras y sin tocar cabeceras:

```c
    AlignmentToSelected = eSA_MIDDLE;
    iCurSelectedIndex = 1;
    fHeight = 0.0f;
    iNumBookEnds = 4;
    fXCenter = 0.0f;
    fYCenter = 0.0f;
    fCurrentAddPos = 0.0f;
    fCurFadeTime = 0.0f;      /* ULTIMO 0.0f -> se lleva el REG_DEAD de f0 */
    fMaxFadeTime = 9.0f;
    bAllowColorAnim = true;
    IdleColor = 0xFFFFFFFF;
    FadeColor = 0x00FFFFFF;
    fWidth = width;
```

### Las tres medidas (28 s cada una, unidad real)

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base (orden anterior) | 384 B | 97,770836 % | 11 |
| **V1** intercambiar `fCurFadeTime` y `fCurrentAddPos` (las dos valen 0.0f) | 384 B | 97,770836 % | 11 |
| **V2** solo `fCurFadeTime` al final de los 0.0f | 384 B | 97,802086 % | **8** |
| **V3** V2 + `fMaxFadeTime`/`bAllowColorAnim`/`IdleColor`/`FadeColor` detras | 384 B | **100,0 %** | **0** |

V1 es la medida que valida el modelo: con el mismo numero de filas, los dos
stores **intercambian su hueco**, o sea que el hueco lo decide la POSICION en la
fuente y no el miembro. V2 confirma la parte del `REG_DEAD`: al pasar
`fCurFadeTime` a ser el ultimo 0.0f, la cola `e4, e8, ec, f4` sale ya exacta.

La veda anterior queda sustituida por el hallazgo en el comentario de
`src/Speed/Indep/Src/Frontend/MenuScreens/Common/feIconScrollerMenu.cpp`, con un
aviso de no reordenar esas 13 sentencias.

---

## 2. `SetScreenNames__13CustomizeMain` (440 B) --- veda medida, no es reorden

Diagnostico firme y NUEVO (el brief la mandaba a `triaje.py` por «estructural»;
no lo es):

- **el tamano es exacto**, 440/440, y el **multiconjunto de instrucciones es
  identico**: 8 `lis $LC@ha`, 8 `addi @l`, 8 `lis glob@ha`, `lis`+`addi` de
  `gCarCustomizeManager` y 8 `stw`. Solo cambia el plan;
- **el orden de la fuente ya es el del original**. El mapa de lineas del
  objetivo (`0x803B9584`, tamano `0x1B8`) da: 2866 la funcion, 2867 el `if`,
  2869..2879 las once asignaciones de backroom (que casan **al 100 %**),
  2883..2890 las ocho del `else`, 2891 el `if (IsCareerMode())`, 2892/2894 el
  `ShoppingCart`, 2895/2896 Hud y Spoiler. Coincide sentencia a sentencia con la
  nuestra;
- la diferencia real: **el objetivo COMPLETA la primera asignacion antes de
  emitir los otros siete `lis`** --- `lis r9,$LC@ha; lis r3,SubPkg@ha; addi r9;
  (lis r11 de la 2a); stw r9,SubPkg@l(r3)` --- y por eso le bastan **SIETE**
  preservados (r24..r30) donde nosotros gastamos **OCHO** (r23..r30). La pila y
  el `stmw r18` son iguales en los dos.

Con el modelo del punto 1: los ocho stores pesan **−2** (mueren el valor y la
direccion) y las ocho cadenas son identicas, asi que **ninguna permutacion puede
adelantar una sola**. Medido:

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base | 440 B | 91,72727 % | 23 |
| primera asignacion (`g_pCustomizeSubPkg`) movida al FINAL del grupo | 440 B | 91,72727 % | **23** |

y en el volcado se ve que lo unico que cambia es el orden de los ocho `lis $LC`
entre si: la forma 8/8/8/8 es invariante. Queda escrito en el comentario de
`CarCustomize.cpp`. **Lo que hace falta es una causa que suba la PRIORIDAD de ese
primer store** (una dependencia de memoria posterior a el), no un reorden; la
barrera `asm("")` ya esta vedada de la r36d (subia a 94,5 % y no cerraba).

---

## 3. `ActualReadJoystickData__Fv` (1.588 B) --- 5 formas nuevas, todas negativas

Base 1.580 B (8 B mas CORTO que el objetivo), 99,24433 %, **19 filas**. Faltan
exactamente dos instrucciones, `mr r9,r0` y `andi. r11,r9,0x8000`, que el
objetivo pone alrededor del `stb r0,0x4(r29)` de stickX (lineas 306/307 del
original). Las otras 17 filas son permutaciones de registro (r18/r19, r8/r10/r11
y el orden de operandos de dos `add`/`sthx`), y **todas caen solas si aparece el
par**.

Las cinco formas (8 s cada una, unidad real):

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base | 1.580 B | 99,24433 % | 19 |
| F1 `v` reutilizada para el bloque de stickY | 1.580 B | 98,90428 % | 38 |
| F2 `int data = v;` al abrir el bloque de stickY | 1.580 B | 99,24433 % | **19 = base** |
| F3 `v = v & 0x8000;` + `int data = v;` | 1.576 B | 98,476074 % | 23 |
| F4 `v = v & 0x8000;` + `v` reutilizada | 1.576 B | 98,16121 % | 41 |
| F5 el `if (v & 0x8000)` repetido tras el bloque de stickY | 1.568 B | 93,12343 % | 98 |

### El dato nuevo que cambia el diagnostico de la r36f

La r36f dedujo que la sentencia que falta es «`data = data & 0x8000;` cuyo
resultado alimenta los cinco `mr rX,r11`». **Esos cinco `mr` no son de la
fuente.** Compilando la misma unidad con `-fno-schedule-insns
-fno-schedule-insns2` (misma linea de `build.ninja` por lo demas) los cinco
desaparecen y el bloque sale limpio: son **copias que mete el reparto DESPUES de
planificar**. Volcado guardado en `scratchpad/r46_game/zPlatform_nosched.s`
(`ActualReadJoystickData__Fv`, lineas 92030-92080) frente a
`scratchpad/r46_game/zPlatform_ours.s`.

O sea que el par que falta puede ser tambien una **rematerializacion del
reparto** y no una sentencia. Con eso, la veda de «formas del `if`» esta
agotada (r36: 5 formas; r36b: 5; r36f: 4; r46: 5 = 19 formas medidas) y el
frente vivo es la **presion en ese punto**, no la forma del bloque.

El DWARF acota lo que se puede escribir: el bloque `if (pad_state == 0)` del
original es `0x8026C6B4..0x8026C878` y tiene **una sola local, `short data`, en
r0** (`symbols/mw_dwarfdump.nothpp`, `ActualReadJoystickData`). La r36f ya midio
que escribirlo con una sola `short data` da 1.564 B y 96,383 %.

---

## 4. `UpdateAllAvoidables` (2.908 B) + `RBGrid::Add` (1.488 B) --- confirmado el mismo frente

Confirmo con `fndiff` que las dos son **el mismo bucle** de
`SAP::Grid<T>::Axis::Node::Node` (`SAP.h:26-29`) inlineado cuatro veces:
`lwz r9,4(r9)` contra `lwz r9,4(r6)` y `b`→cabecera compartida contra
`blt`→cuerpo, x4 en `UpdateAllAvoidables`. Son 4.396 B.

**No he abierto un barrido nuevo**: la r36e dejo el mecanismo con numero de
linea (`jump.c:1760` invierte `ge→FIN` + `b LBODY` en `lt→LBODY` y borra el `b`,
antes de que la pasada con `cross_jump` pueda fundir la cola) y 31 conjuntos de
banderas + 19 formas medidas. Lo unico que aporto es cerrar la puerta que
quedaba entreabierta:

- la unica salida del guardia de `jump.c:1760` es que `invert_jump` FALLE, y
  `invert_jump` solo falla si `can_reverse_comparison_p` devuelve 0; ese
  predicado devuelve **1 incondicionalmente cuando `flag_fast_math` esta puesto**.
  Y `-ffast-math` esta en los cflags de las dos unidades (`build.ninja`) y la
  r36e ya demostro con el `cror` del binario que el original tambien lo llevaba.
  **Asi que no hay forma de fuente, con esas banderas, que salve el `b LBODY`**;
- `SAP.h` solo lo incluyen tres ficheros (`zAI.cpp`, `AIAvoidable.h`,
  `RigidBody.h`), asi que el frente es exactamente estas dos unidades. Sigue
  siendo el 7,2 % de lo que falta al proyecto y sigue necesitando una palanca de
  compilador, no de fuente.

## 5. `UpdateLoaded__Q217SuspensionTraffic4Tireffff` (856 B) --- sin ensayo nuevo

Reconfirmado el diagnostico de la r41 con `fndiff`: al objetivo le sobra un
preservado (`stmw r30`/`lmw r30`, marco 0x30 contra 0x28) porque mantiene
`lbl_803FB6B8@ha` --- la pagina del literal 1.0f --- **en r30** y lo usa en dos
`lfs` (`lfs f0,lbl@l(r30)`); nosotros rematerializamos `lis r9,$LC917@ha` dos
veces (filas 100/101 y 119/120). No hay objeto de C igual a un `@ha`: un float
nombrable va a FPR y un puntero da la direccion COMPLETA (`lfs f0,0(rN)`, sin
desplazamiento), que no es lo que emite el objetivo. La r41 midio tres formas;
no anado ninguna porque la palanca esta en que `cse2` deshace el pseudo que crea
PRE, y eso no se toca desde la fuente.

---

## 6. Lo que dejo puesto

Tres ficheros, todos dentro de mi territorio:

| fichero | que |
|---|---|
| `src/Speed/Indep/Src/Frontend/MenuScreens/Common/feIconScrollerMenu.cpp` | **el cierre** (orden de las 13 asignaciones) + el comentario con la palanca |
| `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp` | **solo comentario**: la veda medida de `SetScreenNames` |
| `src/Speed/GameCube/Src/JoyE.cpp` | **solo comentario**: las 5 formas nuevas y el hallazgo de `-fno-schedule-insns` |

Los dos ficheros de comentario se recompilaron y se comprobo que la funcion no
se mueve (`SetScreenNames` 23 filas, `ActualReadJoystickData` 19 filas). No he
tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, `SAP.h`, ninguna cabecera
compartida ni ningun bloque `#if defined(__ANDROID__)`. Sin commits.

### Verificacion

```
python scripts/build_direct.py zAI zPhysicsBehaviors zPlatform zFe2 zFeOverlay   -> 5 ok, 0 fallidas
fncmp zAI                 1 de 1030 funciones distintas -- 2908 B   (= base)
fncmp zPhysicsBehaviors   2 de 1120 funciones distintas -- 2344 B   (= base)
fncmp zPlatform           1 de  136 funciones distintas -- 1588 B   (= base)
fncmp zFe2                0 de 1307 funciones distintas --    0 B   (base: 1 / 384 B)
fndiff zFeOverlay SetScreenNames__13CustomizeMain   91,72727 %  440/440  (= base)
fndiff zFe2 __12IconScrollerPCcN31f                 100,0 %     384/384  CERRADA
python scripts/lcfix.py --check   -> todas las entradas @lc estan al dia
git diff --check                  -> limpio; feIconScrollerMenu.cpp sigue CRLF entero (802/802)
```

**Ninguna funcion empeora en ninguna de las cinco unidades.**

---

## 7. Propuestas fuera de mi territorio

1. **La palanca de los dos grupos de peso vale para cualquier constructor o
   inicializador con una tirada de escrituras de miembros**, que es justo la
   forma de las funciones mas gordas que quedan abiertas: el
   `__static_initialization_and_destruction_0` de `zCamera` (3.604 B, 97,11 %) y
   el de `zAI` (9.380 B). Receta, sin `asm` y sin tocar cabeceras:
   - lista los stores del objetivo y los nuestros; si el multiconjunto es el
     mismo y solo cambia el orden, esto aplica;
   - marca cada store con su peso: **−1 si es el ULTIMO uso, en orden de fuente,
     del registro que guarda** (el 0.0f compartido, un `li rN,1` reutilizado,
     etc.), **0 si no**;
   - el objetivo emite primero todos los de peso −1 en orden de fuente y luego
     todos los de peso 0 en orden de fuente: reordena las sentencias hasta que
     las dos listas coincidan. En `IconScroller` fueron dos movimientos y cerro.
2. **`fncmp.py` y `triaje.py` no ven `zFeOverlay` ni `zOnline`.** Su codigo no
   esta en `.text` sino en una seccion **`.over`** (comprobado: son los dos
   unicos objetos extraidos con esa seccion). `fncmp.py` aborta con «alguno de
   los dos objetos no tiene .text» y `triaje.py` contesta «0 funciones, 0 B», o
   sea que esas dos unidades son **invisibles al triaje que reparte el trabajo**;
   solo `fndiff.py` funciona ahi. Ojo al arreglarlo: cambiar el nombre de la
   seccion NO basta --- lo probe y da **404 de 467 funciones distintas**, todas
   falsos positivos (comprobado con `fndiff`: `DrawPartName__18FEShoppingCartItem`
   y `RefreshBonusCarList__13UIQRCarSelect` estan al **100 %**), porque la
   normalizacion de saltos internos de `fncmp` esta atada al simbolo de seccion
   `.text`. Hay que generalizar las dos cosas a la vez.
3. **`SAP.h` (4.396 B en `zAI` + `zPhysicsBehaviors`)**: la puerta que la r36e
   dejaba abierta --- «como sobrevive el `b LBODY` a la primera pasada de jump»
   --- esta cerrada: el unico escape del guardia de `jump.c:1760` es que
   `invert_jump` falle, y `can_reverse_comparison_p` devuelve 1 siempre con
   `flag_fast_math`, que estas unidades llevan. Con estos cflags no hay forma de
   fuente. Si alguien quiere los 4.396 B, la unica hipotesis viva sigue siendo
   una bandera o revision de compilador distinta **para esos dos objetos**, y eso
   es trabajo de `configure.py`, no de fuente.

## 8. Utiles que dejo en el scratchpad (no en el arbol)

`scratchpad/r46_game/`:

| fichero | que |
|---|---|
| `probe.py` | compila una unidad y saca `tamano / fuzzy / filas distintas` de una funcion (8-28 s) |
| `asmout.py` | compila cualquier fuente con sus cflags reales de `build.ninja` a `-S`, con flags extra |
| `sweep_joy.py` | barre formas del bloque stickX/stickY de `JoyE.cpp` y restaura la fuente al acabar |
| `overscan.py` | lista los objetos extraidos que no tienen `.text` |
| `zPlatform_ours.s` / `zPlatform_nosched.s` | nuestro asm con y sin planificador, para la prueba del punto 3 |
