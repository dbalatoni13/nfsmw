# r55 · `grid` — la cabecera de la ronda

**Una edición, un fichero, siete líneas: `src/Speed/Indep/Src/World/WCollisionAssets.h`.**
El robo de `_STL::_List_base<WGridManagedDynamicElem,…>::clear` **está cerrado** en las tres
unidades. **−1.447.438 B de DOL distinto** medidos con el enlace, cero regresiones en las cinco
unidades, y el enlace base sigue dando **DOL OK**.

Ninguna unidad promociona: lo que les queda **no es de dueño**, y eso también está medido.

---

## 1. La edición

```diff
--- a/src/Speed/Indep/Src/World/WCollisionAssets.h
+++ b/src/Speed/Indep/Src/World/WCollisionAssets.h
-#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
+// Esta cabecera NO usa WGridManagedDynamicElem: solo necesita WGridNodeElemTag.
+// Incluir WGridManagedDynamicElem.h aqui instanciaba _List_base<...>::clear en
+// toda TU que la incluyese (zGameplay, zMisc, zPhysics), robandoselo a zSim.
+#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
+#include "Speed/Indep/Src/World/Common/WGridNode.h"
 #include "Speed/Indep/bWare/Inc/bChunk.hpp"
-#include "WGridManagedDynamicElem.h"
 #include "WTrigger.h"
```

Es la **ruta (a)** del encargo. `WGridManagedDynamicElem.h` **no se ha tocado**: sigue con
`Init`/`Shutdown` en clase, tal cual estaba.

Las dos cabeceras que se añaden son las que `WGridManagedDynamicElem.h` metía **en ese mismo
punto** (sus líneas 4 y 5), así que el orden de inclusión que ve cada TU no cambia. Y las dos
hacen falta de verdad: `WGridNodeElemTag` (usado en las líneas 37‑70 de la cabecera) sale de
`WGridNode.h`, y `CARP::Trigger` (líneas 62‑63) de `CARP.h`.

**No es atómico con nada.** Es una edición de fuente sola: no pide tocar `configure.py`,
`splits.txt` ni `keep.lst`. Lo «atómico» del encargo era entre las tres unidades, y con la
cabecera arreglada ya no hay traspaso posible entre ellas: **ninguna de las tres define el
símbolo**, así que se pueden promocionar juntas o por separado indistintamente (medido, §4).

---

## 2. El control por ablación, y **corrige el brief**

TU‑sonda = una cabecera + `int dummy;`, con las cflags exactas de zSim. Tres variantes:

| variante | forma de `Init`/`Shutdown` | `.text` de la sonda | ¿emite el COMDAT de 120 B? |
|---|---|---:|---|
| **V0** | cuerpo **EN CLASE** (lo que hay hoy) | 388 | **SÍ** |
| **V1** | declarados en clase, cuerpos `inline` **detrás de la clase, en la misma cabecera** | 388 | **SÍ** |
| **V2** | sólo declarados; el cuerpo **fuera de la cabecera** | 268 | **NO** |

**V1 es el resultado nuevo y refuta la ruta (b) tal y como estaba escrita.** El encargo decía
«sacar `Init`/`Shutdown` fuera de la clase en `WGridManagedDynamicElem.h`»: eso **no quita
nada**. Lo que dispara `instantiate_decl` no es *en clase* contra *fuera de clase*, es que **el
uso de `clear()` esté escrito en la cabecera**, dentro de un cuerpo de función cualquiera. Sacar
el cuerpo de la clase pero dejarlo en el `.h` sigue emitiendo los 120 B, al byte.

Para que la ruta (b) funcionase habría que llevar los cuerpos a
`World/Common/WGridManagedDynamicElem.cpp`, y **eso es una regresión**: el original **no tiene
`Init__23WGridManagedDynamicElem` ni `Shutdown__23WGridManagedDynamicElem` en ningún objeto**
(comprobado sobre los 296 `.o` extraídos), así que aparecerían dos símbolos que no existen y
cambiarían las llamadas de `WCollisionAssets::Init`/`Shutdown`, que **hoy casan al byte**.

**Ruta (b): descartada con medida. Ruta (a): aplicada.**

