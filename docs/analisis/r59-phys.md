# r59 · `phys` — zGameplay: 110.769 → **35.438 B** de DOL, un −68 % por ORDEN DE FUENTE

**Cuatro ficheros reordenados, cero instrucciones tocadas**, `fncmp` **0/768 antes y
despues**, `linkdelta` **`.text +0 · IGUAL`** al empezar y al acabar. Lo que queda deja de ser
«orden de funciones» y pasa a ser **orden del pool de `.rodata`** — el mismo frente que
zPhysics.

    zGameplay  b26b73d007b19d9fcda37469181a8bd7e7f431ff   TRES compilaciones iguales
    zPhysics   db96e33dad5f947f734749450531e565e70ae72b   (sin tocar, fncmp 0/718)
    zMisc      da892bbf6d8dced6050df7d698a8ac60dea20e2e   (sin tocar, fncmp 0/450)

    dolwhere  zGameplay  110.769 -> 35.438 B     (-75.331)
    dolwhere  zPhysics    35.673 -> 35.673 B     (sin tocar)
    linkdelta zMisc       rodata+2840 data+32    (sin tocar)
    trypromo Speed/Indep/SourceLists/zGameplay -> DOL ROTO (2320424c272e)
    lcfix.py --check: 242 pendientes, **3 mias y OBLIGATORIAS** (seccion 6)

**AVISO DE MEDIDA**: la partida de 110.769 B **no es la del informe de la r58** (96.137). La
he vuelto a medir HOY, con el arbol de hoy y `git stash` de mis cinco ficheros, para que los
dos extremos salgan del mismo arbol. Los −75.331 B son la resta de esas dos medidas.

---

## 1. La palanca: el orden de las funciones ES el orden de la fuente

`permorden` decia 549/767 en su sitio con 218 desplazadas y 15 ciclos. La lectura correcta no
era «hay que aplicar el mecanismo 1 en dos cabeceras»: era **`.cpp` por `.cpp`, el objetivo
emite las funciones en un orden que nuestra fuente tiene BARAJADO**, y basta escribirlas en
el orden del objetivo.

La metrica que lo hace iterable no es `permorden` (que mide orden RELATIVO) sino
**cuantas funciones caen en su desplazamiento EXACTO dentro del objeto**, con su primera
desplazada. Cuesta 0 s, no hay que enlazar, y da un escalon por edicion:

| paso | offset exacto | primera desplazada | DOL |
|---|---:|---|---:|
| partida | 161/767 | idx 161 `__13GRaceDatabase` (+3.908) | 110.769 |
| 1. los seis accesores de `GRaceStatus` al final del TU | 208/767 | idx 161 (+3.852) | — |
| 2. `GRaceDatabase.cpp` reordenado (173 bloques) | 379/767 | idx 342 `IsNoLongerUseful` | — |
| 3. `GCharacter.cpp` (4 bloques) | 383/767 | idx 358 `_._8GManager` | — |
| 4. `GManager.cpp` (149 bloques) | **528/767** | idx 529 (**falso**, ver §4) | **35.438** |

`permorden` final: **730 de 767 en su sitio**, 37 desplazadas, 9 ciclos (era 549 / 218 / 15).

---

## 2. El paso 1 es la PUERTA, y no vale hacerlo «en clase»

Los seis cuerpos de `GRaceStatus::SetIsLoading / EnterSuddenDeath / SetTaskTime /
SetActivelyRacing / SetHasBeenWon / GetCacheName` salian en los puestos **161..166** y el
objetivo los tiene en el **728..733**. Mientras estuvieran ahi, **ninguna** funcion posterior
al puesto 160 podia caer en su sitio: son 56 B metidos delante de 98.256.

El puesto 728 esta **detras de `__static_initialization_and_destruction_0`** (puesto 707), o
sea dentro de la cola de `finish_file`. La lectura obvia —«cuerpo EN CLASE = va a la COLA»—
**no se puede aplicar aqui**, y esta medido por que:

- una inline de clase sin `#pragma interface` sale COMDAT, y `decl2.c:3783-3790` solo le
  quita el `DECL_EXTERNAL` si `TREE_SYMBOL_REFERENCED || !DECL_COMDAT`;
- **ninguna de las seis se llama dentro de zGameplay** (`grep` sobre `Gameplay/`), asi que
  `TREE_SYMBOL_REFERENCED` es falso y **cc1plus no emitiria simbolo**: la unidad no enlazaria.
- Y `#pragma interface` queda descartado con prueba: emitiria TODAS las inlines de
  `GRaceStatus.h`, y el objetivo **no tiene** `GetHasBeenWon`, `GetIsTimeLimited` ni
  `SetWarpWhenInFreeRoam`.

