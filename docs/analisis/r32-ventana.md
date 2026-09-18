# Ronda 32 — ventana

**97,974 % matched · 18.337 funciones · `linked` 13,30 % (456 de 604) · Game Code
0,88 % (4 de 33) · `main.dol` byte a byte.**

La ronda deja **tres SourceLists promocionadas** (zDebug en la r31; zDynamics y
zMission en ésta) y **tres más enlazando** (zFe, zSpeech, zPhysics: 499.212 B a
un paso).

## El paquete de bibliotecas: aplicado, roto y revertido

Tenía 19 rangos localizados. Aplicados en bloque, y **el lote no vale**. Se
revirtió entero; base verificada (`main.dol` OK, `config/` limpio).

Dos modos de fallo nuevos, los dos con mensaje exacto y los dos ajenos a si el
rango es *correcto*:

1. **Un rango no puede acabar dentro de un símbolo.**
   `Split pathbank .rodata (0x8041399C..0x80413A40) ends within symbol
   'lbl_80413A2C' (0x80413A2C..0x80413A48)`. `claimrange.py` calcula el final con
   el **último símbolo que emite NUESTRO objeto**, y el del objetivo puede ser
   más grande. Hay que redondear al final del símbolo del objetivo.

2. **Los rangos de datos fijan el ORDEN DE ENLACE, y pueden hacerlo cíclico.**
   `Cyclic dependency: math_support.c -> ... -> e_pow.c -> ...`. En `.text`
   `math_support` va antes que `e_pow`; mis dos rangos de `.sdata2` los ponían al
   revés. **Un rango con anclaje débil se detecta así**: quitando el de
   `math_support` (que ya había salido «anclaje NO único») el ciclo desaparece.

3. Y aun troceando bien, la extensión de frontera de `k_rem_pio2` **rompió el
   enlace base** (`L0039` desde `k_rem_pio2.c`), con `keepchk` subiendo de 19 a
   21 rancias — el síntoma de siempre.

**Conclusión de método: los rangos se reclaman DE UNO EN UNO, con `trypromo` y
`keepchk` entre medias.** En bloque, el primer fallo esconde a los demás y no se
sabe cuál de los ocho es el malo.

## Lo demás de la ventana

- **`filesys/AddToQueue`** (10.872 B): tres ejes barridos —8 formas de fuente, 10
  banderas, 5 de iterador— y sigue a **una instrucción**. `-fno-cse-follow-jumps`
  da el tamaño exacto y **pierde 14 funciones en realcore**.
- **`pathnode/PATHI_nextnode`** (4.204 B): `alloc.py` sobre volcados RTL propios
  da los dos allocnos (#26 `nextnode` pri 923 → r6, #27 `forreal` pri 588 → r12)
  y el umbral de 4 refs para invertirlos. **Probado y refutado**: +1 ref sube
  0,13 pp, +2 y +3 bajan a 90,52 %. **Acertar el orden no basta para acertar el
  registro.**
- **Vtables duplicadas**: 27 en el árbol, 2.464 B. Las dos curas probadas se
  descartan con cifra (el *key method* de `CODEC` cuesta 728 B y 4 funciones; el
  de `GCMessage` rompe el enlace por un segundo interruptor en la cabecera).
- **Upstream**: barridas sus 11 ramas. **Nada que importar** — 275 objetos
  `Matching` aquí contra 1 en `main`, y de los 27 de `csis` los 27 ya lo son.
