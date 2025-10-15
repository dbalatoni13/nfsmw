/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuirender.h
 *  Content:    Xbox 360 UI rendering defines, structures, and functions
 *
 ****************************************************************************/

#ifndef __XUIRENDER_H__
#define __XUIRENDER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XUIDC* HXUIDC;
typedef struct _XUISHAPE* HXUISHAPE;
typedef struct _XUIBRUSH* HXUIBRUSH;
typedef struct _XUITEX* HXUITEX;
typedef struct _XUIFONT* HXUIFONT;
typedef struct XUIRect XUIRect;
typedef struct XUIImageInfo XUIImageInfo;
typedef struct XUIGradientStop  XUIGradientStop;
typedef struct XUIRenderStats XUIRenderStats;
typedef struct XUIFontMetrics XUIFontMetrics;
typedef struct XUICharMetrics XUICharMetrics;
typedef struct CharData CharData;
typedef struct IXuiFontRenderer IXuiFontRenderer;
typedef struct IXuiFontRendererVTBL IXuiFontRendererVTBL;
typedef enum XUI_BLEND_MODE XUI_BLEND_MODE;
typedef struct XUIFigurePoint XUIFigurePoint;

// data structures
struct XUIRect
{
    DECLARE_XUI_ALLOC()

    float left;
    float top;
    float right;
    float bottom;
    
#ifdef __cplusplus
    XUIRect()
    {
    }

    XUIRect(float l, float t, float r, float b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    
    float GetWidth() const
    {
        return right - left;
    }
    
    float GetHeight() const
    {
        return bottom - top;
    }
    
    BOOL Contains(const D3DXVECTOR2& pt) const
    {
        if (pt.x >= left && pt.x <= right &&
            pt.y >= top && pt.y <= bottom)
            return TRUE;
        return FALSE;
    }
#endif
};

struct XUIImageInfo
{
    DECLARE_XUI_ALLOC()
    UINT                 Width;
    UINT                 Height;
    UINT                 Depth;
    UINT                 MipLevels;
    D3DFORMAT            Format;
    D3DRESOURCETYPE      ResourceType;
    UINT                 ImageFileFormat;
};

struct XUIGradientStop
{
    DECLARE_XUI_ALLOC()

    DWORD dwColor;
    float fPos;
};

struct XUIFigurePoint
{
    D3DXVECTOR2 point;
    D3DXVECTOR2 ptCtl1;
    D3DXVECTOR2 ptCtl2;
    BOOL bSmooth;
};

struct XUIRenderStats
{
    DECLARE_XUI_ALLOC()

    DWORD dwFaces;  // number of rendered triangles
    DWORD dwVerts;  // number of rendered vertices
    DWORD dwNumVertBuffers;  // number of allocated vertex buffers
    DWORD dwNumIndexBuffers; // number of allocated index buffers
    DWORD dwVertsAlloced;   // number of vertices allocated
    DWORD dwIndicesAlloced; // number of indices allocated
    
