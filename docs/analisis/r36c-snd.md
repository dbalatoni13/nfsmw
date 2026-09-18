# Ronda 36c — `c36c-snd`: zEagl4Anim, zEAXSound, zEAXSound2, zSpeech

**Encargo verificado.** `triaje.py` sobre las cuatro unidades reproduce el reparto
del brief al byte: **17 funciones, 11.736 B**, y mis cinco salen con el mismo
tamaño, el mismo porcentaje y el mismo veredicto que el encargo.

**Resultado: ninguna función cerrada, nada aplicado.** `git status` del territorio
está **vacío**; `triaje` después es idéntico al de antes (17 funciones, 11.736 B) y
`lcfix.py --check` dice «todas las entradas @lc están al día». **Sin commits.**

Traigo **catorce medidas nuevas**, **dos correcciones al diagnóstico del encargo**,
**un frente probado imposible con 40 formas** y **tres hallazgos de método sobre las
palancas nuevas** que valen para todo el proyecto.

---

## 0. Resumen por función

| función | B | base | mejor ensayo | queda |
|---|---|---|---|---|
| `DynamicLoader::Initialize` | 2352 | 98,876 % (28 filas) | **A3: 25 filas** (bloque de registros del `strtab` CERRADO) | árbol del `switch` (15) + orden de operandos del `PLUS` (6) |
| `FnStatelessQ::EvalSQT` | 1480 | 99,459 % | **no tocada** (veda de la r21 + brief-r27) | — |
| `CARSFX_RoadNoise::GenerateRoadNoise` | 1240 | 97,113 % (23) | E4: 26 filas pero **cierra entera la cadena `frsqrte`** | motion interbloque del 2.º `fmadds` |
| `EAXAemsManager::SetupNextLoad` | 1008 | 98,671 % (23) | ninguno (3 pines negativos) | `this` es PARÁMETRO: no se pincha |
| `SFX_Common::MsgPlayMiscSound` | 780 | 99,405 % (28) | X1: **sitio 1 byte a byte**, 30 filas en total | sitios 2 y 3 son INVARIANTES al orden |

---

## 1. Dos correcciones al diagnóstico del encargo

### 1.1 El `otro=13` de `Initialize` NO son bloques desajustados

El encargo mandaba mirar si son «desajustes de bloque» y aplicar la **etiqueta de
tipo** (`enum { _tag };`). **No lo son.** Las trece filas `REPLACE`/`OP_MISMATCH`
son las **comparaciones del árbol de decisión del `switch (sheader->sh_type)`**
(filas 348-362): el opcode difiere porque el objetivo compara `r21`, `r22`, `9`,
`3`, `3`, `1` y nosotros `3`, `3`, `2`, `r21`, `r22`, `9`. `regmap` no señala ni un
bloque de más ni de menos. **La etiqueta de tipo no pinta nada aquí.**

### 1.2 El `regs=28` de `MsgPlayMiscSound` no es todo registros

Ocho de las 28 filas son `ARG_MISMATCH` sobre el **desplazamiento**, no sobre el
registro: son los seis `stw rX, off(rY)` del constructor **en otro orden**
(objetivo `0x18,0x4,0x8,0xc,0x10,0x14`; nuestro `0x18,0x8,0x4,0xc,0x10,0x14` en el
sitio 1). `triaje` los clasifica como `regs` porque objdiff mira el operando, no su
papel. **Antes de mandar una función al permutador por `regs=` alto, hay que mirar
si los operandos que cambian son registros o desplazamientos.**

---

## 2. `DynamicLoader::Initialize`: el pin CIERRA el bloque, el `switch` bloquea

### 2.1 Lo que consigue el pin (ensayo A3, reproducible)

`regmap` daba dos parejas acopladas (`t`↔`nameLength` y `type_separator`↔`&s[2]`).
La r36b probó que el ensayo A (`&s[nameLength] + 1`) arregla la de abajo y rompe la
de arriba. **Con el pin se arreglan las dos a la vez**, y el bloque entero del
`case SHT_STRTAB` (filas 409-428, **7 filas**) pasa a casar exacto:

```c
// eagl4supportdlopen.cpp, case SHT_STRTAB
register char *type_separator asm("r30") = strstr(&s[2], TYPE_SEPARATOR);
char typebuf[128];
register char *t asm("r29");
...
strcpy(&s[nameLength] + 1, typebuf);      // el ensayo A de la r36b
```

