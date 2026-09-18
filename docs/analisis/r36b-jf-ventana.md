# Ventana de la r36b: cero símbolos indefinidos en todo el proyecto

Trabajo hecho mientras corre la tanda, en territorio que no toca ningún agente
(Lua, Frontend). Tres cierres y un mapa nuevo.

## 1. zLua al 100 % de código: los últimos 44 B eran un NOMBRE

    536/537 funciones, 94.220/94.264 B   ->   537/537, 94.264/94.264

La función ausente era `_GLOBAL_.I.LuaRealloc`, la de inicialización de los
objetos de duración estática. La emitíamos **con el mismo cuerpo byte a byte**
—las 11 instrucciones idénticas, sólo cambia el destino del `bl`, que es
reubicación— pero llamada `_GLOBAL_.I._10LuaRuntime.mObj`.

**GCC 2.9 bautiza `_GLOBAL_.I.<X>` con el PRIMER símbolo global que emite la
unidad.** Con `LuaRuntime *LuaRuntime::mObj` definido antes que `LuaRealloc`, el
primero era `mObj`. Definiéndolo detrás, casa. Barrido a las 33 SourceLists: era
el único caso.

## 2. zFe2 vuelve a enlazar: 16 alias mangled rancios

`ENLACE FALLA` con 16 símbolos indefinidos → enlaza. **250.732 B de SourceList
que no se podían ni medir.**

Una clase declarada **dentro de una función** recibe nombres con un sufijo
numérico que sale de un contador global de declaraciones de la unidad. GCC 2.9
emite la **vtable** de esa clase —que referencia su destructor y sus virtuales—
pero **no los cuerpos**, porque están definidos dentro de la clase y eso en
GCC 2.9 no emite símbolo.

El apaño que ya había en `VehicleDB.cpp` es correcto: un `asm()` con dos
etiquetas, la del objetivo (con `.globl`) y un alias con el número que pide
nuestro compilador. Lo que no se sostiene es el número: **se desplaza en cuanto
cambia cualquier declaración anterior de la unidad**. Los alias decían `.32509`
y el compilador pedía `.32083` — 426 de diferencia, ocho clases locales, 16
símbolos. Y no se ve hasta que se intenta promocionar.

**`scripts/mangfix.py`** lo resuelve: lee los `SHN_UNDEF` del objeto recién
compilado y reescribe el alias, respetando siempre la etiqueta del objetivo.
Verificado que editar los dígitos no mueve los contadores.

## 3. zFeOverlay enlaza: `_overlay_end`

Era **el último símbolo indefinido del proyecto**. El ldscript lo tiene
comentado (`/* _overlay_end = .; */`) y ninguna unidad lo emitía. En el original
es un objeto de 8 B a cero al final de su `.rodata` —que el ldscript coloca
dentro de `.over`— y lo referencia `GameFlow.cpp`. Es la contrapartida del
`_overlay_start` que `zOnline.cpp` ya escribía a mano.

    undlist: 1 símbolo indefinido en todo el proyecto  ->  0

**Las 33 SourceLists enlazan.** Ya no hay ninguna que no se pueda medir.

## 4. El mapa nuevo de distancia al enlace

Con zFe2 y zFeOverlay medibles por primera vez:

    zAttribSys          +0  IGUAL                      <- 48.776 B, una funcion
    zGameModes          +0  rodata+104                 <- solo 124 B de premio
    zSim                +0  rodata-432 data-224
    zEcstasy            -8  rodata-760 data+64 bss+32
    zEagl4Anim         +12  rodata-864 data-96 bss-128
    ...
    zFe                 +0  rodata-3184                <- 174.200 B, 918/921 fns
    zFe2                +0  rodata-4840 data-160       <- 250.732 B (nuevo)
    zFeOverlay          +0  bss-4096 sbss+4 over-1616  <- 141.224 B (nuevo)

Y `stripmap` separa la causa en las tres del frontend:

    zFe          estripado -2880 B,  dato ausente   288 B
    zFe2         estripado -1760 B,  dato ausente  3232 B
    zFeOverlay   estripado -1920 B,  dato SOBRANTE  320 B

**En zFe el 91 % del hueco es estripado, no dato ausente.** Y `dolrod` dice de
qué: son las cadenas **`Attrib::Gen::<clase>`**, el frente catalogado en
`r35-jf-attribgen.md`. `gapchk` da 0, así que no son huecos escritos a mano sino
`$LC` del compilador: la vía es el bloque anónimo (`genrodata --anonimo`) o el
interruptor de literal muerto, no `keep.lst`.

## 5. Lo que NO he tocado, y por qué

- `zGameModes` está a `rodata+104` y al 100 % de código, pero su objeto objetivo
  **no tiene `.rodata` ni `.data` en absoluto**: el enlazador estripó toda la
  implementación de EmotionManager y sólo sobreviven las dos funciones generadas
  por el compilador. El premio son 124 B; no compensa.
- `zFeOverlay::SetScreenNames` (440 B, 91,7 %) es presión de registros con once
  valores vivos: el asignador reparte r30..r26 donde el objetivo usa r3 de
  scratch. Clase dura.
