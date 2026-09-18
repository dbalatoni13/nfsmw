# Tras zBWare y zMiscSmall: qué bloquea a cada unidad, medido

Estado: `linked` **17,41 %** (475/617), `matched` 98,10 % (18.331/18.432
funciones, 74.796 B en 101 funciones). `main.dol` byte a byte.

## El mapa de distancias (`linkdelta` de las 25 SourceLists)

    zAttribSys        +0  IGUAL          <- delta cero y AUN ASI DOL ROTO
    zMiscSmall        +0  rodata+96      <- CERRADA, DOL OK
    zGameModes        +0  rodata+104
    zSim              +0  rodata-432 data-224
    zEcstasy          -8  rodata-760 data+64 bss+32
    zEagl4Anim       +12  rodata-864 data-96 bss-128
    zSpeech           -4  rodata-1416 bss+32
    ... hasta zWorld  -4  rodata-4720 data-128

## zAttribSys: la única con delta cero, y lo que la bloquea

**No es dato: es una función de 300 B en el sitio equivocado.** Todas las
diferencias del DOL son inmediatos de 2 B desplazados exactamente 0x12C = 300,
y `Transfer__Q26Attrib7HashMapRQ26Attrib4Node` mide 300 B.

    objetivo:  RebuildTable(+09A8C)  Transfer(+09C5C)  UpdateSearchLength(+09D88)
    nuestro :  RebuildTable(+09B28)  UpdateSearchLength(+09CF8) ... Transfer(+0B74C)

Las tres son inline en clase (`AttribHashMap.h`), y la única llamada a `Transfer`
sale de `RebuildTable` en los dos objetos. La condición del fuente de SN
(`toplev.c:3010`) es que una inline se emite cuando al llegar a su índice ya está
puesto `TREE_SYMBOL_REFERENCED`; el nuestro no lo está, así que cae en la vuelta
siguiente del `do { } while (reconsider)` y aterriza 6.896 B más tarde.

**Dos ensayos, los dos negativos, con lo que enseñan:**

| # | qué | resultado |
|---|---|---|
| 1 | `Transfer` movida justo detrás de `RebuildTable` | **idéntico**: sigue en +0B74C |
| 2 | `RebuildTable` movida detrás de `Transfer` | `Transfer` **desaparece**: GCC la inlinea, y `UpdateSearchLength` se va a +09DE4 |
| 3 | `Transfer` **declarada en clase y definida FUERA**, `inline HashMap::Transfer` | **idéntico**: sigue en +0B74C |
| 4 | compilar la unidad entera con **`-fkeep-inline-functions`** | **idéntico**: +0B7D8. Y eso que la condición de `toplev.c:3012` es `flag_keep_inline_functions \|\| ...`, o sea que con la bandera TODA inline se emite en su índice de la primera pasada |

Con las tres: la posición en la fuente **no decide** dónde sale `Transfer` (ensayo 1),
pero sí decide si se inlinea (ensayo 2, definida antes que su llamante). El
objetivo no hace ninguna de las tres cosas. El indice de `Transfer` en
`saved_inlines` **no depende de donde este escrita ni de la bandera**: el ensayo
4 es el decisivo, porque con `-fkeep-inline-functions` la condicion del bucle se
cumple para TODAS las inline y aun asi `Transfer` sale al final. Su indice en el
vector esta genuinamente detras de todo, y eso no lo pone la fuente de esta
cabecera. El siguiente sitio donde mirar es QUIEN la mete en la lista: la unica
llamada sale de `RebuildTable`, asi que el orden lo decide el momento en que
cc1plus instancia el cuerpo, no donde este escrito.

## zGameModes: sus cadenas son de zGameplay, y son OTRAS

El objeto extraído de zGameModes tiene **sólo** `.text`=124, `.ctors`=4 y
`.bss`=8 — ni `.rodata` ni `.data`. Nosotros emitimos 248 B de nombres de
enumerado de `EmotionManager.cpp`, de los que sobreviven 104 al estripado.

Buscadas en el DOL, las cadenas están, pero **en el rango de zGameplay** y con
otra grafía:

    nuestro : PURSUIT_BEGINS  PURSUIT_ADDS_CAR  ...  y "Created" al principio
    objetivo: PursuitBegins   PursuitAddsCar    ...  SIN "Created"

y detrás va `"EmotionManagerImpl"`, una clase que no está en nuestro árbol. O sea
que en el original esas tablas viven en un `EmotionManagerImpl` de zGameplay, no
en zGameModes. Corregir la grafía es trabajo de zGameplay, no de aquí.

## zEcstasy: la retirada de `.size` en bloque NO vale

`gapchk.py` da 610 símbolos muertos y 7.323 B en juego, y la unidad está en
`rodata-760`. Quitadas las **691** directivas `.size` de golpe (la palanca que a
zRender le valió −800 → +224):

    rodata-760  ->  rodata+4456

Se pasa de largo en 5.216 B, porque de muchos de esos símbolos **cc1plus emite
además su propia copia**: con `.size` se estripaba la copia a mano y las cuentas
cuadraban de casualidad. Hay que ir símbolo a símbolo, suprimiendo la copia del
compilador donde se escriba el dato a mano. Revertido.

## Frente agotado: los literales duplicados

Contadas las copias de `"GAMECUBE"`, `"Attrib::Attribute"`, `"Attrib::Blob"`,
`"bad_alloc"` y `"STL"` en las 25 unidades, **ninguna** tiene más copias que su
objetivo. Los interruptores `BWARE_PREFIX_*` / `ATTRIB_TAGS_HAND_POOL` ya están
donde hacían falta; sólo faltaban en zMiscSmall.

