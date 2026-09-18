# r54-frontera: por qué mover una frontera de `splits.txt` rompía el DOL

**Titular: no rompía por la frontera. Rompía porque las entradas de `keep.lst` van
CALIFICADAS POR OBJETO, y al cambiar de dueño un rango la entrada deja de aplicar;
`-strip-unused-data` se lleva entonces el bloque entero y todo lo que va detrás se
desplaza.** Es un fallo silencioso: el enlazador no avisa de una entrada de `keep` que
no resuelve.

**Las dos fronteras están APLICADAS y el árbol está VERDE**:
`sha1(build/GOWE69/main.dol) = 9619ba57c9919f95f7f2ac951a2166a3517f91e3`, y el enlace
completo con los objetos recién extraídos vuelve a dar ese mismo sha1.
`zGameModes` **queda desbloqueada**: su `.rodata` pasa de `+104` a `−744`.

---

## 1. Dónde diverge el DOL, al byte

Con la frontera de `zEAXSound2`/`zEcstasy` puesta y `keep.lst` sin tocar (el primer ensayo
del jefe, `b8afa4512b08`):

```
tamano nuestro 4.541.760   original 4.541.888   (delta -128)
primer byte distinto del FICHERO   0x0000002B     <- la tabla de direcciones de la cabecera DOL
tramos distintos 78.850, bytes 363.252
```

El primer byte que cambia está en la **cabecera del DOL** porque el fichero entero encoge:
`.bss` arranca en `0x804534E0` en vez de `0x80453560`, exactamente 128 B antes.

La divergencia de CONTENIDO es una sola y está aquí:

```
VA 0x803DD658   (offset de fichero 0x3DA658, seccion 9 del DOL)
```

Comparando las tablas de símbolos de los dos ELF **enlazados**:

```
desaparece:          lbl_803DD658          (0x803DD658, 128 B)
primer desplazado:   lbl_803DD6D8          delta -128
simbolos desplazados: 14.525
```

Un símbolo desaparece; los 14.524 que van detrás se corren 128 B; y cada `@ha`/`@l` del
`.text` que apunte más allá cambia. **De ahí salen los 363.252 B de diferencia: son la
sombra de 128 B perdidos.**

El segundo ensayo del jefe (`6c55175a7305`) es el mismo mecanismo 92 B más allá:

```
VA 0x803EBE98   (offset de fichero 0x3E8E98)
primer desplazado:   lbl_803EBEF4   delta -88
```

---

## 2. El mecanismo, con el control que lo prueba

`config/GOWE69/keep.lst` tiene dos formas de entrada: `simbolo` a secas y
`objeto.o:simbolo`. La segunda la convierte `ngcld` en una **referencia indefinida cuyo
nombre literal es `objeto.o:simbolo`** — se ve en la tabla de símbolos del ELF enlazado:

```
$ powerpc-eabi-nm build/GOWE69/main.elf | grep 803DD658
         U zEAXSound2.o:lbl_803DD658      <- la entrada de keep, resuelta
  803dd658 R lbl_803DD658                 <- por eso los 128 B siguen ahi
```

Si el objeto nombrado no define ese símbolo, la referencia **se queda sin resolver y el
enlazador no dice nada**. El símbolo pasa a ser dato no referenciado, `-strip-unused-data`
lo borra, y la sección encoge.

**El control que lo demuestra sin mover ninguna frontera**: sobre el árbol ORIGINAL, sin
tocar `splits.txt`, borrando la línea `zEAXSound2.o:lbl_803DD658` de `keep.lst`:

| ensayo | DOL |
|---|---|
| árbol original, `keep.lst` tal cual | OK `9619ba57` |
| árbol original, **borrando** `zEAXSound2.o:lbl_803DD658` | ROTO **`b8afa4512b08`** |
| frontera `zEAXSound2`/`zEcstasy` movida, `keep.lst` sin recalificar | ROTO **`b8afa4512b08`** |

**El mismo hash exacto.** Mover el rango y borrar la entrada de `keep` producen el binario
idéntico: son la misma cosa. Eso cierra la pregunta.

Y por qué el bisecado del jefe no lo cazó: **él sí recalificó esa entrada**. Lo que le
faltaba era la OTRA mitad, que no es una recalificación sino una entrada **nueva**.

---

## 3. Por qué el segundo intento (con `keep.lst` recalificado) también rompía

La frontera de `zGameModes` cae en `0x803EBE98`, y ahí hay un símbolo que la cruza:
`lbl_803EBE94`, 96 B. Hay que **partirlo** en `symbols.txt` (4 B de relleno + los 92 B del
prefijo de bWare de zGameplay). Al partirlo **nace un símbolo que antes no existía**,
`lbl_803EBE98`, y ese símbolo:

