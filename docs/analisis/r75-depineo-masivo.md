# r75 -- Despineo masivo de 7 unidades (Ronda 75)

Fecha: 2026-09-15. Directiva maxima del usuario: QUITAR TODOS LOS PINES ASM
(register ... asm("rXX") y barreras asm("" : ...)). El codigo debe compilar en
PS2/X360/PC. Preferimos un porcentaje verdadero sin pin que un 100% falsificado
con pin. Metodo del piloto zWorld2 (c6caebb1): quitar TODOS los pines, medir con
objdiff, y recuperar lo que se pueda con FORMAS NATURALES de fuente guiadas por
el DWARF del original (python scripts/dwarf1.py fn <nombre>).

Alcance: vfprintf.c (libc), criticalpath.c (vp6), pathnode.cpp (path),
JoyE.cpp (zPlatform), spchpick.c (spch), filesys.cpp (realcore file),
EAXAemsManager.cpp (zEAXSound) + CARSFX_PreColWoosh.cpp y NFSMixMapState.cpp
(ambas incluidas por SourceLists/zEAXSound2.cpp -- ver nota al final).

Resultado global: 0 pines en las 9 ficheros. `ninja build/GOWE69/main.dol`
compila y enlaza. Ningun configure.py, flags ni asm nuevo tocado. Los renombres
__asm__("simbolo") y el asm de COLOCACION de datos (.section/.byte) quedan
(como pide la ronda). El codec VP6 escrito a mano del original (criticalpath.c,
asm con instrucciones reales de psq_l/psq_st) queda: es codigo del original, no
un pin.

Medicion: objdiff-cli diff -1 build/GOWE69/obj/<u>.o -2 build/GOWE69/src/<u>.o
-c function_reloc_diffs=none -c ppc.calculatePoolRelocations=false (columna
"target" = tamano del original; el match_percent es el del objetivo).

--------------------------------------------------------------------------------
1. libc/vfprintf.c  (unidades vfprintf.o y vfprintf_1.o)
--------------------------------------------------------------------------------
Pines retirados (30 lineas, mas de los 18 del censo): 10 pines A de registro
(ap r29, fmt r22 x2, ch r26/r27, flags r24, ret r14, width r17, dprec r15,
number_end r25, finalIndex/format_byte/float_format/field_count/mark/state r0-r8),
todas las barreras B de la funcion grande, las tres de add_separators, y las
cuatro de MACRO que el censo no veia: el clobber "memory" del final de PRINT y
las tres PAD_KEEP_blanks/zeroes/cola.

La clave de la unidad: los pines estaban SUSTITUYENDO A UNA FORMA DE FUENTE.
Con el do{}while(0) + barreras todo quedaba 100%; sin barreras caia a 87,5.
La forma natural de newlib (que es la del original) es PRINT y PAD como bloques
{ } sueltos, sin do-while: el do{}while anade un nivel de bucle y con el cambia
la prioridad (n_refs por loop_depth) de todo lo que hay dentro. Con { } y CERO
asm la escalera de registros del original sale sola.

Funcion                     | con pin | sin pin (naive) | mejor forma natural
----------------------------+---------+-----------------+---------------------
_vfwrite                    | 100,0   | 100,0           | 100,0 (sin cambios)
vfprintf                    | 100,0   | 100,0           | 100,0 (sin cambios)
_vfprintf_r (vfprintf.o)    | 100,0   | 87,48           | 99,85 (6128 B exactos)
_vfiprintf_r (vfprintf_1.o) | 100,0   | 87,94           | 99,84 (5760 B exactos)
add_separators              | 100,0   | 96,33           | 96,33 (392 B exactos)

Formas naturales aplicadas:
- ch = *fmt++; directo (los bloques format_byte asm("r0") eran dos locales
  inventadas; el original no las tiene).
