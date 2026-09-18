# Ronda 24 — el crédito falso

Tres encargos: barrer el **addend** en el árbol entero, quitar el **ensamblador
de `sndvd`**, y arreglar el **falso FALLA de `audit.py`**. Los tres cerrados, y
con dos correcciones de fuente que salieron del barrido.

Base medida por mí al empezar (`build_direct.py --all`: **471 ok, 52 fallidas**,
`report.json` de las 19:33 heredado de la r23).

---

## 1. El barrido del addend: la herramienta no veía NADA

### 1.1 `c23lk_reloc.py` no puede barrer el árbol — y no lo barrió nunca

Lo primero que hice fue lanzarla sobre las 590 unidades. Dos hallazgos, y el
segundo es el gordo:

1. **Revienta en la primera unidad**: `FileNotFoundError` con
   `auto_09_804FF8C0_sbss.o`, que es uno de los 64 comodines sin objeto
   nuestro. Sin capturar la excepción, el barrido no da **ni un dato**.
2. **Es O(n²) en el número de símbolos.** `d[base+off:].split(b"\0")[0]` **copia
   todo el resto del fichero por cada símbolo**. Con un `.o` de SourceList (5,9
   MB el de `zFe`, y el de `zAI` mayor) y sus cientos de miles de símbolos, no
   termina.

   Y esto no es teoría: al mirar los procesos vivos encontré **el proceso de la
   propia ronda 23 todavía corriendo**, lanzado a las 18:27 con
   `@c23lk_cand.txt`, con **6.169 s de CPU acumulados** y sin haber escrito una
   línea. Junto a él, un `c22link_index.py` de las 18:11 con 7.081 s. Los dejé
   vivos (no son míos), pero conviene matarlos: están comiendo dos núcleos.

   Con `bytes.index` en vez del corte, `zAI` pasa de *no terminar* a **0,86 s**.

Es el octavo caso del patrón «la herramienta falla EN SILENCIO»: no daba un dato
malo, daba **cero datos**, y el cero se lee como «limpio».

### 1.2 Y comparar por (sección, desplazamiento) no vale fuera del middleware

Arreglado lo anterior, `zAI` cantó **3.776 divergencias «graves»**. Ninguna lo
era: `c23lk_reloc.py` empareja las reubicaciones por su **desplazamiento dentro
de la sección de la unidad**, y en una SourceList nuestro `.text` mide 18 kB más
que el extraído, así que **todos** los desplazamientos están corridos. Por eso
la r23 sólo la pudo usar sobre las 95 candidatas de fuera de SourceLists.

**La llave tiene que ser el símbolo, no el desplazamiento.** `c24fx_fnreloc.py`:

1. empareja los símbolos (funciones y datos **con tamaño**) que están en los dos
   objetos con el mismo tamaño;
2. comprueba que el cuerpo es idéntico **después de tapar los campos que rellena
   cada reubicación** — eso es exactamente «objdiff da 100 %»;
3. y sólo entonces compara los **destinos resueltos**.

Una divergencia así es, por definición, crédito falso: el mismo código byte a
byte apuntando a otro sitio.

### 1.3 La autoprueba, que era obligatoria y sirvió

Reproduje el caso conocido: saqué de `git show cdcc8b78` la forma **anterior**
de `sfxrevc.c` (`&tablecopyto[vso][0][0][0]` con `+ outputchannels*36`), la
compilé **al scratchpad** con los cflags exactos de su unidad
(`c24fx_build1.py`, que no toca `build/GOWE69/src`) y comparé:

| | resultado |
|---|---|
| objeto con el fallo histórico | **2 DESTINO**: `.text` de `SNDMIXI_modlapifxadd` +486 y +494, `80450C6C` (`SNDDRV_dolbypl2balances+1812`) contra `80450D68` (`tablecopyto+0`) |
| objeto actual (arreglado) | **0** |

Dos reubicaciones, que son los **dos bytes** que la r23 midió en el DOL. La
herramienta ve el caso.

**Y la autoprueba cazó tres fallos míos antes de que contaminaran el barrido:**

- el addend se leía **sin signo** (`>III`): `tablecopyto-252` salía como
  `+4294967044` y el control daba una DESTINO falsa. `c23lk_reloc.py` tiene el
  mismo fallo;
