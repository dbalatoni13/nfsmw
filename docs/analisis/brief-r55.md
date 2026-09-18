# Brief de la ronda 55 — la ronda de las cabeceras

Sustituye a `brief-r54.md`. Léelo entero antes de tocar nada.

**Esta ronda sí es de bytes.** La r54 nombró el mecanismo y el triaje de la r55 —doce agentes,
uno por unidad— ha localizado **dónde pesa**. Cada encargo lleva la edición concreta, con
fichero y línea, y la evidencia de por qué va ahí.

## Estado

    python scripts/estado.py        # el frente
    python scripts/serie.py         # el ritmo, en ENCARGOS

    HECHO    908.452 / 3.946.048 B   23,02 %   <- codigo que sale de NUESTRA fuente
    CODIGO   3.912.360 / 3.946.048 B  99,1463 %   faltan 28 funciones reales
    ENLACE   518 / 616, techo REAL 544

DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Lo que dice el triaje, y es un titular

**`reorden.py` da `.text = 0` en las DOCE unidades del bloque A.** No falta ni una palabra de
código en 1,65 MB. Todo lo que las separa del DOL correcto es **dónde se emite**, y lo poco
que queda de contenido son `.rodata` pequeños (zFe 113 palabras, zPhysics 44, zEAXSound 25).

Y los dos mecanismos de la r54 resultan ser **el mismo**: `instantiate_decl`
(`cp/pt.c:9439-9461`) instancia una plantilla *inline* en el punto del parseo si su primer uso
está **dentro de una función**. Un método definido **en clase** que llama a un método de
plantilla cumple esa condición **en toda TU que incluya la cabecera** — y por eso cinco
objetos nuestros definen un símbolo que el original define una sola vez. El «cambio de dueño»
no es un mecanismo aparte: es la consecuencia.

## LA EDICIÓN DE LA RONDA

`src/Speed/Indep/Src/World/WGridManagedDynamicElem.h:19-25`

```cpp
static void Init()     { fgManagedDynamicElemList.clear(); }   // cuerpo EN CLASE
static void Shutdown() { fgManagedDynamicElemList.clear(); }   // -> instancia clear() aqui
```

**Cuatro agentes independientes llegaron a esta cabecera**, tres midiendo unidades distintas y
el cuarto por el censo global. Probado por ablación con las cflags exactas: una TU-sonda que
sólo incluye la cabecera **ya emite** el COMDAT de 120 B, y con los cuerpos fuera de clase
**desaparece**.

Y `WCollisionAssets.h` la incluye **dos veces** (líneas 7 y 9, una absoluta y otra relativa)
**sin usar el tipo en ningún sitio**.

| unidad | lo que vale |
|---|---:|
| zGameplay | 847.152 B |
| zMisc | 493.816 B |
| zPhysics | 382.240 B |
| **total, con UNA edición** | **1.723.208 B** |

**Es atómico**: arreglar sólo zGameplay le entrega el símbolo a zMisc, y arreglar zMisc se lo
entrega a zPhysics. Sólo la cabecera arregla los tres de golpe.

## Las demás causas de dueño, por rentabilidad

| qué | dónde | vale | unidades |
|---|---|---:|---|
| `Init`/`Shutdown` en clase | `WGridManagedDynamicElem.h:19-25` | 1.723.208 | zGameplay, zMisc, zPhysics |
| dos globales sin `extern` | `EcstasyE.cpp:168,:241` + `EmitterSystem.cpp:803` | 328.484 | zEcstasy |
| catorce globales **definidas** en cabecera | `CarCustomize.hpp:686-698` | 264.488 | zAnim, zFe2 |
| duplicado + instanciación de `Factory` | `EaxSoundTypes.cpp:26` | 246.452 | zEAXSound |
| heredar de `UTL::Std::list` | `SpeechManager.hpp:142` | 152.496 | zSpeech |

**`SpeechManager.hpp` es urgente por una razón distinta del tamaño**: en el mundo
todo-promocionado el dueño pasa a ser **zAI, que es el objeto 0** del orden de enlace, y
entonces ya no se le puede quitar moviendo nada. Hay cinco casos así, latentes, que hoy tapa
un `obj/*.o` sin promocionar.

## PROPIEDAD: quién toca qué

Siete agentes en paralelo sobre el mismo árbol. **Nadie toca el fichero ni la unidad de otro**
— ni para medir, porque reconstruir el `.o` de otro le corrompe la medida (le pasó a
`forense2` en la r54: su cifra saltó de 15.369 a 110.064 B).

| agente | cabeceras que POSEE | unidades que POSEE |
|---|---|---|
| `grid` | `WGridManagedDynamicElem.h`, `WCollisionAssets.h` | zGameplay, zMisc, zPhysics, zSim, zWorld2 |
| `frontend` | `CarCustomize.hpp`, `uiQR*.hpp` | zAnim, zFe2, zFeOverlay |
| `speech` | `SpeechManager.hpp` | zSpeech |
| `eax` | `EAXCarState.hpp` | zEAXSound, zEAXSound2 |
| `ecs` | — | zEcstasy |
| `zai` | — | zAI |
| `orden` | — | zMain, zLua, zGameModes, zFe |

