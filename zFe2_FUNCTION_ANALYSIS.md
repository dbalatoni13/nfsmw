# zFe2 Translation Unit Function Analysis

**Base Address:** 0x80142AC0

---

## 1. PursuitData::ClearData - 88B

### Assembly (from decomp-diff):
```asm
1367c | lis r9, lbl_803E5F30@ha    |
13680 | li r0, 0                   |
13684 | lfs f0, lbl_803E5F30@l(r9) |
13688 | addi r11, r3, 0x2c         |
1368c | stw r0, 0x28(r3)           |
13690 | li r9, 0                   |
13694 | stfs f0, 4(r3)             |
13698 | li r10, 0                  |
1369c | stw r0, 0(r3)              |
136a0 | stw r0, 8(r3)              |
136a4 | stw r0, 0xc(r3)            |
136a8 | stw r0, 0x10(r3)           |
136ac | stw r0, 0x14(r3)           |
136b0 | stw r0, 0x18(r3)           |
136b4 | stw r0, 0x1c(r3)           |
136b8 | stw r0, 0x20(r3)           |
136bc | slwi r0, r9, 2             |
136c0 | addi r9, r9, 1             |
136c4 | stwx r10, r11, r0          |
136c8 | cmpwi r9, 0x1f             |
136cc | ble 0x136bc                |
136d0 | blr                        |
```

### DWARF Info:
```c
// Range: 0x8015613C -> 0x80156194
// this: r3
void PursuitData::ClearData() {
    /* anonymous block */ {
        // Range: 0x8015613C -> 0x80156190
        int i; // r9
    }
}
```

### Address: 0x8015613C (offset: 0x1367c)

**Description:** Clears pursuit data by zeroing out the structure fields. Initializes mPursuitIsActive to false, mPursuitLength to 0.0f, and loops to zero all milestone pointers (32 entries).

---

## 2. FEKeyboard::GetCase - 52B

### Assembly (from decomp-diff):
```asm
   fae4 | lwz r0, 0x3c(r3) |
   fae8 | cmpwi r0, 0      |
   faec | beq 0xfb00       |
   faf0 | lwz r3, 0x40(r3) |
   faf4 | subfic r0, r3, 0 |
   faf8 | adde r3, r0, r3  |
   fafc | blr              |
   fb00 | lwz r0, 0x40(r3) |
   fb04 | li r3, 0         |
   fb08 | cmpwi r0, 0      |
   fb0c | beqlr            |
   fb10 | li r3, 1         |
   fb14 | blr              |
```

### DWARF Info:
```c
// Range: 0x801525A4 -> 0x801525D8
// this: r3
int FEKeyboard::GetCase() {}
```

### Address: 0x801525A4 (offset: 0xfae4)

**Description:** Returns the current case state of the keyboard. Checks mbShift at offset 0x3c and mbCaps at offset 0x40 to determine if text should be capitalized, returning -1 for lower, 0 for no change, or 1 for uppercase.

**Offsets from FEKeyboard struct:**
- 0x3c: mbShift
- 0x40: mbCaps

---

## 3. FEKeyboard::AppendLetter - 56B

### Assembly (from decomp-diff):
```asm
  101f8 | stwu r1, -0x10(r1)               |
  101fc | mflr r0                          |
  10200 | stmw r30, 8(r1)                  |
  10204 | stw r0, 0x14(r1)                 |
  10208 | mr r30, r3                       |
  1020c | bl FEKeyboard::GetLetterMap(int) |
  10210 | mr r4, r3                        |
  10214 | mr r3, r30                       |
  10218 | bl FEKeyboard::AppendChar(char)  |
  1021c | lwz r0, 0x14(r1)                 |
  10220 | mtlr r0                          |
  10224 | lmw r30, 8(r1)                   |
  10228 | addi r1, r1, 0x10                |
  1022c | blr                              |
```

### DWARF Info:
```c
// Range: 0x80152CB8 -> 0x80152CF0
// this: r30
void FEKeyboard::AppendLetter(int nButton /* r4 */) {}
```

### Address: 0x80152CB8 (offset: 0x101f8)

**Description:** Appends a letter to the keyboard input. Calls GetLetterMap(nButton) to get the character, then AppendChar() to add it to the string. Preserves r30 across the call.

**Function Calls:**
- FEKeyboard::GetLetterMap(int)
- FEKeyboard::AppendChar(char)

