# r22 — zEagl4Anim + zEAXSound

Agente de la ronda 22. Encargo: las cinco near-miss que dejó la 21.

## 0. Verificación del encargo

`build_direct.py zEagl4Anim zEAXSound` + `triage.py` reprodujeron **exactamente**
las cinco funciones, tamaños y porcentajes del encargo. Las listas de opcodes que
imprime `triage.py` salen **desordenadas y truncadas a 4** (cada pasada las saca
en otro orden: `lwz+7, ble+2, stw+1, beq-1` / `… mr-1, bgt-1` / `… cmplwi+1,
mfctr+1` son la misma firma). Los multiconjuntos reales están más abajo.

`audit.py` al empezar: **zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 762 ok /
0 FALLA**. No hubo ningún FALLA que confirmar con segunda pasada. Repetido al
cerrar: los mismos 313 y 762, **cero FALLA**.

## 1. Resultado

| | antes | después |
|---|---|---|
| `measure.py --cmp` (zEagl4Anim+zEAXSound+zEAXSound2) | 409.516/434.364 B 94,2795 % | **igual: +0 B, +0 fns, 0 unidades cambian** |
| `UpdateRPM__13SFXCTL_Enginef` (716 B) | 96,45252 % · 720 B · 16 diffs | **99,80447 % · 716 B · 4 diffs** |
| `Play__9EAXCommon…` (492 B) | 96,82927 % | 96,82927 % (revertido) |
| `EvalSQT` `FnStatelessQ` (1.480 B) | 99,45946 % | 99,45946 % (revertido) |
| `EvalState` `FnRawStateChan` (456 B) | 93,50877 % | 93,50877 % (revertido) |
| `Initialize` `DynamicLoader` (2.352 B) | 91,33673 % | 91,33673 % (no tocada, por veda) |

**Cero funciones cerradas, cero bytes.** Lo único que se queda en el árbol es una
corrección estructural en `UpdateRPM` (§3) que sube 3,35 pp, deja el **tamaño
exacto del objetivo** y hace que **las clases de registro coincidan con el
objetivo**; ninguna unidad baja y ninguna vecina se mueve (`SFXCTL_Engine.cpp`
tiene 29 funciones y `UpdateRPM` es la única que no está al 100 %, antes y
después).

**Fichero tocado: `src/Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.cpp`, y
sólo el cuerpo de `UpdateRPM`.** Ninguna cabecera. No he tocado `FE_AEMS.h`,
`EAXCar.hpp`, `EAXAemsManager.cpp` ni `SFXCTL_MasterVol.cpp`.

### Aviso: `zSpeech` ya no coincide con su congelado, y NO es mío

`frozen.py chk Speed/Indep/SourceLists/zSpeech` → `*** HA CAMBIADO ***`.
Medido: **con mi fichero revertido a HEAD y `zSpeech` reconstruido, sigue
diciendo `HA CAMBIADO`**. Ninguno de los 17 `.cpp` de `zSpeech.cpp` incluye
`SFXCTL_Engine.cpp`. El congelado es del 4-sep (ronda 21) y desde entonces han
entrado commits. Estado actual: **176.668/178.888 B = 98,759 %, 698 fns al
100 %**. También `frozen.py chk zEAXSound` dice `HA CAMBIADO`, pero ese
congelado es de **hoy 13:56** (no de la ronda 21: lo dejó otro agente) y ahí sí
es mi mejora de `UpdateRPM`.

**No he ejecutado `frozen.py cong` ni una vez**: los diez
`docs/congelado/*.json` que salen modificados en `git status` (zAI, zAnim,
zCamera, zEcstasy, zFEng, zFe2, zPlatform, zSim, zSpeech, zTrack) son de otros
agentes de esta misma ronda. Tampoco he hecho commit.

## 2. Herramienta nueva: el volcado RTL, y cómo sacarlo

