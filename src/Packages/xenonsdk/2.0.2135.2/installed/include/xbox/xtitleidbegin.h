/************************************************************************
*                                                                       *
*   XTitleIDBegin.h -- This module is used to specify title ID          *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/

#pragma data_seg(".XEXID")
#pragma comment(linker, "/include:_XEXTitleID")
#ifdef __cplusplus
#define XEX_TITLE_ID_EXTERN_C extern "C"
#else
#define XEX_TITLE_ID_EXTERN_C
#endif
#define XEX_TITLE_ID(TitleID) \
    XEX_TITLE_ID_EXTERN_C ULONG _XEXTitleID = (TitleID);
