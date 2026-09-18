# Ronda 26 — ventana: lo libre

Los ocho encargos cubren **89.556 B en 32 unidades**. Libre quedan **12.864 B**:
zFe2 (2.048), zSpeech (2.220), zPlatform (1.760), zFeOverlay (1.248), zFe (800),
zTrack (708) y `LibSN/steering` (4.080, que es mwcc y lleva dos rondas).

**Cero bytes cerrados.** Lo que traigo son cuatro diagnósticos cerrados, **cinco
vedas con cifra**, y una regla que ya va por su **cuarta confirmación** y conviene
subir de categoría.

## 0. CORRECCION: el premio que perseguia NO EXISTIA

**Lo escrito abajo esta MAL, y lo corrigio el agente de `linked` de esta misma
ronda con el barrido de los 577 `STT_FILE`: `zFEng`/`zFe2` NO es un racimo.** Es
la frontera de `.data` **396 B por debajo** de donde toca: `unlockType` sale de
`UnlockSystem.cpp` (zFe2) y `FEDirection_Message` de `FEButtonMap.cpp` (zFEng).
Con esa linea de `splits.txt` arreglada, `zFEng` deja de dar `L0039`; **sin
ella no puede promocionar NUNCA, ni con `zFe2` cerrada al 100 %**.

O sea que toda la ventana persiguio una premisa falsa. La leccion, que es la de
siempre: **un `L0039` no prueba un racimo, prueba que un simbolo esta atribuido
mal**, y eso lo dice el ELF original, no la intuicion. Lo que sigue queda como
diagnostico de zFe2 por su propio valor (2.048 B), no por el racimo.

## 0-bis. Lo que crei que era el premio

**Cerrar `zFe2` desbloquea el racimo con `zFEng`**: la r25 dejó `zFEng` al 100 %
pero no promociona (`L0039`, `unlockType.38610_8041CE90` lo referencia `zFe2`
desde el objeto extraído). Juntas son **~322 kB de `linked`**, el mayor premio
suelto del árbol.

Pero zFe2 son **tres** funciones y **las tres tienen que cerrar**:

```
1572 B  93,32316 %  FEngFont::RenderString      cascada de CUATRO registros
 384 B  95,47916 %  IconScroller::IconScroller  regmap: MISMO REPARTO
  92 B  91,08696 %  FEKeyboard::ToggleCapsLock  2 filas
```

`RenderString` es una cascada de asignador, no una diferencia de fuente. **Está
medida al pseudo** (§2). No cae en una ventana; hay que encargarla.

## 1. LA REGLA QUE YA VA POR LA CUARTA CONFIRMACIÓN

**Disolver la local que el DWARF del original no tiene EMPEORA.** Cuatro casos
medidos, ninguno a favor:

| función | quitando la local |
|---|---|
| `sfir::calcFIRCoeffs` (`halfTmpFloat`) | 89,252 → **87,060 %** |
| `zPlatform::ActualReadJoystickData` (`v`) | 97,355 → **96,499 %** |
| `Convert32To16` (`result`) | 92,674 → **84,302 %** |
| `zGameplay::GTrigger` (`triggerFlags`, r25) | 99,58 → **97,97 %** |

**La lectura correcta es la contraria de la que parece:** la local de más es un
**síntoma** de que nuestra forma de la expresión difiere, no la causa. El
diagnóstico sirve para saber **dónde** mirar; la corrección no es borrarla.

Y hay que separarlo del caso que SÍ paga —`FindConditionBranchTarget`, 65 → 100 %
en la r25—, donde lo que sobraba no era una local suelta sino **la estructura
entera**: dos locales, una etiqueta y un bloque anónimo, y la reescritura fue de
la función completa contra el volcado.

## 2. `zFe2::FEngFont::RenderString` — 1.572 B, medida al pseudo

El DWARF es **casi idéntico**: mismo árbol de bloques, mismas locales, mismos
inlines. Sólo cambian cuatro registros, y es **una cascada con una sola causa**:

```
              ORIGINAL     NUESTRO
  pcString      r26          r27
  matrix        r27          r26
  c             r27          r28      <- el original COMPARTE r27 con matrix
  pGlyph        r28          r29
```

**El original usa un registro salvado MENOS que nosotros**: `matrix` se usa
**una sola vez** (`cached->SetTransform(matrix)`, línea 362) y muere ahí; `c`
nace en la línea siguiente, así que no entran en conflicto y el original los mete
en el mismo r27. Nosotros gastamos r26 en `matrix` y empujamos todo hacia arriba.

`scripts/alloc.py` da la causa al dígito (183 pseudos en `.lreg`, **107 los
asigna `local-alloc` y sólo 80 llegan a `global-alloc`** — justo lo que `lreg.py`
no distinguía):