| # | qué | % | filas |
|---|---|---|---|
| base | — | **98,87585** | 28 |
| A1 | `&s[nameLength] + 1` (reproduce la r36b exacto) | 98,70238 | 30 |
| A2 | A1 + `type_separator asm("r30")` | 98,69388 | 31 |
| **A3** | **A2 + `t asm("r29")`** | 98,744896 | **25** |
| A4 | A3 + `asm("" : "+r"(type_separator))` dentro del `if` | 98,41327 | 26 |

**A3 baja de 28 a 25 filas pero baja el fuzzy** (98,876 → 98,745): las filas que
quedan son `DELETE`/`INSERT` (crédito cero) donde antes había `ARG_MISMATCH`
(crédito parcial). Es exactamente el engaño que avisa el brief. **Lo dejo
REVERTIDO** porque la función no puede cerrar (§2.2) y `matched_code` es
todo-o-nada: 25 filas valen lo mismo que 28, cero bytes, y el fuzzy sí se ve.

Las dos filas que A3 no cierra: `stb r23, 0x0(r30)` sale como `stb r23, 0x0(r3)`
(el pin deja el valor vivo en `r3` y GCC lo usa ahí) y el `add r29, r28, r30`
adelantado por encima del `stbx`.

### 2.2 El árbol del `switch` es INALCANZABLE desde la fuente (40 formas)

Monté una **mini-TU** con el `switch` suelto (`ngccc -S`, **un segundo por
variante**) y barrí **más de 40 formas**. Ninguna produce el árbol del objetivo.
La mini-TU **reproduce nuestro árbol exacto a la primera**, así que el barrido es
concluyente.

Formas barridas: `case 8` plano; `break`↔`continue` en los cuatro grupos vacíos
(las 16 combinaciones); `default:` delante, en medio, detrás y ausente; los rangos
delante de los cuerpos; partir `4...7`; fundir `4...8`; quitar cada uno de los
cuatro grupos; fundir `0x70000005...0xFFFFFFFF`; añadir `case 0`, `case 1`,
`case 0...1`, `case 10`, `case 10...11`, `case 10...0x70000004`, `case 12`,
`case 10...15`, y ocho combinaciones de uno bajo + uno alto.

**El mecanismo, que es lo que hay que anotar:**

- Nuestro árbol sale de la rama «**neither bounded**» de `emit_case_nodes`:
  `bgt test_label` → subárbol IZQUIERDO en línea → `b default` → `test_label:` →
  subárbol DERECHO. Sale **siempre** que la raíz tiene dos hijos y ninguno está
  acotado, y en este `switch` **ninguno puede estarlo**: el hijo izquierdo tiene
  hijo derecho y el derecho tiene hijo derecho, y `node_has_high_bound` devuelve 0
  en cuanto `node->right != 0`.
- El objetivo hace lo contrario (`ble` al izquierdo, **derecho en línea**) y su
  hoja `case 2` no lleva `beq`: emite `cmplwi r0,1 ; ble default` y **cae** al
  cuerpo. Eso sólo lo produce la rama de **hoja NO single-valued**
  (`if (!low_bound) emit LT node->low → default; emit_jump(node->code_label)`, y
  el `emit_jump` se borra por adyacente). Para eso el nodo tendría que ser un
  **rango `{2..7}`**, y `{3}` no puede ser antepasado suyo a la vez. **Contradicción
  cerrada**: no hay conjunto de `case` que lo genere.
- Y el `cmplwi r0, 8 ; ble` del objetivo es un `LTU 9` canonicalizado (GCC
  canonicaliza `LTU C` → `LEU C-1`; se ve en nuestro propio `cmplwi 7 ; bgt` para
  `GEU 8`). Ninguna rama de `emit_case_nodes` compara contra `node->high + 1`.

**Veda medida y explicada.** Confirma las vedas 4 y 5 de `r20-muros.md` y la nota
«en la última hoja el original emite `bne default; b case` y nosotros
`beq case; b default`» de `nfsmw-arbol-de-switch`.

Con eso, `Initialize` tiene **21 filas bloqueadas** (15 del `switch` + 6 del orden
de operandos del `PLUS`, veda 10 de la r36b) de 28. **No cierra esta ronda ni la
que viene.**

---

## 3. `MsgPlayMiscSound`: el pin no llega, y el orden es INVARIANTE

