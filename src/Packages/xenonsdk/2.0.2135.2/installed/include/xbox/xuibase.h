/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuibase.h
 *  Content:    Xbox 360 UI runtime defines, structures and functions
 *
 ****************************************************************************/

#ifndef __XUIBASE_H__
#define __XUIBASE_H__

#pragma warning(push)
#pragma warning(disable: 4201)  // unnamed struct/union

#define XUI_VERSION     0x000C      // high byte is the major version, and the low byte is the minor version

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XUIOBJ* HXUIOBJ;
typedef struct _XUICLASS* HXUICLASS;
typedef struct _XUISTRINGTABLE* HXUISTRINGTABLE;
typedef struct XUICustomPropHandler XUICustomPropHandler;
typedef struct XUIMemoryBuffer XUIMemoryBuffer;
typedef struct IXUIPropObj IXUIPropObj;
typedef struct IXUIPropObjVTBL IXUIPropObjVTBL;
typedef struct XUIQuaternion XUIQuaternion;
typedef struct XUIMessage XUIMessage;
typedef struct XUIObjectMethods XUIObjectMethods;
typedef struct XUIClass XUIClass;
typedef struct XUIInitParams XUIInitParams;
typedef struct IXUIRuntimeHooks IXUIRuntimeHooks;
typedef struct IXUIRuntimeHooksVTBL IXUIRuntimeHooksVTBL;
typedef struct XUIElementPropDef XUIElementPropDef;
typedef struct XUIElementPropVal XUIElementPropVal;
typedef enum XUI_ELEMENT_PROPERTY_TYPE XUI_ELEMENT_PROPERTY_TYPE;

typedef const XUIElementPropDef* (APIENTRY *PFN_GETPROPDEF)(DWORD *pdwCount);

typedef HRESULT (APIENTRY *PFN_GETPROP)(DWORD dwIndex, PVOID pvObj, XUIElementPropVal *pVal);
typedef HRESULT (APIENTRY *PFN_SETPROP)(DWORD dwIndex, PVOID pvObj, const XUIElementPropVal *pVal);
typedef HRESULT (APIENTRY *PFN_GETCOUNT)(PVOID pvObj, DWORD *pdwCount);

struct IXUIPropObjVTBL
{
    const XUIElementPropDef* (*GetPropDef)(IXUIPropObj* pThis, DWORD *pdwCount);
    HRESULT (*GetPropertyId)(IXUIPropObj* pThis, LPCWSTR szPropName, DWORD *pdwId);
    HRESULT (*GetPropertyDef)(IXUIPropObj* pThis, LPCWSTR szPropName, const XUIElementPropDef **ppDef);
    HRESULT (*GetPropertyRef)(IXUIPropObj* pThis, DWORD dwPropId, void **ppvData);
    HRESULT (*GetProperty)(IXUIPropObj* pThis, DWORD dwPropId, DWORD dwIndex, XUIElementPropVal *pVal);
    HRESULT (*SetProperty)(IXUIPropObj* pThis, DWORD dwPropId, DWORD dwIndex, const XUIElementPropVal *pVal);
    HRESULT (*GetPropertyCount)(IXUIPropObj* pThis, DWORD dwPropId, DWORD *pdwCount);
};

struct IXUIPropObj
{
#ifdef __cplusplus

    virtual const XUIElementPropDef *GetPropDef(DWORD *pdwCount) = 0;
    virtual HRESULT GetPropertyId(LPCWSTR szPropName, DWORD *pdwId) = 0;
    virtual HRESULT GetPropertyDef(LPCWSTR szPropName, const XUIElementPropDef **ppDef) = 0;
    virtual HRESULT GetPropertyRef(DWORD dwPropId, void **ppvData) = 0;
    virtual HRESULT GetProperty(DWORD dwPropId, DWORD dwIndex, XUIElementPropVal *pVal) = 0;
    virtual HRESULT SetProperty(DWORD dwPropId, DWORD dwIndex, const XUIElementPropVal *pVal) = 0;
    virtual HRESULT GetPropertyCount(DWORD dwPropId, DWORD *pdwCount) = 0;

#else

    struct IXUIPropObjVTBL* pVtbl;

#endif
};

