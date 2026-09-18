# r68 — escéptico 1 (lente del enlace)

Ataque a `r68-plan-datos.md` sobre la COLOCACIÓN: estripado, keep.lst, compensaciones, alineación.
HEAD 3c08534b. Solo lectura: nada en `src/`, `config/` ni `build/`. Todo enlace se hizo en un
temporal con `scratchpad/lote5_68/relink.py` (base: 9619ba57 en 9,6 s) y toda compilación con una
copia de `trysrc.py` que escribe en `scratchpad/refuta68/try/`. Scripts en `scratchpad/refuta68/`.

## 0. Resumen

| | afirmación | resultado |
|---|---|---|
| **1** | §4 + §8: los paquetes de keep.lst de la tanda 2 «no mueven el DOL enviado» | **REFUTADA, medida.** L8, L9, L10, L11 y L12 rompen 9619ba57 cada uno por separado |
| **2** | §4-prefijo / K8: zDynamics y zMission van «sin `"bad_alloc"`» | **REFUTADA, medida.** Sin su `@lc`, el DOL rompe; sin él en el primer, la `.rodata` sale desordenada. Con él, el objeto sale idéntico |
| **3** | K9/E2: 21 bloques condicionados a la palanca `.data` a 8 | **8 de los 21 son incompatibles** con la frontera actual (zEcstasy y zFe empiezan en 4 mod 8). Medido en zEcstasy |
| K1 | `size & ~(sh_addralign-1)`, cola, alineación de la sección de entrada | **Sobrevive**: 51 de 51 símbolos reales, 0 discrepancias |
| K3/K4 | keep por nombre; una entrada inexistente no da error | **Sobreviven**, con un matiz: también casan las entradas sin `fichero.o:` |
| K10 | borrar marcadores no mueve el DOL | **Sobrevive**: `clamp.c:3`, que era el único sin medir, da 9619ba57 |
| L2 A1 | UMath/USpline «ALLOC idénticas»; Geometry medido solo por separado | **Sobrevive**: relink 9619ba57 de los dos paquetes |
| K5 | «repetir T1c con `= 0`» lo decide | **DÉBIL**: por K1 también rompería; esa medida no discrimina |

Mi propia sospecha de que el primer fundiría el `"STL"` vivo de zFoundation/zMain **cayó al medirla**
(§4.3). La dejo escrita para que nadie la repita.

## 1. REFUTADA: los paquetes de keep.lst de la tanda 2 rompen el DOL que se envía

**La afirmación.** §8: «Los lotes de la tanda 2 no mueven el DOL enviado, porque se enlaza el objeto
extraído». §4 dice «Sustituir estas 33 líneas», «→ borrar», «A -> B».

**El fallo.** keep.lst es UNO solo para el enlace real, y en una pendiente ese enlace usa el objeto
EXTRAÍDO, `build/GOWE69/obj/.../zX.o`. Ese objeto trae justo los símbolos `gap_06_*_data` y `lbl_*` que
el paquete borra: por ejemplo `gap_06_8041A5C4_data`, de 4 B, GLOBAL, en la `.data` a 4 del extraído
de zEcstasy. Al quitar la línea, K1 se lo lleva.

**Medida** (`refuta68/keepvar.py`): keep.lst sin las líneas que el §4 sustituye o borra, objetos por
defecto, una variante cada vez.

| paquete | líneas quitadas | DOL | `.data` del DOL |
|---|---:|---|---|
| L8 zEcstasy (las 43 del §4) | 43 | d1181814… **ROTO** | 267.232 → 266.944 |
| L9 zCamera | 5 | 3e9f2724… **ROTO** | → 267.200 |
| L10 zPhysics | 3 | 5825d959… **ROTO** | → 267.168 |
| L11 zWorld | 3 | ae4e2d98… **ROTO** | igual de tamaño, contenido distinto desde 0x80438F0A |
| L12 zTrack | 8 | 7a7a9b2f… **ROTO** | → 267.200 |
| L14 zSpeech (`gap_06_8043597C_data`) | 1 | 9619ba57 OK | — |

L14 sale bien porque ese hueco es de 4 B en una `.data` a 8, y K1 no le quita nada.