Es lo que ha desbloqueado los tres diagnósticos de abajo, y **la sección 4 de
`HERRAMIENTAS.md` no lo tenía**. `ngccc.exe` pasa `-da` a `cc1plus` tal cual,
pero los volcados van al lado del **fichero preprocesado temporal**, que el
driver borra. La receta que funciona:

    # 1) preprocesar a un .ii propio
    ngccc.exe <cflags de la unidad> -E -o SCR/x.ii SCR/stub.cpp
    # 2) llamar a cc1plus a mano (los cflags reales salen de `ngccc -v`)
    cc1plus.exe -O1 -Wno-ctor-dtor-privacy -Woverloaded-virtual -Wno-multichar \
      -mps-nodf -G0 -fno-static-dtors -ffast-math -fforce-addr -fcse-follow-jumps \
      -fcse-skip-blocks -fforce-mem -fgcse -frerun-cse-after-loop -fschedule-insns \
      -fschedule-insns2 -fexpensive-optimizations -frerun-loop-opt -fmove-all-movables \
      -da -quiet SCR/x.ii -o SCR/x.s

Deja `x.ii.rtl`, `.jump`, `.cse`, `.gcse`, `.loop`, `.cse2`, `.flow`, `.combine`,
`.regmove`, `.sched`, `.lreg`, `.greg`, `.flow2`, `.sched2`, `.jump2`, `.mach`.
Con eso se ve **en qué pase exacto** aparece o desaparece cada instrucción, y
`.gcse` además **imprime el recuento de propagaciones** (`CPROP of <función>,
pass 1: … 0 const props, 1 copy props`). El `.ii` se puede parchear a mano
(struct, sentencia) y recompilar en ~2 s sin tocar el árbol.

**Y se puede parchear la fuente y medir en 1-24 s** con el arnés
`c22ae_sw.py` del scratchpad (multi-parche, restaura siempre, `DUMP=<variante>`
vuelca el diff de instrucciones). Para `zEAXSound` los `.cpp` **no compilan
sueltos**: hay que pasarle en `PRE` el prefijo del SourceList hasta el fichero
(scripts `c22ae_pre_eng.txt` / `c22ae_pre_fe.txt`). Verificado que el stub
reproduce el `.o` real al bit en las tres funciones.

## 3. `UpdateRPM__13SFXCTL_Enginef` — 96,45252 → 99,80447 %, 720 → 716 B

### El diagnóstico de la ronda 21 era el pase equivocado

La 21 dejó escrito que la permuta f1/f31 la causa **`canon_reg` de `cse`**.
**No es cse.** El volcado RTL lo dice al insn:

| pase | la resta del bloque de redlining |
|---|---|
| `.rtl`, `.jump`, `.cse` | `(set (reg/v:SF 85) (minus:SF (reg/v:SF **85**) …))` |
| `.gcse` | `CPROP of void SFXCTL_Engine::UpdateRPM(float), pass 1: … **1 copy props**` |
| `.loop`, `.cse2`, `.combine` | `(set (reg/v:SF 85) (minus:SF (reg/v:SF **84**) …))` |

reg84 = `Cur_RPM`, reg85 = `VisualRPM`. Es **la propagación de copias de `gcse`**
(`cprop`), que sustituye el uso de `VisualRPM` por `Cur_RPM` porque la copia
`VisualRPM = Cur_RPM;` está *disponible* en ese bloque. Eso alarga a `Cur_RPM`
por encima de las dos llamadas a `smooth` del bloque de redlining y la obliga a
un registro salvado. `lreg.py`/`.lreg` lo confirma:

    antes:    Register 84 … across 78 insns; crosses 2 calls   <- Cur_RPM  -> f31
              Register 85 … across 35 insns;                   <- VisualRPM-> f1
    después:  Register 84 … across 19 insns;                   <- Cur_RPM  -> f1
              Register 85 … across 66 insns; crosses 2 calls   <- VisualRPM-> f31

que es **exactamente** el reparto del objetivo.

### La palanca: matar la *disponibilidad* de la copia, no la copia

`gcse.c: hash_scan_set` sólo mete una copia `x = y` en la tabla de cprop si ni
`x` ni `y` se vuelven a escribir; y `cprop_avail_out` la mata si se escribe
cualquiera de las dos **en cualquier bloque del camino**. Basta con que
`Cur_RPM` se vuelva a asignar entre la copia y la resta. Aplicado (y es una
forma de fuente natural, no un truco): el resultado de `smooth` del bloque de
redlining pasa por `Cur_RPM` en vez de ir directo al campo.