- las reubicaciones de 16 bits llevan el `r_offset` **en el semipalabra**
  (instrucción + 2): por eso las de `sfxrevc` salen en `.text+0x222`. Con la
  llave sin normalizar, la función del fallo **no llegaba a compararse**;
- y en las unidades Metrowerks el **mismo** `EMB_SDA21` sale en +2 en nuestro
  objeto y en +0 en el del troceador. Eso solo daba **422 falsos** «le falta una
  reubicación» en `dolphinsdk`. Llevándolo todo a la palabra: 422 → 4.

### 1.4 Y dos clases de FALSO POSITIVO que hay que anotar

| clase | cuántas | por qué |
|---|---|---|
| **nombres repetidos en `symbols.txt`** | 278 | **119 nombres de 38.028 aparecen más de una vez**: `gcc2_compiled.` **389 veces**, `...data.0` 28, `...bss.0` 21, `@1` 13, `ln2HI` 2. El diccionario nombre→dirección se queda con la última y resuelve a una dirección inventada |
| **nombres-CONTADOR del compilador** | 18 | `@178`, `$LC3`, `...data.0`, `_.tmp_N.M` no significan nada fuera de su objeto: el `@178` de `symbols.txt` es el de **otra** unidad. Excepción: el nombre con la dirección **dentro** (`ln2HI_80500378`, `@311_80452860`), que lo pone el troceador y es único |

De **313 «divergencias»** del primer barrido quedaron **19** reales.

### 1.5 EL RESULTADO — 23.272 símbolos idénticos comparados, 526 unidades

Ninguna divergencia es del patrón exacto de `sfxrevc` («mismo nombre, otro
addend»): **ese frente está cerrado**. Las 19 son «mismo código, símbolo
distinto», que es la misma mentira por otra puerta.

| unidad | símbolo | qué pasa | ¿lo ve `audit.py`? |
|---|---|---|---|
| `zFe` | `_vt.16MemcardCallbacks` | **12** entradas de vtable corridas | **NO** (es dato) |
| `zEcstasy` | `EnvmapTargetNames` | **2** punteros a cadena a 1 y 3 bytes de donde toca | **NO** (es dato) |
| `zEAXSound2` | `_19SFXCTL_3DTrafficPos.s_TypeInfo` | clase base equivocada | **NO** (es dato) |
| `hlafile` | `ASYNCFILE_init__Fii` | `request`/`freequeue` en orden cambiado | no (ya FALLA por otra causa) |
| `gc_interface` (realmemcard) | ctor de `GCInterface` | llama a `Init` donde el objetivo llama a `Clear` | **SÍ**, ya lo canta |

**Las tres primeras son crédito falso SILENCIOSO**: 100 % en `measure`, y
`audit.py` **no las puede ver porque sólo audita funciones**. Es la confirmación
medida de la limitación que ya está escrita en `HERRAMIENTAS.md` §3.

Arreglé dos (§1.6). Quedan **5 DESTINO** y **4 `SOLO_OBJ`** (3 en `metrotrk`,
cuyo `.c` ni se compila, y `OSGetTime`), todas listadas en
`c24fx_addend_final.csv`.

**Tres más, en el cajón `SIN_SITUAR`** (donde sólo un lado se puede situar), que
son igual de reales y también invisibles para `audit.py`:

| unidad | vtable | el objetivo tiene | nosotros |
|---|---|---|---|
| `zAI` | `_vt.11AIAvoidable` | `__pure_virtual` | `OnUpdateAvoidable__11AIAvoidable...` |
| `zCamera` | `_vt.14ITrafficCenter` | `__pure_virtual` | `GetTrafficBasis__14ITrafficCenter...` |
| `zFe` | `_vt.14ItemTypeToggle` | `SetFocus__14FEButtonWidget` | `SetFocus__14ItemTypeToggle` |

O sea: en el original esos dos son **virtuales puros** (`= 0`) y `ItemTypeToggle`
**no redefine** `SetFocus`. Tres arreglos de cabecera para quien lleve esas
unidades.

