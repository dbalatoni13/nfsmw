# r38 — spchpick completa: +936 B, dos cierres

2026-09-07. Base: HEAD `590dc4a0` mas los cambios conservados de r37.
Unidad: `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick`.

## Resultado

| funcion | antes | despues |
|---|---:|---:|
| `iSPCH_MakeSampleRequests` | 524 B / 98,32061 % | **524 B / 100 %** |
| `iSPCH_ChooseSamples` | 412 B / 95,53398 % | **412 B / 100 %** |

**40/42 -> 42/42 funciones exactas; 6.900/7.836 -> 7.836/7.836 B.**
No es solo fuzzy: se recuperan dos funciones completas, 936 B.

`trypromo.py` da **DOL OK** al sustituir el objeto extraido de spchpick por el
reconstruido. Esta unidad no necesita primero una reparacion de datos como
filesys. Puede proponerse su promocion para ganar **7.836 B linked**, pero
`configure.py` no se ha modificado, conforme al brief. El porcentaje linked
actual NO incluye aun esos bytes.

## MakeSampleRequests: orden, alcance y un solo pin

El objetivo lee el bankHandle con `lwzx r3,r31,r9` ANTES de materializar
phraseChoice con `add r30,r31,r9`. La fuente hacia primero el `add` y ademas
asignaba el desplazamiento a r11, la base de bancos a r10.

La solucion separa tres valores inicializados:

```cpp
register int phraseOffset __asm__("r9") = i * sizeof(PhraseChoice);
PhraseChoice *phrases = gEventChoice[channel].phrases;
int currentBankHandle = ((PhraseChoice *)((char *)phrases + phraseOffset))->bankHandle;
__asm__("" : "+r"(phraseOffset) : "r"(currentBankHandle));
phraseChoice = (PhraseChoice *)((char *)phrases + phraseOffset);
```

La comparacion y la llamada usan `currentBankHandle`. Reutilizar el
`bankHandle` declarado a nivel de funcion era peor: esa cantidad tambien se
usa en el segundo bucle y el compilador la asignaba a un preservado, emitiendo
un `mr` extra. La local corta evita ese acoplamiento. El desplazamiento fijado
a r9 es exactamente el observado en el objeto original.

| ensayo | fuente / resultado |
|---|---|
| C1 | dependencia desplazamiento/handle usando bankHandle de funcion: 528 B, 98,12977 %, `mr` extra y rotacion de preservados |
| C2 | handle local del primer bucle: 524 B, 99,8855 %, solo tres operandos r0/r9 |
| C3 | fijar solo phraseOffset a r9: **524 B, 100 %** |

La local de desplazamiento sola ya estaba vedada por r36f. Lo nuevo es
consumir el handle como entrada de la dependencia SOBRE el desplazamiento y
dar al handle un alcance corto; no es repetir una barrera sobre phraseChoice.

## ChooseSamples: dos dependencias y dos intercambios independientes

El preencabezado adelantaba `bank + 0xC` y la mascara de parametros. La cadena
siguiente reproduce el orden exacto, sin sumar instrucciones:

```cpp
numParms = bank->parmFlags & 0x7F;
sampleData = (unsigned char *)bank + 0xE;
__asm__("" : "+r"(bank) : "r"(numParms));
sampleTable = (unsigned char *)bank + 0xC;
__asm__("" : "+r"(numParms) : "r"(sampleTable));
sampleSize = numParms + 2;
```

Esto deja dos intercambios puros: sampleTable/postMatchParms (r17/r18) y
sampleSize/sentence (r24/r25). Se resuelven fijando sampleTable a r17 y
usando un alias inicializado del argumento sentence en r25. Se mantienen
tanto el algoritmo como los tres usos del argumento.

| ensayo | tamano / fuzzy | resultado |
|---|---:|---|
| D1: primera dependencia bank/numParms | 412 / 99,78641 % | quedan dos `addi` intercambiados |
| D2: segunda dependencia; sampleSize despues | 412 / 99,41747 % | orden exacto, dos ciclos de reparto / 12 operandos |
| D3: fijar postMatchParms/r18 | 396 / 93,14563 % | elimina mfcr/stw/lwz/mtcrf; se retira |
| D4: sobre D2 fijar sampleTable/r17 | 412 / 99,660194 % | queda el segundo intercambio / 7 operandos |
| D5: D4 + pin sampleSize/r24 | 412 / 99,70874 % | arrastra la carga/mascara y el producto final a r24; se retira |
| D6: D4 + alias real sentence/r25, sampleSize sin pin | **412 / 100 %** | cierre |

El segundo pin no se aplica sobre la cantidad que participa en la aritmetica
intermedia: hacerlo cambia tambien los temporales anonimos. Se aplica sobre
el argumento vivo que ocupa el otro lado del intercambio. Ningun valor queda
sin inicializar; los asm son vacios, no cuerpos de instrucciones.

## Auditoria y enlace

- `fncmp.py`: **0 de 42 funciones con codigo distinto**.
- `audit.py`: todas las 42 funciones pasan contra bytes del ELF original,
  condiciones/destinos de ramas, referencias y literales.
- MakeSampleRequests: 524 B, 14 ramas, 34 referencias, cero literales.
- ChooseSamples: 412 B, 14 ramas, 25 referencias, cero literales.
- `codex_r38_spchpick_audit.py`: 85 simbolos, 19 secciones; otras 40 funciones
  identicas, mismos tamanos/direcciones y datos/reubicaciones. Solo se excluye
  metadata de depuracion modificada por las locales y lineas nuevas.
- Los marcos y registros salvados coinciden: 0x70/r19 para MakeSampleRequests
  y 0x48/r17 mas CR para ChooseSamples.
- `lcfix.py --check`: limpio.
- `trypromo.py`: **DOL OK**, sin modificar main.elf/main.dol/configuracion.
- Fuente conserva CRLF y los dos guillemets Latin-1 preexistentes. Para usar
  apply_patch se normalizo temporalmente a UTF-8; se restituyo la codificacion
  al terminar y se recompilo/audito de nuevo. No se cambiaron literales.

## Artefactos

Prefijo `scratchpad/codex_r38_spchpick_`: `before.c`, `before.json`,
`c1.json`--`c3.json`, `d1.json`--`d6.json`, `after.json`, `audit.py`.
Las compilaciones fueron directas de una sola unidad, nunca ninja completo.
No hay cambios en shared headers, flags, splits, `configure.py` ni `keep.lst`.
