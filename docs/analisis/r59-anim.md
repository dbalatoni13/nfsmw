# r59 — agente `anim`: zAnim **2.697 -> 0 B**, `trypromo` **DOL OK**

Unidad: `Speed/Indep/SourceLists/zAnim` (propiedad exclusiva).
`.o` final `862348568c5417ed19ed877c8cc0e75e9a4b43c0`, **tres compilaciones con el mismo sello**.

    fncmp        ANTES 0 de 315 con el codigo distinto   DESPUES 0 de 315
    permorden    315 de 315 en su sitio, 0 ciclos        (igual que antes)
    dolwhere     2.697 -> 0 B
    agent_anim_gaps  objetivo 197 literales / 3708 B   nuestro 197 / 3708 B
    trypromo     DOL OK   <-- con keep.lst pasado por `lcfix.py`
    linkdelta    .text +0   rodata -96   <-- con el keep.lst RANCIO de HEAD

**El unico paso que falta para promocionar no es de fuente: es correr
`python scripts/lcfix.py`.** Ver §5.

---

## 0. El estado de partida y lo que se ha hecho

La r58 dejo zAnim con el ORDEN cerrado (21 vtables en su direccion exacta, 315/315
funciones) y **2.697 B de `.rodata` de contenido**: 780 B que SOBRABAN por duplicado,
1.031 B que FALTABAN en once bloques y un puñado de CADAVERES. El mapa estaba al byte y
la r58 lo describia como «un paquete atomico que no es media tarde».

Lo es, pero se cierra. Y la mitad del trabajo ya estaba escrito en el arbol sin que nadie
lo hubiera aplicado a esta unidad.

## 1. Los 648 B que estaban a tres `#define`

El bloque duplicado de 780 B era, casi entero, `Attrib::Attribute`..`Attrib::Blob` mas
veinte `Attrib::Gen::*` mas sus cadaveres. **El arbol ya tiene el hook, y lo usan
zRender, zMisc, zSim, zMain y zMiscSmall**:

    #define ATTRIB_TAGS_HAND_POOL 1        // AttribSys.h: ATTRIB_TAG(s) -> NULL
    #define ATTRIB_TAG_ORDER_HAND_POOL 1   // apaga el primer _AttribAllocTagOrder
    #define HAND_POOL_TAG(s) ((const char *)0)
    #define DEAD_STR(s) ((const char *)0)
    #define ATTRIB_EMPTY_STRING _zl_empty

Cinco lineas en `zAnim.cpp` y la `.rodata` baja **672 B** con `fncmp` a 0 de 315. Son
guardas por unidad: el texto que ven las otras catorce unidades no cambia ni un token.

**Leccion**: antes de escribir un mecanismo nuevo, `grep` del hook. El mapa de la r58
describia el sintoma («cc1plus re-interna») y proponia el remedio caro (aliasar a mano);
el remedio barato llevaba tres rondas en `AttribSys.h`.

## 2. El alias tiene que ser un SIMBOLO PLANO (y esto cuesta 4 B si no)

Para las cadenas que el bloque verbatim ya trae y **nuestro codigo si usa**
(`EAGL4::DynamicLoader CAnimBank`, `Anim_CNFSAnimBank_SlotPool`, `MPerpBusted`, las
quince primeras `Arrest*`, ...) hace falta un alias. Las tres formas obvias **no valen**:

| forma | resultado |
|---|---|
| `extern char p[] asm("lbl_803CF388");` + `(p + 0x60)` | `InitAnimBankSlotPool` **96 B en vez de 100** |
| `struct` con el miembro en su offset | lo mismo, 96 B |
| `.set sym, lbl_803CF388 + 0x60` dentro del `asm()` | el ensamblador de SN: `Failed to evaluate` |

La causa: `-fforce-addr` materializa la direccion antes de la llamada **solo si el arbol
llega a `expand_expr` como un `SYMBOL_REF` pelado**. El `+ N` (y el miembro de struct) se
pliegan en un `CONST` y entonces el reparto la rematerializa DESPUES de la llamada, con lo
que se ahorra el preservado y sale una instruccion de menos.

Lo que si vale: **etiquetas `.globl` DENTRO del bloque verbatim, sin `.type` ni `.size`**.
El objeto que ve el enlazador sigue siendo `lbl_803CF388` entero (que es lo que protege
`keep.lst:78`), y `extern char X[] asm("zanim_lc_...")` da el simbolo plano. Comprobado:
con las 24 etiquetas dentro, el bloque **no se estripa** y `fncmp` vuelve a 0 de 315.

