#include "feWidget.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

FEWidget::FEWidget(FEObject *backing, bool enabled, bool hidden) {
    vTopLeft = bVector2(0.0f, 0.0f);
    vSize = bVector2(0.0f, 0.0f);
    vBackingOffset = bVector2(0.0f, 0.0f);
    pBacking = backing;
    bEnabled = enabled;
    bHidden = hidden;
    bMovedLastUpdate = false;
}

FEButtonWidget::FEButtonWidget(bool enabled) : FEWidget(nullptr, enabled, false) {
    pTitle = nullptr;
    vMaxTitleSize = bVector2(0.0f, 0.0f);
}

void FEButtonWidget::CheckMouse(const char *parent_pkg, float mouse_x, float mouse_y) {}

void FEButtonWidget::Position() {
    uint32 alignment = pTitle->Format;
    if ((alignment & 1) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject *>(pTitle), GetTopLeftX(), GetTopLeftY() + vMaxTitleSize.y * 0.5f);
    } else if ((alignment & 2) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject *>(pTitle), GetTopLeftX() + vMaxTitleSize.x, GetTopLeftX() + vMaxTitleSize.y * 0.5f);
    } else {
        FEngSetCenter(reinterpret_cast<FEObject *>(pTitle), GetTopLeftX() + vMaxTitleSize.x * 0.5f, GetTopLeftY() + vMaxTitleSize.y * 0.5f);
    }

    if (GetBacking()) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEButtonWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject *>(pTitle));
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEButtonWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject *>(pTitle));
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FEButtonWidget::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetScript(reinterpret_cast<FEObject *>(GetTitleObject()), 0x249DB7B7, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x249DB7B7, true);
    }
}

void FEButtonWidget::UnsetFocus() {
    FEngSetScript(reinterpret_cast<FEObject *>(GetTitleObject()), 0x7AB5521A, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x7AB5521A, true);
    }
}

FEStatWidget::FEStatWidget(bool enabled) : FEWidget(nullptr, enabled, false) {
    pTitle = nullptr;
    pData = nullptr;
    vMaxTitleSize = bVector2(0.0f, 0.0f);
    vMaxDataSize = bVector2(0.0f, 0.0f);
    vDataPos = bVector2(0.0f, 0.0f);
}

void FEStatWidget::Position() {
    float title_y = GetTopLeftY() + vMaxTitleSize.y * 0.5f;
    if (pTitle) {
        unsigned int format = pTitle->Format;
        float title_x;
        if ((format & 1) != 0) {
            title_x = GetTopLeftX();
        } else if ((format & 2) != 0) {
            title_x = GetTopLeftX() + vMaxTitleSize.x;
        } else {
            title_x = GetTopLeftX() + vMaxTitleSize.x * 0.5f;
        }
        FEngSetCenter(reinterpret_cast<FEObject *>(pTitle), title_x, title_y);
    }
    if (pData) {
        FEObject *data = reinterpret_cast<FEObject *>(pData);
        unsigned int format = pData->Format;
        if ((format & 1) != 0) {
            FEngSetCenter(data, vDataPos.x + vMaxDataSize.x * 0.5f, title_y);
        } else if ((format & 2) != 0) {
            float center_x, center_y;
            FEngGetCenter(data, center_x, center_y);
            FEngSetCenter(data, center_x, title_y);

            FEVector3 pos = reinterpret_cast<FEObject *>(pData)->GetObjData()->Pos;
            pos.x = vDataPos.x + vMaxDataSize.x;
            data->SetPosition(pos, false);
        } else {
            FEngSetCenter(data, vDataPos.x, title_y);
        }
    }
    if (GetBacking()) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEStatWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject *>(pTitle));
    FEngSetVisible(reinterpret_cast<FEObject *>(pData));
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEStatWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject *>(pTitle));
    FEngSetInvisible(reinterpret_cast<FEObject *>(pData));
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FEStatWidget::SetPosX(float x) {
    float old_x = GetTopLeftX();
    SetTopLeftX(x);
    vDataPos.x = x + (vDataPos.x - old_x);
    if (GetBacking()) {
        float bx, by;
        FEngGetTopLeft(GetBacking(), bx, by);
        FEngSetTopLeft(GetBacking(), x - GetBackingOffsetX(), by);
    }
}

