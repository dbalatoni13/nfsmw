# r36e -- cierre de la tanda

DOL verificado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. `lcfix --check`
limpio, arbol limpio.

| | al empezar la r36d | al cerrar la r36e |
|---|---|---|
| matched | 98,33 % | **98,49 %** |
| codigo casado | 3.880.024 B | **3.886.632 B** |
| funciones | 18.338 | **18.360** |

## Funciones cerradas al 100 % en la ronda

| funcion | B | de quien |
|---|---|---|
| `TrackCopCameraMover::Update` | 948 | agente cam |
| `AIPursuit::AssignClosestOffsets` | 1.684 | agente ai (r36d) |
| `PATHI_loadbankdata` | 396 | mio |
| `gc_interface` (unidad entera) | 396 | mio |
| `startnextrequest` | 356 | agente snd |
| `CreateSubMixChannels` | 336 | agente snd |
| `TIMER_init` | 268 | mio |
| `fn_8030F6EC` + `fn_8030F79C` | 220 | mio |
| `DoFSReadHeader` | 140 | mio |
| `__cvt_fp2unsigned` | 72 | mio |
| los siete alias de `ppc2D2` | 28 | mio |
| `Compare__...GatherVisibleIcons` | 16 | mapeo de simbolo |

Cuatro unidades quedan enteras: `inittmr`, `metrotrk`, `gc_interface` y
`pathbank`.

## Palancas nuevas, todas con la medida que las separa de lo ya sabido

1. **`"+m"` fija el `stw` de una local de PILA.** Si la variable se pasa por
   referencia, un `"+r"` se ignora en silencio. En `PATHI_loadbankdata` doce
   filas cayeron a cero; `"m"` de lectura y el clobber de memoria dejan cuatro.
2. **La constante de un store a un temporal con barrera** rompe el desempate
   entre la preparacion de argumentos de una llamada y una sentencia ajena
   (`TIMER_init`). Las dos formas que dictaba el catalogo son PEORES.
3. **Las familias de barrera se COMBINAN**: en `TrackCop` cada una deja
   exactamente dos filas y solo cambia cuales; juntas, cero.
4. **La local de desplazamiento explicito** rompe el orden de operandos de
   `lwzx`/`stwx`; decide la local, no el cast.
5. **Cardinalidad del pin**: pinchar UNO arrastra a los demas; dos pines dan
   24 filas donde uno da 2.
6. **En la cantidad fantasma el REGISTRO importa** (r19 da 4 filas, r30 da 53),
   al reves que en el pin.
   de 64 bits de `ppc2D2`), y `bl simbolo+addend` en un `.s` para que objdiff
   resuelva el destino (`metrotrk`).

## Frentes cerrados con prueba, no con «no lo encontre»

- **El cross-jump de `UpdateAllAvoidables` y `RBGrid::Add`** (4.396 B, el 7,2 %
  de lo que falta): mecanismo, linea de `jump.c` y prueba de que lo unico que
  rompe la guarda rompe tambien el cross-jump que hace falta despues.
- **El arbol de switch de `Initialize`**: 622 medidas y la forma del objetivo
  no es alcanzable desde nuestro juego de nodos.
- **`steering`**: el 53 % no es de cflags ni de version (10 juegos de flags y
  7 versiones de MWCC).
- **`epCalculate`**, **`sfir`**, **`CompletePCreadAsync`**, **`EXI2TCHandler`**,
  **`StartNonAlignedAyncRead`**, **`PATH_createstreamimp`**: acotadas con cifra.

## El error de reparto, y lo que deja

La criba `fncmp.py` con la que monte el reparto estaba validada solo en
unidades libres. En las SourceLists sobreestimaba: cuatro de los seis
encargos llevaban como objetivo de cabecera una funcion que ya estaba al
100 %. Arreglada con seis normalizaciones y **validada ahora en las 31
unidades con codigo pendiente: coincide al byte en 29**.

Los dos agentes que toparon con ello lo detectaron solos por la via que el
brief obliga a usar (`fndiff` antes de empezar), y uno midio la magnitud del
fallo en todo el proyecto: 72.856 B fantasma en 19 unidades.