void APIENTRY XUIMemoryBuffer_Construct(XUIMemoryBuffer* pThis);
void APIENTRY XUIMemoryBuffer_Destruct(XUIMemoryBuffer* pThis);
UINT APIENTRY XUIMemoryBuffer_Append(XUIMemoryBuffer* pThis, VOID * pv, UINT cb);
UINT APIENTRY XUIMemoryBuffer_AppendUint(XUIMemoryBuffer* pThis, UINT ui);
UINT APIENTRY XUIMemoryBuffer_AppendByte(XUIMemoryBuffer* pThis, BYTE b);
UINT APIENTRY XUIMemoryBuffer_AppendUlong(XUIMemoryBuffer* pThis, ULONG ul);
UINT APIENTRY XUIMemoryBuffer_AppendUshort(XUIMemoryBuffer* pThis, USHORT us);
UINT APIENTRY XUIMemoryBuffer_AppendBool(XUIMemoryBuffer* pThis, BOOL f);
UINT APIENTRY XUIMemoryBuffer_AppendInt(XUIMemoryBuffer* pThis, int i);
UINT APIENTRY XUIMemoryBuffer_AppendFloat(XUIMemoryBuffer* pThis, FLOAT f);
UINT APIENTRY XUIMemoryBuffer_AppendWchar(XUIMemoryBuffer* pThis, WCHAR * pwsz);
UINT APIENTRY XUIMemoryBuffer_AppendMemoryBuffer(XUIMemoryBuffer* pThis, XUIMemoryBuffer * pBuf);
UINT APIENTRY XUIMemoryBuffer_RewindBytes(XUIMemoryBuffer* pThis, UINT cb);
VOID APIENTRY XUIMemoryBuffer_WriteByte(XUIMemoryBuffer* pThis, ULONG pos, BYTE data);
VOID APIENTRY XUIMemoryBuffer_WriteUshort(XUIMemoryBuffer* pThis, ULONG pos, USHORT data);
VOID APIENTRY XUIMemoryBuffer_WriteUlong(XUIMemoryBuffer* pThis, ULONG pos, ULONG data);
BYTE* APIENTRY XUIMemoryBuffer_Data(XUIMemoryBuffer* pThis);
UINT APIENTRY XUIMemoryBuffer_Size(XUIMemoryBuffer* pThis);
BOOL APIENTRY XUIMemoryBuffer_Init(XUIMemoryBuffer* pThis, UINT cb);

struct XUIMemoryBuffer
{
    DECLARE_XUI_ALLOC()

    BYTE * m_pb;
    UINT m_cbUsed;
    UINT m_cbTotal;

#ifdef __cplusplus

    XUIMemoryBuffer()
    {
        XUIMemoryBuffer_Construct(this);
    }
    ~XUIMemoryBuffer()
    {
        XUIMemoryBuffer_Destruct(this);
    }
    UINT Append(VOID * pv, UINT cb)
    {
        return XUIMemoryBuffer_Append(this, pv, cb);
    }
    UINT Append(UINT ui)
    {
        return XUIMemoryBuffer_AppendUint(this, ui);
    }
    UINT Append(BYTE b)
    {
        return XUIMemoryBuffer_AppendByte(this, b);
    }
    UINT Append(ULONG ul)
    {
        return XUIMemoryBuffer_AppendUlong(this, ul);
    }
    UINT Append(USHORT us)
    {
        return XUIMemoryBuffer_AppendUshort(this, us);
    }
    UINT AppendBool(BOOL f)
    {
        return XUIMemoryBuffer_AppendBool(this, f);
    }
    UINT Append(int i)
    {
        return XUIMemoryBuffer_AppendInt(this, i);
    }
    UINT Append(FLOAT f)
    {
        return XUIMemoryBuffer_AppendFloat(this, f);
    }
    UINT Append(WCHAR * pwsz)
    {
        return XUIMemoryBuffer_AppendWchar(this, pwsz);
    }
    UINT Append(XUIMemoryBuffer * pBuf)
    {
        return XUIMemoryBuffer_AppendMemoryBuffer(this, pBuf);
    }
    UINT RewindBytes(UINT cb)
    {
        return XUIMemoryBuffer_RewindBytes(this, cb);
    }
    VOID WriteByte(ULONG pos, BYTE data)
    {
        XUIMemoryBuffer_WriteByte(this, pos, data);
    }
    VOID WriteUshort(ULONG pos, USHORT data)
    {
        XUIMemoryBuffer_WriteUshort(this, pos, data);
    }
    VOID WriteUlong(ULONG pos, ULONG data)
    {
        XUIMemoryBuffer_WriteUlong(this, pos, data);
    }
    BYTE * Data()
    {
        return XUIMemoryBuffer_Data(this);
    }
    UINT Size()
    {
        return XUIMemoryBuffer_Size(this);
    }
    BOOL Init(UINT cb)
    {
        return XUIMemoryBuffer_Init(this, cb);
    }
#endif
};