```
  #    pseudo  n_refs  live_len  pri     reg
  37   133     28      233       4806    r28   <- c
  38   84      34      357       4761    r27   <- pcString
  60   86      2       46        434     r26   <- matrix
```

**`c` le gana a `pcString` por 45 puntos** y se lleva r28 primero; `pcString` cae
en r27, y `matrix` recoge r26 en la posición **60 de 80**. Los umbrales, para
quien lo retome:

- **`pcString` con una referencia más** (35 refs): `floor_log2(35)=5`,
  `5·35/357 = 0,4901` → **4901 > 4806**.
- **vida de `pcString` ≤ 353** → 4816 > 4806 (cuatro ranuras).
- **vida de `c` ≥ 236** → 4745 < 4761 (tres ranuras).

Corrección de fidelidad aplicada y **medida a coste cero**: `ulJustification` es
`unsigned int` en el original y nuestro `u32` sale como `unsigned long` en el
volcado.

## 3. `zPlatform::UnlockPalette` — 172 B, y la fusión de `combine`

Dos diferencias reales, y la segunda tiene nombre:

1. El `clrlwi` de `r = entry & 0xFF` va **antes** del `cmplwi` en el nuestro y
   **después** en el objetivo (una ranura de planificador).
2. **El objetivo reutiliza `a` ya materializado** —`rlwinm r11, r0, 7, 17, 19`—
   y nosotros dejamos que **`combine` funda `entry>>24>>5<<12` en un solo
   `rlwinm` desde `entry`** (shift 15), lo que **mantiene `entry` vivo** en la
   rama del `else` cuando en el objetivo ya está muerto.

Es la regla del `fold` en su forma habitual: **si el objetivo no funde una
redundancia que nosotros sí fundimos, el original tenía algo que impedía la
sustitución**. `a` es un `set` único desde `entry`, y por eso `combine` puede
sustituirlo.

**Vedas medidas**: quitar `result` y usar dos `return` → **84,30232 %**; sólo
reordenar las declaraciones a `r,g,b,a` → **92,32558 %**. O sea que **nuestro
orden de declaración YA es el del original** y el volcado lista al revés.

## 4. `zFe2::FEKeyboard::ToggleCapsLock` — 92 B, dos filas

El objetivo materializa el 0 de `mbShift = false` en **r10** y guarda **después**
del `cmpwi r9,3` (entre la comparación y el salto); nosotros lo ponemos en **r0**
y guardamos antes. Una ranura y un registro.

**Veda**: poner `mbShift = false;` delante de `mbCaps = mbCaps != 1;` →
**80,65218 %** (base 91,08696). El orden actual es el correcto.

## 5. Lo que queda apuntado y sin tocar

- **`zFeOverlay::CustomizeMain::NotificationMessage`** (808 B): el original
  expande `IsInBackRoom()` **dos veces inline** donde nosotros tenemos una
  referencia local `mgr` (r28), y le falta un bloque anónimo. Es estructura, y de
  las de verdad — pero léase antes la §1.
- **`zSpeech::EAXDispatch::BreakAway`** (252 B): una sola diferencia, `dir` r0
  contra r10.
- **`zSpeech::PursuitEscalation`** (552 B): `regmap` da **MISMO REPARTO** con 8
  locales; son temporales del compilador.
- **`zTrack::GetLoadingPriority`** (708 B, toda la unidad): árbol de inlines
  idéntico; **nuestro marco es 16 B menor** porque el original materializa
  `angle_factor` (f0) y `priority` (r0), que nosotros plegamos.
- **`LibSN/steering`** (4.080 B): es **mwcc**, no GCC, y lleva las rondas 22 y 23.
  No lo he tocado.

## 6. Herramienta

**`scripts/alloc.py` funciona y sustituye a `lreg.py`.** En `RenderString` la
diferencia es enorme: `lreg.py` habría listado **183 pseudos** cuando a
`global_alloc` sólo llegan **80**. Se invoca con la base del volcado RTL
(`build/GOWE69/lreg/<unidad_con_guiones_bajos>.i`) y la **firma exacta**; para
generarla basta con lanzar `lreg.py` una vez sobre la unidad, que deja el
`.lreg`/`.greg` como efecto secundario.

---

# 7. EL CENSO DE ENSAMBLADOR A MANO — y la CORRECCION de mi propia cifra

**Primero dije 2.820 B. Son 656.** La cifra que di estaba mal en casi todo, y la
prueba que la desmonta cuesta segundos: **contar las lineas de DWARF del
original en el rango de la funcion**. Si el original hubiera sido C, tendria
decenas; si tiene dos, es que **el original tambien llevaba ensamblador ahi**.

Es la misma prueba con la que ya se justifico `metrotrk.s` («no hay ni una linea
de DWARF en su rango»), pero **nadie la habia usado como test general**, y yo
tampoco antes de dar el numero.