---

## 4. FEKeyboard::ToggleCapsLock - 92B

### Assembly (from decomp-diff):
```asm
  1054c | stwu r1, -0x8(r1)            |
  10550 | mflr r0                      |
  10554 | stw r0, 0xc(r1)              |
  10558 | lwz r9, 0x334(r3)            |
  1055c | cmpwi r9, 5                  |
  10560 | beq 0x10598                  |
  10564 | lwz r0, 0x40(r3)             |
  10568 | li r11, 1                    |
  1056c | cmpwi r0, 1                  |
  10570 | bne 0x10578                  |
  10574 | li r11, 0                    |
  10578 | li r10, 0                    |
  1057c | stw r11, 0x40(r3)            |
  10580 | cmpwi r9, 3                  |
  10584 | stw r10, 0x3c(r3)            |
  10588 | bne 0x10594                  |
  1058c | li r0, 1                     |
  10590 | stw r0, 0x40(r3)             |
  10594 | bl FEKeyboard::UpdateVisuals |
  10598 | lwz r0, 0xc(r1)              |
  1059c | mtlr r0                      |
  105a0 | addi r1, r1, 8               |
  105a4 | blr                          |
```

### DWARF Info:
```c
// Range: 0x8015300C -> 0x80153068
// this: r3
void FEKeyboard::ToggleCapsLock() {}
```

### Address: 0x8015300C (offset: 0x1054c)

**Description:** Toggles caps lock mode. Checks current mode (0x334) and if it's not MODE_PROFILE_ENTRY (5), toggles mbCaps at offset 0x40 and clears mbShift at offset 0x3c. For MODE_FILENAME (3), forces mbCaps on. Calls UpdateVisuals() to refresh display.

**Offsets:**
- 0x334: mnMode
- 0x3c: mbShift
- 0x40: mbCaps

---

## 5. FEKeyboard::ToggleShift - 76B

### Assembly (from decomp-diff):
```asm
  105a8 | stwu r1, -0x8(r1)            |
  105ac | mflr r0                      |
  105b0 | stw r0, 0xc(r1)              |
  105b4 | lwz r0, 0x3c(r3)             |
  105b8 | li r9, 1                     |
  105bc | cmpwi r0, 1                  |
  105c0 | bne 0x105c8                  |
  105c4 | li r9, 0                     |
  105c8 | lwz r0, 0x334(r3)            |
  105cc | stw r9, 0x3c(r3)             |
  105d0 | cmpwi r0, 3                  |
  105d4 | bne 0x105e0                  |
  105d8 | li r0, 0                     |
  105dc | stw r0, 0x3c(r3)             |
  105e0 | bl FEKeyboard::UpdateVisuals |
  105e4 | lwz r0, 0xc(r1)              |
  105e8 | mtlr r0                      |
  105ec | addi r1, r1, 8               |
  105f0 | blr                          |
```

### DWARF Info:
```c
// Range: 0x80153068 -> 0x801530B4
// this: r3
void FEKeyboard::ToggleShift() {}
```

### Address: 0x80153068 (offset: 0x105a8)

**Description:** Toggles shift mode. Toggles mbShift at offset 0x3c (inverts between 0 and 1). For MODE_FILENAME (3), forces shift off. Calls UpdateVisuals() to refresh display.

**Offsets:**
- 0x334: mnMode
- 0x3c: mbShift

---

## 6. FEKeyboard::IsNumericSymbol - 96B

### Assembly (from decomp-diff):
```asm
  10130 | stwu r1, -0x18(r1)          |
  10134 | lis r9, lbl_803E5D6C@ha     |
  10138 | addi r11, r1, 8             |
  1013c | lwz r10, lbl_803E5D6C@l(r9) |
  10140 | mr r7, r11                  |
  10144 | addi r9, r9, lbl_803E5D6C@l |
  10148 | lhz r8, 8(r9)               |
  1014c | lwz r0, 4(r9)               |
  10150 | stw r10, 8(r1)              |
  10154 | li r9, 0                    |
  10158 | stw r0, 4(r11)              |
  1015c | sth r8, 8(r11)              |
  10160 | lbzx r0, r7, r9             |
  10164 | extsb r0, r0                |
  10168 | cmpw r4, r0                 |
  1016c | bne 0x10178                 |
  10170 | li r3, 1                    |
  10174 | b 0x10188                   |
  10178 | addi r9, r9, 1              |
  1017c | cmplwi r9, 9                |
  10180 | ble 0x10160                 |
  10184 | li r3, 0                    |
  10188 | addi r1, r1, 0x18           |
  1018c | blr                         |
```