Lo que si funciona --y es lo que he hecho-- es **escribirlas al final de `zGameplay.cpp`**,
detras del ultimo `#include`: es el ultimo punto de parseo del TU, lo mas tarde que una
definicion FUERA de clase puede salir. Cuesta que la cola (puestos 708..767, **4.116 B**) se
desplace 56 B, y esa cola ya estaba descolocada entera.

**Contraste util** (misma cabecera, otro resultado): `GRacerInfo::ClearAll` SI es una inline
de clase (`GRaceStatus.h:264`) y SI se emite, porque `GRaceStatus.cpp:500` y `:527` la
llaman. Esa es la diferencia exacta entre las dos formas.

---

## 3. Reordenar un `.cpp` es mecanico, pero tiene DOS anclas que no se pueden mover

El reordenado es una permutacion pura de bloques (el fichero resultante tiene **el mismo
multiconjunto de lineas y el mismo numero de bytes**; lo compruebo en cada pasada). Lo que
NO es mecanico son las definiciones cuya POSICION cambia el codigo:

**(a) las `inline` fuera de clase.** `GRaceParameters::EnsureLoaded` (`GRaceDatabase.cpp`)
la llaman 75 funciones. Las que estan ANTES de su definicion emiten `bl`; las que estan
DESPUES la expanden. Mi primera pasada la mando al final del fichero y **21 funciones
perdieron 56-60 B cada una** (`fncmp` 21/768, 5.128 B). El corte del objetivo sale **limpio y
comprobable**:

    llamantes con indice de objetivo <= 249  ->  TODOS emitian `bl`   (estaban antes)
    llamantes con indice de objetivo >= 261  ->  TODOS la expandian   (estaban despues)

o sea `EnsureLoaded` va **entre el puesto 249 y el 261** del orden del objetivo. La puse en
249,5 (justo delante de `BlockUntilLoaded`, que es donde estaba en la fuente vieja) y
`fncmp` volvio a 0/768. **Es una prueba independiente de que el orden del objetivo es el
orden de la fuente original**: el corte de inline/no-inline cae en un hueco del orden.

**(b) las plantillas y los ayudantes `static inline`.** En `GRaceDatabase.cpp`, el bloque
`template <typename T> GRaceCustom::SetAttribute` + sus dos instanciaciones explicitas va
delante del primer llamante (puesto 296): clave 295,5. En `GManager.cpp`,
`static inline bConvertFromBond` delante de sus dos llamantes (puestos 454 y 461): clave
453,5; `static inline AlignPointer` delante de los suyos (380 y 381): clave 379,5. Y las dos
funciones LIBRES `NotifyGameZonesChanged` / `NotifyTrackMarkersChanged`, que el troceador por
`Clase::Metodo` no ve, hay que partirlas a mano: son los puestos 448 y 449.

---

## 4. La metrica del OBJETO satura, y dice una mentira concreta

A partir de 528/767 la «primera desplazada» es `Unload__6GVault` con **delta 4**, y el
culpable es `LoadAsyncTransient__6GVault` (4 B) — **una funcion que solo definimos nosotros y
que el enlazador estripa**. En el objeto ocupa sitio; en el enlace no. Lo mismo
`GetObjectBlock__C6GVault` (8 B). Son 194 simbolos asi en zGameplay.

O sea: **`offset exacto` sirve para pilotar las primeras ediciones y deja de servir en
cuanto aparece un simbolo solo-nuestro.** A partir de ahi hay que enlazar.

---

## 5. Lo que le queda a zGameplay, desglosado y con nombre

`dolwhere` con el `keep` corregido: **35.438 B**, y no son 35.438 problemas.

| bloque | B | que es |
|---|---:|---|
| cola diferida, puestos 590..767 | **29.124** | el orden de `instantiate_pending_templates` + `saved_inlines` |
| `.rodata` | **5.108** | el orden del pool de cadenas muertas |
| inmediatos de `.text` (635 rangos de 1-2 B) | 1.141 | consecuencia del anterior: mitades `@ha`/`@l` del pool |
| `.data` | 352 | el limite de `finish_file` ya documentado en la r58 §5 |

**Aviso**: `dolwhere` imputa 19.701 B a `SetAttribute__H1Zb_11GRaceCustom` y 8.236 a
`SetIsLoading__11GRaceStatusb`. **No son de esas funciones**: son los dos huecos de la cola
que empiezan detras de ellas (`nfsmw-huecos-leer-el-dol`). La suma de tamanos del objetivo
para los puestos 590..767 es 29.124 B, que es exactamente lo que suman esas dos imputaciones.

### 5.1 El pool de `.rodata` se lee de un golpe, y las tres unidades tienen la MISMA forma

