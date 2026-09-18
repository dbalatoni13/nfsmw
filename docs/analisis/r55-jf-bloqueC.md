# El bloque C, y un frente de vtables que estuve a punto de inventarme

Segunda mitad de la ventana de la r55. Medido sobre las tres unidades del bloque C que **no
posee ningún agente**: `zWorld`, `zCamera`, `zEagl4Anim` — 399.000 B entre las tres.

---

## 1. El triaje

| unidad | contenido real | `linkdelta` |
|---|---|---|
| `zWorld` | 25 palabras = sus 4 funciones abiertas | `.text +0`, `rodata−472`, `data−128` |
| `zCamera` | 30 palabras = sus 2 `Update` | `.text +16`, `rodata−1384`, `data−224`, `bss+32` |
| `zEagl4Anim` | 18 palabras = `EvalState` + `Initialize` | `.text +4`, `rodata−648`, `data−96`, `bss−128` |

A diferencia del bloque B, aquí **las funciones sí son la mayor parte del contenido**. Pero
las tres arrastran además un `rodata` corto de entre 472 y 1.384 B que nadie contaba, y
`zCamera` es el peor con **−1.384**.

---

## 2. El frente que NO existe, y casi lo publico

`reorden.py` señalaba en las tres unidades un racimo de **vtables con palabras distintas**:
cinco `_vt.*CDAction*` en `zCamera` (una palabra cada una), seis `_vt.Q29EAGL4Anim*` en
`zEagl4Anim`, una en `zWorld`. Once vtables «con contenido» que ninguna herramienta contaba
como trabajo. Escribí que `zEagl4Anim` **estaba bloqueada por seis vtables**.

**Es falso, y lo son las once.** `vtable_audit.py` compara ranura a ranura contra el `.s`
original —que es verdad absoluta— y da **cero ranuras distintas** tanto en `CDActionDrive`
como en `FnCycle`: los símbolos coinciden uno a uno.

La causa es el falso positivo que ya había nombrado el agente de `zAI` en el triaje de esta
misma ronda: **la máscara de `reorden` conserva la mitad alta de un `ADDR32`**, así que un
puntero a algo que se ha movido más de 64 kB delata su desplazamiento como si fuera contenido.

No es «nada» —una vtable que apunta 64 kB más allá es un problema de POSICIÓN real— pero **no
es contenido que escribir**, que es lo que yo iba a decir.

La regla, que ya estaba en el catálogo: **cuando una medida por contenido señale una vtable,
confírmalo con `vtable_audit`, que compara por SÍMBOLO.** Y `vtable_audit` guarda caché: sin
`--refresh` contesta `nuestro=AUSENTE` y parece que no emitimos la vtable.

---

## 3. Lo que sí sobrevive: emitíamos una vtable que el original no tiene

Esto viene de `vtord`, que es otra medida, y por eso no cae con la corrección de arriba:

    zEagl4Anim: objetivo 31 vtables, nuestro 32
    SOLO NUESTRAS (1): Q29EAGL4Anim6FnAnim

Y **ningún objeto original define `_vt.Q29EAGL4Anim6FnAnim`** — comprobado barriendo los 616.

La causa es la *key method*. `FnAnim` (`FnAnim.h:38`) **no tiene ninguna virtual pura** y todas
sus virtuales llevan cuerpo en clase **salvo una**: `GetAttributes()`, declarada en la línea
105 y definida fuera de línea en `FnAnim.cpp:10`. Esa definición fuera de línea la convierte
en la key method, y GCC 2.9 emite ahí la vtable de la clase.

Es el recíproco exacto de [[nfsmw-la-virtual-que-era-pura]], donde tres unidades no enlazaban
porque **no** emitíamos vtables que hacían falta.

**El arreglo, medido**: mover el cuerpo de `GetAttributes` dentro de la clase (son dos líneas
y un `return nullptr;`).

| | antes | después |
|---|---|---|
| `vtord` | objetivo 31, nuestro **32** | **31 y 31** |
| `fncmp` | 2 de 318 | **2 de 318** — cero regresión |
| `linkdelta` | `.text +4 rodata−648 data−96 bss−128` | **idéntico** |
| vtables descolocadas | ~30 de 31 | **26** |

Y la comprobación de que no se ha perdido nada: `GetAttributes__CQ29EAGL4Anim6FnAnim` sigue en
`.text` con **8 B, idéntico al original**, porque las vtables la referencian y GCC la emite
igual estando en clase.

**Honestamente: cero bytes medibles hoy.** La vtable sobrante la estripaba `-strip-unused-data`
y por eso `linkdelta` no se mueve. Lo que gana es fidelidad —dejamos de emitir un símbolo que
el original no tiene— y cuatro filas de vtable colocadas. Se queda porque no cuesta nada y
acerca la unidad; no porque haya pagado.

---

## 4. Lo que bloquea de verdad a `zEagl4Anim`, y es transcripción

Quedan **26 de 31 vtables en otro orden**. Y el orden de las vtables no es libre: sale de
`walk_globals(vtable_decl_p, …)` en `finish_file`, **ordenado por cuándo se completó la
CLASE** — o sea, por el orden de parseo de las definiciones de clase.

Es el mecanismo de la r54 aplicado a vtables, y por tanto **se arregla reordenando los
`#include` de `zEagl4Anim.cpp`**, no tocando codegen:

    objetivo: FnRawStateChan, FnCycle, FnPoseMirror, FnGraft, FnRawLinearChannel,
              FnTurnBlender, FnRunBlender, FnPhaseChan, FnRawEventChannel, …
    nuestro:  FnRawStateChan, FnRawLinearChannel, FnRawEventChannel, FnPoseMirror,
              FnGraft, FnCsisEventChannel, FnTurnBlender, FnRunBlender, FnRawPoseChannel, …

El orden del objetivo está en el ELF y se lee. **Es transcribir, no averiguar.**

---

## 5. Para el reparto de la r56

- **`zEagl4Anim`** (113.016 B): reordenar los `#include` por el orden de vtable del objetivo,
  más sus dos funciones. Confianza alta, y el paso está escrito.
- **`zCamera`** (125.008 B): sus dos `Update` son 30 de sus 35 palabras, pero arrastra
  **`rodata−1.384`**, el mayor déficit de datos del bloque C. Las cinco vtables **no** son
  trabajo.
- **`zWorld`** (160.956 B): cuatro funciones, y dos de ellas —`RenderFlaresOnCar` y
  `UpdateWheelYRenderOffset`— quedaron **vetadas con prueba estructural** en la r54. Las otras
  dos, `SetMemoryPoolSize` (2 palabras) y `DefragmentPool` (23), no las ha mirado nadie.
