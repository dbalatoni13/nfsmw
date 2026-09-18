# r64b - lote libc-trig: los 40 "andamios" no eran andamios

Agente: libc-trig (r64b). Seis unidades ya promocionadas, todas en el enlace:
`kf_cos.c` `k_cos.c` `kf_sin.c` `k_sin.c` `sf_tanh.c` `sf_sinh.c`.

## 1. Censo: el lote esta mal clasificado

El encargo hablaba de "40 barreras en seis ficheros". Medido:

    plantilla __asm__("") (barrera) en las seis unidades ..... 0
    __asm__("nombre") (nombre de ensamblador) ................ 40

Los 40 son anotaciones de NOMBRE, no barreras:

    fichero      __asm__   que son
    kf_cos.c        9      9 definiciones de dato: one C6..C1 half qxc
    k_cos.c         9      9 definiciones de dato: one C6..C1 half qxc
    kf_sin.c        7      7 definiciones de dato: S6..S1 half
    k_sin.c         7      7 definiciones de dato: S6..S1 half
    sf_tanh.c       4      3 datos (one two mtwo) + 1 declaracion (sn_fabsf)
    sf_sinh.c       4      3 datos (half shuge one) + 1 declaracion (sn_fabsf)
                   ---
                    40     38 datos + 2 declaraciones

