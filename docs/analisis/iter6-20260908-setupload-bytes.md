# Iter6 — auditoría estricta de bytes de SetupNextLoad

Se añadió una comprobación independiente que no confía en el 100 % de
`objdiff`. `scratchpad/codex_20260908_iter6_setupload_refs/audit_bytes.py`
extrae los 1008 bytes de `SetupNextLoad` del objeto privado exacto, enumera las
reubicaciones ELF reales de `.rela.text`, resuelve todos sus destinos con las
direcciones del ELF original y aplica los campos PPC correspondientes.

La reconstrucción cubre 43 reubicaciones del cuerpo:

- 24 `R_PPC_REL14`;
- 13 `R_PPC_REL24`;
- 3 `R_PPC_ADDR16_HA`;
- 3 `R_PPC_ADDR16_LO`.

Las reubicaciones locales contra el símbolo de sección `.text` se resuelven
primero al símbolo funcional que contiene el addend y luego a su dirección en
el ELF original. Esto incluye destinos internos de `SetupNextLoad`, la llamada
recursiva, `InitiateLoad` y `RemoveBankListing`. Los símbolos externos y datos
(`g_SndAssetList`, `gFastMem`, `bMemSet` y `FastMem::Free`) se resuelven por su
dirección original.

El resultado reconstruido coincide byte por byte con el cuerpo situado en
`0x800AB3A0` del ELF original: **1008/1008 bytes idénticos, cero diferencias**.
El auditor exige además que el objeto sin relocar sí difiera, para evitar una
prueba vacía, y fija el SHA-256 del objeto privado auditado.

No se compiló ninguna variante nueva y producción no se modificó.
