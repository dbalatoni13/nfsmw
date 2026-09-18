# `UMath::MultYRot` lleva un `r = m;` que sus dos hermanas no llevan

**Listo para ejecutar. No aplicado porque `UMath.h` lo ven 92 ficheros y había
seis agentes midiendo.** Es lo primero que hay que hacer en la ventana siguiente.

## El hallazgo

```c
inline void MultXRot(const Matrix4 &m, float a, Matrix4 &r) {
    MATRIX4_multxrot(&m, a, &r);          // sin copia
}
inline void MultYRot(const Matrix4 &m, float a, Matrix4 &r) {
    r = m;                                 // <-- SOLO ESTA
    MATRIX4_multyrot(&r, a, &r);
}
inline void MultZRot(const Matrix4 &m, float a, Matrix4 &r) {
    MATRIX4_multzrot(&m, a, &r);          // sin copia
}
```

**Medido por el agente de la ronda 22**: el DWARF prueba que el `GTrigger`
original llama a `MultYRot` —`lmap.py` atribuye los `mr r3,r27` / `mr r4,r27` a
`UMath.h:455`— y **con el `r = m;` la llamada cuesta 224 B de más (84,07 %);
sin él el objeto sale byte a byte idéntico**.

## Por qué está ahí, y por eso no basta con borrarlo

El **único** otro llamante es `src/Speed/Indep/Src/World/World.cpp:237`:

```c
UMath::Matrix4 rotMat;                                    // sin inicializar
UMath::MultYRot(UMath::Matrix4::kIdentity, rotInitialVec, rotMat);
```

Pasa `kIdentity` como fuente y `rotMat` **sin inicializar** como destino: depende
de la copia. Si se borra `r = m;` sin más, `MATRIX4_multyrot(&r, a, &r)` lee
memoria basura. **La copia hay que subirla al llamante**, que es además lo que
tuvo que hacer el original:

```c
UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
UMath::MultYRot(rotMat, rotInitialVec, rotMat);
```

## Cómo verificarlo (cabecera compartida: A/B por objetos, no md5)

1. `measure.py -o antes.json` sobre **las 92 unidades que incluyen `UMath.h`**
   (`grep -rl "Utility/UMath.h" src/`), con `build_direct.py` antes.
2. Aplicar los dos cambios: quitar `r = m;` de `MultYRot` y subir la copia a
   `World.cpp:237`.
3. `build_direct.py` de las 92 y `measure.py --cmp`. **Ninguna puede bajar.**
4. `pctsnap.py --cmp` además, porque una función puede caer 20 pp sin mover un
   byte.
5. `audit.py` en zGameplay y zWorld, dos pasadas.
6. Construcción completa y **`main.dol`**.

**El md5 del `.o` no vale** para el A/B: lleva el debug y cambia sin que cambie
una instrucción.

## Lo que se espera

224 B en `GTrigger::GTrigger` (zGameplay) y, si otras funciones inlinean
`MultYRot`, más. `World.cpp` no debería moverse: la copia sigue existiendo, sólo
cambia de sitio.
