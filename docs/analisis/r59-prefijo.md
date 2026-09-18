# r59 — `prefijo`: las once candidatas contadas, siete atacadas, **dos nuevas a IGUAL**

Agente `prefijo`. **Siete ficheros tocados, los siete `SourceLists` de mis unidades.** Ni una
cabecera, ni `config/`, ni `splits.txt`, ni `keep.lst`, ni `configure.py`.

    zWorld              zTrack              zWorld2             zEagl4Anim
    zPlatform           zCamera             zPhysicsBehaviors

---

## 0. Titular

| unidad | ANTES (hoy, con `lcfix` privado) | DESPUES | promociona |
|---|---|---|---|
| **`zWorld`** | `rodata-472 data-128` | **IGUAL en las nueve** | **candidata NUEVA** |
| **`zTrack`** | `rodata-352 data-416 bss-160` | **IGUAL en las nueve** | **candidata NUEVA** |
| `zPlatform` | `text-8 rodata-728 data-64 bss-32` | `text-8` | falta código |
| `zEagl4Anim` | `text+4 rodata-640 data-96 bss-128` | `text+4` | falta código |
| `zCamera` | `text+16 rodata-1384 data-224` | `text+16` | falta código |
| `zPhysicsBehaviors` | `text+4 rodata-40 data-224 bss-348` | `text+4 bss+4` | falta código |
| `zWorld2` | `rodata-408 data-256 bss+64` | `bss+64` | falta quitar 64 B de `.bss` |

**Las siete tienen hoy `.rodata`, `.data`, `.sdata`, `.sdata2`, `.sbss`, `.ctors`, `.dtors` y
`.over` a delta CERO.** Lo único que queda en cinco de ellas es `.text` —código— y en una,
64 B de `.bss` de más.

**1.351.842 B del DOL dejan de diferir**, medidos unidad a unidad contra el DOL original:

| unidad | bytes distintos ANTES | DESPUES | delta |
|---|---:|---:|---:|
| `zEagl4Anim` | 3.324.340 | 2.961.879 | **−362.461** |
| `zPlatform` | 1.483.974 | 1.277.099 | **−206.875** |
| `zTrack` | 249.139 | 57.446 | **−191.693** |
| `zWorld` | 383.938 | 198.523 | **−185.415** |
| `zWorld2` | 319.172 | 141.922 | **−177.250** |
| `zPhysicsBehaviors` | 1.607.212 | 1.471.722 | **−135.490** |
| `zCamera` | 3.462.011 | 3.369.353 | **−92.658** |
| | | | **−1.351.842** |

Y **`dolwhere` vuelve a arrancar en dos de ellas**: ANTES se callaba en las siete
(`LAS SECCIONES NO COINCIDEN`), y hoy da `zWorld` **245.700 B** y `zTrack` **87.702 B**
—en rangos, que es lo que cuenta `dolwhere`—. En las otras cinco sigue callándose, pero ya
sólo por `.text` (o por el `.bss` de `zWorld2`), no por dato.

`fncmp` **idéntico antes y después en las siete**, corrido después de CADA fichero:
`zWorld` 4/582, `zWorld2` 2/357, `zTrack` 1/259, `zEagl4Anim` 2/318, `zPlatform` 1/136,
`zCamera` 3/453, `zPhysicsBehaviors` 1/1120. **Cero regresiones.**

`trypromo` al cerrar: **`DOL ROTO` en las siete**, con estos hashes
(`41a3d8d86baf` / `a38849185f1e` / `2bfcf3c13e0b` / `6e7d73d9c950` / `9171a9892344` /
`32cbeb7338d9` / `ceda72dd026f`).

Escrito en total: **4.024 B de `.rodata`, 1.408 B de `.data` y 696 B de `.bss`**, todo
VERBATIM del ELF original salvo el relleno anónimo de `.bss` (§3).

---

## 1. El censo: la lista de la r58 era buena, y son SIETE mías, no once

`r58-feov.md` §7.4 dejó once candidatas con `rodata-N`. Cuatro (`zAI`, `zLua`, `zSpeech`,
`zGameplay`) tienen agente esta ronda. Las siete restantes son las de arriba, y **la cifra de
`N` de la r58 se reprodujo al byte en las siete** con el `keep.lst` corregido.

De propina, el censo completo de lo que no tiene agente:

| unidad | hoy | qué es |
|---|---|---|
| `criticalpath`, `zGameModes`, `zEAXSound2` | **IGUAL** | ya no les falta sección: sólo colocación |
| `zEAXSound` | `bss+32` | sobran 32 B de `.bss`, no se arregla añadiendo |
| `steering` | `text-8 bss+32` | código (Metrowerks) |
| `madidct` | `text-20` | vetada por compilador ausente |
| `inittmr` | `rodata-144` | **la receta NO le vale**: ver §5 |

---

## 2. La receta del bloque de cabecera: siete de siete, y cuesta minutos

Las siete tenían ya el prefijo de bWare/STL de 0x5C escrito a mano y **las siete lo tienen
nombrado en `keep.lst`**, que protege por NOMBRE y no por TAMAÑO. Alargar ese `asm()` con los
bytes verbatim que el objetivo tiene justo detrás cierra el déficit sin tocar `keep.lst`, sin
añadir ni quitar un `$LC` y sin mover una instrucción:

| unidad | símbolo | 0x5C -> | bytes | de |
|---|---|---|---:|---|
| `zPhysicsBehaviors` | `lbl_803FA7B0` | 0x84 | 40 | `803FA80C` |
| `zTrack` | `lbl_80408FB8` | 0x1BC | 352 | `80409014` |
| `zWorld2` | `lbl_8040DFE8` | 0x1F4 | 408 | `8040E044` |
| `zWorld` | `lbl_80409D58` | 0x234 | 472 | `80409DB4` |
| `zEagl4Anim` | `lbl_803D3FE8` | 0x2DC | 640 | `803D4044` |
| `zPlatform` | `lbl_80402108` | 0x334 | 728 | `80402164` |
| `zCamera` | `lbl_803D1928` | 0x5C4 | 1384 | `803D1984` |

**Acerté `N` al byte en las siete y no hizo falta la ventana de 32**: los bytes están en el
ELF original, así que no hay nada que estimar.

Y la ventana **no es `[N, N+31]`**, ni simétrica: el arranque de `.data` es `align32` del
final del CONTENIDO de `.rodata`, y ese contenido no acaba en un múltiplo de 32. Medido en
este DOL: `.rodata` mide `0x4C50C` desde `803C8C60`, o sea que **acaba en `8041516C` y ya
lleva 20 B de relleno** hasta el `80415180` de `.data`. Así que la ventana real es
**`[N-11, N+20]`**: te puedes pasar 20 y quedarte corto 11, y **cualquier otra cosa mueve
`.data` 32 B enteros**. La regla general: la holgura se calcula del final de contenido real,
no se supone en ±31. (La de `.bss` -> `.sdata` es la misma cuenta y da 20 B por arriba
también: el `.bss` acaba en `804FEDAC` y `.sdata` arranca en `804FEDC0`; es de lo que vive
el `bss+4` de `zPhysicsBehaviors`.)

**Cero `$LC` renumerados**: lo verifiqué comparando las tablas de símbolos de `zPlatform`
antes y después (166 `$LC` los dos, mismos nombres, sólo desplazados 728). El recuento de
`lcfix` de mis unidades es **exactamente el mismo antes y después de mis siete ediciones**.

---

## 3. La segunda palanca: `keep.lst` ya tenía la `.data` y la `.bss` pedidas

La sorpresa 2 de `r58-resto` («`gap_*`/`pad_*`/`lbl_*` en `keep.lst` es una lista de la compra
sin cobrar») **paga también aquí, y es lo que ha llevado a `zWorld` y `zTrack` a IGUAL**. En
mis siete unidades hay **172 entradas** `<u>.o:gap_*|pad_*|lbl_*` en `keep.lst`; **55** de
ellas nombraban un símbolo que el objetivo define y nuestro objeto no. Son las 55 que he
escrito.

Escritos como `asm()` de ámbito de fichero, con `.globl`/`.type`/`.size` —que es lo que
`keep.lst` necesita para salvarlos— y con el contenido leído del ELF original:

| unidad | `.data` escrita | símbolos | resultado |
|---|---:|---|---|
| `zWorld` | 128 | 15 (`gap_06_8043724C` … `gap_06_80438F18`) | `data-128` -> **IGUAL** |
| `zTrack` | 416 | 15 | `data-416` -> `+0` |
| `zWorld2` | 256 | 6 | `data-256` -> `+0` |
| `zCamera` | 224 | 9 | `data-224` -> `+0` |
| `zPhysicsBehaviors` | 224 | 3 | `data-224` -> `+0` |
| `zEagl4Anim` | 96 | 3 | `data-96` -> `+0` |
| `zPlatform` | 64 | 4 | `data-64` -> `+0` |

**Ni uno de los 55 lleva un puntero** (lo comprobé leyendo los bytes: son ceros y constantes
de coma flotante — `1.0f` en `lbl_80438AA4`, `{0, 0x36B04000, 2}` en `lbl_80437614`,
`{0.62f, 0, 1.0f, 0.58f, 229.0f}` en `lbl_804370EC`), así que no hay reubicación que perder.

### 3.1 Cuando la suma no cuadra, se TRUNCA el símbolo, no se inventa

`keep.lst` protege por NOMBRE, así que el tamaño lo pongo yo. En `zCamera` los nueve símbolos
suman 251 y hacían falta 224; en `zPhysicsBehaviors` suman 248 y hacían falta 224. Escribí el
último de cada lista **recortado** (`gap_06_80417085_data` con 40 B de sus 67,
`gap_06_8041F6A0_data` con 112 de sus 136), con sus bytes verbatim desde el principio. **Es
andamio declarado**: el día que se escriban las estáticas de función que faltan hay que
encogerlos lo mismo.

### 3.2 EL HALLAZGO DE LA RONDA: el `.bss` no viaja en el DOL, y su relleno va ANÓNIMO

`.bss` es `NOBITS`: **su CONTENIDO no está en el DOL**. Lo único que hace es empujar
`.sdata`/`.sbss`/`.sdata2`, que sí viajan. O sea que para cerrar un `bss-N` **da igual qué
bytes se pongan**: sólo cuenta el tamaño.

Y hay una forma que no cuesta ni un símbolo ni una línea de `keep.lst`:

```c
asm(".section .bss\n"
    "  .space 160\n"
    ".previous\n");
```

**Sin símbolo, `-strip-unused-data` no lo puede nombrar y no se lo lleva.** (Es la misma razón
que `inittmr.cpp` documenta para su `.sdata` desde hace rondas; aquí queda medido para `.bss`.)

**OJO, y me costó una medida**: `.balign 8` delante del `.space` **sube la alineación de la
sección** y el enlazador añade **32 B de más**. Con `.balign 8` + `.space 160`, `zTrack` pasó
de `bss-160` a `bss+32`; **sin el `.balign`**, a `IGUAL`. No poner `.balign` en este bloque.

Aplicado: `zTrack` 160, `zEagl4Anim` 128, `zPlatform` 32, `zPhysicsBehaviors` 376.

### 3.3 El `.bss` de `zPhysicsBehaviors` está CUANTIZADO en 32, y `+4` ya está bien

En `zPhysicsBehaviors` el `.space` no es lineal: **348 da `bss-28` y 376 da `bss+4`** —28 B de
fuente mueven 32 B de sección—, así que **el `0` no es alcanzable**. Da igual: la holgura del
`align32` de `.sdata` es de 20 B (el `.bss` del objetivo acaba en `804FEDAC` y `.sdata`
arranca en `804FEDC0`), y con `bss+4` **`.sdata` y `.sdata2` caen en su dirección exacta**,
medido. Es el caso simétrico del aviso del dossier: aquí **un `+4` SÍ está bien**.

---

## 4. TRES trampas de medida nuevas, las tres caras

### 4.1 **Un `.o` RENOMBRADO pierde TODA su protección de `keep.lst`**

`keep.lst` empareja por `<basename>.o:<símbolo>`. Medí el «antes» compilando a
`<scratchpad>/zPlatform.antes.o` y el enlace salió con **`data-89856`**: el que desapareció
era `bin_globala_bun`, 89.716 B que `keep.lst` salva con la línea `zPlatform.o:bin_globala_bun`
—y que dejó de casar en cuanto el fichero se llamó `zPlatform.antes.o`—. Con él se fueron
además 30 `$LC` y el `lbl_80402108`.

**Esto envenena la regla 5 del encargo** («compila la unidad ajena a un `.o` PRIVADO»): el
`.o` privado tiene que **conservar el basename y cambiar sólo el DIRECTORIO**. Renombrarlo da
un «antes» catastrófico y totalmente falso, sin un solo error.

