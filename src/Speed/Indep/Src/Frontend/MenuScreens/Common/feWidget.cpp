#include "feWidget.hpp"
#include "CTextScroller.hpp"

struct FEObject;
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetScript(FEObject* object, unsigned int script_hash, bool start_at_beginning);
void FEngGetTopLeft(FEObject* object, float& x, float& y);
void FEngSetTopLeft(FEObject* object, float x, float y);

FEWidget::FEWidget(FEObject* backing, bool enabled, bool hidden)
    : vTopLeft(0.0f, 0.0f) //
    , vSize(0.0f, 0.0f) //
    , vBackingOffset(0.0f, 0.0f) //
    , pBacking(backing) //
    , bEnabled(enabled) //
    , bHidden(hidden) //
    , bMovedLastUpdate(false) //
{}

void FEWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEWidget::Draw() {}
void FEWidget::Position() {}

void FEWidget::Show() {
    if (pBacking) {
        FEngSetVisible(pBacking);
    }
}

void FEWidget::Hide() {
    if (pBacking) {
        FEngSetInvisible(pBacking);
    }
}

void FEWidget::SetFocus(const char* parent_pkg) {}
void FEWidget::UnsetFocus() {}

FEButtonWidget::FEButtonWidget(bool enabled)
    : FEWidget(nullptr, enabled, false) //
    , pTitle(nullptr) //
    , vMaxTitleSize(0.0f, 0.0f) //
{}

void FEButtonWidget::Position() {
    float x, y;
    FEngGetTopLeft(pBacking, x, y);
    vTopLeft.x = x;
    vTopLeft.y = y;
    if (pTitle) {
        FEngGetTopLeft(static_cast<FEObject*>(pTitle), x, y);
        vMaxTitleSize.x = x;
        vMaxTitleSize.y = y;
    }
}

void FEButtonWidget::Show() {
    FEWidget::Show();
    if (pTitle) {
        FEngSetVisible(static_cast<FEObject*>(pTitle));
    }
}

void FEButtonWidget::Hide() {
    FEWidget::Hide();
    if (pTitle) {
        FEngSetInvisible(static_cast<FEObject*>(pTitle));
    }
}

void FEButtonWidget::CheckMouse(const char* parent_pkg, float mouse_x, float mouse_y) {}

void FEButtonWidget::SetFocus(const char* parent_pkg) {
    FEWidget::SetFocus(parent_pkg);
    if (pBacking) {
        FEngSetScript(pBacking, 0x37389004, true);
    }
}

void FEButtonWidget::UnsetFocus() {
    FEWidget::UnsetFocus();
    if (pBacking) {
        FEngSetScript(pBacking, 0x7AB5521A, true);
    }
}

void FEButtonWidget::SetPos(bVector2& pos) {
    FEWidget::SetPosX(pos.x);
    FEWidget::SetPosY(pos.y);
}

FEStatWidget::FEStatWidget(bool enabled)
    : FEWidget(nullptr, enabled, false) //
    , pTitle(nullptr) //
    , pData(nullptr) //
    , vMaxTitleSize(0.0f, 0.0f) //
    , vMaxDataSize(0.0f, 0.0f) //
    , vDataPos(0.0f, 0.0f) //
{}

void FEStatWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEStatWidget::Draw() {}
void FEStatWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}

void FEStatWidget::Position() {
    float x, y;
    FEngGetTopLeft(pBacking, x, y);
    vTopLeft.x = x;
    vTopLeft.y = y;
    if (pTitle) {
        FEngGetTopLeft(static_cast<FEObject*>(pTitle), x, y);
        vMaxTitleSize.x = x;
        vMaxTitleSize.y = y;
    }
    if (pData) {
        FEngGetTopLeft(static_cast<FEObject*>(pData), x, y);
        vDataPos.x = x;
        vDataPos.y = y;
    }
}

void FEStatWidget::Show() {
    FEWidget::Show();
    if (pTitle) {
        FEngSetVisible(static_cast<FEObject*>(pTitle));
    }
    if (pData) {
        FEngSetVisible(static_cast<FEObject*>(pData));
    }
}

