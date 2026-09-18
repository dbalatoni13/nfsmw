# r75 -- epCalculate / zEagl4Anim (Initialize, EvalState)

Estado al abrir: epCalculate 93,305016 % (155 filas), Initialize 99,7449 %,
EvalState 98,0702 % (18 filas). Unidades: zEcstasy y zEagl4Anim.

## Resultado principal: Initialize AL 100 %

`EAGL4::DynamicLoader::Initialize` (2352 B) pasa de 99,7449 % (8 filas) a
**100,0 %**. `.text` de zEagl4Anim: 99,98691 % -> 99,99221 %. Es la unica
funcion con codigo de esa unidad que quedaba por debajo de 100 ademas de
EvalState; ninguna regreso.

### Mecanica del cierre (fichero: src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp, bucle STRTAB)

Las 8 filas residuales eran todas el bloque memmove/0x7F/strcpy. Diagnostico
por registro: el objetivo encadena en r29 TRES valores (&s[2], t y
s+nameLength) y en r30 dos (type_separator, nameLength); nosotros repartiamos
{&s[2], nameLength} en r29 y {type_separator, t} en r30, y ademas el
`s+nameLength` del strcpy sala DETRAS del stbx y en forma s+(nameLength+1)
cuando el objetivo lo materializa ANTES del stbx como (s+nameLength)+1.

La solucion es la CADENA DE VARIABLE UNICA (misma leccion que la "cadena-l"
de otros cierres, ahora en su version puntero):

    char *tail = &s[2];                          // r29 nace
    char *type_separator = strstr(tail, TYPE_SEPARATOR);
    if (type_separator) {
        *type_separator = 0;
        strcpy(typebuf, tail);                   // r29 sigue
        tail = type_separator + strlen(TYPE_SEPARATOR);   // r29 reencarna (t)
        unsigned int nameLength = strlen(tail) + 1;       // r30 hereda
        memmove(s, tail, nameLength);
        tail = &s[nameLength];                   // r29 reencarna (s+nameLength)
        s[nameLength] = 0x7F;                    // stbx indexado (r28,r30)
        strcpy(tail + 1, typebuf);               // add ANTES del stbx, en r29
    }

Pasos medidos (banco vedas_oraculo, bench_eagl4supportdlopen.cpp):
  base (locales separadas t/&) ........ 8 filas / 99,7449 %
  solo la cadena parcial (&s[2]->t en `tail`)  4 filas / 99,6225 % (el
      s+nameLength quedaba tras el stbx y en r30)
  cadena completa (la escrita) ........ 0 filas / 100,0 %   <- APLICADO

Nota para el que vuelva: la asociatividad del strcpy (`&s[nL]+1`) YA ESTABA
medida como negativa en r47/r64 (16 filas) porque se aplicaba sin la cadena;
la cadena la vuelve positiva. Las formas de r47 "arreglan 4 y rompen 5"
precisamente porque reparten los mismos valores en locales distintas.

## EvalState (FnRawStateChan): 18 filas, veda ampliada con el mecanismo exacto

No se cerro, pero esta ronda deja el PORQUE completo, leido en los volcados
del compilador (-dr expand, -dj pre-loop, -dL loop.c, -dt cse2) sobre el banco.
La unica diferencia REAL son el `mr r4,r11` (brazo impar) contra nuestro
`add r4,r30,r11` y su domino de reparto (fila 19 `mr r3,r0` contra
`mr r12,r0` incluida).

Cadena de decisiones que produce nuestro objeto:
1. El EXPANDER emite las dos ramas del accesor como (c+(2nf+10))[+2] en TODOS
   los sitios; el ((2nf)+c)+12 de la cabecera del objetivo lo fabrica COMBINE
   (post-loop) reasociando constantes, y solo puede donde el def vive en el
   mismo bloque (cabecera). En el bucle el def esta en el preheader y no hay
   LOG_LINK: por eso la cabecera es ramificada en los DOS builds y no hay que
   "protegerla" con formas de fuente.