### 4.2 El `keep.lst` rancio **y los objetos rancios** se tapan mutuamente

Al empezar, `lcfix --check` daba **58** correcciones (zWorld/zWorld2). Recompilé mis siete
unidades **sin tocar una línea** y pasaron a **122** (zWorld 39, zPhysicsBehaviors 30,
zCamera 21, zWorld2 19, zPlatform 13). Los `.o` del disco eran de un árbol anterior y
**`lcfix` sólo ve el desfase contra el objeto que hay**.

Con `keep.lst` rancio, cuatro de mis unidades leían un `rodata-N` fantasma **hoy, ya con el
bloque puesto**: `zWorld` `-408`, `zPhysicsBehaviors` `-256`, `zPlatform` `-216`, `zCamera`
`-184`, `zWorld2` `-208`. Con `lcfix` aplicado: **`+0` las cinco**. La regla operativa es
**recompilar ANTES de correr `lcfix --check`**, y volver a correrlo después de cada tanda.

### 4.3 El `sha1` del `.o` no es un sello — **el hash del DOL sí**

Con seis agentes trabajando, `zPhysicsBehaviors`, `zWorld`, `zPlatform` y `zCamera` **oscilan
entre dos `sha1` distintos compilando la MISMA fuente** (pasada 1 y 2 una, pasada 3 otra):
otros agentes editan y revierten cabeceras compartidas (`Generated/Messages/M*.h`,
`Generated/Events/*.cpp`, `EventSequencer.cpp`) en mitad de mi compilación.

**Pero los siete hashes de DOL de `trypromo` son idénticos en todas las pasadas**, y las
secciones también. O sea: el vaivén de cabeceras **no cambia el enlace** de mis unidades.
Confirma la sorpresa 5 de `r58-resto` y la convierte en regla: **cantar promoción con el
`sha1` del `.o` es imposible en una ronda concurrida; el sello es el hash del DOL** (y a
falta de él, el delta de secciones, que aquí se reprodujo en tres árboles distintos).

**El sello bueno, y éste SÍ se ha repetido en cuatro árboles distintos a lo largo de la
ronda** (hash del DOL que produce `trypromo` con cada unidad sustituida):

    zWorld             41a3d8d86baf2790a497fd13f85a9b969d131908
    zWorld2            a38849185f1ef621dccbaa8aeb4728918d0be742
    zTrack             2bfcf3c13e0be4aa3c8a00f75231d2de2de91919
    zEagl4Anim         6e7d73d9c95035fad0ffb8e78548cb19b4272a84
    zPlatform          9171a98923448b9a92cb1138da8124aa0b249ce9
    zCamera            32cbeb7338d9f00dade53fa36aaea2b8fa711210
    zPhysicsBehaviors  ceda72dd026f21c915a9ab154229d58ee0ef8a5e

Sellos del `.o` del último árbol coherente (`build/GOWE69/src/.../<u>.o`), con la advertencia
de arriba —el de `zEagl4Anim` cambió de `4f220e18` a `3d9a8749` entre dos compilaciones de la
MISMA fuente y su hash de DOL no se movió—:

    zWorld      6c85b6388960740a3a96beb036a006cb7175da96
    zWorld2     3ff5ce83804d0f8185bdeeee8188e82e3b9c1fbd
    zTrack      365c04121f01573d2673a0f064cb73aa9109bffc
    zEagl4Anim  3d9a8749e8ad… (inestable)
    zPlatform   03457ae6c7d1cc81ce4ad0bf07f5c0c4221e81db
    zCamera     eb51e40f2add8bf155325d761e927ff8f470244d
    zPhysicsBehaviors  bb75afca1c7c3873b922be8e6897ac6982de9f36

---

## 5. `inittmr`: la receta NO le vale, y la razón es útil

`inittmr` lee `rodata-144` y **no tiene ni una línea en `keep.lst`**, así que no hay bloque
que alargar. Y hay algo más: **el objeto del OBJETIVO no tiene `.rodata` en absoluto** (0 B)
y el nuestro tiene 24. O sea que el `-144` **no es contenido que falte en la unidad**: son
144 B de otro sitio que dejan de estar referenciados cuando entra nuestro objeto y
`-strip-unused-data` se lleva. Es un problema de REFERENCIA, no de bytes, y sigue siendo el
paquete atómico con `splits.txt` que describe el dossier. **No lo he tocado.**