void FEStatWidget::Hide() {
    FEWidget::Hide();
    if (pTitle) {
        FEngSetInvisible(static_cast<FEObject*>(pTitle));
    }
    if (pData) {
        FEngSetInvisible(static_cast<FEObject*>(pData));
    }
}

void FEStatWidget::SetFocus(const char* parent_pkg) {}

void FEStatWidget::UnsetFocus() {}

void FEStatWidget::SetPos(bVector2& pos) {
    SetPosX(pos.x);
    SetPosY(pos.y);
}

void FEStatWidget::SetPosX(float x) {
    float old_x = GetTopLeftX();
    SetTopLeftX(x);
    vDataPos.x = x + (vDataPos.x - old_x);
    if (pBacking) {
        float bx, by;
        FEngGetTopLeft(pBacking, bx, by);
        FEngSetTopLeft(pBacking, x - GetBackingOffsetX(), by);
    }
}

void FEStatWidget::SetPosY(float y) {
    float old_y = GetTopLeftY();
    SetTopLeftY(y);
    vDataPos.y = y + (vDataPos.y - old_y);
    if (pBacking) {
        float bx, by;
        FEngGetTopLeft(pBacking, bx, by);
        FEngSetTopLeft(pBacking, bx, y - GetBackingOffsetY());
    }
}

FEToggleWidget::FEToggleWidget(bool enabled)
    : FEStatWidget(enabled) //
    , pLeftImage(nullptr) //
    , pRightImage(nullptr) //
    , EnableScript(0x7AB5521A) //
    , DisableScript(0x36819D93) //
{}

void FEToggleWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEToggleWidget::BlinkArrows(unsigned int data) {}

void FEToggleWidget::Enable() {
    DisableScript = FEHashUpper("NORMAL");
    bEnabled = true;
    SetScript(EnableScript);
}

void FEToggleWidget::Disable() {
    DisableScript = FEHashUpper("GREY");
    bEnabled = false;
    SetScript(DisableScript);
}

void FEToggleWidget::SetScript(unsigned int script) {
    FEngSetScript(static_cast<FEObject*>(pTitle), script, true);
    FEngSetScript(static_cast<FEObject*>(pData), script, true);
    FEngSetScript(static_cast<FEObject*>(pLeftImage), script, true);
    FEngSetScript(static_cast<FEObject*>(pRightImage), script, true);
    if (pBacking) {
        FEngSetScript(pBacking, script, true);
    }
}

void FEToggleWidget::Show() {
    FEStatWidget::Show();
    if (pLeftImage) {
        FEngSetVisible(static_cast<FEObject*>(pLeftImage));
    }
    if (pRightImage) {
        FEngSetVisible(static_cast<FEObject*>(pRightImage));
    }
}

void FEToggleWidget::Hide() {
    FEStatWidget::Hide();
    if (pLeftImage) {
        FEngSetInvisible(static_cast<FEObject*>(pLeftImage));
    }
    if (pRightImage) {
        FEngSetInvisible(static_cast<FEObject*>(pRightImage));
    }
}

void FEToggleWidget::SetFocus(const char* parent_pkg) {
    FEStatWidget::SetFocus(parent_pkg);
}

void FEToggleWidget::UnsetFocus() {
    FEStatWidget::UnsetFocus();
}

void FEToggleWidget::Position() {
    FEStatWidget::Position();
    if (pLeftImage) {
        float x, y;
        FEngGetTopLeft(static_cast<FEObject*>(pLeftImage), x, y);
    }
    if (pRightImage) {
        float x, y;
        FEngGetTopLeft(static_cast<FEObject*>(pRightImage), x, y);
    }
}

FESliderWidget::FESliderWidget(bool enabled)
    : FEToggleWidget(enabled) //
    , Slider() //
    , fVertOffset(9.5f) //
{}

void FESliderWidget::Position() {
    FEToggleWidget::Position();
    Slider.SetPos(GetTopLeftX(), GetTopLeftY() + fVertOffset);
}

void FESliderWidget::Show() {
    FEToggleWidget::Show();
    Slider.ToggleVisible(true);
}

void FESliderWidget::Hide() {
    FEToggleWidget::Hide();
    Slider.ToggleVisible(false);
}

