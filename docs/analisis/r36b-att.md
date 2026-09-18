# zAttribSys: `DOL OK`. La palanca era la interfaz explícita, y es UNA LÍNEA

    zAttribSys   textorder  ORDEN PERFECTO (0 descolocadas de 197, 0 saltos)
                 trypromo   DOL OK
                 dolwhere   DOL OK
                 measure    48.776 / 48.776 B   100,0000 %   196 funciones

**48.776 B de código listos para promocionar.** La única unidad con `linkdelta`
cero deja de dar `DOL ROTO`. Un solo fichero tocado
(`src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribHashMap.h`, +17 líneas) y
**cero efecto en las otras 24 unidades** que ven esa cabecera (comprobado byte a
byte contra `zRender.o` y `zMiscSmall.o`).

## 1. El diagnóstico del brief estaba en la línea equivocada del compilador

El brief apuntaba a `toplev.c:3012` (`flag_keep_inline_functions || TREE_PUBLIC
&& !DECL_COMDAT || TREE_SYMBOL_REFERENCED`) y por eso el **ensayo 4**
(`-fkeep-inline-functions`) parecía decisivo y negativo: con la bandera la
condición se cumple para todas las inline y `Transfer` seguía saliendo al final.

La condición que manda está **antes**, en `wrapup_global_declarations`
(`toplev.c:2967`):

    if (TREE_ASM_WRITTEN (decl) || DECL_EXTERNAL (decl))
      continue;

`DECL_EXTERNAL` se queda a 1 hasta que lo baja el bucle de `finish_file`
(`cp/decl2.c:3783`), y ahí la bandera no pinta nada:

    if (DECL_NOT_REALLY_EXTERN (decl)
        && DECL_INITIAL (decl)
        && (TREE_SYMBOL_REFERENCED (DECL_ASSEMBLER_NAME (decl))
            || !DECL_COMDAT (decl)))
      DECL_EXTERNAL (decl) = 0;

O sea que **`-fkeep-inline-functions` no podía funcionar nunca**: el filtro que
descarta a `Transfer` está una función más arriba y la bandera no lo toca. Los
cuatro ensayos del brief barrían la fuente; la causa estaba en el **enlace** del
símbolo.

## 2. La causa: comdat

`TREE_SYMBOL_REFERENCED` lo pone **sólo** `assemble_name` (`varasm.c:1855`), es
decir, cuando el nombre se escribe de verdad en el `.s`. Como la única llamada a
`Transfer` sale de `RebuildTable` —comprobado leyendo las reubicaciones de
`.rela.text` de los DOS objetos: **una y sólo una**, y en los dos desde
`RebuildTable`— el nombre no se emite hasta que se emite `RebuildTable`, que es
**dentro** de la pasada. Para cuando pasa eso el bucle de `decl2.c:3783` ya se
ejecutó, así que `Transfer` cae a la vuelta siguiente del `do { } while
(reconsider)` y aterriza 6.740 B más tarde, justo delante del bloque de
sintetizadas (`__Q36Attrib12ClassPrivate17CollectionHashMapUi`, los destructores
y `_GLOBAL_.I`), que es la vuelta 2.

Queda la otra mitad de la condición: **`!DECL_COMDAT`**. Y ahí sí se puede
actuar. `import_export_decl` (`cp/decl2.c:2876`) reparte:

| caso | ruta | `DECL_COMDAT` | binding |
|---|---|---|---|
| clase **sin** interfaz conocida | `comdat_linkage` | **1** | `.weak` |
| clase **con** interfaz conocida | `maybe_make_one_only` | **0** | `.globl` |

`maybe_make_one_only` no pone `DECL_COMDAT` a propósito, y lo dice en su
comentario: *«We can't set DECL_COMDAT on functions, or finish_file will think
we can get away with not emitting them if they aren't used»*. Con
`DECL_COMDAT == 0` la condición de `decl2.c:3789` se cumple **en la primera
vuelta y sin depender de nadie**, y todas las inline de la clase salen en su
índice de `saved_inlines`, que es el orden de fuente.

`Attrib::HashMap` no tiene virtuales, así que `import_export_class` no le
adivina la interfaz por la regla del *key method*: hace falta el `#pragma`.

### El indicio que lo delató

Contando los *bindings* de los símbolos:

    objetivo zAttribSys : 192 GLOBAL FUNC,   0 WEAK
    nuestro  zAttribSys : 135 GLOBAL FUNC,  60 WEAK

