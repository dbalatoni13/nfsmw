# PAQUETE DE RE-EXTRACCIÓN COORDINADA — r71g (preparado, NO ejecutar sin árbol quieto)

**Qué es**: los cuatro cambios de `splits.txt` que cierran los últimos huecos
linked de frontera. Todo verificado con `checksplits.py` (limpio). La
ejecución re-escribe `build/GOWE69/obj/*.o` — **requiere que ningún agente
esté compilando** — y se valida con el `sha1` del DOL.

**Orden de ejecución** (bisecable: cada cambio es independiente):

---

## Cambio 1: frontera .rodata zWorld/zWorld2 (−64)

El extraído de zWorld arrastra 808 referencias `.data`→`.rodata` (16.4 kB)
que nuestro árbol genera en otras unidades; el −64 es la porción viva que
queda corta. La frontera rodata entre zWorld y zWorld2 se mueve **+64**:

```
zWorld.cpp:
	.rodata     start:0x80409D58 end:0x8040DFE8
	                                   ^^^^→ 0x8040E028  (+0x40)
zWorld2.cpp:
	.rodata     start:0x8040DFE8→0x8040E028 end:0x8040F1B8
```

**Verificación post-extracción**: `linkdelta zWorld zWorld2` → ambos IGUAL.

## Cambio 2: bss de zWorld2 (+64)

zWorld2 `.bss` 0x804AAA90..0x804AC6C0 aporta 64 B de más (nuestro objeto
los genera dentro de su rango pero el extraído los tenía en otra unidad).
Mover el **start** de zWorld2 .bss +64... o el end de la unidad ANTERIOR
(¿cuál es? verificar en splits: la unidad antes de zWorld2 en .bss).

**Verificación**: `linkdelta zWorld2` → IGUAL.

## Cambio 3: steering — tres rangos (de ventana-pendiente §1, ya calibrados)

```
LibSN/steering.c:
	.bss        start:0x804B9FFC end:0x804BA024      (añadir — 1 local)
	.sbss       start:0x804FF8C0 end:0x804FF8D4      (mover start −4: 5 locales)
	.sdata2     start:0x80500800 end:0x80500828      (añadir — 9 locales)
```

Cuadran al byte con lo que emitimos. `checksplits` pasa.

**Verificación**: `linkdelta LibSN/steering` (está NonMatching 70.7% — el
delta debe reflejar solo su estado conocido).

## Cambio 4: las dos fronteras de prefijo bWare (ventana-pendiente §2)

```
zGameplay:   el prefijo en 803EBE98 del rango 0x803EBB48..0x803ED338
             → 0x803EBB48..0x803EBE98 es de zGameModes (848 B)
             → zGameplay pierde el prefijo clavado + 29 cadenas ajenas
zEAXSound2:  0x803DD658..0x803DD6D8 es de zEcstasy (128 B)
```

**PAQUETE ATÓMICO**: frontera + bloque en zGameModes.cpp + devolver el
prefijo desde zGameplay.cpp. Si se mueve la frontera sin lo demás, el
prefijo `lbl_803EBB48` queda duplicado.

---

## Protocolo de ejecución

```bash
# 0. Árbol quieto (sin agentes). Backup:
cp config/GOWE69/splits.txt config/GOWE69/splits.txt.bak
cp build/GOWE69/config.json build/GOWE69/config.json.bak 2>/dev/null

# 1. Aplicar el cambio N en splits.txt
# 2. Forzar re-extracción (borrar config.json hace que ninja re-extraiga):
rm build/GOWE69/config.json
ninja build/GOWE69/config.json   # o el target de re-extracción

# 3. Verificar:
python scripts/checksplits.py
python scripts/lcfix.py           # SIEMPRE tras re-extraer
python scripts/linkdelta.py       # los huecos deben cerrar
python scripts/measure.py         # ninguna unidad debe bajar
python tools/hash.py              # sha1 del DOL vs 9619ba57c9919f95f7f2ac951a2166a3517f91e3

# 4. Si rompe: restaurar splits.txt.bak + config.json.bak + re-extraer
#    y bisecar (los 4 cambios son independientes)
```

**Nota**: el DOL no saldrá con sha1 correcto hasta que TODO esté promocionado
— el sha1 se verifica para detectar que la re-extracción no corrompió nada
(como en la r56: `keep.lst` rancio falseaba los deltas).


## r71g-ejecucion: CAMBIO 1 (frontera zWorld) EJECUTADO Y REVERTIDO — dato medido

Tres fronteras probadas con re-extraccion real (ninja split, 4.7 s por pasada):

| frontera | zWorld | zWorld2 |
|---|---|---|
| 0x8040DFE8 (base) | rodata-64 | bss+64 |
| 0x8040E044 (+0x8C) | rodata-72 | rodata+96 bss+64 |
| 0x8040DFA8 (−0x40) | rodata-64 | **ENLACE FALLA** (rompe el vtable) |

CONCLUSION MEDIDA: **ningun borde de simbolo disponible cierra el −64 de
zWorld**. El DFE8+0x5C (E044) empeora; el DFA8 rompe zWorld2; el DFE8 base
es el mejor. Los 64 B vivos que faltan NO estan en la frontera: estan dentro
del rango de otra unidad (las 808 refs .data→.rodata del extraido que
nuestro arbol reparte de otra forma). Cerrar el −64 exige mover datos
ENTRE unidades (re-atribuir partes de rodata de zWorld2/zTrack/etc a
zWorld), no solo el borde.