El resto del reparto: `INTERNO` 31.311, de las cuales **31.295 son el par
`$LC`/`lbl_` de siempre** (la §4 de la r23: mismo dato, otro nombre), y las 16
restantes son los pools en comodín ya documentados (`sf_atan` 8, `GXTev` 4…).
`SIN_SITUAR` 896, de las que 446 son pool en los dos lados.

### 1.6 Dos arreglos de fuente que salieron del barrido

**`zEcstasy` — `EnvmapTargetNames`, medido +0 B, 0 unidades bajan.**
`EcstasyE.cpp:1625` declaraba `lbl_803DDFEB` y `lbl_803DDFFF`; el objetivo
apunta a `lbl_803DDFEC` y `lbl_803DDFFC`. Leído del ELF: `803DDFEB` es la
**cadena vacía** (el NUL de la anterior) y `803DDFFF` es `"GET_ENVMAP0_FULL"`.
Eran `"TARGET_ENVMAP0D"` y `"TARGET_ENVMAP0_FULL"`. Corregido; las dos
divergencias desaparecen.

**`zFe` — el vtable de `MemcardCallbacks`, medido +0 B, 0 unidades bajan.**
Y aquí está el mecanismo, que es el de «cabeceras que mienten»: **hay DOS
`IGameInterface` en el árbol**,

    src/Packages/realmemcard/.../include/common/realmemcard/memcard_interface.h:352
    src/Speed/Indep/Src/Frontend/MemoryCard/MemoryCardHelper.hpp:253

y la del frontend, que es la que ve `MemcardCallbacks`, tenía **otro orden de
virtuales**: `LoadReady` al final en vez de entre `SaveDone` y
`CheckLoadedData`, `CardChanged` antes de `CardChecked`, y `CardRemoved` y
`SetAutosaveDone` intercambiados. Resultado: doce entradas del vtable corridas.
Reordenada la del frontend al orden de la de realmemcard (que ya era el
correcto) → **las 12 divergencias desaparecen**.

**Veda anotada**: reordenar las declaraciones en la clase DERIVADA
(`MemoryCardCallbacks.hpp`) **no hace nada** — probado, reconstruido y medido:
el vtable lo fija la clase base. Revertido.

**Veda anotada (`gc_interface`)**: cambiar los dos `mTaskMsg.Init()` /
`mFindResult.msg.Init()` por `.Clear()` **cuesta −396 B y una función**, porque
`GCMessage::Clear` es `inline` en esa cabecera y se expande en vez de emitir el
`bl`. Revertido; `realmemcard` vuelve a 69.728/69.728 B. El arreglo de verdad
pasa por que `Clear` no sea inline en esa unidad, y eso no lo he probado.

---

## 2. `LibSN/sndvd.c`: el ensamblador NO se puede quitar, y está medido

La unidad sigue en **1.584/1.584 B, 100 %, 11 funciones, `audit.py` 0 FALLA**, y
ahora **congelada** (`frozen.py cong`, huella `2eea38a1fbcd32a0`).

### 2.1 `DSIExcHandler` — imposible en C, con la medida

No es una función: es el **vector de excepción del DSI**. El despachador entra
con `r4` apuntando al `OSContext` y con `r0`, `r1`, `r2` y `r6..r31` todavía
cargados con los valores **del programa interrumpido**, que es justo lo que
guarda (`stw 0,0(4)`, `stw 1,4(4)`, `stmw 6,0x18(4)`). Tres cosas, ninguna
expresable:

1. **leer `r1` y `r2` como valores** — son el puntero de pila y la base de datos
   pequeños; GCC los ha usado antes de la primera sentencia;
2. **los SPR de supervisor**: `mfdsisr`, `mfdar`, `mtsrr0/1`, `mtcrf 255`,
   `mfspr GQR1..GQR7`;
3. **las dos ramas de cola** (`b DSIHandler`, `b DSIentry`).

**Medido** con el propio `ngccc` y los cflags de la unidad:

| forma en C | qué sale |
|---|---|
| `void h(void) { DSIHandler(0,0,0,0); }` | **12 palabras**: `stwu r1,-8(r1)` / `mflr r0` / `stw r0,12(r1)` + los cuatro `li` + `bl` + cuatro de epílogo |
| lo mismo con `for(;;)` detrás | **el prólogo sigue**; sólo cambia `blr` por `b .` |
| el objetivo | **5 palabras**, sin prólogo y sin epílogo, terminando en `b` |

