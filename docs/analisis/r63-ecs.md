# r63 -- agente `ecs` (unidad zEcstasy)

## Resumen

| | al empezar | al acabar |
|---|---:|---:|
| **BYTES DISTINTOS DEL DOL** (`dolwhere zEcstasy`, rangos fundidos) | **9.449** | **9.449** |
| el mismo enlace, rangos SIN fundir (la cifra de la r62) | 7.525 | 7.525 |
| `fncmp Speed/Indep/SourceLists/zEcstasy` | 3 de 539, 4.384 B | 3 de 539, 4.384 B |
| `linkdelta zEcstasy` | `.text +0` IGUAL | `.text +0` IGUAL |
| `textorder zEcstasy` | 6 descolocadas de 540, 5 saltos | 6 de 540, 5 saltos |
| `sha1` del `.o` | `4135718963475f9c38efd15707b35b5940d8380e` | `4fabf7e50bec2a3db7b8529003192afde4fef2a7` (tres compilaciones) |

**NO promociona y NO he ganado un byte de DOL.** Lo unico que cambia en el arbol
son **tres bloques de comentario** (cero lineas de codigo): el `.o` cambia de
sha1 solo por los numeros de linea del DWARF, y `fncmp`, `textorder`,
`linkdelta` y el DOL enlazado dan **exactamente lo mismo antes y despues**
(comprobado con el mismo banco de enlace).

Lo que si dejo es: **la veda de la fila 98 de `UpdatePlatInfo` con 17 medidas
nuevas**, **el eje que la r48 dejo vivo en `eProject` ejecutado y cerrado con 8
medidas**, y --lo que de verdad importa-- **el plan de la `.rodata` que la r62
dejo escrito para "la ronda siguiente" convertido en una tabla ejecutable, con
la medida que le faltaba para poder empezar**.

---

## 0. Correccion al encargo (es lo primero que hay que leer)

El encargo manda hacer el `PASO 2` de `UpdatePlatInfo` sobre "la base correcta de
60 filas" quitando el pin de `eLightE.cpp:415`. **Eso ya se ejecuto en la r61 y
salio peor; y lo que hay en el arbol hoy es el resultado BUENO de esa ronda.**

* Base de hoy: **3 filas / 99,960860 % / 2.044 B** (no 28, no 60).
* Las dos ediciones que la produjeron estan en `eLightE.cpp`: la barrera
  `__asm__("")` detras de `envmap_power = 6.0f` en el `case 0x68E97F75` y la
  conmutacion `envmap_min_b = material_data->EnvmapMinB * envmap_min_scale`.
* El barrido "sin pin" que el encargo pide esta medido en la r61: `sin pin` 60,
  `sin pin + barrera` 57, `sin pin + barrera + minbc` 48. Todos peores que 3.
* La FAMILIA F que el encargo reabre (mover `envmap_power` dentro del case, las
  27 filas de la r36f) tambien esta cerrada por la r61: forma parte de las 78
  permutaciones que barrio y ninguna baja de 3.

Reproducido esta ronda en el banco antes de tocar nada, para que no quede duda.

**Aviso de herramienta**: desde la r62, `eLightE.cpp` **ya no es autocontenido**
(usa `_r62_pad16`, definido en `zEcstasy.cpp`), asi que
`scripts/agent_ecs_upi.py` falla con `syntax error before ';'` en la linea 43. Se
arregla pasandole `--extra=-include` con un fichero que declare `_r62_pad8` y
`_r62_pad16`. Sin eso el banco de la r56/r61 esta MUERTO y no lo dice.

---

## 1. `UpdatePlatInfo` -- 17 medidas nuevas, todas negativas

Lo que queda son 3 filas, y la fila 98 merece una precision que ninguna ronda
habia escrito:

```
  98  fmuls f26, f5, f10   (objetivo)   |   fmuls f26, f10, f5   (nuestro)
```

**Los TRES registros ya son los del objetivo en los dos lados.** El reparto de
esa instruccion esta CERRADO. Lo unico que difiere es el ORDEN DE OPERANDOS de un
`fmuls` conmutativo, y ese orden lo fija el RTL que sale del expand, o sea la
fuente. El problema es el bucle:

