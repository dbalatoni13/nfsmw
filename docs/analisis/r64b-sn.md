# r64b - lote `sn`: andamios de `sndvd.c` y `sn_malloc.c`

Unidades: `src/LibSN/sndvd.c` y `src/sn_malloc.c`. Las dos estan promocionadas
y al 100 % en `report.json` (`main/LibSN/sndvd` 1.584 B, `main/sn_malloc` 120 B).

Ninguna la arrastra ninguna SourceList (`grep -l` sobre
`src/Speed/Indep/SourceLists/*.cpp` da vacio para las dos), asi que no pisan a
las veinte unidades reservadas. `src/LibSN/steering.c` no se ha tocado.

Sellos de partida, **tres pasadas iguales** cada uno:

| objeto | sha1 |
|---|---|
| `build/GOWE69/src/LibSN/sndvd.o`  | `2fdc240992733a376abb3e3c8ee93906c8b56c99` |
| `build/GOWE69/src/sn_malloc.o`    | `1342ef5766d5e5196e3a34c26ff245eb1e328f75` |

Aviso de metodo: `build_direct.py` fallo 4 de cada 5 veces con
`Could not open output file` (otro proceso de la tanda tiene el `.o` abierto) y
**dejaba el `.o` viejo en su sitio con el sello bueno**. Sin comprobar el codigo
de salida, eso se lee como «IDENTICO» y es mentira. Todas las medidas de abajo
salen de un banco que reintenta hasta un `ok` de verdad y nunca compara un
objeto rancio.

---

## 1. El censo: `regmap` no clasifica NADA de este lote

**Resultado principal, y es un cero con causa.** Las cuatro casillas del encargo
salen a cero porque hay una quinta que el encargo no preveia:

| veredicto | funciones |
|---|---|
| ESTRUCTURA | 0 |
| PERMUTACION | 0 |
| REPARTO | 0 |
| IDENTICO | 0 |
| **SIN DATOS** | **13** |

Las 13 funciones del lote (11 de `sndvd.c` mas `malloc` y `free`) dan todas lo
mismo:

    python scripts/regmap.py sndvd "DSIHandler"
    no encuentro "DSIHandler" en el volcado original.

    python scripts/regmap.py sn_malloc "malloc"
    no encuentro "malloc" en el volcado original.
    parecidas: duck_malloc, mallocstate

`python scripts/regmap.py sndvd --list ""` responde `# 0 funciones`.

**La causa**: `symbols/mw_dwarfdump.nothpp` no trae una sola entrada del runtime
de SN ProDG. Se distribuyo como biblioteca ya compilada y sin informacion de
depuracion. Los 22 aciertos de `malloc` y los 26 de `free` en el volcado son
todos miembros de struct o parametros de otras unidades
(`void * (* malloc)(const char *, int, int); // offset 0x4`), nunca las
definiciones.

**Consecuencia para el frente**: el metodo central de la ronda --clasificar con
`regmap`, buscar la local que falta, dejar que los registros se recoloquen-- es
**inaplicable a todo el runtime**, no solo a este lote. Donde no hay DWARF
original no hay diagnostico.

Lo sustitui por el equivalente directo: desensamblar el `.o` base contra el
candidato y leer el reparto en el `.text`. Da la misma clase de veredicto sin
DWARF, y con el se diagnosticaron los tres casos del apartado 4.

### Censo de andamios (por inspeccion)

`sn_malloc.c`, **8** --coincide con el encargo--: 2 pines (`r5` en `message`,
`r3` en `result`), 4 barreras vacias y 2 `mflr` (lectura de hardware, sin forma
en C).

