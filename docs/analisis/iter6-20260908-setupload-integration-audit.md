# Iter6 — auditoría vecinal de la integración SetupNextLoad

`scratchpad/codex_20260908_iter6_setupload_integration/audit_neighbors.py`
comparó los objetos reales recompilados con las copias capturadas inmediatamente
antes de integrar la receta autorizada.

Resultado: **PASS**.

- `zEAXSound`: 997 símbolos `STT_FUNC` definidos; el único cuerpo distinto es
  `SetupNextLoad__14EAXAemsManager`. El objeto tiene 11.309 relocaciones
  runtime tanto antes como después: 43 están dentro del intervalo de
  `SetupNextLoad` y las **11.266 vecinas**, fuera de ese intervalo, son
  idénticas.
- `zEAXSound2`: 1.112 símbolos `STT_FUNC`; ningún cuerpo cambia. Sus 12.976
  relocaciones runtime y el objeto completo son byte-idénticos al baseline
  (`f782f885016cf9d3aaa909b89d18233e0b6a003c48674f51bad0fc0e2da488e6`).
- En ambas unidades son idénticos el inventario, metadatos y bytes de las cinco
  secciones `SHF_ALLOC`: `.text`, `.rodata`, `.data`, `.bss` y `.ctors`. En
  `zEAXSound` se enmascara sólo el intervalo de 1008 bytes de `SetupNextLoad`;
  todos los bytes anteriores y posteriores de `.text` coinciden.
- El cuerpo nuevo se relocaliza de forma independiente con sus 43 registros
  ELF y coincide en 1008/1008 bytes con `NFSMWRELEASE.ELF` en `0x800AB3A0`.
  SHA-256 común:
  `400f80d440bebe90653b74e5856666c451ccc6fe6655831270c193afdd4ce646`.

Las referencias locales que usan símbolo de sección más addend se normalizan a
su función u objeto contenedor más desplazamiento. No se eliminan sufijos de
nombres ni se normalizan símbolos globales de forma genérica.
