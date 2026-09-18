# r36 · grupo `feng` — zFEng: **`trypromo.py zFEng` = DOL OK**

    .rodata  2632 / 2632   byte a byte
    .data     384 /  384   byte a byte
    .text   71460 / 71460  100,0000 %   ORDEN PERFECTO (0 descolocadas de 344)
    linkdelta  .text +0   resto IGUAL
    stripmap   test 0  ->  DOL IGUAL QUE LA BASE
    trypromo   DOL OK          (71.460 B que desbloquea)

---

## 0. Verificación del encargo, antes de tocar nada

Reproduce exacto:

    linkdelta.py zFEng   ->  .text +0   rodata-664 data-64
    stripmap.py  zFEng   ->  test -704   +keep -704   estrip 0   (todo dato ausente)
    textorder.py zFEng   ->  DESCOLOCADAS 0 de 344, SALTOS DE DELTA 0
    datadiff.py  zFEng   ->  .rodata 1984/2632 (falta 750, sobra 102)
                             .data    308/384  (falta 76)

---

## 1. El hallazgo de la ronda: **`-strip-unused-data` sólo toca SÍMBOLOS**

La receta de la r35 (`r35-att` §2) decía que el dato muerto hay que escribirlo
**con los nombres de `dtk`** para que `keep.lst` case. Eso vale, pero **no hace
falta**: lo que el estripado se lleva son símbolos, y **los bytes que no
pertenecen a ningún símbolo sobreviven intactos**.

Dos sondas, medidas en zFEng con `linkdelta` (base `rodata-664`):

| sonda | qué | resultado |
|---|---|---|
| **e1** | una función **nunca llamada** con un `float` y una cadena nuevos (16 B en el objeto) | `rodata-656`: **sobreviven 8 de 16** |
| **e2** | un `asm()` con 16 B crudos **sin `.globl`, sin `.type`, sin `.size`** | `rodata-648`: **sobreviven 16 de 16** |

De e1 sale además la aritmética exacta, que confirma el `size & ~7` de la r35 y
lo afina: **de cada símbolo muerto se van `size & ~7` bytes y quedan `size & 7`.**
El `float` de 4 B (`4 & ~7 == 0`) sobrevivió **entero**; la cadena de 13 B perdió
8. O sea:

- símbolo muerto de **8 B o más** → se va (total o parcialmente);
- símbolo muerto de **≤ 7 B** → sobrevive entero;
- **bytes sin símbolo** → sobreviven siempre.

**Consecuencia práctica**: el dato muerto se escribe a mano **sin nombre**, en el
sitio que le toca, y no hay que tocar `keep.lst` ni acertar con los nombres de
`dtk`. En zFEng son **679 B en 13 bloques**, todos anónimos, y el DOL sale
byte a byte. `genrodata.py` (que emite `.globl`/`.type`/`.size`) es justo lo que
**no** hay que usar para esto; el generador anónimo está descrito en §5.

Esto reabre las 19 unidades donde `stripmap` dice que el estripado pesa más que
el dato ausente: **no necesitan `keep.lst`, necesitan bytes anónimos**.

## 2. La otra palanca: **dónde** cae cada bloque

GCC 2.9 emite el pool de una función (flotantes **y** cadenas) pegado a esa
función en el flujo de ensamblador. Por lo tanto:

> **un `asm()` de ámbito de fichero colocado entre dos definiciones de función
> cae exactamente entre sus dos pools.**

Medido de paso con la sonda e1: los literales de la función sonda aterrizaron
detrás de `$LC189` («Code List») y **delante de las vtables**, o sea que el
bloque diferido de `finish_file` va siempre después de lo que se escribe a mano
al final del fichero.

Con eso, cada uno de los 13 huecos se ancla a su función:

