# r59 — zEcstasy: la `.data` queda EN ORDEN (317 símbolos fuera de sitio → 1) y el DOL cae de 14.323 a 10.658 B

Agente `ecs`. Unidad en propiedad: **zEcstasy**. **17 ficheros tocados**, todos de
propiedad exclusiva de la unidad (medido: cada uno aparece en **UNA** SourceList y en
ninguna regla de `build.ninja`). **Ninguna cabecera**, ningún `config/`, ningún
`keep.lst`, ningún `splits.txt`, ningún commit, ninguna sonda en `scripts/`.

**Resumen en cinco líneas.**

1. El renglón del encargo está **hecho**: el bloque `asm` de `.data` se ha partido en
   **57 fragmentos** repartidos por los `.cpp` de `Ecstasy/`, y con ellos se han movido
   ~60 definiciones de variable al fichero y al punto donde el objetivo las tiene.
   `desplaza`: **317 de 325 símbolos de `.data` fuera de sitio → 1**.
2. La palanca no era el troceado sino **una regla de emisión que había que medir**:
   `assemble_asm` (`varasm.c:976`) **no toca el `in_section` de GCC**, así que un `asm()`
   de fichero sólo es seguro **pegado detrás de una variable de `.data`** o **acabando en
   `.section ".text"`**. Con la forma equivocada la SIGUIENTE función cae en `.data`.
3. **`.bss` tiene DOS COLAS** —los estáticos locales salen en el punto del parseo, los
   globales de fichero se drenan enteros en `finish_file`—, o sea que un `asm(".section
   .bss")` **nunca** puede intercalarse entre globales. Los siete huecos ya no son `asm`:
   son **arrays de C++**. Eso quitó 104 B de desplazamiento a toda la `.bss`.
4. **Los símbolos que el objetivo no tiene salen GRATIS si nadie los referencia**:
   `-strip-unused-data` se los lleva enteros y **su posición no afecta al enlace**.
   Medido moviendo `HeadlightClipTextureTestData` a la cola: DOL **idéntico** al byte.
   Eso desactiva media docena de «hay que compensar» que parecían obligatorios.
5. **DOL real 14.323 → 10.658 B** (−3.665). `data3` (`.data`) de **2.027 a 14**, `text1`
   de 6.567 a 4.922, `text2` de 14 a 7. Lo que queda es **`.rodata` (5.715 B, intacto)**
   y las tres funciones que no casan. `epCalculate` y `eProject` **no se han tocado**.

---

## 1. La medida, antes y después

`ANTES` = HEAD `c0a74169`, que es exactamente el estado con el que cerró la r58
(`dolwhere` daba 21.138, el mismo número de su tabla).

| | ANTES | DESPUÉS |
|---|---|---|
| `fncmp` | 3 de 539, **4.384 B** | 3 de 539, **4.384 B** — sin regresión |
| `linkdelta` | `.text +0  IGUAL` | `.text +0  IGUAL` |
| `desplaza` **DETRÁS** | 0 en las cuatro secciones | **0 en las cuatro** |
| `desplaza` `.data` DENTRO mal | **317** de 325 | **1** de 325 |
| `desplaza` `.bss` DENTRO mal | 151 de 152 | **109** de 152 |
| `desplaza` `.rodata` / `.text` DENTRO mal | 7 / 12 | 7 / 12 (sin tocar) |
| `dolwhere` (cuenta rangos, infla) | 21.138 B | **13.717 B** |
| **BYTES REALES del DOL** | **14.323** | **10.658** |
| `trypromo` | DOL ROTO `5e2a71311179` | DOL ROTO `2d0fe6ca9305` |
| `sha1` del `.o` | `9bceb0308380d0bc4599787bad8dea90a2e87cfe` | `8ec86b5bf98d53eacb3d1be6fc4a1d8d894c427a`, **tres compilaciones iguales** |

Desglose por sección del DOL (`agent_ecs_doldiff.py`):

| | r58 | r59 | delta |
|---|---:|---:|---:|
| `text1` | 6.567 | **4.922** | −1.645 |
| `text2` | 14 | **7** | −7 |
| `data2` (`.rodata`) | 5.715 | 5.715 | 0 |
| `data3` (`.data`) | 2.027 | **14** | **−2.013** |
| **total** | **14.323** | **10.658** | **−3.665** |