- no lo referencia nadie (es el prefijo de bWare, nunca se referencia),
- no lo nombra ninguna entrada de `keep.lst`, porque antes estaba dentro de
  `zGameplay.o:lbl_803EBE94`.

Resultado: `-strip-unused-data` se lo lleva.

Reproducido exactamente: paquete completo **menos** la línea `zGameplay.o:lbl_803EBE98`
→ **`6c55175a7305`**, que es el hash del segundo ensayo del jefe, dígito a dígito.

> **La regla que hay que recordar**: partir un símbolo en `symbols.txt` para poder cortar
> por él NO es una operación neutra. Si el símbolo original estaba en `keep.lst`, **las dos
> mitades necesitan entrada**, cada una calificada a su objeto nuevo.
> `keepchk2.py` NO lo detecta: sólo ve entradas que nombran fantasmas, no entradas que
> FALTAN para un símbolo recién nacido.

---

## 4. El paquete completo, verificado

Los tres ficheros están ya escritos en el árbol. El diff entero son **11 líneas nuevas y 8
borradas**.

### `config/GOWE69/splits.txt`

```
Speed/Indep/SourceLists/zEAXSound2.cpp:
	.rodata     start:0x803D9050 end:0x803DD658      (era end:0x803DD6D8)
Speed/Indep/SourceLists/zEcstasy.cpp:
	.rodata     start:0x803DD658 end:0x803DF568      (era start:0x803DD6D8)

Speed/Indep/SourceLists/zGameModes.cpp:
	.rodata     start:0x803EBB48 end:0x803EBE98      (LINEA NUEVA, entre .ctors y .bss)
Speed/Indep/SourceLists/zGameplay.cpp:
	.rodata     start:0x803EBE98 end:0x803ED338      (era start:0x803EBB48)
```

### `config/GOWE69/symbols.txt`

```
-lbl_803EBE94 = .rodata:0x803EBE94; // type:object size:0x60
+lbl_803EBE94 = .rodata:0x803EBE94; // type:object size:0x4
+lbl_803EBE98 = .rodata:0x803EBE98; // type:object size:0x5C
```

### `config/GOWE69/keep.lst`

```
-zEAXSound2.o:lbl_803DD658          +zEcstasy.o:lbl_803DD658        <- IMPRESCINDIBLE
-zGameplay.o:pad_05_803EBB48_rodata +zGameModes.o:pad_05_803EBB48_rodata
-zGameplay.o:lbl_803EBE94           +zGameModes.o:lbl_803EBE94
                                    +zGameplay.o:lbl_803EBE98       <- IMPRESCINDIBLE, NUEVA
-zGameplay.o:lbl_803EBB48           +zGameModes.o:lbl_803EBB48      (fosil, ver §6)
```

### Ablaciones: qué línea sostiene qué (todo medido sobre el árbol ya aplicado)

| se quita | DOL |
|---|---|
| `zEcstasy.o:lbl_803DD658` | ROTO `b8afa4512b08` |
| `zGameplay.o:lbl_803EBE98` | ROTO `6c55175a7305` |
| las **tres** de `zGameModes.o` a la vez | **OK** — hoy son inertes (§5) |
| nada | **OK `9619ba57c9919f95f7f2ac951a2166a3517f91e3`** |

### Comprobaciones que quedan limpias

```
python scripts/checksplits.py   ->  SOLAPES 0, RANGOS QUE CORTAN UN SIMBOLO 0
python scripts/prefijochk.py    ->  31 prefijos, 115 rangos, LIMPIO: ningun rango con dos
python scripts/keepchk2.py      ->  17 huerfanas (16 preexistentes + zGameModes:lbl_803EBB48, §6)
```

### Efecto en `linkdelta`, contra lo que pronosticó la r53

| unidad | antes | ahora | pronóstico r53 |
|---|---|---|---|
| **zGameModes** | `rodata+104` | **`rodata-744`** | −744 ✔ |
| zGameplay | `rodata-896` | `rodata-136 data-128` | (r53 decía −48) |
| zEAXSound2 | `rodata-176` | `rodata-48 data-288 bss-32` | −48 ✔ |
| zEcstasy | `rodata-288` | `rodata-416 data+64 bss+32` | −416 ✔ |

Tres de las cuatro clavadas al byte. `zGameModes` deja de tener un suelo de `+104`: ahora
tiene un objetivo de 848 B y le faltan 616 de vocabulario, que es trabajo escribible con la
receta de la r53 §«La receta para cerrar zGameModes».

