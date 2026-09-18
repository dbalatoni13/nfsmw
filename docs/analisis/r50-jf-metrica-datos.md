# La métrica de datos (34,54 %) no mide los datos

Auditoría hecha en coordinación mientras corrían los siete agentes de la r50.
Sólo lectura de `report.json` y de los dos ELF; no toca el árbol.

## El número que el proyecto arrastra

`report.json` da `Data: 444.041 / 1.285.741 B (34,54 %)`, y se ha leído siempre
como «nos falta el 65 % de los datos» —**841.700 B**—. Nadie lo había auditado.

## Partido por estado de enlace, se cae solo

| grupo | unidades | datos casados | % | sin casar |
|---|---:|---:|---:|---:|
| **enlazadas** | 232 | 332.245 / 340.533 | **97,6 %** | 8.288 B |
| **sin enlazar** | 117 | 111.796 / 945.200 | **11,8 %** | 833.404 B |

El 99 % del «déficit» está en unidades que no enlazan. Y dentro de ésas, el
patrón no es de datos:

```
zPlatform            4 / 141.188        zFe2            4 /  34.136
zMain                4 /  71.864        zEAXSound2      4 /  31.960
zTrack               4 /  55.648        zAI             4 /  31.316
zWorld               4 /  42.716        zEAXSound       4 /  25.992
zPhysics             4 /  36.192
```

**Exactamente 4 bytes casados** en nueve unidades distintas, con totales que van
de 25 kB a 141 kB. Eso no es una medida de calidad de los datos: es objdiff
**emparejando un solo símbolo por casualidad**.

De las 26 unidades sin enlazar con más de 1 kB de datos, **16 casan 8 B o
menos**, y suman **579.136 B**.

## Por qué

objdiff empareja **por nombre**, y en una unidad sin promocionar los nombres de
los datos no coinciden con los del objeto extraído por tres motivos que ya están
medidos en `r48-jf-censo-linked.md`:

- el **contador de declaraciones** de los estáticos (`hexChars.33689` contra
  `hexChars.27570`);
- los **literales** (`$LC481` contra `lbl_804…`);
- el **relleno** que el troceador nombra (`gap_05_…`, `pad_06_…`).

Emparejando por nombre **base** —quitando el contador y el sufijo de dirección—
el déficit real de esas unidades era **4 B en `.data` y CERO en `.bss`**.

## Qué significa el 34,54 %

**Es un proxy de cuántas unidades enlazan, no de cuántos datos faltan.** Sube
cuando una unidad promociona, no cuando se escribe un dato. Las 232 enlazadas
están al 97,6 %, que es la cifra real de calidad de los datos del proyecto.

**Cómo leerlo a partir de ahora**: si una unidad no enlaza, su porcentaje de
datos no dice nada; y si enlaza, ya produce el DOL correcto por definición. La
métrica **no debería usarse para repartir trabajo**, y ningún encargo debería
justificarse con ella.

## Lo que sí queda, y es pequeño

**8.288 B en 232 unidades ya enlazadas.** Ésos sí son reales —el emparejamiento
funciona ahí— y es la única parte de la métrica que mide algo. Es un frente
minúsculo comparado con los 841.700 B que aparentaba.

## Los comodines `auto_*`, mirados de verdad

No son diez sino **74**, y suman **150.684 B**. No estan en `splits.txt` ni en
`configure.py`: los genera el troceador para los rangos que no atribuye a nadie.
Y **son 74 de las 121 unidades sin enlazar**, asi que el techo de `linked` no es
618 sino ~544 mientras sigan ahi.

| seccion | uds | B | |
|---|---:|---:|---|
| `.data` | 7 | 73.236 | contenido en el DOL |
| `.rodata` | 34 | 8.496 | contenido en el DOL |
| `.sdata2` | 7 | 2.012 | contenido en el DOL |
| `.sdata` | 12 | 612 | contenido en el DOL |
| `.ctors` + `.init` | 2 | 48 | contenido en el DOL |
| **`.bss` + `.sbss`** | **12** | **66.280** | **solo espacio** |

**84.404 B son transcribibles** --tienen bytes en el DOL-- y **66.280 B no**:
`.bss` no viaja en el DOL, asi que ahi no hay contenido que escribir, solo
espacio reservado.