- ch = ch == 'g' ? 'e' : 'E'; (la local float_format era inventada).
- ret += width > realsz ? width : realsz; (la local field_count era inventada).
- PRINT/PAD como bloques { } de newlib, sin barrera de memoria y sin PAD_KEEP.
Variantes medidas en add_separators (4: tres ordenes de declaracion de
mark/state/count y sin `register`): las cuatro dan EL MISMO objeto 96,33; es la
permutacion pura state=r9/mark=r3/count=r6/p=r8 contra r7/r8/r12/r6 del
objetivo, documentada desde la r65 (no hay DWARF en libc: sin oraculo).
Restante en _vfprintf_r (0,15%): solo nombres de simbolo de pool
(lbl_8040FC28@ha contra [.rodata]+0x70@ha) y formato de rama; en
_vfiprintf_r, un par de lwz intercambiados. .text 6948/6972 = exacto.

--------------------------------------------------------------------------------
2. Packages/vp6/.../criticalpath.c  (unidad criticalpath.o)
--------------------------------------------------------------------------------
Pines retirados: finalIndex r27 + finalOffset + su barrera (ReadTokensPredictB),
bp r23 con copia local (PredictFilteredBlock) y TempPtr2 r27.

Dato DWARF: el original tiene a bp como PARAMETRO de verdad en r23 (sin copia
local) y TempPtr2 r27; VP6_ReadTokensPredictB no sale en el DWARF (inline).

Funcion                | con pin | sin pin | mejor forma natural
-----------------------+---------+---------+--------------------
VP6_DecodeBlock        | 100,0   | 99,87   | 99,87 (4728 B exactos)
VP6_PredictFilteredBlock| 100,0  | 96,92   | 96,92 (740 B exactos)
(resto, 19 funciones)  | 100,0   | 100,0   | 100,0

Formas naturales aplicadas:
- ReadTokensPredictB: return pbi->EobOffsetTable[i - 1]; (una sola expresion;
  finalIndex/finalOffset eran locales inventadas para sostener el pin). El
  0,13% que queda es el cruce r26<->r27 documentado en scaf-audio r1 (29 filas,
  todas del par i/CoeffData del inline).
- PredictFilteredBlock: parametro renombrado a `bp` (sin _bp ni copia), que es
  lo que el DWARF describe. Quedan DOS ciclos cerrados de dos del asignador ya
  medidos en r65/r66/r67 (5+5 formas de fuente y 24 flags): bp/OutputPtr
  (r23/r24 al reves) y Stride/TempPtr2 (r26/r27 al reves). Variante medida
  nueva: `register unsigned int TempPtr2` (prioridad) -> mismo objeto.
  .text 12040 B = exacto.

--------------------------------------------------------------------------------
3. Speed/Indep/Libs/path/.../pathnode.cpp  (unidad pathnode.o)
--------------------------------------------------------------------------------
Pines retirados: fevery fr10 y notes fr11 en PATHI_calcwaitbeat (los 8 del
censo eran 2 pines + 6 menciones en comentarios).

Dato DWARF: el original declara scalar f11, fevery f11 (repetido; la buena es
f10), foffset f0, timeinbar r8, firstsynchtime r7, nextsynchtime r3. NO tiene
la local `notes`: nuestra `notes` era un invento para pinar fr11.

Funcion             | con pin | sin pin | mejor forma natural
--------------------+---------+---------+--------------------
PATHI_calcwaitbeat  | 100,0   | 98,51*  | 98,51 (336 B exactos)
(resto, 10 funcs)   | 100,0   | 100,0   | 100,0
(*) 98,87 con la local notes sin pin; 98,51 con la forma DWARF (sin notes).

Forma natural aplicada: scalar = (float)beatinfo->notes / (float)note;
(local notes eliminada). Lo que queda es la rotacion de cuatro FPR
(f8/f9/f10/f11) documentada en r67: las 24 permutaciones de las cuatro
sentencias independientes dan solo DOS objetos; no responde al orden de
emision. .text 4204 B = exacto.

--------------------------------------------------------------------------------
4. Speed/GameCube/Src/JoyE.cpp  (unidad zPlatform)
--------------------------------------------------------------------------------
Pines retirados (7 del censo + 2 que el censo no contaba): el pin guard r8, la
barrera "=r"(guard), las CINCO barreras de relleno "+r"(guard), y los DOS
asm volatile CON INSTRUCCIONES que emparejaban el store de AnalogLeftX
("extsh %0,%1; mr 9,%0" y "andi. 11,9,0x8000"). Esos dos emitian instrucciones
muertas PowerPC: el peor genero de pin ( imposible en PS2/X360/PC ).