| bytes | qué falta | anclado detrás de |
|---|---|---|
| 20 | dos dobles mágicos int→double | `FECodeListBox::Initialize` |
| 36 | 9 flotantes muertos | `FELerpColor` (última de FEKeyInterpLinear.cpp) |
| 56 | `lbl_803EA93C`: `"Scroll Left/Right/Up/Down"` | `FEKeyTypeSize` |
| 16 | 4 flotantes | `FEListBox::FEListBox` |
| 12 | 3 ceros | `FEListBox::SetNumRows` |
| **232** | `"[HEADER]" "Languages" "[LABEL]" … "Build number: 22" …` | `FEListBox::CompleteScroll` |
| **84** | **los nombres de ámbito de perfilado**: `"FEngine::Render()"`, `"Setup"`, `"Object traversal"`, `"SortObjects()"`, `"RenderObjectList()"` | `FEngine::ProcessMouseForPackage` |
| 37 | `"FEngMalloc"`, `"FEngMalloc[somefile]"` | `InitFEngMemoryPool` |
| 8 | `0`, `1e-6` | `FEObject::SetupMoveToTracks` |
| 8 | `0`, `1e-6` | `FEObject::SetPosition` |
| 44 | `"d:/mw/speed/indep/src/feng/FEPackage.cpp"` | `FEPackage::UpdateObject` |
| 12 | `lbl_803EAC3C` | `FETrackOffsets` |
| **114** | `"Init"`, `"Hide"`, `1/255`, `255.0f`, `"cmod" "cset" "cseta" "cadd" "wa" "sr" "sg" "sb" "r" "g" "b" "a" "w"` | `FETypeLib::Startup` |

Los 84 B de `FEngine::Render()` son el frente de los 104 marcadores `"TODO"` del
brief: aquí la cadena real venía **en orden** en el objeto extraído y se ha
escrito tal cual, sin tocar el `.text`.

## 3. Lo que NO se arregla con dato a mano: cuatro causas de fuente

### 3.1 `FETrackOffsets` y `FEKeyTypeSize`: `const` de fichero = enlace interno

Los dos son `bind=1` (**GLOBAL**) en el objeto extraído y caen **dentro** del
pool (+0x428 y +0x138). Los nuestros eran `const u32 X[]` de ámbito de fichero,
que en C++ tiene **enlace interno**: GCC lo difiere a `finish_file` y acaba **al
final** de la `.rodata`. Basta una declaración `extern` delante para que pasen a
enlace externo y se emitan **donde se definen**.

`FEKeyTypeSize` además estaba en el fichero equivocado: el DWARF lo declara en
`speed/indep/src/feng/FEKeyTypes.cpp` y su dirección lo pone entre el pool de
`FEKeyInterpLinear.cpp` y el de `FEListBox.cpp`, o sea **detrás de
`FEKeyTrack.cpp`** en la SourceList (orden alfabético). Se ha creado
`src/Speed/Indep/Src/FEng/FEKeyTypes.cpp` con él y con `FEKeyInterpValid`.

### 3.2 `eFrameCounter` no es de esta unidad

`FEPackage.cpp:295` lo **definía**. El objeto extraído lo tiene **UNDEF** y
`symbols.txt` lo sitúa en `.data:0x8041AC38`, que es de `Ecstasy.cpp`. Sustituido
por una declaración `extern`.

### 3.3 Las dos copias reinternadas y la cadena vacía

`"GAMECUBE"` y `"bad_alloc"` se internaban otra vez (28 B de más en el objeto,
16 tras el estripado): resueltas con el patrón `BWARE_PREFIX_*` que ya existía.

Y quedaba un `""` muerto de 1 B (4 con relleno) que el objetivo no tiene. Sale de
`Attrib::StringKey` (`attribuserinclude.h`, líneas 19 y 45), donde el patrón
estaba **cerrado con `#ifdef ZATTRIBSYS_HAND_POOL`**. Se ha generalizado con
`ATTRIB_EMPTY_STRING`, igual que `BWARE_PREFIX_*`; zFEng la apunta a
`pad_05_803EA7E8_rodata + 0x08`, que es el nulo de `"GAMECUBE"`.

