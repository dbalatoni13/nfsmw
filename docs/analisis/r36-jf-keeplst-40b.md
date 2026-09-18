# Los últimos 40 B de zBWare por `keep.lst`: probado y REVERTIDO

Intento de cerrar el suelo de −40 B que `c36bw` había predicho. **Llegué a delta
cero en todas las secciones y aun así lo he revertido**, porque costaba una
función y el DOL seguía sin casar. Queda anotado para que nadie lo repita a
ciegas.

## El caso

Dos cadenas muertas que el truco de pegarlas al literal vivo anterior no alcanza,
porque el símbolo que les da `dtk` **empieza por una constante de coma flotante**:

    lbl_803D107C   obj +0x0264..+0x0294 (48 B)   {0.5f, 5e-11, 0.5f, 1.0f} + "SlotPool::GetAllocatedSlots"
    lbl_803D1324   obj +0x050C..+0x0524 (24 B)   {0.1f} + "bSharedStringPool"

Las dos cadenas **ya están en nuestra fuente** (`bSlotPool.cpp:154` y
`Strings.cpp:510`, como argumento de `bMalloc`), pero ese `bMalloc` de cuatro
argumentos es `inline` y descarta el nombre, así que quedan muertas y
`-strip-unused-data` se las lleva.

## Los cuatro ensayos, en orden

| # | qué | `linkdelta` | `measure` |
|---|---|---|---|
| base | | `rodata−40` | 98,1043 % · 18.337 fns |
| k1 | símbolo entero (48+24 B) con nombre de dtk, al principio del `.cpp` de la SourceList, + `keep.lst` | `rodata+32` | — |
| k2 | lo mismo, pero cada bloque **en su fichero**, junto a su función | `rodata+32` | — |
| k3 | recortado a **sólo la cola muerta** (28+20 B), y el `bMalloc` apuntando al símbolo | **`IGUAL`** (delta cero) | **98,1016 % · 18.336** |
| k4 | k3 pero devolviendo el literal a la llamada | `rodata+16` | 98,1016 % · 18.336 |

**k1 y k2** fallan por la misma razón: los flotantes que preceden a la cadena
dentro del símbolo de `dtk` **están vivos y los emite el compilador**, así que
escribirlos a mano los duplica. Medido: +32 B exactos, y en `k2` se ve el par
repetido en `src +0x026C`.

**k3 llega a delta cero en todas las secciones** —el mecanismo de `keep.lst`
funciona: el símbolo con nombre de `dtk` sobrevive al estripado— pero:

- `trypromo` sigue dando **`DOL ROTO`**: tamaños correctos, contenido no.
- Y **cuesta una función**: `measure` baja de 98,1043 % a 98,1016 % (−108 B,
  18.337 → 18.336).

**k4 aísla la causa de esa pérdida, y es lo útil del ensayo**: devolver el literal
a la llamada **no la recupera**. O sea que la función no se pierde por sustituir
el literal por el símbolo, sino por **el propio bloque `asm()`**, que al meterse
entre dos funciones recoloca el pool de la que lo rodea.

## Veda

**No insertar un `asm()` de datos entre dos funciones de un `.cpp` cuya `.text`
ya casa**, aunque el bloque no emita una sola instrucción: mueve el pool de la
función vecina y le cuesta el 100 %. La palanca de `c36feng` —«un `asm()` entre
dos definiciones cae entre sus dos pools»— **vale para colocar dato en una unidad
cuyo pool aún no casa, no para una que ya está al 100 %**.

Los 40 B siguen abiertos. La vía que queda sin probar es al revés: **no tocar la
fuente y añadir sólo la entrada de `keep.lst`** apuntando al `$LC` que nuestro
compilador ya genera para esas cadenas — requiere saber su número, que cambia en
cada compilación, así que hace falta resolverlo en el propio `configure.py`.

Estado tras revertir, verificado: `measure` 98,1043 % con 18.337 funciones,
`linkdelta zBWare` `.text +0 rodata−40`, `keep.lst` y los dos `.cpp` con
`git status` vacío.