DATO FANTASMA DESMONTADO: la «caida» de 4.772 B en zWorld tras la primera
re-extraccion era el REPORT RANCIO — las 4 funciones abiertas (RenderFlares
98,1 %, SetMemoryPool 97,4 %...) ya lo estaban. La medida global nunca se
movio: 99,3087 % constante en las tres pasadas. La re-extraccion es SEGURA
con el protocolo (checksplits + lcfix + linkdelta).

El CAMBIO 2 (bss zWorld2), 3 (steering) y 4 (fronteras bWare) quedan PENDIENTES
— el 1 quedo en baseline tras el revert con verificacion completa.


## r71h: ANALISIS SIMBOLO A SIMBOLO — los 64 B de zWorld identificados

Metodo: diff de contenido del .rodata ENLAZADO (base vs nuestro), multiconjunto
de cadenas, y trazado de cada candidata hasta su dueno (symbols.txt + extraidos).

**Las cadenas que el enlace BASE conserva y el NUESTRO no genera** (todas
viven solo en el zWorld.o extraido):

| cadena | B alin | nuestro .o | dueno probable |
|---|---|---|---|
| `CarLoaderDefrag but with a really long debug name!!` | 48 | SI (CarLoader.cpp:2201) | ya la generamos |
| `CarSkin Composite` | 24 | **NO** | CompositeSkin/GetTempCarSkinTextures (zona 0x802E2...) |
| `DEBUGTOPOLOGY` | 16 | **NO** | sin simbolo claro — tag de slot pool o debug |
| `SpaceNode` | 16 | SI (como `SpaceNodeSlotPool`) | InitSpaceNodes |
| `SpongeAllocation` | 16 | SI (CarLoader.cpp:704) | ya la generamos |

**Aritmetica**: las 5 suman 120 B alineados; el hueco del enlace es 64. Las
que YA generamos (3 = 80 B) no explican el hueco — estan en nuestro .o y
sobreviven. El hueco exacto: `DEBUGTOPOLOGY` (16) + `CarSkin Composite` (24)
+ 24 B de datos no-cadena aun por identificar.

**Accion siguiente** (trabajo de fuente, NO de splits):
1. `CarSkin Composite`: encontrar la funcion del original que la pasaba como
   tag (probablemente `GetTempCarSkinTextures` o `CompositeSkin(RideInfo*)`)
   y escribir el tag en nuestra version.
2. `DEBUGTOPOLOGY`: sin dueno en symbols.txt — buscar en el .line del .debug
   que linea de fuente la referencia (dwarf1/lmap).
3. Los otros ~24 B: el diff de cadenas no los cubre — son datos binarios
   (tabla o reorden de punteros) que exigira comparar los 62 simbolos
   ausentes contra los que otras unidades cubren.


## r71h-2: 'CarSkin Composite' RESUELTA la causa (no es tag de malloc)

La caza completo el circuito: lbl_8040CCA4 = 16 bytes de 0xFF + la cadena,
referenciada desde .data+0x1bada/bae2/bb16 (tabla de punteros de
inicializador estatico). El vecino lbl_8040CC94 = los colores swatch
(0xA0000 0F0... = el lookup BF0000FF etc. swizzleado) — es
`swatch_offset_cache`/`swatch_lookup_colours` del ORIGINAL como VARIABLES
ESTATICAS con inicializador, no locales de funcion como las nuestras:

```cpp
// original (deducido): estaticas en .data con init a .rodata
static uint32 swatch_lookup[16] = { ... };      // -> lbl_8040CC94 (16 B)
static uint32 something_FF[16] = { 0xFF x16 };  // -> los 16 bytes FF
static const char *tag = "CarSkin Composite";    // el nombre
```

Nuestra reconstruccion puso `swatch_lookup_colours` como local dentro del
`if (swatch_offset_init == 0)` — el compilador genera la tabla en .rodata
pero SIN los punteros .data ni el nombre. Fix de fuente: mover las tablas
a estaticas de modulo (o de funcion con inicializador estatico) y anadir
el nombre 'CarSkin Composite' como static const char*.

Quedan: 'DEBUGTOPOLOGY' (16 B) y ~24 B binarios por identificar.


## r71h-3: fix de CarSkin APLICADO Y REVERTIDO (dato medido)

Con las estaticas dentro de la funcion: .text **-40** (una funcion cayo),
rodata-72, data+32. La forma estatica-dentro-de-funcion no es la original —
quiza estaticas de FICHERO (fuera de CompositeSkin32) o con otro alcance.
REVERTIDO al baseline (.text +0, rodata-64). La causa raiz sigue valida;
la forma exacta queda pendiente.


## r71h-4: CarSkin estáticas de FICHERO — MISMO -40 (veda con cifra)

Probada la forma de fichero (estáticas fuera de CompositeSkin32, junto a
swatch_offset_cache): .text -40 idéntico a la forma de-función. La función
que cae es CompositeSkin32 misma (al leer la tabla desde fuera del if el
compilador materializa el lwz en otra ranura). El .text es todo-o-nada:
cualquier forma de tabla estática cambia el código de la función que la lee.
REVERTIDO. La solución exige que la función siga leyendo la copia inline
(pero entonces el .data nunca se genera). VEDA: sin forma conocida que
produzca la tabla .data->.rodata sin tocar el lector.