Funcion                 | con pin | sin pin | mejor forma natural
------------------------+---------+---------+--------------------
ActualReadJoystickData  | 100,0   | 98,55   | 98,55 (1588 -> 1576 B)
PlatformInitJoystick    | 100,0   | 100,0   | 100,0
(otras 134 de zPlatform | 100,0   | 100,0   | 100,0: solo esta funcion cambia)

Forma natural aplicada: AnalogLeftX = data; igual que los bloques hermanos
RightX/RightY (el asm convertia v=(short)data antes del store). Variante
medida: store con (short)data + v = (short)data detras -> DCE se lo lleva,
mismo 98,55. Los 12 B que pierde son TRES instrucciones muertas del original
(mr r9,r0 + andi. r11,r9,0x8000 + un extsh) que con este cc1 no salen de
ninguna forma de C (once formas en r48 + escalera r60): la fuente original
tenia una sentencia en la linea 307 que leia `v` que nosotros no hemos
reconstruido; sin ella el codigo es mas corto y HONESTO. Lo que queda del diff:
registro del 0x4330 (r4 contra r3), data en r8/r4 contra r10/r11 del objetivo,
y el par muerto. linkdelta: zPlatform .text -12, resto IGUAL.

--------------------------------------------------------------------------------
5. Speed/Indep/Libs/spch/.../spchpick.c  (unidad spchpick.o)
--------------------------------------------------------------------------------
Pines retirados: activeSentence r25, sampleTable r17, las dos barreras
anti-dependencia de ChooseSamples, y phraseOffset r9 de MakeSampleRequests.

Dato DWARF (ChooseSamples): `sentence` es el PARAMETRO en r25 (no hay
activeSentence) y no hay sampleTable (el r17 es un temporal del compilador).
Dato DWARF (MakeSampleRequests): el original no declara nada en ese bucle;
phraseOffset/phrases/currentBankHandle eran nuestras.

Funcion             | con pin | sin pin (naive) | mejor forma natural
--------------------+---------+-----------------+---------------------
iSPCH_ChooseSamples | 100,0   | 95,53           | 98,00 (412 -> 408 B)
iSPCH_MakeSampleRequests | 100,0 | 98,32         | 98,32 (524 B exactos)
(resto, 41 funcs)   | 100,0   | 100,0           | 100,0

Formas naturales aplicadas y medidas (4 variantes):
- activeSentence -> usar `sentence` directamente (DWARF).
- MakeSampleRequests: el bucle como phraseChoice = &gEventChoice[channel].phrases[i];
  ... != phraseChoice->bankHandle (la forma natural de la r65). Queda UN empate
  de sched1 (mulli r9 contra r11 + lwzx/add intercambiados), medido en r67.
- ChooseSamples, la mejora de la ronda: la asignacion sampleTable = bank + 0xC
  MOVIDA a su punto de uso (el bloque postMatchParms, DESPUES del bucle).
  95,53 -> 98,00. Las barreras eran anti-dependencias de planificacion que
  forzaban el addi r17 abajo; como sampleTable solo se usa tras el bucle, la
  forma natural es computarla alli. (Variante sin local, expresion unica:
  97,95, peor que con local.)
- El -4 B de ChooseSamples: sin el r17 vivo a lo largo del bucle, el stmw del
  prologo baja de r17 a r18 (una instruccion menos). Documentado; splits.txt
  de la unidad no depende del tamano.

--------------------------------------------------------------------------------
6. Speed/Indep/Libs/realcore/.../filesys.cpp  (unidad filesys.o)
--------------------------------------------------------------------------------
Pines retirados: las CINCO barreras de FILEOPERATION::AddToQueue (dos parejas
"+r"/"r"(this) de la cabecera, la "+m" del pending, y las dos "+r"(current)
del bucle).