Los 14 B que quedan en `.data` **no son de `.data`**: 2 B en `DefaultLightMaterial`
(puntero a un símbolo de `.bss` descolocado), 12 B dentro de `EnvmapTargetNames`
(punteros a cadenas de `.rodata`). O sea: **la `.data` de zEcstasy está terminada** y lo
que le queda cuelga de las otras dos secciones.

La unidad **no puede promocionar**: `epCalculateLocalDirectionalPOS16` y `eProject`
siguen VETADAS con prueba y no las he tocado.

---

## 2. LA REGLA QUE HABÍA QUE MEDIR: un `asm()` de fichero rompe la sección de GCC

El encargo decía que partir el bloque en 59 fragmentos «funciona». Funciona, pero no
como está escrito: **falta la mitad de la regla**, y sin ella la unidad no compila bien
—compila, y deja funciones dentro de `.data`—.

`varasm.c:976`:

```c
assemble_asm (string) {
  app_enable ();
  fprintf (asm_out_file, "\t%s\n", TREE_STRING_POINTER (string));
}
```

**No actualiza `in_section`.** GCC lleva su propia contabilidad de en qué sección cree
que está y `data_section()`/`text_section()` **no emiten nada si creen que ya están
ahí**. Medido con `ngccc -O1 -G0 -S` sobre un fichero de nueve líneas:

```c
int a1 = 1;                       /* GCC emite .section ".data"  -> in_data */
asm(".section .data\n ... ");     /* el ENSAMBLADOR sigue en .data, GCC cree in_data */
int a3 = 3;                       /* GCC no emite NADA: correcto, cae en .data  */
int f(int x){...}                 /* GCC emite .section ".text": correcto        */
```

y el caso venenoso (mismo fichero, con `-G8` en vez de `-G0`):

```c
int a1 = 1;                       /* -G8 -> .section ".sdata", in_sdata */
asm(".section .data\n ...");      /* ensamblador en .data, GCC cree in_sdata */
int a3 = 3;                       /* GCC NO emite nada -> a3 CAE EN .data */
```

De ahí las **dos formas** que uso, y no hay una tercera:

* **Fragmento pegado detrás de una variable de `.data`** → no restaura nada. Es la forma
  buena y la que usan 50 de los 57.
* **Fragmento entre dos funciones** (cuando el objetivo lo pone entre dos estáticos de
  función distintos) → **tiene que acabar en `.section ".text"`**. Comprobado con un
  objeto de control: `f__Fi` en `.text+0x00`, `probe_bss` en `.data`, `g__Fi` en
  `.text+0x10`. Sin el restaurador, `g__Fi` cae en `.data`.

**Nunca detrás de una función sin restaurador.** Es un fallo silencioso: compila, enlaza
y el `.text` sale corto.

---

## 3. Lo que de verdad había que mover: no eran 59 huecos, eran ~60 VARIABLES

El bloque `asm` sólo explica una parte. El otro renglón es que **el árbol tiene las
variables en otro fichero y en otro orden**. Ejemplos medidos, todos con su dirección:

| variable | estaba en | la tiene el objetivo en |
|---|---|---|
| `eDisableFixUpTables`, `eDirtyAnimations`, `AllowDuplicateSolids` | `Ecstasy.cpp` | `eSolid.cpp` (`0x8041A610..0x8041A620`) |
| `eDirtyTextures` | `Texture.cpp` | `eSolid.cpp` (`0x8041A61C`) |
| `eFrameCounter` | `Ecstasy.cpp` | `EcstasyE.cpp` (`0x8041AC38`) |
| `TweakLightMaterial` | `eLightE.cpp` | `eLight.cpp` (`0x8041A658`) |
| `WorldLightDirectionVector` | `Sun.cpp` | `eLight.cpp` (`0x8041A678`) |
| `pVisualTreatmentPlat` | `eVisualTreatment.cpp` | `EcstasyE.cpp`, **primer símbolo** (`0x8041ABDC`) |
| `EnvMapScreenZ` | `EcstasyE.cpp` | `eEnvMapE.cpp` (`0x8041B154`) |
| `ForceFERenderStates` | `EcstasyE.cpp` | `ePShader.cpp` (`0x8041B408`) |
| `SphericalPS`, `testl1..testc1` | `EcstasyE.cpp` | `EcstasyEx.cpp` (`0x8041B030..0x8041B084`) |
| `eTextureBucketSlotPool`, `eDataRenderSlotPool`, `g_NumTextureBuckets` | `eViewPlat.cpp` | `EcstasyEx.cpp` (`0x8041B020..`) |

