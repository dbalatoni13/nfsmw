# Ronda 20 — instrucciones comunes

Estado: **96,85% matched**, 18.281 funciones, `linked` 7,92%, `main.dol: OK`.

## LEE PRIMERO `docs/HERRAMIENTAS.md`

Está todo ahí: qué herramienta usar para cada caso, cómo se invoca, cuál está
superada por cuál, y las trampas medidas. **No repitas este documento en tu
cabeza: consúltalo.**

## Lo que cambia en esta ronda, y por qué

La ronda 19 cerró 18 funciones. **Dieciséis vinieron de unidades que estaban a
cero** (11.500 B) y **dos de near-miss** (916 B) — con seis de ocho agentes
asignados a near-miss. El reparto estaba mal, y era mío.

La causa: `censo.py` dice que en las SourceLists quedan **16 B sin escribir**. El
frente barato del código del juego está agotado. Lo que queda ahí son **muros del
asignador**: 26 unidades y 98.076 B a una o dos instrucciones, donde barrer
formas de fuente no abre nada.

**Lo barato vive en el middleware**: 30 unidades y 21.124 B por debajo del 90%,
diez de ellas a cero. Ahí **cada función escrita es una función cerrada**.

Por eso esta ronda va **seis agentes a middleware y dos a near-miss**, y los dos
con un mecanismo concreto identificado, no por estar al 99%.

## Las tres reglas de esta ronda

1. **`build_direct.py` antes de cualquier medida.** `ninja` no construye el
   middleware (cero reglas para `libc/`, `LibSN/`, `Libs/path/`) y `measure.py`
   no compila. Me ha mordido dos veces esta sesión.
2. **`audit.py`: UNA pasada al empezar, no la ronda entera.** Ha encontrado más
   de treinta funciones al 100% que estaban mal —incluido el remapeo del volante—
   y eso vale, pero **cuesta 0 bytes**. Tráeme la lista y sigue con lo tuyo.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Y congélalo después
   con `frozen.py cong`.

## Escribir desde cero: el método que funciona

Es el frente más productivo del proyecto, y `eaxadecf` lo demostró la ronda
pasada: **1.316 B y 9 funciones, con cuatro ya al 100% en la primera
compilación**. En este orden:

1. **`libdwarf.py cu <fichero>`** para el middleware: da las structs con el
   nombre y el desplazamiento de cada campo. Y `fuse.py`, que funde el
   desensamblado con el mapa de líneas: **convierte escribir en transcribir**.
2. **Las unidades hermanas ya cerradas**, para el estilo y las convenciones.
   Media función suele estar ahí.
3. El asm original en `build/GOWE69/asm/`.

## Método

- **Antes de editar**, escribe en `docs/analisis/r20-<grupo>.md`: tamaño e
  instrucciones, dirección, censo de llamantes (`bl` directas — dilo así), los
  llamados y su contrato, **y qué NO has probado**.
- **Ensayos numerados** `c1`, `c2`… **cada uno con su cifra medida**. Los
  descartados no quedan en el árbol.
- **Si no cierra, revierte** y anota la veda diciendo **qué sentencia barriste**.

## Lo que está prohibido, y por qué

- **Escribir ensamblador.** No cierra una función, la falsifica. Se han retirado
  **tres** casos esta sesión, uno «al 100%» con un salto a mano (`asm("b .+12")`)
  que costó devolver 144 B falsos.
- **Restricciones de registro**: último recurso, tras documentar la búsqueda de
  la forma, sólo vacías de entrada, y **tienen que pasar `audit.py`**. Aviso: un
  pin nuestro llegó a ser **la causa** de que una función no casara — quitarlo la
  cerró.
- **`configure.py` sólo si tu encargo lo dice**, y **siempre con `trypromo.py`
  antes**: promocionar rompe el DOL aunque la unidad esté al 100%. Cinco unidades
  de libm lo hacen.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos** sobre todas las unidades que la
  incluyen. Y **el md5 del `.o` no vale** para eso: lleva el debug.

## Convivencia

El árbol es compartido y hay ocho agentes. **Compila tu `.o` al scratchpad**,
nombra tus ficheros con un prefijo propio, y **confirma todo fallo de `audit.py`
con una segunda pasada**: si otro recompila en medio salen fantasmas.
