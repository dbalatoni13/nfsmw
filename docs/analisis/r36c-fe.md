# Ronda 36c — el frontend (zFe / zFe2 / zFeOverlay)

**Cuatro cierres, +1.908 B de codigo casado y un defecto de enlace de 4 B
cerrado.** Las tres unidades siguen a `.text +0` y `lcfix --check` limpio.

    zFe          184.380 -> 184.624 B al 100 %   (+244)
    zFe2         261.515 -> 263.179 B al 100 %   (+1.664)
    zFeOverlay   146.884 -> 146.884 B            (sin cambio)

    linkdelta    zFe        .text +0  rodata-2480
                 zFe2       .text +0  rodata-4216 data-160 over-16
                 zFeOverlay .text +0  bss-4096 sbss+4 over-1136

Verificado con instantanea simbolo a simbolo antes/despues de las tres unidades:
**ninguna funcion baja**. Los unicos cambios ademas de los cuatro cierres son
cuatro estaticas locales de zFe que cambian de sufijo numerico
(`iSystem.31260` -> `.31257`, y tres mas) porque quitar una declaracion de
`uiWorldMap.cpp` corre el contador de la unidad; siguen al 100 % y `undlist`
sigue dando **0 simbolos indefinidos**.

---

## 1. `FEngFont::RenderString` (zFe2, 1.572 B): 93,323 % -> 100 %

**El premio de la ficha, y no era el asignador.** `regmap` decia PERMUTACION con
cuatro locales mal repartidas (`pcString` r26/r27, `matrix` r27/r26, `c` r27/r28,
`pGlyph` r28/r29) y 61 de las 86 diferencias eran de registro. El pin de registro
—la palanca que pide la seccion 1 del brief para ese diagnostico— **no era la
respuesta**: barrido de `c` y `pGlyph` por r26..r31, ninguno cierra y casi todos
cambian el tamano.

    c=r26..r31            1.588 B   86,3 .. 91,8 %
    pGlyph=r26..r31       1.564 B   88,8 .. 96,1 %   <- tamano MAL
    c=r27 + pGlyph=r28    1.580 B   94,4 %

La causa estaba **una funcion mas abajo**, en el `PrintCharacter` inlineado, en
la asociatividad de una suma de tres terminos:

    // antes: 93,32316 %
    float s1 = static_cast<float>((pGlyph->mWidth + 1) + pGlyph->mU) / ...;
    // ahora: 100 %
    float s1 = static_cast<float>((pGlyph->mU + 1) + pGlyph->mWidth) / ...;

El objetivo hace `addi (mWidth),1` + `add mU, eso`; nosotros `addi (mU),1` +
`add mWidth, eso`. **GCC 2.9 no reasocia `(a+C)+b`: lo deja como `b + (a+C)`**,
asi que el orden de la fuente decide quien se lleva el `+1`. Las nueve
inserciones y las nueve supresiones del diff, y **las 61 diferencias de
registro**, eran todas rio abajo de esa sola expresion: con el reparto de
f0/f12/f13 correcto en el bloque de `PrintCharacter`, el asignador coloca solo
`pcString`, `matrix`, `c` y `pGlyph` en r26, r27, r27 y r28.

Barrido completo de la forma (10 variantes, mini-TU de 1,5 s):

    (mWidth+1)+mU  93,32316   mU+(mWidth+1)  93,32316   1+mWidth+mU  93,32316
    mU+mWidth+1    94,25191   (mU+mWidth)+1  94,25191   mWidth+1+mU  93,32316
    mWidth+(mU+1)  95,20611   (mU+1)+mWidth  100,00000  int(mU)+(mWidth+1) 93,32316

**Regla que sale de aqui:** cuando `regmap` dice PERMUTACION y el pin empeora o
cambia el tamano, el reparto es un SINTOMA. Busca la primera diferencia que NO
sea de registro —aqui un `addi`/`add` con los operandos cambiados— y arreglala
primero.

