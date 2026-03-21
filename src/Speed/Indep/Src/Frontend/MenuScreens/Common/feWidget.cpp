#include "feWidget.hpp"
#include "CTextScroller.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"

struct FEObject;
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetScript(FEObject* object, unsigned int script_hash, bool start_at_beginning);
void FEngGetCenter(FEObject* object, float& x, float& y);
void FEngGetSize(FEObject* object, float& x, float& y);
void FEngSetCenter(FEObject* object, float x, float y);
void FEngGetTopLeft(FEObject* object, float& x, float& y);
void FEngSetTopLeft(FEObject* object, float x, float y);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
inline void FEngSetCurrentButton(const char* pkg_name, FEObject* obj) {
    FEngSetCurrentButton(pkg_name, obj->NameHash);
}

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
    unsigned int format = pTitle->Format;
    if ((format & 1) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x, vTopLeft.y + vMaxTitleSize.y * 0.5f);
    } else if ((format & 2) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x + vMaxTitleSize.x,
                      vTopLeft.y + vMaxTitleSize.y * 0.5f);
    } else {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x + vMaxTitleSize.x * 0.5f,
                      vTopLeft.y + vMaxTitleSize.y * 0.5f);
    }

    if (pBacking) {
        FEngSetTopLeft(pBacking, vTopLeft.x - vBackingOffset.x, vTopLeft.y - vBackingOffset.y);
    }
}

void FEButtonWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject*>(pTitle));
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEButtonWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject*>(pTitle));
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FEButtonWidget::CheckMouse(const char* parent_pkg, float mouse_x, float mouse_y) {}

void FEButtonWidget::SetFocus(const char* parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), 0x249DB7B7, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x249DB7B7, true);
    }
}

void FEButtonWidget::UnsetFocus() {
    FEngSetScript(GetTitleObject(), 0x7AB5521A, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x7AB5521A, true);
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
    float title_y = vTopLeft.y + vMaxTitleSize.y * 0.5f;
    if (pTitle) {
        unsigned int format = pTitle->Format;
        float title_x;
        if ((format & 1) != 0) {
            title_x = vTopLeft.x;
        } else if ((format & 2) != 0) {
            title_x = vTopLeft.x + vMaxTitleSize.x;
        } else {
            title_x = vTopLeft.x + vMaxTitleSize.x * 0.5f;
        }
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), title_x, title_y);
    }
    if (pData) {
        FEObject *data = reinterpret_cast<FEObject*>(pData);
        unsigned int format = pData->Format;
        if ((format & 1) != 0) {
            FEngSetCenter(data, vDataPos.x + vMaxDataSize.x * 0.5f, title_y);
        } else if ((format & 2) != 0) {
            float center_x, center_y;
            FEngGetCenter(data, center_x, center_y);
            FEngSetCenter(data, center_x, title_y);

            FEVector3 pos = pData->GetObjData()->Pos;
            pos.x = vDataPos.x + vMaxDataSize.x;
            data->SetPosition(pos, false);
        } else {
            FEngSetCenter(data, vDataPos.x, title_y);
        }
    }
    if (pBacking) {
        FEngSetTopLeft(pBacking, vTopLeft.x - vBackingOffset.x, vTopLeft.y - vBackingOffset.y);
    }
}

void FEStatWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject*>(pTitle));
    FEngSetVisible(reinterpret_cast<FEObject*>(pData));
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEStatWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject*>(pTitle));
    FEngSetInvisible(reinterpret_cast<FEObject*>(pData));
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
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

FEToggleWidget::~FEToggleWidget() {}

void FEToggleWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEToggleWidget::BlinkArrows(unsigned int data) {}

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
    FEngSetScript(GetTitleObject(), script, true);
    FEngSetScript(GetDataObject(), script, true);
    FEngSetScript(GetLeftImage(), script, true);
    FEngSetScript(GetRightImage(), script, true);
    if (GetBacking()) {
        FEngSetScript(GetBacking(), script, true);
    }
}