### DWARF Info:
```c
// Range: 0x80152BF0 -> 0x80152C50
// this: r3
bool FEKeyboard::IsNumericSymbol(char character /* r4 */) {
    // Local variables
    char symbols[10]; // r1+0x8

    /* anonymous block */ {
        // Range: 0x80152BF4 -> 0x80152C44
        int i; // r9
    }
}
```

### Address: 0x80152BF0 (offset: 0x10130)

**Description:** Checks if a character is a numeric symbol (0-9 or specific punctuation). Loads a symbol string from lbl_803E5D6C and loops through 9 entries comparing with the input character parameter. Returns true if found, false otherwise.

**Local Variable:**
- symbols[10]: Stack buffer at r1+0x8 containing the numeric symbol string

---

## 7. FEKeyboard::Dispose - 176B

### Assembly (from decomp-diff):
```asm
   f3b0 | stwu r1, -0x10(r1)                                 |
   f3b4 | mflr r0                                            |
   f3b8 | stmw r30, 8(r1)                                    |
   f3bc | stw r0, 0x14(r1)                                   |
   f3c0 | mr r31, r3                                         |
   f3c4 | mr. r30, r4                                        |
   f3c8 | beq 0xf3dc                                         |
   f3cc | lwz r3, 0x324(r31)                                 |
   f3d0 | li r4, 0                                           |
   f3d4 | li r5, 0x9c                                        |
   f3d8 | bl bMemSet                                         |
   f3dc | cmpwi r30, 1                                       |
   f3e0 | bne 0xf408                                         |
   f3e4 | lis r11, cFEng::mInstance@ha                       |
   f3e8 | lwz r9, 0x328(r31)                                 |
   f3ec | lwz r3, cFEng::mInstance@l(r11)                    |
   f3f0 | li r6, 0xff                                        |
   f3f4 | lwz r11, 0x48(r9)                                  |
   f3f8 | lwz r4, 0x330(r31)                                 |
   f3fc | lwz r5, 0xc(r11)                                   |
   f400 | bl cFEng::QueueGameMessage(unsigned int, char const *, unsigned int) |
   f404 | b 0xf428                                           |
   f408 | lis r11, cFEng::mInstance@ha                       |
   f40c | lwz r9, 0x328(r31)                                 |
   f410 | lwz r3, cFEng::mInstance@l(r11)                    |
   f414 | li r6, 0xff                                        |
   f418 | lwz r11, 0x48(r9)                                  |
   f41c | lwz r4, 0x32c(r31)                                 |
   f420 | lwz r5, 0xc(r11)                                   |
   f424 | bl cFEng::QueueGameMessage(unsigned int, char const *, unsigned int) |
   f428 | lis r9, cFEng::mInstance@ha                        |
   f42c | li r4, 1                                           |
   f430 | lwz r3, cFEng::mInstance@l(r9)                     |
   f434 | bl cFEng::QueuePackagePop(int)                     |
   f438 | li r0, 0                                           |
   f43c | lis r9, gFEKeyboard@ha                             |
   f440 | lis r11, KeyboardActive@ha                         |
   f444 | stw r0, gFEKeyboard@l(r9)                          |
   f448 | stw r0, KeyboardActive@l(r11)                      |
   f44c | lwz r0, 0x14(r1)                                   |
   f450 | mtlr r0                                            |
   f454 | lmw r30, 8(r1)                                     |
   f458 | addi r1, r1, 0x10                                  |
   f45c | blr                                                |
```

### DWARF Info:
```c
// Range: 0x80151E70 -> 0x80151F20
// this: r31
void FEKeyboard::Dispose(bool bBack /* r30 */) {
    // Range: 0x80151EA4 -> 0x80151EA4
    static inline struct cFEng * cFEng::Get() {}

    // Range: 0x80151EA4 -> 0x80151EA4
    inline struct FEPackage * FEPackage::GetParentPackage() {}

    // Range: 0x80151EA4 -> 0x80151EA4
    inline const char * FENode::GetName() const {}

    // Range: 0x80151EC8 -> 0x80151EC8
    static inline struct cFEng * cFEng::Get() {}

    // Range: 0x80151EC8 -> 0x80151EC8
    inline struct FEPackage * FEPackage::GetParentPackage() {}

    // Range: 0x80151EC8 -> 0x80151EC8
    inline const char * FENode::GetName() const {}

    // Range: 0x80151EE8 -> 0x80151EE8
    static inline struct cFEng * cFEng::Get() {}
}
```

