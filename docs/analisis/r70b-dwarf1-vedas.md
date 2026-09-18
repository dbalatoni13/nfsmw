# r70b-dwarf1: el parser del oráculo y las vedas abiertas, respondidas

`scripts/dwarf1.py` lee el `.debug` DWARF-1 (92 MB) del ELF de GameCube y
extrae por función: CU con la ruta real `D:/mw/...`, parámetros y locales con
nombre+tipo+ubicación (registro o `r1+off`), bloques léxicos con rango, y el
árbol completo de `TAG_inlined_subroutine` con nombres. Es `dwbody.py` pero
del build FINAL, con el compilador exacto en cada CU:

    XGNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube

Uso:
    python scripts/dwarf1.py fn TrackCarCameraMover::Update
    python scripts/dwarf1.py cus
    python scripts/dwarf1.py at 0x9E82CC      # qué función contiene un offset
    python scripts/dwarf1.py die 0x9E82BE     # dump crudo de un DIE
    python scripts/dwarf1.py scan             # validación global

## 1. El formato, confirmado contra el productor (no contra la espec)

El GCC exacto está en el árbol: `orig/prodg/NGC_GNU_SRC/NGC/gcc/dwarfout.c`
+ `dwarf.h`. Con eso y los anclajes de r70b, el formato real queda cerrado —
y CORRIGE la hipótesis a mano de r70b en tres puntos:

- Los TAG eran al revés: **`TAG_local_variable` es 0x000c** (no "0x000e =
  TAG_variable": 0x000e está reservado), `TAG_formal_parameter` = **0x0005**
  (no 0x000d, que es `TAG_member`), `TAG_inlined_subroutine` = 0x001d ✓,
  `TAG_lexical_block` = 0x000b ✓.
- El "0x0012 repetido" NO es AT_mod_fund_type: es **AT_sibling** (0x0012,
  REF). AT_mod_fund_type real = 0x0063 (BLOCK2). La secuencia
  `0000 5500 0e00 2300 0501 0000 003f 0000` de r70b es, byte a byte:
  `00 55`=AT_fund_type(D2) `00 0e`=**FT_float**, `00 23`=AT_location(B2)
  `00 05`=bloque de 5: `01`=**OP_REG** `00 00 00 3f`=registro 63 → **f31**
  (la UBICACIÓN; los operandos de OP_REG/OP_BASEREG son de **4 bytes**,
  `output_reg_number` los emite como `.unaligned int`).
- Cada attr es `[u16 nombre]` con la **clase del valor en el nible bajo**
  (1=ADDR, 2=REF, 3=BLOCK2, 5=DATA2, 8=STRING...) y el DIE termina en su
  `length`; los DIE de `length==4` sin tag son marcadores `end_sibling_chain`.
  AT_sibling apunta al FINAL del subárbol (empieza el siguiente hermano):
  las 316 CUs encadenan por sibling EXACTO 315/315.

Registros: numeración DBX de GCC/PPC — 0-31 = r0..r31 (basereg 1 = SP),
32-63 = f0..f31. Validación: 2.503.029 DIEs + 809.626 marks (= 3,31 M, cuadra
con el walk de pubnames), 80 DIE truncados por el enlazador (se saltan),
30.267 rutinas con dirección, 10 rutinas "contenidas" en otra (statics
locales reales, p.ej. `Compare` dentro de `GatherVisibleIcons`).

Los 7 anclajes de r70b resuelven (`dwarf1.py at`): vert_comp→Update,
halfVP2→eProject, ndotl2→epCalculate, hole_punch_safety_margin→
HolePunchAvoidables, SignedMod→abstract inline en zAnim, bTan→abstract
inline en zAI, RenderFlaresOnCar ✓.

## 2. TrackCarCameraMover::Update — la veda R3, respondida en bytes

Ficha (`fn TrackCarCameraMover::Update`, 992 B, 0x80075C44):

| local | tipo | ubicación |
|---|---|---|
| this | TrackCarCameraMover* | r31 |
| dT | float | f29 |
| up | bVector3 | [r1+8] |
| fov | unsigned short | (optimizado fuera) |
| displacement | bVector3 | [r1+24] |
| distance | float | **f31** |
| vert_comp | float | **f30** |
| horiz_comp | float | (optimizado fuera) |
| hcomp | bVector3 | [r1+40] |
| look_offset | bVector3 | [r1+56] |
| lookdir | bVector3 | [r1+72] |
| m | bMatrix4 | [r1+104] |
| focal_dist | float | **f31** (reutiliza distance) |

Árbol de inlines (53): IsGameFlowPaused, SetSimTimeMultiplier, bVector3/bFill
(up), GetGeometryPosition ×6, __mi/bSub ×2, bLength{bDot, bSqrt}, bClamp
{bMax, bMin}, SetFieldOfView, __as/bCopy (Look=), **bTan ×2 muertos**,
__adv/bScale ×2 (inv_scale=f31), GetGeometryOrientation, __apl/bAdd,
bMatrix4/bVector4 ×4 (ctor de m), **GetVelocity** (0 B), SetTargetDistance
(376 B), SetdValDesired, SetFocalDistance, SetDepthOfField, GetPosition,
GetWorldID. Los 2 bTan muertos (.debug+0x9E8CED/0x9E8D25) y el GetVelocity
están donde el DWARF-1 de Ghidra los vio — y además hay 6 GetGeometryPosition
(r69 contaba 3 "de más": hay más plegados con rango 0).

**¿Qué ocupa f0/f13 en la ventana del racimo R3 (filas 134-143)? NADA.**
Desensamblado el original fila a fila:

```
127  lwz   r0, 0xD4(r31)        ; índice
131  rlwinm r0, r0, 2, 0, 29    ; índice*4
132-134  addi r11/r9/r10, ...   ; T+0x3B18 / T+0x3B30 / T+0x3B24
135  lfsx  f10, r9,  r0         \ tres cargas de TABLA (bTan inline
137  lfsx  f11, r11, r0          > como lookup: mismas bases relativas
139  lfsx  f12, r10, r0         / con stride 0xC, mismo índice)
141  stfs  f11, 0x38(r1)        ; look_offset.x
142  stfs  f12, 0x3C(r1)        ; look_offset.y
144  stfs  f10, 0x40(r1)        ; look_offset.z
145-147  stfs f30, 0x28/0x2C/0x30(r1)   ; hcomp = {vert_comp ×3}
```

- Los tres componentes de `look_offset` son **bTan plegado a carga de tabla**
  (`lfsx` directa): x←T[0x3B18+i], y←T[0x3B24+i], z←T[0x3B30+i]. Nunca
  transitan por f0/f13.
- **f0 y f13 están MUERTAS en la ventana**: última def de f0 = fila 118
  (`fdivs f0,f0,f31`), sin uso hasta su redefinición en fila 149 (`lfs`);
  última def+uso de f13 = filas 120/122 (`fmuls`/`stfs` a 0x20(r1)). No hay
  cantidad viva ninguna — con nombre (DWARF) ni sin él (desensamblado).
- La premisa de r61 §3.2 ("en el objetivo f0 y f13 están ocupados y por eso
  look_offset cae en f10/f11/f12") es **falsa en el binario**. Que T1a/T1b
  (fantasmas fr0/fr13) bajaran a 13 filas fue efecto lateral sobre el
  reparto de OTROS allocnos, no la reconstrucción de una ocupación real.
  La diferencia es asignador puro: los tres pseudos de las lfsx recibieron
  f10/f11/f12 en el original y f12/f0/f13 en el nuestro. (De paso, el DWARF
  da el orden de fuentes: bases consecutivas 0x3B18→x, 0x3B24→y, 0x3B30→z.)
- Regalo lateral para R1: el `bCopy` de `Look = *GetGeometryPosition()` tie-
  ne x=**f0**→0x90, y=**f13**→0x94, z=**f12**→0x98; el original almacena en
  orden 0x98/0x90/0x94 (152/144/148), exactamente los tres stfs de r63 §4.

## 3. RenderFlaresOnCar — todos los locales con nombre y ubicación

`fn RenderFlaresOnCar` (2908 B, zWorld.cpp): 126 params+locales, 49 inlines,
3 bloques. Los de r69 §3 confirmados y ampliados:

| local | tipo | loc | | local | tipo | loc |
|---|---|---|---|---|---|---|
| this | CarRenderInfo* | r28 | | flashHeadlights | unsigned int | **r14** |
| view | eView* | r27 | | intensity (fn) | float | f13 |
| position | bVector3* | r15 | | preview_part | CarPart* | r9 |
| body_matrix | bMatrix4* | r31 | | preview_part_id | CAR_PART_ID | r30 |
| force_light_state | int | r30 | | FlareCount | int | r29 |
| reflexion | int | r29 | | constFlicker | float | f24 |
| renderFlareFlags | int | r17 | | light_flare | eLightFlare* | r31 |
| Ftime | float | f29 | | name_hash | unsigned int | r11 |
| local_world | bMatrix4* | r26 | | is_brakelight | int | r30 |
| car_pixel_size | int | r31 | | is_headlight | int | r17 |
| base_headlight_intensity | float | f12 | | intensity (bloque) | float | f1 |
| base_brakelight_intensity | float | f11 | | sizescale | float | f31 |
| headlight_left_intensity | float | f27 | | lightPosition | bVector3* | r5 |
| headlight_right_intensity | float | f28 | | extraIntensity | float | f13 |
| brakelight_left_intensity | float | f26 | | NisLightPosition | bVector3 | [r1+8] |
| brakelight_centre_intensity | float | f20 | | ShaperLightsCharacters | eShaperLightRig | (sin loc) |
| brakelight_right_intensity | float | f25 | | idem ...Backup | eShaperLightRig | (sin loc) |
| reverselight_left_intensity | float | f21 | | | | |
| coplight_intensityR/B/W | float | f23/f30/f19 | | | | |

vs r69 §3: **flashHeadlights en r14 ✓** (nosotros r16). El inline del ctor
`ProfileNode(this, section_name:const char*, categories:int)` ✓ — con los dos
argumentos. `address` (dentro de `eFrameMallocMatrix`) está **optimizado
fuera AQUÍ** (el "address con registro" de r69 era el mdebug del PS2 alpha;
en el final GC no tiene casa). `world_view`, `camera`, `in_front_end`,
`visibility_state`, `car_type_info`, `profile_node`: optimizados fuera.

## 4. EvalState (FnRawStateChan) — locales y árbol

`EvalState__Q29EAGL4Anim14FnRawStateChanfPQ29EAGL4Anim5State` (456 B,
zEagl4Anim.cpp): this=r29, time=**f1**, s=r5; locales `c` (RawStateChan*)=
r30, `i`=r31, `k` (unsigned char*)=r4, `dest`=optimizado fuera. Sin bloques
léxicos. El árbol: **GetKeyData ×5 con rangos EXACTOS 52/12/36/12/56 B**
(los que r69 midió), GetKeySize ×5 (0 B), GetNumKeys ×3, GetCompoundChannel,
InitSubChannels{GetNumChannels, NewBlock, NewFnAnim} — todos con this=r30.
Ojo: hay MUCHOS `EvalState` abstractos por CU (inline); el bueno es el de
zEagl4Anim con low_pc 0x800A3090.

## 5. epCalculateLocalDirectionalPOS16 — los double con nombre, con tipos exactos

`fn epCalculateLocalDirectionalPOS16` (2072 B, zEcstasy.cpp). **TODO el
bloque de iluminación es double**, y sus casas:

- Cascada de rangos: `dmin_rg`[r1+16], `dmin_ba`=**f18**, `drng_rg`[r1+24],
  `drng_ba`=**f19**, `smin_rg`[r1+32], `smin_b`[r1+40], `srng_rg`[r1+48],
  `srng_b`[r1+56], `pow_scale`[r1+64], `exy`[r1+80], `ez`[r1+88].
- Luces: `ld01x/ld01y/ld01z` = **f20/f21/f22** (double), `ld2x`/`ld2z` =
  **f14/f15**, `ld2y`[r1+96]; colores `lc0rg/lc1rg/lc2rg` = **f27/f23/f24**,
  `lc0b/lc1b/lc2b` = **f28/f25/f26**; `zero` (fn) = **f29**.
- Bucle interior: `ndotl01` double (sin casa), **`ndotl2` double (sin casa:
  CSE'd en el final; el f3 del PS2-alpha ya no existe)**, `dottmp`,
  `rdotl0`, `rxy/rz`, `dsrg/dsba`, `ssrg/ssb`, `pxy/pz`, `nxy/nz`,
  `vxy/vz`, `vl`, `tvdotn`, `pow_se` — todos double optimizados fuera;
  **con casa: `dcrg`=f3, `dcba`=f2, `scrg`=f11, `scba`=f4 (double)**,
  `specular_a`=float [r1+124], `envvdotn`=float [r1+116].
- El inline `my_fpow3` (double): `e3`=f9, `e2`=f2, `e1`=f3, `l3`=f8,
  `l2`=f4, **`l1`=f7**, `one`=f1, **`zero`=f31**. Y `my_fpow`/`fClamp` con
  `clamp_max`=f17, `clamp_min`=f16.

Conclusión para la veda: r69 §3 decía "el original usa DOUBLE: ndotl2,
dcrg//f3, dcba//f2, scba//f4, l1//f7, zero//f31" — **confirmado al 100 %**
(f3/f2/f4/l1/f7/zero-f31 idénticos; ndotl2 existe, es double, pero en el
build final no tiene casa). El eje float-vs-double de nuestra
reconstrucción sigue siendo LA diferencia estructural.

## 6. MsgBarrier y eProject

- **MsgBarrier__18CARSFX_PreColWoosh** (140 B, zEAXSound2.cpp): this=r10,
  message=r4, DOS inlines (GetPlayerNum, GetStateBase, ambos 0 B) y **CERO
  locales** — consistente con r67/r69; `interpolationTime` (nombre que
  inventamos en r65) no existe, como decía el filtro de r70b.
- **eProject** (268 B, zEcstasy.cpp): x=f1, y=f2, z=f3, mtx=r3,
  pm=r31, vp=r30, sx=r29, sy=r28, sz=r27 (los cinco últimos float*/array);
  `local`/`eye` Vec en [r1+8]/[r1+24];
  **`clipX`=f13, `clipZ`=f10, `halfVP2`=f0, `halfVP3`=f13**, `oneOverW` y
  `clipY` optimizados fuera; inline `eRecip` con `recip`=f9. La "conmutación
  f0/f13" de r69 tiene nombres: halfVP2 vive en **f0** y halfVP3 comparte
  **f13** con clipX (ventanas de vida disjuntas).

## 7. Qué queda abierto

- R3/TrackCar: la respuesta "ocupación de f0/f13" era falsa; queda palanca
  de asignador puro sobre los tres pseudos de las lfsx de bTan (r61 ya
  probó permutación de fuentes = negativo). No hay forma de fuente
  conocida; documentado y parked como el resto de near-miss de asignador.
- La resolución de tipos de arrays (`mtx` en eProject) es mejorable en
  dwarf1.py; el source-file de cada inline (cabecera de origen) requeriría
  fusionar con `.debug_sfnames`/`.debug_srcinfo` (futuro).
- Cache del walk en `%TEMP%/nfsmw_dwarf1_cache.pkl` (16 s la primera vez,
  instantáneo después).