---

## 5. Un hecho medido que NO sé explicar (y por eso la regla es conservadora)

Después de mudarse a `zGameModes.o`, `pad_05_803EBB48_rodata` (840 B, LOCAL, **cero
referencias**) y `lbl_803EBE94` (4 B, GLOBAL, cero referencias) **sobreviven sin entrada de
`keep`**. En `zGameplay.o`, ese mismo bloque de 840 B, en la misma dirección, **sí** se
estripaba sin ella (medido: borrar `zGameplay.o:pad_05_803EBB48_rodata` del árbol original
da ROTO `319207a72765`).

Tres hipótesis probadas y **las tres refutadas**:

- *«las secciones sin `.rela.rodata` no se trocean»* — `zPlatform.o` y `zMission.o` no
  tienen `.rela.rodata` y sus entradas SÍ son imprescindibles.
- *«los objetos sin `.data` no se trocean»* — `zMission.o` tampoco tiene `.data`.
- *«depende del tamaño del bloque o de la sección»* — el mismo bloque de 840 B se estripa en
  `zGameplay.o` y no en `zGameModes.o`.

`zMission.o` es el gemelo estructural de `zGameModes.o` (secciones `.bss`/`.ctors`/`.rodata`,
`pad_05_*` LOCAL sin referencias al principio, un `lbl_*` referenciado al final, sin
`.rela.rodata`) y se comporta al revés. **Queda abierto.**

**Regla operativa mientras no se explique: recalifica SIEMPRE todas las entradas del rango
que muevas, no confíes en que sobra ninguna.** Cuesta una línea y la alternativa es un DOL
roto que tarda una hora en bisecarse.

---

## 6. Lo que queda pendiente y no es mío

1. **La mitad de FUENTE del paquete sigue sin hacer, y ahora está señalizada.**
   `keepchk2.py` marca `zGameModes.o:lbl_803EBB48` como huérfana. Es correcto y es útil:
   nuestro `build/GOWE69/src/.../zGameplay.o` **define hoy `lbl_803EBB48`** en el offset 0
   de su `.rodata` — el prefijo de bWare y las 29 cadenas que la r53 dice que tienen que
   volver a `zGameModes.cpp`. La entrada de `keep` ya apunta al objeto correcto; cuando la
   fuente se mueva, deja de ser huérfana sola. **No la borres: es el marcador del trabajo
   pendiente.** Y nada de esto afecta al enlace de hoy (zGameplay no promociona).
2. `EmotionManagerImpl.cpp` sigue vacío y sin incluir en `zGameModes.cpp` (r53).
3. El resto de fronteras: `prefijochk.py` ya sale **LIMPIO**, así que este frente está
   cerrado. De las tres unidades que la r53 listaba sin rango de `.rodata`, `zGameModes` ya
   lo tiene y las otras dos no son un caso: `zOnline` y `zFeOverlay` lo declaran como
   `.over ... rename:.rodata`. **Hoy no queda ninguna SourceList sin `.rodata`.**

---

## 7. Cómo se midió (y cómo repetirlo sin pisar a nadie)

**No hace falta re-extraer sobre `build/GOWE69/obj` para probar una frontera.** `dtk dol
split` acepta un directorio de salida cualquiera:

1. Copia `splits.txt`/`symbols.txt` a un directorio **bajo la raíz del repo** — `dtk` no
   acepta rutas absolutas de Windows en esos dos campos del `.yml`: falla con un
   `Section .data not found` que no tiene nada que ver.
2. `dtk dol split <yml_copia> <dir_scratch>` escribe ahí los 616 objetos.
3. Enlaza con la lista de `build.ninja` sustituyendo el prefijo `build/GOWE69/obj/` por
   `<dir_scratch>/obj/`, con
   `ngcld -strip-unused-data -keep <keep> -T config/GOWE69/ldscript.ld`, y `dtk elf2dol`.

**Control obligatorio**: con los ficheros SIN modificar, los 616 objetos salen **byte a byte
idénticos** a los del árbol y el enlace da `9619ba57`. Comprobado antes de cada medida de
este informe.

Para localizar la rotura, lo que la encuentra en un segundo no es el diff de bytes (78.850
tramos) sino **diferenciar las tablas de símbolos de los dos ELF ENLAZADOS**: sale «un
símbolo desaparece, N se desplazan −K». Es `nfsmw-text-cero-dos-errores` aplicado a `.rodata`.

Todas las re-extracciones de esta sesión fueron a scratch salvo **una** al final, sobre
`build/GOWE69`, con el paquete ya verificado.
