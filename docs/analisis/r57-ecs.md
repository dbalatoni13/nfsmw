# r57 — zEcstasy: el orden de `.text` cae de 34 desplazadas a 4, y son 77.040 B de DOL

Agente `ecs`. Unidad en propiedad: **zEcstasy**. Ficheros tocados, todos de `Ecstasy/` y
todos **exclusivos de zEcstasy** (comprobado: ninguna otra SourceList los incluye):
`EcstasyE.cpp`, `EcstasyEx.cpp`, `eViewPlat.cpp`, `EmitterSystem.cpp`, `eLight.cpp`,
`eSolid.cpp`. **Ninguna cabecera**, ningun `config/`, ningun `keep.lst`, ningun commit.

**Resumen en cuatro lineas.** (1) El encargo pedia cambiar de eje y el eje que pagaba era el
`.text`: **535 de 539 funciones en su sitio** contra las 505 de partida, y el DOL pasa de
**487.279 a 410.239 bytes distintos (−77.040)**. (2) La palanca que lo abre no es mover
codigo, es **declarar**: un bloque de `extern` en el destino deja mover 9 kB de funciones sin
tocar un byte de `.data`. (3) Tres funciones vivian en el `.cpp` equivocado —el DWARF del
original lo dice— y colocarlas valio **42.120 B de una sentada**. (4) `.rodata −424` es
**todo-o-nada**: arreglarlo a medias paga CERO, y esta medido.

---

## 1. La medida, antes y despues

| | ANTES (HEAD `e140d029`) | DESPUES |
|---|---|---|
| `fncmp` | 3 de 539, 4.384 B | **3 de 539, 4.384 B** (sin regresion) |
| `permorden` en su sitio | 505 / 539 | **535 / 539** |
| `permorden` desplazadas | 34, 18 ciclos no triviales | **4, 2 ciclos** |
| `linkdelta` | `.text +0 rodata−416 data+64 bss+32` | `.text +0 rodata−424 data+64 bss+32` |
| `trypromo` | DOL ROTO `368c71e25065` | DOL ROTO `92b47d3c939e` |
| **bytes distintos del DOL** | **487.279** | **410.239** |
| `.text` distinto DENTRO de zEcstasy | 85.606 B | **7.423 B** |
| `sha1` del `.o` | — | `b26ee6b814ddd2cb90fded22773770a4c0137b3a`, **tres compilaciones iguales** |

La unidad **no puede promocionar** y no es culpa de esta ronda: dos de sus tres funciones
(`epCalculate`, `eProject`) estan VETADAS con prueba y no se han tocado.

El desglose del progreso, medida a medida (todas con `agent_ecs_doldiff.py`, ver §6):

| edicion | bytes distintos del DOL | delta |
|---|---:|---:|
| base | 487.279 | |
| eSolid: dos funciones a su sitio | 484.978 | −2.301 |
| eLight: `eResestLightFlarePool` detras de `eRenderLightFlarePool` | 484.415 | −563 |
| EcstasyE: el bloque de `epRenderStrips` (13 fn) delante de `eInitGX` | 473.120 | −11.295 |
| EcstasyE: `DisplayRVMs`/`eFacePixelate`/`eDisplayLetterBoxes`/`eDisplaySafezone` | 467.672 | −5.448 |
| EcstasyE: `eScreenQuadReplace`/`eMotionBlurEffect`/`eRenderSky` delante de `eEURGB60ModeCheck` | 457.981 | −9.691 |
| **EcstasyEx: las tres funciones de `eTextureBucket` cambian de fichero** | **415.861** | **−42.120** |
| `ClassKey` pasa a `inline` | 410.239 | −5.622 |

---

## 2. LA PALANCA DE LA RONDA: `extern` en el destino

El orden de `.text` de una unidad es **literalmente** el orden de las definiciones en el
fuente, asi que colocar una funcion es mover su cuerpo. El problema es que un `.cpp` real
tiene **datos de fichero entre las funciones**, y arrastrarlos mueve la `.data`.

