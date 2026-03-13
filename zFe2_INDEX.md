# zFe2 Translation Unit Analysis - Complete Documentation

## Overview

Comprehensive context gathering for 10 functions in the zFe2 translation unit (Speed/Indep/SourceLists/zFe2). All functions analyzed with full assembly, DWARF debug info, memory offsets, and dependency tracking.

**Base Address:** `0x80142AC0`

## Documents Generated

### 1. **zFe2_FUNCTION_ANALYSIS.md** (636 lines)
Detailed analysis of all 10 functions with:
- Full assembly listings from decomp-diff
- DWARF debug information 
- Function addresses and offsets
- Description of logic and purpose
- Memory offset tables
- Structure definitions
- Function call dependencies

### 2. **zFe2_QUICK_REFERENCE.txt** (192 lines)
Quick lookup guide including:
- Function addresses and sizes
- Function signatures and parameters
- Critical memory offsets
- PowerPC calling conventions
- Register preservation patterns
- Stack frame sizes
- Mode values and special handling

### 3. **zFe2_ANALYSIS_COMPLETE.txt** (306 lines)
Executive summary with:
- Analysis scope and methodology
- Risk assessment matrix
- Technical findings and patterns
- Offset reference tables
- Register usage patterns
- Assembly patterns identified
- Decompilation notes and recommendations
- Integration checklist

---

## Function Index

| # | Function | Address | Size | Type | Category |
|---|----------|---------|------|------|----------|
| 1 | `PursuitData::ClearData` | 0x8015613C | 88B | Struct Method | Post-Race Data |
| 2 | `FEKeyboard::GetCase` | 0x801525A4 | 52B | Method | Keyboard Input |
| 3 | `FEKeyboard::AppendLetter` | 0x80152CB8 | 56B | Method | Keyboard Input |
| 4 | `FEKeyboard::ToggleCapsLock` | 0x8015300C | 92B | Method | Keyboard Input |
| 5 | `FEKeyboard::ToggleShift` | 0x80153068 | 76B | Method | Keyboard Input |
| 6 | `FEKeyboard::IsNumericSymbol` | 0x80152BF0 | 96B | Method | Keyboard Input |
| 7 | `FEKeyboard::Dispose` | 0x80151E70 | 176B | Method | Keyboard Input |
| 8 | `LanguageSelectScreen::NotificationMessage` | 0x8015AD00 | 32B | Virtual Override | UI Screens |
| 9 | `LanguageSelectScreen::LanguageSelectScreen` | 0x8015ABDC | 140B | Constructor | UI Screens |
| 10 | `LanguageSelectScreen::~LanguageSelectScreen` | 0x8015AC68 | 152B | Destructor | UI Screens |

---

## Key Data Structures

### FEKeyboard (0x360 bytes)
Keyboard input management for various text entry modes.

**Critical Offsets:**
- `0x3C`: `mbShift` (bool)
- `0x40`: `mbCaps` (bool)
- `0x324`: `mString` (char*)
- `0x334`: `mnMode` (enum MODE)

### PursuitData (0xAC bytes)
Race pursuit statistics collection.

**Critical Offsets:**
- `0x0`: `mPursuitIsActive` (bool)
- `0x28`: `mNumMilestonesThisPursuit` (int)
- `0x2C`: `mMilestonesCompleted[32]` (GMilestone** array)

### LanguageSelectScreen (0x170 bytes)
UI screen for language selection with scrolling icon list.

**Critical Offsets:**
- `0x28`: Virtual table
- `0x2C`: IconScroller (linked list)
- `0x16C`: StartedTimer (Timer struct)

---

## Analysis Methodology

### Data Extraction
1. **Assembly**: `python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zFe2 -d 'FUNCTION_NAME' --no-collapse`
2. **DWARF**: `python tools/lookup.py ./symbols/Dwarf function 0xADDRESS`
3. **Address Calculation**: Base (0x80142AC0) + Offset

### Source Files Reviewed
- `feKeyboardInput.hpp` - FEKeyboard class definition
- `FEpkg_MU_Keyboard.cpp` - Keyboard implementation
- `FEPkg_PostRace.hpp` - PursuitData struct
- `FEMenuScreen.hpp` - MenuScreen base class
- `FELanguageSelect.hpp` - LanguageSelectScreen class

---

## Critical Findings

