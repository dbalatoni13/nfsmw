/*++ BUILD Version: 0000     Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    xtl.h

Abstract:

    Master include file for Xbox applications.

--*/
#pragma once

#ifndef _XTL_
#define _XTL_

#ifndef WINVER
#define WINVER 0x0500
#endif



#ifndef _INC_XTL
#define _INC_XTL

#include <xdk.h>
#define _XTL_VER _XDK_VER

#ifndef _INC_WINDOWS
#define _INC_WINDOWS
#endif  /* _INC_WINDOWS */

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _PPC_
#define _PPC_
#endif  /* _PPC_ */

#ifndef _XBOX_VER
#define _XBOX_VER 200
#endif  /* _XBOX_VER */

#if     ( _MSC_VER >= 800 )
#pragma warning(disable:4100)
#pragma warning(disable:4514)
#ifndef __WINDOWS_DONT_DISABLE_PRAGMA_PACK_WARNING__
#pragma warning(disable:4103)
#endif
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#endif

#include <excpt.h>
#include <stdarg.h>

#include <windef.h>
#include <winbase.h>
#include <xbox.h>
#include <xam.h>
#include <pix.h>

#include <stdlib.h>

#ifndef NOD3D
#include <d3d9.h>
#include <d3dx9.h>
#endif  /* NOD3D */

#ifndef NONET
#include <winsockx.h>
#include <xonline.h>
#endif  /* NONET */

#if     ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4001)
#pragma warning(default:4201)
#pragma warning(default:4214)
/* Leave 4100 disabled.  Unreferenced parameter for inline methods. */
/* Leave 4514 disabled.  It's an unneeded warning anyway. */
#endif
#endif

#endif /* _INC_XTL */
#endif /* _XTL_ */
