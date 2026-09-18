# Ronda 18 — grupo zEagl4Anim / zBWare / zPlatform / zSim / zAnim / zEcstasy

Línea base `base_r18_plat.json`: 438.012/462.744 B, **94,6554%**, 1.932 funciones.
Cierre `despues_r18_plat.json`: **idéntico**. `measure.py --cmp` -> `+0 B, +0
funciones, 0 unidades cambian`. **Cero bytes ganados y cero perdidos.**

Lo que sí ha salido: **dos funciones ya al 100% de objdiff que referenciaban
símbolos equivocados**, una de ellas con un **error de comportamiento real**, y
una trampa de medida nueva que ha estado dando falsos positivos a `audit.py`.

## 1. La auditoría: el estado real de las seis unidades

`audit.py` sobre las seis, dos pasadas separadas ~1 h:

| unidad | ok | falla (2ª pasada, estable) |
|---|---|---|
| zAnim | 315 | — |
| zSim | 401 | `Load__11NISActivity…` (`__builtin_new` / `__builtin_vec_new`) |
| zPlatform | 134 | — |
| zBWare | 238 | — |
| zEagl4Anim | 310 | `InitInternal__…Initializer` (**falso positivo demostrado**) |
| zEcstasy | 530 | `__static_initialization_and_destruction_0`, `epInitViews__Fv` |

### TRAMPA NUEVA Y MEDIDA: `audit.py` corre sobre el `build/` compartido

La **primera** pasada dio **8 fallos** que la segunda no reproduce:
`Load__11CAnimPlayerUiib` y `__19NISListenerActivity` (zAnim), cuatro `_.tmp_N.M`
más `__11NISActivity` (zSim), `DVDErrorTask__FPvi` (zPlatform), y
`eDisplayFrame__Fv` + `eMotionBlurEffect__FP5eView` (zEcstasy, 5.488 B entre las
dos). Ninguna de esas funciones se tocó entre las dos pasadas.

Causa: `audit.py` lee `build/GOWE69/src/<unidad>.o`, que es **compartido**, y en
esta ronda hay varios agentes compilando a la vez. Si otro agente reescribe el
objeto entre tu `build_direct.py` y tu `audit.py`, salen fallos fantasma.
**Regla: compilar y auditar sin hueco, y confirmar todo fallo con una segunda
pasada antes de perseguirlo.** Me costó ~40 min de investigación de tres fallos
inexistentes.

### Falso positivo real de `audit.py`: `gcc2_compiled.`

`InitInternal__Q29EAGL4Anim11InitializerUib` "falla" porque el objetivo referencia
`gcc2_compiled.` donde nosotros referenciamos
`Constructor__Q29EAGL4Anim8AnimBankPvPQ25EAGL413DynamicLoaderPCc`. **Comprobado
con `rodata.syms` sobre los dos objetos: los dos símbolos están en el offset 0 de
`.text`, con addend 0.** Es objdiff eligiendo el marcador de tamaño cero.
`audit.py` debería tratar `gcc2_compiled.` como alias del símbolo de offset 0.

## 2. Arreglado: `__InitRenderMode__Fv` (600 B) — bug de comportamiento

`src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp`. `audit.py` cazó
`PALefbxfbFOVscl` contra `PALefbxfbAspect`. Al desenredarlo salieron **dos**
diferencias, no una:

1. **El orden de las dos sentencias.** El objetivo liga r29 a `PALefbxfbFOVscl` y
   r30 a `PALefbxfbAspect`; nosotros al revés. El registro callee-saved lo recibe
   el símbolo de la **primera** sentencia, y el `stfs` que se emite primero es el
   de la **segunda**. Con `FOVscl` delante de `Aspect` en las dos parejas
   (la de `= 1.0f` y la del bloque PAL) el reparto casa.
2. **Los dos cocientes estaban intercambiados.** Con el orden ya corregido,
   `audit.py` pasó a señalar `efbHcrt` contra `xfbHcrt`. El objetivo carga
   `efbHcrt` primero, y de ahí sale `Aspect = xfbHcrt/efbHcrt` y
   `FOVscl = efbHcrt/xfbHcrt` — **lo contrario de lo que teníamos**. Es decir: en
   PAL 50 Hz el juego venía escalando FOV y aspecto al revés.

Queda `ok 600 B, 13 ramas, 77 relocs, 10 literales`.

## 3. Arreglado: `ServiceWorldEffects__13EmitterSystem` (340 B)

`src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp:218`. Llamábamos a la sobrecarga
`IsCloseEnough(const bVector3*, …)` con un `reinterpret_cast<bVector3*>`; el
objetivo llama a la de `bVector4*`. Cambiado el cast. Queda `ok 340 B, 10 ramas,
27 relocs, 4 literales`.