2. loop.c scan_loop mueve la base del brazo PAR (temporal de un solo set cuyo
   uso es un ADD: validate_replace_rtx falla porque addsi3 no acepta
   (plus (plus r r) c)) al preheader. La base del brazo IMPAR alimenta keyData
   (2 sets): ni movible ni emparejable -> se queda dentro. Ese es nuestro add.
3. El `mr` del objetivo exigiria un temporal impar con uso-copia. Todo
   temporal asi lo PLEGUA el camino de single-usage de scan_loop (loop.c:
   loop_has_call && reg_single_usage && no_labels_between_p; el bucle tiene la
   llamada a Decode, y reemplazar una COPIA por una expresion siempre valida).
   Medido esta ronda, todo identico o peor:
     a) `unsigned char *d` local en el brazo impar, keyData=d ... IDENTICO
        (el fold lo devuelve al estado base; verificado en el volcado .jump).
     b) d en dos ramas, keyData=d tras el join ............ IDENTICO.
     c) base incondicional + keyData+=2 si par ............ 54,96 % / 412 B.
     d) polaridad del if del accesor invertida ............ 61,47 % / 496 B.
4. Cota: cse2 PODRIA plegar el add impar contra la base hoistead (la expresion
   esta en el hash del preheader), pero cse2 sigue CAMINOS y salta la rama
   nf&1 hacia el join sin procesar el bloque fallthru del brazo impar (se ve
   en los "Processing block from..to" del volcado .cse2). La palanca, si
   alguien la busca, es de grafo de caminos de cse2, no de prioridades.

Todo esto queda escrito como comentario r75 en
src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp para no repetirlo.

## epCalculateLocalDirectionalPOS16: 93,305016 %, veda r72 re-verificada

- Banco reproducido exacto (93,305016 % / 155 filas / 2072 B / frame 0x178).
- DOS permutaciones de la cadena especular que ninguna ronda habia medido:
    swap de los dos ps_mul finales (scba*ssb antes de scrg*ssrg) -> 93,295364 %
    (una fila peor); swap de los dos ps_muls0(ssrg/ssb, specular_a) -> IDENTICO.
  Con esto la ordenacion de esa cadena queda agotada; anotado como r75 en
  EcstasyEx.cpp junto a la funcion.
- La veda r72 sigue en pie: alargar la vida de scba exige que sched1 suba su
  def, y la unica via nueva que se ocurre (subir la PRIORIDAD del def
  alargando su camino de latencia hacia abajo, en vez de moverlo) no sale de
  ninguna forma de fuente sin mover posiciones del arbol.

## Datos finales

  unidad      .text (antes -> despues)   funciones < 100 %
  zEagl4Anim  99,98691 -> 99,99221 %     EvalState 98,0702 % (unica)
  zEcstasy    99,89329 (sin cambio)      epCalculate 93,3050 / eProject 93,9701
                                         / UpdatePlatInfo 99,9609 %

UpdatePlatInfo: su unica fila es DIRECCION DE POOL, no codigo (los bytes de
`lis r9,0 / lfs f0,0(r9)` son identicos; el objetivo relocaliza a
lbl_803DECD0 en .rodata+0x1678 y nosotros a $LC819 en +0x17f0). Cerrarla
exige clavar el layout del pool de rodata de la unidad entera (match 2,93 %),
fuera de alcance.

eProject: sin avances; r63 cerro el eje "barrera x2" y r67 dejo escrito que
la forma sin asm necesita una palanca desconocida. No re-medido.

## Ficheros tocados

- src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp  (cierre Initialize + nota r75)
- src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp        (nota r75: mecanismo + 4 negativas)
- src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp          (nota r75: 2 permutaciones medidas)
- RawStateChan.h se toco para experimentos y quedo revertido byte a byte (git limpio).