```cpp
if (this->bRedliningBounce) {
    Cur_RPM = smooth(this->RedlineingVisualOffset, Target, 50.0f);
    this->RedlineingVisualOffset = Cur_RPM;
    if (Cur_RPM == Target) { this->bRedliningBounce = false; }
} else {
    Cur_RPM = smooth(this->RedlineingVisualOffset, 0.0f, 50.0f);
    this->RedlineingVisualOffset = Cur_RPM;
    if (Cur_RPM == 0.0f) { this->bRedliningBounce = true; }
}
```

Ensayos (base 96,45252 % / 720 B / 16 diffs):

    p1  NormalRPM desde Cur_RPM                              idéntico
    p2  la copia después de SetEngRPM                        92,53632   PEOR
    p3  la copia dentro de la rama else                      92,53632   PEOR
    p4  la resta escrita con Cur_RPM (lo que produce cprop)  idéntico
    p5  `float VisualRPM = 0.0f;`                            idéntico
    p6  `float Cur_RPM = 0.0f;`                              idéntico
    q1  la copia duplicada tras SetEngRPM                    idéntico
    q2  NormalRPM antes de la copia                          92,53632   PEOR
    q3  `VisualRPM = Cur_RPM; Cur_RPM = VisualRPM;`          **99,80447  716 B  4 diffs**
    q4  q3 con NormalRPM desde Cur_RPM                       **99,80447  716 B**
    q5  la copia en las dos ramas del if del embrague        94,35195   PEOR
    q6  copia encadenada por una tercera variable            96,31285   PEOR
    r1  matador tras SetEngRPM                               **99,80447  716 B**
    r2  matador entre NormalRPM y SetEngRPM                  **99,80447  716 B**
    r3  matador `Cur_RPM = NormalRPM;`                       **99,80447  716 B**
    r4  matador `Cur_RPM = 0.0f;` al final del bloque        90,78771   732 B  PEOR
    r5  r4 pegado a la copia                                 90,78771   732 B  PEOR
    s1  matador al principio de la rama else                 **99,80447  716 B**
    s2  matador dentro de `if (bIsRedlining)`                **99,80447  716 B**
    s3  matador justo antes de la resta                      **99,80447  716 B**
    s4  matador justo DESPUÉS de la resta                    96,45252   (no vale: ya no domina)
    v1  Cur_RPM como temporal de los dos `smooth` (APLICADA) **99,80447  716 B  4 diffs**
    v2  v1 comparando el campo en vez del temporal           **99,80447  716 B**
    v3  v1 sólo en una de las dos ramas                      **99,80447  716 B**

**Regla nueva, y es de las que valen para todo el árbol**: cuando `gcse` imprime
`N copy props` y sobra/falta un `mr`/`fmr`, **no busques la forma de la copia:
busca dónde volver a escribir uno de los dos registros en el camino**. La copia
deja de estar disponible y el reparto cambia entero.

### Lo que queda: 4 diffs, y es un empate de `local-alloc`

    59 lfs f0, 0x13c(r31)   | lfs f11, 0x13c(r31)      <- m_RPM_LFO: f0 / f11
    63 fadds f1, f31, f0    | fadds f0, f31, f11
    65 fadds f1, f1, f13    | fadds f0, f0, f13
    67 fadds f0, f1, f0     | fadds f0, f0, f11

`NormalRPM` cae en f0 en los dos. Lo que cambia es que **el objetivo ata el
destino del último `fadds` al SEGUNDO operando** (`m_RPM_LFO`, que muere ahí) y
nosotros **al PRIMERO** (el temporal de la cadena, que también muere ahí). Es
`combine_regs` de `local-alloc` recorriendo los operandos en orden: el primero
que muere se lleva el destino. En el objetivo la cadena queda en f1 y el LFO en
f0; en el nuestro la cadena y `NormalRPM` comparten f0 y el LFO se va a f11
(f13 y f12 ocupados). Barridas **20 formas** sin moverlo:

    t1 lfo en local · t2 NormalRPM desde Cur_RPM · t3 sin NormalRPM (expresión
    en SetEngRPM) · t4 la suma en tres sentencias · t5 comp en local ·
    t6 orden LFO+LFO+comp (99,77653, PEOR) · y1/y2 NormalRPM antes de la copia
    (96,59218, PEOR) · y3 LFO primero (99,77653) · y4 `+=` final · y5/y6 orden
    de declaración de las locales · y7 paréntesis · z1 LFO leído antes de la
    copia · z2 LFO declarada arriba · z3 comp antes · z4 SetEngRPM directo ·
    z5 dos temporales explícitos · z6 PhysicsNewAudioRPM declarada la primera
    · aa1/aa2 LFO a la izquierda del último `+` (99,46928, PEOR) ·
    aa3 temporales con LFO a la derecha · aa4 doble lectura de LFO ·
    aa5 comp al final (99,77653)