Comparando las cadenas de los dos objetos **en orden**:

| unidad | cadenas correctas desde el byte 0 | primera que falla |
|---|---|---|
| zGameplay | 5 (hasta `%f,%f,%f,%f`) | la 5, `Attrib::Attribute`, **desfasada 0x18** |
| zPhysics | **21** (hasta `SmackTrigger`) | la 21, `Attrib::Gen::pvehicle`, **desfasada 8** |

- En **zGameplay** el desfase de 0x18 son los **16 B que `keep.lst` salva de mas**: `$LC3`
  ("bad_alloc") y `$LC59` ("GAMECUBE") estan DUPLICADOS con el prefijo `lbl_803EBE98` que
  escribimos a mano. Es la propuesta 1 de la r58, y ahora esta **medida**: quitar las dos
  entradas a secas deja `rodata-16` y el DOL sube a **214.677 B**. Es un paquete atomico, no
  una linea.
- En **zPhysics** el desfase de 8 son **dos flotantes que el objetivo tiene y nosotros no**
  (`3F800000 00000000`) entre `SmackTrigger` y `Attrib::Gen::pvehicle`.
- Detras del desfase, en las dos, el ORDEN tambien difiere: son las cadenas de
  `USE_ATTRIB_ALLOC("Attrib::Gen::xxx")` (cabeceras generadas de AttribSys) y de
  `HAND_POOL_TAG(...)`, todas MUERTAS. Su posicion la decide **el orden en que se parsea la
  cabecera** (el cuerpo inline genera RTL al parsearse, y ahi se crea el `$LC`), o sea es el
  mismo eje que mide `parseord.py` para las vtables.

---

## 6. `lcfix`: 242 pendientes, **3 mias, y las tres son OBLIGATORIAS**

    CORRIGE linea 2069: zGameplay.o:$LC596 -> $LC595     ("Attrib::Gen::speech")
    CORRIGE linea 2533: zGameplay.o:$LC581 -> $LC580     ("MSetTrafficSpeed")
    CORRIGE linea 2535: zGameplay.o:$LC598 -> $LC597     ("Attrib::Gen::milestonetypes")

Las tres son `-1` y las causa mover `GetCacheName` al final del TU (su literal "GRaceStatus"
se crea mas tarde y corre un puesto a las de detras).

**Ninguna de las 242 es VENENOSA para mis unidades** — ninguna otra toca zGameplay, zPhysics
ni zMisc. Pero **estas tres no son opcionales**: sin ellas mi trabajo sale NEGATIVO.

    con las 3 correcciones:  linkdelta IGUAL      dolwhere  35.438 B   trypromo 2320424c272e
    con el keep.lst del repo: linkdelta rodata-40  dolwhere  NO ALINEA  trypromo e3c4d4066ec3

He medido con un `keep.lst` **privado en el scratchpad** (`trypromo --ldflags`, y un envoltorio
de seis lineas para `linkdelta`/`dolwhere`, que no lo aceptan). **`config/GOWE69/keep.lst`
esta intacto**: `sha1 5547328dd54b6083dd2e73bf5ff1ceeb14657d7d`, igual que al empezar.

---

## 7. Regresiones

**Ninguna.** `fncmp` 0/768, 0/718 y 0/450 en las tres unidades, antes y despues.

Los cinco ficheros tocados —`SourceLists/zGameplay.cpp`, `Gameplay/GRaceStatus.cpp`,
`GRaceDatabase.cpp`, `GManager.cpp`, `GCharacter.cpp` y la nota en `GActivity.cpp`— **los
incluye solo `zGameplay.cpp`** (comprobado con `grep -rln` sobre todo `src/`; la unica otra
aparicion de "GManager.cpp" en el arbol es un COMENTARIO en `Frontend/FEPackageData.cpp`).
**No he tocado ninguna cabecera**, ni `config/GOWE69/*`, ni `splits.txt`, ni `keep.lst`.

zPhysics y zMisc: no tocadas. Su `sha1` **si cambio** respecto al de la r58
(`b88fe7fe`/`bc161c7d`), pero **no por mi**: durante la ventana otros agentes editaron
cabeceras compartidas (`Interfaces/SimModels/IModel.h`, `World/WorldConn.h`,
`Gameplay/GState.h`). Comprobado que **no les mueve el DOL**: zPhysics sigue en 35.673 B
exactos y zMisc en `rodata+2840 data+32`.

---

## 8. Un negativo medido, y donde esta escrito

