# Ronda 25 — instrucciones comunes

Estado: **97,40 % matched**, 18.312 funciones, **`linked` 10,25 %** (417/590),
`main.dol: OK`, las 69 huellas idénticas. Quedan **102.564 B en 40 unidades**.

## LEE PRIMERO `docs/HERRAMIENTAS.md`

Las **tres reglas de cabecera**, la **8-bis** y la **9**.

## Lo primero: VERIFICA TU ENCARGO

`build_direct.py` tus unidades y pásales `triage.py`. Si no coincide, **dímelo y
trabaja sobre lo que midas tú**.

## Lo que ha funcionado, y por qué

La ronda 24 cerró dos funciones y 11 unidades de `linked`, y **ninguna salió de
barrer formas de fuente**. Las dos salieron de **un caso mínimo de 20 líneas a
1 s por compilación** y del **volcado RTL por pase**:

- **`jump.c` iza el guardia cuya ETIQUETA DE DESTINO está seguida INMEDIATAMENTE
  por un `set` del mismo registro.** Tres rondas y 56 formas buscaron en la forma
  de los guardias; no estaba ahí. *Si sobra un `mr` a un registro duro y falta un
  `li` izado, mira qué hay detrás de la etiqueta.*
- **El número de expansiones inline del destructor en el DWARF es el número de
  `return` del original.** 21 el original, 41 nosotros → los 44 refs que decidían
  el reparto *eran* los sitios del destructor.
- **`_bOutput`: `size` eran DOS variables.** El ancho en bits ya casaba en r10 y
  el conteo de bytes quería r9; el mismo nombre no puede estar en dos registros.
- **`#pragma interface` en la cabecera Y en las privadas** quita las vtables que
  viven en un comodín. Clave: `implementation` sólo funciona **si el nombre
  casa**, no la ruta relativa.

**La receta del RTL** (corrigió **siete** diagnósticos en tres rondas): `ngccc`
pasa `-da` a `cc1plus` pero **borra el temporal**. Preprocesa a un `.ii` propio y
llama a `cc1plus` a mano; salen los 16 volcados y cada pase dice lo que hace.
Añade **`-fsched-verbose=3`** para ver las listas de listos por ciclo.

## Diez herramientas arregladas, y la mayoría fallaban EN SILENCIO

No daban un dato malo: daban **menos datos**. Dos ejemplos de la ronda pasada:

- Un detector de la r23 **nunca barrió nada** — su proceso seguía vivo horas
  después con **cero líneas escritas**. Era O(n²) y reventaba en el primer
  comodín. **El cero se lee como limpio.**
- **La caché de `dwbody.py` sólo la regenera `regmap.py <unidad> "<Clase::Func>"
  --ours`.** Ni `dwbody` ni `--list` la refrescan. Una caché de un día enseñaba
  una local inexistente **y escondía un hallazgo real**.

**Si una herramienta te da un resultado cómodo —sobre todo un cero— reproduce
primero un caso conocido.** Esa autoprueba cazó tres fallos propios al agente del
addend antes de que se fiara de su barrido.

**Y la llave importa**: comparar por *(sección, desplazamiento)* **no vale** en
las SourceLists —nuestro `.text` mide decenas de kB más y todo va corrido—. Dos
herramientas cayeron en eso y yo estuve a punto de hacerlo por tercera vez.
`gcc2_compiled.` sale **389 veces** en `symbols.txt`: resolver una dirección a
nombre a ciegas da falsos positivos en masa.

## Con ocho agentes, tu medida no es tuya

- **Base y medida seguidas**, cada una con `build_direct.py` delante. A un agente
  le cantó **−19.800 B** que eran de otro.
- **A/B de cabecera compartida sobre los `.ii` preprocesados**: `zMain` compilado
  dos veces sin tocar nada da **36 bytes distintos**.
- `pctsnap --cmp` miente si el «antes» se toma sin reconstruir.
- `frozen.py chk` gritará en unidades que no has tocado. **No re-congeles lo que
  no es tuyo.**
- **Un diagnóstico guardado hace horas hay que reconfirmarlo**: un hallazgo se
  cayó porque otro agente reconstruyó el objeto en medio.

## Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida.**
2. **`audit.py`: UNA pasada al empezar**, y **confirma todo FALLA con una segunda**.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Congélalo después.
4. **Tamaño exacto = desconfía**: van **doce** casos, y en dos mentía además el
   recuento de palabras.

## Método

- Antes de editar, escribe en `docs/analisis/r25-<grupo>.md`: tamaño, dirección,
  censo de llamantes (`bl` directas — dilo así), y **qué NO has probado**.
- **Ensayos numerados con su cifra.** Si no cierra, **revierte** y anota la veda
  **con la sentencia que barriste**.
- **Antes de dar por bueno un 100 %, mide el efecto en los demás llamantes**: uno
  costaba **−12.808 B y ocho funciones**.
- Si tu palanca es un constructo que el original no tenía, **dilo**. Esta sesión
  ha retirado cinco invenciones nuestras y ha dejado dos puestas *marcadas como
  deuda*; lo que no vale es dejarlas sin decirlo.

## Prohibido

- **Escribir ensamblador.** Renombrar un símbolo con `__asm__("nombre")` sí vale.
- **`configure.py`, `config/GOWE69/*` y `splits.txt` sólo si tu encargo lo dice.**
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **La sonda del pin tiene CUATRO contraejemplos medidos.** `pines.py` dice
  cuáles están en funciones que no casan.

## Convivencia

Scratchpad con tu prefijo, y **vigila el disco**: se llenó cuatro veces en la r22
y dos en la r23. Borra tus volcados grandes al terminar.