**Por qué no lo vio nadie.** Los tests de la tanda 2 (lote1 V5, lote6 M4/M5, lote4 zWorld, y el
§3 «hash del enlace = base») enlazan NUESTRO objeto en lugar del extraído. En ese enlace los nombres
viejos ya no existen y el cambio es inocuo. Es el caso CarRender: correcto en el objeto de prueba,
roto en el enlace real. El cierre del coordinador («build; sha1 del DOL») lo detectaría, pero el
paquete, tal como está escrito, no se puede aplicar.

**Corrección medida.** Añadir los 58 nombres nuevos de L9+L10+L11+L12 SIN quitar ninguna línea da
**9619ba57**. Ninguno de esos nombres existe en los extraídos (grep de su tabla de símbolos), y por
K4 una entrada inexistente es inocua. Regla: en una pendiente, keep.lst solo AÑADE; las líneas
viejas se retiran en la promoción de la unidad. Las 10 líneas R1 de L8 (huecos de 1-3 B) sí se
pueden quitar, porque K1 no les quita nada: las 10 dan «quitado 0» en la tabla de §3.1.

## 2. REFUTADA: el prefijo de zDynamics y zMission sin `"bad_alloc"` (tanda 1, L2)

**2.1 El `"bad_alloc"` de cc1plus está muerto y hoy vive bajo un paraguas.**

En zDynamics:
- `$LC162` (`"bad_alloc"`, 10 B, en `.rodata`+0x34) tiene 0 reubicaciones (`relrefs.py`) y no está
  en keep.lst;
- sobrevive porque cae dentro del `.size 0x5C` de `lbl_803D3DC8`, el asm guardado por keep, que
  cubre desde zDynamics.cpp:20 hasta la mitad de Collision.cpp:9.

zMission es igual: `$LC60` bajo `pad_05_803F6890_rodata`.

`refuta68/badalloc.py` recorta en una copia del `.o` el `st_size` del paraguas a 0x34, que es lo que
pasa al quitar el asm:

| unidad | DOL | primera diferencia |
|---|---|---|
| zDynamics | c0a2a9fa… **ROTO** | 0x803D3DFC = 0x803D3DC8 + 0x34 |
| zMission | 1cf2951d… **ROTO** | 0x803F68C4 = 0x803F6890 + 0x34 |

`refuta68/umbrella.py` recorre TODOS nuestros objetos enlazados buscando símbolos con tamaño que
cubren otro símbolo con tamaño sin referencias ni keep. Salen **exactamente estos dos** y ninguno más.

**2.2 El primer sin `"bad_alloc"` desordena la `.rodata`.** `refuta68/zdyn.py V5`: primer con 5
literales y la mitad de Collision.cpp:9 borrada. La `.rodata` sale de 544 B, pero con
`… "%f,%f,%f,%f" "STL" "bad_alloc"`: el `"bad_alloc"` queda en 0x50 en lugar de 0x34. Ningún keep.lst
arregla eso.

**2.3 El primer con los 6 literales da el objeto exacto.** `zdyn.py V6` (la receta S2 de zSim
entera): la `.rodata` es **idéntica byte a byte** a la actual (544 B, `"bad_alloc"` en 0x34). Aquí
GCC funde el literal del primer con el de cc1plus; la premisa de K8 («no puede llevarlo porque se
funden») está al revés.

**Paquete correcto** para zDynamics y zMission:
- receta S2 con los 6 literales;
- borrar también las mitades de Collision.cpp:9 y MissionEdit.cpp:5;
- **6** `@lc`, `"bad_alloc"` incluido;
- lcfix.

El relink no está medido: exige renumerar con lcfix sobre una copia de keep.lst, y la regla prohíbe
correr lcfix. Mis dos relinks sin renumerar rompen, como era de esperar.

## 3. `.data` a 8 en unidades que empiezan en 4 mod 8

Una sección de entrada con `sh_addralign = 8` empieza en un múltiplo de 8. Inicios de `.data` en
splits.txt:
- zEcstasy, 0x8041A5A4 (`ePolySlotPool` en symbols.txt);
- zFe, 0x8041B5CC (`gOnlineMainMenu`).

Los dos son ≡ 4 mod 8.

En la lista E2 de `sintesis68/lotes.py` hay **8 bloques** de esas dos unidades:
- zEcstasy: `ePShader.cpp:1042`, `eVShader.cpp:19`, `EcstasyEx.cpp:145`, `EcstasyE.cpp:287`,
  `eLight.cpp:135`;