Y dentro de `EcstasyE.cpp` había que **reagrupar 23 variables** que estaban repartidas
por las líneas 150…4670 en el bloque de cabecera que el objetivo abre en `0x8041ABDC`.
El truco para no romper los usos: dejar el **adelanto `extern`** donde estaba la
definición (no emite un byte) y bajar la definición al sitio bueno. Sólo `e_bDither` es
`static` y no admite adelanto — pero sus tres usos están **después** del punto nuevo, así
que basta con moverla.

Un caso aparte, y con el valor leído del DOL:

```c
void eDLSaveContext(Bool bEnabled) {
    static Bool _enabled = true;   // r59: sin inicializador se iba a .bss;
}                                  // el objetivo lo tiene en .data (0x8041AE01) y vale 01
```

---

## 4. `-strip-unused-data` HACE GRATIS a los símbolos que sobran

Los 13 símbolos de `.data` que definimos y el objetivo no tiene (`LFrot`, `cBfR`,
`ScreenTintScale`, `KColorWorldReflection`, `TestMWDesaturation`, …) **no cuestan nada en
el enlace** si nadie los referencia: el enlazador se lleva `size & ~3`, que para un
símbolo de 4 B es el símbolo entero.

**Prueba limpia**, porque la intuición dice lo contrario: moví
`HeadlightClipTextureTestData` de mitad de `EcstasyE.cpp` al final de `zEcstasy.cpp`
—4 B de desplazamiento en el OBJETO— y el DOL salió **idéntico al byte** (17.454 antes y
después). O sea que **la posición de un símbolo estripado no se ve en el enlace**.

Consecuencias prácticas, las dos útiles:

* No hay que compensar por ellos. Lo que sí hay que hacer es **borrarlos** cuando el
  objetivo tiene ahí un `lbl_`/`gap_` del mismo tamaño, porque si no emitimos los dos y
  el recorte de la cola tiene que absorber la diferencia. Y en varios casos **coinciden
  al byte**: `LFrot`+`LFangle`+`Rotty` = los 12 B de `gap_06_8041AB58_data`;
  `SmearBiasBASE`+`SmearBiasNOS` = los 8 de `lbl_8041AE5C`; `KColorWorldReflection` = los
  4 de `gap_06_8041B418_data`; `TestMWDesaturation`+`TestMWColourTint` = 8 de los 12 de
  `lbl_8041B4C4`. **No son basura: son las variables del original que nadie usa.**
* El que **sí** paga es el referenciado: `SmearBiasBASE/NOS` se usan en
  `EcstasyE.cpp:3870`, sobreviven, y por eso su definición se ha bajado al final de
  `zEcstasy.cpp`, donde el hueco de cola los absorbe.

**Y el corolario que ahorra una vuelta**: `desplaza` mide el ENLACE, pero mi comparador
de posiciones leía el OBJETO, donde los estripados sí ocupan. Los dos números difieren
justo en la suma de los estripados (140 en el objeto contra 104 en el enlace, para la
`.bss`). Si se mezclan, se persigue un desfase que no existe.

---

## 5. LA `.bss` TIENE DOS COLAS — y una de ellas es inalcanzable desde un `asm()`

La r58 dejó esto como «no comprobado». Ya está comprobado, y el resultado **cierra la vía
del `asm` para `.bss`**. Control (`ngccc -O1 -G0 -c`, cinco líneas):

```c
int gA[4];                                   /* global de fichero, sin inicializar */
int f1(){ static int sA[2]; return sA[0]++; }
asm(".section .bss\n probe_bss: .space 8 \n .section \".data\"");
int f2(){ static int sB[2]; return sB[0]++; }
int gB[4];
```
```
    00000000   8  sA.3          <- estatico LOCAL: en el punto del parseo
    00000008   8  probe_bss     <- el asm cae ENTRE los dos estaticos locales
    00000010   8  sB.7
    00000018  16  gA            <- globales de fichero: TODOS al final,
    00000028  16  gB               en finish_file, DETRAS de cualquier asm
```