## 2. `SubTitler::GetElapsedTime` (zFe, 116 B): 95,517 % -> 100 %

Llevaba desde la r25 con la ficha «lo que queda es invertir la direccion de esa
copia». Eran **dos cosas a la vez** y ninguna sola bastaba:

    register float thetime_ms asm("fr1");     // (2) el pin
    ...
        float t;                              // (1) el temporal
        t = bGetTickerDifference(lastTime, timenow) * 0.001f + timeElapsed;
        lastTime = timenow;
        thetime_ms = t;
        __asm__("" : "+f"(thetime_ms));       // (2) la barrera selectiva
        timeElapsed = t;

1. El objetivo tiene **dos** pseudos donde nosotros teniamos uno
   (`fmadds f0` + `fmr f1,f0` + `stfs f0`): sin el temporal `t` salen 112 B en
   vez de 116 y el `fmr` no existe. Pero **el temporal solo no basta**: GCC
   coalesce la copia y se vuelven a quedar 112 B.
2. El pin de `thetime_ms` a **fr1** (el registro de retorno; no vive sobre
   ninguna llamada en ninguna de las dos ramas) rompe el coalescing, y la
   **barrera selectiva entre la copia y el store** hace ademas que el `stfs`
   sea sobre `t` y no sobre `thetime_ms` —sin ella CSE sustituye `t` por
   `thetime_ms` y sale `stfs f1`—. Con eso los dos `lfs` caen solos en f12/f13.

    base                             112 B  95,51724 %
    solo temporal t                  112 B  95,51724 %
    t + pin fr1, sin barrera         112 B  95,51724 %
    t + pin fr0 + pin fr1            116 B  99,13793 %   (stfs f1, orden mal)
    t + pin fr0 + fr1 + barrera      116 B  99,31035 %
    t + pin fr1 + barrera            116 B  100 %

**Lo interesante es el negativo:** pinchar `t` a fr0 —el registro que el
objetivo usa— es justo lo que impide cerrar, porque entonces la constante 0,001f
reutiliza f0 y no cae en f13. Con `t` SIN pinchar, f0 le toca sola.

## 3. `FEKeyboard::ToggleCapsLock` (zFe2, 92 B): 91,087 % -> 100 %

El «empate de planificador de dos instrucciones en veintitres» de la r36b. Eran
**dos palancas, en este orden**:

    mbCaps = mbCaps != 1;
    register bool off asm("r10") = false;   // (1) el pin: el cero nos caia en r0
    __asm__("" : "+r"(off));                // (2) la barrera de ranura, DELANTE
    mbShift = off;

1. El cero de `mbShift` nos caia en **r0** y el objetivo lo pone en **r10**
   (barrido r8..r12: solo r10 mejora, 3 -> 2 diffs).
2. Con el registro ya bueno queda el empate de emision: el objetivo emite
   `stw 0x40` / `cmpwi r9,3` / `stw 0x3c` y nosotros los dos `stw` seguidos.
   La barrera de ranura sobre el propio `off` lo desempata — **pero solo puesta
   ANTES del store**; detras no mueve nada.

    pin r10 solo                            2 diffs
    barrera sola (sin pin)                  8 diffs
    pin + barrera DETRAS del store          2 diffs
    pin + barrera DELANTE                   0 diffs   <- 100 %
    pin + barrera delante con clobber r0    0 diffs   (el clobber no hace falta)

**Matiz al brief:** la seccion 3 receta `asm("" : "+m"(v) : : "r0")` y explica
que el clobber de `r0` es lo que le da el tercer dependiente. Aqui la que gana
es `asm("" : "+r"(off))` **sin clobber**, y con `"+m"` no se mueve nada. Lo que
decidio no fue el clobber sino **de que lado del store va la barrera**.

## 4. `_vt.14ItemTypeToggle` (zFe, 128 B) y el `.text +4` escondido

