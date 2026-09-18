# `inittmr`: la unidad más pequeña del frente, con el bloqueo entero localizado

Tercera pieza de la ventana de la r55. `inittmr` es la treceava del bloque A y **la unidad más
pequeña de todo el frente**: 472 B de código, al 100 %, y `estado.py` la marca como
«código COMPLETO -> le bloquea un DATO». Nadie la ha mirado en toda la sesión y no la posee
ningún agente.

---

## El bloqueo, en una línea

    python scripts/promote.py Speed/Indep/Libs/realcore/.../inittmr
    - secciones que emitimos de mas: .rodata(24B)

Nada más. `.text` 472 B en los dos objetos, `.bss` 40 en los dos, `.sdata` 8 en los dos.

Y el objeto **original no tiene sección `.rodata` en absoluto**. Nosotros emitimos tres
literales: `$LC0` (8 B), `$LC1` (8 B), `$LC2` (4 B).

---

## De dónde salen, y el código es el MISMO

Vienen de la única expresión de coma flotante del fichero, `inittmr.cpp:56`:

```c
tmp = (int)((float)OS_TIMER_CLOCK * (1.0f / (float)hz));
```

Las dos de 8 B son las constantes mágicas de la conversión entero→float de PowerPC.

**Y el original hace exactamente lo mismo.** Comparadas las reubicaciones de `.text` de los dos
objetos, la correspondencia es offset a offset y tipo a tipo:

| offset | nuestro | original |
|---:|---|---|
| 78, 90 | `$LC1` | `lbl_80413EC0` |
| 86, 106 | `$LC2` | `lbl_80413EC8` |
| 94, 126 | `$LC0` | `lbl_80413EB8` |

Seis referencias, los mismos seis sitios. **No es que el original no use literales: es que los
suyos viven en otra parte.**

---

## Dónde viven los del original, y por qué eso lo bloquea

`0x80413EB8`, `0x80413EC0`, `0x80413EC8` — y **ninguna ventana de `splits.txt` contiene esas
direcciones**. Los rangos de `inittmr` son sólo `.text 0x8038607C..0x80386254`,
`.bss 0x804D5018..0x804D5040` y `.sdata 0x804FF628..0x804FF630`: **no tiene `.rodata`**.

`symbols.txt` sí los conoce, y con sus tamaños:

```
lbl_80413EB8 = .rodata:0x80413EB8;  // size:0x8   align:8  data:double
lbl_80413EC0 = .rodata:0x80413EC0;  // size:0x8   align:8  data:double
lbl_80413EC8 = .rodata:0x80413EC8;  // size:0x98  align:4  data:float   <- 152 B
```

O sea que el objetivo aporta **168 B** de `.rodata` (8 + 8 + 152) desde una región que el
troceador no atribuye a nadie, y nosotros emitimos **24** desde el objeto. Por eso `promote`
dice «24 B de más»: comparado contra un objetivo que hoy vale 0.

---

## El paquete para la r56, y es atómico

1. **`splits.txt`: dar a `inittmr` un rango de `.rodata`** que cubra `0x80413EB8` en adelante.
   **Riesgo conocido y con memoria propia** (`nfsmw-rango-no-basta`): asignar un rango huérfano
   a su dueña **mueve el dato de posición de enlace y puede romper el DOL**. Sólo se comprueba
   re-extrayendo y mirando el `sha1`. Es mío, no de un agente.
2. **Fuente: escribir los 144 B que faltan.** Con el rango puesto, el objetivo aporta 168 B y
   nosotros 24, así que pasaríamos de `+24` a `−144`. El grueso es `lbl_80413EC8`, un bloque de
   152 B de `float` del que sólo emitimos los primeros 4.
3. **`lcfix.py`** para que los `$LC` casen por contenido con `lbl_80413EB8/EC0/EC8`.

**Sin comprobar, y hay que comprobarlo antes de mover el rango:** que los 152 B de
`lbl_80413EC8` sean **todos** de `inittmr`. Un bloque de floats de ese tamaño puede estar
compartido con la función vecina, y si lo está, el rango correcto es otro.

---

## Dos errores míos por el camino, los dos cazados a tiempo

**1. Leí un relleno de reubicación como si fuera control de flujo.** Comparando los bytes de
`.text` salían 5 palabras distintas, todas ramas condicionales, y una parecía decisiva: el
original saltaba **hacia atrás** (`FFE4`) donde nosotros saltamos **hacia delante** (`0180`).
Escribí que era estructura de control distinta.

Es falso. Las cinco **apuntan al mismo sitio**: el original guarda el desplazamiento ya
resuelto (offset 28 + 0xD8 = 244) y el nuestro guarda el *addend* (244) porque lleva
reubicación. Es la trampa que el catálogo ya tenía escrita — `dtk` pone a cero los campos
reubicados y objdiff los enmascara.

**2. Mi primera sonda de reubicaciones indexaba mal la tabla de símbolos** y daba una
correspondencia disparatada: el original referenciando `gap_08_804FF62A_sdata` donde nosotros
referenciábamos `SYSTEM_addexit`. Lo que lo delató fue que el emparejamiento era **sistemático**
—`lbl_80413EB8`↔`tmrsub` en dos offsets distintos—, que es firma de desfase de índice, no de
diferencias reales. Reescrita leyendo la `.symtab` por índice crudo, la correspondencia salió
perfecta.

La regla: **un emparejamiento sistemático entre dos listas que deberían ser distintas es un
error de indexado, no un hallazgo.**