### Address: 0x80151E70 (offset: 0xf3b0)

**Description:** Disposes of the keyboard. If bBack is true, clears the input string (0x324, 0x9c bytes). Queues game message based on bBack flag (different message pointers at 0x330 and 0x32c). Pops package from FE queue and clears global gFEKeyboard pointer and KeyboardActive flag.

**Offsets:**
- 0x324: mString
- 0x328: mThis (FEPackage)
- 0x32c: Decline message pointer
- 0x330: Accept message pointer

**Function Calls:**
- bMemSet
- cFEng::QueueGameMessage
- cFEng::QueuePackagePop

---

## 8. LanguageSelectScreen::NotificationMessage - 32B

### Assembly (from decomp-diff):
```asm
  18240 | stwu r1, -0x8(r1)                                  |
  18244 | mflr r0                                            |
  18248 | stw r0, 0xc(r1)                                    |
  1824c | bl IconScrollerMenu::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) |
  18250 | lwz r0, 0xc(r1)                                    |
  18254 | mtlr r0                                            |
  18258 | addi r1, r1, 8                                     |
  1825c | blr                                                |
```

### DWARF Info:
```c
// Range: 0x8015AD00 -> 0x8015AD20
// Overrides: MenuScreen
// this: r3
void LanguageSelectScreen::NotificationMessage(unsigned long msg /* r4 */, struct FEObject * obj /* r5 */, unsigned long param1 /* r6 */, unsigned long param2 /* r7 */) override {}
```

### Address: 0x8015AD00 (offset: 0x18240)

**Description:** Override of MenuScreen::NotificationMessage. Simply delegates to the parent class IconScrollerMenu::NotificationMessage with all parameters passed through (msg, obj, param1, param2).

**Function Calls:**
- IconScrollerMenu::NotificationMessage

---

## 9. LanguageSelectScreen::LanguageSelectScreen (ctor) - 140B

### Assembly (from decomp-diff):
```asm
  1811c | stwu r1, -0x10(r1)                                 |
  18120 | mflr r0                                            |
  18124 | stw r31, 0xc(r1)                                   |
  18128 | stw r0, 0x14(r1)                                   |
  1812c | mr r31, r3                                         |
  18130 | bl IconScrollerMenu::IconScrollerMenu(ScreenConstructorData *) |
  18134 | lis r9, LanguageSelectScreen virtual table@ha      |
  18138 | lwz r0, 0x14c(r31)                                 |
  1813c | li r11, 0                                          |
  18140 | addi r9, r9, LanguageSelectScreen virtual table@l  |
  18144 | stw r9, 0x28(r31)                                  |
  18148 | cmpwi r0, 0                                        |
  1814c | stw r11, 0x16c(r31)                                |
  18150 | beq 0x18170                                        |
  18154 | li r0, 1                                           |
  18158 | stw r11, 0x134(r31)                                |
  1815c | stw r0, 0x130(r31)                                 |
  18160 | lis r9, lbl_803E66A0@ha                            |
  18164 | stw r11, 0x13c(r31)                                |
  18168 | lfs f0, lbl_803E66A0@l(r9)                         |
  1816c | stfs f0, 0x124(r31)                                |
  18170 | addi r3, r31, 0x2c                                 |
  18174 | li r4, 0                                           |
  18178 | bl IconScroller::SetInitialPos(int)                |
  1817c | mr r3, r31                                         |
  18180 | bl IconScrollerMenu::RefreshHeader                 |
  18184 | lis r9, RealTimer@ha                               |
  18188 | mr r3, r31                                         |
  1818c | lwz r0, RealTimer@l(r9)                            |
  18190 | stw r0, 0x16c(r31)                                 |
  18194 | lwz r0, 0x14(r1)                                   |
  18198 | mtlr r0                                            |
  1819c | lwz r31, 0xc(r1)                                   |
  181a0 | addi r1, r1, 0x10                                  |
  181a4 | blr                                                |
```

