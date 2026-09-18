# Ronda 36c — cuatro palancas nuevas contra lo que era techo

Estado: **98,20 % matched** (18.334 / 18.432 funciones), **18,64 % linked**
(476 / 617), `main.dol` `9619ba57c991` verificado byte a byte.

**Lee `brief-r36b.md` y `brief-r36.md` primero.** Esto sólo añade lo de la r36b.

## Las cuatro palancas, y cuál va con cuál

En la r36b se cerraron cuatro funciones que llevaban rondas atascadas, cada una
con una palanca distinta. **No son intercambiables**: usa la que toca.

### 1. Pin de registro — contra el REPARTO (`local_alloc`)

    register float x asm("fr7");

**Es la única palanca que llega a `local_alloc`.** `qty_compare` sólo mira refs y
LUIDs, y la fuente no los toca: por eso cuatro rondas barrieron 28 órdenes de
declaración en `UpdatePlatInfo` sin mover un byte, y con dos pines pasó de
98,659 % a **99,266 %** sin emitir un byte.

- **Uno a uno**, con el registro que nombra `regmap`, **releyendo el diff después
  de cada pin**: el mapa cambia (a `envmap_min_scale` la r28 le había asignado
  fr8 y su registro bueno es fr5).
- Sintaxis: el banco flotante es **`fr13`, no `f13`**. Un **PARÁMETRO no se puede
  pinchar** (parse error).
- **Siete pines negativos medidos**: vale con vida corta y racimo pequeño, y
  cuesta caro con la función saturada del mismo banco. Mide el tamaño también.

**Va con**: todo veredicto `PERMUTADOR` y todo `regs=` alto.

### 2. Barrera selectiva — DOS usos, no uno

    asm("" : "+f"(x));

Además de frenar un adelanto del planificador (r36), **impide el COALESCING**, y
para eso **no hace falta que haya ninguna llamada de por medio**. Ésa es la
corrección de la r36b, y así se cerró `CookieTrailCurvature`: la barrera **sola
empeora** (99,657 → 99,081) y sólo cierra combinada con dos pines:

    register float mx__ asm("fr13") = UMath::Max(...);   // el volátil del objetivo
    asm("" : "+f"(mx__));                                // impide el coalescing
    register float apex_width asm("fr31") = mx__;        // el preservado

### 3. Barrera de RANURA — contra un empate de emisión

    asm("" : "+m"(v) : : "r0");

Ocupa una **ranura**, no ata un valor. El bloque emite dos instrucciones por
ciclo y el objetivo emite **una sola** en un ciclo: hace falta un insn de cero
bytes que gane el desempate de `rank_for_schedule`, que va por **número de
dependientes**. Un `asm("" : "+m"(v))` a secas tiene 2 contra los 3 de un
`mr r3`; **el `clobber` de `r0` le da el tercero**. Cerró `GTrigger::GTrigger`.

Diagnóstico con `-dS -dR -fsched-verbose-5` sobre una **mini-TU** (un `.cpp` de
una línea que incluye el fichero: la función byte a byte en 11 s en vez de 29).
El **temporal delante es imprescindible** si hay una llamada que parte el bloque.

### 4. La etiqueta de tipo — para reproducir un BLOQUE VACÍO

    { enum { _tag }; ... }

Es la **única** construcción que fuerza un `lexical_block` del DWARF **sin emitir
una instrucción** (medido: llaves de más → no se emite; una local o un `typedef`
→ se ven; etiqueta → sube a función). Cerró el frente que la r36 dio por
imposible en `DefragmentPool`. **El volcado del original tiene 90 bloques vacíos
en 76 funciones**, así que esto se usa mucho: cada vez que `regmap`/`dwbody` diga
«el original abre un bloque que nosotros no».

### Y una quinta, de la r36b, para desbloquear una SourceList entera

`DECL_COMDAT`: una inline en clase que **sólo la llama otra inline de la misma
clase** sale una vuelta tarde y aterriza miles de bytes después. Interfaz
explícita **acotada a la clase** y bajo la macro de esa unidad. Desbloqueó
zAttribSys (+48.776 B). Se ve en `textorder --saltos`.

## Dos medidas que ENGAÑAN — léelas antes de aplicar nada

- **El fuzzy de objdiff da crédito parcial** a una fila que sólo cambia de
  registro: **puede subir mientras casan menos instrucciones exactas** (+0,097 pp
  con 690 → 680 exactas). `pctsnap` sólo ve el fuzzy.
- **El permutador guiado da falsos positivos.** Su `score` es compuesto y sube
  mientras su propio `opcodes` baja: su mejor variante de `HolePunchAvoidables`
  cuesta **tres puntos** con objdiff, y la de `RenderFlaresOnCar` **no reproduce
  al compilar la unidad entera**. **Mide con `fndiff` antes de aplicar nada
  suyo**, y mira `insn`/`opcodes`, no el `score`.

## Método (igual que siempre)

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- Ensayos numerados con su cifra; si no cierra, **revierte** y anota la veda.
- `build_direct.py <unidad>`; **nunca un `ninja` completo**.
- **Sólo los ficheros de tu lista.** Hay cinco agentes más en paralelo.
- **`lcfix.py --check` antes de terminar** si tocas literales.
- Informe en `docs/analisis/r36c-<grupo>.md`. **No commits.**
- `configure.py`, `config/GOWE69/*` y `splits.txt`: **proponer**, no aplicar.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp`. Borra tus temporales.

---

## AÑADIDO tras `c36c-snd`: tres reglas del pin y un tercer uso de la barrera

**Del pin**, las tres medidas y las tres caras:

1. **Un pin sobre una local con la DIRECCIÓN TOMADA se ignora en silencio** —
   cero error, cero aviso, binario idéntico. `grep '&nombre'` antes de gastar
   una compilación.
2. **El radio del pin es el rango de vida ENTERO, desde la DECLARACIÓN.** Pinchar
   una local declarada en la línea 1 rompió diez filas noventa filas más arriba.
3. **El pin fuerza el coalescing de las actualizaciones**: `x asm("r26")`
   convierte el `addi rY, r26, 1` + `mr r26, rY` del objetivo en un `addi` en
   sitio, y pierde 4 B.

Y **no vale en unidades de Metrowerks** (`LibSN/*`): `mwcceppc` da error de
sintaxis. Las cuatro palancas son todas específicas de GCC.

**De la barrera, un TERCER uso**: ancla un valor contra el **hundimiento a través
de una rama** (motion interbloque del Haifa). Van tres — adelanto de sched,
coalescing, y hundimiento interbloque.

**Y un aviso de triaje**: un `regs=N` alto no siempre es reparto. En
`MsgPlayMiscSound`, 8 de las 28 son `ARG_MISMATCH` sobre el **desplazamiento**
(orden de los `stw`), no sobre el registro. Mira el diff antes de pinchar.
