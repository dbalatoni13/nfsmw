#include "feWidget.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstdio>
#include <cstring>

FEWidget::FEWidget(FEObject *backing, bool enabled, bool hidden)
    : vTopLeft(0.0f, 0.0f), vSize(0.0f, 0.0f), vBackingOffset(-276.0f, -10.0f), pBacking(backing), bEnabled(enabled), bHidden(hidden),
      bMovedLastUpdate(false) {}

FEButtonWidget::FEButtonWidget(bool enabled) : FEWidget(nullptr, enabled, false), vMaxTitleSize(0.0f, 0.0f), pTitle(nullptr) {}

void FEButtonWidget::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {
    if (FEngTestForIntersection(mouse_x, mouse_y, GetTitleObject())) {
        Act(parent_pkg, __BUTTON_PRESSED__);
    }
}

void FEButtonWidget::Position() {
    uint32 alignment = GetTitleObject()->Format;
    if ((alignment & FESTRING_JUSTIFY_HCENTER) != 0) {
        FEngSetCenter(GetTitleObject(), GetTopLeftX(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else if ((alignment & FESTRING_JUSTIFY_HRIGHT) != 0) {
        FEngSetCenter(GetTitleObject(), GetTopLeftX() + GetMaxTitleWidth(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else {
        FEngSetCenter(GetTitleObject(), GetTopLeftX() + GetMaxTitleWidth() * 0.5f, GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    }

    if (GetBacking() != nullptr) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEButtonWidget::Show() {
    FEngSetVisible(pTitle);
    if (GetBacking() != nullptr) {
        FEngSetVisible(GetBacking());
    }
}

void FEButtonWidget::Hide() {
    FEngSetInvisible(pTitle);
    if (GetBacking() != nullptr) {
        FEngSetInvisible(GetBacking());
    }
}

void FEButtonWidget::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), FEHASH_HIGHLIGHT, true);
    if (GetBacking() != nullptr) {
        FEngSetScript(GetBacking(), FEHASH_HIGHLIGHT, true);
    }
}

void FEButtonWidget::UnsetFocus() {
    FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
    if (GetBacking() != nullptr) {
        FEngSetScript(GetBacking(), FEHASH_UNHIGHLIGHT, true);
    }
}

FEStatWidget::FEStatWidget(bool enabled)
    : FEWidget(nullptr, enabled, false), vMaxTitleSize(0.0f, 0.0f), vMaxDataSize(0.0f, 0.0f), vDataPos(0.0f, 0.0f), pTitle(nullptr), pData(nullptr) {}

void FEStatWidget::Position() {
    float y = GetTopLeftY() + GetMaxTitleHeight() * 0.5f;
    float x;
    if (GetTitleObject() != nullptr) {
        uint32 alignment = GetTitleObject()->Format;
        if ((alignment & FESTRING_JUSTIFY_HCENTER) != 0) {
            x = GetTopLeftX();
        } else if ((alignment & FESTRING_JUSTIFY_HRIGHT) != 0) {
            x = GetTopLeftX() + GetMaxTitleWidth();
        } else {
            x = GetTopLeftX() + GetMaxTitleWidth() * 0.5f;
        }
        FEngSetCenter(GetTitleObject(), x, y);
    }
    if (GetDataObject() != nullptr) {
        uint32 alignment = GetDataObject()->Format;
        if ((alignment & FESTRING_JUSTIFY_HCENTER) != 0) {
            FEngSetCenter(GetDataObject(), GetDataPosX() + GetMaxDataWidth() * 0.5f, y);
        } else if ((alignment & FESTRING_JUSTIFY_HRIGHT) != 0) {
            FEngSetCenterY(GetDataObject(), y);

            FEVector3 pos = GetDataObject()->GetObjData()->Pos;
            pos.x = GetDataPosX() + GetMaxDataWidth();
            GetDataObject()->SetPosition(pos, false);
        } else {
            FEngSetCenter(GetDataObject(), GetDataPosX(), y);
        }
    }
    if (GetBacking() != nullptr) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEStatWidget::Show() {
    FEngSetVisible(pTitle);
    FEngSetVisible(pData);
    if (GetBacking() != nullptr) {
        FEngSetVisible(GetBacking());
    }
}

void FEStatWidget::Hide() {
    FEngSetInvisible(pTitle);
    FEngSetInvisible(pData);
    if (GetBacking() != nullptr) {
        FEngSetInvisible(GetBacking());
    }
}

void FEStatWidget::SetPosX(float x) {
    float x_offset = vDataPos.x - GetTopLeftX();
    SetTopLeftX(x);
    SetDataPosX(x + x_offset);
    if (GetBacking() != nullptr) {
        FEngSetTopLeftX(GetBacking(), x - GetBackingOffsetX());
    }
}

void FEStatWidget::SetPosY(float y) {
    float y_offset = vDataPos.y - GetTopLeftY();
    SetTopLeftY(y);
    vDataPos.y = y + y_offset;
    if (GetBacking() != nullptr) {
        FEngSetTopLeftY(GetBacking(), y - GetBackingOffsetY());
    }
}

FEToggleWidget::FEToggleWidget(bool enabled)
    : FEStatWidget(enabled), pLeftImage(nullptr), pRightImage(nullptr), EnableScript(FEHASH_UNHIGHLIGHT), DisableScript(FEHASH_DISABLE) {}

void FEToggleWidget::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {}

void FEToggleWidget::Position() {
    FEStatWidget::Position();
    float left_img_width = FEngGetSizeX(GetLeftImage());
    float right_img_width = FEngGetSizeX(GetRightImage());
    FEngSetCenter(GetLeftImage(), GetDataPosX(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    FEngSetCenter(GetRightImage(), GetDataPosX() + GetMaxDataWidth(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
}

void FEToggleWidget::Enable() {
    SetDisableScript(FEHashUpper("NORMAL"));
    FEWidget::Enable();
    SetScript(EnableScript);
}

void FEToggleWidget::Disable() {
    SetDisableScript(FEHashUpper("GREY"));
    FEWidget::Disable();
    SetScript(DisableScript);
}

void FEToggleWidget::SetScript(uint32 script) {
    FEngSetScript(GetTitleObject(), script, true);
    FEngSetScript(GetDataObject(), script, true);
    FEngSetScript(GetLeftImage(), script, true);
    FEngSetScript(GetRightImage(), script, true);
    if (GetBacking() != nullptr) {
        FEngSetScript(GetBacking(), script, true);
    }
}

void FEToggleWidget::Show() {
    FEngSetVisible(GetTitleObject());
    FEngSetVisible(GetDataObject());
    FEngSetVisible(GetLeftImage());
    FEngSetVisible(GetRightImage());
    if (GetBacking() != nullptr) {
        FEngSetVisible(GetBacking());
    }
}

void FEToggleWidget::Hide() {
    FEngSetInvisible(GetTitleObject());
    FEngSetInvisible(GetDataObject());
    FEngSetInvisible(GetLeftImage());
    FEngSetInvisible(GetRightImage());
    if (GetBacking() != nullptr) {
        FEngSetInvisible(GetBacking());
    }
}

void FEToggleWidget::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    SetScript(FEHASH_HIGHLIGHT);
}

void FEToggleWidget::UnsetFocus() {
    SetScript(FEHASH_UNHIGHLIGHT);
}

void FEToggleWidget::BlinkArrows(uint32 data) {}

FESliderWidget::FESliderWidget(bool enabled) : FEToggleWidget(enabled) {
    fVertOffset = 9.5f;
}

void FESliderWidget::Position() {
    uint32 alignment = GetTitleObject()->Format;
    if ((alignment & FESTRING_JUSTIFY_HCENTER) != 0) {
        FEngSetCenter(GetTitleObject(), GetTopLeftX(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else if ((alignment & FESTRING_JUSTIFY_HRIGHT) != 0) {
        FEngSetCenter(GetTitleObject(), GetTopLeftX() + GetMaxTitleWidth(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else {
        FEngSetCenter(GetTitleObject(), GetTopLeftX() + GetMaxTitleWidth() * 0.5f, GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    }

    float data_x = GetDataPosX() + GetMaxDataWidth() * 0.5f;
    Slider.SetPos(data_x - Slider.GetBaseWidth() * 0.5f, GetDataPosY() + fVertOffset);
    Slider.Draw();

    float left_img_width = FEngGetSizeX(GetLeftImage());
    float right_img_width = FEngGetSizeX(GetRightImage());

    FEngSetCenter(GetLeftImage(), GetDataPosX(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    FEngSetCenter(GetRightImage(), GetDataPosX() + GetMaxDataWidth(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    if (GetBacking() != nullptr) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FESliderWidget::Show() {
    FEngSetVisible(GetTitleObject());
    FEngSetVisible(GetLeftImage());
    FEngSetVisible(GetRightImage());
    ToggleSlider(true);
    if (GetBacking() != nullptr) {
        FEngSetVisible(GetBacking());
    }
}

void FESliderWidget::Hide() {
    FEngSetInvisible(GetTitleObject());
    FEngSetInvisible(GetLeftImage());
    FEngSetInvisible(GetRightImage());
    ToggleSlider(false);
    if (GetBacking() != nullptr) {
        FEngSetInvisible(GetBacking());
    }
}

void FESliderWidget::Enable() {
    FEWidget::Enable();
}

void FESliderWidget::Disable() {
    FEToggleWidget::Disable();
}

void FESliderWidget::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), FEHASH_HIGHLIGHT, true);
    Slider.Highlight();
    if (GetBacking() != nullptr) {
        FEngSetScript(GetBacking(), FEHASH_HIGHLIGHT, true);
    }
}

void FESliderWidget::UnsetFocus() {
    FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
    Slider.UnHighlight();
    if (GetBacking() != nullptr) {
        FEngSetScript(GetBacking(), FEHASH_UNHIGHLIGHT, true);
    }
}

void FESliderWidget::UpdateSlider(uint32 msg) {
    if (Slider.Update(msg)) {
        BlinkArrows(msg);
        bMovedLastUpdate = true;
    } else {
        bMovedLastUpdate = false;
    }
}

FEScrollBar::FEScrollBar(const char *parent_pkg, const char *name, bool vert, bool resize, bool arrows_only)
    : bVertical(vert), bResizeHandle(resize), bArrowsOnly(arrows_only), bHandleGrabbed(false), bVisible(false), vGrabbedPos(0.0f, 0.0f),
      vCurPos(0.0f, 0.0f), vGrabOffset(0.0f, 0.0f), vBackingPos(0.0f, 0.0f), vBackingSize(0.0f, 0.0f), vHandleMinSize(0.0f, 0.0f), pBacking(nullptr),
      pHandle(nullptr), pFirstArrow(nullptr), pSecondArrow(nullptr), pFirstBackingEnd(nullptr), pSecondBackingEnd(nullptr) {
    char sztemp[32];
    if (name != nullptr) {
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Backing");
        pBacking = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Handle");
        pHandle = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Arrow_1");
        pFirstArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Arrow_2");
        pSecondArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Backing_End_1");
        pFirstBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "%s%s", name, "_Backing_End_2");
        pSecondBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngGetTopLeft(pBacking, vBackingPos.x, vBackingPos.y);
        FEngGetSize(pBacking, vBackingSize.x, vBackingSize.y);
        if (bResizeHandle) {
            if (bVertical) {
                vHandleMinSize.x = vHandleMinSize.y = vBackingSize.x;
            } else {
                vHandleMinSize.x = vHandleMinSize.y = vBackingSize.y;
            }
        } else {
            FEngGetSize(pHandle, vHandleMinSize.x, vHandleMinSize.y);
        }
    }
}

void FEScrollBar::SetGroupVisible(bool visible) {
    if (visible) {
        bVisible = true;
        SetVisible(pBacking);
        SetVisible(pHandle);
        SetVisible(pFirstArrow);
        SetVisible(pSecondArrow);
    } else {
        bVisible = false;
        SetInvisible(pBacking);
        SetInvisible(pHandle);
        SetInvisible(pFirstArrow);
        SetInvisible(pSecondArrow);
        SetInvisible(pFirstBackingEnd);
        SetInvisible(pSecondBackingEnd);
    }
}

void FEScrollBar::Update(int num_view_items, int num_list_items, int view_head_index, int selected_item) {
    if (selected_item == -1) {
        selected_item = view_head_index;
    }
    if (num_list_items <= num_view_items || num_list_items == 0) {
        SetGroupVisible(false);
    } else {
        SetGroupVisible(true);
        SetPosResized(num_view_items, num_list_items, view_head_index);
        if (selected_item == 1) {
            SetArrow1Dim(true);
        } else if (selected_item == num_list_items) {
            SetArrow2Dim(true);
        }
    }
}

void FEScrollBar::SetPosResized(int num_view_items, int num_list_items, int view_head_index) {
    if (bVertical) {
        float barsize = (static_cast<float>(num_view_items) / static_cast<float>(num_list_items)) * vBackingSize.y;
        FEngSetSizeY(pHandle, barsize);
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f);
        float range = (vBackingSize.y - barsize) / bMax(1.0f, static_cast<float>(num_list_items) - static_cast<float>(num_view_items));
        fSegSize = range;
        float num_segs = view_dist_to_head * range;
        vCurPos.y = num_segs + vBackingPos.y;
        if (!bHandleGrabbed) {
            FEngSetTopLeftY(pHandle, vCurPos.y);
        }
    } else {
        float barsize = (static_cast<float>(num_view_items) / static_cast<float>(num_list_items)) * vBackingSize.x;
        FEngSetSizeX(pHandle, barsize);
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f);
        float range = (vBackingSize.x - barsize) / bMax(1.0f, static_cast<float>(num_list_items) - static_cast<float>(num_view_items));
        fSegSize = range;
        float num_segs = view_dist_to_head * range;
        vCurPos.x = num_segs + vBackingPos.x;
        if (!bHandleGrabbed) {
            FEngSetTopLeftX(pHandle, vCurPos.x);
        }
    }
}

void FEScrollBar::SetArrowVisibility(int arrow_num, bool visible) {
    if (visible) {
        if (arrow_num == 1) {
            SetVisible(pFirstArrow);
        } else if (arrow_num == 2) {
            SetVisible(pSecondArrow);
        }
    } else {
        if (arrow_num == 1) {
            SetInvisible(pFirstArrow);
        } else if (arrow_num == 2) {
            SetInvisible(pSecondArrow);
        }
    }
}

void FEScrollBar::SetVisible(FEObject *obj) {
    FEngSetVisible(obj);
    FEngSetScript(obj, FEHASH_SHOW, true);
}

void FEScrollBar::SetInvisible(FEObject *obj) {
    FEngSetInvisible(obj);
    FEngSetScript(obj, FEHASH_HIDE, true);
}

void FEScrollBar::SetArrow1Dim(bool dim) {
    FEngSetScript(pFirstArrow, dim ? FEHASH_DIM : FEHASH_NORMAL, true);
}

void FEScrollBar::SetArrow2Dim(bool dim) {
    FEngSetScript(pSecondArrow, dim ? FEHASH_DIM : FEHASH_NORMAL, true);
}

CTextScroller::CTextScroller()
    : m_TopLine(0), m_ScrollDownMsg(__PAD_DOWN__), m_ScrollUpMsg(__PAD_UP__), m_pOwner(nullptr), m_pFont(nullptr), m_pScrollBar(nullptr),
      m_NumAddedLines(0), m_pLines(nullptr), m_pRawDataBlock(nullptr) {}

CTextScroller::~CTextScroller() {
    if (m_pRawDataBlock != nullptr) {
        delete[] m_pRawDataBlock;
    }
}

void CTextScroller::Initialise(MenuScreen *pOwner, int ViewWidth, int ViewLines, char *pTextDisplayNameTempl, FEngFont *pFont) {
    m_pOwner = pOwner;
    m_ViewWidth = ViewWidth;
    m_ViewVisibleLines = ViewLines;
    bStrNCpy(m_TextBoxNameTemplate, pTextDisplayNameTempl, 31);
    m_pFont = pFont;
}

void CTextScroller::SetTextHash(uint32 language_hash) {
    char *text = GetTranslatedString(language_hash);
    int TextSize = bStrLen(text) + 1;
    if (TextSize > 0) {
        int16 *pUCSText = new ("CTextScrollerWideToUCS2", 0) int16[TextSize];
        PackedStringToWideString(reinterpret_cast<uint16 *>(pUCSText), TextSize * 2, text);
        SetText(pUCSText);
        delete[] pUCSText;
    }
}

// UNSOLVED
void CTextScroller::SetText(int16 *pText) {
    if (!m_pFont || !pText) {
        m_NumAddedLines = 0;
    } else {
        m_DataBlockCurPos = 0;
        m_NumAddedLines = 0;
        int totalLines = 0;
        int totalChars = 0;
        int16 *lineStart = pText;
        int16 *lineEnd = FindCR(pText);
        if (!lineEnd) {
            lineEnd = FindEND(pText);
        }
        while (lineEnd) {
            if (*lineStart == 0) {
                break;
            }
            if (lineEnd == lineStart) {
                totalLines++;
                totalChars++;
            } else {
                int numLines, numChars;
                WordWrapCountLinesAndChars(lineStart, lineEnd, numLines, numChars);
                totalLines += numLines;
                totalChars += numChars;
            }
            if (*lineEnd == 0) {
                lineEnd = nullptr;
                if (!lineEnd) {
                    break;
                }
            } else {
                lineStart = lineEnd + 1;
                lineEnd = FindCR(lineStart);
                if (!lineEnd) {
                    lineEnd = FindEND(lineStart);
                }
            }
        }
        if (totalLines > 0) {
            if (!m_pRawDataBlock) {
                m_DataBlockSize = totalChars * 2 + totalLines * 4;
                m_pRawDataBlock = new ("CTextScroller(Raw)", 0) char[m_DataBlockSize];
            }
            m_DataBlockCurPos = totalLines * 4;
            m_pLines = reinterpret_cast<short **>(m_pRawDataBlock);
            lineEnd = FindCR(pText);
            if (!lineEnd) {
                lineEnd = FindEND(pText);
            }
            if (lineEnd) {
                while (*pText != 0) {
                    if (lineEnd == pText) {
                        AddLine(nullptr, 0);
                    } else {
                        WordWrapAddLines(pText, lineEnd, false, nullptr);
                    }
                    if (*lineEnd == 0) {
                        lineEnd = nullptr;
                        if (!lineEnd) {
                            break;
                        }
                    } else {
                        pText = lineEnd + 1;
                        lineEnd = FindCR(pText);
                        if (!lineEnd) {
                            lineEnd = FindEND(pText);
                            if (!lineEnd) {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    m_TopLine = 0;
    UpdateScrollBar();
    Display(m_TopLine);
}

void CTextScroller::Scroll(int Amount) {
    if (m_NumAddedLines > m_ViewVisibleLines) {
        int TopLine = m_TopLine + Amount;
        if (TopLine < 0) {
            TopLine = 0;
        } else if (TopLine + m_ViewVisibleLines >= m_NumAddedLines) {
            TopLine = m_NumAddedLines - m_ViewVisibleLines;
        }
        this->m_TopLine = TopLine;
        Display(TopLine);
    }
}

bool CTextScroller::HandleNotificationMessage(uint32 Msg) {
    if (Msg == m_ScrollUpMsg) {
        Scroll(-1);
        UpdateScrollBar();
        return true;
    }
    if (Msg == m_ScrollDownMsg) {
        Scroll(1);
        UpdateScrollBar();
        return true;
    }
    return false;
}

void CTextScroller::Display(int32 TopLine) {
    if (m_pOwner != nullptr) {
        int LineCount = TopLine;
        for (int i = 0; i < m_ViewVisibleLines; i++) {
            int16 Blank = 0;
            char TextBoxName[34];
            sprintf(TextBoxName, m_TextBoxNameTemplate, i + 1);
            if (LineCount < m_NumAddedLines) {
                FEString *str = FEngFindString(m_pOwner->GetPackageName(), FEHashUpper(TextBoxName));
                FESetString(str, m_pLines[LineCount]);
                str->Flags |= FF_Dirty;
            } else {
                FEString *str = FEngFindString(m_pOwner->GetPackageName(), FEHashUpper(TextBoxName));
                FESetString(str, &Blank);
                str->Flags |= FF_Dirty;
            }
            LineCount++;
        }
    }
}

void CTextScroller::AddLine(int16 *pLine, int Size) {
    if (m_DataBlockCurPos <= m_DataBlockSize - (Size + 1) * 2) {
        m_pLines[m_NumAddedLines] = reinterpret_cast<int16 *>(m_pRawDataBlock + m_DataBlockCurPos);
        memcpy(m_pLines[m_NumAddedLines], pLine, Size * 2);
        m_pLines[m_NumAddedLines][Size] = 0;
        m_NumAddedLines++;
        m_DataBlockCurPos += (Size + 1) * 2;
    }
}

void CTextScroller::WordWrapCountLinesAndChars(int16 *pTextStart, int16 *pTextEnd, int &NumLines, int &NumChars) {
    NumLines = WordWrapAddLines(pTextStart, pTextEnd, true, &NumChars);
}

// UNSOLVED
int CTextScroller::WordWrapAddLines(int16 *pTextStart, int16 *pTextEnd, bool bCountOnly, int *pNumCharsOut) {
    int NumLines = 0;

    if (pNumCharsOut) {
        *pNumCharsOut = 0;
    }

    for (; pTextStart < pTextEnd; NumLines++) {
        int StringLength = (reinterpret_cast<unsigned int>(pTextEnd) - reinterpret_cast<unsigned int>(pTextStart)) >> 1;
        int StringSize = 0;
        float TextWidth = 0.0f;
        int16 *pChar = pTextStart;
        int16 PrevChar = 0;
        bool bStringSizeOverflow = false;

        while (TextWidth < static_cast<float>(m_ViewWidth - 16) && pChar < pTextEnd) {
            bStringSizeOverflow = false;
            if (IsNewlineChar(*pChar)) {
                bStringSizeOverflow = true;
            }
            if (bStringSizeOverflow) {
                break;
            }

            if (FEngFont::IsJoyEventTexture(pChar, 0)) {
                const short *pNewChar;

                PrevChar = 0;
                TextWidth += m_pFont->GetJoyEventTextureWidth(pChar);
                pNewChar = m_pFont->SkipJoyEventTexture(pChar, 0);
                StringSize += pNewChar - pChar;
                pChar = const_cast<short *>(pNewChar);
            } else {
                TextWidth += m_pFont->GetCharacterWidth(*pChar, PrevChar, 0);
                StringSize++;
                PrevChar = *++pChar;
            }
        }

        if (StringSize < StringLength) {
            int WordBreak = StringSize - 1;

            while (WordBreak > 0 && pTextStart[WordBreak] != ' ') {
                WordBreak--;
            }

            if (WordBreak > 0) {
                StringSize = WordBreak + 1;
            }

            if (!bCountOnly) {
                AddLine(pTextStart, StringSize);
            }

            if (pNumCharsOut) {
                *pNumCharsOut += 1 + StringSize;
            }

            pTextStart += StringSize;
        } else {
            if (!bCountOnly) {
                AddLine(pTextStart, StringSize);
            }

            pTextStart = pTextEnd;

            if (pNumCharsOut) {
                *pNumCharsOut += 1 + StringSize;
            }
        }
    }

    return NumLines;
}

// UNSOLVED
int16 *CTextScroller::FindCR(int16 *pText) {
    int16 *pRet = nullptr;
    if (*pText != 0) {
        do {
            if (IsNewlineChar(*pText)) {
                pRet = pText;
            }
            pText++;
        } while (*pText != 0 && pRet == nullptr);
    }

    return pRet;
}

int16 *CTextScroller::FindEND(int16 *pText) {
    int16 *pRet = pText;
    while (*pRet != 0) {
        pRet++;
    }
    return pRet;
}

void CTextScroller::UpdateScrollBar() {
    if (m_pScrollBar != nullptr) {
        m_pScrollBar->Update(GetNumVisibleLines(), GetNumLines(), GetTopLine() + 1, GetTopLine() + 1);
    }
}
