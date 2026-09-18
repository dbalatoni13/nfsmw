# Ronda 23 — instrucciones comunes

Estado: **97,26 % matched**, 18.308 funciones, **`linked` 9,24 %** (381/590),
`main.dol: OK`, las 64 huellas de `frozen.py` idénticas.

## LEE PRIMERO `docs/HERRAMIENTAS.md` (secciones 8 y 9)

## Lo primero: VERIFICA TU ENCARGO

Los números los saqué hoy con `triage.py` sobre objetos recién construidos. Aun
así: **`build_direct.py` tus unidades, pásales `triage.py`, y si no coincide,
dímelo y trabaja sobre lo que midas tú.**

## La ronda 22 cerró CERO bytes. Lee esto antes de empezar

Ocho agentes, ~700 ensayos numerados, y **ni un byte de código**. Lo que sí dio
fueron mecanismos, promociones y **seis herramientas arregladas**. La lección
para ti:

- **Un near-miss al 99 % que lleva tres rondas no se abre barriendo formas.** Si
  tu función ya tiene 20+ ensayos anotados, no hagas el 21: ve al volcado RTL.
- **`ngccc` pasa `-da` a `cc1plus` pero borra el temporal.** Preprocesa a un
  `.ii` propio y llama a `cc1plus` a mano: salen los 16 volcados por pase, y cada
  pase dice lo que hace (`.gcse` imprime «N copy props»). La receta está en
  `docs/analisis/r22-ae.md` §2. Eso corrigió **tres** diagnósticos de la r21.
- **Un caso mínimo de 40 líneas a 1 s por compilación** vale más que barrer sobre
  la unidad entera. Con eso se cerró `HandleTriggers` y se demostró que
  `Effect_Init` es muro.

## Cinco herramientas que mentían, ya arregladas — y la que aún miente

| herramienta | qué le pasaba |
|---|---|
| `dwbody.py` | leía un volcado que **no se regenera solo** Y cogía en silencio el primer cuerpo de **46 homónimos**. Ahora avisa y admite un selector |
| `lreg.py` | se comía `-G0`, que es la bandera que decide el reparto |
| `audit.py` | no resolvía `@N_ADDR` ni `gcc2_compiled._ADDR` |
| `censo.py` | no aplicaba los renombres de `configure.py` |
| `promote.py` | **demasiado conservador**: da 3 «LIMPIA» donde el enlace real da 16 |
| **`audit.py`, aún** | no resuelve nuestros `$LC` a una dirección, así que canta FALLA cuando el objetivo aparca un `@ha` en LR y el troceador no puede emparejarlo. **Confirma siempre con los bytes** |

**`triage.py` saca la lista de opcodes desordenada y truncada a cuatro**, distinta
en cada pasada: los números de `faltan/sobran/SUST` sí son fiables, la lista de
opcodes **no**.

## Cuatro invenciones nuestras retiradas en dos rondas

Un pin de registro inerte, ensamblador falsificado, un `asm` de más y un
`volatile` de facto. **Si tu función lleva algo nuestro que el original no
tiene** —un `asm`, un `register`, una barrera— **mídelo quitándolo antes de
barrer nada más**. Con dos matices medidos: `pines.py` dice qué pines están en
funciones que no casan (de 193 sólo dos lo estaban), y **quitar el pin tiene dos
contraejemplos** (−1,2 pp y −10,6 pp): es una sonda, no una ley.

## Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida.**
2. **`audit.py`: UNA pasada al empezar**, y **confirma todo FALLA con una segunda**.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Congélalo después.
4. **Cuando una variante dé el tamaño exacto del objetivo, desconfía**: van
   **once** casos medidos en que empeora, y uno en que también mentía el
   recuento de palabras.

## Método

- **Antes de editar**, escribe en `docs/analisis/r23-<grupo>.md`: tamaño,
  dirección, censo de llamantes (`bl` directas — dilo así), y **qué NO has probado**.
- **Ensayos numerados con su cifra.** Si no cierra, **revierte** y anota la veda
  **con la sentencia que barriste**.
- Una corrección estructural real que no cierre se queda si `measure.py --cmp`
  demuestra que ninguna unidad baja — **y dímelo si baja el porcentaje**.

## Prohibido

- **Escribir ensamblador** (van cuatro casos retirados). Renombrar un símbolo con
  `__asm__("nombre")` sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt` sólo si tu encargo lo dice.**
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos** sobre todas las unidades que la
  incluyen — y calcula el cierre transitivo: `SAP.h` parecía llegar a 3 unidades
  y llega a **20**.

## Convivencia

Ocho agentes. Compila al scratchpad, prefija tus ficheros, y **vigila el disco**:
se llenó **cuatro veces** en la ronda 22 y `No space left on device` se lee como
«la variante no cambia nada». Borra tus volcados grandes al terminar.
