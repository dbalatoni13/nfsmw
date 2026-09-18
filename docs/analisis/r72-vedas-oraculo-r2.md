# r72: RenderFlaresOnCar CERRADA al 100% — el +1 ref como palanca universal

Ronda 2 de vedas con oráculo. **RenderFlaresOnCar (zWorld, 2.908 B) cae de 62
filas a 0 (100%)** con un cambio de una línea; epCalculate queda VEDADA con
cifras nuevas. Herramientas: banco+measure1/showdiff/rfoc_table de vedas_r2
(copiadas de vedas_oraculo), el oráculo DWARF y el fuente del compilador.

## 1. RenderFlaresOnCar (zWorld, 2.908 B): 62 → 0 filas

El diff DWARF (dw1ours) ya casa salvo la rotación position r16→r15,
flashHeadlights r15→r14. El .lreg del banco revela que GCSE YA unifica los
tres usos de `lbl_8040AD04` del bucle en UN pseudo @ha (1007) que nace
EXACTAMENTE donde el objetivo (bloque is_traffic_car) — el problema es puro
de ASIGNACIÓN:

- La pelea: 4 allocnos por {r14,r15,r16} (REG_ALLOC_ORDER de enteros
  DESCENDENTE: el de mayor prioridad se lleva r16; rs6000.h:932).
- Prioridad (global.c): `pri = floor_log2(n_refs)*n_refs/live*10000`.
- Nuestro estado: position 6/607=198 → **r16**; CR-image 573 3/183=164 → r15;
  CR-image 641 3/184=163 → r14; **@ha 7/994=141 → SPILL** → reload lo
  rematerializa TRES veces (filas 452/598/600) y el CR queda en r14.
- El objetivo es la MISMA lista con el @ha encima: @ha→r16, position→r15,
  CR573→r14 (comparte ventana con flashHeadlights 58,6), CR641→spill
  (mfcr r9+stw+lwz+mtcrf). El tamaño cuadra solo: -2 lis del remat, +2 insns
  del vuelco de CR.

**LA PALANCA**: subir el @ha de 7 a 8 refs (3*8/994=241 > 198). La 8ª
referencia NO puede venir de la zona de declaraciones (V1 medida:
`coplight_intensityB = lbl_8040AD04` — su (high) NO se une al del bucle, refs
siguen 7, y además rota todos los FPR: 105 filas). GCSE/CSE solo unen los
(high) DENTRO del bucle. La forma que cierra (V2): **partir el check de
preview en dos sitios de asignación**:

```cpp
if (preview_part_id == CARPARTID_BRAKELIGHT && is_brakelight != 0) {
    intensity = lbl_8040AD04;
} else if (preview_part_id == CARPARTID_HEADLIGHT && is_headlight != 0) {
    intensity = lbl_8040AD04;
}
```

El bucle gana una 4ª carga enlazada al pseudo @ha → 8 refs → gana r16 →
position cae a r15, flashHeadlights a r14, el CR se vuelca a pila y todo el
racimo (mfcr, stw de cr, hoist del lis) casa. Verificado con diff BASE→V2 del
.o del banco: SOLO cambia esta función. **zWorld+zWorld2: 291.776 → 294.684 B
(+2.908, 578→579 funciones al 100%)**.

Es la misma familia que Initialize (r71): el oráculo señala el registro
OBJETIVO, el asignador dice QUÉ cantidad hay que tocar, y la palanca de
fuente es ±1 referencia en el sitio que el pase de unificación (GCSE/CSE)
realmente enlaza — nunca donde uno esperaría.

## 2. epCalculateLocalDirectionalPOS16 (zEcstasy, 2.072 B): VEDA con cifras

Verificación pedida por el brief: la cascada **double YA está** (DWARF double
en dcrg/dcba/scba/l1/zero, lfd/stfd en .text); quedan 155 filas y el frame
+8. Aportaciones nuevas (tabla local-alloc del .lreg, "Register N in M"):

- **112/114 (specular_power/bias) están DERRAMADAS en los DOS builds** (viven
  en la ranura 0x8/0xc): las filas 68-71 son scratches de RELOAD, no el
  "empate de allocno" de r68c — aquel diagnóstico era del estado pre-r36b.
- Qtys largas del bucle (pri): 440 f6 5185 | 443(l1) f7 3448 | 509 f1 1518 |
  512 f5 909 | 506(zero) f30 882 | 515 f4 851 | 541 f3 816 | 518 f8 816 |
  544 f2 784. **Los f3/f2/f4 del objetivo son exactamente los picks de
  nuestras qtys 541/544/515**: la pelea es una PERMUTACIÓN de
  {dcrg,dcba,scba} sobre {f2..f5,f13} decidida por vidas de 88-102 insns que
  difieren de ~6 en 6.
- Nuestro scba vive 3 insns (pri 40000 → pick f13); el del objetivo ~90
  (sched1 iza su def ~85 insns) → pri ~850 → f4. Es exactamente la veda de
  r68e (sched1 ordena por LUID = orden de fuente).
- f31 lo retiene una qty DF de la cola double de my_fpow (6 refs/vida 3, pri
  40000): zero (PS, 6/136, 882) no puede ganarle por prioridad.
- Negativas de hoy: W1 `v = fClamp(v,...)` en my_fpow (la palanca ICEMover de
  r69, motivada por el `v//f7` del DWARF objetivo) IDENTICO — GCC pliega la
  copia; F3 `(int)(t0*pow+bias)` sin t1 IDENTICO; G1 swap pares psq_st 158
  (peor); G2 scba antes de scrg 156 (peor); H5 ternario hack_scale IDENTICO.

Con las ~500 medidas de r30/r36/r46/r47/r68 la función queda vedada en
93,305016 % / 155 filas / 2.072 B. zEcstasy intacto: 141.500/145.884.

## 3. Lección transferible

`pri = floor_log2(n_refs)*n_refs/live*10000` con desempate por nº de pseudo
(local) o nº de allocno (global), y REG_ALLOC_ORDER que fija QUÉ registro se
lleva cada posición del orden. Cerrar una rotación = calcular qué cantidad
necesita ±refs o ±live para cruzar el umbral del vecino, y dar esa variación
con una forma de fuente que el pase correcto (GCSE para (high)s entre bloques
dominados, CSE dentro de bloque, sched1 para vidas) enlace sin añadir
instrucciones. El +1 ref funciona cuando existe un sitio de unificación
(bucle, bloque dominado); fracasa cuando la vida la decide el planificador
(epCalculate) — ahí la fuente solo puede mover posiciones, y moverlas rompe
el árbol que ya casa.