- `FEKeyboard::ToggleCapsLock` (92 B) es un empate de planificador de dos
  instrucciones en veintitrés.

---

## 6. zFe: 712 B cerrados y el resto diagnosticado al detalle

    linkdelta zFe   .text +0 rodata-3184   ->   .text +0 rodata-2472

**Lo cerrado (712 B): cadenas muertas que el objetivo sí tiene.** La ficha de la
r35 daba zFe como «faltan 0» porque comparó OBJETOS, y ahí las cadenas están
enteras. Lo que pasa es que nadie las referencia y `-strip-unused-data` se lleva
`size & ~7` de cada una, dejando una cola de 1..7 B. **En el objeto no se ve; en
el enlace faltan.**

`scripts/deadstr.py` cruza las tres cosas que hacen falta para decidir: los `$LC`
de nuestra `.rodata` con su contenido, cuáles no aparecen en ninguna reubicación
(muertos), y cuáles de esos contenidos están en la `.rodata` del objeto extraído.
En zFe: **45 de 62 cadenas muertas, 712 B de 1.080**. Las otras 17 (368 B) el
objetivo no las tiene y salvarlas sería pasarse.

**Lo que queda (−2.472 B) NO es estripado: es ORDEN DE PARSEO.** Cada
`Generated/AttribSys/Classes/<n>.h` lleva `USE_ATTRIB_ALLOC("Attrib::Gen::<n>")`,
y cc1plus emite ese literal donde se parsea. Tenemos **las 19 del objetivo y 11
de más**, en otro orden:

    objetivo (19)  simsurface, ecar, camerainfo, effects, audioimpact, audioscrape,
                   speech, pvehicle, engine, engineaudio, audiosystem, gameplay,
                   milestonetypes, aivehicle, pursuitlevels, pursuitescalation,
                   pursuitsupport, frontend, speechtune

    nuestro (30)   milestonetypes, audioimpact, audioscrape, speechtune, effects,
                   speech, emitterdata*, emittergroup*, pvehicle, simsurface,
                   chassis*, engine, induction*, nos*, tires*, transmission*,
                   gameplay, camerainfo, ecar, aivehicle, pursuitescalation,
                   pursuitlevels, pursuitsupport, presetride*, engineaudio,
                   audiosystem, frontend, acceltrans*, shiftpattern*, turbosfx*

    (*) las 11 que sobran: todas de piezas de coche.

Las dos palancas, en orden de coste:

1. **Quitar las 11 de más.** Son cabeceras que incluimos y el objetivo no. Hay
   que encontrar qué `#include` de zFe las arrastra: son un grupo coherente
   (chassis/induction/nos/tires/transmission/acceltrans/shiftpattern/turbosfx/
   presetride/emitterdata/emittergroup), así que muy probablemente entran por
   una sola cabecera de vehículo.
2. **Colocar las 19 en el orden del objetivo.** O reordenando includes, o con el
   pool escrito a mano de `brief-r36` §1 (bloque `asm()` delante de todos los
   `#include` + guarda por unidad para que cc1plus no interne su copia), que es
   como se llevó la `.rodata` de zAttribSys al 100 %.

---

## 7. El catálogo: 10.904 B de cadenas muertas en 24 unidades

`deadstr.py` pasado a las 33 SourceLists. Cadenas que **emitimos**, que el
**objetivo tiene**, y que el enlazador **se lleva** porque nadie las referencia:

    zPhysics            50  800 B     zEcstasy            28  472 B
    zSpeech             57  792 B     zFeOverlay          28  480 B
    zFe                 45  712 B     zWorld2             31  464 B
    zAttribSys (*)      41  648 B     zPlatform           27  456 B
    zFe2                42  624 B     zAI                 30  432 B
    zEAXSound           41  616 B     zGameplay           26  432 B
    zPhysicsBehaviors   38  584 B     zAnim               20  328 B
    zEAXSound2          41  584 B     zTrack              17  264 B
    zWorld              39  584 B     zSim                20  248 B
    zCamera             36  536 B     zMain               22  288 B
    zLua                25  416 B     zMisc               12  128 B
                                      TOTAL          10.904 B

**(*) CUIDADO con el filtro, y es la lección del barrido.** El criterio de
`deadstr` es «el contenido está en la `.rodata` del objeto extraído», y eso **no
basta** cuando la unidad ya provee esos bytes por otra vía. `zAttribSys` sale con
648 B en la lista y está a `linkdelta` **IGUAL** y `DOL OK`: sus cadenas ya están
escritas a mano con los nombres de `dtk`, y cc1plus emite ADEMÁS su propia copia
muerta, que se estripa y debe seguir estripándose. Salvarlas la rompería.

**Regla: aplicar sólo a unidades con DÉFICIT en `linkdelta`, y volver a medir.**
En zFe dio los 712 B exactos; en una unidad ya promocionada duplicaría.

No lo he aplicado al resto **a propósito**: `keep.lst` es global y hay cuatro
agentes midiendo `linkdelta` y `trypromo` de esas mismas unidades ahora mismo.
Cambiarlo debajo de ellos les falsea la medida. Va en la ventana, unidad por
unidad y midiendo cada una.