La solucion cuesta cero bytes: **dejar los datos donde estan y poner sus declaraciones
`extern` en el destino**. En `EcstasyE.cpp` hicieron falta 22 (`epRenderStrips_psReset`,
`EnableHarmonicClear`, los seis `DOF_*`, los tres `BLOOM_*`, `SphericalPS`, `testl1..testc1`
y `QSizeAccumulationI8`) y con ellas el bloque de 13 funciones se movio 1.500 lineas hacia
arriba con `fncmp` intacto y `linkdelta` **identico**.

Y no es un truco: **el objetivo hace lo mismo**. Volcando los simbolos de la `.data` de
zEcstasy en orden de direccion se lee el orden de definicion del original, y ahi
`EnableHarmonicClear` sale en `0x8041AD0C`, **detras** del `__sync_token` de `eEmitSync`
(que es `.text` 256) aunque lo use `Render(ePoly*,TextureInfo*,int)`, que es `.text` 228.
O sea: el original tambien declara antes y define despues.

Ese volcado (`.data` del objetivo, simbolo a simbolo, con los huecos) es la mejor fuente que
he encontrado para reconstruir el orden del fuente original. Ejemplo directo: dice que
`SphericalPS` y `testl1..testc1` estan en `0x8041B030`/`0x8041B04C`, **detras** de
`eTextureBucketSlotPool`/`g_NumTextureBuckets`, o sea que en el original **no estan en
EcstasyE.cpp sino en EcstasyEx.cpp**. No lo he movido (es `.data`, ver §5), pero queda medido.

---

## 3. TRES FUNCIONES EN EL FICHERO EQUIVOCADO: 42.120 B

`symbols/debug_lines.txt` da fichero y linea de **cada instruccion** del original. Cruzandolo
con los simbolos del objeto extraido sale el mapa **funcion -> .cpp del original** para las
539 funciones. Comparado con el nuestro:

    eSubmitMesh, eTextureBucket::Flush, eFlushTextureBucketList
      nuestro : eViewPlat.cpp   (se parsea el 20o de 23)
      original: EcstasyEx.cpp   (el 11o)

Eran **520 B mal colocados que arrastraban 24.908 B** de funciones (obj 336..425) y otros
**31.560 B** (obj 426..535) por los 128 B de `Flush`. El arreglo:

1. `struct eTextureBucket` y las tres funciones se van al **final** de `EcstasyEx.cpp`
   —detras de `struct eDataRender`, que `AddMeshRender` necesita completo—.
2. El bloque `InitSlotPoolsEx .. epCalculate` de `EcstasyEx.cpp` (lineas 1208-1921) **baja
   detras de ellas**. Asi el orden emitido queda `eResetIndirectTextureSetup`,
   `eFlushTextureBucketList`, `eSubmitMesh`, `InitSlotPoolsEx`, `gain`,
   `eResetContrastSurface`, `eInitContrastSurface`, `epCalculate` = obj 333..340 **exacto**.
3. `typedef float sn_ps` y `#define V2` viajaban dentro de ese bloque y hay codigo anterior
   que los usa: suben a la cabecera del fichero. Cero bytes.

**Y esto es un frente, no un caso.** El mapa funcion->fichero se saca en segundos para
cualquier unidad y nadie lo habia mirado. Si en zEcstasy tres funciones mal ubicadas valian
42 kB, el barrido de las otras 26 unidades merece un encargo entero.

---

## 4. `inline` DETRAS DEL LLAMANTE = llamada + cola de `finish_file`

Dos posiciones del objetivo (`Flush__14eTextureBucket` en la 536 y
`ClassKey__Q36Attrib3Gen15light_flares_cg` en la 533) estan **detras de las plantillas de
EmitterSystem**, o sea en la cola que drena `finish_file`, no en el punto del parseo.

**Cuerpo EN CLASE no vale, y esta medido**: con `void Flush() {...}` dentro de
`struct eTextureBucket`, GCC 2.9 la expande en `eFlushTextureBucketList` (**296 B contra los
204 del objetivo**) y el simbolo `Flush__14eTextureBucket` **desaparece**. Revertido.