Al medir `linkdelta` despues del cierre 2, zFe se fue a **`.text +4`**. No era la
funcion nueva: comparando las tablas de simbolos de los dos ELF **enlazados**
sale un simbolo que nosotros metemos en el enlace y el original no:

    SetFocus__14ItemTypeTogglePCc   4 B

`uiWorldMap.cpp` declaraba `void SetFocus(const char *parent_pkg) override {}`
—cuerpo vacio, **dentro** de la clase—. Un cuerpo en clase no emite simbolo…
**salvo si es virtual**: GCC lo necesita fuera de linea para la ranura de la
vtable. Las reubicaciones lo confirman sin ambiguedad:

    objetivo  _vt.14ItemTypeToggle +0x54 -> SetFocus__14FEButtonWidgetPCc
    nuestro   _vt.14ItemTypeToggle +0x54 -> SetFocus__14ItemTypeTogglePCc

**El original no redefine `SetFocus`.** Quitada la redefinicion: la vtable pasa
de 97,826 % a **100 %** (128 B) y el enlace vuelve a `.text +0`.

Los 4 B llevaban ahi desde siempre; estaban **enmascarados** por los 4 B que le
faltaban a `GetElapsedTime`. Los dos defectos se cancelaban y `linkdelta` decia
`+0`. Merece la pena repetir el metodo en otras unidades:

> **Un `.text +0` puede ser dos errores que se anulan.** Enlazar las dos veces y
> diferenciar las tablas de simbolos del ELF (no de los objetos) los separa en
> un minuto, y ademas dice el NOMBRE del sobrante.

---

## Lo que NO cierra, con su medida (vedas de la r36c)

### `CustomizeMain::NotificationMessage` (zFeOverlay, 808 B, 97,896 %, 9 diffs)

`regmap`: la referencia `mgr` es una local **que el original no tiene**. Pero
disolverla EMPEORA (sexta vez que pasa en el proyecto):

    base (con mgr)                            97,89604 %   9 diffs
    sin mgr (gCarCustomizeManager directo)    96,60891 %  11 diffs
    mgr como puntero                          97,89604 %   9 diffs
    mgr.IsInBackRoom() en vez de la global    igual
    mgr antes de la constante con nombre      igual
    local para el resultado de IsInBackRoom   igual
    esa local pinchada a r28                  96,58416 %  12 diffs

**El pin de `mgr` a r31 da 97,970 % y 6 diffs pero MISCOMPILA.** GCC asigna r31
tambien al temporal de `CustomizeIsInBackRoom()` (`mr. r31, r3`), que esta en el
camino que luego lee `mr r3, r31` para `RelinquishControl()`. No es utilizable:
**un pin no fija el registro contra el asignador, solo lo sugiere**, y aqui la
sugerencia gana la primera asignacion y pierde la vida del valor. Va anotado
como limite de la palanca 1 del brief.

Debajo hay prioridad de `global_alloc`: `&gCarCustomizeManager` (3 refs, rango
64..114) contra el resultado de `IsInBackRoom` (3 refs, rango 70..99). Con
`pri = log2(refs)*freq/live_length`, el de rango corto gana r31. En el objetivo
gana el otro, asi que **al original le sobra alguna referencia a
`gCarCustomizeManager` en ese `case`, o le falta vida al temporal**. Las otras 4
diffs son el par `lis/ori` del hash 0x6d5d86a1 de la rama `else` adelantado dos
ranuras (el mismo par en la rama `if` casa).

### `CustomizeMain::SetScreenNames` (zFeOverlay, 440 B, 91,727 %, 23 diffs)

Confirmado el diagnostico de la r36b y afinado: **la rama `else` del objetivo
hace el PRIMER store pronto y con `r3` de scratch**

    obj:  lis r9,LC542@ha / lis r3,g_pCustomizeSubPkg@ha / addi r9 / lis r11,LC543@ha
          / stw r9, g_pCustomizeSubPkg@l(r3)      <- store inmediato
    ours: los once `lis`+`addi` primero y los once `stw` al final, con r30..r23