Contraste con el arbol entero (grep sobre src/**.c,cpp,h,hpp):
plantilla vacia 317, plantilla no vacia 1.114, de ellas 247 nombres de
ensamblador, 241 pines `register T x asm("frN")`. El total agregado de 687 del
encargo es aproximadamente correcto; el reparto POR LOTE no lo es: aqui el
100 % de las ocurrencias son nombres, y el censo las conto como barreras.

## 2. Censo de veredictos de regmap: 0 de 6 clasificables

    __kernel_cos   no encuentro "__kernel_cos" en el volcado original.
    __kernel_cosf  no encuentro "__kernel_cosf" en el volcado original.
    __kernel_sin   no encuentro "__kernel_sin" en el volcado original.
    __kernel_sinf  no encuentro "__kernel_sinf" en el volcado original.
    tanhf          aviso: "tanhf" no esta en la unidad sf_tanh del original;
                   esta en: zFoundation   (es el tanhf de CARP::ExprValType,
                   otra funcion: static, argumento union, 0x40 bytes)
    sinhf          idem

    ESTRUCTURA 0 / PERMUTACION 0 / REPARTO 0 / IDENTICO 0
    SIN DWARF ORIGINAL 6

`symbols/mw_dwarfdump.nothpp` no trae ninguna de las seis: la libm de SN entro
en el juego como biblioteca precompilada sin informacion de depuracion.
`symbols.txt` si les da nombre a las funciones (`__kernel_cos = .text:0x8039C348`)
pero a sus constantes no: salen como `lbl_804FF760`, es decir, en el objeto
original esos datos NO TIENEN SIMBOLO. **regmap no es aplicable a este lote.**

## 3. Lo que si esta medido: la forma fiel reproduce el codigo EXACTO

Los 38 andamios de dato son de la forma

    double one __asm__("lbl_804FF760") = 1.0;

La forma de fdlibm, en su rama `#else` de `__STDC__`, es

    static double one = 1.0;

Compilando las dos con los cflags reales de la unidad (`-O1 -gdwarf+ ... -O2 -x c`)
y comparando la secuencia de instrucciones normalizando SOLO el nombre del
simbolo:

    kf_cos    defs=9  instr base= 58  fiel= 58   IDENTICO
    k_cos     defs=9  instr base= 60  fiel= 60   IDENTICO
    kf_sin    defs=7  instr base= 42  fiel= 42   IDENTICO
    k_sin     defs=7  instr base= 42  fiel= 42   IDENTICO
    sf_tanh   defs=3  instr base= 67  fiel= 67   IDENTICO
    sf_sinh   defs=3  instr base= 83  fiel= 83   IDENTICO
                            ---      ---
                            352      352

**6 de 6, 352 instrucciones, identicas una a una, mismo registro y mismo orden.**
El unico cambio es el NOMBRE del simbolo (`one@sda21(0)` en vez de
`lbl_804FF760@sda21(0)`) y su enlace (local en vez de global).

Conclusion: en estas seis unidades el andamio de dato **no compensa nada de
codigo**. Lo unico que compra es el nombre `lbl_804FFxxx` que
`config/GOWE69/symbols.txt` exige. Es **deuda de configuracion, no de fuente**.

### 3.1 Y el `const` de fdlibm es lo que NO vale

La rama `#ifdef __STDC__` de fdlibm dice `static const double`. Medido sobre
`k_cos`:

    nuestro (globales con __asm__)     60 instrucciones
    static double,  SIN __asm__        60 instrucciones   <- identico
    static const double (fdlibm STDC)  73 instrucciones   <- +13

Con `const`, GCC 2.95 saca las constantes por partida doble: una copia en
`.sdata2` y ademas un pool de nueve entradas `.LC0..LC8` en `.rodata`, y cada
lectura pasa de un `lfd fN,lbl@sda21(0)` a la pareja `lis rM,.LCk@ha` +
`lfd fN,.LCk@l(rM)`, gastando r7/r8/r9/r11. El objetivo usa `@sda21`: sus
constantes viven en `.sdata`, que es ESCRIBIBLE. Es decir, la libm de SN se
compilo con la rama NO-`__STDC__` (o sencillamente sin `const`), y ese detalle
esta ahora medido, no supuesto.

## 4. Los dos andamios de declaracion (`sn_fabsf`): negativos medidos

`sf_tanh.c` y `sf_sinh.c` declaran

    extern float sn_fabsf(float x) __asm__("fabsf");

porque `fabsf` es builtin de GCC 2.95 y el objetivo LLAMA. Formas probadas:

    forma probada                               resultado
    extern float fabsf(float x);                fabs 1,1 (se pliega)   NEGATIVO
    extern float fabsf();  (K&R, fdlibm #else)  bl fabsf pero + crset cr1eq
                                                por llamada: .text 0x110 -> 0x118
                                                (+8 B)                 NEGATIVO
    float (*const pf)(float) = fabsf;           fabs 1,1 (se pliega)   NEGATIVO
    extern double fabsf(float x);               bl fabsf, pero devuelve
                                                double: no fiel        DESCARTADO
    extern float fabsf(double x);               bl fabsf + fadds 1,1,1,
                                                sin crset: MISMA secuencia,
                                                pero es una mentira de prototipo,
                                                no una mejora de fidelidad
                                                                       DESCARTADO

El unico que da el codigo del objetivo Y es fdlibm literal es la forma
prototipada correcta, y esa se pliega. Sin `-fno-builtin` (que esta en
`configure.py`, prohibido en este encargo) **el nombre de ensamblador es
irreducible**. Los dos andamios se quedan, y ahora con la razon medida.

Medida en el arbol, protocolo completo, variante K&R sobre `sf_tanh.c`:

    antes      88d25bf8af96985288e3dd43cdb518ef2aece19d
    despues    766341c6eb2d6508806f4acec087d63b9fded5ad   -> REVERTIDO
    restaurado 88d25bf8af96985288e3dd43cdb518ef2aece19d

## 5. Medida en el arbol del negativo principal (`k_cos.c`)

Variante fiel: `static double one = ...` etc., sin ningun `__asm__`.

    antes      322bf44e19fe575f234d3056feefe1b893d7bef4
    despues    2f43f105e7012309ff7a4c5c90848254f880cd2d   -> REVERTIDO
    restaurado 322bf44e19fe575f234d3056feefe1b893d7bef4

El `.text` sale de 0xF4 bytes en los dos casos y la tabla de simbolos pasa de
`lbl_804FF760 .. lbl_804FF7A0` (global) a `one C6 C5 C4 C3 C2 C1 half qxc`
(local). Es exactamente la diferencia esperada: solo el nombre.

## 6. AVISO METODOLOGICO: el criterio del sha1 rechaza hasta un comentario

Estas unidades se compilan con `-gdwarf+`. Las secciones `.line` y `.debug`
guardan el numero de linea de cada instruccion. Medido sobre `sf_sinh.c`
anadiendo **una sola linea de comentario** y nada mas:

    base            79a56cdc4954bce7e9676d55910f3afef5a634dc
    +1 comentario   beac4e2f47f5a54b0a45718498d3e28f068b8bbd
    revertido       79a56cdc4954bce7e9676d55910f3afef5a634dc

y el desensamblado de `.text` y `.sdata` de los dos objetos es **identico**:
solo cambia la depuracion, que el enlazador tira y que no toca el DOL.

Consecuencia para toda la tanda r64b: **"sha1 del .o identico" es MAS ESTRICTO
que "el DOL no se mueve"**. Rechaza cualquier refactor que anada o quite una
linea, aunque el codigo generado sea byte a byte el mismo. Si el experimento
quiere medir fidelidad de fuente, el criterio correcto es comparar
`.text`/`.rodata`/`.data`/`.sdata`/`.bss` (por ejemplo con `dtk elf disasm` de
los dos objetos), no el sha1 del fichero entero. Con el sha1 a secas, un agente
que solo documente su negativo con un comentario en el fuente ya ha "roto" su
unidad.

## 7. Veredicto: NO funciona aqui, y por una razon estructural

    andamios del lote ..................... 40   (0 barreras, 40 nombres)
    clasificables con regmap ............... 0
    retirados con .o identico .............. 0
    intentados en el arbol ................. 2   (k_cos fiel, sf_tanh K&R)
    formas de fuente probadas y medidas .... 8
    unidades cuyo CODIGO reproduce la
      forma fiel de fdlibm ................. 6 de 6 (352/352 instrucciones)

En este lote el andamio no tapa un hueco de codigo: **es el nombre del simbolo
en el objeto**. Y el nombre del simbolo forma parte del `.o`, asi que el propio
criterio de aceptacion del experimento hace imposible retirarlo, por
construccion, independientemente de lo fiel que sea la fuente. La hipotesis del
encargo ("nuestras barreras compensan temporales que el original no tiene") es
**falsa aqui, pero por el mejor motivo posible**: no hay barreras, y la forma de
fdlibm ya produce exactamente el codigo del objetivo.

Lo que si hay es una via abierta que NO es de fuente: si se les diera a esos
datos su nombre real (`one`, `C1`, `S1`, `half`, ...) en
`config/GOWE69/symbols.txt` en vez de `lbl_804FFxxx`, las 38 anotaciones de dato
de este lote caerian de golpe y el fuente quedaria identico al fdlibm publicado.
Esa es una tarea de configuracion, prohibida en este encargo, y es donde esta el
premio: 38 de 40.

## 8. Estado del arbol

Cero cambios. Los seis fuentes intactos, los seis `.o` en su sello de partida:

    kf_cos   071dae12c5155aa1bd25177d082dd5914718a4f6
    k_cos    322bf44e19fe575f234d3056feefe1b893d7bef4
    kf_sin   460a127f37cee995c81a67494090387bb9008633
    k_sin    beb18d23cebf67527fe48e7302c5fb3f72e2b50d
    sf_tanh  88d25bf8af96985288e3dd43cdb518ef2aece19d
    sf_sinh  79a56cdc4954bce7e9676d55910f3afef5a634dc

(`git status src/libc/` solo marca `ef_pow.c` y `sf_log10.c`, modificados antes
de empezar y ajenos a este lote.)

Nota de operacion: `build_direct.py` fallo con
`error: Could not open output file build/GOWE69/src/libc/<u>.o` en aproximadamente
1 de cada 4 invocaciones, dejando el `.o` ANTERIOR en su sitio y devolviendo
`0 ok, 1 fallidas`. Sin releer la linea de resultado, el sha1 que se mide es el
del objeto viejo. Toda medida de este informe se tomo con un bucle que reintenta
hasta ver `1 ok`.