---

## 6. `lcfix`: **267 pendientes al cerrar, 122 mías, NINGUNA venenosa**

    python scripts/lcfix.py --check
       62  CORRIGE  zSpeech      <- de otro agente
       50  CORRIGE  zAI          <- de otro agente
       39  CORRIGE  zWorld       <- MIA
       30  CORRIGE  zPhysicsBehaviors  <- MIA
       22  FALLO    zAnim        <- de otro agente
       21  CORRIGE  zCamera      <- MIA
       19  CORRIGE  zWorld2      <- MIA
       13  CORRIGE  zPlatform    <- MIA
        5  CORRIGE  zAnim        <- de otro agente
        3  CORRIGE  zGameplay    <- de otro agente
        1  FALLO    zSpeech / zLua / zAI: 'GAMECUBE' sin $LC propio  <- cosmético (r58 §4.1)

**No lo he corrido.** Las 122 mías **no las ha creado esta ronda**: estaban las 122 antes de
mi primera edición, en cuanto recompilé (§4.2).

**¿Alguna rompería mis unidades? NINGUNA — al revés: sin ellas mis medidas no valen.** Está
medido en las dos direcciones (§4.2): con el `keep.lst` del árbol tal cual, cinco de mis
unidades leen un `rodata-N` que no existe; con `lcfix` aplicado leen `+0`. **Todas mis cifras
de este informe van contra una copia corregida en el scratchpad**, enlazando con
`--ldflags "-strip-unused-data -keep <copia>"`, y el enlace base con esa copia **reproduce el
DOL original al byte** (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`), comprobado tres veces a
lo largo de la ronda.

---

## 7. Lo que pido, con la cifra

| propuesta | vale | quién |
|---|---:|---|
| **`python scripts/lcfix.py`** — 242 correcciones. **Sin esto mis siete unidades no se pueden medir ni promocionar** | obligatorio | ventana |
| **Promocionar `zWorld` y `zTrack`**: son IGUAL en las nueve. Van a `DOL ROTO` por ORDEN, no por contenido | 2 candidatas nuevas | ventana |
| **Barrer `gap_*`/`pad_*`/`lbl_*` de `keep.lst` sin escribir en las 20 unidades restantes** — aquí valió 1.408 B de `.data` en siete unidades y dos IGUAL | alto | ventana |
| Añadir a `gapchk.py` que **un `.o` renombrado pierde el `keep.lst`** (§4.1), que es la trampa que más caro sale | media ronda por agente | ventana |
| `deadstr`/`keepfalta` para los 5 `lbl_` muertos de `zWorld` y el 1 de `zCamera` que `gapchk` señala (20 B y 4 B, `N & ~7 = 0`, hoy inofensivos) | 0 B hoy | el dueño |

---

## 8. Siguiente paso, por unidad

1. **`zWorld` y `zTrack` — IGUAL en las nueve.** Lo que queda es ORDEN puro:
   `permorden`/`rodorden`/`parseord`. `zTrack` está a **57.446 B** del DOL, que es la unidad
   **más cerca de todo el frente después de las cinco del encargo**.
2. **`zWorld2` — `bss+64`.** Sobran 64 B de `.bss` NUESTROS. El mapa está hecho: el objetivo
   tiene `roadSpline` dos veces y nosotros `roadSpline` + **`debugSpline.17719` (108 B)** +
   `roadSpline`; ahí está el hilo. **No se arregla añadiendo, hay que dejar de emitir.**
3. **`zPlatform` (`text-8`), `zEagl4Anim` (`text+4`), `zCamera` (`text+16`),
   `zPhysicsBehaviors` (`text+4`)** — no les queda **ni un byte de dato**: son cuatro encargos
   de CÓDIGO puro, y los cuatro con el resto del enlace ya limpio, que es la mejor posición
   posible para medir una función.
4. **Recortar el andamio**: `gap_06_80417085_data` (40 de 67) en `zCamera` y
   `gap_06_8041F6A0_data` (112 de 136) en `zPhysicsBehaviors` tienen que encoger cuando se
   escriban las estáticas de función que faltan (§3.1). Está anotado junto a cada bloque.