## 4. Frente estructural pendiente: `__builtin_new` contra `__builtin_vec_new`

`epInitViews__Fv` (zEcstasy, 4 sitios) y `Load__11NISActivity…` (zSim, 1 sitio)
llaman a `__builtin_vec_new` donde el original llama a `__builtin_new`. **No es un
error de esas funciones**: es la consecuencia del `operator new` inline de
`bWare.hpp:136`, que reenvía a `operator new[]` para que los ~623 sitios del
juego emitan `__builtin_vec_new`. El original tiene además **19 sitios** que
emiten `__builtin_new`, o sea unidades de traducción donde ese inline no era
visible. Con el build unitario (SourceLists) y la guarda
`BWARE_NO_INLINE_GLOBAL_NEW` puesta al principio de `zMisc.cpp`, **no hay forma
por fichero de apagarlo**: cuando `EcstasyE.cpp` se compila, `bWare.hpp` ya
entró con su guarda de inclusión.

No he probado nada aquí. Cuesta 0 B hoy (las dos funciones ya están al 100% de
objdiff) pero **sí cambia el DOL enlazado**. Es un frente propio, no de este
grupo: hay que encontrar qué 19 sitios son y qué cabecera falta en ellos.

## 5. Ensayos numerados (todos revertidos)

    c1  RawStateChan.h: factorizar `base = &mDecodeData[mNumFields]` en GetKeyData
        -> EvalState 97,763 -> 58,921 Y **FindTime cae de 100% a 85,845%**.
        Descartado. Y con valor: prueba que la forma actual de GetKeyData ES la
        del original.
    c2  FnStatelessF3/Q: quitar la local `times` y usar `X->mTimes[...]`
        -> F3 99,346 -> 98,120; Q 98,811 -> 97,959. Aparecen DOS `lwz` de recarga
        de `mTimes` dentro de los bucles y un `mr r8,r11` de más. Descartado.
    c3  FnStatelessQ: quitar el `static_cast<unsigned short>` de
        `floorKey >= (unsigned short)(mNumKeys - 1)` -> 98,811 -> 97,403.
        El cast MEJORA; sin él el `clrlwi` se mueve delante del `subi` en vez de
        detrás. Descartado (pero el objetivo no tiene NINGÚN `clrlwi` ahí).
    c4  FnStatelessF3: declarar `times` antes del `if (mPrevKey)` -> **neutro**.
    c5  FnStatelessQ: declarar `times` al principio del `else` externo y usar
        `times[0]` -> **neutro**. c4+c5 juntos prueban que el SITIO de la
        declaración no es la palanca.
    c12 TextureInfoPlat.cpp: renombrar r/b en `Convert32To16` según el DWARF
        -> UnlockPalette 92,674 -> 92,326. Descartado.
    c13 Reordenar los términos del `|` según el orden de operandos del `or` del
        objetivo -> 92,674 -> 89,419. Descartado. **El orden de operandos de un
        `or` NO es señal del orden en la fuente: GCC canonicaliza conmutativos.**
    c14 c12+c13 juntos -> 89,070. El peor. Descartado.
    c15 eagl4supportdlopen.cpp: `case A ... B` -> dos `case` sueltos
        -> **neutro** (90,000%). Descartado.
    c16 EcstasyEx.cpp: calcular `grid_pointY` antes que `multiple = i / 2`
        -> GenerateHorizonFogDisplayList 98,995 -> 91,482. Descartado.

Se queda: **ninguno**. Los dos cambios que quedan en el árbol son los de las
secciones 2 y 3, que no mueven bytes pero corrigen código.

## 6. `DynamicLoader::Initialize` (2.352 B, 90%) — el árbol de switch, resuelto y bloqueado

La hipótesis del brief («hay un `if` DELANTE del switch») es **falsa**. Con las
direcciones del objetivo bien calculadas (había que descontar las filas INSERT
del lado izquierdo antes de sumar 4 por instrucción) el árbol se lee entero:

    04e4c cmpwi  r0,8      ; beq 0x4ff0        raíz = case 8
    04e54 cmplwi r0,8      ; ble 0x4e78        subárbol bajo
    04e5c cmplw  r0,0x70000006 ; bgt 0x4ff0    cota alta del rango
    04e64 cmplw  r0,0x70000005 ; bge 0x4ff0    cota baja del rango
    04e6c cmpwi  r0,9      ; beq 0x4fc4        case 9
    04e74 b      0x4ff0
    04e78 cmpwi  r0,3      ; beq 0x4ee4        case 3
    04e80 cmplwi r0,3      ; bgt 0x4ff0
    04e88 cmplwi r0,1      ; ble 0x4ff0
    04e90 (caída)                              case 2