- zFe: `zFe.cpp:388`, `FEManager.cpp:53`, `MoviePlayer.cpp:69` (unidad por grep de las SourceLists).

**Medida.** Con `patchalign.py` pongo la `.data` del extraído de zEcstasy a 8 y enlazo: afb4abef…
**ROTO**; la `.data` del DOL crece +32 B y la primera diferencia está en la cabeza de zEcstasy.
zFe solo está comprobado por la aritmética del inicio, no parcheado.

**Contradicción interna del plan.** K2 usa «0x8041D7EC no es múltiplo de 8» para negar que zMain
tenga colas a 8. Lote1, con ese mismo criterio, tendría que negarlas en zEcstasy, y las clasifica
como colas a 8: `eLightFlareParams` de 44 B que dejan 4, y `ScreenTint` `float[3]` que deja 4 B,
cosa imposible a 4, donde 12 & ~3 = 12. Una de dos:
- el inicio 0x8041A5A4 está mal (disputa K12);
- esos restos no salen de la regla K1.

Sin resolverlo, E2 son 13 bloques, no 21, y el 180 B del resumen está inflado.

Medida que lo decide: el inicio de `.data` de zEcstasy y zFe en `.debug_aranges`, y la CU de
`ePolySlotPool` en el DWARF. `jefe/aranges.py --list` no da la tabla por unidad; hay que sacarla
del volcado.

## 4. Lo que sobrevive al ataque

**4.1 K1, sobre enlaces reales.** `refuta68/k1check.py` compara `base.elf` con cada variante de §1
en los objetos EXTRAÍDOS. Mira los muertos aislados, es decir, un solo símbolo entre dos vivos:

| objeto | alineación de `.data` | muertos | quitado medido = predicho | resto en la COLA / en la CABEZA |
|---|---:|---:|---|---|
| zEcstasy | 4 | 41 | 300 = 300 B | — (un caso ambiguo, bytes a cero) |
| zCamera | 8 | 5 | 32 = 32 | — |
| zPhysics | 8 | 3 | 88 = 88 | 1 / 0 |
| zTrack | 8 | 8 | 56 = 56 | 2 / 0 |
| zWorld | 8 | 3 | 8 = 8 | — |

- 51 de 51 sin discrepancias.
- Los tamaños impares salen como predice la regla: `lbl_8041AE3A` de 26 B pierde 24, los huecos de
  1-3 B pierden 0.
- Es el mismo enlazador con dos alineaciones distintas, así que manda la de la sección de ENTRADA.
- `FEKeyInterpDefault` = {0,1,…,1} de lote4 no distinguía cola de cabeza: solo excluía el elemento 0.
  Estos 3 casos sí la distinguen.

**4.2 K3/K4.**
- Añadir 58 nombres inexistentes no da error y deja 9619ba57.
- **Matiz de K4:** una entrada SIN `fichero.o:` también casa. Las 3 líneas de L10 escritas como
  `lbl_8041EFF0`, sin prefijo, dan 9619ba57. Casa por nombre de símbolo en cualquier objeto: sirve
  para L15, pero una errata con nombre repetido engancharía otra unidad.

**4.3 K8 en zFoundation y zMain, a nivel de objeto.** `prefdup.py` encontró `"STL"` vivo de cc1plus
fuera del prefijo:
- zFoundation, `$LC164`, 104 reubicaciones;
- zMain, `$LC1043`, 10;
- además zAI y zSpeech.

Una sonda mínima (`fusion.py`, primer muerto + función que usa `"STL"`) FUNDE: una sola copia, en la
posición del primer.

Pero compilando las unidades reales con el primer de 6 literales en lugar del asm:
- **zFoundation**: `.rodata` de 2.328 B, `"STL"` en 0x58 y en 0x4C0;
- **zMain**: `.rodata` de 21.320 B, `"STL"` en 0x58 y en 0x2DA0.

Las dos idénticas en tamaño y con las dos copias. **No hay fusión en esas unidades**; la sonda
mínima no es representativa. Queda sin medir el enlace, que necesita renumerar con lcfix. En zAI y
zSpeech (L14) sigue sin medir; no lo extrapolo.

