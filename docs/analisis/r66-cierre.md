# r66 — cierre

Segunda pasada de limpieza de andamios, atacada por **causa** y no por fichero.
Ronda de fidelidad: no buscaba bytes de DOL, buscaba fuente legítima.

## Progreso oficial verificado

| medida | antes r66 | después r66 |
|---|---:|---:|
| unidades completas | 525 / 619 | 525 / 619 |
| andamios reales (barreras + pines) | 190 | **166** |
| bytes de DOL distintos | — | 0 (ronda de fidelidad) |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, **relinkado** desde el edge de
`main.elf` (nunca leído de `build/GOWE69/main.dol`, que está congelado y dice OK siempre).

Censo de rancidez al cerrar, sobre las unidades promocionadas: **`RANCIO 0`**,
`NOCOMPILA 0`, `ok 497`, `ok_debug 28`.

Serie de la limpieza: **362 → 190 (r65) → 166 (r66)**. Retirados 196 de 362, el 54 %.

---

## Lo que la ronda estableció: los 190 eran cuatro causas

| lote | antes | después | causa y resultado |
|---|---:|---:|---|
| libc-flags | 53 | 37 + 16 por paquete | **flag de compilador olvidado** en `sf_log10.c`; refutado en `itoa.c` y `vfprintf.c` |
| libs-sn | 73 | 61 | **ALIAS** (`true_dependence`) en `path`; los flags, descartados con 2.352 compilaciones |
| juego-oculto | 97 | 96 | **un solo** andamio tapaba código, y era código **muerto** |
| juego-resto | 97 | 97 | 21 re-medidos, todos irreducibles: la local **era** la portadora del pin |

(Los recuentos por lote usan censos distintos; la cifra homogénea es el 190 → 166 del
árbol, con el mismo patrón sin comentarios.)

## El hallazgo que se lleva la ronda: un flag disfrazado de 16 andamios

Los 16 andamios de `src/libc/sf_log10.c` los dio la r65 por irreducibles tras barrer los
2.516 subconjuntos. **No eran de registros: le faltaba `-msafe-sda`**, que ocho hermanos
de libc llevan desde la r19. Con el flag, **fdlibm limpio —cero andamios— reproduce el
objeto**.

El paquete es atómico —flag en `configure.py:2211` + rango `.sdata` en `splits.txt:510` +
fuente limpia— y cualquiera de los tres solo rompe el DOL. Los cuatro sellos que predijo el
agente salieron exactos al aplicarlo.

**El discriminador no es el cruce con `configure.py`**: son las ~70 unidades de libc sin
flag y sin andamios. **Es una línea del asm objetivo**: si el pool se carga por `@sda21`
pide `-msafe-sda`; si por `@ha/@l`, no. `itoa.c` y `vfprintf.c` cargan por `@ha/@l`.

## Los otros tres

- **path era ALIAS, no flags.** Se resuelve con la vista agregada `pfstate_agg` que el
  árbol ya usaba. Retira 10 de sus 14.
- **Andamio sobre código muerto.** En `CDActionDrive.cpp` la barrera se sustituye por el
  bloque `if (false) { ... }` que describe el DWARF, y `regmap` pasa de 46 iguales + 6
  sólo del original a 51 iguales. Corrige la r65: no era código que faltara vivo, sino
  contenido muerto que el compilador ve aunque luego lo tire.
- **En los 12 sitios de «sobra una local nuestra», la local era el pin.** Quitarla deja
  2 a 12 filas de reparto puro. El patrón de `ef_pow` no se traslada: el camino es el
  `.greg`, no más formas de fuente.

---

## En la ventana: el protocolo corregido, aplicado por primera vez

1. `lcfix`: leídas las 56 líneas `FALLO` una a una. Todas de la clase inerte («no tiene
   símbolo `$LC` propio»), **ninguna de la familia `@lcsrc`**.
2. `censorancios.py` antes de commitear: dio **5 RANCIO** (`pathaction`, `pathevent`,
   `pathnode`, `pathsnd`, `saemstimupdt`). Las cinco sólo diferían en `.rela.debug`:
   `pathi.h` cambió y `build_direct` no mira cabeceras. Reconstruidas → `ok`.
3. Y eso destapó **un falso positivo de la herramienta**: `DEBUGISH` era
   `^\.(line|debug|comment|stab|note)`, que no casa `.rela.debug`. Arreglado a
   `^\.(rela\.)?(...)` y comprobado **sobre el patrón escrito en el fichero** (11 de 11
   casos, en los dos sentidos). El primer control validaba un regex aparte y no podía
   fallar.
4. `censorancios.py` quedó versionado: es el único control de cierre que compara `.o`
   contra fuente.

## Incidente entre agentes

`juego-oculto` copió `QuickGame.cpp` en plena variante V5 de `juego-resto` y lo restauró.
Comprobado al cerrar: el arreglo del `$LC523` de `zSim` sobrevive (líneas 734/749/753) y
`zSim` no está rancio.

## Lo que queda

**166 andamios: 52 barreras + 114 pines.** Todos los irreducibles llevan el diagnóstico
escrito junto al andamio, donde `previo.py` los encuentra. Los mayores restos: `vfprintf.c`
(pool por `@ha/@l`, reparto global de 18 preservados) y los empates de `allocno_compare` en
código de juego, cuya fórmula es conocida pero cuya inversión cambia lo que hace la función.