Eso es **exactamente** `balance_case_nodes` sobre el conjunto
`{2, 3, 8, 9, [0x70000005..0x70000006]}` — que es **el conjunto que ya tiene
nuestra fuente**. Mediana 8 -> raíz; izquierda {2,3} con raíz 3; derecha
{9, rango} con raíz el rango, emitido como `bgt default` + `bge case`. No falta
ni sobra ningún `case`, y no hay ningún `if` delante.

Lo que falla es que **GCC borra las dos comparaciones del rango**. Reproducido en
un `.c` suelto con el compilador real (2 s por prueba, `ngccc.exe` de
`build/compilers/ProDG/3.9.3`, con los cflags exactos de la unidad):

    v3  case 0x70000005 ... 0x70000006: break;   -> podado
    v4  case 0x70000005 ... 0x70000006: fq();break; -> **reproduce el objetivo**
                                          (lis 0x7000 / ori 6 / cmplw, y ori 5)
    w1  el rango cae en `default:`              -> podado
    w2  `default:` cae en el rango              -> podado
    w3  el rango con `continue;` y sin default  -> podado
    w4  el rango primero en el fuente           -> podado
    x1  `default: continue;` + rango `break;`   -> podado
    x2  rango `continue;` + `default: break;`   -> podado
    y1/y2 con los cflags reales de zEagl4Anim, con y sin
        `-fexpensive-optimizations`             -> podado

Es decir: **la única forma encontrada que conserva el nodo de rango es que su
cuerpo emita código**, y en el objetivo ese cuerpo salta a 0x4ff0, el mismo sitio
que el `default`. En todas mis variantes con cuerpo vacío GCC genera la
comparación (el `lis 0x7000` sobrevive izado del bucle en el `.s`) y luego la
BORRA, porque la etiqueta del case y la del default acaban siendo la misma.

**Lo que no he probado y es el siguiente paso**: que el cuerpo del case
`SHT_LOPROC+5 ... +6` del original haga algo que hoy no está en nuestra fuente y
que, tras optimizar, converja en 0x4ff0 — p.ej. una asignación que GCC elimine
por muerta, o un `goto` a una etiqueta propia. Ahí es donde razonablemente vive
parte de los `faltan 23`.

## 7. La familia `mr r11, r8` de zEagl4Anim (4.232 B) — acotada, sin cerrar

`FnStatelessF3::EvalSQT` (1.468 B, 99,346%), `FnStatelessQ::EvalSQT` (1.480 B,
98,811%) y `FnPoseAnim::EvalPose` (1.284 B, 98,816%) tienen **el mismo** defecto,
y es el único que le queda a F3:

    objetivo:  ... subi r7,r9,1 ; mr r11,r8 ; add r9,r7,r7 ; lhzx r0,r9,r11
    nuestro:   ... subi r7,r9,1 ;           ; add r9,r7,r7 ; lhzx r0,r9,r8

Dos copias por función (una en el punto de unión tras `if (mPrevKey)`, otra en el
precabezal del bucle descendente). El objetivo mete `mTimes` en r11 y reutiliza
r8 para `mNumKeys - 2`; nosotros coalescemos y usamos r11 para `mNumKeys - 2`.

**El DWARF dice que la local `times` NO existe en el original.** Los tres bloques
anónimos correspondientes listan sólo `int timeIndex; // r7` (F3 y Q) y, en
`FnPoseAnim::EvalPose`, ninguna `times` entre las doce locales de primer nivel.
Pero quitarla (c2) hace que GCC recargue `mTimes` dentro de los bucles. Veda:
**la ausencia de `times` está confirmada por el DWARF y sin embargo ninguna forma
sin ella compila mejor**; el mecanismo que falta es el que hace que GCC mantenga
vivo el CSE de `X->mTimes` sin local.

## 8. `EmitterSystem::Render` (696 B) — el hueco de pila localizado

El marco del objetivo es **8 B mayor** (0x128 contra 0x120) con **los mismos**
registros salvados (r14..r31 en los dos), y usa una ranura en `0xc8(r1)`:

    objetivo:  addi r0,r23,0x37c ; stw r0,0xc8(r1) ; lwz r0,0xc8(r1) ; cmpw r25,r0
    nuestro:   addi r14,r23,0x37c ;                                  ; cmpw r25,r14

Es la firma exacta del brief: **un pseudo que no consigue registro y recibe hueco
de pila**. Por el mapa de rangos del DWARF, el `addi`+`stw` caen en la expansión
de `bTList<EmitterGroup>::GetHead()` (0x8011294C→0x80112968) y el `lwz` en la de
`EndOfList()` (0x80112968). El resto de la función es un desplazamiento de un
registro (nuestros r16..r19 contra r15..r18 del objetivo) causado por esa
diferencia. Las locales del DWARF coinciden una a una con nuestra fuente, así que
lo que sobra es **una presión de registro de más en el original**, no una
variable que falte. No probado: nada.