Funcion                   | con pin | sin pin | mejor forma natural
--------------------------+---------+---------+--------------------
AddToQueue                | 100,0   | 95,16   | 95,16 (252 -> 248 B)
(resto, 73 de 74 funcs)   | 100,0   | 100,0   | 100,0

Lo que queda son los DOS frentes que la r36e/r37 cerraban con barreras y que
estan diagnosticados como estructura del CFG, no forma de sentencia (~23 formas
de fuente + 24 flags + volcado del scheduler en scaf-r1):
- el swap lwz Head()/slwi prioridad (el consumidor de la carga vive en otro
  bloque; INSN_PRIORITY 2 contra 3, pierde siempre), y
- la prueba de nulo redundante del incremento (mr.+beq), que GCC pliega por
  jump threading al entrar por goto con current != 0 conocido.
Sin las barreras el tamano baja 4 B (la prueba plegada). .text de la unidad
10872 -> 10868. La unidad filesys no esta en la lista de linkdelta (unidad
promovida/saltada): el -4 B queda documentado aqui.

--------------------------------------------------------------------------------
7. EAXAemsManager.cpp (zEAXSound) + CARSFX_PreColWoosh.cpp y NFSMixMapState.cpp
--------------------------------------------------------------------------------
NOTA DE UNIDADES: EAXAemsManager.cpp se compila en zEAXSound.o, pero
CARSFX_PreColWoosh.cpp y NFSMixMapState.cpp las incluye
SourceLists/zEAXSound2.cpp, o sea que SUS funciones viven en zEAXSound2.o.
Solo se han tocado esos tres ficheros; el resto de ficheros de zEAXSound2
quedan intactos (GenerateRoadNoise 96,80 es PREEXISTENTE, igual antes que
despues). linkdelta: zEAXSound y zEAXSound2 .text +0 IGUAL.

Pines retirados: las cuatro barreras "+r"/"r"(this) de SetupNextLoad, las
cuatro de MsgBarrier (+m bGoingToCollide, +r fadeOut, pin r4 de
interpolationTime y su barrera), y los tres pines de CreateSubMixChannels
(offset r0, zeroAdded r10, numCh r0).

Funcion                | con pin | sin pin | mejor forma natural
-----------------------+---------+---------+--------------------
SetupNextLoad          | 100,0   | 99,66*  | 99,66 (1008 B exactos)
MsgBarrier             | 100,0   | 94,29   | 94,29 (140 B exactos)
CreateSubMixChannels   | 100,0   | 99,40   | 99,40 (336 B exactos)
(*) la nota r67b predecia 17 filas para la forma natural; la medida real es
  99,66 (unas 6 filas): mejor de lo que decia el historico.

