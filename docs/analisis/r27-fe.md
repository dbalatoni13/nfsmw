# Ronda 27 — `zFEng`: el 100 % que no lo era

Base al empezar: `report.json` da `zFEng` al **100,0000 %**, 71.460/71.460 B, 343
funciones. Las tres secciones **no cuadraban** con el objetivo (r26-lk §6).

## 0. Resultado en una tabla

| sección | objetivo | ANTES | AHORA | pendiente |
|---|---|---|---|---|
| `.text`   | **71.460** | 73.948 (+2.488) | **73.756 (+2.296)** | +2.296, **diagnosticado al byte** |
| `.rodata` | **2.632**  | 1.920 (−712)    | **1.888 (−744)**    | −744, itemizado en §4 |
| `.data`   | **308**    | 336 (+28)       | **308 (exacto)**    | tamaño exacto; queda **1 objeto de 4 B** mal colocado |
| `.bss`    | 332        | 332             | 332                 | — |
| `.ctors`  | 4          | 4               | 4                   | — |

`matched_code` **no se mueve**: 71.460/71.460 B, 343 funciones, **100,0000 %**
antes y después. Los 192 B de `.text` y 28 B de `.data` que he quitado eran
**crédito falso**: objdiff no los comparaba. Las tres vecinas (`zFe`, `zFe2`,
`zFeOverlay`) miden **+0 B, +0 funciones** contra su base. `audit.py`: **dos
pasadas limpias**, cero fallos.

**Y con una bandera más, `.text` queda a 28 B**: con `-fno-implement-inlines`
(compilado y medido, **no aplicado**: `configure.py` me está prohibido) el
objeto mide **71.488 B** contra 71.460, y la diferencia entera son tres símbolos
concretos (§3).

---

## 1. Por qué objdiff daba 100 % — y por qué no es exclusivo de `zFEng`

objdiff empareja **por nombre de símbolo**. Todo lo que nuestro objeto emite y
el del objetivo no tiene **no se compara y no penaliza**. En `zFEng` eran
**156 funciones, 2.488 B**, y el objetivo tiene **cero** símbolos que nosotros no
tengamos.

**Esto no es un caso aislado.** `c27fe_secs.py` compara el **tamaño de cada
sección** nuestra contra la del objeto extraído, unidad por unidad. Salen
**393.208 B de `.text` de más repartidos en 184 unidades**, y seis SourceLists
que `report.json` da al **100 %** llevan de 3.996 a 18.908 B sin comparar:

| unidad | % en report.json | `.text` de más |
|---|---|---|
| `zLua` | 100,00 | **+18.908** |
| `zSim` | 100,00 | **+18.312** |
| `zAnim` | 100,00 | +9.076 |
| `zMisc` | 100,00 | +8.360 |
| `zRender` | 100,00 | +4.028 |
| `zMiscSmall` | 100,00 | +3.996 |
| `zFEng` | 100,00 | +2.296 (ya diagnosticado) |

Las de porcentaje bajo salen peor todavía (`zPhysics` +43.960, `zEAXSound`
+24.080, `zMain` +20.188). **Ninguna puede promocionar mientras eso siga ahí**,
y el porcentaje no lo dice. La herramienta tarda 3 minutos y está en el
scratchpad.

---

## 2. `.text`: el mecanismo, con caso mínimo

Las 156 funciones de más son **accesores triviales** (`GetName`, `GetNext`,
`SetFlags`, `GetNumRows`…) **definidos DENTRO de la clase** en las cabeceras de
`FEng`. La nota de memoria «en clase = inline en GCC 2.9, no emite símbolo» es
cierta **sólo si la clase no es polimórfica**.

Caso mínimo, tres compilaciones de 1 s (`c27fe/t2.cpp`..`t4.cpp`):

| fuente | emite `Get`/`Set` fuera de línea? |
|---|---|
| clase **sin** virtuales, accesores en clase | **no** |
| clase con virtuales y **`~A` definido en este TU** | **SÍ, globales, 8 B cada uno** |
| clase con virtuales y `~A` **sólo declarado** | no |
| clase con virtuales **todos en clase** | no |