### El grande de `.data` (70.208 B) SI era reclamable

Y ademas ya estaba escrito. Ver el commit de `OSUtf.c`: 136 simbolos con nombre
--las tablas Unicode del SDK--, el fuente existe con 6.341 lineas, y **no lo
compila nadie**. Faltaban dos tablas de punteros, ya escritas.

### Los dos grandes de `.bss` (62.716 B) NO lo son

- `auto_07_804F4040_bss` (43.404 B), detras de `AXVPB.c`.
- `auto_07_804B54D0_bss` (19.312 B), entre `vfprintf.c` y `OS.c`.

Cuatro medidas, todas negativas:

1. **Cero simbolos con nombre** en los dos rangos --contra los 136 del de
   `.data`--. No hay nada que nombrar.
2. **Ninguna vecina se queda corta**: `AXVPB` emite 71.692 B contra los 71.680
   de su rango y `vfprintf` los 128 exactos. El hueco no es de ellas.
3. **`prodg_fixes.cpp`, que va justo antes del primero en el orden de enlace,
   no tiene fuente**: su objeto son 584 B sin una sola seccion asignada y sus
   tres entradas de `splits.txt` llevan `skip`. Es un hueco vacio.
4. Y **`.bss` no viaja en el DOL**: reclamarlo seria inventar una declaracion
   del tamano exacto, que es la familia del relleno escrito a mano --y esa se
   estripa entera salvo que lleve entrada en `keep.lst`--.

**Conclusion: no se reclaman, y no compran nada.** Sin codigo no mueven
`matched` ni `linked`, y la unica metrica que moverian es la de datos, que este
mismo informe documenta como que no mide datos.

Lo que queda de verdad en el frente de los comodines son los **14.196 B**
restantes de `.data`/`.rodata`/`.sdata2` repartidos en 58 rangos pequenos, y
esos si tienen bytes en el DOL. El mayor es `auto_06_804394D0_data` (2.632 B).

### Y los 58 rangos pequeños (14.196 B) tampoco se reclaman

Intentado en serio, con las tres herramientas que ya existen y un barrido
sistemático. **Las tres dan lo mismo: nada limpiamente reclamable.**

| medida | resultado |
|---|---|
| `claimrange.py` (por nombre de símbolo en `symbols.txt`) | **0 LIBRE**, 2 `CHOCA` |
| `claimlbl.py` (por emparejamiento `$LC`/`lbl_`) | 5 candidatos, **los 5 con pega** |
| barrido de tamaño exacto (`scratchpad/r50_jf/comodines.py`) | **ninguno** |

El barrido es el que cierra la pregunta: comprueba, para los 58 comodines con
contenido, si alguno mide **exactamente** lo que emite una unidad adyacente en
el orden de enlace que no tenga rango de esa sección — que es el caso que sí
funcionó con `sfir` en la r49. **No hay ninguno.**

Los dos únicos con los bytes correctos rompen la **regla A**:

| unidad | rango | nuestro | hueco detrás | |
|---|---|---:|---:|---|
| `pathnode` | `0x80413A48..0x80413A68` | 32 B | **120 B** | rompe |
| `bigswizzler` | `0x8041010C..0x80410124` | 24 B | **52 B** | rompe |

Y los dos tienen además causa conocida detrás del hueco:

- En `pathnode`, los 120 B restantes de ese comodín son **once `lbl_` que
  referencia `auto_01_8037C0F8_text`**, un comodín de CÓDIGO. Son literales de
  código que no tenemos. Es lo mismo que ya midió la r48 al proponer el rango
  entero de 152 B.
- En `bigswizzler`, la r48 midió que **48 de los 52 B de detrás son suyos** —la
  constante de pool, `0.0625f` y la cadena de assert que `NDEBUG` elimina—. Es
  la veda estructural del pool, no atribución de rango.

**Conclusión: de los 150.684 B de comodines, lo único reclamable era el de
`OSUtf` (70.208 B), y ya está hecho a falta de la ventana.** El resto son
`.bss` sin contenido (66.280 B) o rangos pequeños que la regla A veta
(14.196 B).