`#define DEAD_STR_DONE(s) NULL` en `zGameplay.cpp` (usando la guarda inerte que `r59-resto`
acaba de poner en `GState.h:26`) **si** quita el literal muerto `"done"` de la cabeza del
pool —`Attrib::Gen::simsurface` baja de 0x16C a 0x164, los 8 B justos— pero **rompe
`GActivity::SerializeVars`**: 656 → 652 B, `fncmp` 1/768. El `"done"` de
`GState::IsTerminalState` y el de `SerializeVars` son **el mismo literal fundido**; al quitar
el primero cc1plus lo vuelve a crear dentro de la segunda y cambia 4 B de codigo. Revertido.
El negativo esta escrito **junto a `GActivity::SerializeVars`**, que es donde `previo.py` lo
encuentra.

La veta sigue viva: hace falta una forma que **no cambie el valor** —una etiqueta dentro del
`asm()` del pool (`extern const char lbl_XXXX[];`) en vez de `NULL`—, que es la misma receta
que la r58 §6 propone para zMisc.

---

## 9. Propuestas (no las aplico)

1. **Aplicar las tres correcciones de `lcfix` de zGameplay** (§6). Sin ellas mis −75.331 B
   son un +104.000. Es la unica dependencia de mi trabajo fuera de mis ficheros.
2. **`reord.py` merece subir a `scripts/`**: trocea un `.cpp` en definiciones de nivel
   superior, las empareja con los simbolos de nuestro `.o` por nombre desmangIado y orden de
   aparicion, y las reescribe en el orden del OBJETIVO, comprobando que la salida es una
   permutacion pura (mismo multiconjunto de lineas). Con `--split linea=clave` para las
   anclas de §3. Ha valido las tres ediciones grandes de esta ronda y corre en 1 s.
3. **`rodseq.py` tambien**: lista las cadenas de `.rodata` de los dos objetos EN ORDEN, lado
   a lado, y dice cuantas van bien desde el byte 0 y cual es la primera que falla. Es lo que
   convirtio «5.108 B de `.rodata`» en «zPhysics tiene 21 cadenas exactas y le faltan 8 B de
   flotantes en la 21». Las dos estan en el scratchpad con su docstring.
4. **`offset exacto`** (funciones que caen en su desplazamiento exacto del objeto, con la
   primera desplazada) deberia ser una columna de `permorden`: es la metrica que pilota este
   frente, cuesta 0 s y no hay que enlazar. Con el aviso de §4.
5. El paquete atomico de `keep.lst` de zGameplay (quitar `$LC3`/`$LC59` **y** emitir los
   16 B que faltan) sigue sin resolver: medido que por separado cuesta **179.239 B**.

---

## 10. Sorpresas

1. **`scripts/lcfix.py` NO tiene guarda `if __name__ == '__main__'`**: la llamada a `main()`
   esta en la linea 173, a nivel de modulo. **Importarlo lo EJECUTA y reescribe
   `config/GOWE69/keep.lst`.** Me paso al intentar reusar su lector de `.rodata`; lo revert
   con `git checkout` en el acto (58 lineas cambiadas). Con agentes en paralelo esto rompe
   el enlace de todos en silencio.
2. **El corte inline/no-inline de `EnsureLoaded` cae en un HUECO del orden del objetivo**
   (llamantes <=249 emiten `bl`, >=261 la expanden, y no hay ni uno en medio). Es una
   verificacion independiente, y gratis, de que el orden del objetivo es el orden de la
   fuente original.
3. **El orden de las funciones y el orden del pool son EJES DISTINTOS.** He puesto 730 de 767
   funciones en su sitio y el pool de `.rodata` sigue exactamente igual de mal (5.108 B): las
   cadenas muertas de `USE_ATTRIB_ALLOC` no las crea la funcion que las usa, sino el parseo de
   la cabecera.
4. **zGameplay y zPhysics estaban rotas por lo mismo, no por cosas distintas.** La r58
   escribio que zGameplay era «funciones permutadas» y zPhysics «pool»: era verdad de la
   primera capa. Debajo de las funciones de zGameplay hay el mismo pool, con la misma forma
   (21 cadenas exactas en zPhysics, 5 en zGameplay) y magnitudes casi iguales (35.438 contra
   35.673).
5. **La metrica del objeto miente en cuanto hay un simbolo solo-nuestro** (§4): `delta 4` que
   en el enlace es cero.
6. **`git stash` avisa de CRLF y no pasa nada**: el arbol se saca con CRLF (`core.eol` nativo)
   pero `.gitattributes` lleva `text=auto`, asi que el indice es LF y mis ficheros reescritos
   en LF dan un diff limpio de contenido.
7. **Un comentario cambia el `sha1` del `.o`** (linea de depuracion), no el `.text`. Hay que
   volver a sellar despues de escribir un negativo, no antes.