Es `import_export_class` de GCC 2.95: si la clase tiene virtuales y su **primer
virtual no-inline no-puro** («key method») está **definido en esta unidad**, la
clase queda *exportada* y entonces **todos** sus miembros —incluidos los
definidos en clase— salen fuera de línea, **globales**, al final del `.text`.
`FENode`, `FEObject`, `FEListBox`, `FECodeListBox`, `FEPackage`, `FEScript` y
`FEMessageResponse` cumplen las dos condiciones.

Las 37 vtables son **idénticas** en los dos objetos, así que las clases están
exportadas también en el original: **la única forma de que el original no las
emita es `flag_implement_inlines = 0`**, o sea `-fno-implement-inlines`.

**Medido sobre la unidad real (no aplicado):**

```
                objetivo    ahora    +flag
.text              71460    73756    71488     <- +28 B
.rodata             2632     1888     1888
.data                308      308      308
.bss                 332      332      332
```

Y la diferencia son **tres símbolos, ni uno más**:

- **sobran** `__as__9FEVector2RC9FEVector2` (24 B) y `__as__9FEVector3RC9FEVector3`
  (32 B), los dos **weak** (§3),
- **falta** `GetPCellData__9FEListBoxUlUl` (28 B), que el objetivo **sí tiene** y
  que llaman fuera de línea `ConnectListBoxResources` (2 sitios) y
  `ProcessListBoxTag` (4 sitios), las dos al 100 % en las dos versiones. Con la
  bandera puesta pasaría a **referencia indefinida**: hay que sacar su cuerpo de
  la clase a `FEListBox.cpp` **antes** de tocar la bandera.

**Propuesta verificada para `configure.py`** (no aplicada, requiere permiso):
`extra_cflags=["-fno-implement-inlines"]` en `Speed/Indep/SourceLists/zFEng.cpp`,
**junto con** sacar `FEListBox::GetPCellData` de la clase. Antes de generalizarla
a otra unidad hay que repetir el mismo par de medidas: la bandera **quita**
copias fuera de línea que otra unidad puede necesitar.

---

## 3. Lo que sí he arreglado (tres ensayos, cada uno medido)

### c1 — `FEKeyInterpDefault`: 28 B muertos de `.data`  → `.data` 336 → **308**

`FETypeNode.cpp:9` definía `i32 FEKeyInterpDefault[7] = {0,1,1,1,1,1,1}` (el
propio comentario decía `address: 0xFFFFFFFF`). **No está en el objetivo**: he
buscado su contenido byte a byte en `.data` **y** en `.rodata` del rango de
`zFEng` y no aparece, y **ninguna de las 526 unidades lo referencia**. Comentado.
`.data` pasa a **308 B exactos**. `.text` y `.rodata` sin tocar, 100,0000 %.

*Ojo con el PS2*: `PS2_globals.hpp` **sí** lo tiene (`data 614c00`, 28 B), entre
`FEKeyNode::NodePool` y `FEMessageResponse::NodePool`. Es una diferencia real de
plataforma, no un error de lectura.

### c2 — 13 funciones sin un solo llamante  → `.text` 73.948 → **73.808**

Once eran **cuerpos vacíos `{}`** marcados `// STRIPPED` en la fuente: emiten un
`blr` de 4 B que objdiff no compara y que devuelve basura. Dos tenían cuerpo real
pero tampoco existen en el objetivo. Barrí **las 526 unidades** buscando
reubicaciones a sus símbolos (`.rela.*` de todas las secciones, no sólo `.text`):
**cero llamantes, cero entradas de vtable**.

| símbolo | B | fichero |
|---|---|---|
| `GetDefault__11FEFieldNodePv` | 56 | `FETypeNode.cpp:22` |
| `GetTypeSize__10FETypeNode` | 40 | `FETypeNode.cpp:48` |
| `Insert/AddEvent/FindEvent/Delete/SortEvents__11FEEventList` | 4×5 | `FEEvent.cpp` |
| `ComputeSize__10FEKeyTrack` | 4 | `FEKeyTrack.cpp:35` |
| `FELowerCase__Fc`, `FEStrInStr__FPCcT0` | 4+4 | `FEList.cpp:10,25` |
| `SetPivot__8FEObject…`, `SetSize__8FEObject…` | 4+4 | `FEObject.cpp:315,362` |
| `FindObjectByName__9FEPackagePCc` | 4 | `FEPackage.cpp:165` |