Si tu edición necesita una cabecera que no posees, **proponla y no la apliques**.

## Las trampas, todas con su caso medido

- **`movidos.py` NO mide cambio de dueño.** Lo etiqueté mal en la r54. `enlazado − delta`
  mezcla robo, **código que el enlazador tira** (`-strip-unused-data` va por SÍMBOLO, así que
  emitir de más es lo normal) y relleno. Refutado con `zTrack`: cero robos y sigue imprimiendo
  `.text −3.536`. El censo bueno está en `r55-censo-dueno.md`.
- **`reorden.py` puede dar un falso CONTENIDO**: la máscara conserva la mitad alta de un
  `ADDR32`, así que un símbolo que se mueve más de 64 kB delata su desplazamiento como si fuera
  contenido. Y al revés, **`CONTENIDO 0` esconde déficit**, porque sólo compara lo que empareja
  por nombre: zAI tiene 388 B de `.data` que no emitimos y le salía 0.
- **`GLOBAL` gana a `WEAK` aunque vaya DESPUÉS** en el orden de enlace. Y `dtk` marca `GLOBAL`
  lo que el compilador emitía `WEAK`.
- **`SHN_COMMON` no define**: tratar `shndx != 0` como definición inventaba una familia entera
  de 20 símbolos y 4,6 MB. Refutado enlazando.
- **`dolwhere` cuenta RANGOS, no bytes**: 183.693 donde había 15.369.
- **El DOL rellena cada sección a 32 B**: un −24 puede estar ya bien y un +8 no.
- **La familia de plantillas más numerosa no existe en el original**: los 25
  `Attrib::TAttrib<T>::Get` no los define ningún objeto original y el enlazador los tira
  enteros. Cierto en número, falso en peso.

## Palancas del catálogo

- **En clase = inline en GCC 2.9** → `mark_inline_for_output` (`cp/decl2.c:2140`) → el final del
  array `saved_inlines` → `wrapup_global_declarations` (`cp/decl2.c:3795`) lo vuelca al final de
  la unidad. **Fuera de clase = en el punto del parseo.** Es la palanca que mueve un cuerpo
  entre el sitio y la cola, en las dos direcciones.
- **El primer de instanciación**: una `static` MUERTA que usa el símbolo dentro de una función,
  puesta donde el objetivo lo emite. Cuesta `.text +0` porque el enlazador la estripa. Marcarla
  como andamio.
- **Un `asm()` de ámbito de fichero** GCC lo emite donde está escrito, así que no se puede
  diferir a la cola: si el objetivo lo tiene al final, hay que **moverlo** al final del `.cpp`.
- **El primer de pool**: una `static inline` muerta con `switch` de `return "literal"` mueve
  cadenas hacia ATRÁS por cero bytes. No vale para floats.

## Reglas duras

1. **Nunca `ninja` completo ni `configure.py`.** Usa `build_direct.py`, y **sólo con tus
   unidades**.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.** Proponlo con la
   medida y **di si el paquete es atómico**.
4. **Cabecera que no posees: se propone, no se aplica.** Y déjala compilable en todo momento.
5. **Cero bytes con `asm` puesto es deuda.**
6. Borra tus volcados. **No borres con comodines en el scratchpad**: es compartido.
7. **Ninguna sonda de un solo uso en `scripts/`.**
8. **Exígele a tu medida un control que TENGA que cambiar.** En la r54 eso valió tres
   correcciones de herramienta; en el triaje de la r55, dos más — incluida la mía.
9. **Antes de dar por buena una edición: `fncmp` de todas tus unidades, antes y después.** Una
   regresión en una vecina no la ve nadie hasta la ronda siguiente.

## El reparto

| agente | encargo | vale |
|---|---|---:|
| `grid` | la cabecera de la ronda, paquete atómico de tres unidades | **1.723.208 B** |
| `frontend` | `CarCustomize.hpp` a `extern` + los tres destructores de zFeOverlay + los cinco `asm()` de zFe2 | 495.776 B |
| `ecs` | `EcstasyE.cpp` y `EmitterSystem.cpp` a `extern` | 328.484 B |
| `eax` | `EaxSoundTypes.cpp` + el cuerpo de `EAX_CarState` a la clase | 326.040 B |
| `speech` | `SpeechManager.hpp`, y **urgente por el orden de enlace** | 152.496 B |
| `zai` | las dos ediciones de zAI, ninguna en cabecera compartida | 83.340 B |
| `orden` | zMain, zLua, zGameModes y zFe: orden puro, con el paso escrito | cierran unidades |

**La meta de la ronda es `linked`, no bytes de diff.** Un agente que baje 800 kB de
desplazamiento y no promocione ha hecho su trabajo; uno que promocione una unidad ha hecho el
de dos.