* con el orden bueno (`scale * MinB`) el primer operando es el hard reg `f5` del
  pin, `set_preference` (`global.c:1551`) toma `XEXP (src, 0)` y le cuelga a
  `envmap_min_b` una `hard_reg_preferences` de f5 que el original no tiene: **24
  filas**;
* conmutando, la preferencia desaparece y el reparto casa: **3 filas**, pero el
  `fmuls` sale al reves.

Medidas nuevas (banco de ~2 s por medida):

| eje | medidas | resultado |
|---|---:|---|
| sin conmutar + uso extra de `envmap_min_scale` detras de `min_b`, forma VOLATIL `("" : : "f"(x))` | 4 | 259-272 filas y **1.952/1.960 B** (parte el bloque de sched) |
| lo mismo, forma no volatil `("" : "+f"(x))` | 4 | **24 filas en los cuatro sitios**: `flow` lo borra, uso cero |
| sin conmutar + **pin sobre el DESTINO** `register float envmap_min_b asm("frN")`, N = 0,1,4,12,24..31 | 12 | 45-114 filas |
| sin conmutar + **temporal pinchado de vida 1 insn** `register float _mb asm("frN") = scale*MinB; float envmap_min_b = _mb;` | 8 | 24-54 filas |
| control: temporal sin pin | 1 | 24 |

El **pin sobre el destino** es el unico camino teoricamente limpio que quedaba, y
por eso lo mido y lo dejo escrito: con `dest` y `src` los dos hard reg, **las dos
ramas de `set_preference` (`global.c:1596` y `:1615`) piden
`>= FIRST_PSEUDO_REGISTER` en el otro extremo y ninguna dispara**, o sea que mata
la preferencia SIN conmutar. Y aun asi pierde: el pin ata f26 en TODO el ambito y
`envmap_min_b` se reescribe en cinco `case`. El **temporal de vida 1 insn** es la
version acotada de esa misma idea (y ademas le regala a `envmap_min_b` una
`hard_reg_copy_preferences` a f26, que es la que `find_reg` mira PRIMERO): con
fr31/fr0/fr1/fr12 el temporal se absorbe y da 24 clavadas, con fr25/26/27/30 da
27-54.

**Veredicto**: para cerrar la fila 98 hace falta que `envmap_min_scale` NO sea un
`register asm` y aun asi caiga en f5. Todo lo demas esta barrido. La unica via
que sigue sin probar es la que la r61 dejo escrita: atacar el ORDEN de reparto de
la banda f5-f8 en la base sin pin (`allocno_compare`), que es prioridad y no
preferencia.

Anotado en `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`, donde `previo.py` lo
encuentra.

---

## 2. `eProject` -- el eje que la r48 dejo vivo, ejecutado y CERRADO

La r48 escribio, literal: *"Con dos barreras el ciclo (a) casa EXACTAMENTE
(107=f13, 137=f0) pero el resto del reparto se desmonta (25 filas). Quien retome
esto tiene ahi el unico eje vivo: cerrar (a) con x2 y arreglar lo que rompe."*

Ejecutado (banco propio: `eMathE.cpp` suelto con los cflags de zEcstasy, 1,5 s):

| variante | filas | pct | tam |
|---|---:|---:|---:|
| base (`asm x1`, lo que hay) | **14** | 93,970146 | 268 |
| `asm x2` | 26 | 87,701490 | 268 |
| `x2` + pin `halfVP3` fr13 / fr12 | 26 / 26 | | 268 |
| `x2` + pin `clipY` fr12 / fr13 | 26 / 26 | | 268 |
| `x2` + pin `halfVP2` fr0 | 26 | | 268 |
| `x2` + pin `clipX` fr0 | 22 | 88,149254 | 268 |

Y un detalle que corrige la lectura de la r48: con `x2` **no queda "(a) cerrado y
un domino de registros roto"**. Se desmonta la cadena entera de `*sx` (las filas
29-31 desaparecen y aparecen 39/43/44 nuevas), o sea que lo que rompe no es
repinchable. **Eje cerrado.**