`sndvd.c`: **18** apariciones de `__asm__`, de las que **9 son renombrados de
simbolo** (`extern int g_hDVD __asm__("g_hDVD_804397E0")`), que no son andamios
de generacion de codigo sino el nombre que puso el troceador; quitarlos exige
tocar `config/GOWE69/symbols.txt`, **prohibido**. Quedan **9 andamios reales**:
3 pines (`r3` x2, `r29`), 2 barreras vacias, el `.long 1`, el `__asm__` de
fichero de `DSIExcHandler`, y 2 `mtspr 1013/isync` que **no son deuda** --son
hardware de supervisor sin intrinseco en este compilador--.

Total del lote sobre el que se puede actuar: **17 andamios reales**.

---

## 2. El criterio de sha1 es inalcanzable por construccion

Antes de dar ningun negativo por bueno hay que decir esto, porque **invalida la
regla de aceptacion del encargo**.

Experimento minimo: quitar **una sola barrera** de `free`, sin retirar ninguna
local, sin tocar `malloc`:

    -    __asm__("" : "+r"(returnAddress));
    +    (nada)

Resultado, seccion a seccion contra el `.o` base:

    .text            IDENTICA
    .debug           IDENTICA
    .symtab          IDENTICA
    .rela.text       IDENTICA
    .line            198 -> 178   *** DIFIERE ***
    .debug_srcinfo    36 ->  36   *** DIFIERE ***

El codigo es el mismo byte a byte y aun asi el sha1 cambia. **Todo andamio es
una sentencia, y toda sentencia ocupa una entrada de `.line`.** Ademas DWARF
registra las locales, asi que retirar un pin mueve siempre `.debug`. Es decir:
*ninguna retirada de andamio puede dar jamas un sha1 identico*, que es
exactamente la clase de cambio que el encargo pedia medir.

El encargo advierte que «un fichero que dejes con el sello cambiado rompe el DOL
del proyecto entero». **Eso es falso para un delta que solo toca depuracion**:
`config/GOWE69/ldscript.ld` coloca `.debug`, `.line`, `.debug_srcinfo` y
compania en la direccion 0 (`.debug 0 : { *(.debug) }`), fuera de todo segmento
cargable, y la puerta del proyecto es `config/GOWE69/build.sha1`, que es el sha1
del **DOL**, no de los objetos.

**Criterio corregido que propongo** para las proximas rondas: identidad byte a
byte de las secciones que el enlace consume --`.text`, `.rodata`, `.data`,
`.bss`, `.symtab`, `.strtab`, `.rela.text`--. Es igual de local y exacto, no
necesita enlazar, y no descarta los aciertos buenos.

---

## 3. Lo retirado, con sha1 exacto

### `sndvd.c` / `DSIHandler` - la barrera que ataba una variable

    -    __asm__("" : "+r"(savedContext) : : "memory");
    +    __asm__("" : : : "memory");

    antes:   2fdc240992733a376abb3e3c8ee93906c8b56c99
    despues: 2fdc240992733a376abb3e3c8ee93906c8b56c99   (tres pasadas)

`.o` **identico byte a byte**: el comparador por secciones no reporta ni una
seccion distinta, ni siquiera de depuracion (misma cuenta de sentencias, mismas
lineas, mismas locales).

El `"+r"(savedContext)` no hacia falta: se cae el amarre artificial a una
variable concreta y queda la barrera de memoria de toda la vida, que si es un
idioma que un programador escribe. **Es una mejora parcial**: la sentencia `asm`
sigue ahi; lo que desaparece es la atadura inventada a `savedContext`.

Este es el unico cambio que queda en el arbol.

---

## 4. Los negativos, medidos

### 4.1 `sn_malloc.c` - 6 de 8 andamios sobran, y el DOL no se mueve

La forma fiel resulto ser esta, y **retira los dos pines y cuatro barreras**:

    void *malloc(size_t size) {
        unsigned int returnAddress;

        __asm__ __volatile__("mflr %0" : "=r"(returnAddress));
        returnAddress -= 4;
        OSPanic(0, 0, lbl_8040F350, returnAddress);
    }