### 1. Keyboard State Management
- **3 orthogonal flags**: shift, caps lock, special characters
- **6 keyboard modes**: All keys, alphanumeric, password, filename, email, profile
- **Mode-aware behavior**: Different toggles for different modes
- **Visual refresh**: UpdateVisuals() called after state changes

### 2. String Input Pipeline
- Max length: 0x9C bytes (156 chars)
- Cursor tracking and "first key" mode
- AppendLetter → GetLetterMap → AppendChar chain

### 3. Destruction Pattern
- Virtual table swapping before destruction
- Linked list iteration with virtual method dispatch
- Parent destructor called last

### 4. Resource Cleanup
- Dispose() must be called for proper cleanup
- Clears global gFEKeyboard and KeyboardActive
- Queues appropriate game messages

---

## Register Conventions

### PowerPC ABI (GameCube)
- **Arguments**: r3-r10
- **Return**: r3 (32-bit), r3:r4 (64-bit)
- **Preserved**: r13-r31
- **Volatile**: r0, r12, r3-r12

### Notable Patterns
- Functions preserve r30/r31 when needed
- Stack frames typically 0x8-0x18 bytes
- LR always saved/restored when calling other functions

---

## Quick Start Guide

### For Decomp Implementation

1. **Start with small functions** (GetCase, IsNumericSymbol)
2. **Match register allocation exactly** (esp. AppendLetter with r30)
3. **Handle boolean logic patterns** (subfic/adde negation)
4. **Implement dependencies first**: UpdateVisuals, GetLetterMap, AppendChar
5. **Test mode-aware branching** in ToggleCapsLock/ToggleShift

### For Integration

- Ensure all base classes available (MenuScreen, IconScrollerMenu)
- Verify message queue and global access
- Check Symbol array location (lbl_803E5D6C)
- RealTimer global must exist

### For Testing

- Test all keyboard modes (6 total)
- Verify dtor handles empty/populated icon lists
- Check Dispose path with bBack=true/false
- Validate global pointer cleanup

---

## Assembly Patterns

### Pattern 1: Boolean State Toggle
```asm
lwz r0, offset(r3)      # Load current
li rN, 1                # Prepare alternative
cmpwi r0, 1
bne skip
li rN, 0                # Invert if needed
skip: stw rN, offset(r3) # Store new
```

### Pattern 2: Loop with Counter
```asm
li r9, 0                # Counter = 0
loop:
  slwi r0, r9, 2        # Multiply by 4
  stwx r10, r11, r0     # Store
  addi r9, r9, 1        # Counter++
  cmpwi r9, limit       # Check limit
  ble loop              # Branch if <= limit
```

### Pattern 3: Virtual Method Dispatch
```asm
lwz r9, 0x58(r10)       # Load vtable
lha r3, 8(r9)           # Load offset
add r3, r10, r3         # Calculate address
lwz r0, 0xc(r9)         # Load function ptr
mtlr r0
blrl                    # Call virtual method
```

---

## References

### Files in Repository
- `tools/decomp-diff.py` - Assembly extraction tool
- `tools/lookup.py` - DWARF debug info lookup
- `symbols/Dwarf/` - DWARF debug information database
- All source files under `src/Speed/Indep/Src/Frontend/`

### Related Functions (Not Analyzed)
- `FEKeyboard::UpdateVisuals()` - Must be implemented
- `FEKeyboard::GetLetterMap(int)` - Called by AppendLetter
- `FEKeyboard::AppendChar(char)` - Called by AppendLetter
- `IconScrollerMenu::NotificationMessage()` - Parent implementation
- `IconScrollerMenu::RefreshHeader()` - Called by ctor
- `IconScroller::SetInitialPos()` - Called by ctor

---

## Document Statistics

| Document | Lines | Size | Focus |
|----------|-------|------|-------|
| FUNCTION_ANALYSIS.md | 636 | 23K | Detailed technical |
| QUICK_REFERENCE.txt | 192 | 6.1K | Quick lookup |
| ANALYSIS_COMPLETE.txt | 306 | 13K | Executive summary |
| INDEX.md | This file | - | Navigation guide |

**Total**: 1,134+ lines of comprehensive analysis

---

## Version History

- **v1.0**: Initial analysis complete (March 2024)
  - All 10 functions analyzed
  - Assembly and DWARF extracted
  - Memory offsets documented
  - Dependencies mapped

---

*This documentation is generated from decompilation analysis and PowerPC architecture study. Use as reference for decomp matching and implementation.*
