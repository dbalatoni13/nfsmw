# r69: ICEMover::Update al 100% — la veda de 1.606 compilaciones cae por DWARF+RTL

+3.868 B en zCamera (93,23 -> 96,32 %, 451/453 funciones al 100 %).
`audit.py` ok (86 ramas, 261 relocs, 94 literales), `frozen.py` congela `80e35a0d7cc0596b`.

## 1. El método que la cerró (reproducible, aplicarlo al resto de vedas)

Tres piezas de evidencia primaria, ninguna usada antes contra esta función juntas:

1. **DWARF del original** (`dwbody.py`): el listado de locales del bloque shake dice
   `int frame; // r8` — el original mantiene `frame` en UN registro del `lwz` al
   `mulli`. Nosotros no teníamos localización (pseudo partido). Y `SignedMod`
   inline solo lista `int c; // r0` — sin temporal de usuario.
2. **Atribución de líneas** (`lmap.py`): la copia `mr r11, r8` lleva línea de
   SENTENCIA (ICEMath.hpp:136), no la de apertura de la función (que es lo que
   lleva una copia de binding: la nuestra decía 150). El original tenía una
   sentencia-copia... o no la tenía: ver 3.
3. **RTL nuestro** (`-dl -dg` vía `lreg.py`): `pseudo9 := lwz` (resultado de
   FloatToInt) y `a(pseudo11) := pseudo9` (binding del argumento inline). El
   objetivo no tiene esa copia: el retorno del inline cae DIRECTO en el pseudo
   del llamador.

## 2. La solución, con las cifras

Dos cambios SINÉRGICOS (ninguno vale solo — r48 midió el split con el
SignedMod viejo y dio objeto idéntico):

**ICEMath.hpp::SignedMod** — returns en vez de asignación:

```cpp
inline int SignedMod(int a, int b) {
    if (b > 0) {
        while (a < 0) {
            a += b;
        }
        int c = a / b;
        c = c * b;
        return a - c;      // antes: a -= c; ... return a;
    } else {
        return 0;          // antes: a = 0;
    }
}
```

**ICEMover.cpp** — split del llamador:

```cpp
int frame = ICE::FloatToInt(fParam * length * current_sec);
frame = ICE::SignedMod(frame, pShake->GetNumKeys());
// antes: SignedMod(FloatToInt(...), ...) anidado
```

Progresión medida (filas divergentes del diff de instrucciones):

| variante | filas |
|---|---:|
| base (r48-r63, 1.606 compilaciones) | 15 |
| V2 `int t = a` + while(t) | 7 |
| **V11 returns + split** | **0** |
| V11 solo (sin split) | 19 |
| split solo (r48, SignedMod viejo) | 15 (identico) |

Mecánica: con returns, `a` es de solo lectura tras el binding y GCC NO copia el
argumento — el `lwz` carga directo en el pseudo de `frame`, y el `subf` del
return escribe ese mismo pseudo (r8 del objetivo). El split del llamador es lo
que hace que el argumento sea una variable ligable sin copia.

## 3. Barrido DWARF del resto de vedas — hallazgos NUEVOS (sin medir aún)

`dwbody.py` sobre todas las funciones abiertas destapa diferencias
estructurales que nadie había visto:

| función | hallazgo |
|---|---|
| `TrackCarCameraMover::Update` (992 B) | al original le sobran 2×`bTan` (entre `Look=` y `displacement/=`, rango CERO) + 3×`GetGeometryPosition` + 1×`GetVelocity` (entre el ctor de `m` y SetTargetDistance). Codigo muerto plegado — sin impacto en codegen, pero arregla el DWARF si algun dia importa. |
| `EvalState` (zEagl4Anim, 98,07 %) | faltan 5×`GetKeyData(int)` + 2×`GetKeySize()` inline — sentencias ausentes |
| `DynamicLoader::Initialize` (99,64 %) | faltan locales `sheader`, `e`, `p` y N×`unsigned int ul` — estructura de bucle distinta |
| `epCalculateLocalDirectionalPOS16` (93,3 %) | el original usa DOUBLE: `ndotl2`, `dcrg//f3`, `dcba//f2`, `scba//f4`, `l1//f7`, `zero//f31`. Nosotros reconstruimos con float — es una reconstrucción de la cascada aritmética entera |
| `eProject` (94,0 %) | solo registros (f0/f13 conmutados); 200+ variantes ya medidas (r30/r36d/r36f/r65/r67) |
| `RenderFlaresOnCar` (98,05 %, 480 filas) | `flashHeadlights` en r14 (nosotros r16) + ctor `ProfileNode(const char*, int)` con argumentos (nosotros default) + `address` con registro |

## 4. Micro-vedas verificadas hoy (sin cambios)

- **HolePunchAvoidables** (4 filas): H1 = mover el ternario de `is_drag`
  delante del `+= bCross` -> **162 filas** (el beq se mueve en el asm y arrastra
  8 B de offsets). El objetivo numérico de r68 sigue en pie: hace falta
  live(is_drag) en [484,491] o live(is_racer) en [495,509]; todo movimiento de
  sentencia cae en la veda de reordenar llamadas (r36b).
- **SetMemoryPoolSize** (2 filas): confirmada veda r68/r68b — la inversión
  addi/stw vive en las ENTRADAS de sched2 (post-reload).
- **UpdatePlatInfo** (3 filas): conmutar `DiffuseMinB * scale` no controla el
  orden de operandos del `fmuls` (GCC canoniza) y rompe la fila 62 -> 4 filas.
- **MsgBarrier** (zEAXSound2): IRREDUCIBLE r67 (empate INSN_LUID en carga de
  parametro en registro duro) — no reintentar sin palanca nueva.

## 5. En vuelo

Tres agentes sobre andamios (ramas agent/scaf-fe-r1, agent/scaf-rcore-r1,
agent/scaf-audio-r1): familias zFe/zFeOverlay/zFe2, realcore/filesys/path, y
audio/LibSN/vp6/STICH. Protocolo: quitar -> medir -> forma de fuente (método
de §1) -> veda documentada si no cierra. Sin audit/frozen (los corre el jefe).

## 6. EvalState (zEagl4Anim): forma confirmada, queda el reparto

Medido tras el hallazgo DWARF (5x GetKeyData con rangos IDENTICOS 52/12/36/12/56 B):

- accesor SIN multiplicacion interna + productos externos: **18 filas** (igual que base)
- accesor con returns directos por rama (sin local `keyData`): **90 filas** — el
  `keyIdx * GetKeySize()` se evalua EN CADA RAMA y duplica codigo. REVERTIDO.
- el lmap del original resuelve la atribucion: el multiply de la condicion (cpp:126)
  es EXTERNO y el del bucle (h:161/162 + cpp:130) INTERNO — exactamente la forma
  que ya teniamos. La local `keyData` del accesor tambien es la forma buena
  (una sola evaluacion del producto).

Lo que queda en EvalState es SOLO la rotacion de registros del primer bloque
({r3,r6,r7,r9,r10,r11} contra {r12,r3,r6,r11,r8}) — asignador, mismo modo que
los otros near-miss. El arbol DWARF anidado-vs-hermano del GetKeySize es
emision del compilador, no forma de fuente.
