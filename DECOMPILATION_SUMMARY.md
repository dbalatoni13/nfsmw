# Ghidra Decompilation Results - zFeOverlay Translation Unit

## Summary
Successfully decompiled all 21 functions from the zFeOverlay translation unit on 2024-03-14.

---

## Function Results

### 1. SelectablePart::GetPart
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C520C
- **Mangled Name**: `GetPart__14SelectablePart`
- **Signature**: `inline struct CarPart * SelectablePart::GetPart()`
- **Ghidra Decompile**:
```c
undefined4 GetPart__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 8);
}
```

### 2. SelectablePart::GetPartState
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C5234
- **Mangled Name**: `GetPartState__14SelectablePart`
- **Signature**: `inline enum eCustomizePartState SelectablePart::GetPartState()`
- **Ghidra Decompile**:
```c
undefined4 GetPartState__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x1c);
}
```

### 3. SelectablePart::GetPhysicsType
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C5224
- **Mangled Name**: `GetPhysicsType__14SelectablePart`
- **Signature**: `inline enum Type SelectablePart::GetPhysicsType()`
- **Ghidra Decompile**:
```c
undefined4 GetPhysicsType__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x14);
}
```

### 4. SelectablePart::GetPrice
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C523C
- **Mangled Name**: `GetPrice__14SelectablePart`
- **Signature**: `inline int SelectablePart::GetPrice()`
- **Ghidra Decompile**:
```c
undefined4 GetPrice__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x20);
}
```

### 5. SelectablePart::GetSlotID
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C5214
- **Mangled Name**: `GetSlotID__14SelectablePart`
- **Signature**: `inline int SelectablePart::GetSlotID()`
- **Ghidra Decompile**:
```c
undefined4 GetSlotID__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0xc);
}
```

### 6. SelectablePart::GetUpgradeLevel
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C521C
- **Mangled Name**: `GetUpgradeLevel__14SelectablePart`
- **Signature**: `inline unsigned int SelectablePart::GetUpgradeLevel()`
- **Ghidra Decompile**:
```c
undefined4 GetUpgradeLevel__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x10);
}
```

### 7. SelectablePart::IsJunkmanPart
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C5244
- **Mangled Name**: `IsJunkmanPart__14SelectablePart`
- **Signature**: `inline bool SelectablePart::IsJunkmanPart()`
- **Ghidra Decompile**:
```c
undefined4 IsJunkmanPart__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x24);
}
```

### 8. SelectablePart::IsPerformancePkg
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C522C
- **Mangled Name**: `IsPerformancePkg__14SelectablePart`
- **Signature**: `inline bool SelectablePart::IsPerformancePkg()`
- **Ghidra Decompile**:
```c
undefined4 IsPerformancePkg__14SelectablePart(int param_1) {
  return *(undefined4 *)(param_1 + 0x18);
}
```

### 9. CustomizeMainOption::IsStockOption
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C5464
- **Mangled Name**: `IsStockOption__19CustomizeMainOption`
- **Signature**: `inline virtual bool CustomizeMainOption::IsStockOption()`
- **Ghidra Decompile**:
```c
undefined4 IsStockOption__19CustomizeMainOption(void) {
  return 0;
}
```

### 10. SetStockPartOption::IsStockOption
- **Status**: Missing in decomp output (8 bytes)
- **Address**: 0x803C54EC
- **Mangled Name**: `IsStockOption__18SetStockPartOption`
- **Signature**: `inline bool SetStockPartOption::IsStockOption() override`
- **Ghidra Decompile**:
```c
undefined4 IsStockOption__18SetStockPartOption(void) {
  return 1;
}
```

### 11. CustomizationScreen::GetSelectedPart
- **Status**: Missing in decomp output (12 bytes)
- **Address**: 0x803C5614
- **Mangled Name**: `GetSelectedPart__19CustomizationScreen`
- **Signature**: `inline virtual struct SelectablePart * CustomizationScreen::GetSelectedPart()`
- **Ghidra Decompile**:
```c
undefined4 GetSelectedPart__19CustomizationScreen(int param_1) {
  return *(undefined4 *)(*(int *)(param_1 + 0x34) + 0x5c);
}
```

### 12. CustomizePaint::GetSelectedPart
- **Status**: Missing in decomp output (12 bytes)
- **Address**: 0x803C5988
- **Mangled Name**: `GetSelectedPart__14CustomizePaint`
- **Signature**: `inline struct SelectablePart * CustomizePaint::GetSelectedPart() override`
- **Ghidra Decompile**:
```c
undefined4 GetSelectedPart__14CustomizePaint(int param_1) {
  return *(undefined4 *)(*(int *)(param_1 + 0x26c) + 0x24);
}
```

### 13. CustomizeMeter::SetCurrent
- **Status**: Missing in decomp output (32 bytes)
- **Address**: 0x803B3F7C
- **Mangled Name**: `SetCurrent__14CustomizeMeterf`
- **Signature**: `void CustomizeMeter::SetCurrent(float current /* f1 */)`
- **Ghidra Decompile**:
```c
void SetCurrent__14CustomizeMeterf(double param_1, float *param_2) {
  double dVar1;
  
  if ((float)(param_1 - (double)*param_2) < 0.0) {
    param_1 = (double)*param_2;
  }
  dVar1 = (double)param_2[1];
  if ((float)(param_1 - (double)param_2[1]) < 0.0) {
    dVar1 = param_1;
  }
  param_2[2] = (float)dVar1;
  return;
}
```