    DWORD dwNumCachedTextures;      // total number of textures in the cache (includes unused textures)
    DWORD dwTextureMemory;          // approximate number of bytes used by active textures in the cache
    DWORD dwNumUnusedTextures;      // number of unused textures still in the cache
    DWORD dwUnusedTextureMemory;    // approximate number of bytes used by unreferenced textures in the cache
};

// blending modes
enum XUI_BLEND_MODE
{
    XUI_BLEND_DEFAULT,  // inherit current blending mode
    XUI_BLEND_NORMAL,
    XUI_BLEND_MULTIPLY,
    XUI_BLEND_DARKEN,
    XUI_BLEND_LIGHTEN,
    XUI_BLEND_ADD,
    XUI_BLEND_SUBTRACT
};


// color buffer write flags
#define XUI_COLOR_WRITE_DEFAULT         0
#define XUI_COLOR_WRITE_RED             1
#define XUI_COLOR_WRITE_GREEN           2
#define XUI_COLOR_WRITE_BLUE            4
#define XUI_COLOR_WRITE_ALPHA           8
#define XUI_COLOR_WRITE_ALL             (XUI_COLOR_WRITE_RED|XUI_COLOR_WRITE_GREEN|XUI_COLOR_WRITE_BLUE|XUI_COLOR_WRITE_ALPHA)

//
// Texture Loader
//
typedef HRESULT (APIENTRY *PFN_XUITEXTURELOADER)(IDirect3DDevice9 *pDevice, LPCWSTR szFileName, XUIImageInfo *pImageInfo, IDirect3DTexture9 **ppTex);

//
// Texture brush wrap modes (passed to XuiSelectBrushEx)
//
#define XUI_TEXTURE_U_WRAP        0
#define XUI_TEXTURE_U_MIRROR      1
#define XUI_TEXTURE_U_CLAMP       2
#define XUI_TEXTURE_U_MIRROR_ONCE 3
#define XUI_TEXTURE_U_MASK        3
#define XUI_TEXTURE_U_SHIFT       0

#define XUI_TEXTURE_V_WRAP        0
#define XUI_TEXTURE_V_MIRROR      4
#define XUI_TEXTURE_V_CLAMP       8
#define XUI_TEXTURE_V_MIRROR_ONCE 12
#define XUI_TEXTURE_V_MASK        12
#define XUI_TEXTURE_V_SHIFT       2


//
// Texture filtering modes (passed to XuiSelectBrushEx)
//
#define XUI_TEXTURE_MIN_LINEAR      0
#define XUI_TEXTURE_MIN_POINT       16
#define XUI_TEXTURE_MIN_MASK        16
#define XUI_TEXTURE_MIN_SHIFT       4

#define XUI_TEXTURE_MAG_LINEAR      0
#define XUI_TEXTURE_MAG_POINT       32
#define XUI_TEXTURE_MAG_MASK        32
#define XUI_TEXTURE_MAG_SHIFT       5


// Initialization
HRESULT APIENTRY XuiRenderInit(HWND hWnd, PFN_XUITEXTURELOADER pfnTextureLoader);
HRESULT APIENTRY XuiRenderInitShared(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams, PFN_XUITEXTURELOADER pfnTextureLoader);
HRESULT APIENTRY XuiRenderAttachDevice(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams);
HRESULT APIENTRY XuiRenderUninit();
void APIENTRY XuiRenderResolutionChanged(UINT uWidth, UINT uHeight);

// DC functions
HRESULT APIENTRY XuiRenderBegin(HXUIDC hDC, DWORD dwClearColor);
HRESULT APIENTRY XuiRenderEnd(HXUIDC hDC);
HRESULT APIENTRY XuiRenderGetBackBufferSize(HXUIDC hDC, UINT *puWidth, UINT *puHeight);
HRESULT APIENTRY XuiRenderPresent(HXUIDC hDC, HWND hWnd, const RECT *prctSrc, const RECT *prctDest);
HRESULT APIENTRY XuiRenderCreateDC(HXUIDC *phDC);
HRESULT APIENTRY XuiRenderDCDeviceChanged(HXUIDC hDC);
HRESULT APIENTRY XuiRenderDestroyDC(HXUIDC hDC);
HRESULT APIENTRY XuiRenderRestoreState(HXUIDC hDC);
HRESULT APIENTRY XuiRenderGetDevice(HXUIDC hDC, IDirect3DDevice9 **ppDevice);
HRESULT APIENTRY XuiRenderSetTransform(HXUIDC hDC, const D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiRenderSetViewTransform(HXUIDC hDC, const D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiRenderGetViewTransform(HXUIDC hDC, D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiSelectBrush(HXUIDC hDC, HXUIBRUSH hBrush);
HRESULT APIENTRY XuiSelectBrushEx(HXUIDC hDC, HXUIBRUSH hBrush, DWORD dwFlags);
HRESULT APIENTRY XuiFillRect(HXUIDC hDC, const XUIRect *pRect);
HRESULT APIENTRY XuiDrawShape(HXUIDC hDC, HXUISHAPE hShape);
HRESULT APIENTRY XuiStrokeShape(HXUIDC hDC, HXUISHAPE hShape, float fStrokeWidth, BOOL bClose);
HRESULT APIENTRY XuiSetTextDropShadowColor(HXUIDC hDC, DWORD dwColor);
HRESULT APIENTRY XuiSetColorFactor(HXUIDC hDC, DWORD dwColorFactor);
HRESULT APIENTRY XuiSetBlendMode(HXUIDC hDC, XUI_BLEND_MODE nBlendMode);
HRESULT APIENTRY XuiGetDCStats(HXUIDC hDC, XUIRenderStats *pStats);
HRESULT APIENTRY XuiSetWireFrame(BOOL bWireFrame);
HRESULT APIENTRY XuiSetColorWriteFlags(HXUIDC hDC, DWORD dwColorWriteFlags);

// Brush functions
HRESULT APIENTRY XuiCreateSolidBrush(DWORD dwColor, HXUIBRUSH *phBrush);
HRESULT APIENTRY XuiCreateTextureBrush(LPCWSTR szFileName, HXUIBRUSH *phBrush);
HRESULT APIENTRY XuiAttachTextureBrush(IDirect3DTexture9 *pTex, HXUIBRUSH *phBrush);
HRESULT APIENTRY XuiCreateLinearGradientBrush(int nNumStops, const XUIGradientStop *pStops, HXUIBRUSH *phBrush);
HRESULT APIENTRY XuiCreateRadialGradientBrush(int nNumStops, const XUIGradientStop *pStops, HXUIBRUSH *phBrush);
HRESULT APIENTRY XuiBrushSetXForm(HXUIBRUSH hBrush, const D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiBrushGetDims(HXUIBRUSH hBrush, UINT *puWidth, UINT *puHeight);
HRESULT APIENTRY XuiDestroyBrush(HXUIBRUSH hBrush);
HRESULT APIENTRY XuiBrushGetTexture(HXUIBRUSH hBrush, IDirect3DTexture9 **ppTex);

// Shape functions
HRESULT APIENTRY XuiRenderCreateShape(int nNumVertices, const D3DXVECTOR2 *pVerts, BOOL bReverseWinding, HXUISHAPE *phShape);
HRESULT APIENTRY XuiRenderDestroyShape(HXUISHAPE hShape);
HRESULT APIENTRY XuiShapeGetVertices(HXUISHAPE hShape, D3DXVECTOR2 **ppVerts, int *pnNumVerts, USHORT **ppuIndices, int *pnNumIndices);
HRESULT APIENTRY XuiRenderCreateShapeScale(const XUIFigurePoint *pPoints, int nNumPoints, const D3DXVECTOR2 *pvScalingFactor, HXUISHAPE *phShape);

// Texture functions
HRESULT APIENTRY XuiAddTexture(LPCWSTR szFileName, IDirect3DTexture9 *pTex, const XUIImageInfo *pImageInfo, HXUITEX *phTex);
HXUITEX APIENTRY XuiLookupTexture(LPCWSTR szFileName, IDirect3DTexture9 **ppTex, XUIImageInfo *pImageInfo);
void APIENTRY XuiReleaseTexture(HXUITEX hTexture, BOOL bRemove);
void APIENTRY XuiFreeUnusedTextures(BOOL bFlushAllUnused);
DWORD APIENTRY XuiSetTextureMemoryLimit(DWORD dwMemoryLimit);
               
HRESULT APIENTRY XuiRenderSetTextureLoader(PFN_XUITEXTURELOADER pfnTextureLoader);
HRESULT APIENTRY XuiCreateTextureFromPNG(BYTE *pbInputBuffer, DWORD dwInputBufferLen, IDirect3DDevice9 *pDevice, IDirect3DTexture9 **ppTex, XUIImageInfo *pImageInfo);
// XuiD3DXTextureLoader may be removed in the future in order to eliminate dependency on D3DX. Xui
// clients should implement their own texture loader functions (registered via XuiRenderInit(...), etc.).
HRESULT APIENTRY XuiD3DXTextureLoader(IDirect3DDevice9 *pDevice, LPCWSTR szFileName, XUIImageInfo *pImageInfo, IDirect3DTexture9 **ppTex);
HRESULT APIENTRY XuiPNGTextureLoader(IDirect3DDevice9 *pDevice, LPCWSTR szFileName, XUIImageInfo *pImageInfo, IDirect3DTexture9 **ppTex);

// Text functions

// StyleFlags to be passed to XuiDrawText
#define XUI_FONT_STYLE_NORMAL            0x00000000
#define XUI_FONT_STYLE_DROPSHADOW        0x00000001
#define XUI_FONT_STYLE_NO_WORDWRAP       0x00000010
#define XUI_FONT_STYLE_SINGLE_LINE       0x00000020
#define XUI_FONT_STYLE_LEFT_ALIGN        0x00000100   // This is the default alignment
#define XUI_FONT_STYLE_RIGHT_ALIGN       0x00000200
#define XUI_FONT_STYLE_CENTER_ALIGN      0x00000400 
#define XUI_FONT_STYLE_VERTICAL_CENTER   0x00001000
#define XUI_FONT_STYLE_MEASURE_HEIGHT    0x00002000
#define XUI_FONT_STYLE_ELLIPSIS          0x00004000


HRESULT APIENTRY XuiCreateFont(LPCWSTR lpszTypeface, FLOAT fPointSize, HXUIFONT * phFont);
HRESULT APIENTRY XuiSelectFont(HXUIDC hDC, HXUIFONT hFont);
HRESULT APIENTRY XuiMeasureText(HXUIFONT hFont, LPCWSTR lpszText, int cch, DWORD dwStyleFlags, int iLineSpacingAdjust, XUIRect * pRect);
HRESULT APIENTRY XuiDrawText(HXUIDC hDC, LPCWSTR lpszText, DWORD dwStyleFlags, int iLineSpacingAdjust, XUIRect * pRect);
HRESULT APIENTRY XuiDrawTextEx(HXUIDC hDC, LPCWSTR lpszText, int cch, DWORD dwStyleFlags, int iLineSpacingAdjust, XUIRect * pRect, float vScroll);
HRESULT APIENTRY XuiGetTextBreaks(HXUIFONT hFont, LPCWSTR lpszText, DWORD dwStyleFlags, int iLineSpacingAdjust, BOOL fIgnoreHeight, XUIRect * pRect, UINT *pnCount, UINT ** pLineOffsets);
HRESULT APIENTRY XuiGetFontHeight(HXUIFONT hFont, UINT * pnHeight);
VOID APIENTRY XuiReleaseFont(HXUIFONT hFont);

// Replaceable text renderer definitions

typedef HANDLE HFONTOBJ;

#define XUI_FONT_RENDERER_CAP_INTERNAL_GLYPH_CACHE   0x00000001
#define XUI_FONT_RENDERER_CAP_POINT_SIZE_RESPECTED   0x00000002

#define XUI_FONT_RENDERER_STYLE_DROPSHADOW           0x00000001


typedef struct TypefaceDescriptor
{
    LPCWSTR szTypeface;
    LPCWSTR szLocator;
    LPCWSTR szReserved1;
} TypefaceDescriptor;

struct XUIFontMetrics
{
   DECLARE_XUI_ALLOC()
 
   // units are pixels at current font size

   float fLineHeight; // font line height
   float fMaxAscent;  // max ascent above baseline
   float fMaxDescent; // max descent below baseline
   float fMaxWidth;   // max character width
   float fMaxHeight;  // max character height
   float fMaxAdvance; // max advance
};

struct XUICharMetrics
{
   DECLARE_XUI_ALLOC()

   // units are pixels at current font size

   float fMinX;    // min x coordinate
   float fMinY;    // min y coordinate
   float fMaxX;    // max x coordinate
   float fMaxY;    // max y coordinate
   float fAdvance; // advance value
};

struct CharData
{
    WCHAR wch;
    DWORD dwStyle;
    DWORD dwColor;
    float x, y;
};

struct IXuiFontRendererVTBL
{
    HRESULT (*Init)(IXuiFontRenderer* pThis, float fDpi);
    VOID    (*Term)(IXuiFontRenderer* pThis);
    HRESULT (*GetCaps)(IXuiFontRenderer* pThis, DWORD * pdwCaps);
    HRESULT (*CreateFont)(IXuiFontRenderer* pThis, const TypefaceDescriptor * pTypefaceDescriptor, float fPointSize, HFONTOBJ * phFont);
    VOID    (*ReleaseFont)(IXuiFontRenderer* pThis, HFONTOBJ hFont);
    HRESULT (*GetFontMetrics)(IXuiFontRenderer* pThis, HFONTOBJ hFont, XUIFontMetrics *pFontMetrics);
    HRESULT (*GetCharMetrics)(IXuiFontRenderer* pThis, HFONTOBJ hFont, WCHAR wch, XUICharMetrics *pCharMetrics);
    HRESULT (*DrawCharToTexture)(IXuiFontRenderer* pThis, HFONTOBJ hFont, WCHAR wch, IDirect3DTexture9 * pTexture, UINT x, UINT y, UINT width, UINT height, UINT insetX, UINT insetY);
    HRESULT (*DrawCharsToDevice)(IXuiFontRenderer* pThis, HFONTOBJ hFont, CharData * pCharData, DWORD dwCount, RECT *pClipRect, IDirect3DDevice9 * pDevice, D3DXMATRIX * pWorldViewProj);
};

struct IXuiFontRenderer
{    
#ifdef __cplusplus
    
    virtual HRESULT Init(float fDpi)=0;
    virtual VOID    Term()=0;
    virtual HRESULT GetCaps(DWORD * pdwCaps)=0;
    virtual HRESULT CreateFont(const TypefaceDescriptor * pTypefaceDescriptor, float fPointSize, HFONTOBJ * phFont)=0;
    virtual VOID    ReleaseFont(HFONTOBJ hFont)=0;
    virtual HRESULT GetFontMetrics(HFONTOBJ hFont, XUIFontMetrics *pFontMetrics)=0;
    virtual HRESULT GetCharMetrics(HFONTOBJ hFont, WCHAR wch, XUICharMetrics *pCharMetrics)=0;
    virtual HRESULT DrawCharToTexture(HFONTOBJ hFont, WCHAR wch, IDirect3DTexture9 * pTexture, UINT x, UINT y, UINT width, UINT height, UINT insetX, UINT insetY)=0;
    virtual HRESULT DrawCharsToDevice(HFONTOBJ hFont, CharData * pCharData, DWORD dwCount, RECT *pClipRect, IDirect3DDevice9 * pDevice, D3DXMATRIX * pWorldViewProj)=0;

#else
 
    struct IXuiFontRendererVTBL * pVtbl;

#endif
};

HRESULT APIENTRY XuiFontSetRenderer(IXuiFontRenderer * pRenderer);


// Helper functions
HRESULT APIENTRY XuiCreateGradientTexture(int nWidth, int nNumStops, const XUIGradientStop *pStops, IDirect3DTexture9 **pTex);
BOOL APIENTRY XuiPointInRect(const XUIRect *pRect, const D3DXVECTOR2 *pPoint);

#ifdef __cplusplus
}
#endif

#endif // __XUIRENDER_H__