Formas naturales aplicadas:
- SetupNextLoad: la forma del mapa de lineas del original, SIN locales
  (endIndex/nextBankIndex/currentLoad/bankIndex eran nuestras):
  if (this->m_nCurLoadedBankIndex < this->m_nEndOfList - 1) {
      this->m_nCurLoadedBankIndex++;
      ... RemoveBankListing(this->m_nCurLoadedBankIndex); ...
  Lo que queda es `this` en r27 contra r29 del objetivo (permutacion).
- MsgBarrier: las tres sentencias a pelo (DWARF: cero locales):
  bGoingToCollide = true; mDurationActive = 0.0f;
  WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
  Queda UNA transposicion (li r4,1 delante o detras del stfs), diagnosticada
  con volcados .sched en la r65: empate a cinco niveles de rank_for_schedule
  que decide INSN_LUID, y las cargas de parametro en registro duro las emite
  load_register_parameters siempre pegadas a la llamada. Sin palanca de fuente.
- CreateSubMixChannels: `int offset` conservada (el DWARF del original la
  tiene en r0), zeroAdded -> this->m_SubMixChannelsAdded = 0; y numCh ->
  if (this->m_pSubChHdr->NumMixChannels > 0). Queda el ciclo offset r9
  contra r0 / cero r0 contra r10 (2 filas, medidas en r66/r67).

--------------------------------------------------------------------------------
QUE PINES ERAN SUSTITUTOS DE EXPRESIONES REALES (dato DWARF/line-map)
--------------------------------------------------------------------------------
1. vfprintf: el clobber "memory" de PRINT, las tres PAD_KEEP y el do{}while(0)
   sustituian a la FORMA newlib de las macros (bloques { }). Recuperado al
   99,85/99,84 sin saberlo las rondas r65-r67: el comentario de la cabecera ya
   decia que con { } la escalera de prioridades salia sola y los pines de
   ret/fmt/ap/ch sobraban.
2. vfprintf: format_byte/float_format/field_count eran locales inventadas por
   el pin; su forma natural es una expresion unica (ch = *fmt++, ternarios).
3. pathnode: `notes` no esta en el DWARF del original; su pin fr11 tapaba la
   forma natural `scalar = (float)beatinfo->notes / (float)note;`.
4. spchpick: `activeSentence` no esta en el DWARF (sentence ES el parametro
   r25); la copia con pin sustituia usar el parametro.
5. spchpick: `sampleTable` no esta en el DWARF; el par de barreras sustituia
   COLOCAR la asignacion en su punto de uso (tras el bucle): +2,5 pt.
6. spchpick: phraseOffset/phrases/currentBankHandle no estan en el DWARF; el
   pin r9 sustituia la expresion &gEventChoice[channel].phrases[i].
7. EAXAemsManager: endIndex/nextBankIndex/currentLoad/bankIndex no estan en el
   mapa de lineas; las cuatro barreras "+r"(this) inflaban artificialmente la
   prioridad de this (16 refs contra 12) que la forma sin locales ya da mejor
   de lo historico (99,66).
8. NFSMixMapState: zeroAdded y numCh no estan en el DWARF (solo offset r0);
   eran relleno de reparto.
9. vp6: la copia `bp = _bp` con pin sustituia tener el parametro con el nombre
   bueno (DWARF: bp parametro r23).

FALSOS 100% confirmados (pines que solo maquillaban): todos los de la tabla
que estaban a 100,0 con pin y no a 100,0 sin el. El caso extremo es
ActualReadJoystickData: su 100% necesitaba EMITIR INSTRUCCIONES MUERTAS por
asm (12 B de codigo que no sale de ninguna fuente); el 98,55 actual es el
codigo real.

Pines que NO sustituian una expresion (permutacion/sched puro, historicos
correctos): add_separators (permutacion de 4), PredictFilteredBlock (dos
ciclos de dos), calcwaitbeat (rotacion FPR), AddToQueue (CFG), MsgBarrier
(LUID), CreateSubMixChannels (ciclo r0/r9-r10).

--------------------------------------------------------------------------------
VEREDICTO
--------------------------------------------------------------------------------
- 9 ficheros despineados, 0 asm de pin en el codigo; compila y enlaza
  (main.dol OK). Renombres de simbolo y asm de colocacion de datos intactos.
- Coste honesto total: 12 funciones bajan (ninguna por debajo de 94,29) y todo
  lo demas queda igual. Suma de .text: -20 B en 3 objetos (vfprintf/vp6/
  pathnode/zEAX* exactos; spchpick -4, filesys -4, zPlatform -12).
- Dos recuperaciones que el historico daba por muertas: PRINT/PAD a llaves
  newlib (87,5 -> 99,85) y sampleTable en su punto de uso (95,53 -> 98,00).
  Una tercera, SetupNextLoad, sale MEJOR (99,66) de lo que el historico
  predecia para la forma natural.
- Las unidades que pierden mas (AddToQueue 95,16, MsgBarrier 94,29,
  PredictFilteredBlock 96,92) son permutaciones de registro y empates de
  sched1 con diagnostico de compilador cerrado (r65/r67/scaf-r1): sin pin no
  hay hoy forma de fuente; quedan como deuda honesta.
- Splits/keep: splits.txt solo mapea rangos de direcciones de seccion; ningun
  keep.lst afectado. linkdelta: zPlatform .text -12 resto IGUAL; zEAXSound y
  zEAXSound2 +0 IGUAL (otras unidades que mueven el delta -- zWorld2, zAI,
  zMisc, zPhysicsBehaviors -- son de otros agentes en el mismo arbol).
