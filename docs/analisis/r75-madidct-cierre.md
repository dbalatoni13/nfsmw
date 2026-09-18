# r75 - madidct.cpp: avance de IdctRow/IdctColumn por busqueda de ordenes validas

Estado al cerrar r75 (sin un solo asm en la fuente):

| funcion   | antes r68        | despues r75      | tamano     |
|-----------|------------------|------------------|------------|
| IdctColumn| 50,449368 %      | 55,265823 %      | 632 -> 624 |
| IdctRow   | 46,372093 %      | 49,36434 %       | 496 -> 496 |
| idctcompute | 100 %          | 100 %            | 356 (intacto) |

Datos intactos: idctinput, idctprescale, work y .data-0 al 100 %.
La unidad sigue en NonMatching (no hay 100 %); configure.py no se toca.

## 1. Receta aplicada (lo que cierra)

Nada de macros raras ni asm: SOLO el orden de las 22 sentencias de computo de
cada funcion. El espacio de busqueda son los ordenes topologicos validos del
grafo de dependencias; cada candidato se verifica SEMANTICAMENTE por
simulacion (aritmetica de 32 bits con el MULT exacto, 6 vectores de entrada
incluido el vector cero) y se mide con objdiff contra el objeto original.

- IdctRow gana con el orden (etiquetas r41/r67):
  `d b t a c h e i j f o k g n l m p q r s u v`
  es decir: t4, t2, t7, t1, t3, t9(MULT t1), t5, t2(MULT t2), t1=t3+t2,
  t6(MULT t4-t5), t8(MULT src2-src6), t3=t9-t3, t4=t5+t4, t5=t6+t1,
  t4=t4+t3, t3=t3+t6, t9=(src2+src6)+t8, t6, t2=t6-t9, t6=t6+t9, t9, t7.
  46,37 -> 49,36 % con el mismo histograma (sigue sin mtctr/mfctr).
- IdctColumn gana con el orden (c5_0):
  `o d e a t b c f g i h j n k l m p q r s u v`
  La clave: t8 = MULT(src[2] - src[6], 46341) como PRIMERA sentencia y
  t2 = MULT(t2, 35468) ANTES de t9 = MULT(t1, 85627).
  50,45 -> 55,27 %. Con este orden la CABECERA queda identica al original
  salvo renombramiento de registros: las cargas caen en el orden exacto
  src2, src4, src1, src3, src6, src5, src7; src[4] se derrama a 0x8(r1)
  y la cadena de `or` hereda el registro vivo (el defecto "lwz r11,0x8(r1)"
  de r36f/r63 desaparece: lwz 18/18 como el objetivo), y el bloque principal
  recarga src[4] UNA vez (antes dos). Quedan solo 2 mr (10 contra 12).

## 2. La trampa que habia que cerrar: ordenes invalidos

Tres "ganadores" previos de busquedas sin verificador eran codigo INCORRECTO:
r1_9 (55 %), x4 (57 %) y r31_51 (53 %) intercambiaban silenciosamente
`l`/`m` (t4=t4+t3 con el t3 equivocado), `r`/`s` (t2 con el t6 equivocado) o
`g`/`n` (t4 con el t5 equivocado). El conjunto COMPLETO de restricciones de
reescritura para las 22 sentencias (a..v) es:

    h<j, i<j, j<k, k<l, l<m, g<n (ademas de las de datos),
    r<s, u<v, k<p, m<q, n<q, f<g, j<r

El verificador semantico (scratchpad/zai_r75_madidct/semcheck.py) es la unica
garantia fiable: simula el cuerpo con enteros de 32 bits y MULT exacto y
compara los 8 dest contra el cuerpo de referencia. Cualquier busqueda futura
de ordenes DEBE pasar por ahi.

## 3. Mecanica del resto del delta (diagnostico para el que siga)

3.1 El defecto de IdctRow es de ASIGNACION, no de forma: los cinco
mtctr/mfctr/mr que faltan existen solo si DOS pseudos fallan los GPR:
el acarreo de redondeo de MULT D (t6) a LR y src[6] a CTR. Hoy solo falla
uno (src[6]) y local-alloc lo manda a LR.