**Veda**: barridas la sentencia `NormalRPM = VisualRPM + m_RPM_LFO + comp +
m_RPM_LFO` en 20 formas (asociatividad, temporales, locales para los sumandos,
sin `NormalRPM`, y el orden de declaración de las cuatro locales) más el sitio
de la copia. Ninguna mueve el empate de `combine_regs`.

## 4. `EvalSQT__…FnStatelessQ` — sigue en 99,45946 %, y el diagnóstico es al insn

`faltan 0, sobran 2 · clrlwi-2`. El RTL cierra la pregunta que dejó abierta la
21 («¿dónde nace el pseudo en modo corto?»). **No nace en modo corto: nace igual
que en `FnStatelessF3`.** Los dos ficheros producen, hasta `cse2` inclusive,
**el mismo RTL byte a byte salvo el número de pseudo y el desplazamiento del
campo**:

    (insn 151 …  (set (reg:HI 144) (reg:HI 628)))            ; 595 en F3
    (insn 152 …  (set (reg:SI 143) (zero_extend:SI (reg:HI 628))))
    (insn 154 …  (set (reg:SI 145) (plus:SI (reg:SI 143) (const_int -1))))
    (insn 155 …  (set (reg:CC 146) (compare:CC (reg/v:SI 108) (reg:SI 145))))

En **`combine`**, `FnStatelessF3` funde 152 en 154 y deja
`(plus:SI (subreg:SI (reg:HI 595) 0) (const_int -1))` — que es **exactamente lo
que hace el objetivo**. En `FnStatelessQ` **combine no funde** y quedan las dos
insns. Con el `static_cast` actual, combine sí genera el
`(plus:SI (subreg:SI (reg:HI 628) 0) -1)` correcto y **lo único que sobra es el
`(zero_extend:SI (subreg:HI …))` del cast**, o sea que estamos a **una insn** por
los dos lados y ninguna forma de fuente la quita.

Descartado con medida (cada uno es una hipótesis muerta, no un barrido a ciegas):

| hipótesis | prueba | resultado |
|---|---|---|
| el uso extra de `mNumKeys` (línea 155) rompe la fusión | añadido ese mismo uso a `FnStatelessF3` | **F3 sigue fundiendo** (`plus:SI (subreg:SI (reg:HI 600) 0)`) |
| el número de pseudo (628 vs 595) | encogido el cuerpo de `EvalSQT` hasta pseudo **594** | **sigue sin fundir** |
| el desplazamiento del campo (0x14 vs 0x10) | `mNumKeys` movida a 0x10 en el `.ii` | **sigue sin fundir** |
| combine no sabe fundir en esta función | el `mNumKeys - 2` de la línea 96, **sin cast**, sí da `(plus:SI (subreg:SI (reg:HI 590) 0) -2)` | **combine sí funde aquí** |
| el número de defs/usos del pseudo PRE | contados en `.cse2`: **5 sets y 9 menciones en las dos** | idénticos |

Y quitar el cast **no cuesta instrucciones**: son los mismos 1.488 B y los mismos
2 `clrlwi`, sólo que GCC extiende la **entrada** (`clrlwi r9,r5,16; subi`) en vez
de enmascarar la **salida** (`subi; clrlwi`); el 98,05 % que anotó la 21 es ruido
del planificador, no código de más.