## 3. El bloque verbatim va PARTIDO EN DOS

Esto no estaba en el mapa de la r58 y es lo que bloqueaba la pieza grande: **el objetivo
emite los 20 B de pool de `GetClosestMarker` (AnimCandidates.cpp) EN MEDIO de su prefijo**,
en `803CF4B0`. Con un bloque unico de `803CF3A4..803CF8F4` esos 20 B se duplican siempre,
pase lo que pase con los alias.

    A  803CF388..803CF4B0   lbl_803CF388, .size 0x128   (arriba de zAnim.cpp)
       803CF4B0..803CF4C4   <- aqui cae solo el pool de GetClosestMarker
    B  803CF4C4..803CF8F4   ANONIMO, entre AnimCandidates.cpp y AnimChooseArrest.cpp

**Un `asm()` de fichero se emite donde esta escrito**, asi que colocarlo entre dos
`#include` de la SourceList lo mete entre los pools de los dos ficheros.

## 4. Los cadaveres se matan en la FUENTE, no en keep.lst

`-strip-unused-data` se lleva `size & ~7` y deja el resto: cada literal muerto que
emitimos deja de 1 a 7 B donde el objetivo no tiene nada. La r58 concluyo «un cadaver no
se arregla con un `asm()`: o se mantiene la cadena entera, o no se emite». Correcto —
y **la forma de no emitirla era arreglar la fuente**:

| cadaver | de donde salia | lo que era de verdad |
|---|---|---|
| `.cpp` / `cpp` (x2) | `BNEW` = `new (__FILE__, __LINE__)` | `new ("<clase>", 0)` en los **SIETE** sitios de `Animation/`. El objetivo tiene `CAnimScene`, `CAnimWorldScene`, `CAnimEntityData`, `CAnimSceneData`, `CAnimProperty`, `CAnimResourceFileProxy` y `GenericNISControlScenario` **justo donde nosotros dejabamos el cadaver del `__FILE__`** |
| `TODO` (5 B, entera) | `AnimInternal.cpp:6`, `new ("TODO")` | era `new (name)`: la funcion recibe `name` y lo tiraba. El objetivo no tiene `"TODO"` en toda la ventana |
| `ssis` `uction` `stems` `ry` `e` `p` | `Attrib::Gen::chassis`, `induction`, `EventSequencerSystems`, `EAGL4::SymbolEntry`, `ScrollerDatumNode`, `emittergroup` | se van solos con las guardas del §1 |
| `e` (4 B) | `"Unnamed MyEAGLNewOverride"`, muerta | bloque anonimo de 28 B con etiqueta, y el fuente usa el alias. **La unidad ya no depende de ningun `$LC` de keep.lst para esa cadena** |

Con `BNEW` corregido aparecen ademas **dos funciones que el arbol tiene STRIPPED y cuyo
pool si esta en el objetivo**: `CAnimCtrl::GetFlagString` (`"%s%s,"`, los ocho `AC_*` y un
`")"`) y `CWorldAnimCtrl::GetFlagString` (los ocho `WAC_*`). Sus 232 y 224 B van como
bloque anonimo entre las dos funciones que los rodean.

**Trece bloques anonimos en total** (`.section .rodata` + los bytes del DOL objetivo +
`.previous`), cuatro en `zAnim.cpp` y nueve dentro de `Animation/*.cpp`. Ninguno lleva
simbolo de objeto: un hueco sin simbolo es inestripable y **no toca `keep.lst`**.

## 5. La UNICA peticion: correr `lcfix.py`

Quitar y reordenar literales **renumera los `$LC`**, y cinco entradas de `keep.lst` se
quedan rancias. Medido las dos veces, con el mismo `.o`:

    keep.lst de HEAD              rodata -96   ->  trypromo DOL ROTO (eaca9ad49cd3)
    keep.lst con `lcfix.py`       IGUAL        ->  trypromo DOL OK

Los 96 B son exactamente `size & ~7` de las cinco cadenas que esas entradas mantenian
vivas (`CBasicCharacterAnimEntity` 24 + `CPropAnimEntity` 16 + `WorldAnimEntityInfo*[]` 16
+ `EAGL4::SQT buffer CAnimPart` 24 + `CWorldAnimEntity*` 16).