## Frente nuevo y medido: 9 unidades emiten una sección que su objeto extraído no tiene

Barridas las 617 unidades comparando **todas** las secciones (no sólo las que
mira objdiff), hay **9 cuyo único desajuste es una sección que el objetivo no
tiene**, 1.460 B en total:

    920 B  libc/ef_rem_pio2                         .sdata2
    152 B  libc/math_support                        .sdata2
    128 B  realcore/.../file/gc/hd_device           .rodata
     84 B  libc/kf_rem_pio2                         .sdata2
     80 B  egami/.../vd/gc/bigyuvswizzler           .rodata
     32 B  realmemcard/.../lib/gc/tasks             .bss
     24 B  libc/libgcc2_4                           .rodata
     24 B  egami/.../vd/gc/bigswizzler              .rodata
     16 B  snd/9/.../coda/cmn/eaxadecform           .rodata

Y **no es dato que sobre**: el dato ESTÁ en el DOL, pero `splits.txt` no se lo
asigna a nadie. `"VD::tBigSwizzler"` está en `0x804100A4` y **ningún split
cubre esa dirección** — se lo queda la unidad autogenerada
`auto_05_80410080_rodata`, junto con `"Assert: %s:%i %s (%s)"` de otra unidad.

Es el frente de límites de `splits.txt` (`dwarfmap --limites`), y aquí se ve
medido: las 12 unidades que están al **100 % en código Y en dato** y siguen
marcadas `NonMatching` fallan todas por esto, no por su código.

Una diferencia real que hay que resolver antes: nuestro literal se alinea a 8
(17 B de cadena + 7 de relleno) y el del objetivo a 4 (17 + 3), así que el rango
que habría que asignar mide 76 B y el nuestro 80.

### El caso desmenuzado: `bigswizzler` / `bigyuvswizzler`

`"VD::tBigSwizzler"` aparece **dos veces** en el DOL, en `0x804100A4` y
`0x8041010C`, y **ninguna de las dos direcciones tiene dueño en `splits.txt`**:
se las queda `auto_05_80410080_rodata`. O sea que el dato nuestro es correcto y
lo que falla es la atribución.

Pero hay además un hueco real de reconstrucción. Desmenuzados los dos bloques
del objetivo contra nuestros dos objetos:

    objetivo 0x804100A4 : "VD::tBigSwizzler" + 20 B de relleno + siete flotantes
    objetivo 0x8041010C : "VD::tBigSwizzler" + 28 B de relleno + 43300000_00000000
                                                              + 3d800000_00000000
    nuestro  bigyuvswizzler (80 B) : la cadena + TODOS los flotantes de los dos
    nuestro  bigswizzler    (24 B) : sólo la cadena

O sea que **a nuestro `bigswizzler` le faltan los dos dobles** —la pareja mágica
de la conversión `int`->`double`— que el original sí emite. Es una sentencia que
no hemos reconstruido, no un problema de límites.

Las dos cosas hacen falta para promocionar estas dos unidades: la sentencia que
falta en `bigswizzler.cpp` y el rango de `.rodata` en `splits.txt`.

Y el hueco de `bigswizzler` está identificado del todo: sus **cinco** funciones
están las cinco al 100 % y son exactamente las cinco del objeto extraído, así que
los dos dobles que faltan son **el pool de una SEXTA función que el original
tenía y el enlazador descartó** —el mismo mecanismo que cerró zBWare, donde
`-strip-unused-data` se lleva la función pero conserva su pool. La pareja es
`43300000_00000000` + `3d800000_00000000`: una conversión `unsigned`->`double`.
Escribir esa función por su pool es el trabajo, igual que `bGetPolyArea`.

### VEDA: asignar el rango a su dueño NO basta — probado y revertido

Ensayo completo sobre `libc/libgcc2_4`, el caso más limpio de los nueve (24 B de
`.rodata`, localizados en el DOL en `0x8040FE28`, dentro de
`auto_05_8040FE28_rodata`):

1. `splits.txt`: `.rodata start:0x8040FE28 end:0x8040FE40` en `libgcc2_4.c`.
   El split **falla**: `ends within symbol 'lbl_8040FE38' (0x8040FE38..0x8040FED0)`.
2. `symbols.txt`: ese símbolo es un amasijo de `dtk` —8 B de doble y 144 B de la
   cadena `"
*** Library error ***..."` de ProDG, todo bajo un `size:0x98
   data:double`. Partido en `lbl_8040FE38` (0x8) + `lbl_8040FE40` (0x90).
3. `dtk dol split` ahora sí: 617 objetos. `configure.py`. `ninja`.
4. **`main.dol` ROTO** (`951091f0ce4b`) — y es exactamente el hash que daba
   `trypromo libc/libgcc2_4` antes de tocar nada.

**Por qué**: los bytes existen y son correctos, pero al pasar de la unidad
autogenerada a `libgcc2_4.o` cambian de **posición de enlace**. Las unidades
autogeneradas se colocan por DIRECCIÓN y por eso reproducen la disposición; una
unidad con nombre se coloca por su sitio en la lista de enlace, y ahí no cae.

O sea que estas nueve unidades no se arreglan con el rango: hace falta además
que su posición en el orden de enlace sea la del original. Es el frente del
**orden de enlace**, no el de los límites. Todo revertido y verificado:
`splits.txt` y `symbols.txt` con `git status` vacío, split de 617 objetos,
`main.dol` en `9619ba57c991`.