Para encontrarlo hizo falta preprocesar la unidad y listar cada `""` con su
`fichero:línea`: el literal se emite en el flujo **al final de `FEngine.cpp`**,
que es donde GCC materializa la `inline` diferida, no donde está escrito.

### 3.4 El **orden del pool** dentro de una función

Esto no cambia ni un byte de tamaño, pero cambia **direcciones**, y sin
arreglarlo el DOL no casa aunque `.rodata` mida lo que debe.

En `FEInterpLinear(FEKeyTrack*, i32, void*)`:

    objetivo: 0.0f , 1.0f , 43300000_80000000 , 1e-5f
    nuestro : 0.0f , 43300000_80000000 , 1e-5f , 1.0f

GCC 2.9 mete las constantes en el pool **en el orden en que expande el RTL**, y
en un `if/else` expande **primero la rama `then`**. El objetivo tenía la rama del
`1.0f` como `then`. Invirtiendo la condición:

    - if (pPrevKey && tTime < pKey->tTime) { ...div... } else { t = 1.0f; }
    + if (!pPrevKey || tTime >= pKey->tTime) { t = 1.0f; } else { ...div... }

el pool sale en el orden del objetivo y el **`.text` sale byte a byte idéntico**
(comprobado contra el objeto anterior: 73.756 B iguales, sólo cambian los
destinos de 18 reubicaciones, que es justo lo que se quería). El relleno de
alineación de 8 del doble reproduce solo los 4+4 B de hueco del objetivo.

Se probaron dos formas, `!A || B` y `!(A && B)`: **las dos** dan `.text`
idéntico y `.rodata` 2632/2632. Se ha dejado la primera.

## 4. Ficheros tocados

    src/Speed/Indep/SourceLists/zFEng.cpp        ATTRIB_EMPTY_STRING, los 2 bloques de .data,
                                                 el #include de FEKeyTypes.cpp
    src/Speed/Indep/Src/FEng/FEKeyTypes.cpp      NUEVO  (FEKeyTypeSize + FEKeyInterpValid)
    src/Speed/Indep/Src/FEng/FECodeListBox.cpp   bloque de 20 B
    src/Speed/Indep/Src/FEng/FEKeyInterpLinear.cpp  bloque de 36 B + el orden del pool (§3.4)
    src/Speed/Indep/Src/FEng/FEListBox.cpp       bloques de 16, 12 y 232 B
    src/Speed/Indep/Src/FEng/FEngine.cpp         bloque de 84 B (perfilado)
    src/Speed/Indep/Src/FEng/FEngStandard.cpp    bloque de 37 B
    src/Speed/Indep/Src/FEng/FEObject.cpp        bloques de 8 y 8 B
    src/Speed/Indep/Src/FEng/FEPackage.cpp       bloque de 44 B + eFrameCounter extern
    src/Speed/Indep/Src/FEng/FEScript.cpp        FETrackOffsets extern + bloque de 12 B
    src/Speed/Indep/Src/FEng/FETypeLib.cpp       bloque de 114 B
    src/Speed/Indep/Src/FEng/FETypeNode.cpp      FEKeyTypeSize/FEKeyInterpValid fuera
    src/Speed/Indep/Src/Misc/attribuserinclude.h ATTRIB_EMPTY_STRING (inerte para los demás)

**AVISO**: `src/Speed/Indep/Src/FEng/FEKeyTypes.cpp` **es nuevo y está sin
seguir por git**. Otro agente ya se llevó de paso el resto de estos cambios en
los commits `15e29362` y `4ce1c497` de las 02:59/03:01, así que **HEAD incluye
hoy un `#include` de un fichero que no está en el repo**: quien commitee después
tiene que hacer `git add` de él.

### Radio de explosión: **cero, medido**

- Ningún otro `SourceList` incluye `src/Speed/Indep/Src/FEng/` (comprobado con
  `grep -rl`): las 11 `.cpp` tocadas son exclusivas de zFEng.
- `attribuserinclude.h` sólo gana una rama `#elif defined(ATTRIB_EMPTY_STRING)`,
  y **nadie más define esa macro** (`grep` en todo `src/`).