Nosotros sostenemos once direcciones vivas (r30..r23) y el objetivo diez
(r30..r24) porque suelta la primera. La rama del `if` (once asignaciones
seguidas, sin `if` en medio) **casa entera**: lo que rompe es el
`if (gCarCustomizeManager.IsCareerMode())` que parte el bloque del `else`. No
tocada: es presion de registros, no forma.

### `IconScroller::IconScroller` (zFe2, 384 B, 97,771 %, 11 diffs)

Solo el ORDEN de los `stfs f0` de la inicializacion. El objetivo saca
`fCurFadeTime` (0xF8) al frente y deja `fCurrentAddPos` (0xF4) al final;
nosotros al reves. **El orden de las sentencias es IRRELEVANTE**: tres
permutaciones distintas de las trece asignaciones producen la MISMA emision byte
a byte.

    base / fCurrentAddPos al final / swap f8<->f4 / tras fHeight  11 diffs (identicos)
    fCurFadeTime primero                                          13 diffs
    fCurFadeTime o fCurrentAddPos en la lista de init             13 diffs
    fCurFadeTime = fCurrentAddPos = 0.0f  (y al reves)            11 diffs
    barrera de ranura antes de fCurrentAddPos                     11 diffs

Descartado que la cabecera mienta: `IconScroller::Update` usa `fCurFadeTime` y
`fMaxFadeTime` y casa al 100 %, o sea que los desplazamientos 0xF4/0xF8 estan
bien.

### `MemcardCallbacks::FoundEntry` (zFe, 512 B, 97,969 %, 3 diffs)

`regmap` dice **IDENTICO**. Empate de emision en la preparacion de
`GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault)`:

    obj:   lwz r4 / lis r5 / mr r6 / mr r7 / addi r5 / bl
    ours:  lis r5 / lwz r4 / addi r5 / mr r6 / mr r7 / bl

Mismas seis instrucciones, tres ciclos, orden distinto dentro del ciclo. Es el
desempate por LUID: en el objetivo la materializacion de la direccion del
literal `""` va **detras** de los `mr`, como si el literal se materializara en la
fase de cargar los registros de argumento y no en la de evaluarlos.

Probado sin exito: literal en local (15 diffs), `GetScreen()` en local (igual),
`info->mName` en local (15), quitar la local muerta `sec` (504 B, mal), barrera
de ranura antes de la llamada (16 diffs), y el **permutador ciego con las 114
variantes del catalogo** — ninguna mejora.

### `UIMemcardKeyboard::UIMemcardKeyboard` (zFe, 172 B, 92,977 %, 19 diffs)

`regmap` dice **IDENTICO** (solo dos constantes con nombre, sin registro en los
dos lados). Es un intercambio limpio de dos temporales del compilador:

    obj:   this -> r29,  pkgname / base de gButtonIDs -> r30
    ours:  this -> r30,  pkgname / base de gButtonIDs -> r29

y ademas el objetivo reutiliza el registro del `lis @ha` para el `addi @l`
(`lis r30` / `lwz r4, @l(r30)` / `addi r30, r30, @l`) mientras nosotros gastamos
un r9 aparte. Probado sin exito: cachear `GetPackageName()` en una local
(73,5 %), `gButtonIDs` en un puntero local (igual), constantes con nombre
disueltas (igual), constantes movidas de sitio (igual), el hash en una local
(168 B).

### Los datos que parecen pendientes y no lo son