### DWARF Info:
```c
// Range: 0x8015ABDC -> 0x8015AC68
// this: r31
LanguageSelectScreen::LanguageSelectScreen(struct ScreenConstructorData * sd /* r4 */) {
    // Range: 0x8015ABEC -> 0x8015ABEC
    inline Timer::Timer() {}

    // Range: 0x8015ABEC -> 0x8015AC30
    inline void IconScrollerMenu::SetInitialOption(int index) {
        // Range: 0x8015AC14 -> 0x8015AC30
        inline void IconScroller::StartFadeIn() {}
    }

    // Range: 0x8015AC30 -> 0x8015AC30
    inline struct Timer & Timer::operator=(const struct Timer & t) {}
}
```

### Address: 0x8015ABDC (offset: 0x1811c)

**Description:** Constructor for LanguageSelectScreen. Calls parent IconScrollerMenu constructor, sets virtual table pointer at offset 0x28, initializes StartedTimer at offset 0x16c with RealTimer value. Conditionally initializes animation parameters if header exists (0x14c). Sets initial scroller position to 0 and refreshes header.

**Offsets:**
- 0x28: vftable
- 0x14c: Header reference
- 0x16c: StartedTimer (Timer struct)
- 0x124-0x13c: Animation parameters
- 0x2c: IconScroller (parent)

**Function Calls:**
- IconScrollerMenu::IconScrollerMenu (parent ctor)
- IconScroller::SetInitialPos
- IconScrollerMenu::RefreshHeader

---

## 10. LanguageSelectScreen::~LanguageSelectScreen (dtor) - 152B

### Assembly (from decomp-diff):
```asm
  181a8 | stwu r1, -0x18(r1)                            |
  181ac | mflr r0                                       |
  181b0 | stmw r29, 0xc(r1)                             |
  181b4 | stw r0, 0x1c(r1)                              |
  181b8 | lis r9, IconScrollerMenu virtual table@ha     |
  181bc | lis r11, IconPanel virtual table@ha           |
  181c0 | mr r30, r3                                    |
  181c4 | addi r9, r9, IconScrollerMenu virtual table@l |
  181c8 | addi r11, r11, IconPanel virtual table@l      |
  181cc | stw r9, 0x28(r30)                             |
  181d0 | stw r11, 0x60(r30)                            |
  181d4 | addi r31, r30, 0x2c                           |
  181d8 | mr r29, r4                                    |
  181dc | lwz r10, 0(r31)                               |
  181e0 | cmpw r10, r31                                 |
  181e4 | beq 0x18220                                   |
  181e8 | lwz r11, 0(r10)                               |
  181ec | cmpwi r10, 0                                  |
  181f0 | lwz r9, 4(r10)                                |
  181f4 | stw r11, 0(r9)                                |
  181f8 | stw r9, 4(r11)                                |
  181fc | beq 0x181dc                                   |
  18200 | lwz r9, 0x58(r10)                             |
  18204 | li r4, 3                                      |
  18208 | lha r3, 8(r9)                                 |
  1820c | lwz r0, 0xc(r9)                               |
  18210 | add r3, r10, r3                               |
  18214 | mtlr r0                                       |
  18218 | blrl                                          |
  1821c | b 0x181dc                                     |
  18220 | mr r3, r30                                    |
  18224 | mr r4, r29                                    |
  18228 | bl MenuScreen::~MenuScreen(void)              |
  1822c | lwz r0, 0x1c(r1)                              |
  18230 | mtlr r0                                       |
  18234 | lmw r29, 0xc(r1)                              |
  18238 | addi r1, r1, 0x18                             |
  1823c | blr                                           |
```

