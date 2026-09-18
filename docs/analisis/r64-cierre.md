# r64 — cierre

## Progreso oficial verificado

| medida | antes r64 | despues r64 |
|---|---:|---:|
| unidades completas | 525 / 619 | 525 / 619 |
| bytes de DOL distintos eliminados | — | **105.447 B** |

**CERO promociones.** Y con una salvedad grave que hay que leer entera: el
`DOL OK` que verifiqué al cerrar esta ronda **era falso**. Ver el apartado final.

---

## Ocho bytes desplazaban 89.627

Es el hallazgo mayor de toda la serie y cabe en una frase. El único símbolo
descolocado de `zEagl4Anim` era `GetAttributes__CQ29EAGL4Anim6FnAnim`, **8 bytes**.

Tenía el cuerpo DENTRO de la clase en `FnAnim.h`, así que GCC 2.9 lo trata como
inline y lo drena en `finish_file` — posición 245 de 424, junto a las otras cinco
virtuales vacías que el enlace estripa. El objetivo lo tiene en la **81 de 318**,
en la ranura de su `.cpp`. Sacando el cuerpo de la clase:

    linkdelta   `.text +4  resto IGUAL`  ->  `.text +0  resto IGUAL`
    textorder   163 descolocadas de 319, 3 saltos  ->  0 y 0, ORDEN PERFECTO
    permorden   1 símbolo desplazado (ciclo de 163)  ->  0
    dolwhere    89.627 B  ->  11.309 B
    movidos     18 símbolos permutados  ->  3

Reparto de sus saltos del bloque diferido: **0 vtables** (las 26 las cerró la
r63), **0 plantillas, 1 inline**. Confirma lo que la r63 apuntó y le pone
mecanismo.

## Y rompe una veda «firme» de dieciséis rondas

Los 4 B de `Initialize` estaban vedados desde la **r47** con prueba de `stmt.c` y
**~200 combinaciones** barridas entre r47 y r60. La veda era **falsa**: las dos
rondas barrieron casos AÑADIDOS dejando `case SHT_SYMTAB:` FIJO, y hacen falta
**tres cosas a la vez** en el árbol de `switch`.

Lo mejor es que el agente lo dice contra sí mismo: **su propio barrido ciego de
1.458 combinaciones también dio CERO**. Repitió el error de la r47 antes de
arreglarlo. `Initialize` queda en **2352/2352** — tamaño ya exacto.

## Reparto

| encargo | bytes | nota |
|---|---:|---|
| eagl | 78.318 | lo de arriba |
| ecs | 9.518 | |
| track | 6.712 | **zLua queda a 1.445 B**, la más cerca del proyecto |
| diferido-phys | 4.278 | zPhysics 29.937 -> 25.659, con `fncmp` 0/718 |
| diferido-cam | 3.972 | zCamera: 8.181 B de orden + 7.337 de `.rodata` |
| world | 1.371 | |
| ai | 1.262 | |
| fe | 16 | pero medido sobre direcciones ENLAZADAS, que es lo que vale |
| plat | 0 | |

## Dos negativos que corrigen medidas anteriores

- Los duplicados de `AnimBankType`/`SkeletonType` hacían que **`linkdelta` dijera
  `.rodata IGUAL` POR COMPENSACIÓN**, no porque estuviera bien. Nos faltan 24 B
  de `.rodata` en otro sitio de la unidad.
- En `.data` el mecanismo es el CONTRARIO que en `.text`: los 12 B de
  `lbl_80417104` y `lbl_8041710C` **no pueden ir en un `asm()` de fichero**,
  porque GCC 2.9 drena los globales en `finish_file` DETRÁS de todos los `asm()`.
  Un `asm()` no se puede intercalar entre dos globales.

---

## LO QUE SE DESCUBRIÓ AL CERRAR, y es lo más importante de la ronda

El `DOL OK` con el que cerré esta ronda **era falso**, y lo fue durante varias
rondas anteriores.

Al reconstruir los objetos en la ventana de la tanda de andamios (r64b), el
enlace base pasó a `a79730eb`. La rotura es **un solo byte**, en
`OnManageTime__9QuickGameff` de **`zSim`** (unidad PROMOCIONADA), offset +47:

    80281F48   objetivo:  lfs f13, 0x4864(r11)
               nuestro:   lfs f13, 0x4870(r11)     <- 12 B tarde

`0x4864` es la mitad baja de `lbl_80404864`, que `QuickGame.cpp:682` declara
ALIAS del literal `$LC526`. El pool se corrió y `$LC526` pasó a valer `0.25f`.

**Y `lcfix` lo avisaba desde el primer día**, en una línea `FALLO` que nunca leí
porque sólo contaba los `CORRIGE`. El fichero incluso traía la directiva
`// @lcsrc zSim $LC526 3f800000` puesta ahí para vigilarlo.

La causa de que estuviera oculto: el `.o` de `zSim` llevaba rondas **RANCIO**, y
el enlace usa los objetos en disco, no la fuente. **Un `DOL OK` sobre un `.o` sin
reconstruir certifica el objeto viejo, no el árbol.**

Descartado con medida, para que nadie lo repita: no son los ficheros de la r64b
(revertidos y reconstruidos, mismo hash), no es `SimTypes.h` de la r60 (probado
con `626f2ded~1`), no es `FnAnim.h` de la r64 (zSim no lo incluye), y no es
ninguno de los 99 objetos tocados ese día (reconstruidos los 99).

Las dos entradas de protocolo que salen de aquí están en `TRAMPAS.md`.

## Aviso de herramienta

`build_direct.py --help` **no imprime ayuda: lo interpreta como «compílalo todo»**
y lanzó las 33 unidades. Varias fallaron con *«Could not open output file»* porque
otros agentes tenían los ficheros abiertos, y **dejaron el `.o` viejo en disco
reportando «0 ok, 1 fallidas»**. Con nueve agentes en paralelo eso envenena
medidas en silencio, y es una de las dos formas conocidas de fabricar un `.o`
rancio.