- Las cuatro SourceLists ya promocionadas (`zDebug`, `zDynamics`, `zMission`,
  `zOnline`) **recompiladas: md5 idéntico** antes y después.
- `undlist.py` → **0 símbolos por definir** en todo el proyecto.
- `build/GOWE69/main.dol` en disco sigue en `9619ba57c991…`.

## 5. Propuesta para `configure.py` (paquete aparte, sin aplicar)

Una línea, verificada por `trypromo.py` (que enlaza el proyecto entero
sustituyendo el objeto y compara el sha1 del DOL):

    configure.py:664
    -            Object(NonMatching, "Speed/Indep/SourceLists/zFEng.cpp"),
    +            Object(Matching,    "Speed/Indep/SourceLists/zFEng.cpp"),

`config/GOWE69/keep.lst` **no hace falta tocarlo**. Sus 6 entradas de `zFEng.o`
siguen ahí; después de promocionar, cuatro de ellas (`lbl_803EA93C`,
`lbl_803EAC3C`, `lbl_8041D078`, `lbl_8041D174`) quedarán **rancias** —esos bytes
ahora van sin nombre— y `keepchk.py` las marcará. No rompen el enlace: el
`trypromo` que da DOL OK se hace con el `keep.lst` de serie.

### `genrodata.py --anonimo` (añadido, compatible hacia atrás)

El modo de siempre de `genrodata.py` **no** sirve para esto: emite
`.globl`/`.type`/`.size`, y esos símbolos muertos los estripa el enlazador. El
bloque bueno no lleva **ni una etiqueta**, y cierra con `.previous` (r35 §2):

    asm(
        ".section .rodata\n"
        "  .byte 0x00, 0x00\n"          // hasta alinear a 4, si el rango no lo está
        "  .4byte 0x496E6974, 0x00000000\n"
        ".previous\n"
    );

Se ha añadido a `scripts/genrodata.py` la opción que los genera, más
`--seccion` para usarla también en `.data`:

    python scripts/genrodata.py zFEng 2E0 334 --anonimo
    python scripts/genrodata.py zFEng 134 180 --anonimo --seccion .data

El modo clásico (con nombres de `dtk`) queda **igual**, incluida su aserción de
límites; comprobado con `zAttribSys 5C 85`.

## 6. Vedas y avisos

- **VEDA ROTA (r35-att §5)**: «la función `inline` de orden no sirve para
  constantes numéricas». Sigue siendo cierta —una función no emitida no emite su
  pool de flotantes— pero **ya no importa**: las constantes numéricas MUERTAS se
  escriben como bytes anónimos. Sólo las **vivas** tienen que salir del
  compilador, y ésas ya salen.
- **VEDA ROTA (r35-att §5)**: «ampliar `keep.lst` con nuestros nombres no cierra
  una promoción». Cierto, pero la conclusión que se sacaba —que hacía falta
  acertar con los nombres de `dtk`— **no lo es**: no hace falta nombre ninguno.
- **`datadiff.py` resincroniza de forma aproximada** y reparte mal los bloques
  cuando faltan 700 B: la lista de «FALTA» de la primera pasada no es la lista de
  huecos reales. Lo que sí es exacto es cruzar las reubicaciones `.text→.rodata`
  de los dos objetos por su desplazamiento en `.text`: da un emparejamiento 1:1
  de cada `$LCnn` con su `lbl_` y, de ahí, el hueco exacto detrás de cada uno.
  Es lo que convirtió «faltan 750 B» en las 13 filas de la §2.
- **`measure.py` dice `343 funciones al 100%` de 344** y a la vez
  `71460/71460 B 100,0000%`. No es un fallo pendiente: el DOL sale byte a byte.
- **Otro agente commitea el árbol entero** (`git commit -a`, se ve en
  `15e29362`). Trabajar contando con que la copia de trabajo es tuya no vale;
  y un fichero NUEVO no entra en esos commits (ver el aviso de la §4).
