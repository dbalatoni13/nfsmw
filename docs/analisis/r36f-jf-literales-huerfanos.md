# El frente de `linked`: los literales huerfanos

**Hallazgo:** 18 unidades tienen el codigo al **100 %** y no promocionan, y en 14
de ellas el bloqueo es el mismo y esta a una linea de `splits.txt`.

## Que pasa

El objeto extraido de esas unidades **no tiene `.rodata`** (o la tiene mas
pequena) y referencia sus literales como simbolos externos `lbl_<direccion>`.
Nosotros los emitimos como `$LC<n>` en nuestra propia `.rodata`. Al enlazar, esos
bytes de mas desplazan todo lo que viene detras y el DOL sale distinto.

La cuenta de literales **coincide exactamente**: donde el objetivo usa 12 `lbl_`
nosotros emitimos 12 `$LC` (`bigyuvswizzler`), 3 y 3 (`stagpat`, `ssysinit`,
`inittmr`), 1 y 1 (`sserver`, `rcmpbase`, `sdspmix`)... O sea que **el original
SI tenia esos literales**: lo que fallo fue la atribucion del troceador, que dejo
sus bytes sin dueno.

## Y estan en el sitio correcto

Para cada unidad he mirado si la direccion del primer `lbl_` cae en un **hueco no
asignado** de `.rodata` situado **entre sus dos vecinas del orden de enlace**.
Doce de catorce dicen que si:

| unidad | lbl_ que usa | hueco libre, y entre quienes |
|---|---|---|
| `libgcc2_4` | 3 en 0x8040FE28 | 0x8040FE28..0x8040FED0, entre `mbtowc_r.c` y `Padclamp.c` |
| `avplayer` | 3 en 0x8040FEE0 | 0x8040FEE0..0x8040FEFC, entre `Padclamp.c` y `avplayer.cpp` |
| `bigyuvswizzler` | 12 en 0x804100A4 | 0x80410080..0x80410158, entre `audioplayer.cpp` y `rcmpbase.cpp` |
| `bigswizzler` | 2 en 0x8041010C | el mismo hueco |
| `rcmpbase` | 1 en 0x80410154 | el mismo hueco |
| `rcmp_vp6_codec` | 5 en 0x80410188 | 0x80410188..0x80410230, entre `rcmpbase.cpp` y `rcmp_vp6_codec.cpp` |
| `rcmp_mad_codec` | 6 en 0x80410308 | 0x804102E8..0x80410400 |
| **`sserver`** | 1 en 0x80412880 | **0x80412880..0x80412888, entre `spktplay.c` y `sst.c`** |
| `ssysinit` | 3 en 0x804129D0 | 0x80412930..0x804129F0, entre `sstvol.c` y `svol.c` |
| `stagpat` | 3 en 0x804129E0 | el mismo hueco |
| `inittmr` | 3 en 0x80413EB8 | 0x80413E30..0x80414398 |
| `gc_interface` | 2 en 0x804144DC | 0x804144C0..0x80414530, entre `gc_driver.cpp` y `gc_interface.cpp` |

Las dos que no: `zMisc` (0x8041F8C3, dentro de otra unidad) y `sdspmix`
(0x80412AEC).

## Por que esto no contradice la veda de `libgcc2_4`

La veda dice: «asignar en splits.txt un rango huerfano a su duena rompe el DOL,
porque el dato cambia de posicion de enlace». **Es cierta y sigue siendo cierta
cuando la posicion no cuadra.** Lo que anade esta medida es el criterio que
faltaba: hay que comprobar antes que el hueco este **entre las dos vecinas de la
unidad en el orden de enlace**. En `sserver` lo esta al byte --su `.text` va
entre `spktplay` y `sst`, y el hueco de `.rodata` tambien--.

## Lo que NO vale: referenciar el literal desde la fuente