| unidad · funcion | B | lineas DWARF | veredicto |
|---|---|---|---|
| `criticalpath::IDct64_GC` | 1.740 | **2** (`gcidct.inl` 100 y 101) | el original llevaba `asm`: LEGITIMO |
| `criticalpath` · otras seis | 716 | 2 a 4 cada una | idem: LEGITIMO |
| `metrotrk` | 128 | — | **el `.c` es CODIGO MUERTO**: el `.o` sale de `metrotrk.s`, declarado en `configure.py:1378` con `source=`. Cero |
| **`avplayer::GetFirstFrame`** | **656** | **68** | **era C. Credito falso.** |

Los dos errores que cometi, para no repetirlos: **atribui los bloques `asm` a la
funcion equivocada** (una heuristica de «ultima definicion vista» los colgo de
`VP6_ReconstructBlock` cuando estaban en `ScalarReconIntra_GC` y compania), y
**no comprobe si el original tenia asm ahi**. Contar llaves para delimitar la
funcion y cruzar con `debug_lines.txt` da la respuesta buena.

**Retirado: `avplayer::GetFirstFrame`.** Tenia
`asm("mr %0,%1" … : "r4")` con clobber y un pin `asm("r29")`, sustituidos por
`int video_latency = VideoLatencyInMs;`. **100 % → 98,78049 %**, o sea **−656 B
de `matched`**, que es exactamente el punto: esos bytes no estaban decompilados.
El DOL no se toca — `avplayer` es `NonMatching` en `configure.py`, asi que el
enlace usa el objeto extraido.

**La regla que queda, y vale para todo el arbol:** antes de llamar falsificacion
a un `asm`, mira cuantas lineas de DWARF tiene el original en ese rango. Con
`asm` en el original salen 2-4 en cientos de bytes; con C salen decenas.

## 8. La séptima falsificación, esa sí retirada (§ commit aparte)

`CustomizeMain::NotificationMessage` tenía
`asm("" : : "r"(engine), "r"(pkg) : "r4")` con dos locales que el DWARF no tiene,
forzando el orden de evaluación de los argumentos. **Sostenía 0,99 pp y CERO
bytes** —la función no llega al 100 % en ninguna de las dos formas—, así que
retirarla es gratis en lo único que cuenta. Medido aparte: los dos locales
cuestan cero, todo el porcentaje lo ponía el `asm`.

Con eso y con llamar al **miembro** `gCarCustomizeManager.IsInBackRoom()` en vez
de a la función libre —el original lo hace, y por eso su DWARF tiene tres
expansiones que nos faltaban; coste cero—, el diff de DWARF de esa función pasa
de cinco bloques a **una sola línea**: la referencia `mgr`, cuya eliminación
cuesta 1,29 pp y es la **quinta** confirmación de la regla de la §1.

---

# 9. La regla, afilada: SEIS contraejemplos y uno solo a favor

`zSpeech::Manager::LoadSpeechBank` (316 B) añade el sexto, y es el que cambia la
formulación porque **no es una local: es una ETIQUETA**. El DWARF dice que el
original **no tiene la etiqueta `found:`** que nosotros usamos con cuatro `goto`.
Reescrita la búsqueda binaria en forma estructurada equivalente (`break` más la
comprobación posterior, que ya estaba): **95,31645 → 85,50633 %**.

| construcción nuestra que el DWARF no tiene | al quitarla |
|---|---|
| `sfir::calcFIRCoeffs` · local `halfTmpFloat` | 89,252 → 87,060 |
| `ActualReadJoystickData` · local `v` | 97,355 → 96,499 |
| `Convert32To16` · local `result` | 92,674 → 84,302 |
| `zGameplay::GTrigger` · local `triggerFlags` (r25) | 99,58 → 97,97 |
| `NotificationMessage` · referencia `mgr` | 98,886 → 97,599 |
| **`LoadSpeechBank` · etiqueta `found` + 4 `goto`** | **95,316 → 85,506** |

**Y el único caso a favor —`FindConditionBranchTarget`, 65 → 100 %— quitó las
tres cosas A LA VEZ**: dos locales, la etiqueta y el bloque anónimo, reescribiendo
la función entera contra el volcado.

**La formulación correcta, entonces, no es «quita lo que sobra» sino: mientras
quede UNA diferencia de estructura, quitar las otras empeora.** El diff de DWARF
sirve para saber si la función está a una reescritura completa o no lo está; usar
sus líneas de una en una es lo que falla seis de seis.

Corolario que lo confirma desde el otro lado, y es de esta misma ronda: el agente
de middleware cerró `srender` (188 B, 0 → 100 % de unidad) **añadiendo** un
`continue` y **conservando** un `__asm__("")` declarado como deuda. La barrera
hace falta: sin ella la recarga se iza, el bloque queda vacío y `jump.c` lo funde
invirtiendo la rama (184 B contra 188).