Formas nuevas medidas (además de las 22 de la ronda 21):

    n1  `(unsigned)floorKey >= (unsigned)(mNumKeys-1)`   96,97027   PEOR
    n2  `static_cast<short>(mNumKeys - 1)`               idéntico
    n3  los dos lados a `unsigned short`                 97,32162   PEOR (1.496 B)
    n4  `>= mNumKeys - 2 + 1`                            98,05135
    n5  `int numKeys` leída una vez arriba del todo      94,22703   (1.468 B)
    n6  `unsigned short numKeys` arriba del todo         94,22703   (1.468 B)
    n7  a través de una referencia                       98,05135
    n8  a través de `*(&mNumKeys)`                       98,05135
    n9  `floorKey - (mNumKeys-1) >= 0`                   97,04054   PEOR
    n10 `floorKey+1 >= mNumKeys-1+1`                     97,31081   PEOR
    q1  sin cast + sin el uso de la línea 155            96,57838   PEOR
    q2  q1 + sin el de la 240 (que es de OTRA función)   96,57838   PEOR
    q3  con cast + sin 155/240                           97,98649   PEOR
    q4  sin cast + sin 240                               98,05135

**Veda (definitiva, con RTL)**: la sentencia
`floorKey >= static_cast<unsigned short>(statelessQ->mNumKeys - 1)` está
**agotada por fuente** — 22 formas en la 21 y 14 más aquí, y el volcado demuestra
que el RTL de entrada a `combine` es el mismo que el de la hermana que sí casa.
Lo que falta es entender **por qué `try_combine` falla en `FnStatelessQ` con RTL
idéntico**; y también está descartado que sea el uso extra, el número de pseudo,
el tamaño de la función o el desplazamiento del campo. **`nvol`, ojo: la línea
240 es de `EvalSQTMask`, no de `EvalSQT`** — la ronda 21 la contaba como uso de
esta función y no lo es.

## 5. `Play__9EAXCommon18eMenuSoundTriggers` — sigue en 96,82927 %, pero la causa ya está localizada

`faltan 3, sobran 2` (`faltan li, bne, b` / `sobran beq, mr`). La 21 lo dejó como
«una decisión de `jump.c`». **No es `jump.c`: es `cse`, y la consecuencia es un
conflicto con el registro duro r3.**

En `.jump` los cuatro guardias tienen **estructura idéntica** (salto condicional
+ bloque `r3 = cte; goto EPI` + barrera + etiqueta). En **`.cse`** el
`(set (reg/i:SI 3 r3) (const_int 0))` del **cuarto** guardia (insn 80) sale de su
bloque y reaparece como **insn 456 entre la comparación y el salto** del bloque
que lee `0xc(ptr)`. A partir de ahí:

- r3 está vivo dentro del rango del pseudo del puntero →
  `.greg` lo dice literal: `;; 96 conflicts: 82 83 96 97 102 103 0 1 **3**`;
- el puntero `m_pSFXOBJ_FEHUD` no puede ir a r3 y cae en r9;
- hace falta `mr r3, r9` antes de `GetDMixOutput`;
- y como los tres `return -1` quedan idénticos, `jump2` los cruza en un solo
  bloque y **el `li r3,-1` del segundo guardia ya no se puede izar**.

En el objetivo pasa al revés: cse iza el `li r3,-1` del **segundo** guardia, el
puntero se carga directamente en r3 (`lwz r3, 0x24(r28)`) y no hace falta `mr`.

**Y sí se puede mover — pero desde el CUERPO, no desde los guardias.**
Metiendo el cuerpo entero dentro de `if (…GetOutputBlockPtr()) { … }`, **las
filas 0-21 pasan a casar EXACTAMENTE con el objetivo**, incluidos el `li r3,-1`
izado (fila 13), el `lwz r3, 0x24(r28)` (fila 15) y el `lwz r0, 0xc(r3)`
(fila 20). Lo que se rompe entonces es la cola: el `return 0` pasa a ser un punto
de unión, `li r3,0` queda pegado al epílogo y `jump2` lo cruza (−4 B).

    bb1  cuerpo dentro de `if (GetOutputBlockPtr())`      96,61789  484 B   6 diffs
    bb2  `goto ret0` con la etiqueta al final             96,61789  484 B   6 diffs
    bb3  guardia con llaves                               idéntico
    cc1  bb1 + `return 0` también dentro del if           96,61789  **492 B**  6 diffs
    cc2  `if (…== nullptr) {} else { cuerpo }`            96,61789  484 B   6 diffs
    cc3  `goto ret0` con el `return 0` antes de la etiqueta 96,61789 **492 B**  6 diffs
    cc4  `else { return 0; }`                             96,61789  484 B   6 diffs
    dd1  `int nvol = 0;` y `return nvol`                  idéntico
    dd2  local `result = 0` como valor de retorno         idéntico
    dd3  local `SFX_Base *pobj`                           idéntico
    dd4  guardia de Debug con llaves + `== 0` en el 4.º   idéntico
    ee1  `return 0` dentro del `if (handle)` del delete   90,03252   PEOR
    ee2  `!= nullptr` explícito                           idéntico
    ee3  comparación del slider invertida                 idéntico