void FEToggleWidget::Show() {
    FEngSetVisible(GetTitleObject());
    FEngSetVisible(GetDataObject());
    FEngSetVisible(GetLeftImage());
    FEngSetVisible(GetRightImage());
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FEToggleWidget::Hide() {
    FEngSetInvisible(GetTitleObject());
    FEngSetInvisible(GetDataObject());
    FEngSetInvisible(GetLeftImage());
    FEngSetInvisible(GetRightImage());
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FEToggleWidget::SetFocus(const char* parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    SetScript(0x249DB7B7);
}

void FEToggleWidget::UnsetFocus() {
    SetScript(0x7AB5521A);
}

void FEToggleWidget::Position() {
    FEStatWidget::Position();
    float left_width, left_height;
    FEngGetSize(reinterpret_cast<FEObject*>(pLeftImage), left_width, left_height);
    float right_width, right_height;
    FEngGetSize(reinterpret_cast<FEObject*>(pRightImage), right_width, right_height);
    FEngSetCenter(reinterpret_cast<FEObject*>(pLeftImage), vDataPos.x, vDataPos.y + vMaxTitleSize.y * 0.5f);
    FEngSetCenter(reinterpret_cast<FEObject*>(pRightImage), vDataPos.x + vMaxDataSize.x,
                  vDataPos.y + vMaxTitleSize.y * 0.5f);
}

FESliderWidget::FESliderWidget(bool enabled)
    : FEToggleWidget(enabled) //
    , Slider() //
    , fVertOffset(9.5f) //
{}

FESliderWidget::~FESliderWidget() {}

void FESliderWidget::Position() {
    unsigned int format = pTitle->Format;
    if ((format & 1) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x, vTopLeft.y + vMaxTitleSize.y * 0.5f);
    } else if ((format & 2) != 0) {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x + vMaxTitleSize.x,
                      vTopLeft.y + vMaxTitleSize.y * 0.5f);
    } else {
        FEngSetCenter(reinterpret_cast<FEObject*>(pTitle), vTopLeft.x + vMaxTitleSize.x * 0.5f,
                      vTopLeft.y + vMaxTitleSize.y * 0.5f);
    }

    float slider_center_x = vDataPos.x + vMaxDataSize.x * 0.5f;
    float slider_width, slider_height;
    FEngGetSize(*reinterpret_cast<FEObject**>(&Slider), slider_width, slider_height);
    Slider.SetPos(slider_center_x - slider_width * 0.5f, vDataPos.y + fVertOffset);
    Slider.Draw();

    float left_width, left_height;
    FEngGetSize(reinterpret_cast<FEObject*>(pLeftImage), left_width, left_height);
    float right_width, right_height;
    FEngGetSize(reinterpret_cast<FEObject*>(pRightImage), right_width, right_height);
    FEngSetCenter(reinterpret_cast<FEObject*>(pLeftImage), vDataPos.x, vDataPos.y + vMaxTitleSize.y * 0.5f);
    FEngSetCenter(reinterpret_cast<FEObject*>(pRightImage), vDataPos.x + vMaxDataSize.x,
                  vDataPos.y + vMaxTitleSize.y * 0.5f);
    if (pBacking != nullptr) {
        FEngSetTopLeft(pBacking, vTopLeft.x - vBackingOffset.x, vTopLeft.y - vBackingOffset.y);
    }
}

void FESliderWidget::Show() {
    FEngSetVisible(reinterpret_cast<FEObject*>(GetTitleObject()));
    FEngSetVisible(reinterpret_cast<FEObject*>(GetLeftImage()));
    FEngSetVisible(reinterpret_cast<FEObject*>(GetRightImage()));
    ToggleSlider(true);
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
    }
}

void FESliderWidget::Hide() {
    FEngSetInvisible(reinterpret_cast<FEObject*>(GetTitleObject()));
    FEngSetInvisible(reinterpret_cast<FEObject*>(GetLeftImage()));
    FEngSetInvisible(reinterpret_cast<FEObject*>(GetRightImage()));
    ToggleSlider(false);
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
    }
}

void FESliderWidget::Disable() {
    FEToggleWidget::Disable();
}

void FESliderWidget::SetFocus(const char* parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), 0x249DB7B7, true);
    Slider.Highlight();
    if (GetBacking()) {
        FEngSetScript(GetBacking(), 0x249DB7B7, true);
    }
}

void FESliderWidget::UnsetFocus() {
    FEngSetScript(GetTitleObject(), 0x7AB5521A, true);
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

int CTextScroller::WordWrapAddLines(short *pTextStart, short *pTextEnd, bool bCountOnly, int *pNumCharsOut) {
    int NumLines = 0;

    if (pNumCharsOut) {
        *pNumCharsOut = 0;
    }

    for (; pTextStart < pTextEnd; NumLines++) {
        int StringLength = (reinterpret_cast<unsigned int>(pTextEnd) - reinterpret_cast<unsigned int>(pTextStart)) >> 1;
        int StringSize = 0;
        float TextWidth = 0.0f;
        short *pChar = pTextStart;
        short PrevChar = 0;
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
        reinterpret_cast<FEObject*>(feStr)->Flags |= 0x2000000;
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
        m_pScrollBar->Update(GetNumVisibleLines(), GetNumLines(), GetTopLine() + 1, GetTopLine() + 1);
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