### 3.1 Por qué el pin no llega (y sale gratis descubrirlo)

El encargo apuntaba el pin al `regs=28`. Los registros que bailan son `r27`/`r28`,
y **`r27` del objetivo es el cero de larga vida** que sirve a la vez al `stw
r27, 0x8(r31)` (`volume = 0`) y al `stw r27, 0x8(r1)` (`int refCount = 0;` de
`FX_UVES::GetRefCount`).

Pinché `refCount`:

```c
register int refCount asm("r27") = 0;     // FX_UVES::GetRefCount, MAIN_AEMS.h:1611
```

**Resultado: el binario sale byte a byte IDÉNTICO** (99,40513 %, 28 filas, y
`SFX_Common::ProcessUpdate` sigue al 100 %). El pin se **ignora en silencio**
porque a `refCount` se le toma la dirección (`GetRefCount(&refCount)`): es una
local de memoria y GCC no puede meterla en un registro.

> **Regla nueva, gratis:** antes de gastar una compilación en un pin, `grep` la
> variable buscando `&`. Un `register … asm()` sobre una local con la dirección
> tomada no da error, no da aviso y **no cambia un byte**.

### 3.2 El orden de los `Set` no puede arreglar los tres sitios (medido, no deducido)

Reconstruí la regla de emisión con dos permutaciones del constructor y la mini-TU
del punto anterior:

**En el sitio 1** (`FX_UVES(0,0,0,0,0,0)`, los seis valores en el mismo registro)
la emisión es el **orden de la fuente con el ÚLTIMO `Set` rotado al frente**:

| orden en la fuente | emisión en el sitio 1 |
|---|---|
| `v,i,w,p,n,s` (base) | `s,v,i,w,p,n` |
| `s,i,v,w,p,n` (D2) | `n,s,i,v,w,p` |
| **`i,v,w,p,n,s` (X1)** | **`s,i,v,w,p,n` = OBJETIVO, byte a byte** |

**X1 deja el sitio 1 exacto** (filas 31-39 sin una sola diferencia). Es
`SetID` antes de `SetVolume`, o sea deshacer el commit `5d1ac0cf`.

**En los sitios 2 y 3** (`iD` vale 2 y 1) la emisión **no depende del orden**:

| orden en la fuente | emisión en el sitio 2 |
|---|---|
| `v,i,w,p,n,s` (base) | `v,w,p,n,i,s` |
| **`i,v,w,p,n,s` (X1)** | **`v,w,p,n,i,s` — IDÉNTICA** |
| `s,i,v,w,p,n` (D2) | `s,v,w,p,n,i` |

Base y X1 dan **exactamente la misma emisión** en los sitios 2/3 aunque el orden
de la fuente cambie. El objetivo quiere `v,i,s,w,p,n`, que **ninguna de las tres
formas produce**. Totales: base 28 filas (99,40513 %), X1 30 filas (97,26154 %),
D2 44 filas y **8 B menos** (94,94359 %).

**Lo que de verdad separa a los sitios 2/3 es el REPARTO DEL CERO**, no el orden:

    objetivo: {volume} <- r27 (cero de larga vida) ; {stop,width,pitch,intensity} <- r0
    nuestro : {volume,width,pitch,intensity} <- r28 ; {stop} <- r9

Los dos crean dos registros con cero; el corte cae detrás del **primer** almacén en
el objetivo y detrás del **cuarto** en el nuestro. Eso lo decide `reload_cse_regs`
después de la asignación, y la fuente del constructor no lo alcanza.

**Veda cerrada**: `MsgPlayMiscSound` no se cierra por el orden del constructor —
ahora con las tres permutaciones medidas y la invarianza de los sitios 2/3
demostrada, no sólo con el X1 de la r36b.

---

## 4. `SetupNextLoad`: los dos pines posibles son negativos

`this` va en r29 en el objetivo y en r27 en el nuestro, pero **`this` es un
parámetro y no se puede pinchar** (lo avisa el brief). Los únicos nombres
pinchables están dentro de `SndAssetQueue::DeleteRefToAsset(Attrib::StringKey)`,
que se expande inline y **tiene un solo llamante en todo el árbol**
(`EAXAemsManager.cpp:465`; el de la línea 952 es la sobrecarga de `SndBase *`), o
sea que pinchar ahí no tiene daño colateral.

