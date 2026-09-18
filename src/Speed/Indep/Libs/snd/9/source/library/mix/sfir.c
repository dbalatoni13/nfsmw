#include <snd/sfilter.h>

/* calcFIRCoeffs: CERRADA en r48 --- 936/936 B, 100,0 %, cero filas.
 * `python scripts/fndiff.py Speed/Indep/Libs/snd/9/source/library/mix/sfir \
 *      calcFIRCoeffs__FP11SNDFIRSTATEi`
 *   # calcFIRCoeffs__FP11SNDFIRSTATEi  target=100.0%  ours=100.0%  size=936/936
 *
 * La causa era la que localizo r47 (PRE de gcse.c inserta un bloque puente en
 * la arista default del switch y tres copias `mr`), pero NO hacia falta ningun
 * flag: la condicion es de FUENTE y esta escrita en
 * orig/prodg/NGC_GNU_SRC/NGC/gcc/gcse.c.
 *
 *   hash_scan_set (gcse.c:1884):
 *     int antic_p = ! optimize_size && oprs_anticipatable_p (src, insn);
 *   oprs_unchanged_p, caso REG, con avail_p==0 (gcse.c:1200):
 *     return (reg_first_set[REGNO(x)] == NEVER_SET
 *             || reg_first_set[REGNO(x)] >= INSN_CUID (insn));
 *   y `reg_first_set` se REINICIA EN CADA BLOQUE (compute_hash_table).
 *
 * O sea: una ocurrencia deja de ser anticipable si algun operando se ESCRIBE
 * antes de ella EN SU MISMO BLOQUE BASICO. Y eso basta para apagar PRE entero,
 * porque `pre_delete` recorre SOLO `expr->antic_occr`: sin ocurrencia
 * anticipable no hay borrado, sin borrado no se crea `reaching_reg`, y tanto
 * `pre_edge_insert` como `pre_insert_copies` empiezan comprobando
 * `reaching_reg != NULL`. Las dos expresiones que nos sobraban eran
 *   expression 7 = (ashift (reg halfLen) 2)   y   expression 8 = (plus (reg pfir) 32)
 * y sus ocurrencias redundantes estan las dos en bb 27 y bb 28 (la cola).
 *
 * Por eso el `asm` inout de abajo: `__asm__("" : "+r"(halfLen), "+r"(pfir))` es
 * un `(set (reg))` de los dos pseudos, cuesta CERO bytes, y por estar en bb 28
 * mata el puente y las cinco copias `mr` de golpe. TIENE que ser UN SOLO asm
 * con los dos operandos y TIENE que estar en el bloque de la cola: en el bloque
 * del `if (filtType == 3)` (bb 27) da 944 B / 90,02 % / 57 filas, en los dos
 * 944 B / 88,93 % / 72, y dos asm sueltos en bb 28 dan 976 B / 88,97 % / 77.
 * Solo `halfLen` 956 B / 84,43 % / 120; solo `pfir` 960 B / 87,74 % / 87.
 * La forma de SOLO ENTRADA (`: : "r"(...)`) NO vale aqui: es un uso, no un
 * `set`, no toca `reg_first_set` --932 B / 87,84 % / 91 filas--.
 *
 * Las otras dos piezas, las dos necesarias:
 *   - `halfLen = 4;` delante del ultimo bucle. El asm deja halfLen opaco para
 *     cprop y el bucle final pasa de `cmpwi r31,4` a `cmpw` mas una guarda
 *     (+2 CMP, +1 BGT). Sin el: 944 B / 95,45 % / 28 filas. Escribir
 *     `cnt <= 4` en ese bucle da EL MISMO OBJETO (100 %), es cuestion de gusto.
 *   - `__asm__("" : : "f"(sum))` de SOLO ENTRADA detras de la resta: coloca la
 *     carga de $LC5 (0.0f) detras del `fsubs` y no delante. Cero bytes. Sin el
 *     936 B / 98,23 % / 7 filas. Un `__asm__ __volatile__("")` en su sitio
 *     tambien da 100 %.
 *   - el `__volatile__` del asm inout es la TERCERA pieza: sin el (asm inout no
 *     volatil) se queda en 936 B / 99,06 % / 5 filas, con el `fadds sum+sum`
 *     adelantado por delante del `lfsx`. Equivale a poner el inout no volatil
 *     mas una barrera de ranura `__asm__ __volatile__("")` delante (identico).
 *
 * Camino medido entero (scripts/fndiff.py, filas = lineas `>>>`):
 *   base ................................................ 940 B / 89,25214 % / 75
 *   asm inout (no volatil) solo en bb 28 ................ 940 B / 93,82478 % / 35
 *   + halfLen = 4 ....................................... 932 B / 97,24359 % / 11
 *   + `asm("" : "+r"(pfir))` dentro del `if (sum<0)` .... 936 B / 98,20513 % / 8
 *   + `asm("" : : "f"(sum))` ............................ 936 B / 99,05983 % / 5
 *   + barrera de ranura delante de la resta ............. 936 B / 100,0 %    / 0
 * y luego PODADO: el `+r`(pfir) del `if (sum<0)` sobra (sigue en 100 %) y la
 * barrera de ranura se funde en el inout haciendolo `__volatile__` (100 %).
 * Quedan DOS asm de cero bytes y una reasignacion; el tamano es exacto y el
 * diff es cero, asi que por la regla 9 del brief no es deuda.
 *
 * Tambien medido y peor sobre la base de 5 filas: `sum += sum - coef[halfLen]`
 * 99,08 % / 4 filas; `tmpFloat = pfir->coef[halfLen]` antes 99,08 % / 5;
 * partir la resta en dos 99,06 % / 5; `-(coef - (sum+sum))` 99,06 % / 5;
 * `asm("" : : "f"(pfir->coef[halfLen]))` delante 948 B / 34 filas;
 * anadir `"r"(pfir)` a la entrada 936 B / 35 filas.
 *
 * PARA EL RESTO DEL ARBOL: la firma de este caso es "al objetivo le FALTA un
 * bloque puente que nosotros tenemos, y le SOBRAN recalculos", y se confirma en
 * un segundo con `scripts/rtldump.py <unidad> <fn> -dG`, que imprime
 * `PRE: redundant insn N (expression E) in bb B` y `PRE/HOIST: end of bb B`.
 * El arreglo es un `asm` inout con TODOS los operandos de la expresion, EN EL
 * BLOQUE donde el volcado dice "redundant".
 *
 * ------------------------------------------------------------------------
 * Historia (todo lo de abajo se conserva porque documenta lo que NO era):
 *
 * calcFIRCoeffs: partial 89.25% (936/940 B; HEAD was 87.18/920). The tree
 * edit (float decls first, if/else abs fold) is the previous agent's live
 * work. VEDA (r2, 2026-09-04): the remaining 75 diffs are one INT rotation
 * rooted in OUR extra CSE cache of halfLen*4 (r28) placed on the
 * switch-default edge: target keeps halfLen in callee-saved r26 live
 * across the SNDI_sin/cos calls and recomputes slwi r26,2 at each use
 * (insns 39/202/210); ours caches halfLen*4+&coef (INSERTs at 33/34)
 * and rematerializes halfLen in scratch r11. Tried and identical/worse:
 * register halfLen, halfLen=sizeof(...)-1, halfLen=4 after the clamp,
 * the 96.3% if(sum>=0) form is semantically wrong (rejected). The true
 * fold form and the hamming-loop addressing are still hidden.
 *
 * r36e: sigue en pie, y ahora con la cuenta exacta. El deficit es UNA sola
 * instruccion: 5 INSERT reales contra 4 DELETE reales; el resto de los 21
 * INSERT/DELETE son reordenaciones emparejadas y las etiquetas del pool
 * ($LC5/$LC8 contra lbl_80412F7C/88), que no cuentan. Marco y preservados
 * coinciden: los dos hacen `lmw r26, 0x50(r1)`. Nosotros cacheamos DOS
 * valores vivos toda la funcion --halfLen*4 en r28 y pfir+0x20 en r29-- y el
 * objetivo los recalcula: `slwi r0,r26,2` mas `addi r11,r29,0x20` en los dos
 * usos de la cola (insns 202/203 y 210/211) y `addi r3,r29,0x20` en el bucle
 * final. Anadido al barrido y descartado, todo medido con scripts/tamfn.py:
 *   barrera selectiva sobre halfLen tras la asignacion (+44 B), antes del
 *   switch (+44), en las dos (+44), `volatile int halfLen` (+116), barrera
 *   sobre pfir antes de la cola (+40), sobre halfLen antes de la cola (+20),
 *   sobre las dos antes de la cola (+56).
 *   cflags: -fno-gcse (+36), -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *   -fno-expensive-optimizations y -fno-strength-reduce (los cuatro sin
 *   efecto, siguen en +4).
 * O sea que el cacheo no viene de gcse ni se rompe con la barrera: es cse1.
 * La barrera aqui NO vale, y eso confirma la regla --cierra adelantos del
 * planificador, no empates del asignador ni la CSE local.
 *
 * r46: recuento exacto del deficit, para no volver a contarlo. 11 INSERT y 10
 * DELETE; net +1 instruccion = +4 B. Los INSERT nuestros son `slwi r28,r11,2` y
 * `addi r29,r30,0x20` en la ARISTA DEFAULT del switch (indices 33/34) y tres
 * `mr r29,r10` --uno por case-- en 46/77/113; los DELETE del objetivo son
 * `slwi r0,r26,2` en 39/70/105/202/210 y `addi rN,r29,0x20` en 203/211. Es
 * decir: GCC nos crea un bloque puente en la arista default para que
 * `&pfir->coef[halfLen]` este disponible en la cola, y el objetivo lo recalcula
 * en cada uso. Cuatro formas de fuente NUEVAS, todas 940 B / 89,25214 %, o sea
 * el objeto IDENTICO a la base: `default: break;` en el switch, un puntero
 * local `float *coef = pfir->coef` inicializado antes del switch, quitar la
 * local muerta `fir_coef_var`, y recalcular la cola. EMPEORA partir
 * `sum = sum + sum - pfir->coef[halfLen]` en dos sentencias: 89,209404 %.
 *
 * r47 --- MECANISMO LOCALIZADO Y FUENTE DE GCC CITADA. El bloque puente es
 * PRE (gcse.c). El volcado -da lo imprime literal:
 *   PRE: redundant insn 533 (expression 7) in bb 27 / 565 in bb 28
 *   PRE/HOIST: end of bb 6/8/9/12/15, copying expression 7 y 8
 * con expression 7 = (ashift (reg halfLen) 2) y expression 8 = (plus (reg
 * pfir) 32). El objetivo NO hace ninguna de las dos: recalcula las cinco
 * `slwi r0,r26,2` y las cuatro `addi rN,r29,0x20`.
 * El interruptor esta en orig/prodg/NGC_GNU_SRC/NGC/gcc/gcse.c:737
 *   if (optimize_size) one_classic_gcse_pass(); else one_pre_gcse_pass();
 * y en gcse.c:1884  antic_p = ! optimize_size && oprs_anticipatable_p(...).
 * Es decir: con optimize_size gcse hace CPROP pero NO inserta. Y -Os
 * reproduce EXACTAMENTE los tres mnemonicos que nos separan --addi 13, mr 5,
 * slwi 11, los del objetivo-- pero rompe el reparto flotante (stfd 6/3,
 * lis 19/23, lfs 22/24, lfd 15/13, mfcr 2/1) via local-alloc.c:1506/1527 y
 * toplev.c:4419: 924 B / 57,97863 %. O sea que el objetivo NO se compilo con
 * -Os; PRE corrio igual que aqui y no encontro las expresiones.
 * Medido y negativo en r47 (todo contra 940 B / 89,25214 %):
 *   -Os 924 B/57,979 %; -fno-gcse 243 insns (mr y slwi CUADRAN pero CMP 17/10
 *   y se pierde el par mfcr/mtcrf); -fno-gcse con literales 4 en los limites
 *   de bucle 233 insns (mr, slwi, addi y BR cuadran; faltan mfcr/mtcrf y
 *   sobran 2 CMP); las cinco versiones de ProDG (3.5, 3.5b140, 3.7, 3.8.1,
 *   3.9.3) dan el MISMO objeto; `halfLen = 4` repetido tras el switch 944 B,
 *   `default: halfLen = 4; break;` 928 B pero pierde mfcr/mtcrf,
 *   `default: break;` primero identico, `int halfLen = 4` identico,
 *   `const int halfLen = 4` 900 B/77,41 %; pin `halfLen` a r26 (que es lo que
 *   dice el DWARF) 980 B/68,49 %, pin `cnt` a r31 920 B/79,08 %, los dos
 *   964 B/63,95 %; asm de barrera sobre halfLen dentro del if de la cola
 *   944 B, antes de la cola 956 B, los dos 944 B; sobre pfir 960/944 B;
 *   switch en la cola, `+=` explicito, orden de `sum`, `4 || 2` y la division
 *   sin llaves: los cinco IDENTICOS a la base.
 *
 * r47 --- FIDELIDAD: el DWARF (mw_dwarfdump.nothpp) da las locales del
 * original y son CINCO --sum f31, tmpFloat f29, halfLen r26, cnt r31,
 * fir_coef_var (sin registro)--: NO existe `halfTmpFloat`. Se ha quitado
 * escribiendo `... * 0.5f * tmpFloat` en los dos argumentos, y el objeto sale
 * BYTE-IDENTICO (940 B, sha256 de la funcion 03227a79de44c912). OJO: las
 * formas que agrupan `(tmpFloat * 0.5f)` --con o sin parentesis, con
 * `/ 2.0f`, con `(0.5f * tmpFloat)`-- bajan a 936 B pero NO por quitar el
 * puente: `fold` las reasocia a `((a-b)*0.5f)*tmpFloat` y el bucle del case 4
 * pierde el `fmuls f30,f29,f27` compartido, gana un `fmuls` y un preservado
 * flotante (87,05983 %). Los 936 B de esas formas son una COINCIDENCIA.
 *
 * ------------------------------------------------------------------------
 * scaf-audio r1 (2026-09-14): CERO-ANDAMIOS intentado, VEDA RECONFIRMADA.
 * 6 formas nuevas sobre la base cerrada (936 B / 100 % / 0 filas):
 *   F1 sin los DOS asm ......................... 940 B / 89,25214 % (base)
 *   F2 sin el asm f(sum) ....................... 932 B / 96,64530 % / 12 filas
 *      (la carga de $LC5 se adelanta delante del fsubs y cambia r9->r3)
 *   F3 F2 con `__builtin_fabssf(sum)` .......... 932 B / 96,64530 % (identico:
 *      fabs expande a la misma secuencia fcmpu/cror/bso/fneg)
 *   F5 asm inout -> escritura real halfLen = 4 . 932 B / 95,36325 % (mata la
 *      expression 7 de PRE pero la 8 (pfir+32) sigue viva y el fadds se adelanta)
 *   F6 F5 + fabs, CERO asm ..................... 932 B / 95,36325 % / 38 filas
 * El mecanismo de la r47/r48 no se ha movido: en fuente solo se puede escribir
 * `halfLen` en bb 28 (la expression 7); NO existe escritura C de `pfir` que
 * pliegue a cero y mate la expression 8, y la colocacion de $LC5 es sched2.
 * Los DOS asm emiten 0 B y valen el partido entero: SE QUEDAN. */