struct XUICustomPropHandler
{
    HRESULT (*Set)(void *pvVal, void *pvSrc, void *pvThis);
    HRESULT (*Get)(void *pvVal, void *pvDest, void *pvThis);
    HRESULT (*ToString)(void *pvVal, WCHAR **ppszDest, void *pvThis);
    HRESULT (*FromString)(void *pvVal, LPCWSTR szSrc, void *pvThis);
    HRESULT (*ToBinary)(void *pvVal, XUIMemoryBuffer * pDest, void *pvThis);
    HRESULT (*FromBinary)(void *pvVal, const BYTE * pb, UINT cb, void *pvThis);
    BOOL (*IsEqual)(void *pvVal, void *pvOther, void *pvThis);
    HRESULT (*ShowUI)(void *pvThis, IXUIPropObj *pObj, HWND hWndParent);
    // TODO: think about undo/redo, save/load for custom handlers
};

enum XUI_ELEMENT_PROPERTY_TYPE
{
    XUI_EPT_EMPTY,
    XUI_EPT_BOOL,
    XUI_EPT_INTEGER,
    XUI_EPT_UNSIGNED,
    XUI_EPT_FLOAT,
    XUI_EPT_STRING,
    XUI_EPT_COLOR,
    XUI_EPT_VECTOR,
    XUI_EPT_QUATERNION,
    XUI_EPT_OBJECT,
    XUI_EPT_CUSTOM
};

struct XUIElementPropDef
{
    DECLARE_XUI_ALLOC()

    DWORD dwFlags;
    DWORD dwId;
    DWORD dwOffset;
    DWORD dwExtra;
    LPCWSTR szPropName;
    XUI_ELEMENT_PROPERTY_TYPE Type;
    const XUIElementPropVal *pDefaultVal;
    XUICustomPropHandler *pCustomHandler;
    PFN_GETPROP pfnGetProp;
    PFN_SETPROP pfnSetProp;
    PFN_GETPROPDEF pfnGetPropDef;
    PFN_GETCOUNT pfnGetCount;

#ifndef _XBOX
    LPCWSTR szPropertyEditor;
#endif
};

// property flags
#define XUI_ELEMENT_PROPF_NONE      0x0000       // no flags specified
#define XUI_ELEMENT_PROPF_INDEXED   0x0001       // the property is indexed
#define XUI_ELEMENT_PROPF_HIDDEN    0x0002       // the property is hidden in the design time environment
#define XUI_ELEMENT_PROPF_LOCALIZE  0x0004       // the property is localizable
#define XUI_ELEMENT_PROPF_NOANIM    0x0008       // the property must not be animated
#define XUI_ELEMENT_PROPF_FILEPATH  0x0010       // the property represents a file path

struct XUIQuaternion
{
    DECLARE_XUI_ALLOC()

    float x;
    float y;
    float z;
    float w;

#ifdef __cplusplus

    D3DXQUATERNION& operator=(const D3DXQUATERNION& q)
    {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
        return *((D3DXQUATERNION *) this);
    }

    operator D3DXQUATERNION&()
    {
        return *((D3DXQUATERNION *) this);
    }

#endif
};