O sea: **los estáticos locales salen donde están escritos** (y un `asm` se intercala
entre ellos), pero **los globales de fichero se drenan enteros en `finish_file`**. Como
los siete huecos de `.bss` del objetivo caen todos en la zona de globales
(`0x32A4`…`0x134C4`, y la zona de estáticos locales sólo mide `0x40`), **el bloque `asm`
no podía llegar nunca**: se quedaba en el byte `0x64` y empujaba **104 bytes** a toda la
`.bss` de la unidad.

La forma que sí llega es **un array de C++**:

```c
char gap_07_8046A760_bss[16];     // en Ecstasy.cpp, detras de OtherEcstacyTextures_name_hash
```

`keep.lst` ya los protege por nombre (`zEcstasy.o:gap_07_...`) y un array global de C++
**no se decora**, así que el símbolo sale con el nombre exacto. Con eso:
`.bss` DENTRO mal **151 → 109** y el DOL **−530 B**.

### Lo que NO se ha podido colocar, y por qué (para la próxima ronda)

**Dentro de la cola de `finish_file` hay AÚN OTRA partición.** Medido: de los siete
huecos, cinco caen justo detrás del símbolo al que los pego y **dos no**:

| hueco | dónde lo escribí | dónde cae | dónde lo quiere el objetivo |
|---|---|---:|---:|
| `gap_07_80461F54_bss` (8) | tras `PoolLightFlareList` (`eLight.cpp`) | `0xC4B0` | `0x32A4` |
| `gap_07_80466F98_bss` (16) | tras `tN` (`EcstasyEx.cpp`) | `0x137EC` | `0x82E8` |

Sus vecinos del objetivo (`PoolLightFlareList` de `eLightFlare[50]`, `eViews` de
`eView[]`, `tN` de `bVector3[][]`) son **objetos de tipo con constructor**, y salen en un
tramo ANTERIOR de la `.bss`; los `char[]` sin inicializar salen en el tramo POSTERIOR.
Los cinco que sí caen bien tienen vecinos POD.

**La palanca obvia —declarar el hueco como un tipo con constructor— está VETADA por
medida indirecta**: emitiría una llamada más en
`__static_initialization_and_destruction_0`, y esa función hoy casa al byte (está en el
saco de «sólo nombres distintos», 4.544 B). Cambiaría código a cambio de 24 bytes de
colocación. **No lo he hecho.**

Queda por probar (no medido): si `finish_file` recorre las declaraciones del espacio de
nombres en un orden que se pueda leer con `-fdump-*` o con un control de 30 líneas, la
partición se conocería y el hueco se podría colocar con el tipo POD adecuado.

---

## 6. El sitio de cada fragmento se lee del objeto, y el contenido se lee del DOL

Dos cosas que conviene no re-descubrir:

* **El sitio**: `sort` de los símbolos de `.data` del objeto extraído; para cada
  `gap_06_*`/`lbl_804*`, el símbolo NO-asm que le precede. Salen **57 líneas** del tipo
  `TRAS MAXreflectionStretch : lbl_8041A64C`, y esa lista **es** el plan de trabajo.
* **El contenido**: el bloque de la r58 tenía **13 fragmentos que emitían más bytes que
  su `.size`** (`gap_06_8041A661_data` declaraba `0xB` y emitía 12; `lbl_8041A65C`
  declaraba 5 y emitía 8; …), **28 bytes de más en total**. Mientras el bloque iba
  entero al final eso era relleno inocuo y se compensaba recortando el hueco de cola.
  **Al partirlo deja de serlo**: cada byte de más desplaza todo lo que va detrás del
  fragmento. Los 57 fragmentos se han regenerado leyendo los bytes **del DOL original**
  (`main.dol`, sección de `.data` en `0x8041A5A4`) con el tamaño exacto, `.4byte` cuando
  la dirección está alineada y `.byte` cuando no.
  Efecto colateral bueno: el hueco de cola `gap_06_8041B530_data` vuelve a su tamaño
  íntegro (`0x9C`), sin recorte. **La unidad ya no depende de un número mágico.**

---

## 7. Trampas de medida nuevas (todas con su caso)

* **Emparejar símbolos por nombre normalizado COLISIONA** cuando una función tiene dos
  estáticos con el mismo nombre en funciones distintas (`lastactive.31181` y
  `lastactive.31182`). Mi comparador se quedaba con el último y marcaba `MAL` **treinta
  símbolos que estaban perfectos**. Hay que emparejar por (nombre, n-ésima aparición).