void FEStatWidget::SetPosY(float y) {
    float old_y = GetTopLeftY();
    SetTopLeftY(y);
    vDataPos.y = y + (vDataPos.y - old_y);
    if (GetBacking()) {
        float bx, by;
        FEngGetTopLeft(GetBacking(), bx, by);
        FEngSetTopLeft(GetBacking(), bx, y - GetBackingOffsetY());
    }
}

FEToggleWidget::FEToggleWidget(bool enabled) : FEStatWidget(enabled) {
    pLeftImage = nullptr;
    pRightImage = nullptr;
    EnableScript = 0x7AB5521A;
    DisableScript = 0x36819D93;
}

void FEToggleWidget::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {}

void FEToggleWidget::Position() {
    FEStatWidget::Position();
    float left_width, left_height;
    FEngGetSize(reinterpret_cast<FEObject *>(GetLeftImage()), left_width, left_height);
    float right_width, right_height;
    FEngGetSize(reinterpret_cast<FEObject *>(GetRightImage()), right_width, right_height);
    FEngSetCenter(reinterpret_cast<FEObject *>(GetLeftImage()), GetDataPosX(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    FEngSetCenter(reinterpret_cast<FEObject *>(GetRightImage()), GetDataPosX() + GetMaxDataWidth(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
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

void FEToggleWidget::SetScript(unsigned int script) {
    FEngSetScript(reinterpret_cast<FEObject *>(GetTitleObject()), script, true);
    FEngSetScript(reinterpret_cast<FEObject *>(GetDataObject()), script, true);
    FEngSetScript(reinterpret_cast<FEObject *>(GetLeftImage()), script, true);
    FEngSetScript(reinterpret_cast<FEObject *>(GetRightImage()), script, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), script, true);
    }
}

void FEToggleWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetVisible(reinterpret_cast<FEObject *>(GetDataObject()));
    FEngSetVisible(reinterpret_cast<FEObject *>(GetLeftImage()));
    FEngSetVisible(reinterpret_cast<FEObject *>(GetRightImage()));
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEToggleWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetDataObject()));
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetLeftImage()));
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetRightImage()));
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FEToggleWidget::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, reinterpret_cast<FEObject *>(GetTitleObject()));
    SetScript(0x249DB7B7);
}

void FEToggleWidget::UnsetFocus() {
    SetScript(0x7AB5521A);
}

void FEToggleWidget::BlinkArrows(uint32 data) {}

FESliderWidget::FESliderWidget(bool enabled) : FEToggleWidget(enabled) {
    // Slider = Slider();
    fVertOffset = 9.5f;
}

void FESliderWidget::Position() {
    uint32 alignment = GetTitleObject()->Format;
    if ((alignment & 1) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject *>(GetTitleObject()), GetTopLeftX(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else if ((alignment & 2) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject *>(GetTitleObject()), GetTopLeftX() + GetMaxTitleWidth(), GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    } else {
        FEngSetCenter(reinterpret_cast<FEObject *>(GetTitleObject()), GetTopLeftX() + GetMaxTitleWidth() * 0.5f,
                      GetTopLeftY() + GetMaxTitleHeight() * 0.5f);
    }

    float slider_center_x = GetDataPosX() + GetMaxDataWidth() * 0.5f;
    Slider.SetPos(slider_center_x - Slider.GetBaseWidth() * 0.5f, GetDataPosY() + fVertOffset);
    Slider.Draw();

    float left_img_width = FEngGetSizeX(reinterpret_cast<FEObject *>(GetLeftImage()));
    float right_img_width = FEngGetSizeX(reinterpret_cast<FEObject *>(GetRightImage()));

    FEngSetCenter(reinterpret_cast<FEObject *>(GetLeftImage()), GetDataPosX(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    FEngSetCenter(reinterpret_cast<FEObject *>(GetRightImage()), GetDataPosX() + GetMaxDataWidth(), GetDataPosY() + GetMaxTitleHeight() * 0.5f);
    if (GetBacking()) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FESliderWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetVisible(reinterpret_cast<FEObject *>(GetLeftImage()));
    FEngSetVisible(reinterpret_cast<FEObject *>(GetRightImage()));
    ToggleSlider(true);
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FESliderWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetLeftImage()));
    FEngSetInvisible(reinterpret_cast<FEObject *>(GetRightImage()));
    ToggleSlider(false);
    if (GetBacking()) {
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
    FEngSetCurrentButton(parent_pkg, reinterpret_cast<FEObject *>(GetTitleObject()));
    FEngSetScript(reinterpret_cast<FEObject *>(GetTitleObject()), 0x249DB7B7, true);
    Slider.Highlight();
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x249DB7B7, true);
    }
}

void FESliderWidget::UnsetFocus() {
    FEngSetScript(reinterpret_cast<FEObject *>(GetTitleObject()), 0x7AB5521A, true);
    Slider.UnHighlight();
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x7AB5521A, true);
    }
}