void APIENTRY XUIElementPropVal_Construct(XUIElementPropVal* pThis);
void APIENTRY XUIElementPropVal_Destruct(XUIElementPropVal* pThis);
void APIENTRY XUIElementPropVal_ConstructFromInt(XUIElementPropVal* pThis, int n);
void APIENTRY XUIElementPropVal_ConstructFromUint(XUIElementPropVal* pThis, unsigned int u);
void APIENTRY XUIElementPropVal_ConstructFromFloat(XUIElementPropVal* pThis, float f);
void APIENTRY XUIElementPropVal_ConstructFromD3DXVECTOR(XUIElementPropVal* pThis, const D3DXVECTOR3* vOther);
void APIENTRY XUIElementPropVal_ConstructFromD3DXQUATERNION(XUIElementPropVal* pThis, const D3DXQUATERNION* qOther);
void APIENTRY XUIElementPropVal_Clear(XUIElementPropVal* pThis);
void APIENTRY XUIElementPropVal_Reset(XUIElementPropVal* pThis);
void APIENTRY XUIElementPropVal_SetBool(XUIElementPropVal* pThis, BOOL val);
void APIENTRY XUIElementPropVal_SetInt(XUIElementPropVal* pThis, int val);
void APIENTRY XUIElementPropVal_SetUint(XUIElementPropVal* pThis, unsigned int val);
void APIENTRY XUIElementPropVal_SetFloat(XUIElementPropVal* pThis, float val);
void APIENTRY XUIElementPropVal_SetColorFromUint(XUIElementPropVal* pThis, unsigned int val);
void APIENTRY XUIElementPropVal_SetColorFromARGB(XUIElementPropVal* pThis, BYTE a, BYTE r, BYTE g, BYTE b);
HRESULT APIENTRY XUIElementPropVal_SetString(XUIElementPropVal* pThis, LPCWSTR val);
void APIENTRY XUIElementPropVal_SetD3DVECTOR(XUIElementPropVal* pThis, const D3DVECTOR *val);
void APIENTRY XUIElementPropVal_SetD3DVECTOR3(XUIElementPropVal* pThis, const D3DXVECTOR3 *val);
void APIENTRY XUIElementPropVal_SetXUIQuaternion(XUIElementPropVal* pThis, const XUIQuaternion *val);
void APIENTRY XUIElementPropVal_SetD3DXQUATERNION(XUIElementPropVal* pThis, const D3DXQUATERNION *val);
void APIENTRY XUIElementPropVal_SetIXUIPropObj(XUIElementPropVal* pThis, IXUIPropObj *pObject);
void APIENTRY XUIElementPropVal_SetCustom(XUIElementPropVal* pThis, void *pvData, XUICustomPropHandler *pSrcHandler);
HRESULT APIENTRY XUIElementPropVal_Copy(XUIElementPropVal* pThis, const XUIElementPropVal *pVal);
HRESULT APIENTRY XUIElementPropVal_GetStringLen(XUIElementPropVal* pThis, size_t *pdwLen);
HRESULT APIENTRY XUIElementPropVal_ToString(XUIElementPropVal* pThis, WCHAR **szDest);
HRESULT APIENTRY XUIElementPropVal_ToBinary(XUIElementPropVal* pThis, XUIMemoryBuffer * pDest);
BOOL APIENTRY XUIElementPropVal_IsEqual(XUIElementPropVal* pThis, const XUIElementPropVal *pOther);

struct XUIElementPropVal
{
    DECLARE_XUI_ALLOC()

    XUI_ELEMENT_PROPERTY_TYPE type;
    union
    {
        BOOL bVal;
        int nVal;
        unsigned int uVal;
        float fVal;
        WCHAR *szVal;
        D3DVECTOR vecVal;
        XUIQuaternion quatVal;
        IXUIPropObj *pObj;
        struct
        {
            void *pvCustomData; // pointer to custom data type
            XUICustomPropHandler *pHandler;
        } CustomVal;
    };

#ifdef __cplusplus

