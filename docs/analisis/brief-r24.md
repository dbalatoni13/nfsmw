# Ronda 24 — instrucciones comunes

Estado: **97,35 % matched**, 18.310 funciones, **`linked` 10,07 %** (406/590),
`main.dol: OK`, las 69 huellas de `frozen.py` idénticas.

## LEE PRIMERO `docs/HERRAMIENTAS.md`

Las **tres reglas de cabecera**, la sección **8-bis** (mecanismos de la r23) y la
**9**. No repitas este documento en tu cabeza: consúltalo.

## Lo primero: VERIFICA TU ENCARGO

`build_direct.py` tus unidades y pásales `triage.py`. Si no coincide con lo que
pone aquí, **dímelo y trabaja sobre lo que midas tú**.

## La regla que más ha costado, y son DOS agentes independientes

**CON OCHO AGENTES, TU MEDIDA NO ES TUYA.**

- `measure.py --cmp` contra una base de hace una hora **no mide tu cambio**: a un
  agente le cantó **−19.800 B en tres unidades** y era otro tocando
  `UVectorMathGC.hpp`. **Toma base y medida seguidas**, o revierte y vuelve a
  medir para separar el ruido.
- **El A/B por objetos también miente**: `zMain` compilado dos veces sin tocar
  nada da `.text` del mismo tamaño con **36 bytes distintos**. El A/B de cabecera
  compartida hay que hacerlo sobre los **`.ii` preprocesados**, comprobando que
  el diff de los dos `.ii` es exactamente tu cambio.
- `pctsnap.py --cmp` **miente si el «antes» se toma sin reconstruir**: dio
  «EMPEORAN: ninguna» sobre un cambio de **−14.972 B**. La regla de
  `build_direct.py` vale para **cada** instantánea.
- `frozen.py chk` gritará en unidades que no has tocado. **No re-congeles lo que
  no es tuyo.**

## Nueve herramientas arregladas en tres rondas, y siete fallaban EN SILENCIO

No daban un dato malo: daban **menos datos de los que hay**, y eso no se ve
mirando la salida. `lreg.py` se comía **152 de 240** pseudos (los de coma
flotante); `dwbody.py` comparaba la función de **otra unidad** entre 46
homónimos; `censo.py` daba 2.544 B de trabajo fantasma. **Están arregladas** —
pero desconfía de cualquier conclusión anterior que se apoyara en ellas, y si una
herramienta te da un resultado cómodo, compruébalo por otra vía.

## Los mecanismos que abrieron la ronda 23

- **`fold` pliega `X & Y` cuando `Y` es EL MISMO ÁRBOL que un operando de `X`.**
  Regla: *si el objetivo NO pliega una redundancia que a nosotros se nos pliega,
  el original usaba una variable o un helper.*
- **Un corte de `allocno_compare` SÍ se cierra con una sentencia de fuente**: el
  DWARF contó una expansión inline de más, partir el `||` en dos `if` metió
  `d=+4` en `live_length` y `jump2` se llevó las insns. 1.820 B exactos.
- **El modelo de `global_alloc`, verificado al dígito**: la lista
  `;; N regs to allocate` del `.greg` es el orden de
  `floor_log2(n_refs)*n_refs/live_length*10000`.
- **El pool del compilador vive en un comodín: nómbralo `extern`** (escalar →
  `@sda21`, array `[]` → `@ha/@l`; elegir mal costó −73 pp). Cerró 9 unidades.
- **El ADDEND es invisible para objdiff**: una unidad daba 100 % en `measure`, 0
  FALLA en `audit`, y **dos bytes distintos en el DOL**.
- **Los volcados RTL por pase**: `ngccc` pasa `-da` a `cc1plus` pero **borra el
  temporal**. Preprocesa a un `.ii` propio y llama a `cc1plus` a mano. Corrigió
  **cinco** diagnósticos en dos rondas.

## Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida.**
2. **`audit.py`: UNA pasada al empezar**, y **confirma todo FALLA con una segunda**.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Congélalo después.
4. **Tamaño exacto = desconfía**: van **doce** casos medidos, y en dos mentía
   además el recuento de palabras.

## Método

- Antes de editar, escribe en `docs/analisis/r24-<grupo>.md`: tamaño, dirección,
  censo de llamantes (`bl` directas — dilo así), y **qué NO has probado**.
- **Ensayos numerados con su cifra.** Si no cierra, **revierte** y anota la veda
  **con la sentencia que barriste**.
- Una corrección estructural real que no cierre se queda si `measure --cmp` (bien
  hecho) demuestra que ninguna unidad baja — **y dímelo si baja el porcentaje**.
- **Antes de dar por bueno un 100 % encontrado, mide el efecto en los demás
  llamantes**: uno de la r23 costaba **−12.808 B y ocho funciones**.

## Prohibido

- **Escribir ensamblador.** Van **cuatro** retirados. Renombrar un símbolo con
  `__asm__("nombre")` sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt` sólo si tu encargo lo dice.**
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **La sonda del pin tiene CUATRO contraejemplos medidos** (−1,2 / −10,6 / −0,15
  / −17,9 pp). `pines.py` dice cuáles están en funciones que no casan; el resto
  no se tocan.

## Convivencia

Compila al scratchpad, prefija tus ficheros, y **vigila el disco**: se llenó
cuatro veces en la r22 y dos en la r23.