**Ojo, esto NO es prueba**: los objetos de `build/GOWE69/obj` los fabrica `dtk
dol split` desde el DOL, y ahí no existe el concepto de `weak`. Ningún objeto
extraído del proyecto tiene un solo símbolo débil. Sirvió como pista, no como
medida. La medida es la del apartado 4.

## 3. El arreglo, y por qué está acotado a la clase

Los pragmas de GCC 2.9 son por FICHERO, no por clase, pero se pueden encender y
apagar en mitad de uno: `#pragma interface` pone `interface_unknown = 0` y
`#pragma implementation` lo vuelve a poner a 1 (`cp/lex.c:4995`). Con eso el
alcance se acota a las clases que queden **entre los dos**.

Hacen falta tres pragmas seguidos porque `#pragma implementation` sólo se admite
fuera del fichero principal si ya se vio un `#pragma interface`
(`write_virtuals == 3`), y porque `interface_only` se decide comparando el nombre
contra la cadena de ficheros implementados:

    #pragma interface "__attribsys_no_such_file__"   // write_virtuals = 3
    #pragma implementation "AttribHashMap.h"         // mete la cabecera en la cadena
    #pragma interface                                // interface_only = 0, unknown = 0
    class HashMap { ... };
    #pragma implementation                           // unknown = 1: se acabó el alcance

Y el bloque entero va bajo `#ifdef ZATTRIBSYS_HAND_POOL` —la macro que
`zAttribSys.cpp:522` ya define— por lo del apartado 4.

Sale un aviso, inofensivo:
`warning: '#pragma implementation' for "AttribHashMap.h" appears after its #include`.

## 4. Por qué la guarda: medido

Sin guarda, las 24 unidades restantes que ven la cabecera emiten también todas
las inline de `HashMap`, y GLOBAL en vez de WEAK:

    zRender      29 fns / 2.864 B  ->  55 fns / 7.624 B   (+26 fns, +4.760 B)
    zMiscSmall   30 fns / 3.292 B  ->  56 fns / 8.052 B   (+26 fns, +4.760 B)

Con la guarda puesta, `zRender.o` y `zMiscSmall.o` vuelven a ser **byte a byte
idénticos** a los de antes de tocar nada (md5 comparado). Las 25 unidades
afectadas, sacadas del grafo de `#include`: zAI zAnim **zAttribSys** zCamera
zEAXSound zEAXSound2 zEagl4Anim zEcstasy zFe zFe2 zFeOverlay zGameplay zLua zMain
zMisc **zMiscSmall** zPhysics zPhysicsBehaviors zPlatform **zRender** zSim
zSpeech zTrack zWorld zWorld2. De ésas, las únicas que hoy están en el enlace son
zRender y zMiscSmall, y son las dos que verifiqué.

## 5. Lo que cuesta, y por qué no cuesta nada

Ahora emitimos **23 funciones que el objetivo no tiene** (3.132 B): las tres
muertas que ya había (`Get__Q26Attrib8Database`, `IsInitialized__Q26Attrib8Database`,
`__dl__Q26Attrib8DatabasePvUi`) más 20 inline de `HashMap`/`HashMapTablePolicy`
que nadie llama (`KeyIndex`, `WrapIndex`, `Alloc`, `Free`, ...). Todas MUERTAS:
`-strip-unused-data` se las lleva y el DOL sale **byte a byte**.

Por eso `promote.py` la rechaza y hay que ignorarlo aquí:

    - .text mide 51908 B y el extraido 48776 B
    - 35 simbolo(s) en OTRO SITIO de su seccion

Los 35 son el mismo desplazamiento de +3.132 B, y desaparece al estripar.
**`trypromo` y `dolwhere` mandan, y los dos dicen `DOL OK`.** El orden relativo
de las 197 funciones vivas es exacto (`textorder`: ORDEN PERFECTO).

## 6. Lo que hay que hacer ahora (no lo hago yo: `configure.py` está vedado)

Marcar `Speed/Indep/SourceLists/zAttribSys` como `Matching` en `configure.py`.
Verificado por adelantado con `trypromo.py zAttribSys` y `dolwhere.py zAttribSys`:
los dos `DOL OK`. Son **+48.776 B de `linked`**.

## 7. El frente que esto abre

La regla es general y no la teníamos: **una inline en clase que sólo llama otra
inline de la misma clase sale una vuelta tarde de `finish_file`**, y eso mueve
kilobytes de `.text` sin cambiar un solo byte de código. Se reconoce a ojo en
`textorder --saltos`: un salto de delta grande cuyo símbolo es una función
pequeña que en el objetivo va pegada a su llamante.