void FESliderWidget::UpdateSlider(unsigned int msg) {
    if (Slider.Update(msg)) {
        BlinkArrows(msg);
        bMovedLastUpdate = true;
    } else {
        bMovedLastUpdate = false;
    }
}

FEScrollBar::FEScrollBar(const char *parent_pkg, const char *name, bool vert, bool resize, bool arrows_only) {
    bVertical = vert;
    bResizeHandle = resize;
    bArrowsOnly = arrows_only;
    bHandleGrabbed = false;
    bVisible = false;
    vGrabbedPos = bVector2(0.0f, 0.0f);
    vCurPos = bVector2(0.0f, 0.0f);
    vGrabOffset = bVector2(0.0f, 0.0f);
    vBackingPos = bVector2(0.0f, 0.0f);
    vBackingSize = bVector2(0.0f, 0.0f);
    vHandleMinSize = bVector2(0.0f, 0.0f);
    fSegSize = 0.0f;
    pBacking = nullptr;
    pHandle = nullptr;
    pFirstArrow = nullptr;
    pSecondArrow = nullptr;
    pFirstBackingEnd = nullptr;
    pSecondBackingEnd = nullptr;

    if (name) {
        char sztemp[32];
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing");
        pBacking = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Handle");
        pHandle = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Arrow_1");
        pFirstArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Arrow_2");
        pSecondArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing_End_1");
        pFirstBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing_End_2");
        pSecondBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngGetTopLeft(pBacking, vBackingPos.x, vBackingPos.y);
        FEngGetSize(pBacking, vBackingSize.x, vBackingSize.y);
        if (!bResizeHandle) {
            FEngGetSize(pHandle, vHandleMinSize.x, vHandleMinSize.y);
        } else {
            if (!bVertical) {
                vHandleMinSize.x = vBackingSize.y;
            } else {
                vHandleMinSize.x = vBackingSize.x;
            }
            vHandleMinSize.y = vHandleMinSize.x;
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
        float range = static_cast<float>(num_list_items) - static_cast<float>(num_view_items);
        float num_segs = (vBackingSize.y - barsize) / bMax(1.0f, range);
        fSegSize = num_segs;
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f) * num_segs + vBackingPos.y;
        vCurPos.y = view_dist_to_head;
        if (!bHandleGrabbed) {
            FEngSetTopLeftY(pHandle, view_dist_to_head);
        }
    } else {
        float barsize = (static_cast<float>(num_view_items) / static_cast<float>(num_list_items)) * vBackingSize.x;
        FEngSetSizeX(pHandle, barsize);
        float range = static_cast<float>(num_list_items) - static_cast<float>(num_view_items);
        float num_segs = (vBackingSize.x - barsize) / bMax(1.0f, range);
        fSegSize = num_segs;
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f) * num_segs + vBackingPos.x;
        vCurPos.x = view_dist_to_head;
        if (!bHandleGrabbed) {
            FEngSetTopLeftX(pHandle, view_dist_to_head);
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
    FEngSetScript(obj, 0x001CA7C0, true);
}

void FEScrollBar::SetInvisible(FEObject *obj) {
    FEngSetInvisible(obj);
    FEngSetScript(obj, 0x0016A259, true);
}

void FEScrollBar::SetArrow1Dim(bool dim) {
    FEngSetScript(pFirstArrow, dim ? 0x9E99 : 0x6EBBFB68, true);
}

void FEScrollBar::SetArrow2Dim(bool dim) {
    FEngSetScript(pSecondArrow, dim ? 0x9E99 : 0x6EBBFB68, true);
}

CTextScroller::CTextScroller() {
    m_TopLine = 0;
    m_ScrollDownMsg = 0x911C0A4B;
    m_ScrollUpMsg = 0x72619778;
    m_pOwner = nullptr;
    m_pFont = nullptr;
    m_pScrollBar = nullptr;
    m_NumAddedLines = 0;
    m_pLines = nullptr;
    m_pRawDataBlock = nullptr;
}

CTextScroller::~CTextScroller() {
    if (m_pRawDataBlock) {
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

void CTextScroller::SetTextHash(uint32 hash) {
    const char *str = GetTranslatedString(hash);
    int len = bStrLen(str);
    if (len + 1 > 0) {
        int size = (len + 1) * 2;
        int16 *wideStr = new int16[size];
        PackedStringToWideString((u16 *)wideStr, size, str);
        SetText(wideStr);
        delete[] wideStr;
    }
}

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
                int blockSize = totalChars * 2 + totalLines * 4;
                m_DataBlockSize = blockSize;
                m_pRawDataBlock = new char[blockSize];
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
    if (m_NumAddedLines <= m_ViewVisibleLines) {
        return;
    }
    int top = m_TopLine + Amount;
    if (top < 0) {
        top = 0;
    } else {
        int maxTop = m_NumAddedLines - m_ViewVisibleLines;
        if (top + m_ViewVisibleLines > m_NumAddedLines) {
            top = maxTop;
        }
    }
    m_TopLine = top;
    Display(top);
}

bool CTextScroller::HandleNotificationMessage(uint32 Msg) {
    if (Msg == m_ScrollUpMsg) {
        Scroll(-1);
        UpdateScrollBar();
        return true;
    }
    if (Msg != m_ScrollDownMsg) {
        return false;
    }
    Scroll(1);
    UpdateScrollBar();
    return true;
}

void CTextScroller::Display(int32 topLine) {
    if (!m_pOwner || m_ViewVisibleLines <= 0) {
        return;
    }
    for (int i = 0; i < m_ViewVisibleLines; i++) {
        int16 emptyStr[1];
        emptyStr[0] = 0;
        char szName[32];
        FEngSNPrintf(szName, 32, m_TextBoxNameTemplate, i);
        FEString *feStr = FEngFindString(m_pOwner->GetPackageName(), FEHashUpper(szName));
        int16 *text;
        if (topLine < m_NumAddedLines) {
            text = m_pLines[topLine];
        } else {
            text = emptyStr;
        }
        FESetString(feStr, text);
        reinterpret_cast<FEObject *>(feStr)->Flags |= FF_Dirty;
        topLine++;
    }
}

void CTextScroller::AddLine(int16 *pText, int numChars) {
    if (m_DataBlockSize - m_DataBlockCurPos < static_cast<unsigned int>((numChars + 1) * 2)) {
        return;
    }
    m_pLines[m_NumAddedLines] = reinterpret_cast<int16 *>(m_pRawDataBlock + m_DataBlockCurPos);
    bMemCpy(m_pLines[m_NumAddedLines], pText, numChars * 2);
    m_pLines[m_NumAddedLines][numChars] = 0;
    m_NumAddedLines++;
    m_DataBlockCurPos += (numChars + 1) * 2;
}

void CTextScroller::WordWrapCountLinesAndChars(int16 *pTextStart, int16 *pTextEnd, int &NumLines, int &NumChars) {
    NumChars = WordWrapAddLines(pTextStart, pTextEnd, true, nullptr);
}

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

int16 *CTextScroller::FindCR(int16 *pText) {
    int16 c = *pText;
    int16 *result = nullptr;
    if (c == 0) {
        return result;
    }
    do {
        bool found = false;
        if (c == '\n' || c == '^') {
            found = true;
        }
        if (found) {
            result = pText;
        }
        pText++;
        c = *pText;
        if (c == 0)
            break;
        if (result) {
            return result;
        }
    } while (true);
    return result;
}

int16 *CTextScroller::FindEND(int16 *pText) {
    while (*pText != 0) {
        pText++;
    }
    return pText;
}

void CTextScroller::UpdateScrollBar() {
    if (m_pScrollBar) {
        m_pScrollBar->Update(GetNumVisibleLines(), GetNumLines(), GetTopLine() + 1, GetTopLine() + 1);
    }
}