3.2 LOCAL y GLOBAL difieren en el orden efectivo de los especiales:
- local-alloc: LR(65) ANTES que CTR(66). Prueba: base/p5 dan 65 con 66
  libre en .lreg ("147 in 65", "116 in 65", "180 in 65").
- global-alloc: CTR(66) antes que LR(65). Prueba: IdctColumn nuestro da
  102->66 y 94->65 con 102 de prioridad MAYOR (lista ordenada por
  allocno_compare en el .greg).
IdctRow es UN UNICO bloque basico: todo lo asigna local-alloc, y ahi CTR no
sale. Para que src[6] caiga en CTR tiene que FALLAR local-alloc (la extension
fake +-2 instrucciones de vida de find_free_reg crea conflictos
instruccion-a-instruccion que global no ve) y llegar a global. Eso es
exactamente lo que consigue el original y lo que no hemos logrado desde
fuente. p5 (los cinco productos izados como long long) SI lo logra (aparece
mtctr en Row) pero cuesta +28 B; la presion correcta esta entre base y p5.

3.3 Los 2 mr de IdctColumn: el original copia hi Y lo del MULT E
(t1*85627) justo tras sus mulhw/mullw (mr r3,r10 / mr r4,r11) porque esos
registros se reusan enseguida; nosotros no reusamos y no copiamos. Es el
mismo fenomeno que los mr r27,r8 / mr r28,r9 de MULT C en el IdctRow
original.

3.4 Versiones de compilador: el ELF mezcla productores v1.72 (13 CU),
v1.76 (260) y v1.83 (15); esta unidad es v1.72 y solo tenemos v1.76.
NO es la explicacion del delta: ProDG 3.7, 3.8.1 y 3.9.3 (v1.46, v1.54,
v1.76) producen el MISMO objeto para este fuente. El fuente de GCC del
arbol (orig/prodg/NGC_GNU_SRC) tampoco cuadra al 100 % con el binario
(REG_ALLOC_ORDER del arbol pone 64 antes de 66 antes de 65 y MQ(64) no
esta FIXED, pero el binario nunca entrega 64 ni 66 en local-alloc).

## 4. Negativos de esta ronda (no repetir)

- Formas del MULT: long long con nombre, macro #define, /65536, 32768LL,
  conmutacion de operandos en las llamadas, MUL+ROUND partido: TODAS dan
  el mismo objeto (o peor: /65536 da 800 B). El eje no es la forma del
  macro sino el orden de sentencias.
- Arboles del or-chain de Column (agrupado por pares, invertido,
  plegado a derecha, con nombre en local): canonizados al mismo objeto
  salvo el invertido (-0,09 %).
- Hoists s0/s4/d26/s26 individuales o por pares sobre los ordenes
  validos: canonizados. Solo el hoist de los CINCO productos (p5) cambia
  la asignacion (aparece CTR en Row, 544 B).
- Permutar el orden ganador de una funcion a la otra: Row con el orden de
  c5_0 baja a 33,7 %. Las dos funciones necesitan ordenes distintos.

## 5. Herramientas nuevas (scratchpad/zai_r75_madidct/)

- harness.py: compila sombras con los flags exactos de la unidad y reporta
  tamano/match/histograma/indices de instrucciones especiales/filas diff
  normalizadas.
- semcheck.py: simulador semantico de los cuerpos (Row y Column).
- perm.py / permcol.py / permcol2.py / perm2.py: generadores de ordenes
  topologicos con restricciones completas + semcheck + medida; semillas
  reproducibles por numero.
- flow.py / xdiff.py: vuelcos numerados y diff normalizado contra original.

Siguiente paso mas prometedor: cazar la presion que hace fallar local-alloc
para src[6] en IdctRow (entre base y p5), y en IdctColumn forzar que E.hi/E.lo
se copien. Ambos son fenomenos de reuso de registros del asignador; el
espacio de ordenes validos ya esta saturado en 49,36/55,27.