void FESliderWidget::Disable() {
    FEWidget::Disable();
}

void FESliderWidget::SetFocus(const char* parent_pkg) {
    FEToggleWidget::SetFocus(parent_pkg);
    Slider.Highlight();
}

void FESliderWidget::UnsetFocus() {
    FEToggleWidget::UnsetFocus();
    Slider.UnHighlight();
}

void FESliderWidget::UpdateSlider(unsigned int msg) {
    if (Slider.Update(msg)) {
        BlinkArrows(msg);
        bMovedLastUpdate = true;
    } else {
        bMovedLastUpdate = false;
    }
}

void FESliderWidget::Enable() {
    FEWidget::Enable();
}

short *CTextScroller::FindEND(short *pText) {
    while (*pText != 0) {
        pText++;
    }
    return pText;
}

extern char *bStrNCpy(char *dst, const char *src, int n);

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

bool CTextScroller::HandleNotificationMessage(unsigned int Msg) {
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

short *CTextScroller::FindCR(short *pText) {
    short c = *pText;
    short *result = nullptr;
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
        if (c == 0) break;
        if (result) {
            return result;
        }
    } while (true);
    return result;
}

void CTextScroller::WordWrapCountLinesAndChars(short *pTextStart, short *pTextEnd, int &NumLines, int &NumChars) {
    NumChars = WordWrapAddLines(pTextStart, pTextEnd, true, nullptr);
}

extern const char *GetTranslatedString(unsigned int hash);
extern int bStrLen(const char *str);
extern void PackedStringToWideString(short *dst, int dstSize, const char *src);

void CTextScroller::SetTextHash(unsigned int hash) {
    const char *str = GetTranslatedString(hash);
    int len = bStrLen(str);
    if (len + 1 > 0) {
        int size = (len + 1) * 2;
        short *wideStr = new short[size];
        PackedStringToWideString(wideStr, size, str);
        SetText(wideStr);
        delete[] wideStr;
    }
}

extern unsigned long FEHashUpper(const char *str);
extern FEString *FEngFindString(const char *pkg, int hash);
extern void FESetString(FEString *str, short *text);
extern int FEngSNPrintf(char *dest, int size, const char *fmt, ...);

void CTextScroller::Display(int topLine) {
    if (!m_pOwner || m_ViewVisibleLines <= 0) {
        return;
    }
    for (int i = 0; i < m_ViewVisibleLines; i++) {
        short emptyStr[1];
        emptyStr[0] = 0;
        char szName[32];
        FEngSNPrintf(szName, 32, m_TextBoxNameTemplate, i);
        FEString *feStr = FEngFindString(m_pOwner->GetPackageName(), FEHashUpper(szName));
        short *text;
        if (topLine < m_NumAddedLines) {
            text = m_pLines[topLine];
        } else {
            text = emptyStr;
        }
        FESetString(feStr, text);
        feStr->Flags |= 0x2000000;
        topLine++;
    }
}

extern void bMemCpy(void *dst, const void *src, unsigned int size);

void CTextScroller::AddLine(short *pText, int numChars) {
    if (m_DataBlockSize - m_DataBlockCurPos < static_cast<unsigned int>((numChars + 1) * 2)) {
        return;
    }
    m_pLines[m_NumAddedLines] = reinterpret_cast<short *>(m_pRawDataBlock + m_DataBlockCurPos);
    bMemCpy(m_pLines[m_NumAddedLines], pText, numChars * 2);
    m_pLines[m_NumAddedLines][numChars] = 0;
    m_NumAddedLines++;
    m_DataBlockCurPos += (numChars + 1) * 2;
}

void CTextScroller::UpdateScrollBar() {
    if (m_pScrollBar) {
        int pos = m_TopLine + 1;
        m_pScrollBar->Update(m_ViewVisibleLines, m_NumAddedLines, pos, pos);
    }
}

void CTextScroller::SetText(short *pText) {
    if (!m_pFont || !pText) {
        m_NumAddedLines = 0;
    } else {
        m_DataBlockCurPos = 0;
        m_NumAddedLines = 0;
        int totalLines = 0;
        int totalChars = 0;
        short *lineStart = pText;
        short *lineEnd = FindCR(pText);
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
