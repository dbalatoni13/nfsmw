# `TAttrib<T>::Get` fuera de clase: 601 símbolos espurios en todo el árbol

**Medido el 2-sep-2026.** Es el mayor bloqueo de `linked` identificado hasta ahora,
y no mueve `matched_code` ni un byte: por eso llevaba escondido tantas rondas.

## El hecho

`src/Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h:656` declara

```cpp
template <typename T> class TAttrib : public Attribute {
    const TypeOf &Get(unsigned int index) const;      // <-- sólo la declaración
};
template <typename T> const T &TAttrib<T>::Get(unsigned int index) const { ... }
```

Al estar **fuera de la clase**, cada instanciación emite un símbolo. Resultado:

| | símbolos `Get__CQ26Attribt7TAttrib1Z…` |
|---|---|
| **el DOL entero** | **3** |
| **nuestras SourceLists** | **601** |

Y por unidad: zEAXSound 32, zFe2 32, zEAXSound2 31, zFe 29, zPhysicsBehaviors 29,
zAI 28, zFeOverlay 28, zMain 28, zSim 28, zSpeech 28, zWorld2 28, zGameplay 27…

## La prueba de que sobran

Comparando los `.o` extraídos con los nuestros:

```
zMisc      objetivo: def  0  und  0     nuestro: def 26  und 0
zAI        objetivo: def  3  und  0     nuestro: def 28  und 0
zEAXSound  objetivo: def  0  und  0     nuestro: def 32  und 0
```

**El objetivo no los define NI los referencia** salvo tres en zAI. O sea: en el
original `Get` está **inline** en todas partes.

## Medido con cabecera sombra sobre zMisc (sin tocar el árbol)

Metiendo el cuerpo **dentro de la clase**:

```
ACTUAL     matched 78.008 B   .text 86.368 B   símbolos Get: 26
EN CLASE   matched 78.008 B   .text 84.288 B   símbolos Get:  0
```

**`matched_code` no se mueve, `.text` baja 2.080 B y los 26 símbolos espurios
desaparecen.** Es exactamente lo que hace falta para promocionar.

## Por qué importa

`zMisc` casa al **100 %** y no se puede promocionar porque **exporta 106 símbolos
de más** (`.text` +8.360 B). **82 de esos 106 son `Get__…`** — se van solos con
este cambio. Los otros 24 son de la misma familia: funciones que el original tiene
**inline en clase** y nosotros fuera (`QuickSpline::CalibrateLength`,
`QuickSpline::ClampParam`, `AverageWindow::GetOldestValue`,
`DisculatorDriver::Get`, `IsCompressInPlaceData`…), y **ninguna existe en el DOL**
— comprobado contra `symbols.txt`.

## Qué falta antes de aplicarlo

1. **Medir las 33 SourceLists**, no sólo zMisc: es cabecera compartida por todas.
2. **Los 3 de zAI**: el comentario del header dice que la definición fuera de línea
   existe porque el `.o` objetivo de zAI define tres (`bool`, `UMath::Vector4`,
   `GCollectionKey`). Con el cuerpo en clase GCC ya no las emitirá; hay que
   reponerlas explícitamente si el objetivo las tiene. **Nadie las referencia**
   (`und 0` en los tres objetos mirados), así que no rompen el enlace.
3. Ejecutarlo en **ventana**, con el DOL verificado antes y después.

## Cómo se encontró

`promote.py` mejorado (dice **quién** referencia cada símbolo que falta) señaló que
zMisc exportaba 106 de más. Compilando **uno a uno los 28 ficheros** de su
SourceList con sus cflags exactos salió el reparto: `EasterEggs` 52, `Joylog` 30,
`QuickSpline` 22, `Rumble` 10, `Timer` 9, `Table` 7… y el patrón común: **un
símbolo que no existe en ninguna parte del DOL y que nosotros emitimos fuera de
línea estaba inline en el original**.