Con `cc1`/`cc3` el **tamaño es el del objetivo (492 B)** y sólo quedan 6 diffs:
la cabecera casa entera y la cola casa entera; lo único que falla es que el
bloque `li r3,0; b EPI` del cuarto guardia se emite **al final de la función** en
vez de justo detrás del guardia (GCC 2.95 no reordena bloques: el sitio del
bloque lo decide el orden en la fuente, y con `if (!X) return 0;` sale bien pero
entonces cse vuelve a izar ese `li`).

**Revertido** porque baja el porcentaje (96,83 → 96,62) y no cierra. **Lo que
queda por hacer está a un paso**: hace falta una fuente en la que (a) el
`return 0` del cuarto guardia siga escrito inmediatamente detrás del guardia
—para que su bloque se emita ahí— y (b) el CFG tenga el elemento extra que hace
que cse ice el `li` del segundo guardia y no el del cuarto. Los 25 barridos de
la ronda 21 y los 13 de aquí demuestran que **no está en la forma de los cuatro
guardias**; está en el cuerpo.

## 6. `EvalState__…FnRawStateChan` — sigue en 93,50877 %

`faltan 2, sobran 2 · add-2, mr+2`. Localizado al byte: son **dos sitios** (filas
40 y 80 del diff) donde el objetivo hace `mr r4, r11` y nosotros
`add r4, r30, r10`. Es la base de `GetKeyData`:

    objetivo:  add r11, r30, r9  |  addi r4, r11, 0x2  |  beq  |  mr  r4, r11
    nuestro:   add r8,  r30, r10 |  addi r4, r8,  0x2  |  beq  |  add r4, r30, r10

O sea: los dos calculan la base impar antes de la bifurcación y derivan la par
con `+2`; el objetivo **reutiliza** la base en la rama impar y nosotros la
**recalculamos**. Lo demás del diff (41 filas) es el arrastre de registros.

Barridas **11 formas nuevas** (además de las 12 de la ronda 21):

    ff1 base común + `if (!(nf&1)) base += 2`             55,25439  404 B  PEOR
    ff2 `unsigned short *p` común a las dos ramas         57,68421  412 B  PEOR
    ff3 `+=` en vez de `= base + 2`                       56,14912  404 B  PEOR
    ff4 ternario                                          55,25439  404 B  PEOR
    ff5 base par y `-2` en la impar                       57,68421  412 B  PEOR
    gg1 ramas invertidas                                  55,92105  500 B  PEOR
    gg2 `+ 2` sobre el puntero de bytes                   idéntico
    gg3 `-2` sobre la base par en la rama impar           idéntico
    gg4 autoasignación de keyData                         idéntico
    gg5 `(nf & 1) != 0`                                   idéntico
    hh1 `int keySize` local en el bucle                   91,73684   PEOR
    hh2 `int numKeys` local en el bucle                   80,48245  464 B  PEOR
    hh3 `*(float*)(k + GetKeySize())`                     idéntico
    hh4 `const int keyIdx`                                idéntico
    hh5 `this->mKeyIdx`                                   idéntico

**Veda**: barridas las 10 formas de `RawStateChan::GetKeyData` (base común,
ternario, `+=`, ramas invertidas, `+2` en bytes, `-2`) y 5 del bucle `while` de
`EvalState`. **Hoisting la base a una expresión común es catastrófico** (−38 pp,
la función pierde 52 B): el `if/else` de dos ramas de `GetKeyData` **es** la
forma del original y no se toca. Lo que falta es que el pseudo de la base impar,
que ya está calculado antes de la bifurcación, se **use** en la rama impar en
vez de recalcularse — es una decisión de `loop.c`/`local-alloc`, no de la
sentencia.

