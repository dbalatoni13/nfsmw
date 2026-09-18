# `order:` de `splits.txt` no sirve para los ciclos, y el fuente de ProDG no trae el enlazador

Dos encargos, dos respuestas negativas útiles y un premio de consolación que vale
más que lo que se buscaba.

## 1. `order:` — probado y descartado

`r35-web2` lo daba por documentado «exactamente para los ciclos que dan los rangos
de datos». Existe, pero **no hace eso**.

**Dónde va.** El propio `dtk` lo delata en su cadena de serialización:

    ":\n" "\t" " comment:" " order:" " start:" " end:" " align:" " common" " rename:" " skip"

Puesto en la **línea de sección** da `Unknown split attribute 'order'`. Va en la
**línea de la unidad**, y ahí se acepta sin queja:

    Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface.cpp: order:700

**Que no funciona, medido en tres ensayos.** El caso es el ciclo que `lib2` dejó
sin resolver en la r35 (`input/cmn/interface`, rango `.rodata`
`0x80414DD0..0x80414E80`), que reproduce siempre:

    Cyclic dependency encountered while resolving link order:
      interface.cpp -> memvectors.cpp -> gc_device.cpp -> auto_05_80414CE0_rodata

| ensayo | qué | resultado |
|---|---|---|
| 1 | `order:900` sólo en `interface.cpp` | **CICLO**, idéntico |
| 2 | `order:` 700/710/720 en las tres unidades con nombre | **CICLO**, idéntico |
| 3 | absorbido el rango intermedio (`0x80414CE0`) + `order:` en las tres | **CICLO**, y ahora sin ninguna unidad autogenerada dentro |

El ensayo 3 es el que cierra el asunto. Absorber el rango intermedio **sí** cambia
algo: el ciclo se acorta y deja de pasar por `auto_05_80414CE0_rodata`, así que
las tres unidades implicadas son ya unidades con nombre, todas capaces de llevar
`order:`. Y aun así el ciclo persiste **igual**. O sea que **la detección de
ciclos corre sobre el grafo derivado y `order:` no interviene en ella**.

**Decisión: no usarlo para esto.** Sirve para desempatar unidades sin restricción
derivada, no para romper un ciclo. La palanca buena sigue siendo la de la r35:
mirar qué extremo del rango contradice el orden del `.text` y **reasignar el rango
a su dueño real**, que es como `lib2` resolvió el de `gc_interface` (era la vtable
`_vt.Q26Realmc18BlockCalculatorImp`, y pertenecía a `gc_blockcalculator.cpp`).

Árbol restaurado y verificado: `splits.txt` con `git status` vacío, `dtk dol
split` 617 objetos, `ninja` rc=0 y `main.dol` `9619ba57…`.

## 2. El fuente de ProDG: no trae `ngcld`, trae algo mejor

Bajado `NGC/ProDGforNGCv393_Source_Code.zip` (28.626.142 B, item
`sn_sys_consoles_2` de archive.org), extraído en **`orig/prodg/`** — que git
ignora por el patrón `orig/*/*`, así que no contamina el repo.

**Las dos preguntas que iba a contestar siguen sin contestar**, y su propio README
dice por qué. Lo que SN libera es sólo la parte GPL:

    cpp.exe · cc1.exe · cc1plus.exe · make · sed · sh · cygwin1.dll · libgcc.a

y añade: «*All other software distributed in the ProDG for Nintendo Gamecube
package is copyright of SN Systems and is subject to the SUL*». **El enlazador
`ngcld` y el ensamblador `ngcas` no están.** Así que la aritmética `size & ~7` del
estripado y el marcador `0xFFFFFFFF` **siguen siendo sólo medida nuestra**, y hay
que seguir tratándolos como tal.

**Pero está el compilador entero, y es el nuestro.** `NGC_GNU_SRC.zip` trae 2.034
ficheros con el front-end de C++ completo (`gcc/cp/decl2.c` con `finish_file` en
la línea 3643, `class.c`, `method.c`, `varasm.c`, `toplev.c`), con las cabeceras
de revisión de SN (`$Archive: /Dolphin/gnu tools/gcc/gcc/version.c $`). Y la
identificación es **por construcción**, no por parecido:

    version.c:135  char *version_string =
        "2.95.3 SN BUILD v" MAJOR_VERSION_S "." MINOR_VERSION_S " for Nintendo Gamecube";

que es exactamente el `AT_producer` que leemos en el DWARF del juego:
`GNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube`.

Lo que eso habilita: todo el frente del **orden de emisión** deja de ser deducción
y pasa a ser lectura del fuente que compiló el juego — `finish_file` y sus
pasadas, el bloque diferido de inline, el orden de las vtables, la síntesis de
destructores implícitos y la emisión del pool de constantes. Y sobre todo permite
**diferenciar contra el GCC 2.95.3 de serie** para aislar las modificaciones de
SN, que son justo las que no se pueden adivinar.