| # | qué | % | filas |
|---|---|---|---|
| base | — | **98,67063** | 23 |
| B1 | `register int deleteCount asm("r26") = 0;` | 98,11508 | 26, y **4 B menos** |
| B2 | `register int idx asm("r7");` | 97,75794 | 34 |
| B3 | B1 + B2 | 97,16270 | 37, y **4 B menos** |

**Por qué B1 pierde una instrucción, que es el hallazgo:** sin pin, GCC calcula
`deleteCount + 1` en un pseudo aparte (`addi r29, r31, 1`) y lo mueve al contador
mucho después (`mr r31, r29`) — **igual que el objetivo** (`addi r28, r26, 1` …
`mr r26, r28`). Con el pin, el incremento se **coalesciona** en sitio
(`addi r26, r26, 1`) y desaparece el `mr`. El pin no sólo elige registro: **fuerza
el coalescing de las actualizaciones de la variable pinchada**.

Contando allocnos: el objetivo mete 8 rangos de vida en **6** registros salvados
(r25-r30, r31 sin usar) y nosotros 7 en **7** (r25-r31). Nos sobra un registro
salvado porque `idx` nos vive cruzando una llamada y al objetivo no. Forzarlo a
`r7` (B2) es peor. **Frente cerrado por esta vía.**

---

## 5. `GenerateRoadNoise`: la barrera cierra la cadena `frsqrte` entera

`regmap` sigue diciendo lo mismo (6 locales con el mismo registro, `slipBoost`
sólo nuestra). Lo que falla, ya localizado por la r36b, son los dos `fmadds` de
`fRightVol`: el objetivo los emite en las filas 102 y 114 y nosotros en la 114 y
la **134** — y la 134 está **al otro lado de un `bso`**, o sea en otro bloque
básico. No es un empate de emisión: es **motion interbloque** del planificador
Haifa.

| # | qué | % | filas |
|---|---|---|---|
| base | — | **97,11290** | 23 |
| E1 | los dos `fmadds` juntos, `slipBoost` local, delante del 2.º `bLength` | 96,10968 | 55 |
| E2 | `register float slipBoost asm("fr10") = 0.1f;` | 97,06451 | 25 |
| E3 | `asm("" : "+f"(fRightVol));` tras el **segundo** `fmadds` | 96,98387 | 30 |
| **E4** | **`asm("" : "+f"(fRightVol));` tras el PRIMER `fmadds`** | 96,91936 | **26** |
| E5 | E4 + `register float fRightVol asm("fr29") = 0.0f;` | 96,95161 | 28, y 4 B menos |
| E6 | `asm("" : : "f"(fRightVol));` (sólo entrada) tras el primero | 96,88710 | 28 |
| E7 | E4 + E3 | 96,75806 | 32 |

**E4 es el interesante y es un USO NUEVO de la barrera selectiva.** Adelanta el
primer `fmadds` de la fila 114 a la 102 (la del objetivo) y con eso **casan exactas
las filas 110, 114 y toda la cadena `frsqrte` 118-127** — diez filas que en la base
fallaban. Lo que rompe es el destino: `fRightVol` aterriza en `f30` en vez de
`f29`, porque el `"+f"` crea un pseudo nuevo.

E5 intenta arreglarlo con el pin y **es la lección más cara de la ronda**: el
rango de vida de `fRightVol` empieza en `float fRightVol = 0.0f;`, la **primera
línea** de la función, así que el pin lo repinta todo y rompe **diez filas noventa
filas más arriba** (24-42, donde `f1` pasa a `f29`).

> **Regla nueva:** el radio de acción de un pin es el rango de vida ENTERO de la
> variable. Sobre una local inicializada al principio de la función y usada al
> final, el pin repinta la función entera. Sirve para vidas cortas, como decía el
> brief, y **la vida se mide desde la DECLARACIÓN, no desde el uso que te
> interesa**.

---

## 6. Tres hallazgos de método (valen fuera de estas cuatro unidades)

1. **La mini-TU para árboles de `switch` cuesta un segundo por variante.** Un
   `.cpp` de 25 líneas con el `switch` suelto y `ngccc -S` reproduce el árbol
   exacto de la unidad grande. Barrí 40 formas en dos tandas; con `build_direct`
   habrían sido dos horas. Está en `scripts/` nada, lo monté al vuelo: el molde es
   `for (i…) switch (g[i]) { … }` con `f(k)` de cuerpo, y se compara la secuencia
   `cmpX const / bcond`. **Extensible a cualquier `switch` del proyecto.**