    XUIElementPropVal()
    {
        XUIElementPropVal_Construct(this);
    }
    ~XUIElementPropVal()
    {
        XUIElementPropVal_Destruct(this);
    }
    XUIElementPropVal(int n)
    {
        XUIElementPropVal_ConstructFromInt(this, n);
    }
    XUIElementPropVal(unsigned int u)
    {
        XUIElementPropVal_ConstructFromUint(this, u);
    }
    XUIElementPropVal(float f)
    {
        XUIElementPropVal_ConstructFromFloat(this, f);
    }
    XUIElementPropVal(const D3DXVECTOR3& vOther)
    {
        XUIElementPropVal_ConstructFromD3DXVECTOR(this, &vOther);
    }
    XUIElementPropVal(const D3DXQUATERNION& qOther)
    {
        XUIElementPropVal_ConstructFromD3DXQUATERNION(this, &qOther);
    }
    void Clear()
    {
        XUIElementPropVal_Clear(this);
    }
    void Reset()
    {
        XUIElementPropVal_Reset(this);
    }
    void SetVal(bool val)
    {
        XUIElementPropVal_SetBool(this, (BOOL) val);
    }
    void SetBoolVal(BOOL val)
    {
        XUIElementPropVal_SetBool(this, val);
    }
    void SetVal(int val)
    {
        XUIElementPropVal_SetInt(this, val);
    }
    void SetVal(unsigned int val)
    {
        XUIElementPropVal_SetUint(this, val);
    }
    void SetVal(float val)
    {
        XUIElementPropVal_SetFloat(this, val);
    }
    void SetColorVal(unsigned int val)
    {
        XUIElementPropVal_SetColorFromUint(this, val);
    }
    void SetColorVal(BYTE a, BYTE r, BYTE g, BYTE b)
    {
        XUIElementPropVal_SetColorFromARGB(this, a, r, g, b);
    }
    HRESULT SetVal(LPCWSTR val)
    {
        return XUIElementPropVal_SetString(this, val);
    }
    void SetVal(const D3DVECTOR *val)
    {
        XUIElementPropVal_SetD3DVECTOR(this, val);
    }
    void SetVal(const D3DVECTOR &val)
    {
        XUIElementPropVal_SetD3DVECTOR(this, &val);
    }
    void SetVal(const D3DXVECTOR3 *val)
    {
        XUIElementPropVal_SetD3DVECTOR(this, val);
    }
    void SetVal(const XUIQuaternion *val)
    {
        XUIElementPropVal_SetXUIQuaternion(this, val);
    }
    void SetVal(const D3DXQUATERNION &val)
    {
        XUIElementPropVal_SetD3DXQUATERNION(this, &val);
    }
    void SetVal(const D3DXQUATERNION *val)
    {
        XUIElementPropVal_SetD3DXQUATERNION(this, val);
    }
    void SetVal(IXUIPropObj *pObject)
    {
        XUIElementPropVal_SetIXUIPropObj(this, pObject);
    }
    void SetVal(void *pvData, XUICustomPropHandler *pSrcHandler)
    {
        XUIElementPropVal_SetCustom(this, pvData, pSrcHandler);
    }
    HRESULT Copy(const XUIElementPropVal *pVal)
    {
        return XUIElementPropVal_Copy(this, pVal);
    }
    HRESULT GetStringLen(size_t *pdwLen)
    {
        return XUIElementPropVal_GetStringLen(this, pdwLen);
    }
    HRESULT ToString(WCHAR **szDest)
    {
        return XUIElementPropVal_ToString(this, szDest);
    }
    HRESULT ToBinary(XUIMemoryBuffer * pDest)
    {
        return XUIElementPropVal_ToBinary(this, pDest);
    }
    BOOL IsEqual(const XUIElementPropVal *pOther)
    {
        return XUIElementPropVal_IsEqual(this, pOther);
    }
#endif
};

