# `steering`: los tres rangos que le faltan, probados con `STT_FILE`

La r51 midió que **cerrar las seis funciones de `steering` no la promocionaría**:
le sobran 40 B de `.bss` y 40 de `.sdata2` que el objeto extraído no tiene, y
`g_bInitialized` resuelve fuera de su rango. Eso es coordinación de
`splits.txt`, o sea mía, y va antes que su codegen. Por eso `steering` no entró
en el reparto de la r52.

## La atribución, que no es conjetura

El ELF original trae 577 entradas `STT_FILE`, y una de ellas es **`allsrc.c`**
con 47 locales que van de `0x8031F5D8` a `0x80500824`. Enfrentándola con lo que
`splits.txt` le da hoy a `steering`:

| rango | locales de `allsrc.c` | los primeros |
|---|---:|---|
| `.text` `0x8031F5D8..0x80321810` | 27 | `SteeringResetCallback`, `HandlePedals`, `HandleTriggers`, `CookValues` |
| `.data` `0x80439F18..0x80439F40` | 1 | `a$1517` |

**`allsrc.c` ES `steering.c`** — sus locales son las funciones de steering. El
nombre es el de la amalgama con que SN lo compiló.

Y entonces los tres huecos se atribuyen solos:

| sección | rango | locales | nuestro objeto |
|---|---|---:|---:|
| `.sbss` | `0x804FF8C0..0x804FF8D4` | **5** (el primero `g_lgInitialized` en `0x804FF8C0`) | 20 B ✔ |
| `.bss` | `0x804B9FFC..0x804BA024` | **1** (`ia$1518`) | 40 B ✔ |
| `.sdata2` | `0x80500800..0x80500828` | **9** (`@956`…`@1546`) | 40 B ✔ |

**Las tres cuadran al byte con lo que emitimos** (`.bss=40`, `.sbss=20`,
`.sdata2=40` contra `.sbss=16` y nada más en el extraído).

## Una corrección a la lectura de la r51

El informe decía que las 9 constantes de `.sdata2` estaban en
`0x805007F8..0x80500828`, que son **48 B**, y no cuadraba con nuestros 40.

No es así: **los nueve símbolos empiezan en `0x80500800`**. Los 8 B de
`0x805007F8..0x80500800` no tienen símbolo con nombre y no son de `steering`. El
rango correcto son los **40 B** de `0x80500800..0x80500828`, que sí cuadra.

Y `symbols.txt` ya los tiene nombrados con el tamaño bueno:

```
ia$1518      = .bss:0x804B9FFC;    size:0x28
lbl_804FF8C0 = .sbss:0x804FF8C0;   size:0x4
@956         = .sdata2:0x80500800; size:0x4
```

## El cambio propuesto

```
LibSN/steering.c:
	.text       start:0x8031F5D8 end:0x80321810
	.data       start:0x80439F18 end:0x80439F40
+	.bss        start:0x804B9FFC end:0x804BA024
-	.sbss       start:0x804FF8C4 end:0x804FF8D4
+	.sbss       start:0x804FF8C0 end:0x804FF8D4
+	.sdata2     start:0x80500800 end:0x80500828
```

`checksplits.py`: **0 solapes, 0 rangos que corten un símbolo.**

## Lo que queda por comprobar, y por qué no está hecho

`nfsmw-rango-no-basta`: dar a una unidad un rango huérfano **puede romper el
DOL** si el dato cambia de posición de enlace. Aquí los bytes salen de comodines
`auto_*` y pasan al objeto de `steering`, que en el orden de enlace está entre
`auto_06_804394D0_data.o` y `OS.o` — o sea que sí se mueven.

Eso sólo se comprueba **re-extrayendo**, y la re-extracción reescribe los
`build/GOWE69/obj/*.o` que los siete agentes de la r52 están leyendo. **Así que
el cambio está medido y validado pero NO aplicado**: va en la ventana, con el
`sha1` del DOL comprobado en el acto.

Las colas, para cuando se compruebe:

- `.bss`: quedan **28 B** detrás (`0x804BA024..0x804BA040`, hasta `OS.c`), sin
  símbolos con nombre. `.bss` no viaja en el DOL, así que es espacio reservado,
  no contenido.
- `.sdata2`: **0 B** detrás — `OSFont.c` empieza justo en `0x80500828`. Delante
  quedan los 8 B huérfanos.
- `.sbss`: **0 B** detrás; el propio rango de `steering` continúa.

## Y lo que NO arregla

Las **4 reubicaciones con sufijo de dirección** (`g_iGammaLookup_804B9E78`,
`g_iQuarterSineTable_804B9EFC`, `g_iRampUpTable_804B9F7C`) son de la familia de
`mangfix.py` y siguen pendientes. Y los 8 B de `.text` que faltan son las seis
funciones abiertas: eso es codegen, y con el catálogo de Metrowerks de
`r51-steer.md` §1, no con el de GCC.