Lo unico que sigue sin explorar en esta funcion es lo que la r49 demostro con
`lmap` sobre el original: **el `asm` no esta en la fuente original** (el mapa de
lineas no deja hueco para el). Quitarlo cuesta 4 filas (13 -> 17). Quien vuelva
tiene que buscar la forma de fuente que da el horario del objetivo **con cero
asm**, no una barrera mas.

Anotado en `src/Speed/GameCube/Src/Ecstasy/eMathE.cpp`, delante de `eProject`.

---

## 3. `epCalculateLocalDirectionalPOS16` -- no la he tocado, y por que

El encargo la manda atacar con el decompilado de Ghidra. La he perfilado antes de
gastar una medida y **el reparto dice que no es ahi donde esta el trabajo**:

* de las 155 filas del `fndiff`, **50 son literalmente el `+8` del marco** (48
  desplazamientos `+8` de `(r1)` mas el `stwu` y el `addi` del prologo/epilogo);
* la r54 ya demostro que ese `+8` **no tiene palanca de fuente** (la ranura
  huerfana la reserva `reload1.c:4012` y la deshereda `reload1.c:6878`, dos cosas
  que no aparecen en el codigo emitido) y que **se cae sola** cuando se arreglen
  las ~105 filas de codigo real;
* esas ~105 estan concentradas en 301-479 y son reparto/horario, no forma.

Contrastado el decompilado de Ghidra con nuestra fuente: el unico apunte de forma
que trae (`r63-ghidra-27.md`, el `case -1: break;` de `EcstasyEx.cpp:4117` que el
original no compara nunca) **es un andamio de arbol de switch, no una
diferencia**: el segundo `switch` del original tiene 9 `case` y nosotros esos 9
mas el `-1`, y un `case` vacio en el extremo del rango mueve el pivote sin emitir
comparacion propia (`memory/nfsmw-arbol-de-switch`). No lo he quitado: quitarlo es
una medida, no una lectura, y esta funcion **no es el cuello de botella de la
unidad** (ver el apartado 4).

---

## 4. EL HALLAZGO: el 67 % del DOL de zEcstasy NO son las tres funciones

`dolwhere zEcstasy` da **9.449 B** y **6.336 de ellos son UN SOLO rango de
`.rodata`** que empieza en `0x803DD970`. Las tres funciones abiertas ponen ~660 B.

Esto **no contradice** a la r61 ni a la r62: las dos midieron la `.rodata` del
OBJETO y por TAMANO, y por tamano esta exacta (`linkdelta` = `IGUAL`). Lo que
esta mal es el CONTENIDO del ENLACE, y ahi la cuenta es simetrica y limpia
(`dolrod.py zEcstasy .rodata`):

```
FALTA (el objetivo tiene y el enlace nuestro no):  3.148 B
SOBRA (emitimos y el objetivo no):                 3.148 B
```

Simetria perfecta = **ni falta ni sobra contenido, esta en el sitio de menos**,
que es exactamente lo que la r62 escribio en `zEcstasy.cpp` y dejo como plan para
"la ronda siguiente": partir el bloque `asm()` de la cabecera --que hoy cae ENTERO
delante de todos los `#include`, o sea al principio de la `.rodata`-- y colocar
cada fragmento entre las dos funciones cuyos `$LC` lo rodean.

### 4.1 La medida que le faltaba al plan: **4 inversiones de 551**

Antes de partir nada hay que saber si ademas hay que mover `$LC`. **No hay que
mover ninguno.** `lcmap.py zEcstasy` empareja **552 `$LC` con su `lbl_` del
objetivo, 0 conflictos**; ordenando esos 552 por la direccion del OBJETIVO y
comparandolo con el orden en que los emitimos hay **4 inversiones de 551 pares
consecutivos**.

O sea: **el pool que emite cc1plus ya sale en el orden del original.** El unico
elemento descolocado es el bloque `asm()`. Eso convierte el plan de la r62 de
"hay que reordenar la `.rodata`" en "hay que INTERCALAR 111 fragmentos", que es un
problema mecanico.