**4.4 K10, `clamp.c:3`.** Compilado sin la línea: objeto con las mismas secciones y relink 9619ba57.

**4.5 L2 A1.**
- UMath.cpp:107/173 + USpline.cpp:196 como literales, tal como los escribe el plan: relink
  **9619ba57**. Lote6 solo lo había medido con ALLOC, que no ve el destino de las reubicaciones.
- Geometry.cpp:607 + :639 JUNTOS, que el plan pedía medir así: relink **9619ba57**.
- zFoundation, zDynamics y zMission tienen 0 `@lc`, así que los literales flotantes no renumeran
  nada de keep.lst.

**4.6 Riesgo de cabecera de L6 (incompleto, pero inocuo).** `static uint32 gCarTypeNameHash;` de
CarCustomize.hpp llega a más unidades de las que lista el plan:
- zAnim, promocionada: AnimChooseArrest.cpp → uiQRCarSelect.hpp → CarCustomize.hpp (en `zAnim.o`:
  `.bss` local, 4 B);
- zFe, pendiente;
- zFe2 y zFeOverlay, que el plan ya lista.

En `base.elf` no aparece, y ese ELF conserva 479 locales de `.bss` (`_.tmp_0.10689`, `k.16707`…):
se estripa. Quitarlo de la cabecera no debería mover el DOL, pero no está compilado.

## 5. Débiles

- **K5, la medida propuesta no decide.** P1 dice que `static int x = 0;` va a `.data`. K1, medido
  arriba, dice que 4 B a 4 sin keep desaparecen enteros. T1c «con `= 0`» rompería igual (`.data` −4)
  aunque GCC lo emita, y la «hipótesis .bss» no se distingue. Medidas que sí deciden: la tabla de
  símbolos del objeto T1c (¿está `seen_yellow_screen.<N>` en `.data`?) o T1c con keep del nombre
  numerado.
- **K2 en zMain.** Con `.data` a 4 (inicio 0x8041D7EC), un global muerto de 4 B desaparece entero
  en el enlace original. Los huecos «enteros» de zMain (zMain.cpp:864 y :883) solo pueden ser
  estáticos, que no tienen símbolo en el ELF original y por eso no se estripan, o datos vivos.
  Acota su R4; no lo refuta.
- **K9** sigue sin medir en el enlace en forma C (lo reconoce el plan). Además, §3.
- **§4-prefijo de zOnline, incompleto.** Lista `zOnline.o:lbl_803a4234`, y keep.lst tiene otras 5 del
  mismo prefijo: `lbl_803A4240`, `lbl_803A4268`, `lbl_803A4274`, `lbl_803A4280`, `lbl_803A428C`.
  Dejarlas es inocuo por K4; keepchk puede quejarse.
- **K11** solo verificado en la forma «paraguas de `.size`» (§2): hay dos casos en todo el árbol
  promocionado. Las compensaciones por tamaño de sección (zWorld, zTrack) no las he atacado.
- **K8** en el enlace: medido solo en zSim (lote3). En zFoundation, zMain y zDynamics V6 lo está en
  el objeto.
- K12 y K13: no los he atacado.

## 6. Reproducir

```
python scratchpad/refuta68/keepvar.py              # §1: los 6 paquetes de la tanda 2 contra el DOL enviado
python scratchpad/refuta68/badalloc.py zDynamics lbl_803D3DC8 0x34
python scratchpad/refuta68/badalloc.py zMission pad_05_803F6890_rodata 0x34
python scratchpad/refuta68/umbrella.py             # §2.1: paraguas de .size en todo lo promocionado
python scratchpad/refuta68/zdyn.py V5 ; python scratchpad/refuta68/zdyn.py V6
python scratchpad/refuta68/k1check.py L8 build/GOWE69/obj/Speed/Indep/SourceLists/zEcstasy.o
                                                   # necesita base.elf/L*.elf: relink.py save=...
python scratchpad/refuta68/prefdup.py ; python scratchpad/refuta68/fusion.py
python scratchpad/refuta68/trysrc2.py scratchpad/refuta68/spec_{a1,geom,clamp,k8,k8_zmain}.json
```
Los volcados (ELF enlazados, `try/`, `pa/`) están borrados; los scripts y los spec se quedan.