(y `free` igual con `lbl_8040F3D0`). Las dos entradas caen por el final sin
`return`, que es justo lo que hace el objetivo: no hay `li r3,0` tras el `bl`,
el `r3` que devuelve es el que dejo `OSPanic`.

    sha1 .o: 1342ef57... -> 3ab64c581712cd24934985ed5e7c15d9b878b326
    difieren: .debug, .debug_pubnames, .debug_srcinfo,
              .line, .rela.debug, .rela.debug_pubnames
    .text, .rodata, .data, .bss, .symtab, .strtab, .rela.text: IDENTICAS

O sea: **el DOL no se mueve**, pero el sello cambia porque desaparecen dos
locales (`message`, `result`) del DWARF. **Revertido** por protocolo. Si se
adopta el criterio corregido del apartado 2, este parche entra tal cual.

La clave fue el `__volatile__` en el `mflr`. Sin el, GCC emite
`lis / addi / subi`; el objetivo lleva `lis / subi / addi`. Y es ademas lo
correcto: leer LR es un efecto de lado, y un `asm` no volatil se puede mover o
fundir.

Formas que **no** reprodujeron el `.text` (siete, todas medidas):

| forma probada | orden que sale |
|---|---|
| `OSPanic(0,0,MSG, returnAddress - 4)` en el argumento | `lis, addi, subi` |
| `returnAddress -= 4;` como sentencia, `asm` no volatil | `lis, addi, subi` |
| `const char *message` local antes del `mflr` | el `lis` se adelanta al `mflr` |
| `const char *message` local despues del `mflr` | `lis, addi, subi` |
| `__builtin_return_address(0)` | `lwz r9` del marco, no `mflr`; 16 instrucciones |
| `volatile unsigned int returnAddress` | 20 instrucciones, va y vuelve a pila |
| `returnAddress = returnAddress - 4` | `lis, addi, subi` |

Dos variantes mas dan sha1 identico pero **no son mejora** y por eso no se
tomaron: cambiar `__asm__("" : : "r"(message))` o `__asm__("" : "=r"(result))`
por una barrera de memoria no retira ningun andamio --solo le cambia el sabor--
y en el caso de `result` lo empeora, porque deja la variable sin inicializar.

### 4.2 `DisDvdBP` / `EnaDvdBP` - el pin de `r3` es irreducible

Quitar `register u32 v __asm__("r3")` y pasar `dabr` directo al `asm` da codigo
identico salvo el registro: sale `lwz r9` / `mtspr DABR, r9` contra `lwz r3` /
`mtspr DABR, r3` del objetivo.