Lo que si vale es **`inline` FUERA de la clase y DETRAS de su unico llamante**:

    struct eTextureBucket { ... void Flush(); ... };
    void eFlushTextureBucketList() { ... bucket->Flush(); ... }   // sale `bl`: aun no hay RTL
    inline void eTextureBucket::Flush() { ... }                   // DECL_INLINE -> a la cola

Cuando se compila el llamante la funcion **todavia no tiene `DECL_SAVED_INSNS`**, asi que
`expand_call` no puede integrarla y emite la llamada; y por ser `inline` no se emite en el
punto del parseo sino en la cola. Las dos cosas a la vez, que es justo lo que hace el objetivo.

La misma receta cierra `ClassKey`: la definicion out-of-line del final de `EmitterSystem.cpp`
(la que suministra la guarda `ATTRIB_NO_INLINE_CLASSKEY`) pasa a `inline Key
Gen::light_flares_cg::ClassKey()`. Sale de la posicion 520 y **deja de empujar 7.484 B**
(las nueve plantillas de EmitterSystem, `__static_initialization_and_destruction_0` y
`GetPtr__11LoadedTableUi`). Vale 5.622 B de DOL y la llamada sigue siendo `bl` porque en los
llamantes la guarda apaga la copia de la cabecera y solo hay declaracion.

Quedan 404 B de cola sin colocar: el objetivo pone `Init__20GrandSceneryCullInfo`,
`Init__11DefragFixer`, `ClassKey`, los dos `_type_map`, `Flush`, `Render`; nosotros ponemos
los `_type_map` antes de los dos `Init__` y `ClassKey` detras de `Flush`.

---

## 5. Los negativos, con su cifra

1. **`elRotateLightContext` en la posicion del objetivo: NEGATIVO CATASTROFICO, y es veda.**
   El objetivo la emite en `.text` 103, detras de `CartesianToSpherical`. Movida ahi, la
   sentencia `*new_light_context = *light_context;` **deja de expandirse en linea**: GCC
   sintetiza `__as__20eDynamicLightContextRC20eDynamicLightContext` **fuera de linea**, la
   funcion baja de 420 a 312 B y **el simbolo nuevo (312 B) sobrevive al enlace**, asi que
   `.text` crece y todo lo que va detras se desplaza. Medida:
   **DOL 2.980.979 bytes distintos** contra 415.861. Probadas dos posiciones (detras de
   `UnloaderLights` y detras de `CartesianToSpherical`): las dos rompen; en la posicion
   actual (delante de `LoaderLights`) casa al byte. Revertido.
   *Lo que valdria colocarla bien: 2.764 B. Lo que hace falta: escribir la copia miembro a
   miembro a mano, porque la sintetizada depende de la posicion.*
2. **`eTextureBucket::Flush` con el cuerpo EN CLASE**: 296 B contra 204 y el simbolo se
   evapora (§4). Revertido.
3. **Arreglar la `.rodata` a medias paga CERO.** El deficit es `−424 B` y **todo lo que va
   detras de la `.rodata` de zEcstasy se desplaza esos 424 B**: eso es lo que produce los
   168.452 B de `data2`, los 162.984 de `data3` y **64.293 de los 71.716 de `.text`** (son
   reubicaciones `@ha/@l/@sda21` de TODA la imagen apuntando a datos corridos). Bajar el
   deficit a −296 deja el desplazamiento igual de roto. Es todo-o-nada, como `matched_code`.

### Lo que le falta de verdad a la `.rodata` (medido, para quien la ataque)

* **Los primeros 128 B del rango NO los emitimos**: `GAMECUBE`, `d:/mw/speed/indep/bware/inc/bware.hpp`,
  `bad_alloc`, `%f,%f,%f`, `%f,%f,%f,%f`, `STL`, `High`, `Medium`, `Low`, `Reflection` y un
  `0.5f`. Es el prefijo de vocabulario compartido de `prefijotu` (que cuenta 7 cadenas / 79 B
  porque compara con nuestro OBJETO, no con el ENLACE).
* El resto es **permutacion**: alineando por palabras salen unos 40 bloques, el mayor un
  `insert` nuestro de 3.128 B (el vocabulario de AttribSys, que el objetivo reparte) y un
  `delete` del objetivo de 916 B (`ePS_LINE`, `ePS_GOURAUD`...). Neto exacto: **−424**.