GCC 2.9 **no emite llamadas finales** — la misma razón por la que los siete
alias de 4 B de `ppc2D2` se quedaron como estaban. Y `register X __asm__("rN")`
sí compila sin marco (probado: `li r0,0; stw r0,0(r4); blr`), pero no da acceso
a `r1`/`r2` ni cambia el `blr`.

**Se queda, y queda ANOTADO en el fuente**: la cabecera del bloque `__asm__`
lleva ahora las tres razones y la medida, y dice explícitamente que esa función
**no está decompilada, está transcrita**.

### 2.2 El `__asm__(".long 1")` — tampoco, y ahora se sabe qué es

El objetivo lleva `00 00 00 01` justo detrás del `bl OSReport` del `default:`
(0x80312A60). Lo he identificado:

- **es la ÚNICA palabra inválida de los 590 `.s`** del troceador (barrido
  hecho). No es un idioma del compilador: es un opcode **ilegal** en PowerPC,
  o sea la trampa de depuración del Target Manager de ProDG detrás del aviso;
- `__builtin_trap()` **sí existe** en este `ngccc`, pero emite `tw 31,0,0`
  (`0x7FE00008`) y además es `noreturn`: se lleva por delante el resto de la
  función (la de prueba salió de **una** palabra);
- ni la llamada varargs ni la que devuelve `struct` emiten nada detrás del `bl`
  (las dos compiladas y volcadas: `crclr cr1eq` / `bl` / y a continuación ya el
  `lis r0,5`).

Anotado igual en el fuente. Los `mtspr 1013` de `DisDvdBP`/`EnaDvdBP` y los
nueve `__asm__("nombre")` de renombre **son legítimos y no se han tocado**.

---

## 3. `audit.py`: el falso FALLA, arreglado y regresionado

**El fallo.** `ttInit__Fv` de `timerthread` cantaba
`tipo/addend de reubicacion distinto en systemtasksubs_804D63F8`. El objetivo
referencia `systemtasksubs_804D63F8+0` y nosotros `TimerThreadStack+0x1000`;
`symbols.txt` pone `TimerThreadStack` en `0x804D53F8` **con tamaño `0x1000`** y
`systemtasksubs` en `0x804D63F8`: **es la misma dirección**. El camino
`mismo_destino` ya existía, pero `DirsMW` no sabía situar
`systemtasksubs_804D63F8`, así que `ea` salía `None`.

**El arreglo.** El troceador tiene **una sola forma** de inventar nombres:
pegarle la dirección al nombre, `<nombre>_8xxxxxxx`. `@N_ADDR` y
`gcc2_compiled._ADDR` —los dos casos que ya estaban, cada uno pagado con un
FALLA falso (vp6 en la r22, éste en la r23)— **son casos particulares de esa
forma**. `DirsMW` resuelve ahora la forma general, con dos cautelas:

- la dirección tiene que caer en una **sección cargable del ELF original**
  (`dirs.rangos`, calculado en `direcciones()`), y
- `__missing__` sólo entra si el nombre **no está ya** en el mapa, así que no
  puede pisar ningún símbolo real.

**La regresión, que es lo que importa en herramienta compartida.** Comparé la
`audit.py` de `HEAD` contra la parcheada sobre los **mismos objetos**, seguidas:

| tanda | unidades | líneas auditadas | diferencias |
|---|---|---|---|
| seis grandes + `timerthread` | `zAI`, `zPhysicsBehaviors`, `zWorld`, `zMain`, `zEcstasy`, `zGameplay`, `timerthread` | **5.411** | **1**, la buscada: `ttInit__Fv` FALLA → ok |
| middleware con renombres `_ADDR` | `sndvd`, `hlafile`, `gc_interface`, `fopen`, `sn_buf`, `sf_asin`, `sfrsf`, `scrsfl`, `pathaction`, `TokenEntropy`, `DebuggerDriver`, `OS` | **123** | **0** (4 FALLA antes, 4 después) |