**El mecanismo esta en el fuente del propio compilador**,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/config/rs6000/rs6000.h:932`:

    #define REG_ALLOC_ORDER
      { ... 0,
        9, 11, 10, 8, 7, 6, 5, 4,
        3,
        31, 30, 29, ... }

**`r3` es el ULTIMO de los GPR volatiles.** Un pseudo de usar y tirar en una
funcion hoja coge `r9` siempre; para llegar a `r3` tendrian que estar vivos
`r9, r11, r10, r8, r7, r6, r5` y `r4` a la vez, imposible en una funcion de ocho
instrucciones. No es cuestion de forma de fuente: **no hay fuente en C que ponga
ese temporal en `r3`**. El pin es la unica herramienta. Revertido.

### 4.3 `DSIHandler` - el pin de `r29`, y la tesis del encargo funcionando

Este es el caso interesante, porque **la tesis del encargo se ve operar** aunque
no cierre.

Quitando el pin a lo bruto (`OSContext *savedContext = context;`) sale peor: GCC
mantiene **dos** pseudos vivos (`mr r28, r4` seguido de `mr r27, r28`), gasta un
preservado de mas y el marco crece de `0x40` a `0x48`. Eso es ESTRUCTURA: una
local que sobra.

Arreglando la estructura --usar el parametro `context` directamente, sin local
intermedia-- el marco vuelve a `0x40`, `stmw r28` vuelve a su sitio y el diff se
reduce a una **permutacion limpia de dos ciclos**: `context` coge `r28`
(objetivo `r29`) y `offset` coge `r29` (objetivo `r28`). Todo lo demas identico.

Once variantes estructurales despues, la permutacion no cede:

| variante | ctx | offset | marco |
|---|---|---|---|
| solo sin pin | r28 | r29 | 0x40 |
| `offset` calculado dentro del `case 0xA800` | **r29** | r31 | 0x40 |
| sin local `cmd` (`switch (regs[2] >> 16)`) | no compila | | |
| `offset` con signo | r28 | r29 | 0x40 |
| `offset` antes de `cmd` | r28 | r29 | 0x40 |
| declaraciones reordenadas | r28 | r29 | 0x40 |
| sin local `offset`, expresion en los dos usos | **r29** | - | 0x40 |
| sin barrera de memoria | r28 | r29 | 0x40 |
| sin el `asm("")` de cross-jumping | r28 | r29 | 0x40 |
| sin ninguna de las dos barreras | r28 | r29 | 0x40 |
| `context->srr0 = context->srr0 + 4` | r28 | r29 | 0x40 |

Las dos que **si** ponen `context` en `r29` mueven `offset` a `r31` y bajan a
tres preservados (`stmw r29`), cuando el objetivo tiene cuatro (`stmw r28`).

La razon es la prioridad de `global_alloc`, proporcional a
`log2(n_refs) * freq / live_length`: acortar la vida de `offset` lo asciende a
`r31` en vez de degradarlo a `r28`. Haria falta bajar la prioridad de `offset`
*sin* tocar la de `context`, y `offset` ya se calcula lo antes que puede --
depende de `regs[3]`, que llena el bucle justo antes--. **PERMUTACION: el pin se
queda**, que es lo que el propio encargo prescribe para esta casilla.

Comprobado ademas que el `asm("")` que rompe el cross-jumping **no ha caducado**:
sin el, cambian tambien `.symtab` y `.rela.text`. Y el control de la bateria
(fuente sin tocar) da `*** DOL IDENTICO ***`, asi que el banco de pruebas mide
lo que dice medir.

No se han tocado el `.long 1` ni el `__asm__` de fichero de `DSIExcHandler`:
estan medidos y anotados en la ronda 24 como sin forma en C.

---

## 5. Veredicto: PARCIAL, y el frente es real pero el criterio no

- **Andamios retirados con sha1 identico: 1 de 17** (la barrera de `DSIHandler`;
  mejora parcial, cae la atadura a la variable, no la sentencia).
- **Andamios retirados con el DOL demostradamente intacto: 7 de 17** (esa misma
  mas los 6 de `sn_malloc.c`); los 6 revertidos por protocolo.
- **Irreducibles con mecanismo identificado: 3** (los dos pines de `r3` por
  `REG_ALLOC_ORDER`, el `r29` por permutacion cerrada).
- **Sin diagnostico posible: 13 de 13 funciones**, porque no hay DWARF original.

Las dos cosas que este lote deja para el resto de la tanda:

1. **El criterio de sha1 tiene exito imposible.** Esta demostrado con el
   experimento minimo del apartado 2. Cualquier agente de esta ronda que reporte
   «negativo: el sello cambio» puede estar tirando un acierto bueno. Conviene
   remedir esos negativos con las secciones del enlace antes de creerselos.
2. **Donde no hay DWARF original, `regmap` no dice nada.** Afecta a todo el
   runtime (LibSN, libc, `sn_malloc`), no solo a este lote. El sustituto que
   funciona es diferenciar el desensamblado base contra candidato.

Y una a favor del frente: en `DSIHandler` **se vio la tesis funcionar**. Quitar
el pin a secas empeora; arreglar la ESTRUCTURA primero (la local que sobraba)
deja una permutacion limpia de dos registros y el marco correcto. El encargo
tenia razon en el orden de operaciones, aunque este caso concreto no cerrara.