`MapJoyEventToFEPad` (448 B, 87,5 %) **tiene los 448 B identicos byte a byte**:
lo que difiere son las reubicaciones del campo `Name` a los literales
(`$LCnnn` contra `lbl_XXXXXXXX`). Sospecho lo mismo de `sMovieNameMap`,
`MovieVolumeArray`, `ScreenNames`, `ScreenFactoryData`, `GameTipInfoTable`,
`LanguageInfoTable`, `MovieData` y `_7Minimap.kGameplayIconInfo`: son tablas de
punteros a cadena y su porcentaje mide el nombre del literal, no el dato. **No
son trabajo hasta que se cierre el pool de `.rodata`.**

---

## El `rodata-2480` de zFe: las 11 cadenas de mas, con dueno

Confirmado el censo de `r36b-jf-ventana.md` §6 leyendo los dos objetos:
**tenemos 30 cadenas `Attrib::Gen::` y el objetivo 19**, y las 11 que sobran
entran por cabeceras que **no son de este territorio**:

    emitterdata, emittergroup     <- Ecstasy/EmitterSystem.h
    acceltrans, shiftpattern,
    turbosfx                      <- EAXSound/EAXCar.hpp
    chassis, induction, nos,
    tires, transmission           <- Physics/Behaviors/*.h y Physics/PhysicsInfo.hpp
    presetride                    <- Frontend/Database/VehicleDB.cpp (nuestro)
                                     Y TAMBIEN Physics/PhysicsUpgrades.hpp

O sea: **de las 11, diez son intocables desde el frontend, y la unica nuestra
(`presetride`) tambien entra por `PhysicsUpgrades.hpp`**, asi que quitarla de
`VehicleDB.cpp` no la elimina de la unidad. La palanca 1 de la ficha
(«quitar las 11 de mas») **no se puede ejecutar desde este territorio**: hay que
coordinarla con quien lleve Physics, Ecstasy y EAXSound, o ir por la palanca 2
(el pool escrito a mano de `brief-r36` §1, que es lo que llevo la `.rodata` de
zAttribSys al 100 %).

---

## Metodo: dos trampas de herramienta que cuestan una tarde

**1. La mini-TU de zFeOverlay no compara nada si no se renombra la seccion.**
El objeto objetivo de zFeOverlay tiene su codigo en **`.over`**, no en `.text`
(`build_direct` aplica `section_rename = .text=.over`; `mn_repro.py` NO). Sin el
renombrado, `objdiff` empareja solo los simbolos de datos y **deja
`match_percent` a `None` en las 906 funciones**, que con `diffs=0` se lee como
un acierto. Solucion: `python tools/rename_section.py -q <objeto> .text=.over`
justo detras de `mn_repro.py`.

**2. El scratchpad esta COMPARTIDO entre los agentes de la tanda.** Escribi
`$SCRATCH/sweep.py` y a los diez minutos era el fichero de otro agente. Todos
los auxiliares de esta ronda estan bajo `$SCRATCH/c36cfe/`.

Ademas, los `PRE` que hacen falta para compilar cada fichero suelto (variable
`PRE` de `mn_repro.py`, con `|` por salto de linea):

    zFe          #define ZMAIN_MESSAGES_LUA_INLINE
                 #define ATTRIB_NO_INLINE_CLASSKEY
      uiMemcardBase.cpp      + MenuScreens/Loading/FEBootFlowManager.hpp
    zFe2         + #define ANIMWORLDTYPES_EMIT_MESSAGE_HASHES
      FEpkg_MU_Keyboard.cpp  + FEngInterfaces/FEngInterfaceFEObjects.hpp
      feIconScrollerMenu.cpp + Speed/Indep/Src/EAXSound/EAXSOund.hpp
    zFeOverlay   #define BWARE_NO_INLINE_GLOBAL_NEW
                 #define ATTRIB_NO_INLINE_CLASSKEY   + rename .text=.over

Ficheros tocados (sin commit):

    src/Speed/Indep/Src/Frontend/FEngFont.cpp
    src/Speed/Indep/Src/Frontend/SubTitle.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/InGame/FEpkg_MU_Keyboard.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/InGame/uiWorldMap.cpp