### 14. CustomizeMeter::SetPreview
- **Status**: Missing in decomp output (40 bytes)
- **Address**: 0x803B3F9C
- **Mangled Name**: `SetPreview__14CustomizeMeterf`
- **Signature**: `void CustomizeMeter::SetPreview(float preview /* f1 */)`
- **Ghidra Decompile**:
```c
void SetPreview__14CustomizeMeterf(double param_1, float *param_2) {
  double dVar1;
  
  if ((float)(param_1 - (double)*param_2) < 0.0) {
    param_1 = (double)*param_2;
  }
  param_2[4] = param_2[3];
  dVar1 = (double)param_2[1];
  if ((float)(param_1 - (double)param_2[1]) < 0.0) {
    dVar1 = param_1;
  }
  param_2[3] = (float)dVar1;
  return;
}
```

### 15. _SetQRMode
- **Status**: Missing in decomp output (12 bytes)
- **Address**: 0x803A7988
- **Mangled Name**: `_SetQRMode__Fi`
- **Signature**: `static void _SetQRMode(int mode /* r3 */)`
- **Ghidra Decompile**:
```c
void _SetQRMode__Fi(undefined4 param_1) {
  QRMode = param_1;
  return;
}
```

### 16. GetCarTypeInfo
- **Status**: Missing in decomp output (20 bytes)
- **Address**: 0x803C51C4
- **Mangled Name**: `GetCarTypeInfo__F7CarType`
- **Signature**: `inline struct CarTypeInfo * GetCarTypeInfo(enum CarType car_type)`
- **Ghidra Decompile**:
```c
int GetCarTypeInfo__F7CarType(int param_1) {
  return CarTypeInfoArray + param_1 * 0xd0;
}
```

### 17. QRCarSelectBustedManager::ShowImpoundedTexture
- **Status**: Missing in decomp output (28 bytes)
- **Address**: 0x803AA294
- **Mangled Name**: `ShowImpoundedTexture__24QRCarSelectBustedManager`
- **Signature**: `bool QRCarSelectBustedManager::ShowImpoundedTexture()`
- **Ghidra Decompile**:
```c
undefined4 ShowImpoundedTexture__24QRCarSelectBustedManager(int *param_1) {
  if (*(char *)(*param_1 + 4) != '\0') {
    return 1;
  }
  return 0;
}
```

### 18. CarCustomizeManager::CanInstallJunkman
- **Status**: Missing in decomp output (36 bytes)
- **Address**: 0x803B34DC
- **Mangled Name**: `CanInstallJunkman__19CarCustomizeManagerQ37Physics8Upgrades4Type`
- **Signature**: `bool CarCustomizeManager::CanInstallJunkman(enum Type type /* r4 */)`
- **Ghidra Decompile**:
```c
void CanInstallJunkman__19CarCustomizeManagerQ37Physics8Upgrades4Type(int param_1) {
  CanInstallJunkman__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type(param_1 + 8);
  return;
}
```

### 19. CarCustomizeManager::GetNumPackages
- **Status**: Missing in decomp output (36 bytes)
- **Address**: 0x803B15F0
- **Mangled Name**: `GetNumPackages__19CarCustomizeManagerQ37Physics8Upgrades4Type`
- **Signature**: `int CarCustomizeManager::GetNumPackages(enum Type type /* r4 */)`
- **Ghidra Decompile**:
```c
void GetNumPackages__19CarCustomizeManagerQ37Physics8Upgrades4Type(int param_1) {
  GetMaxLevel__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type(param_1 + 8);
  return;
}
```

### 20. CarCustomizeManager::IsCareerMode
- **Status**: Missing in decomp output (48 bytes)
- **Address**: 0x803B3500
- **Mangled Name**: `IsCareerMode__19CarCustomizeManager`
- **Signature**: `bool CarCustomizeManager::IsCareerMode()`
- **Ghidra Decompile**:
```c
undefined4 IsCareerMode__19CarCustomizeManager(void) {
  if (((*(uint *)(FEDatabase + 0x1c0) & 1) == 0) && (g_bTestCareerCustomization == 0)) {
    return 0;
  }
  return 1;
}
```

### 21. CarCustomizeManager::IsHeroCar
- **Status**: Missing in decomp output (48 bytes)
- **Address**: 0x803B3838
- **Mangled Name**: `IsHeroCar__19CarCustomizeManager`
- **Signature**: `bool CarCustomizeManager::IsHeroCar()`
- **Ghidra Decompile**:
```c
bool IsHeroCar__19CarCustomizeManager(int param_1) {
  int iVar1;
  
  iVar1 = GetType__11FECarRecord(*(undefined4 *)(param_1 + 4));
  return iVar1 == 0x29;
}
```

---

## Statistics
- **Total Functions**: 21
- **Successfully Decompiled**: 21 (100%)
- **Total Bytes**: 496 bytes
- **Average Function Size**: ~23.6 bytes

## Notes
- All functions are marked as "missing in decomp output" which indicates these are implementations that need to be verified against the original source code.
- The majority of these functions are small getter/setter methods or simple property accessors.
- Several functions involve calling out to external APIs (e.g., `CanInstallJunkman`, `GetNumPackages`, `GetType__11FECarRecord`).
- Floating-point operations are used in `SetCurrent` and `SetPreview` functions.