---

## 3. El antes y el después, con el enlace de juez

Medida: se enlaza el DOL completo con **una** unidad promocionada y se cuentan los bytes que
difieren del `main.dol` original, más la dirección a la que resuelve el símbolo. Es la misma
receta de `trypromo.py` con dos cifras extra.

| unidad | `clear__…WGrid…` ANTES | DESPUÉS | bytes de DOL distintos ANTES | DESPUÉS | ganancia |
|---|---|---|---:|---:|---:|
| **zGameplay** | `801B6504` (robado) | **`80286528`** | 1.151.090 | 435.780 | **−715.310** |
| **zMisc** | `8020C488` (robado) | **`80286528`** | 774.184 | 345.081 | **−429.103** |
| **zPhysics** | `80227698` (robado) | **`80286528`** | 664.796 | 361.771 | **−303.025** |
| **zWorld2** | `80286528` (ya correcto) | `80286528` | 318.189 | 318.189 | 0 |
| | | | | | **−1.447.438** |

`80286528` es la dirección del enlace base, o sea **la de zSim**, que es el dueño del original.

**zWorld2 es el control negativo que exige la regla 9**: va detrás de zSim en el orden de
enlace (31 contra 27), nunca le robó nada, y su cifra **no se mueve ni un byte**. Si se hubiera
movido, la medida estaría midiendo otra cosa.

La cifra del censo (`847.152 / 493.816 / 382.240 B`) y ésta miden el mismo fenómeno con reglas
distintas: aquélla es la **unión de rangos desplazados**, ésta son **bytes que difieren de
verdad**. La mía es más pequeña y más dura.

### Quién define el símbolo

```
ANTES   zGameplay  zMisc  zPhysics  zSim  zWorld2      (CINCO, y el original lo da UNA vez)
DESPUES                             zSim  zWorld2      (y zSim gana por ir antes)
```

---

## 4. El dueño está cerrado: lo que queda **no** es robo

Con la cabecera arreglada quedan 25 / 24 / 11 símbolos que cambian de ganador al promocionar,
y **todos cuestan cero**:

* 24 / 23 / 10 son la familia `Attrib::TAttrib<T>::Get` (80 B cada uno) — la familia (F) del
  censo;
* 1 es `_M_erase__…_Rb_tree<…Hermes::_h_HHANDLER__…>` (104 B).

**Comprobado, no supuesto**: ni `Get__CQ26Attribt7TAttrib1ZiUi` ni
`_M_erase__Q24_STLt8_Rb_tree5ZPQ26Hermes13_h_HHANDLER__Z` **aparecen en la tabla de símbolos
del ELF enlazado**. `-strip-unused-data` se los lleva enteros, así que da igual quién los gane.

**Ni un solo robo vivo queda en zGameplay, zMisc y zPhysics.**

El segundo `clear__` de la familia, el de `WGridNodeElem`, lo siguen definiendo seis objetos
nuestros — pero uno es **zAI, el objeto 0**, que no puede perder. Coste cero, como decía el
censo. Mi edición no lo toca a propósito: `WGridNode.h` **sigue incluido** porque la cabecera
necesita `WGridNodeElemTag`.

---

## 5. Veredictos y red de seguridad

`fncmp` de las **cinco** unidades, antes y después. **Idéntico**, cero regresiones:

| unidad | antes | después |
|---|---|---|
| zGameplay | 0 de 768 · 0 B | **0 de 768 · 0 B** |
| zMisc | 0 de 450 · 0 B | **0 de 450 · 0 B** |
| zPhysics | 0 de 718 · 0 B | **0 de 718 · 0 B** |
| zSim | 0 de 402 · 0 B | **0 de 402 · 0 B** |
| zWorld2 | 2 de 357 · 3.796 B | **2 de 357 · 3.796 B** |

`trypromo` (ruta completa) y `linkdelta`:

| unidad | `trypromo` | `linkdelta` |
|---|---|---|
| zGameplay | DOL ROTO (`89ea30d64004`) | `.text +0`, `rodata −136`, `data −128` |
| zMisc | DOL ROTO (`f974dc07f931`) | `.text +0`, `rodata +2840`, `data +32` |
| zPhysics | DOL ROTO (`b59610753077`) | `.text +0`, `rodata +8`, `bss +32` |
| zWorld2 | DOL ROTO (`6494061671c3`) | `.text +0`, `rodata −408`, `data −256`, `bss +64` |
| **zSim** | ya promocionada | — |

**zSim no se ha roto.** El enlace base (con `src/…/zSim.o` dentro, recompilado con la cabecera
nueva) da `sha1 = 9619ba57c9919f95f7f2ac951a2166a3517f91e3`, **DOL OK, 0 bytes distintos**.

`dolwhere` no da nada útil en ninguna de las cuatro: aborta con «LAS SECCIONES NO COINCIDEN»
porque `.rodata`/`.data` todavía miden distinto. La cifra de bytes del §3 lo sustituye.

---

## 6. Sorpresa: un `build_direct.py` que dice `ok` y deja un `.o` malo

A mitad de la ronda el enlace base pasó de **DOL OK** a **ROTO por 66 B en 7 rangos**, todos en
literales de `.rodata` de zSim (`$LC554`, `$LC555`, `$LC557`, `$LC630`, `$LC631`, en
`0x8040490C` y `0x80404B48`): nos sobraba un `"MNISComp"` y un `"MNotifyS"` y nos faltaban
`"ISimable"` y `"TimeStep"`.

**No era la edición.** Recompilando **la misma fuente sin cambiar nada** el enlace volvió a
DOL OK. Compilando después las cinco unidades **en serie** y sellando el `.o` con sha1,
`zGameplay` y `zMisc` **también cambiaron de sello sin que la fuente cambiara**.

O sea: la tanda paralela de `build_direct.py` (6 hilos, y siete agentes compilando a la vez)
no sólo falla a veces sin mensaje — **a veces dice `ok` y escribe un objeto que no
corresponde a la fuente**. Una medida tomada sobre él es basura silenciosa. Con siete agentes
en paralelo esto puede haber envenenado cifras de otras rondas.

**Cómo protegerse, y cuesta segundos**: compilar las unidades de una en una y comprobar el
sha1 del `.o` antes y después; si no cambia cuando la fuente sí cambió, o cambia cuando no
debía, repetir. Las cifras de este informe están todas tomadas después de una tanda en serie
verificada, y los sha1 del §5 **se reprodujeron dos veces**.

---

## 7. Qué haría falta para cerrar estas unidades

El dueño ya no es el problema. Lo que queda es **dónde se emite dentro de la unidad**:

* **zPhysics** es la más cerca en tamaño (`+32 B` de DOL) pero le quedan **541 símbolos
  realmente permutados** (`movidos.py`: 391 en `.text` = 58.324 B, 143 en `.rodata` = 10.546 B)
  y seis escalones sin explicar. Es orden de emisión, no código.
* **zMisc** tiene `rodata +2.840`: emite de más en datos, no le falta código.
* **zGameplay** tiene `rodata −136` y `data −128`: le **falta** dato.
* **zWorld2** sigue con sus dos funciones (`HolePunchAvoidables`, `InitAtSegment`, 3.796 B) y
  `rodata −408 / data −256`.

Y un hallazgo colateral que **no cuesta un byte hoy** pero explica el mecanismo: el
**zWorld2 original REFERENCIA `clear__…WGrid…` sin definirlo** (`shndx = 0`), mientras que el
nuestro sí lo define. Es la prueba de que en el original la vía deferida (`saved_inlines` →
`finish_file`) dejaba el símbolo externo y sólo la vía anidada de `instantiate_decl` lo emitía.
Como zWorld2 (31) va detrás de zSim (27), su copia WEAK se descarta y el coste es **cero**
—verificado: con zWorld2 promocionada, `clear__` sigue en `80286528`—. Quitarlo exigiría los
dos símbolos fuera de línea que el original no tiene, así que **no se toca**.

---

## 8. Propuestas que no aplico (no las poseo)

Ninguna. La edición entera cae dentro de `WCollisionAssets.h`, que es mía, y no necesita
cambios en `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.