Reproducido en 20 líneas con el compilador de verdad (`ngccc -O1 -fno-inline`),
que es la prueba real de todo lo de arriba:

    class M { void trans(int); void rebuild(int){...trans(i)...}; void upd(int); };
    void drive(M&m,int k){ m.rebuild(k); m.upd(k); }

    sin pragmas : drive, .weak rebuild, .weak upd, ......, .weak trans   <- trans al final
    con pragmas : drive, .globl trans, .globl rebuild, .globl upd        <- orden de fuente

Y el alcance también está probado ahí: una clase declarada antes del
`#pragma interface` y otra después del `#pragma implementation` siguen `.weak` y
siguen mandando su callee a la vuelta 2. Las vtables (`_vt.1V`, `_vt.1W`) no se
mueven de sitio, o sea que `write_virtuals = 3` no repite aquí lo que le pasó a
zBWare con el par `interface`/`implementation` a nivel de fichero.

Candidatas: cualquier unidad cuyo `textorder --saltos` tenga un salto con nombre
de inline en clase. Con la guarda por unidad el riesgo es cero.

---

# `ActualReadJoystickData`: cinco formas más barridas, todas negativas

Sigue en **1.580 B / 99,244 %** contra 1.588 B del objetivo. **Revertido.**

## Lo que son las dos instrucciones que faltan

Están localizadas al byte, en el bloque de `stickX` (`+0x1378` y `+0x1380`):

    objetivo                          nuestro
    189 extsh r0, r11                 extsh r0, r8
    190 mr    r9, r0        <-- FALTA
    191 stb   r0, 0x4(r29)            stb r0, 0x4(r29)
    192 andi. r11, r9, 0x8000  <-- FALTA
    193 lbz   r0, 0x33(r29)           lbz r0, 0x33(r29)
    194 mr    r9, r11                 mr r9, r11

Corrección importante sobre cómo se leen: en PowerPC **`andi.` es la única
and-inmediata que hay**, no existe una forma que no toque CR0. Así que **no falta
ningún salto**: no es una condición, es `v & 0x8000` calculado a un registro. Ese
registro (`r11`) es el que alimenta los cinco `mr r9/r10/r8/r6/r7, r11` de las
líneas 194-203, que están en los DOS objetos y **están muertos** (r9, r8 y r7 se
recargan de `0x14(r1)` antes de usarse). En el nuestro esos cinco `mr` leen el
`r11` que dejó el `lwz r11, 0x14(r1)` de la línea 179.

O sea: al original le sobra un pseudo-registro con el valor `v & 0x8000` que no
usa nadie. Es un residuo del asignador, no una sentencia que nos falte.

## Las cinco formas barridas (anotadas también en `JoyE.cpp`)

Todas sobre el bloque de `stickX`, midiendo con `fndiff`:

| # | forma | tamaño | % |
|---|---|---|---|
| A | `joy_data->stickX = (short)data;` y `v = (short)data;` detrás | 1.576 B | 99,055 % |
| B | el store y el `if` **dentro** del bloque de `data` | 1.580 B | igual |
| C | `v = v & 0x8000;` añadido al final del bloque | 1.580 B | igual |
| D | `v = v & 0x8000; if (v) v = 0;` en vez del `if` | 1.580 B | igual |
| E | store sin cast y `v = (short)data` aparte | 1.576 B | 99,055 % |

Sumadas a la del brief (quitar las cuatro locales `data`: 1.568 B / 98,237 %),
son **seis formas** de la misma sentencia. `regmap` señala esas cuatro locales
como «SOLO NUESTRA», pero eso ya está descartado: quitarlas aleja, y el bloque
`{int data = v; if ((short)data > 255) data = 255;}` reproduce **exactamente** la
forma del objetivo en substickX/substickY (`mr r10,r0; extsh r0,r10; cmpwi
r0,0xff; ble; li r10,0xff; stb r10,7(r29)`), sólo que con otro registro duro.

**VEDA: la forma de la sentencia de `stickX` está agotada.** Lo que queda por
mirar es la presión de registros de todo el cuerpo (nosotros metemos las cuatro
locales en r8/r8/r8/r9 donde el original usa r10 y r11), no esas once líneas.

## De regalo, tres diferencias de 0 B que sí son de fuente

    310  add  r31, r29, r27   |  add  r31, r27, r29
    334  sthx r0,  r29, r27   |  sthx r0,  r27, r29
    352  add  r10, r29, r27   |  add  r10, r27, r29

Es el orden de los operandos de `((LGPosition *)(void *)joy_data + slot)`: el
objetivo suma `joy_data + slot` y nosotros `slot + joy_data`. No mueve un byte,
pero es lo único de esta función que sigue siendo un problema de fuente y no de
asignador.