* **El bloque `asm()` de `.rodata` de `zEcstasy.cpp` esta ENTERO MUERTO**: de sus ~620
  simbolos, **615 (7.415 B) los estripa el enlazador** por no tener entrada en `keep.lst` ni
  referencia. No cuesta nada, pero tampoco sirve de nada hoy.
* Meter el prefijo de 128 B exige **una entrada de `keep.lst`** y eso es del jefe (regla 3).
  **No lo he hecho**, y aviso de que no paga un byte hasta que el deficit entero sea 0.

---

## 6. Herramientas que dejo en `scripts/`

| | |
|---|---|
| **`agent_ecs_doldiff.py`** | `trypromo` **contando bytes distintos del DOL, por seccion**. Es la unica medida de progreso de una unidad que no puede promocionar, y `trypromo` solo da OK/ROTO. Generico: `python scripts/agent_ecs_doldiff.py <unidad>` |
| **`agent_ecs_move.py`** | mueve un bloque de lineas dentro de un fichero por indices y **verifica que el multiconjunto de lineas no cambia**. Respeta CRLF. Es lo que ha hecho las once mudanzas de esta ronda sin una sola errata |

---

## 7. Trampas nuevas

* **Mover una funcion PUEDE cambiar su codigo.** No es teorico: `elRotateLightContext`
  cambia de 420 a 312 B solo por cambiar de sitio (§5.1). La regla 6 del brief —`fncmp`
  despues de CADA fichero— lo caza, y aqui lo cazo dos veces.
* **Y puede meter un simbolo NUEVO en el enlace.** El `__as__` sintetizado no aparece en
  `fncmp` (compara funciones comunes): hay que mirar `linkdelta` o `doldiff`.
* **`fncmp`/`permorden`/`textorder` quieren la RUTA COMPLETA** (`Speed/Indep/SourceLists/zEcstasy`);
  con el atajo `zEcstasy` dicen «no existe .../obj/zEcstasy.o» y se paran.
* **El `.o` NUESTRO esta en `build/GOWE69/src/...`; en `build/GOWE69/obj/...` esta el
  EXTRAIDO.** Selle el sha1 del extraido durante media hora y no cambiaba nunca: claro.
* **Los `.cpp` de este arbol son CRLF** y `grep` o `cat -A` a traves de `sed` no lo enseñan.
  Una insercion escrita con `'\n'.join(...)` deja lineas LF sueltas en un fichero CRLF; se
  ve con `git diff --stat` («LF will be replaced by CRLF»). Normalizado antes de cerrar, y
  comprobado que **no cambia el `.o`** (mismo sha1).
* **`build_direct.py` solo enseña 6 lineas de error.** Con un fichero de 4.600 lineas eso
  esconde la causa; hay que llamar a `build_direct.compile_one` a mano para ver el resto.

---

## 8. Lo siguiente para zEcstasy

1. **404 B de cola**: `ClassKey` y los dos `_type_map` respecto de los dos `Init__`.
2. **`elRotateLightContext`, 2.764 B**: solo sale escribiendo la copia miembro a miembro
   (§5.1). Hoy es veda con prueba.
3. **`.rodata`**: paquete atomico de 424 B = prefijo de 128 B (necesita `keep.lst`) + 7
   cadenas + el orden del pool. Hasta que sea 0 no paga NADA.
4. **`epCalculate` y `eProject` siguen VETADAS y no las he tocado**; `UpdatePlatInfo` la he
   dejado como estaba: `previo.py UpdatePlatInfo` lista r27/r30/r36b/r36c/r36e/r36f mas la
   r48/r49 y la r56, que la dejo en 3 filas con dos andamios que no pagan un byte.

## 9. Propuesta para el jefe

**Un encargo de barrido**: el mapa `funcion -> .cpp del original` que sale de
`symbols/debug_lines.txt` en segundos, aplicado a las 26 unidades restantes. En zEcstasy
destapo tres funciones en el fichero equivocado y valieron **42.120 B**. Nadie lo ha mirado
en ninguna otra unidad.
