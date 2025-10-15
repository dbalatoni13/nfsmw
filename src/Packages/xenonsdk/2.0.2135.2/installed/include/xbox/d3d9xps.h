/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d9xps.h
 *  Content:    Xbox 360 include file for using Xbox Procedural Synthesis
 *
 ****************************************************************************/

#ifndef _D3D9XPS_H_
#define _D3D9XPS_H_

// Prototype for the D3D-maintained, thread-specific XPS context given to
// an XPS routine:

typedef struct XpsThread XpsThread;

// Prototype for XPS routines:

typedef void (*XpsCallback)(XpsThread* pThreadContext, void* pCallbackContext, CONST void* pSubmitData, DWORD InstanceIndex);

// Size, in bytes, of the ring-buffer space consumed by various
// 'Xps' calls:

#define XPS_DRAWVERTICES_SIZE                   132
#define XPS_DRAWINDEXEDVERTICES_SIZE            140

// Default command size:

#define XPS_COMMAND_SIZE                        XPS_DRAWINDEXEDVERTICES_SIZE

// Maximum number of threads that can be enabled for XPS:

#define XPS_MAX_THREADS                         6

// Hardware fetch constant used for XPS:

#define XPS_FETCH_CONSTANT (GPU_CONVERT_D3D_TO_HARDWARE_VERTEXFETCHCONSTANT(0) / 3)

//------------------------------------------------------------------------------
// The Xps class is only available with C++.

#ifdef __cplusplus

class Xps;

// Builder function prototypes:

void Xps_Initialize(Xps* pXps, XpsThread* pThreadContext);
void Xps_Uninitialize(Xps* pXps);
void* Xps_Allocate(Xps* pXps, DWORD DataSize, DWORD CommandSize);
void Xps_DrawVertices(Xps* pXps, D3DPRIMITIVETYPE PrimitiveType, DWORD VertexCount, CONST void* pVertexData);
void Xps_DrawIndexedVertices(Xps* pXps, D3DPRIMITIVETYPE PrimitiveType, DWORD IndexCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexData);
void Xps_KickOff(Xps* pXps);
BOOL Xps_KickOffAndGet(Xps* pXps, DWORD* pInstanceIndex);
BOOL Xps_Get(Xps* pXps, DWORD* pInstanceIndex);

// 'Xps' is the class used by XPS routines to synchronize with the
// other XPS threads, to write to the L2 buffer, and to kick-off commands
// to the GPU.
//
// NOTE: This object is intended to be instantiated on the stack at the
//       entry of every XPS data generation routine.
//
// NOTE: Once final hardware arrives, all of the member functions will become
//       '__forceinline' and so will always be inlined in the calling routine.

class Xps
{
private:

    DWORD m_Reserved[100];

public:

    Xps(XpsThread* pThreadContext)
    {
        Xps_Initialize(this, pThreadContext);
    }

    ~Xps()
    {
        Xps_Uninitialize(this);
    }

    void* WINAPI Allocate(DWORD DataSize, DWORD CommandSize = XPS_COMMAND_SIZE)
    {
        return Xps_Allocate(this, DataSize, CommandSize);
    }

    void WINAPI DrawVertices(D3DPRIMITIVETYPE PrimitiveType, DWORD VertexCount, CONST void* pVertexData)
    {
        Xps_DrawVertices(this, PrimitiveType, VertexCount, pVertexData);
    }

    void WINAPI DrawIndexedVertices(D3DPRIMITIVETYPE PrimitiveType, DWORD IndexCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexData)
    {
        Xps_DrawIndexedVertices(this, PrimitiveType, IndexCount, pIndexData, IndexDataFormat, pVertexData);
    }

    void WINAPI KickOff()
    {
        Xps_KickOff(this);
    }

    BOOL WINAPI KickOffAndGet(DWORD* pInstanceIndex)
    {
        return Xps_KickOffAndGet(this, pInstanceIndex);
    }
};

#endif /* __cplusplus */

#endif /* _D3D9XPS_H_ */
