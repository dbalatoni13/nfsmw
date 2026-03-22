#ifndef FRONTEND_MENUSCREENS_COMMON_CTEXTSCROLLER_H
#define FRONTEND_MENUSCREENS_COMMON_CTEXTSCROLLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct MenuScreen;
struct FEngFont;
class FEScrollBar;

// total size: 0x54
struct CTextScroller {
    MenuScreen* m_pOwner;              // offset 0x0, size 0x4
    FEngFont* m_pFont;                 // offset 0x4, size 0x4
    FEScrollBar* m_pScrollBar;         // offset 0x8, size 0x4
    char m_TextBoxNameTemplate[32];    // offset 0xC, size 0x20
    int m_ViewWidth;                   // offset 0x2C, size 0x4
    int m_ViewVisibleLines;            // offset 0x30, size 0x4
    int m_NumAddedLines;               // offset 0x34, size 0x4
    short** m_pLines;                  // offset 0x38, size 0x4
    char* m_pRawDataBlock;             // offset 0x3C, size 0x4
    unsigned int m_DataBlockSize;      // offset 0x40, size 0x4
    unsigned int m_DataBlockCurPos;    // offset 0x44, size 0x4
    int m_TopLine;                     // offset 0x48, size 0x4
    unsigned int m_ScrollDownMsg;      // offset 0x4C, size 0x4
    unsigned int m_ScrollUpMsg;        // offset 0x50, size 0x4

    inline void UseScrollBar(FEScrollBar* pScrollBar) { m_pScrollBar = pScrollBar; }

    CTextScroller();
    ~CTextScroller();
    void Initialise(MenuScreen* pOwner, int ViewWidth, int ViewLines, char* pTextDisplayNameTempl, FEngFont* pFont);
    void SetTextHash(unsigned int language_hash);
    void SetText(short* pText);
    void Scroll(int Amount);
    bool HandleNotificationMessage(unsigned int Msg);
    void Display(int TopLine);
    void AddLine(short *pLine, int Size);
    void WordWrapCountLinesAndChars(short* pTextStart, short* pTextEnd, int& NumLines, int& NumChars);
    int WordWrapAddLines(short* pTextStart, short* pTextEnd, bool bCountOnly, int* pNumCharsOut);
    short* FindCR(short* pText);
    short *FindEND(short *pText);
    int GetNumVisibleLines() { return m_ViewVisibleLines; }
    int GetNumLines() { return m_NumAddedLines; }
    int GetTopLine() { return m_TopLine; }
    void UpdateScrollBar();
};

#endif