`RawStateChan.h` sólo entra en `RawStateChan.cpp` y `MemoryPoolManager.cpp`, las
dos en zEagl4Anim: si alguien vuelve ahí, el A/B es barato.

## 7. `DynamicLoader::Initialize` — no tocada

Por la veda explícita del encargo («no gastes la ronda en el switch») y porque
las tres rondas anteriores dejaron 14+ ensayos sin ganancia. Sigue en
91,33673 %, `faltan 14, sobran 5`. Confirmado que el `register HashPointer *pHP
asm("r31")` sigue en su sitio.

## 8. Qué NO he probado

- **`FnStatelessQ`**: no he mirado `try_combine` con `-fdump`-equivalente
  (GCC 2.95 no tiene `-fdump-rtl-combine-details`); no he probado a compilar la
  función en un fichero aparte para ver si fuera de contexto sí funde; no he
  probado `permuter.py`; y **no he probado banderas por función** (la unidad es
  ProDG/GCC 2.9, sin `#pragma`, pero `mn_sweepflags.py` sí existiría para probar
  `-fno-gcse`/`-fno-expensive-optimizations` en un `.cpp` suelto y ver si la
  fusión aparece: **eso es lo primero que haría el siguiente**, aunque sea sólo
  como diagnóstico, porque diría qué pase la bloquea).
- **`Play__9EAXCommon`**: no he encontrado la fuente que junta (a) el bloque del
  cuarto guardia en su sitio y (b) el CFG que hace que cse ice el segundo. No he
  probado `permuter.py` ni a mover funciones dentro de `EAXFrontEnd.cpp`. No he
  probado a añadir `SndBase::GetOutputPtr(int)` a `SndBase.hpp` (la 21 lo midió:
  idéntico al bit, y es cabecera compartida).
- **`UpdateRPM`**: no he probado `permuter.py` sobre los 4 diffs que quedan, ni
  un pin de entrada `register float asm("f1")` (último recurso). No he mirado
  `SetEngRPM`/`SetEngTorque`.
- **`EvalState`**: no he probado a nombrar la base en la fuente de `EvalState`
  (`unsigned char *base = c->GetKeyData(0);` + `k = base + i*GetKeySize()`),
  que rompería el árbol de inlines del DWARF pero podría dar el `mr`. Ni
  `permuter.py`. Ni `lreg.py` sobre esta función.
- **`Initialize`**: nada, por veda.
- **`.sdata`/`.sdata2`**: no comparadas byte a byte contra el ELF (`audit.py` no
  las mira enteras).
- **El muro** de zEAXSound (10 fns, 7.004 B) y el de zEagl4Anim: no tocados.

## 9. Herramientas dejadas en el scratchpad (prefijo `c22ae_`)

- **`c22ae_sw.py <spec.py> [nombres…]`** — arnés multi-parche sobre **stub**
  (compila un `.cpp` suelto con los cflags exactos, 1 s en zEagl4Anim, 24 s en
  zEAXSound), mide con objdiff y **restaura siempre**. `DUMP=<variante>` vuelca
  el diff de instrucciones completo. Casa los bloques con `\r?\n` por línea.
- `c22ae_pre_eng.txt` / `c22ae_pre_fe.txt` — el prefijo del SourceList que hay
  que pasar en `PRE` para que `SFXCTL_Engine.cpp` / `EAXFrontEnd.cpp` compilen
  sueltos (sin él fallan con `parse error` y `no member named mmsgMCoundown`).
- `c22ae_spec_*.py` — los ~90 ensayos de arriba, reproducibles.
- `c22ae_q.ii`, `c22ae_qnc.ii`, `c22ae_f3.ii`, `c22ae_f3x.ii`, `c22ae_qsm.ii`,
  `c22ae_qoff.ii`, `c22ae_eng.ii`, `c22ae_engk.ii`, `c22ae_fe.ii` y sus
  volcados `.rtl/.cse/.gcse/.loop/.cse2/.combine/.lreg/.greg/.sched` — los
  experimentos de RTL de §2-§5, listos para repetir.
- `c22ae_cflags_anim.txt` / `c22ae_cflags_eax.txt` — los cflags exactos.
