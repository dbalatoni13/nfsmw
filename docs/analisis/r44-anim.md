# R44 — helper directo de base en `RawStateChan.cpp`

## Resultado

No se cerró `FnRawStateChan::EvalState`. La única variante solicitada fue
claramente negativa y se restauró. No hubo segundo ajuste porque la primera
diferencia no dejó una palanca local compatible con el objetivo.

Estado de entrada y salida:

```text
EvalState: 93,50877 %, 456/456 B, 52 instrucciones distintas
zEagl4Anim: 4 de 318 funciones distintas, 5.008 B
```

## Variante medida

Se añadió temporalmente un helper `static inline` privado del `.cpp` con un
parámetro `int keyIdx` real, manteniendo el producto
`keyIdx * c->GetKeySize()` en cada caller. A diferencia del wrapper de R43, el
helper implementó directamente la selección de base mediante los accesores
públicos:

```cpp
if (c->GetNumFields() & 1)
    return reinterpret_cast<unsigned char *>(
        c->GetDecodeData() + c->GetNumFields());
return reinterpret_cast<unsigned char *>(
    c->GetDecodeData() + c->GetNumFields() + 1);
```

Resultado:

```text
66,89474 %, 492/456 B
```

La causa aparece en el primer bloque. El objetivo forma la base impar como
`addi offset,10; add base,c,offset`; el helper materializa primero
`addi tmp,c,10` y después suma el índice escalado a ese pseudo. La misma forma
se duplica en las ramas impar/par. Esto añade instrucciones, prolonga la vida
de la base y obliga al prólogo a salvar `r28` (`stmw r28`) donde el objetivo y
la base estable sólo salvan desde `r29`.

No se probó el ajuste opcional: corregir esa primera diferencia exigiría dejar
de usar los accesores públicos o forzar otra forma de la suma, contradiciendo
la hipótesis concreta. Tampoco se repitieron `GetKeyData(0)`, cache de
`keySize`, local `keyOffset`, pines ni barridos de registros.

## Verificación

El helper y todos los cambios de `EvalState` fueron retirados. Se restauró el
CRLF completo del fuente y se reconstruyó sólo `zEagl4Anim`.

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEagl4Anim
  -> 1 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> before/after idénticos; 4 de 318 distintas
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim
  -> 314 ok, 0 FALLA
python scripts/lcfix.py --check
  -> todas las entradas @lc están al día
python scratchpad/codex_r44_anim/final_gate.py
  -> PASS
```

La instantánea objdiff completa, no sólo los porcentajes, queda idéntica antes
y después. El objeto vuelve al SHA-256
`2F539FE182B004F2812BABC21436973E7108A4AD06225C570CE4115FA084296E` y
`RawStateChan.cpp` al SHA-256
`C9772865568CDB096661C6AF6707529209B18793252FFA36E02D542C4B5D3096`.

Artefactos: `scratchpad/codex_r44_anim/`. No se tocaron cabeceras,
configuración, splits ni flags; no se hizo `ninja`, staging ni commit.