Las declaraciones se quedan en las cabeceras; los dos cuerpos reales quedan bajo
`#if 0` para no perder la reconstrucción.

### c3 — `#pragma interface` en `FEObjectCallback.h`  → `.text` −52 B, `.rodata` −32 B

El objetivo **importa** `_vt.16FEObjectCallback` y `_._16FEObjectCallback` en
`zFEng` y en `zFe2`; **sólo `zFe` los define, y como GLOBAL (bind 1), no weak**.
Nosotros los definíamos **weak en las tres**. Esa combinación —una unidad los
exporta como globales, las demás los importan— es la firma exacta de
`#pragma interface` + `#pragma implementation`, patrón que el árbol ya usa en
`zCamera.cpp`.

Aplicado: `#pragma interface` en `FEObjectCallback.h` y
`#pragma implementation "FEObjectCallback.h"` al principio de `zFe.cpp`.
Resultado en las cuatro unidades FE: **+0 B, +0 funciones**, y `zFEng`/`zFe2`
pasan a importarlos igual que el objetivo.

**Por qué es seguro y está comprobado**: `#pragma interface` sólo afecta a las
clases declaradas en ESE fichero, y `FEObjectCallback.h` declara **una**. Barrí
las 526 unidades buscando cualquier símbolo con `16FEObjectCallback`: aparecen
**sólo** en `zFe`, `zFe2` y `zFEng`, las tres medidas.

---

## 4. `.rodata`: los 744 B, itemizados

`c27fe_rodiff.py` alinea las dos secciones en bloques de 4 B y
`c27fe_roown.py` dice **qué función del objetivo referencia cada tramo**. Se
parte limpiamente en dos:

### 4.1 — 472 B que **nadie referencia ni en el objetivo**

Constantes muertas: el original las emitió y luego el optimizador se llevó el
código que las usaba. Es la firma de **llamadas a funciones vacías con literal de
cadena** (asserts y marcadores de perfil que en release se pliegan a nada). El
literal se emite al evaluar el argumento y **ya no se borra**.

| objetivo | B | contenido |
|---|---|---|
| +0..92 | 92 | `GAMECUBE`, `d:/mw/speed/indep/bware/inc/bware.hpp`, `bad_alloc`, `%f,%f,%f`, `%f,%f,%f,%f`, `STL` |
| +480..712 | **232** | `[HEADER]`, `Languages`, `[LABEL]`, `[PLATFORM]`, `[COMMENT]`, `[FLAGS]`, `[MAX_SIZE]`, `Major version: 3`, `Minor version: 3`, `Build number: 22`, `Languages: ` |
| +736..820 | 84 | `FEngine::Render()`, `Setup`, `Object traversal`, `SortObjects()`, `RenderObjectList()` |
| +888..924 | 36 | `FEngMalloc`, `FEngMalloc[somefile]` |
| +1000..1040 | 40 | `d:/mw/speed/indep/src/feng/FEPackage.cpp` (un `__FILE__`) |
| +1464..1576 | 88 | `Init`, `Hide`, `cmod`, `cset`, `cseta`, `cadd`, `wa`, `sr`, `sg`, `sb`, `r`, `g`, `b`, `a`, `w` |

Pista concreta para el primer bloque: `zFEng.cpp:8` define
`BWARE_NO_INLINE_GLOBAL_NEW`, que **apaga el `operator new` en línea de
`bWare.hpp`**. `GAMECUBE` sale de `bGetPlatformName()` de esa misma cabecera y
`bad_alloc` de `<new>` de STLport — nosotros los emitimos, pero **más tarde**
(+228 y +300) y sin los otros cuatro literales. Los 84 B de `FEngine::Render()`
son marcadores de perfil de `FEngine::Render` (`FEngine.cpp:761`), que existe y
casa al 100 % **sin** ellos.

### 4.2 — 240 B que **sí se referencian**: son literales DUPLICADOS y dos arrays fuera de sitio