Probado en `sserver`. Declarando `extern const float x __asm__("lbl_80412880")`
y usandolo en vez de `0.0f`, la `.rodata` baja a delta cero **pero `.text` sube
4 B**: GCC iza un `lis` de mas (`lis r27, lbl@ha` en el indice 33) que el
objetivo no tiene, y `SNDSYSI_100hzserver` pasa de 608/608 a 612. Se pierden
608 B de `matched` para ganar 4 de `.rodata`. Descartado.

## Segunda medida de la via de la fuente: tambien rompe (r36f)

`libc/kf_rem_pio2` es el caso mas limpio del frente, porque el objetivo
referencia sus dos tablas como simbolos **UNDEF** con nombre del troceador:
`init_jk_805007C4` y `PIo2_805007D0`. Declararlas asi en la fuente
--`extern const int init_jk[] __asm__("init_jk_805007C4")`-- deja `.sdata2` en
+28 en vez de +84, pero **sube `.text` 12 B y rompe la unidad entera**: de 0 a
2.180 B de codigo pendiente. Cambia el modo de direccionamiento.

Con `sserver` son dos medidas independientes que dicen lo mismo: **el arreglo
no esta en la fuente, esta en `splits.txt`.**

Y de paso, el reparto de la familia `rem_pio2` queda diagnosticado: la version
`double` (`e_rem_pio2`) casa ENTERA, incluidos sus 392 B de `.sdata2`; las que
sobran son las de `float` --`ef_rem_pio2` +920 B y `kf_rem_pio2` +84-- mas
`k_rem_pio2` +56. O sea que el troceador atribuyo bien las tablas de la version
double y no las de la float.

## Mas unidades con datos en la seccion equivocada (r36f)

Un barrido distinto --unidades con `.data` y `.bss` descompensadas EN SENTIDOS
OPUESTOS-- da 15 candidatas, que es la firma de un global mal colocado. Cuatro
arregladas ya: `eathread_thread` (26.624 B de ceros que salian en `.data` por un
`= {0}`), `sserver` (`lastTick`), `ssysinit` y `stagpat` (el mismo `freekey`, y
cada uno en la seccion contraria). Las once restantes son SourceLists con
agente.

Y una quinta por otra via: `input.h` tenia el par `#pragma interface` /
`implementation`, que metia en `interface.cpp` las vtables de Device, Effect y
Event mas siete talones --232 B--. Quitandolo, `interface` pasa a delta CERO y
las 96 B que quedan se van a `effect` y `event`, que son sus duenas por orden de
enlace.

## El plan

Una linea por unidad en `config/GOWE69/splits.txt`, del tipo

```
Speed/Indep/Libs/snd/9/source/library/cmn/sserver.c:
	.text       start:0x8036178C end:0x80361B6C
	.ctors      start:0x803C8BF4 end:0x803C8BF8
	.rodata     start:0x80412880 end:0x80412884      <-- nueva
	.data       start:0x804502F4 end:0x80450300
	.bss        start:0x804BEC90 end:0x804BEC9C
```

Hay que hacerlo **con los agentes parados**: cambiar `splits.txt` obliga a
`configure.py` y eso vuelve a trocear el DOL, o sea reescribe todos los
`build/GOWE69/obj/*.o`, que son justo los que los agentes leen para medir.

Orden propuesto, de menos a mas riesgo, verificando el DOL despues de cada una:

1. `sserver` (4 B, el hueco es exacto y no lo comparte con nadie).
2. `ssysinit` y `stagpat` (comparten hueco: hay que repartirlo entre las dos).
3. `inittmr`, `gc_interface`, `libgcc2_4`.
4. La familia de `egami` (`avplayer`, los dos swizzlers, `rcmpbase`, los dos
   codecs), que comparte tres huecos y es la mas enredada.

## Cuanto vale

6.456 B de `.rodata` de mas en 18 unidades con el codigo al 100 %. Si caen,
`linked` sube de 476 a hasta 494 ficheros de 617 --y `gc_interface` arrastra
ademas a `asd2`, que hoy falla el enlace por un nombre--.
