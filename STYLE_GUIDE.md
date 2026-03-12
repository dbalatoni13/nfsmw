# Code Style Guide for zAnim Decompilation

This guide describes the coding style to follow when writing decompiled code for this project.
It is derived by comparing **human-written** zAI source files against **AI-written** zAnim source files.

## 1. Cast Spacing

Use C++ casts **without** spaces inside the angle brackets.

```cpp
// CORRECT (human style)
static_cast<float>(value)
reinterpret_cast<char *>(ptr)
static_cast<unsigned int>(door)

// WRONG (AI style)
static_cast< float >(value)
reinterpret_cast< char * >(ptr)
static_cast< unsigned int >(door)
```

## 2. Comment Style

Comments should be **concise and purposeful**. Do not add comments that explain compiler matching
difficulties, assembly register differences, instruction scheduling, or other decompilation artifacts.
Those are machine concerns, not source concerns.

```cpp
// CORRECT — purposeful comment
// Functionally matching

// WRONG — machine concern comment
// NOTE: Stack layout mismatch - original compiler places ePoly at r1+0x28
// (before bVector2s at r1+0xD0) despite source declaration order, causing
// different register allocation (r24-r31 vs r28-r31). This 0x10 frame size
// difference cascades through all stack-relative instructions.
```

`// TODO` comments are acceptable when the intent is genuinely unclear or work is incomplete.

## 3. Inline Assembly

Inline assembly (`asm(...)`, `__asm__ volatile(...)`) is **acceptable** when it is required to reproduce
dead code sequences that the compiler cannot generate from source alone. This is a valid decompilation
technique, not a style violation.

```cpp
// Acceptable — matches compiler-generated dead compare
asm("cmpw 7, %0, %1" : : "r"(mThisInstanceNameHash), "r"(info->mParentInstanceNameHash) : "cr7");

// Acceptable — memory barriers to control instruction scheduling
__asm__ volatile("" : : : "memory");
```

## 4. Null Pointer

Use `nullptr` exclusively. Never use `NULL` or `0` as a null pointer.

## 5. Include Guards

Every header must use both the `#ifndef` guard **and** the `EA_PRAGMA_ONCE_SUPPORTED` block:

```cpp
#ifndef PATH_FILENAME_H
#define PATH_FILENAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
```

The guard name follows `SUBPATH_FILENAME_H` (all uppercase), e.g. `ANIMATION_ANIMSCENE_H`,
`AI_ACTIVITIES_AICOPMANAGER_H`.

## 6. Constructor Initializer Lists

Each initializer goes on its own line with a leading `, ` and a trailing `//` comment (empty) to
prevent clang-format from collapsing them to one line:

```cpp
CBasicCharacterAnimEntity::CBasicCharacterAnimEntity()
    : mDrawShadow(true),             //
      mTypeID(0),                    //
      mThisInstanceNameHash(0),      //
      mSpaceNode(nullptr),           //
      mWorldModel(nullptr) {}
```

## 7. Struct vs Class

Follow the `struct vs class` rule from the lookup skill: if the PS2 dump shows **no visibility
modifiers** → use `struct`; if any `public:`/`private:`/`protected:` appear → use `class`.

## 8. Member Variable Naming

Member variable naming reflects what the original DWARF says. Some classes use `mVariable`
(no underscore), others use `m_variable` (with underscore). Follow the DWARF dump. Do not
invent naming prefixes.

## 9. `using namespace`

Never add `using namespace` directives. Qualify all names fully.

## 10. Namespace-qualified Types

All types must be fully qualified at point of use:

```cpp
// CORRECT
UMath::Vector3 pos;
Sim::IActivity *act;

// WRONG
using namespace UMath;
Vector3 pos;
```

## 11. DECLARE_CONTAINER_TYPE

When using `UTL::Std::list` or `UTL::Std::vector` type aliases, pair them with
`DECLARE_CONTAINER_TYPE`:

```cpp
DECLARE_CONTAINER_TYPE(AICopManagerSpawnRequests);
typedef UTL::Std::list<SpawnCopRequest, _type_AICopManagerSpawnRequests> SpawnList;
```

## 12. Inline Functions in Headers

Define inline member functions in the header only when they are genuinely inlined in the binary
(confirmed by DWARF). Keep non-inlined functions as declarations with implementations in `.cpp`.

## 13. Offset Comments

When writing struct definitions, add offset and size comments matching the DWARF:

```cpp
UMath::Vector3 InitialPos; // offset 0x0, size 0xC
UMath::Vector3 InitialVec; // offset 0xC, size 0xC
char VehicleName[32];      // offset 0x18, size 0x20
bool InPursuit;            // offset 0x38, size 0x1
```
