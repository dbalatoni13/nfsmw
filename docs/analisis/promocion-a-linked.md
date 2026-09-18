# La promoción a *linked*: por qué 177 unidades al 100% no cuentan

`matched_code` (83,96%) mide instrucciones. `complete_code` —lo que se suele
llamar *linked*— está en **5,55%** y sólo cuenta las unidades marcadas
`Matching` en `configure.py`, que son las que el enlazador toma de **nuestro**
objeto en vez del extraído. Son dos números que miden cosas distintas, y el
segundo es el que dice cuánto del juego se reconstruye de verdad desde la
fuente.

Hoy hay **177 unidades que casan al 100% en código y datos y no están
promocionadas**: 125.764 B de código. Este documento dice por qué, unidad por
unidad, y qué hace falta para cada grupo. Lo comprueba `scripts/promote.py`
**leyendo los dos ELF, sin construir nada** — importante, porque hay agentes
compilando en el mismo árbol y `configure.py` regenera `build.ninja`.

## 1. Limpias: 76 unidades, 22.584 B

No tienen ninguna pega. Mismo conjunto de secciones con contenido, mismos
símbolos globales definidos, ninguna reubicación a símbolo con sufijo de
dirección. **Se pueden marcar `Matching` tal cual**; sólo falta la ventana sin
agentes para reconfigurar y verificar `main.dol: OK`.

Son sobre todo middleware y libc: `spat2hdr`, `idctpart`, `locale`,
`pathcontrol`, `supxalf`, `pb_globals`, `Huffman`, `fopen`, `e_sqrt`,
`memcopy`, `spchrslv`, `filesys_c`, `filesys_cc`, `syncfile`…

## 2. Les falta una línea en `splits.txt`: 11 unidades, 13.164 B

Su `.rodata` existe en el DOL pero **sin dueño**, así que decomp-toolkit lo mete
en los objetos comodín `auto_*_rodata.o`. Al enlazar, nuestro objeto aporta un
`.rodata` que el extraído no tenía y el DOL se desplaza.

La dirección **no hay que adivinarla**: nuestros bytes ya son correctos, así que
basta buscarlos en el montón sin reclamar. `scripts/claimdata.py` lo hace, y con
`--write` escribe la línea. Las que salen más de una vez se desambiguan por
**orden de enlace** (dentro de una biblioteca el `.rodata` va en el mismo orden
que el `.text`, así que las vecinas ya resueltas acotan por arriba y por abajo).

    .rodata start:0x80413028 end:0x80413090  mtdecf   (4.808 B de código)
    .rodata start:0x804138A8 end:0x80413958  pathvol  (4.124 B)
    ...

## 3. Un rango mal cerrado que se traga a sus vecinas

**El `.rodata` declarado para `mpegl3base.cpp` es demasiado largo.** Dentro de
él viven los datos de `smixer` (144 B), `stretch` (64 B) y `satospkr` (88 B),
que por eso no pueden reclamarlos. Partir ese rango libera **8.712 B** de código
de tres unidades que ya casan.

## 4. El `.rodata` fantasma: otra cara del «100% que miente»

24 unidades (59.084 B) emiten un `.rodata` que **no está en ninguna parte del
DOL**. No es un problema de troceado: es que **nuestra fuente emite datos que el
original no tiene**.

Y aquí está lo importante, porque es una trampa de medición:

> **Una unidad sin `.rodata` declarado en `splits.txt` SIEMPRE mide 100% en
> datos**, porque `total_data` es 0 y no hay nada contra lo que comparar. El
> 100% no dice que sus datos estén bien: dice que nadie los ha mirado.

Los dos casos encontrados:

- **`'ssss'` y `'ssii'` en las cuatro unidades de realmemcard** (`trctasks`
  20.176 B, `tasks` 4.520 B, `memcard_interface` 1.736 B,
  `memcard_interface_impl` 1.716 B). Salen de dos llamadas a
  `Locale::GetString(msgId, "ssss", ...)` en
  `include/common/realmemcard/impl/memcard_interface_impl.h`. La *release*
  original no las tiene, así que ese camino está compilado a nada por una macro
  de traza que nosotros no reproducimos. **~28 kB detrás de un `#define`.**
- **`'VD::tBigSwizzler'` en `bigyuvswizzler`**: sale de
  `RCMP::rcmp_sys.AllocMem("VD::tBigSwizzler", ...)`. Su hermana `bigswizzler`
  **sí** tiene esa cadena en el DOL (0x8041010C), así que la cadena es correcta
  y lo que sobra en `bigyuvswizzler` son los otros ~56 B, constantes flotantes
  que el original coloca en `.sdata2`.

## Orden de ataque

1. Promocionar las 76 limpias (22.584 B) — sólo hace falta la ventana.
2. `claimdata.py --write` para las 11 (13.164 B).
3. Partir el rango de `mpegl3base` (8.712 B).
4. La macro de traza de realmemcard (~28 kB) — es el bocado grande.

Los cuatro juntos llevarían `complete_code` de 5,55% a **≈7,3%**, sin escribir
ni una línea de código nuevo.