### 4.2 La tabla de colocacion (111 huecos, 4.105 B)

Con este emparejamiento (552 pares; la r62 llego a 584 mapeando 32 mas por
contenido, y su cuenta era 128 huecos / 3.417 B) los huecos son 111 y suman
4.105 B. Para cada uno se sabe **delante de que funcion va el fragmento**: la
primera funcion de `.text` que referencia el `$LC` que sigue al hueco.

Los diez mayores:

| direccion | bytes | antes de | colocar delante de |
|---|---:|---|---|
| `803DEE08` | 908 | `$LC882` | `GetPlaneState__FPC8bVector4PC8bVector3` |
| `803DDD74` | 504 | `$LC596` | `__InitRenderMode__Fv` |
| `803DE944` | 412 | `$LC729` | `GenerateHorizonFogDisplayList__FPPvPUl9_GXVtxFmt` |
| `803DD6D9` | 295 | `$LC377` | `eInitEngine__Fv` |
| `803DDBA4` | 256 | `$LC496` | `__5eView` |
| `803DD990` | 212 | `$LC449` | `SphericalToCartesian__FP8bVector3fff` |
| `803DE025` | 199 | `$LC529` | `eFixUpTablesPlat__Fv` |
| `803DD658` | 128 | `$LC57` | `InternalLoadStreamingEntry__17eStreamPackLoader...` |
| `803DDF9A` | 118 | `$LC528` | `InitSlotPools__Fv` |
| `803DF3D4` | 100 | `$LC976` | `__static_initialization_and_destruction_0` |

La tabla completa esta en el anexo.

### 4.3 Los dos avisos que hacen falta para no perder la ronda

**ORDEN.** Los fragmentos hay que colocarlos **de delante a atras**. El enlazador
empaqueta en orden de emision: la direccion de un fragmento solo es la buena si
todo lo anterior ya esta colocado. Colocar solo el hueco grande de 908 B **no
mueve una direccion** y la medida saldria plana. El observable por pasos es **la
primera direccion que difiere** en `dolrod.py zEcstasy .rodata` --hoy
`0x803DD970`--, no el total de bytes.

**TAMANO.** La `.rodata` enlazada ya mide exacto, asi que **el plan tiene que
SUSTITUIR contenido, no anadirlo**. Cualquier variante que salve bytes de mas
rompe las secciones, y lo he medido tres veces con un banco de enlace propio
(`scratchpad/ecs63/keepprobe.py`, que enlaza con un `keep.lst` alternativo sin
tocar el del repo):

| variante | efecto |
|---|---|
| + los 5 `$LC` muertos que `deadstr.py` propone y aun no estan en `keep.lst` | `.rodata` **+0x60** -> `LAS SECCIONES NO COINCIDEN` |
| + las 668 etiquetas del bloque `asm` que no estan en `keep.lst` | `.rodata` **+0x1340** -> secciones no coinciden |
| + `pShaderNames` / `vShaderNames` (los propone `stripped.py`) | **DOL identico**: esos simbolos no existen en NUESTRO objeto |

Lo que el bloque `asm` aporta hoy son 3.417 B en forma de **restos `size & 7`** de
simbolos muertos grandes (`-strip-unused-data` se lleva `size & ~7`) mas 397 B de
los 23 `lbl_` que si estan en `keep.lst`. Los fragmentos nuevos tienen que aportar
ESA misma cantidad, en los sitios buenos.

### 4.4 Los 5 `$LC` de `deadstr` (propuesta CONDICIONADA)

`deadstr.py zEcstasy` propone 33 cadenas; **28 ya estan en `keep.lst` y las 28
estan al dia** (comprobado una a una: contenido y numero de `$LC` coinciden con el
objeto de hoy). Faltan **5**, que son exactamente las que la trampa 1 del
docstring de `deadstr` ya anticipaba ("zEcstasy 5"):

```
# @lc zEcstasy "GAMECUBE"
zEcstasy.o:$LC58
# @lc zEcstasy "bad_alloc"
zEcstasy.o:$LC59
# @lc zEcstasy "Attrib::Gen::emitterdata"
zEcstasy.o:$LC233
# @lc zEcstasy "Attrib::Gen::emittergroup"
zEcstasy.o:$LC234
# @lc zEcstasy "Attrib::Gen::light_flares_cg"
zEcstasy.o:$LC448
```