**Aquí es donde `audit.py` no llega**: compara el **valor** del literal, así que
un `1.0f` duplicado en el objetivo y compartido por nosotros **pasa el audit y
pasa objdiff**. Es el mismo caso del `0.0f` de `zWorld`.

| objetivo | B | quién lo referencia | qué es |
|---|---|---|---|
| +168 | 8 | `Update__13FECodeListBoxf` | copia extra del SESGO `int->double` |
| +208 | 8 | `FEInterpLinear__FP10FEKeyTracklPv` | copia extra de `1.0f` |
| +396..412 | 16 | `__9FEListBox` | copias extra de `1.0f` y `-0.00138889f` |
| +428 | 16 | `SetNumRows__9FEListBoxUl` | copia extra de `1.0f` |
| +976 | 16 | `SetPosition__8FEObjectRC9FEVector3b` | **`358637bd` DOS VECES** |
| **+312** | **28** | `AddField__10FETypeNodePCcl` | **`FEKeyTypeSize`**: lo tenemos, pero en **+1892** |
| **+1064** | **44** | `FindTrack__C8FEScript…` | **`FETrackOffsets`**: lo tenemos, pero en **+1848** |

Los dos arrays (72 B) **ya están en nuestro objeto**, sólo que al final: son
`const` de ámbito de fichero, que en C++ tienen enlace interno y GCC **difiere**
hasta `finish_file` (van detrás de las vtables). En el objetivo salen **entre los
pools de las funciones**, o sea con enlace externo y emitidos en su punto de
definición. Es una diferencia de **fuente**, no de banderas.

---

## 5. `.data`: el tamaño ya cuadra, falta un objeto de 4 B

Con c1 la sección mide **308 B exactos**. El contenido casa byte a byte hasta
`+56`; de `+60` en adelante **todo está corrido 4 B**, porque el objetivo tiene
un objeto de 4 B más:

```
objetivo  +52 mpDefaultCallback | +56 ???? = 0x00000001 | +60 SysGUID | +64 ImpulseDir …
nuestro   +52 mpDefaultCallback |                        +56 SysGUID | +60 ImpulseDir …
```

**La propuesta de la r26 §6 —«mover `eFrameCounter` entre `mpDefaultCallback` y
`SysGUID`»— cuadra el tamaño pero NO el contenido**: el hueco de `0x8041D078`
vale **`0x00000001`** y `eFrameCounter` vale 0. Lo mismo con el otro «hueco»:
`lbl_8041D14C` = `0x0098967F` = 9.999.999, que es **nuestro
`FEngMemoryPoolUnknown`, ya colocado bien** (el PS2 lo llama
`FEngMemoryPoolHighwaterWarning`).

Lo que sé del objeto que falta, todo verificado:

- 4 B, valor `0x00000001`, en `.data`;
- **nadie lo referencia**: cero reubicaciones a `lbl_8041D078` en **todo** el
  árbol de objetos extraídos y en `zFEng.s`;
- **no tiene símbolo en el ELF original ni entrada DWARF**. Regla empírica que
  sale de aquí: **un `static` muerto pierde su símbolo local en el ELF final**
  (le pasa igual a `FEngMemoryPoolUnknown`);
- por el orden de emisión, se define **entre `FECodeListBox.cpp` y `FEngine.cpp`**
  — 13 ficheros, ninguno de los cuales tiene hoy variables de ámbito de fichero;
- **el PS2 no lo tiene**: entre `mpDefaultCallback` y `SysGUID` el PS2 pone
  `FEKeyNode::NodePool`, `FEKeyInterpDefault` y `FEMessageResponse::NodePool`, y
  en GameCube los dos NodePool están en `.bss` y `FEKeyInterpDefault` no existe.

Es un `static int/unsigned = 1` muerto. **No lo he inventado**: fabricar una
variable con nombre puesto a dedo sería falsificar la sección, y el tamaño ya
cuadra sin ella.

---

## 6. El aviso de `splits.txt` sigue en pie

`zFEng` **no puede promocionar** aunque se cierre todo esto: la frontera `.data`
entre `zFe2` y `zFEng` está 396 B por debajo (`unlockType` es de `zFe2`), tal y
como dejó dicho la r26. Además la de `.rodata` merece una segunda mirada: el
bloque de 92 B de `0x803EA7E8` (`pad_05_803EA7E8_rodata`) es la primera cosa del
rango y **no la emite ninguna función de la unidad**.

