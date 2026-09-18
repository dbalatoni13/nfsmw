extern "C" {

static inline int MULT(int a, int b) {
    return (int)((((long long)(a) * (b)) + 32768) >> 16);
}

int idctinput[64];

int idctprescale[64] = {
    0x00002000, 0x00001712, 0x0000187E, 0x00001B37, 0x00002000, 0x000028BA, 0x00003B21, 0x000073FC,
    0x00001712, 0x000010A2, 0x000011A8, 0x0000139F, 0x00001712, 0x00001D5D, 0x00002AA1, 0x0000539F,
    0x0000187E, 0x000011A8, 0x000012BF, 0x000014D4, 0x0000187E, 0x00001F2C, 0x00002D41, 0x000058C5,
    0x00001B37, 0x0000139F, 0x000014D4, 0x00001725, 0x00001B37, 0x000022A3, 0x00003249, 0x000062A3,
    0x00002000, 0x00001712, 0x0000187E, 0x00001B37, 0x00002000, 0x000028BA, 0x00003B21, 0x000073FC,
    0x000028BA, 0x00001D5D, 0x00001F2C, 0x000022A3, 0x000028BA, 0x000033D6, 0x00004B42, 0x0000939F,
    0x00003B21, 0x00002AA1, 0x00002D41, 0x00003249, 0x00003B21, 0x00004B42, 0x00006D41, 0x0000D650,
    0x000073FC, 0x0000539F, 0x000058C5, 0x000062A3, 0x000073FC, 0x0000939F, 0x0000D650, 0x0001A463,
};

static int work[64];

static void IdctColumn(int *src, int *dest) {
    int t1, t2, t3, t4, t5, t6, t7, t8, t9;

    if ((src[1] | src[2] | src[3] | src[4] | src[5] | src[6] | src[7]) == 0) {
        dest[0] = src[0];
        dest[8] = src[0];
        dest[16] = src[0];
        dest[24] = src[0];
        dest[32] = src[0];
        dest[40] = src[0];
        dest[48] = src[0];
        dest[56] = src[0];
        return;
    }

    t8 = MULT(src[2] - src[6], 46341);
    t4 = src[1] + src[7];
    t5 = src[5] + src[3];
    t1 = src[1] - src[7];
    t7 = src[0] - src[4];
    t2 = src[5] - src[3];
    t3 = MULT(t2 + t1, 25080);
    t6 = MULT(t4 - t5, 46341);
    t4 = t5 + t4;
    t2 = MULT(t2, 35468);
    t9 = MULT(t1, 85627);
    t1 = t3 + t2;
    t5 = t6 + t1;
    t3 = t9 - t3;
    t4 = t4 + t3;
    t3 = t3 + t6;
    t9 = (src[2] + src[6]) + t8;
    t6 = src[0] + src[4];
    t2 = t6 - t9;
    t6 = t6 + t9;
    t9 = t7 - t8;
    t7 = t7 + t8;

    dest[0] = t6 + t4;
    dest[8] = t7 + t3;
    dest[16] = t9 + t5;
    dest[24] = t2 + t1;
    dest[32] = t2 - t1;
    dest[40] = t9 - t5;
    dest[48] = t7 - t3;
    dest[56] = t6 - t4;
}

/* UNSOLVED r44. Corrige el diagnostico r36f; ver r44-madidct-audit.md.
   IdctColumn mide 632/632 B (158 instrucciones), pero NO es puro reparto:
   el objetivo tiene dos `mr` mas y dos `lwz` menos que nuestra compilacion.
   IdctRow mide 496/516 B: el delta objetivo-fuente es `mtctr` +1, `mfctr` +2
   y `mr` +2. NO faltan cinco copias high-word: ambos lados tienen las cinco.
   El objetivo conserva src[6] en CTR (indices reales 8, 17, 90) y un acarreo
   en LR (65, 80); nosotros conservamos src[6] en LR y el acarreo en un GPR.
   Los indices de fndiff incluyen huecos de alineacion: no contarlos como codigo.
   Vedas anteriores: MULT macro deja Column en 612 B; las cuatro formas con
   temporal long long, suma separada y casts dan el mismo 496/632 de la base.
   La serie historica de guardas r14..r31 no cambio los 496 B de Row; no repetir
   cantidades sin inicializar. No demuestra que cualquier vida real sea neutra.
   R44 en sombra, con src6=src[6] real y sus dos usos: pin directo asm("ctr")
   falla en el backend al cargar memoria; input "c" compila a 504 B/39.87597%;
   inout "+c" da 512 B/44.279068%, un solo mfctr y dos parejas internas de LR.
   Ninguna reproduce las vidas del objetivo: ensayos descartados, sin ASM nuevo
   retenido. idctcompute sigue exacto y los datos permanecen sin cambios.

   R46 --- CTR Y LR VIVOS A LA VEZ, que era lo unico sin probar. SI se alcanza,
   y con el se alcanza el TAMANO EXACTO de IdctRow por primera vez. Receta:

     int src6 = src[6];                 // y src[6] -> src6 en los dos usos
     __asm__("" : "+c"(src6));          // tras las declaraciones
     ...
     t9 = (src[2] + src6) + t8;
     __asm__("" : "+l"(t9));            // detras de la sentencia de t9

   Da 516/516 B, 129 instrucciones contra 129, y 48,24031 % (la base es 496 B /
   124 instrucciones / 46,372093 %). Lo que queda del multiconjunto:
     lwz  objetivo  9 / nuestro 10      mr   objetivo 10 / nuestro  8
     mfctr objetivo 2 / nuestro  1      mflr objetivo  2 / nuestro  3
     mtlr objetivo  2 / nuestro  3
   O sea: falta la SEGUNDA recuperacion de CTR y sobra una vuelta por LR.
   Medido encima de esa base y PEOR: `"+c"` delante de cada uso, o arriba mas
   delante del segundo, 524 B / 40,32558 %; anadir `"+l"(t8)`, 520 B /
   47,372093 %; `"+l"(t6)` detras, identico (516 / 48,24031). Sueltos: `"+l"(t8)`
   solo 500 B / 49,155037 %, `"+l"(t9)` solo 524 B / 43,953487 %, `"+c"(src6)`
   solo 476 B / 31,891474 %.
   NO SE DEJA PUESTA: son dos asm y una local `src6` que el DWARF no nombra, a
   cambio de cero bytes. La receta esta aqui entera y se reaplica en un minuto.
   IdctColumn no se mueve en ninguno de los ensayos: 632 B / 50,449368 %.

   R47 --- RECETA REPRODUCIDA Y NO RETENIDA, Y LA CANTIDAD FANTASMA CERRADA.
   La receta de r46 se reproduce exacta (516/516 B, 48,24031 %), pero el diff
   sigue teniendo ~120 filas distintas de 154: no es cero ni de lejos, asi que
   sigue siendo deuda de asm por cero bytes y NO se deja puesta.
   Lo nuevo es el diagnostico, sacado de los VOLCADOS RTL propios (CPP.exe +
   cc1plus.exe con los cflags reales y `-dg -dl`). El `.greg` imprime
   `;; Register dispositions` y `;; Hard regs used`, y dicen esto:
     IdctColumn  -> 94 in 65 (LR), 102 in 66 (CTR), 106 in 68 (CR0)
     IdctRow     -> 147 in 65 (LR) y NADA en CTR; hard regs usados
                    0 3..12 14..31 65
   O sea: nuestro IdctRow mete UN pseudo en registro especial (LR) y el
   objetivo mete DOS (uno en CTR con dos lecturas y otro en LR). Las cinco
   instrucciones que nos faltan son exactamente `mtctr` x1 + `mfctr` x2 + dos
   `mr`. Y REG_ALLOC_ORDER de rs6000.h pone 66 (CTR) ANTES que 65 (LR)
   --`..., 12, 64, 66, 65, 73, 1, 2, 67, 76`--, asi que al segundo pseudo que
   no cabe en GPR le tocaria CTR sin pelear: lo que falta es PRESION, un valor
   vivo mas.
   LA CANTIDAD FANTASMA, BIEN FORMADA, ES NEGATIVA AQUI --y esto cierra la
   duda que dejo r44--. Con la forma buena (`register int g asm("rN");`
   `__asm__("" : "=r"(g));` arriba y `__asm__("" : "+r"(t6) : "r"(g));` delante
   de `dest[0]`, que SI toca un valor vivo y no la barre el DCE) se han medido
   los 24 registros r3..r12 y r14..r31 uno a uno y ocho combinaciones de dos a
   cinco guardas. **Los 32 ensayos siguen en 496 B** (unicas excepciones: r4
   da 500 B y la terna r23+r25+r29 da 512 B con 24,64 %). El fuzzy sube hasta
   52,99 % (r17) sin mover el tamano, que es justo la trampa del fuzzy.
   Conclusion: la presion no se sube desde la fuente en esta funcion; el eje de
   las guardas queda CERRADO, ahora si con la forma correcta.

   R48 --- EL EJE DEL ORDEN DE SENTENCIAS, BARRIDO Y NEGATIVO, Y DOS DATOS
   NUEVOS QUE ACOTAN EL PROBLEMA.

   1) El deficit de IdctRow contado por MNEMONICO sobre la base limpia (sin
      ningun asm) es EXACTAMENTE tres entradas y nada mas:
        mtctr  objetivo 1 / nuestro 0
        mfctr  objetivo 2 / nuestro 0
        mr     objetivo 10 / nuestro 8
      mflr y mtlr CUADRAN en la base (lo de "mflr 2/3, mtlr 2/3" de r46 era ya
      CON la receta puesta). O sea: al objetivo le sobra un pseudo entero en
      CTR y sus dos lecturas, y dos copias; lo demas es reparto.
   2) Las 13 ultimas instrucciones de las dos versiones son IDENTICAS EN FORMA
      (mismo orden de calculo y el mismo orden de stores, con dest[7] primero):
      el problema esta acotado al centro de la funcion.
   3) IdctColumn (632/632, tamano ya exacto) NO es reparto puro: el histograma
      da `lwz` 18/20 y `mr` 12/10. Nosotros RECARGAMOS de la pila lo que el
      objetivo mantiene en registro. Concretamente, en la cadena de `or` del
      atajo (`src[1]|...|src[7]`) los dos guardamos src[4] en 0x8(r1), pero el
      objetivo lo sigue teniendo en r5 y lo usa (`or r0,r0,r5`) mientras que
      nosotros hacemos `lwz r11,0x8(r1)`. Los dos lados usan LR **y** CTR en
      esta funcion, asi que el mecanismo "pseudo a registro especial" SI se
      alcanza en nuestra compilacion --lo que falta en IdctRow es presion, no
      la posibilidad--.
   4) EL MAPA DE LINEAS DEL ORIGINAL NO EXISTE PARA ESTA UNIDAD.
      symbols/debug_lines.txt tiene UNA sola entrada para todo el rango
      0x8034BAC4..0x8034C0A4:
        0x8034BAC4: D:/env/egami/rcmp/dev/source/decoder/cmn/madidct.cpp (line 73)
      mientras que maddec.cpp, su vecino, trae linea por instruccion. Es decir:
      las tres funciones de madidct.cpp se atribuyen a la linea 73. Eso encaja
      con que los cuerpos vinieran de UNA macro (o de una sola sentencia
      logica), y explicaria por que ninguna reordenacion de sentencias mueve
      nada: en el original no habia sentencias que ordenar. NO lo he
      perseguido; queda anotado porque es la pista mas concreta que hay.

   BARRIDO NUEVO de r48 sobre IdctRow (base 496 B / 46,372093 % / 137 filas),
   el eje que las vedas anteriores no habian tocado --el ORDEN de las
   sentencias--; todo medido con scripts/fndiff.py:
     bloque t8/t9/t6/t7 delante del bloque t1..t5 ..... 452 B / 24,66 % / 157
     solo `t8 = MULT(...)` arriba del todo ............ 496 B / 38,23 % / 143
     `t9 = src[2]+src[6]; t9 = t9 + t8;` (partido) .... 496 B / 48,92 % / 136
     `t9 = t8 + (src[2] + src[6]);` .................... 496 B / 44,19 % / 140
     `t6`/`t7` arriba del todo ........................ 504 B / 41,08 % / 143
     `t9 = (src[6] + src[2]) + t8;` ................... 496 B / 46,36 % / 138
     `t8` intercalado detras de los MULT del bloque 1 . 496 B / 46,37 % / 137
                                                        (OBJETO IDENTICO)
     solo `t6` arriba ................................. 504 B / 41,08 % / 143
     solo `t7` arriba ................................. 496 B / 51,30 % / 134
     `t6`/`t7` arriba + `t8` arriba ................... 508 B / 34,47 % / 149
     `t6`/`t7` arriba + `t9` partido .................. 504 B / 41,47 % / 143
     `t6`/`t7` arriba + la receta "+c"/"+l" ........... 516 B / 49,16 % / 138
   Ninguna baja de 134 filas y ninguna llega a 516 B sin la receta. Subir la
   presion moviendo `t6`/`t7` arriba SI cuesta 8 B (504) --es el unico eje de
   fuente que mueve el tamano-- pero coloca mal el resto.

   La receta de r46 reproducida otra vez en r48: 516/516 B, 129 instrucciones
   contra 129, 48,24031 %, 135 filas, y el `mtctr` cae EN LA MISMA RANURA que
   el objetivo (indice 8). Lo que le sobra a partir de ahi es la vuelta
   `mfctr` + `mtlr` de los indices 11 y 13: el objetivo lee CTR dos veces
   (mfctr 2) y nosotros lo sacamos a un GPR que el asignador acaba metiendo en
   LR (mflr 3 / mtlr 3 contra 2 y 2). Sigue sin dejarse puesta: cero bytes.  */

static void IdctRow(int *src, int *dest) {
    int t1, t2, t3, t4, t5, t6, t7, t8, t9;

    t4 = src[1] + src[7];
    t2 = src[5] - src[3];
    t7 = src[0] - src[4];
    t1 = src[1] - src[7];
    t3 = MULT(t2 + t1, 25080);
    t9 = MULT(t1, 85627);
    t5 = src[5] + src[3];
    t2 = MULT(t2, 35468);
    t1 = t3 + t2;
    t6 = MULT(t4 - t5, 46341);
    t8 = MULT(src[2] - src[6], 46341);
    t3 = t9 - t3;
    t4 = t5 + t4;
    t5 = t6 + t1;
    t4 = t4 + t3;
    t3 = t3 + t6;
    t9 = (src[2] + src[6]) + t8;
    t6 = src[0] + src[4];
    t2 = t6 - t9;
    t6 = t6 + t9;
    t9 = t7 - t8;
    t7 = t7 + t8;

    dest[0] = t6 + t4;
    dest[1] = t7 + t3;
    dest[2] = t9 + t5;
    dest[3] = t2 + t1;
    dest[4] = t2 - t1;
    dest[5] = t9 - t5;
    dest[6] = t7 - t3;
    dest[7] = t6 - t4;
}

void idctcompute(int *dest, int stride) {
    IdctColumn(&idctinput[0], &work[0]);
    IdctColumn(&idctinput[8], &work[1]);
    IdctColumn(&idctinput[16], &work[2]);
    IdctColumn(&idctinput[24], &work[3]);
    IdctColumn(&idctinput[32], &work[4]);
    IdctColumn(&idctinput[40], &work[5]);
    IdctColumn(&idctinput[48], &work[6]);
    IdctColumn(&idctinput[56], &work[7]);

    IdctRow(&work[0], dest);
    IdctRow(&work[8], dest + stride);
    IdctRow(&work[16], dest + stride * 2);
    IdctRow(&work[24], dest + stride * 3);
    IdctRow(&work[32], dest + stride * 4);
    IdctRow(&work[40], dest + stride * 5);
    IdctRow(&work[48], dest + stride * 6);
    IdctRow(&work[56], dest + stride * 7);
}

} // extern "C"