**NO las propongo para aplicar hoy**: medidas solas dan `.rodata +0x60` y rompen
las secciones. Van con el paquete de los fragmentos, restando 96 B en otro sitio.
Las dejo escritas porque el numero de `$LC` es correcto AHORA y se desplaza en
cuanto alguien meta un literal antes.

---

## 5. `textorder`: 6 descolocadas, y son de la cola de `finish_file`

`textorder zEcstasy` da **6 de 540** descolocadas y 5 saltos de delta, sin cambio
esta ronda. Las seis estan en el ultimo tramo de
`wrapup_global_declarations (saved_inlines)`, o sea el bloque diferido, y el diff
es de **indice en `saved_inlines`**, que es el orden en que se COMPILA el cuerpo de
cada inline (`cp/decl.c:14701` -> `mark_inline_for_output`, `cp/decl2.c:2140`):

```
OBJETIVO : GetPtr, Init_GSCI, Init_DefragFixer, ClassKey, tmData, tmGroup, Flush, Render
NUESTRO  : GetPtr, tmData, tmGroup, Init_GSCI, Init_DefragFixer,
           [GetTexture, SetEFBCaptureRegion], Flush, ClassKey, Render
```

(mas `__8bVector3RC8bVector3` delante de `GetPtr`; los tres entre corchetes son
nuestros y no existen en el objetivo). Dos movimientos:

1. `Init__20GrandSceneryCullInfo` + `Init__11DefragFixer` tienen que salir
   **antes** de los dos `_type_map`;
2. `ClassKey__Q36Attrib3Gen15light_flares_cg` tambien **antes** de los dos
   `_type_map` (hoy sale una vuelta despues, detras de `Flush`).

El (2) es el caso de `memory/nfsmw-decl-comdat-inline` de manual: `ClassKey` es una
`inline` explicita puesta a mano en `EmitterSystem.cpp:2197`, o sea `DECL_COMDAT`,
y cae una vuelta tarde porque a su llamante (el constructor por `Key` de
`light_flares_cg`) no lo ha emitido nadie cuando le toca el indice. La receta
--interfaz explicita ACOTADA a la clase y bajo guarda de esta unidad-- esta escrita
en esa nota. **No la he aplicado**: mover una vuelta de `finish_file` cambia el
`.text` entero de la cola y no tenia banco para medirlo sin arriesgar la unidad en
una ronda que ya iba a la `.rodata`.

**Orden de `#include` que lo mueve: ninguno.** Estos seis simbolos son del bloque
DIFERIDO, no del codigo normal: su sitio lo fija el indice en `saved_inlines`, no
la posicion del `#include` del `.cpp`. Cambiar el orden de los `#include` de
`zEcstasy.cpp` mueve el codigo normal --y con el la `.rodata` y todo lo del
apartado 4-- sin tocar la cola. Es un frente distinto del de la `.rodata`.

---

## 6. Administrativo

* **Ficheros tocados: tres, y solo comentarios.**
  `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp` (bloque de `UpdatePlatInfo`),
  `src/Speed/GameCube/Src/Ecstasy/eMathE.cpp` (delante de `eProject`),
  `src/Speed/Indep/SourceLists/zEcstasy.cpp` (delante del bloque `asm` de
  `.rodata`). Cero lineas de codigo, cero `asm` nuevos, cero deuda.
* **Correcciones de `lcfix.py` pendientes: CERO, y NO he corrido `lcfix`.** La
  `.rodata` de la unidad no se ha movido un byte: los tres cambios son comentarios
  y el banco de enlace da el mismo DOL (7.525 B / 1.062 rangos) antes y despues.
  **Ninguna entrada venenosa para otra unidad**: no he anadido ni un literal, asi
  que ningun `$LC` de zEcstasy se desplaza y las 28 lineas de `keep.lst` que la
  nombran siguen validas.