#define XUI_BEGIN_PROPERTY_MAP(className)\
    static const XUIElementPropDef *_GetPropDef(DWORD *pdwCount)\
    { \
        int nIndex=0; \
        typedef className _currClass; \
        static XUIElementPropDef _defs[] = \
        {

#define XUI_IMPLEMENT_EMPTY_PROPERTY_MAP(className)\
    static const XUIElementPropDef *_GetPropDef(DWORD *pdwCount)\
    { \
        *pdwCount = 0; \
        return NULL; \
    }


#ifdef _XBOX
//
// These macros map only the values valid at runtime.
//

#define XUI_IMPLEMENT_PROPERTY(propName, propMember, propType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EXTRA(propName, propMember, extra, propType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EDITOR(propName, propMember, propType, pDefVal, szPropEditor)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_GET_SET(propName, propMember, propType, pDefVal, GetFunc, SetFunc, szPropEditor)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_GET_SET_FLAGS(flags, propName, propMember, propType, pDefVal, GetFunc, SetFunc, szPropEditor)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_FLAGS(flags, propName, propMember, propType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EXTRA_FLAGS(flags, propName, propMember, extra, propType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_FLAGS(flags, propName, propMember, propType, pDefVal, szPropEditor)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_INDEXED_PROPERTY(propName, propMember, propType, pDefVal, GetFunc, SetFunc, GetCountFunc)\
    { XUI_ELEMENT_PROPF_INDEXED, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc, NULL, GetCountFunc },

#define XUI_IMPLEMENT_COMPOUND_PROPERTY(propName, propMember, propCompoundType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_OBJECT, pDefVal, NULL, NULL, NULL, propCompoundType::_GetPropDef },

#define XUI_IMPLEMENT_COMPOUND_PROPERTY_FLAGS(flags, propName, propMember, propCompoundType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_OBJECT, pDefVal, NULL, NULL, NULL, propCompoundType::_GetPropDef },

#define XUI_IMPLEMENT_PROPERTY_GET_SET(propName, propMember, propType, pDefVal, GetFunc, SetFunc)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_EXTRA(propName, propMember, extra, propType, pDefVal, GetFunc, SetFunc)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_FLAGS(flags, propName, propMember, propType, pDefVal, GetFunc, SetFunc)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_FLAGS_EXTRA(flags, propName, propMember, extra, propType, pDefVal, GetFunc, SetFunc)\
    { flags, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_CUSTOM(flags, propName, propMember, pDefVal, pHandler)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_CUSTOM, pDefVal, pHandler },

#else
//
// These macros map design time values also.
//

#define XUI_IMPLEMENT_PROPERTY(propName, propMember, propType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EXTRA(propName, propMember, extra, propType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EDITOR(propName, propMember, propType, pDefVal, szPropEditor)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, NULL, NULL, NULL, NULL, szPropEditor },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_GET_SET(propName, propMember, propType, pDefVal, GetFunc, SetFunc, szPropEditor)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc, NULL, NULL, szPropEditor },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_GET_SET_FLAGS(flags, propName, propMember, propType, pDefVal, GetFunc, SetFunc, szPropEditor)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc, NULL, NULL, szPropEditor },

#define XUI_IMPLEMENT_PROPERTY_FLAGS(flags, propName, propMember, propType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EXTRA_FLAGS(flags, propName, propMember, extra, propType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal },

#define XUI_IMPLEMENT_PROPERTY_EDITOR_FLAGS(flags, propName, propMember, propType, pDefVal, szPropEditor)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, NULL, NULL, NULL, NULL, szPropEditor },

#define XUI_IMPLEMENT_INDEXED_PROPERTY(propName, propMember, propType, pDefVal, GetFunc, SetFunc, GetCountFunc)\
    { XUI_ELEMENT_PROPF_INDEXED, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc, NULL, GetCountFunc },

#define XUI_IMPLEMENT_COMPOUND_PROPERTY(propName, propMember, propCompoundType, pDefVal)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_OBJECT, pDefVal, NULL, NULL, NULL, propCompoundType::_GetPropDef },

#define XUI_IMPLEMENT_COMPOUND_PROPERTY_FLAGS(flags, propName, propMember, propCompoundType, pDefVal)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_OBJECT, pDefVal, NULL, NULL, NULL, propCompoundType::_GetPropDef },

#define XUI_IMPLEMENT_PROPERTY_GET_SET(propName, propMember, propType, pDefVal, GetFunc, SetFunc)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_EXTRA(propName, propMember, extra, propType, pDefVal, GetFunc, SetFunc)\
    { XUI_ELEMENT_PROPF_NONE, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_FLAGS(flags, propName, propMember, propType, pDefVal, GetFunc, SetFunc)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_GET_SET_FLAGS_EXTRA(flags, propName, propMember, extra, propType, pDefVal, GetFunc, SetFunc)\
    { flags, nIndex++, offsetof(_currClass, propMember), extra, propName, propType, pDefVal, NULL, GetFunc, SetFunc },

#define XUI_IMPLEMENT_PROPERTY_CUSTOM(flags, propName, propMember, pDefVal, pHandler)\
    { flags, nIndex++, offsetof(_currClass, propMember), 0, propName, XUI_EPT_CUSTOM, pDefVal, pHandler },

#endif

#define XUI_END_PROPERTY_MAP() \
        }; \
        *pdwCount = sizeof(_defs)/sizeof(_defs[0]);\
        return _defs;\
        }

struct XUIMessage
{
    DWORD cbSize;
    DWORD dwMessage;
    BOOL bHandled;
    DWORD cbData;
    void *pvData;
};

typedef HRESULT (APIENTRY *PFN_OBJECT_PROC)(HXUIOBJ hObj, XUIMessage *pMessage, void *pvThis);
typedef HRESULT (APIENTRY *PFN_CREATEINST)(HXUIOBJ hObj, void **ppvObj);
typedef HRESULT (APIENTRY *PFN_DESTROYINST)(void *pvObj);
typedef HRESULT (APIENTRY *PFN_MESSAGE_FILTER)(HXUIOBJ hObj, XUIMessage *pMessage);

struct XUIObjectMethods
{
    PFN_OBJECT_PROC ObjectProc;
    PFN_CREATEINST CreateInstance;
    PFN_DESTROYINST DestroyInstance;
};

struct XUIClass
{
    DWORD cbSize;
    LPCWSTR szClassName;
    LPCWSTR szBaseClassName;
    XUIObjectMethods Methods;
    const XUIElementPropDef *pPropDefs;
    DWORD dwPropDefCount;
};

// XUI Init Parameters

#define XUI_INIT_PARAMS_FLAGS_NONE          0x00000000

// Runtime hook functions should return E_NOTIMPL if they choose not to provide any behavior.
struct IXUIRuntimeHooksVTBL
{
    HRESULT (*RewriteResourceLocator)(IXUIRuntimeHooks* pThis, LPCWSTR szLocator, LPWSTR * pszNewLocator);
    HRESULT (*FindClassByName)(IXUIRuntimeHooks* pThis, LPCWSTR szClassName, HXUICLASS *phClass);
    HRESULT (*GetTextPresenterText)(IXUIRuntimeHooks* pThis, HXUIOBJ hObj, LPCWSTR * pszText);
    HRESULT (*LogMessage)(IXUIRuntimeHooks* pThis, HRESULT hr, LPCWSTR pszText);
};

struct IXUIRuntimeHooks
{
#ifdef __cplusplus
    
    virtual HRESULT RewriteResourceLocator(LPCWSTR szLocator, LPWSTR * pszNewLocator) = 0;
    virtual HRESULT FindClassByName(LPCWSTR szClassName, HXUICLASS *phClass) = 0;
    virtual HRESULT GetTextPresenterText(HXUIOBJ hObj, LPCWSTR * pszText) = 0;
    virtual HRESULT LogMessage(HRESULT hr, LPCWSTR pszText) = 0;

#else

    struct IXUIRuntimeHooksVTBL * pVtbl;

#endif
};

#define XUI_INIT_PARAMS(InitParams) \
        memset(&(InitParams), 0, sizeof(XUIInitParams)); \
        (InitParams).cbSize = sizeof(XUIInitParams);        

struct XUIInitParams
{
    DWORD cbSize;
    DWORD dwFlags;
    IXUIRuntimeHooks * pHooks;
};

// public APIs
HRESULT APIENTRY XuiInit(const XUIInitParams *pInitParams);
HRESULT APIENTRY XuiUninit(void);
IXUIRuntimeHooks* APIENTRY XuiGetRuntimeHooks(void);
HRESULT APIENTRY XuiSetRuntimeHooks(IXUIRuntimeHooks* pHooks);
USHORT APIENTRY XuiGetVersion(void);
HRESULT APIENTRY XuiLoadFromBinary(LPCWSTR szFileName, HXUIOBJ * phRootObj, BOOL bInitNow);
HXUICLASS APIENTRY XuiFindClass(LPCWSTR szClassName);
BOOL APIENTRY XuiClassDerivesFrom(HXUICLASS hClass, HXUICLASS hClassBase);
HRESULT APIENTRY XuiRegisterClass(const XUIClass *pClass, HXUICLASS *phClass);
HRESULT APIENTRY XuiUnregisterClass(LPCWSTR szClassName);
HRESULT APIENTRY XuiGetClass(HXUICLASS hClass, XUIClass *pClass);
HRESULT APIENTRY XuiGetBaseClass(HXUICLASS hClass, HXUICLASS * phBaseClass);
HRESULT APIENTRY XuiClassGetPropDef(HXUICLASS hClass, const XUIElementPropDef **ppDef, DWORD *pdwCount);
HRESULT APIENTRY XuiClassGetPropertyDefById(HXUICLASS hClass, DWORD dwPropId, const XUIElementPropDef **ppDef);
HRESULT APIENTRY XuiClassGetPropertyDef(HXUICLASS hClass, LPCWSTR szPropName, const XUIElementPropDef **ppDef);
HRESULT APIENTRY XuiObjectFromHandle(HXUIOBJ hObj, void **ppObj);
HRESULT APIENTRY XuiGetBaseObject(HXUIOBJ hObj, HXUIOBJ *phBaseObj);
HXUICLASS APIENTRY XuiGetObjectClass(HXUIOBJ hObj);
HRESULT APIENTRY XuiCreateObject(LPCWSTR szClassName, HXUIOBJ *phObj);
HRESULT APIENTRY XuiDestroyObject(HXUIOBJ hObj);
BOOL APIENTRY XuiHandleIsValid(HXUIOBJ hObj);
BOOL APIENTRY XuiIsInstanceOf(HXUIOBJ hObj, HXUICLASS hClass);
HXUIOBJ APIENTRY XuiDynamicCast(HXUIOBJ hObj, HXUICLASS hClass);
HRESULT APIENTRY XuiSendMessage(HXUIOBJ hObj, XUIMessage *pMessage);
HRESULT APIENTRY XuiBubbleMessage(HXUIOBJ hObj, XUIMessage *pMessage);
HRESULT APIENTRY XuiBroadcastMessage(HXUIOBJ hObj, XUIMessage *pMessage);
HXUIOBJ APIENTRY XuiGetOuter(HXUIOBJ hObj);
PFN_MESSAGE_FILTER APIENTRY XuiSetMessageFilter(PFN_MESSAGE_FILTER pfnFilter);
HRESULT APIENTRY XuiSetTimer(HXUIOBJ hObj, DWORD nId, DWORD nElapse);
HRESULT APIENTRY XuiKillTimer(HXUIOBJ hObj, DWORD nId);
HRESULT APIENTRY XuiTimersRun(void);

HRESULT APIENTRY XuiElementPropValFromString(LPCWSTR szText, XUI_ELEMENT_PROPERTY_TYPE type, XUIElementPropVal *pVal);
HRESULT APIENTRY XuiElementPropValFromBinary(const BYTE * pb, UINT * pcb, XUI_ELEMENT_PROPERTY_TYPE type, XUIElementPropVal *pVal);

const XUIElementPropDef* APIENTRY XuiObjectGetPropDef(HXUIOBJ hObj, DWORD *pdwCount);
HRESULT APIENTRY XuiObjectGetPropertyId(HXUIOBJ hObj, LPCWSTR szPropName, DWORD *pdwId);
HRESULT APIENTRY XuiObjectGetPropertyDefById(HXUIOBJ hObj, DWORD dwPropId, const XUIElementPropDef **ppDef);
HRESULT APIENTRY XuiObjectGetPropertyDef(HXUIOBJ hObj, LPCWSTR szPropName, const XUIElementPropDef **ppDef);
HRESULT APIENTRY XuiObjectGetProperty(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, XUIElementPropVal *pVal);
HRESULT APIENTRY XuiObjectSetProperty(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, const XUIElementPropVal *pVal);
HRESULT APIENTRY XuiObjectGetPropertyRef(HXUIOBJ hObj, DWORD dwPropId, void **ppvData);
HRESULT APIENTRY XuiObjectGetPropertyCount(HXUIOBJ hObj, DWORD dwPropId, DWORD *pdwCount);

HRESULT APIENTRY XuiLoadStringTableFromFile(LPCWSTR szFile, HXUISTRINGTABLE *ph);
HRESULT APIENTRY XuiLoadStringTableFromMemory(PVOID pData, HXUISTRINGTABLE *ph);
HRESULT APIENTRY XuiFreeStringTable(HXUISTRINGTABLE h);
LPCWSTR APIENTRY XuiLookupStringTable(HXUISTRINGTABLE h, LPCWSTR szKey);
LPCWSTR APIENTRY XuiLookupStringTableByIndex(HXUISTRINGTABLE h, DWORD nIndex);

HRESULT APIENTRY XuiMuteSound(BOOL bMute);

HRESULT APIENTRY XuiSetLocale(LPCWSTR szLocale);
LPCWSTR APIENTRY XuiGetLocale(void);
HRESULT APIENTRY XuiApplyLocale(HXUIOBJ hObj, HXUISTRINGTABLE hStringTable);

HRESULT APIENTRY XuiCopyString(LPWSTR *ppszOut, LPCWSTR szIn);

#ifdef __cplusplus
}
#endif

#pragma warning(pop)

#endif // __XUIBASE_H__