* **`dolwhere` imputa al símbolo equivocado** cuando la sección está descolocada: los
  370 B de la línea `lbl_8041A62C +0xA` estaban en realidad a 3,5 kB de ahí. La
  atribución sólo vale cuando el tamaño y el orden ya casan.
* **`linkdelta` sigue diciendo `IGUAL` con la unidad hasta 31 B corta** (la trampa de la
  r58). La usé sólo como criba; el juez fue `desplaza` en cada una de las ~15 vueltas.
* **Un `char[N]` no se puede desalinear**: `__attribute__((aligned(1)))` no baja la
  alineación, GCC alinea a 4 todo array de 4 B o más. Por eso
  `gap_07_8046D14B_bss` (21 B en una dirección impar, `0x8046D14B`) cae 1 byte tarde y
  hay 5 B de relleno de más detrás de `HorizonCurrentUVS`. Medido, no arreglado.

---

## 8. Lo que le queda a zEcstasy: 10.658 B, y 5.715 son `.rodata`

* **`.rodata`: 5.715 B en UN SOLO rango**, de `0x803DD970` a `0x803DF190`. La cabeza casa
  792 B y se rompe **exactamente en el byte 0x318**, dentro de `lbl_803DD958`: el
  objetivo tiene ahí cadenas (`…table`, `eSmoothVertices`) y nosotros floats
  (`3f800000 47800000 …`). **`rodorden.py` dice que nuestras 261 cadenas están en el
  MISMO ORDEN que las suyas**, así que no es orden de cadenas: es que el compilador emite
  su pozo de floats antes que sus cadenas y el objetivo los intercala. Partir el bloque
  `asm` de `.rodata` entre los `#include` (la receta de la r58) es necesario pero **no
  suficiente**: hace falta además saber qué decide el orden float/cadena dentro de un
  fichero. Es el frente siguiente y es una ronda entera.
* **`.text`: 4.922 B**, de los que **~1.000 son las tres funciones que no casan** y el
  resto son reubicaciones a `.rodata` y `.bss` descolocadas — caen solas cuando caigan
  esas dos.
* **`.bss`: 109 de 152 fuera de sitio**, con el diagnóstico del §5. Vale, como cota
  superior, la parte de esos 4.922 B cuyo desfase de campo de 16 bits coincide con un
  desplazamiento de `.bss` (los deltas `−104 ×105`, `−96 ×75`, `−64 ×22` de la
  distribución son de `.bss`; los de miles, de `.rodata`).

---

## 9. Estado de coordinación

* **`lcfix.py --check`: 242 correcciones pendientes, y NINGUNA es de zEcstasy** (grep
  literal: 0 líneas). **Ninguna rompería mi unidad**: no he añadido ni movido literales
  del compilador —sólo `asm`, arrays y definiciones de variable—, así que la numeración
  `$LC` de zEcstasy no se toca. Las 242 vienen de otros agentes; las aplica el jefe.
* **No he tocado** `config/GOWE69/*`, `splits.txt` ni `keep.lst`. Las entradas de
  `keep.lst` de zEcstasy siguen valiendo todas tal cual (los 57 `gap_06`/`lbl_8041` y los
  7 `gap_07` conservan su nombre exacto). La única entrada que queda muerta es
  `zEcstasy.o:gap_07_80467048_bss`, que ya lo estaba desde la r58.
* **Cero regresiones posibles en unidades ajenas, y está medido, no supuesto**: los 17
  ficheros que toco aparecen en **una** SourceList (`zEcstasy.cpp`) y en ninguna regla de
  `build.ninja`; **no he modificado ni una cabecera**. `fncmp` da el mismo 3 de 539 /
  4.384 B antes y después, y `desplaza` da **DETRÁS = 0** en las cuatro secciones.
* Ficheros tocados (17): `SourceLists/zEcstasy.cpp`; `Indep/Src/Ecstasy/{Ecstasy,
  eModel, eSolid, eLight, Texture, EmitterSystem}.cpp`; `Indep/Src/World/Sun.cpp`;
  `GameCube/Src/Ecstasy/{EcstasyE, EcstasyEx, eLightE, eEnvMapE, ePShader, eVShader,
  eStrip, eViewPlat, eVisualTreatment}.cpp`.
  *(`Sun.cpp` no está en `Ecstasy/`, pero es de zEcstasy en exclusiva y el objetivo tiene
  su `WorldLightDirectionVector` dentro del bloque de `eLight`; lo señalo por si el jefe
  prefiere revertir ese único cambio.)*