### DWARF Info:
```c
// Range: 0x8015AC68 -> 0x8015AD00
// Overrides: MenuScreen
// this: r30
LanguageSelectScreen::~LanguageSelectScreen() override {
    // Range: 0x8015AC78 -> 0x8015ACEC
    // Overrides: MenuScreen
    inline IconScrollerMenu::~IconScrollerMenu() override {
        // Range: 0x8015AC78 -> 0x8015ACE0
        // Overrides: IconPanel
        inline IconScroller::~IconScroller() override {
            // Range: 0x8015AC78 -> 0x8015AC78
            inline FEScrollBar::~FEScrollBar() {}

            // Range: 0x8015AC78 -> 0x8015ACE0
            inline virtual IconPanel::~IconPanel() {
                // Range: 0x8015AC9C -> 0x8015ACE0
                inline void bTList<IconOption>::bTList(const int __in_chrg) {
                    // Range: 0x8015AC9C -> 0x8015AC9C
                    inline int bList::IsEmpty() {
                        // Range: 0x8015AC9C -> 0x8015AC9C
                        inline struct bNode * bNode::GetNext() {}
                    }

                    // Range: 0x8015ACA8 -> 0x8015ACA8
                    inline struct IconOption * bTList<IconOption>::RemoveHead() {
                        // Range: 0x8015ACA8 -> 0x8015ACA8
                        inline struct bNode * bList::RemoveHead() {
                            // Range: 0x8015ACA8 -> 0x8015ACA8
                            inline struct bNode * bList::GetHead() {
                                // Range: 0x8015ACA8 -> 0x8015ACA8
                                inline struct bNode * bNode::GetNext() {}
                            }

                            // Range: 0x8015ACA8 -> 0x8015ACA8
                            inline struct bNode * bNode::Remove() {
                                // Local variables
                                struct bNode * prev_node; // r9
                                struct bNode * next_node; // r11
                            }
                        }
                    }

                    // Range: 0x8015ACE0 -> 0x8015ACE0
                    inline bList::~bList() {
                        // Range: 0x8015ACE0 -> 0x8015ACE0
                        inline bNode::~bNode() {}
                    }
                }
            }
        }
    }
}
```

### Address: 0x8015AC68 (offset: 0x181a8)

**Description:** Destructor for LanguageSelectScreen. Calls parent destructors through the inheritance chain. Sets virtual table pointers to parent classes (IconScrollerMenu, IconPanel) at offsets 0x28 and 0x60. Iterates through the scroller list at offset 0x2c, removing and destructing each IconOption. Finally calls MenuScreen destructor.

**Offsets:**
- 0x28: vftable
- 0x60: IconPanel vftable
- 0x2c: IconScroller (doubly-linked list head)
- 0x58: Option vftable/vtable pointer

**Function Calls:**
- IconOption destructors (virtual)
- MenuScreen::~MenuScreen (parent dtor)

---

## Structure Offsets Summary

### PursuitData (0xAC bytes total):
- 0x0: mPursuitIsActive (bool)
- 0x4: mPursuitLength (float)
- 0x8: mNumCopsDamaged (int)
- 0xC: mNumCopsDestroyed (int)
- 0x10: mNumSpikeStripsDodged (int)
- 0x14: mNumRoadblocksDodged (int)
- 0x18: mCostToStateAchieved (int)
- 0x1C: mRepAchievedNormal (int)
- 0x20: mRepAchievedCopDestruction (int)
- 0x24: mExitToSafehouse (int)
- 0x28: mNumMilestonesThisPursuit (int)
- 0x2C: mMilestonesCompleted[32] (GMilestone* array)

### FEKeyboard (0x360 bytes total):
- 0x2C: mnLetterMapIndex (int)
- 0x30: mnCursorIndex (int)
- 0x34: mnMaxLength (int)
- 0x38: mbIsFirstKey (bool)
- 0x3C: mbShift (bool)
- 0x40: mbCaps (bool)
- 0x44: mbOnSpecialCharacters (bool)
- 0x48: mpInputString (FEString*)
- 0x4C: mpCursor (FEObject*)
- 0x50: mpTextBox (FEImage*)
- 0x54-0x107: mpKeyName[45] (FEString* array)
- 0x108-0x1BB: mpKeyNameShadow[45] (FEString* array)
- 0x1BC-0x26F: mpKeyButton[45] (FEObject* array)
- 0x270-0x323: mpKeyDisable[45] (FEObject* array)
- 0x324: mString (char*)
- 0x328: mThis (FEPackage*)
- 0x32C: mnDeclineHash (unsigned long) / message pointer
- 0x330: mnAcceptHash (unsigned long) / message pointer
- 0x334: mnMode (MODE enum)
- 0x338: mnWindowStartIdx (int)
- 0x33C: mpCursorTestString (FEString*)
- 0x340: mDisplayString[31] (char array)

### LanguageSelectScreen (0x170 bytes total):
- 0x28: vftable (FEObject vtable)
- 0x60: IconPanel vftable
- 0x2C: IconScroller (parent, contains linked list)
- 0x130-0x13C: Animation parameters
- 0x14C: Header reference
- 0x16C: StartedTimer (Timer struct)