* **Propuestas para `keep.lst` / `splits.txt` / `symbols.txt` / `configure.py`:
  las 10 lineas del apartado 4.4, y CONDICIONADAS** (solas rompen las secciones).
  Ningun `pad_` nuevo.
* `sha1` del `.o`, tres compilaciones iguales:
  `4fabf7e50bec2a3db7b8529003192afde4fef2a7`.
* Banco nuevo, en mi scratchpad (no en `scripts/`):
  `scratchpad/ecs63/keepprobe.py` --enlaza el DOL con `zEcstasy` promocionada y un
  `keep.lst` ALTERNATIVO, sin tocar el del repo-- y `scratchpad/ecs63/bench.py`
  --compila un `.cpp` suelto con los cflags de zEcstasy y compara UN simbolo--.
  Si sirven, que los suba a `scripts/` el jefe.

---

## Anexo: la tabla completa de los 111 huecos de `.rodata`

| direccion | bytes | antes de | colocar delante de |
|---|---:|---|---|
| `803DD658` | 128 | `$LC57` | `InternalLoadStreamingEntry__17eStreamPackLoaderP23eStreamingPackLoadTableP14eStreamingPackP15eStreamingEntry` |
| `803DD6D9` | 295 | `$LC377` | `eInitEngine__Fv` |
| `803DD80E` | 2 | `$LC378` | `ePreDisplay__Fv` |
| `803DD824` | 64 | `$LC384` | `InitStreamingPacks__Fv` |
| `803DD87B` | 1 | `$LC385` | `PrintStreamingPackMemoryWarning__FPCcii` |
| `803DD88F` | 1 | `$LC386` | `InternalLoadedStreamingEntryCallback__17eStreamPackLoaderPviT1` |
| `803DD893` | 1 | `$LC388` | `InternalLoadStreamingEntry__17eStreamPackLoaderP23eStreamingPackLoadTableP14eStreamingPackP15eStreamingEntry` |
| `803DD899` | 3 | `$LC389` | `InternalLoadStreamingEntry__17eStreamPackLoaderP23eStreamingPackLoadTableP14eStreamingPackP15eStreamingEntry` |
| `803DD8AA` | 82 | `$LC390` | `InternalLoadingHeaderPhase1Callback__17eStreamPackLoaderPviT1` |
| `803DD91B` | 13 | `$LC392` | `GetBoundingBox__6eModelP8bVector3T1` |
| `803DD92C` | 4 | `$LC393` | `eInitModels__Fv` |
| `803DD946` | 6 | `$LC394` | `SmoothNormals__6eSolidPP13eSmoothVertexi` |
| `803DD95C` | 44 | `$LC398` | `InternalLoaderSolidChunks__FP6bChunkP16eSolidListHeader` |
| `803DD990` | 212 | `$LC449` | `SphericalToCartesian__FP8bVector3fff` |
| `803DDAAC` | 4 | `$LC466` | `elSetupLights__FP20eDynamicLightContextP15eShaperLightRigP8bVector3P8bMatrix4T3P5eView` |
| `803DDAC4` | 32 | `$LC471` | `UpdateLightFlareParameters__Fv` |
| `803DDB84` | 4 | `$LC490` | `eRenderLightFlare__FP5eViewP11eLightFlareP8bMatrix4f19eLightReflexionType9flareTypefUif` |
| `803DDBA4` | 256 | `$LC496` | `__5eView` |
| `803DDCD4` | 16 | `$LC508` | `eInitTextures__Fv` |
| `803DDCFF` | 33 | `$LC511` | `UpdateTextureAnimations__Fv` |
| `803DDD24` | 4 | `$LC512` | `UpdateTextureAnimations__Fv` |
| `803DDD4B` | 1 | `$LC518` | `SetCurrentSunInfo__Fv` |
| `803DDD53` | 1 | `$LC519` | `RenderSunAsFlare__Fv` |
| `803DDD74` | 504 | `$LC596` | `__InitRenderMode__Fv` |
| `803DDF9A` | 118 | `$LC528` | `InitSlotPools__Fv` |
| `803DE025` | 199 | `$LC529` | `eFixUpTablesPlat__Fv` |
| `803DE0F7` | 1 | `$LC530` | `eFixUpTablesPlat__Fv` |
| `803DE103` | 1 | `$LC531` | `eFixUpTablesPlat__Fv` |
| `803DE117` | 1 | `$LC532` | `EnvMapTextureLoadedCallback__Fi` |
| `803DE11E` | 2 | `$LC533` | `EnvMapTextureLoadedCallback__Fi` |
| `803DE130` | 4 | `$LC534` | `__5ePoly` |
| `803DE1C2` | 2 | `$LC566` | `SetScreenBuffers__Fv` |
| `803DE1D3` | 1 | `$LC567` | `SetScreenBuffers__Fv` |
| `803DE1E3` | 1 | `$LC568` | `SetScreenBuffers__Fv` |
| `803DE1F7` | 1 | `$LC569` | `SetScreenBuffers__Fv` |
| `803DE241` | 3 | `$LC572` | `SetScreenBuffers__Fv` |
| `803DE25E` | 2 | `$LC573` | `SetScreenBuffers__Fv` |
| `803DE299` | 3 | `$LC575` | `Render__18eViewPlatInterfaceP5ePolyP11TextureInfoi` |
| `803DE2A4` | 24 | `$LC577` | `DisplayRVMs__FP5eView` |
| `803DE304` | 16 | `$LC597` | `__InitRenderMode__Fv` |
| `803DE32D` | 3 | `$LC594` | `__InitRenderMode__Fv` |
| `803DE334` | 4 | `$LC595` | `__InitRenderMode__Fv` |
| `803DE340` | 32 | `$LC598` | `__InitGXlite__Fv` |
| `803DE38C` | 4 | `$LC607` | `__InitMatrices__Fv` |
| `803DE3E4` | 4 | `$LC626` | `eSetOrthographicScreenQuadProjection__FP13eRenderTarget` |
| `803DE43C` | 4 | `$LC645` | `eScreenQuadReplace__FP5eViewi` |
| `803DE460` | 28 | `$LC651` | `eMotionBlurEffect__FP5eView` |
| `803DE484` | 4 | `$LC652` | `eMotionBlurEffect__FP5eView` |
| `803DE4AC` | 84 | `$LC661` | `eRenderSky__FP5eView` |
| `803DE52E` | 2 | `$LC667` | `eProgressiveScan_EURGB60Proceed__Fi` |
| `803DE545` | 3 | `$LC668` | `eProgressiveScan_EURGB60Proceed__Fi` |
| `803DE58A` | 2 | `$LC670` | `eProgressiveScan_EURGB60Proceed__Fi` |
| `803DE59B` | 1 | `$LC671` | `eProgressiveScan_EURGB60Proceed__Fi` |
| `803DE5AF` | 1 | `$LC672` | `eProgressiveScan_EURGB60Proceed__Fi` |
| `803DE5E7` | 1 | `$LC675` | `eNTSCInterlace_PALProceed__Fi` |
| `803DE60B` | 1 | `$LC677` | `eNTSCInterlace_PALProceed__Fi` |
| `803DE627` | 1 | `$LC679` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE65D` | 3 | `$LC681` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE667` | 1 | `$LC683` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE6AB` | 1 | `$LC686` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE6C6` | 2 | `$LC687` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE6E3` | 1 | `$LC688` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE72B` | 1 | `$LC691` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE737` | 1 | `$LC692` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE74E` | 2 | `$LC693` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE783` | 1 | `$LC695` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE7E1` | 3 | `$LC698` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE7EF` | 1 | `$LC699` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE7FB` | 1 | `$LC700` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE88D` | 3 | `$LC706` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE89B` | 1 | `$LC707` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE8A7` | 1 | `$LC708` | `eProgressiveScan_EURGB60DialogBox__Fi` |
| `803DE8AA` | 14 | `$LC709` | `eDEMOInitROMFont__Fv` |
| `803DE8D6` | 2 | `$LC710` | `eDEMOInitROMFont__Fv` |
| `803DE91C` | 4 | `$LC714` | `eDEMOBeforeRender__Fv` |
| `803DE934` | 4 | `$LC718` | `eDEMOSetupScrnSpc__Fllf` |
| `803DE944` | 412 | `$LC729` | `GenerateHorizonFogDisplayList__FPPvPUl9_GXVtxFmt` |
| `803DEB0F` | 1 | `$LC730` | `GenerateHorizonFogDisplayList__FPPvPUl9_GXVtxFmt` |
| `803DEB2C` | 4 | `$LC725` | `GenerateHorizonFogDisplayList__FPPvPUl9_GXVtxFmt` |
| `803DEB40` | 32 | `$LC732` | `__10cSphereMap` |
| `803DEB74` | 20 | `$LC737` | `genSphere__10cSphereMapPPvPUlUs9_GXVtxFmt` |
| `803DEBD4` | 12 | `$LC754` | `eSetFogConstantColour__Fv` |
| `803DEC14` | 52 | `$LC804` | `InitSlotPoolsEx__Fv` |
| `803DEC5F` | 1 | `$LC805` | `InitSlotPoolsEx__Fv` |
| `803DEC84` | 48 | `$LC814` | `epCalculateLocalDirectionalPOS16__FPUiT0iPUsPiPUciiP14eLightMaterialP13eLightContext` |
| `803DECC8` | 4 | `$LC821` | `UpdatePlatInfo__27eLightMaterialPlatInterface` |
| `803DECD4` | 4 | `$LC820` | `UpdatePlatInfo__27eLightMaterialPlatInterface` |
| `803DED2F` | 1 | `$LC835` | `UpdateCameras__7eEnvMapP8bVector3T1` |
| `803DEDA4` | 4 | `$LC863` | `eRotateX__FP8bMatrix4T0Us` |
| `803DEDC4` | 4 | `$LC869` | `eRotateY__FP8bMatrix4T0Us` |
| `803DEDFC` | 8 | `$LC881` | `eInvertRotationMatrix__FP8bMatrix4T0` |
| `803DEE08` | 908 | `$LC882` | `GetPlaneState__FPC8bVector4PC8bVector3` |
| `803DF1B0` | 60 | `$LC891` | `RenderMWVisualLook__20IVisualTreatmentPlatP5eView` |
| `803DF204` | 36 | `$LC896` | `ExpandVector__FPC9smVector3P8bVector3` |
| `803DF22C` | 4 | `$LC897` | `ExpandVector__FPC9smVector3P8bVector3` |
| `803DF265` | 3 | `$LC906` | `InitEmitterSlotPool__Fv` |
| `803DF28D` | 3 | `$LC908` | `ServiceWorldEffects__13EmitterSystem` |
| `803DF29C` | 4 | `$LC911` | `GetFloatColor__FUiR8bVector4` |
| `803DF2EC` | 4 | `$LC929` | `GetAnimatedUVs__F23EffectParticleAnimationiPUiT2` |
| `803DF2FC` | 4 | `$LC931` | `GetAnimatedUVs__F23EffectParticleAnimationiPUiT2` |
| `803DF364` | 4 | `$LC953` | `PlatRotateScaleParticle__FP15EmitterParticleRQ25UMath7Vector3N41` |
| `803DF37C` | 4 | `$LC958` | `UpdateTriggers__Fv` |
| `803DF3C4` | 4 | `$LC974` | `SpawnParticles__7Emitterff` |
| `803DF3D4` | 100 | `$LC976` | `__static_initialization_and_destruction_0` |
| `803DF474` | 4 | `$LC992` | `__static_initialization_and_destruction_0` |
| `803DF48C` | 4 | `$LC998` | `__static_initialization_and_destruction_0` |
| `803DF4B4` | 4 | `$LC1008` | `__static_initialization_and_destruction_0` |
| `803DF4FC` | 4 | `$LC1025` | `__static_initialization_and_destruction_0` |
| `803DF51C` | 20 | `$LC1033` | `Render__11eDataRenderP11TextureInfo` |
| `803DF534` | 4 | `$LC1034` | `Render__11eDataRenderP11TextureInfo` |
| `803DF554` | 4 | `$LC1040` | `Render__11eDataRenderP11TextureInfo` |