float SNDI_sin(float x);
float SNDI_cos(float x);

// total size: 0x3C (sfir8.c)
typedef struct SNDFIRSTATE {
    float history[8]; // offset 0x0, size 0x20
    float coef[5];    // offset 0x20, size 0x14
    float cutLow;     // offset 0x34
    float cutHigh;    // offset 0x38
} SNDFIRSTATE;

void calcFIRCoeffs(SNDFIRSTATE *pfir, int filtType) {
    float sum;
    float tmpFloat;
    int halfLen;
    int cnt;
    int fir_coef_var;

    halfLen = 4;
    if ((filtType == 3 || filtType == 4) && pfir->cutLow > 0.8f) {
        pfir->cutLow = 0.8f;
    }
    switch (filtType) {
    case 2:
        pfir->coef[halfLen] = pfir->cutHigh;
        for (cnt = 1; cnt <= halfLen; cnt++) {
            tmpFloat = cnt * 3.1415927f;
            pfir->coef[halfLen - cnt] = SNDI_sin(pfir->cutHigh * tmpFloat) / tmpFloat;
        }
        break;
    case 3:
        pfir->coef[halfLen] = pfir->cutLow;
        for (cnt = 1; cnt <= halfLen; cnt++) {
            tmpFloat = cnt * 3.1415927f;
            pfir->coef[halfLen - cnt] = SNDI_sin(pfir->cutLow * tmpFloat) / tmpFloat;
        }
        break;
    case 4:
        pfir->coef[halfLen] = pfir->cutHigh - pfir->cutLow;
        for (cnt = 1; cnt <= halfLen; cnt++) {
            tmpFloat = cnt * 3.1415927f;
            pfir->coef[halfLen - cnt] =
                2.0f * SNDI_sin((pfir->cutHigh - pfir->cutLow) * 0.5f * tmpFloat) *
                SNDI_cos((pfir->cutHigh + pfir->cutLow) * 0.5f * tmpFloat) / tmpFloat;
        }
        break;
    }
    tmpFloat = 0.7853982f;
    sum = 0.0f;
    for (cnt = 0; cnt <= halfLen; cnt++) {
        if (filtType == 2 || filtType == 4) {
            pfir->coef[cnt] *= 0.54f - SNDI_cos(tmpFloat * cnt) * 0.46f;
            sum += pfir->coef[cnt];
        } else if (filtType == 3) {
            pfir->coef[cnt] *= SNDI_cos(tmpFloat * cnt) * 0.46f - 0.54f;
            if ((cnt & 1) == 0) {
                sum += pfir->coef[cnt];
            } else {
                sum -= pfir->coef[cnt];
            }
        }
    }
    if (filtType == 3) {
        pfir->coef[halfLen] += 1.0f;
        sum += 1.0f;
    }
    sum = sum + sum - pfir->coef[halfLen];
    if (sum < 0.0f) {
        sum = -sum;
    }
    halfLen = 4;
    for (cnt = 0; cnt <= halfLen; cnt++) {
        pfir->coef[cnt] /= sum;
    }
}