> **PETICION** (regla 3, no lo he tocado): `python scripts/lcfix.py`. Corrige 242 entradas
> en nueve unidades; **cinco son de zAnim y son las que hacen falta**. `lcfix` **NO toca**
> las entradas que marca `FALLO`, asi que no puede romper nada por ese lado.
>
> **Ninguna correccion de `lcfix` es venenosa para zAnim**: al reves, sin ellas la unidad
> no promociona. (Esto INVIERTE el aviso de la r58, que pedia NO aplicar el lcfix de
> zAnim: aquel `$LC437` de `"Unnamed MyEAGLNewOverride"` ya no existe — la cadena la pone
> ahora un bloque anonimo, ver §4.)
>
> **Limpieza opcional y medida**: las **22** entradas `@lc zAnim` que `lcfix --check` marca
> `FALLO` son basura (sus cadenas ya no se emiten, estan dentro del bloque verbatim).
> Borrarlas da EXACTAMENTE el mismo resultado — `DOL OK` — que dejarlas.

## 6. Regresiones: CERO, medidas objeto a objeto

Dos cabeceras que no son solo de zAnim, las dos **bajo `#ifndef`** (las demas unidades ven
los mismos tokens):

| cabecera | por que | unidades ajenas que la ven |
|---|---|---|
| `Generated/Messages/MPerpBusted.h` | `static UCrc32 k("MPerpBusted")` -> `MPERPBUSTED_KIND_STR` | zAI, zEAXSound, zEAXSound2, zSim, zSpeech |
| `Frontend/.../customize/CarCustomize.hpp` | `CustomizeMainOption("", ...)` -> `CARCUSTOMIZE_EMPTY_STRING`; era el ULTIMO `""` de la unidad, 4 B en 803CF8F4 | zFe, zFe2, zFeOverlay, zMain |

Metodo de la regla 5: compilar cada unidad ajena a un `.o` **PRIVADO** (nunca
`build/GOWE69/src`) con mis cabeceras y con las de HEAD, y comparar secciones asignables y
el VALOR de cada simbolo.

    zAI zEAXSound zEAXSound2 zFe zFe2 zFeOverlay zMain zSim zSpeech
    -> 9 de 9 IGUAL: 0 secciones distintas, 0 simbolos con otro valor.

## 7. Sorpresas

1. **`agent_anim_gaps.py` agrupa por difflib y sus cifras engañan.** «FALTA 276 / SOBRA 32»
   eran dos huecos reales de 232 y 12 B **con nuestro pool en medio**. Lo mismo «FALTA 340
   / SOBRA 76» = 32 + 4 + 228. Hay que bajar a comparar el DOL **palabra a palabra** para
   partir el hueco por donde toca; los ultimos 4 B (tres palabras de codigo) solo se ven
   asi.
2. **Un `keep.lst` rancio hace mentir a `linkdelta` en las DOS direcciones.** Con las
   entradas rancias medi `rodata -808` donde la verdad era `-704`: los `$LC` desplazados
   dejaban de mantener vivas cadenas que si contaban. Toda medida intermedia de esta ronda
   se hizo con una copia PRIVADA de `keep.lst` regenerada **despues de cada compilacion**.
3. **`ATTRIB_TAG_ORDER_HAND_POOL` y `ATTRIB_TAGS_HAND_POOL` son macros distintas a
   proposito** y hay un comentario en `AttribSys.h` explicando por que (zSim necesita una y
   no la otra). zAnim necesita las dos.
4. **El pool de una funcion sale DELANTE de su cuerpo**, no detras: un `asm()` escrito
   justo encima de la definicion de `f` cae entre el pool de la funcion anterior y el de
   `f`. Es lo que hace colocables los trece bloques.
5. **`.set sym, otro_simbolo + N` no existe en el ensamblador de SN** (`Failed to
   evaluate`). La unica forma de dar un nombre a un punto interior de un bloque es una
   etiqueta de verdad.

## 8. Herramientas

Ninguna sonda nueva en `scripts/` (todas en el scratchpad). De las que dejo la r58, las
dos que merecen quedarse son **`agent_anim_gaps.py`** (alinea la ventana por literal) y
**`agent_anim_dw.py`** (`dolwhere` con un `keep.lst` alternativo, imprescindible para medir
sin tocar el fichero compartido). Las de esta ronda que valdria la pena promover:

| | |
|---|---|
| `blk.py` | inserta un `asm()` ANONIMO con los bytes del DOL objetivo de un rango, delante de un ancla del fuente, con etiquetas `.globl` en los offsets que se le pidan |
| `peek.py` | compara el DOL objetivo con el nuestro **palabra a palabra** en una direccion, enlazando con un `keep.lst` alternativo |
| `mkkeep.py` | genera la copia privada de `keep.lst` al dia (renumera lo que `lcfix` marca `CORRIGE`, borra lo que marca `FALLO`) |