**18 unidades, 5.534 líneas, un solo cambio y es el pretendido.** Los FALLA
reales (`hlafile` por el literal `__FILE__`, `gc_interface` por el símbolo
equivocado) **siguen saliendo**: el arreglo no tapa nada. Confirmado en segunda
pasada, como manda la regla 2.

---

## 4. Lo que NO he probado

1. **`hlafile`**: el orden de `request` / `freequeue`. El objetivo emite
   `stw freequeue+4` → `stw request` → `stw freequeue`; nosotros
   `stw freequeue+4` → `stw freequeue` → `stw request`. Es un problema de
   planificación de tres sentencias y no lo he barrido. La función además falla
   por el literal `__FILE__` (`d:/packages/realcore/...` contra nuestra ruta),
   que es un frente distinto y de todo el árbol.
2. **`gc_interface`**: sacar `GCMessage::Clear` de `inline` en esa unidad, que
   es lo que haría falta para emitir el `bl Clear` del objetivo sin perder los
   396 B.
3. **`zEAXSound2`**: `DEFINE_CREATABLE(0x50000, SFXCTL_3DTrafficPos,
   SFXCTL_3DObjPos)` en `CARSFX_TrafficFX.cpp:131` — el objetivo dice que la
   base es **`SFXCTL_3DCarPos`**. No lo he tocado porque la unidad la está
   editando otro agente ahora mismo.
4. **Las tres vtables de §1.5** (`AIAvoidable`, `ITrafficCenter`,
   `ItemTypeToggle`): diagnóstico hecho, arreglo no intentado; son cabeceras de
   `zAI`, `zCamera` y `zFe`.
5. **Las 896 `SIN_SITUAR`**: 446 son pool en los dos lados y el resto está sin
   clasificar una a una. Ahí puede quedar más de lo mismo; el CSV las lleva.
6. **Comparar las secciones de DATOS enteras.** Mi barrido compara los símbolos
   **con tamaño**; lo que no tiene símbolo (relleno, huecos entre objetos) no lo
   mira nadie todavía, ni `audit.py` ni yo.
7. **No he reenlazado el DOL.** Mi única unidad `Matching` tocada es `sndvd` y
   sólo lleva comentarios: mide 1.584/1.584 B (o sea, byte a byte igual al
   extraído), `audit.py` 0 FALLA y `trypromo.py` confirma que ya está en la
   lista de enlace con NUESTRO objeto. `zFe` y `zEcstasy` son `NonMatching`.
8. **Los dos procesos zombis** de sesiones anteriores (§1.1). No los he matado.

---

## 5. Ficheros

**Fuente y herramientas del árbol** (sin commit):

| fichero | qué |
|---|---|
| `scripts/audit.py` | `DirsMW` resuelve la forma general `<nombre>_8xxxxxxx` contra los rangos del ELF |
| `src/LibSN/sndvd.c` | sólo comentarios: la medida que demuestra que el `asm` no tiene forma en C |
| `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp` | `lbl_803DDFEB`→`EC`, `lbl_803DDFFF`→`FC` |
| `src/Speed/Indep/Src/Frontend/MemoryCard/MemoryCardHelper.hpp` | orden de virtuales de `IGameInterface`, con la nota de por qué |

**Scratchpad, prefijo `c24fx_`:**

| script | qué hace |
|---|---|
| `c24fx_reloc.py` | heredero de `c23lk_reloc.py`: no revienta con los comodines, `bytes.index` en vez del corte O(n²), addend con signo, filtra `.debug` y el ruido de los saltos internos |
| **`c24fx_fnreloc.py`** | **el barrido bueno**: por SÍMBOLO, con la comprobación de «idéntico tras tapar las reubicaciones» y la resolución por dirección/contenedor |
| `c24fx_build1.py` | compila un fuente suelto con los cflags exactos de su unidad **a una salida arbitraria**, sin tocar `build/GOWE69/src` (es lo que hace posible la autoprueba con ocho agentes) |
| `c24fx_addend_final.csv` | las divergencias, una por línea |
| `c24fx_reg_*.txt`, `c24fx_reg2_*.txt` | las dos regresiones de `audit.py` |
