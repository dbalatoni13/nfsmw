/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pix.h
 *  Content:    Xenon PIX include file
 *
 ****************************************************************************/

#ifndef _PIX_H_
#define _PIX_H_

#if ((defined(_DEBUG) || DBG || (defined(PROFILE) && !defined(FASTCAP)) || defined(PROFILE_BUILD)) && !defined(i386))
#define USE_PIX
#endif

#ifdef  __cplusplus
extern "C" {
#endif

void WINAPI PIXAddNamedCounter( float value, const char *szName, ...);

#if defined(USE_PIX)

    void WINAPI PIXBeginNamedEvent( DWORD Color, const char *szName, ...);
    void WINAPI PIXEndNamedEvent( );
    void WINAPI PIXSetMarker( DWORD Color, const char *szName, ...);
    void WINAPI PIXNameThread( const char *szName );
    
#else // USE_PIX
    
    // eliminate these APIs in Release builds
    #define PIXBeginNamedEvent 0&&
    #define PIXEndNamedEvent()
    #define PIXSetMarker 0&&
    #define PIXNameThread 0&&

    // don't show warnings about expressions with no effect
    #pragma warning(disable:4548)		
    #pragma warning(disable:4555)
    
#endif // USE_PIX

#ifdef  __cplusplus
}
#endif


#endif // _PIX_H_