---

## 7. Lo que NO he probado

1. **`-fno-implement-inlines` de verdad**, en `configure.py`. Está compilado y
   medido (§2) pero no aplicado: el encargo me prohíbe `configure.py`. **Es el
   trozo grande: 2.240 de los 2.296 B.**
2. **Sacar `FEListBox::GetPCellData` de la clase.** No lo he tocado porque bajo
   las banderas de HOY sólo puede hacer crecer `.text` (los llamantes que ahora
   lo inlinan pasarían a llamarlo), y su valor está atado a la bandera del punto 1.
   **No he medido cuánto crece**: eso es lo primero que haría el siguiente.
3. **`__as__9FEVector2/3` (56 B).** El objetivo los importa de `zFe` igual que el
   destructor de c3, pero `FEVector2`/`FEVector3` viven en `FEMath.h` **junto con
   `FEQuaternion`**, y `__ml__12FEQuaternionRC12FEQuaternion` (212 B, en clase) el
   objetivo **sí** lo define en `zFEng`. Poner `#pragma interface` en `FEMath.h`
   se llevaría por delante esos 212 B. La hipótesis que queda sin probar es que
   el original tuviera `FEVector2/3` en **otra cabecera** y que nuestros
   reconstructores las fundieran en `FEMath.h`.
4. **Los 472 B de constantes muertas de `.rodata`** (§4.1). Hay que reconstruir
   llamadas a funciones vacías con literal; tengo el orden y el contenido exacto
   de cada bloque, pero no la sentencia.
5. **Los duplicados de literal** (§4.2, 168 B). No he buscado la forma de fuente
   que hace que GCC emita dos entradas de pool en vez de una.
6. **`FEKeyTypeSize` / `FETrackOffsets` con enlace externo** (72 B, §4.2). No lo
   he probado: mueve la posición dentro de nuestra `.rodata` pero, con los
   bloques de alrededor ausentes, no puede casar todavía. **Cuando se cierre §4.1
   es lo siguiente.**
7. **`dwbody.py` sobre el resto de `zFEng`.** El encargo lo sugería. No lo he
   hecho: al ver que `.text` se explica **al byte** con tres símbolos concretos y
   una bandera, auditar la estructura de 343 funciones que ya casan no habría
   dado nada.
8. **Las seis unidades del §1 que leen 100 %** (`zLua`, `zSim`, `zAnim`, `zMisc`,
   `zRender`, `zMiscSmall`, 62.680 B en total). Sólo las he contado.

---

## 8. Herramientas (scratchpad, prefijo `c27fe_`)

| script | qué hace |
|---|---|
| **`c27fe_secs.py`** | **el tamaño de cada sección contra el objeto extraído, TODAS las unidades.** Es la medida que objdiff no da |
| `c27fe_extra.py` | símbolos nuestros que el objetivo no tiene y al revés, por sección, con tamaños y binding |
| `c27fe_data.py` | `.data` nuestra con reubicaciones resueltas contra el ELF original, byte a byte |
| `c27fe_rodiff.py` | alinea `.rodata` en bloques de 4 B y lista los tramos que faltan/sobran con su texto |
| `c27fe_roown.py` | qué función del objetivo referencia cada tramo de `.rodata` (separa las constantes muertas de los duplicados) |
| `c27fe_scan.py` | símbolos `.text` de más por unidad, separando globales de weak |
| `c27fe/t1..t4.cpp` | el caso mínimo del §2, 1 s por compilación |

## 9. Ficheros de fuente tocados

- `src/Speed/Indep/Src/FEng/FETypeNode.cpp` (c1 y c2)
- `src/Speed/Indep/Src/FEng/FEEvent.cpp`, `FEKeyTrack.cpp`, `FEList.cpp`,
  `FEObject.cpp`, `FEPackage.cpp` (c2)
- `src/Speed/Indep/Src/FEng/FEObjectCallback.h` y
  `src/Speed/Indep/SourceLists/zFe.cpp` (c3)

**Sin commit.** `configure.py`, `config/GOWE69/*` y `splits.txt` **sin tocar**.