## 9. `_bOutput` (zBWare, 5.180 B) — no falta ni sobra NADA

Medido directamente sobre las listas de instrucciones: **1.296 contra 1.296**,
mismo tamaño (5.180 B) y **delta de opcodes vacío**. `triage` dice
`faltan 8, sobran 3` porque normaliza a patrones, no a mnemónicos.

Lo único estructural son **3 filas**: el objetivo emite `lis rN,0x7ff0` antes de
`li r27,0` (`stringOut = nullptr`) y nosotros después. Todo lo demás son ~20
diferencias de registro repartidas en unas seis zonas de r8/r9/r11/r0.

El orden de declaración de las locales de primer nivel **ya coincide exactamente
con el DWARF** (flags, state, charType, outLen, width, precision, ch, stringOut,
stringLength, prefix[2], prefixSz, padding, cvtbuf[64]) y los registros grandes
también (flags r21, width r15, precision r22, stringOut r27, stringLength r26,
padding r30). Lo que no casa son los temporales. Pista concreta y sin usar: el
DWARF da `char *q; // r11` para el bloque 0x8005F324→0x8005F388 (el bucle de
redondeo del flotante) y **nosotros ponemos `q` en r9**.

## 10. `UnlockPalette` (172 B) — el DWARF contra la emisión

`Convert32To16` inline. El DWARF lista `a//r0, b//r9, g//r10, r//r8` en ese orden
de declaración, y el objetivo emite en orden de desplazamiento descendente
(`>>24`, `(>>16)&0xff`, `(>>8)&0xff`, `&0xff`) asignando `r0, r8, r10, r9`.
Las dos cosas juntas obligan a `b = entry & 0xFF` y `r = (entry >> 16) & 0xFF`
—al revés que nuestra fuente— **pero entonces el orden de declaración y el de
emisión se contradicen**, y las tres variantes que probé (c12, c13, c14) empeoran.
El objetivo mete `entry` en r9 y **reutiliza r9 para `b`** (una copia menos);
nosotros dejamos `entry` en r10 y gastamos r11. No probado: forzar la muerte de
`entry` antes del último uso.

## 11. Qué NO he probado

- El frente `__builtin_new` (sección 4): ninguna variante.
- `eProgressiveScan_EURGB60DialogBox` (1.848 B) y `UpdatePlatInfo` (2.044 B):
  sólo leídos en el triaje (94 y 64 diferencias de registro). Cero ensayos.
- `UpdateParticles` (1.820 B, 99,956%, 4 registros): **lo tiene otro agente en
  vuelo** (`EmitterSystem.cpp` modificado a las 14:46 por alguien que no soy yo,
  con `pangle = 257.0f * …`). No lo toqué a propósito.
- `epCalculateLocalDirectionalPOS16` (2.072 B): comparé la lista de locales del
  DWARF con la fuente y **coinciden una a una, incluida `double zero`**. Cero
  ensayos sobre el reorden.
- `eProject` (268 B) y `FindMatchTime` (720 B): sólo volcados.
- `ActualReadJoystickData` (1.588 B): volcado leído; el objetivo tiene un
  `mr r9,r0` + `andi. r11,r9,0x8000` que no tenemos, y tres `add rD,r29,r27`
  donde nosotros ponemos `add rD,r27,r29` (orden de operandos). Cero ensayos.

## 12. Herramientas dejadas en el scratchpad

`plat18_fns.py` (porcentaje por función de una unidad), `plat18_side.py`
(objetivo|nuestro lado a lado, con dirección real del objetivo descontando las
filas INSERT, y `--relocs`), `plat18_it.sh` (compila + lista near-miss, ~35 s).
Nombres únicos, como pide el brief.

## 13. Aviso de concurrencia

Otro agente tiene en vuelo `src/Speed/Indep/Libs/Support/Utility/UTLVector.h`
(cabecera compartida) y `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp`
(`pangle = 257.0f * static_cast<uint32>(...)`, dentro de `UpdateParticles`, que
es de zEcstasy). Efecto medido en mis unidades: el objeto de zEcstasy cambia
--`frozen.py chk` lo caza-- **sin mover ni un byte ni un porcentaje**
(`measure --cmp` +0 B, lista de near-miss idéntica). Congelado de nuevo al
estado verificado. Si `chk` vuelve a saltar en zEcstasy, contrastarlo con
`measure --cmp` antes de tratarlo como regresión.