2. **Un pin sobre una local con la dirección tomada se ignora en silencio.** Cero
   error, cero aviso, binario idéntico. `grep '&nombre'` antes de compilar.
3. **La barrera `asm("" : "+f"(x))` tiene un TERCER uso**: además de frenar un
   adelanto de `sched` (r36) e impedir el coalescing (r36b), **ancla una
   instrucción contra el hundimiento a través de una rama** (motion interbloque del
   Haifa). Es lo que hace E4. Coste: crea un pseudo nuevo, o sea que **cambia el
   registro del destino**; sólo cierra si ese registro no importa o si se puede
   pinchar una variable de vida corta.

---

## 7. Vedas que dejo medidas (no las repitáis)

1. `Initialize`: el **árbol del `switch`** no se alcanza desde la fuente —
   **40 formas** barridas en mini-TU (rangos, `break`/`continue`, sitio del
   `default`, cases vacíos altos y bajos, particiones y fusiones). Mecanismo
   explicado en §2.2.
2. `Initialize`: `A2` (`&s[nameLength]+1` + pin a `type_separator`) sin el pin de
   `t`: 98,694 %, 31 filas. **Los dos pines van juntos o no van.**
3. `Initialize`: `asm("" : "+r"(type_separator))` dentro del `if`: 98,413 %.
4. `MsgPlayMiscSound`: pin `refCount asm("r27")` — **byte a byte idéntico**
   (variable con la dirección tomada).
5. `MsgPlayMiscSound`: orden del constructor `s,i,v,w,p,n`: 94,944 % y **8 B
   menos**. Y **los sitios 2/3 dan la misma emisión con el orden base y con X1**:
   el orden de los `Set` no los toca.
6. `SetupNextLoad`: pin `deleteCount asm("r26")` (98,115 %, **−4 B**), pin
   `idx asm("r7")` (97,758 %), y los dos juntos (97,163 %, **−4 B**).
7. `GenerateRoadNoise`: los dos `fmadds` juntos **con la local `slipBoost`** y
   delante del `bLength` izquierdo: 96,110 % / 55 filas. (Completa la veda 1 de la
   r36b, que sólo lo había medido con el literal.)
8. `GenerateRoadNoise`: pin `slipBoost asm("fr10")`: 97,065 %.
9. `GenerateRoadNoise`: barrera tras el **segundo** `fmadds` (96,984 %), barrera de
   sólo entrada (96,887 %), y las dos barreras a la vez (96,758 %).
10. `GenerateRoadNoise`: pin `fRightVol asm("fr29")` sobre E4: rompe diez filas
    noventa filas más arriba.

## 8. Lo que dejo servido para el siguiente

- **`Initialize` sólo puede cerrar si alguien rompe el árbol del `switch`.** El
  resto está resuelto: el bloque del `strtab` con la receta A3 de §2.1 (siete
  filas), y las seis filas del orden de operandos del `PLUS` son la veda 10 de la
  r36b. Si el `switch` cae, A3 se reaplica tal cual.
- **`MsgPlayMiscSound`**: el sitio 1 se cierra **hoy** con `SetID` antes de
  `SetVolume`; lo que falta son los sitios 2 y 3, y no es la fuente del
  constructor sino el corte de `reload_cse_regs` sobre el cero. Si alguien
  encuentra cómo mover ese corte, la función cae con X1 puesto.
- **`GenerateRoadNoise`**: E4 cierra la cadena `frsqrte` entera. Falta (a) que
  `fRightVol` se quede en `f29` con la barrera puesta —hace falta una barrera que
  no cree pseudo, o un pin sobre una variable de vida corta— y (b) subir el
  segundo `fmadds` por encima del `bso`.
- **`SetupNextLoad`**: nos sobra un registro salvado respecto al objetivo (7
  contra 6) porque `idx` nos vive cruzando la llamada a `Free`. Quien consiga que
  `idx` no cruce esa llamada probablemente arrastre `this` a `r29` y con él las 21
  filas.
- **`EvalSQT`**: **no la toquéis.** Frente cerrado desde la r21 (22 formas) y el
  brief-r27; el encargo de la r36b ya lo corrigió y esta ronda lo respeta.
