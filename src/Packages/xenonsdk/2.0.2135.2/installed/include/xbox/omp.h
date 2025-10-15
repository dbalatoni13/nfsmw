//-----------------------------------------------------------------------------
// OpenMP runtime support library for Visual C++
// Copyright (C) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

// OpenMP C/C++ Version 2.0 March 2002

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef OPENMP_DLL
#define _OMPIMP
#endif

#define _OMPAPI     __cdecl

#if !defined(_OMP_LOCK_T)
#define _OMP_LOCK_T
typedef void * omp_lock_t;
#endif

#if !defined(_OMP_NEST_LOCK_T)
#define _OMP_NEST_LOCK_T
typedef void * omp_nest_lock_t;
#endif

#if !defined(_OPENMP)

#if defined(_DEBUG)
    #pragma comment(lib, "vcompd")
#else   // _DEBUG
    #pragma comment(lib, "vcomp")
#endif  // _DEBUG

#endif // _OPENMP

#if !defined(_OMPIMP)
#define _OMPIMP     __declspec(dllimport)
#endif

_OMPIMP void _OMPAPI
omp_set_num_threads(
    int _Num_threads
    );

_OMPIMP int _OMPAPI
omp_get_num_threads(
    void
    );

_OMPIMP int _OMPAPI
omp_get_max_threads(
    void
    );

_OMPIMP int _OMPAPI
omp_get_thread_num(
    void
    );

_OMPIMP int _OMPAPI
omp_get_num_procs(
    void
    );

_OMPIMP void _OMPAPI
omp_set_dynamic(
    int _Dynamic_threads
    );

_OMPIMP int _OMPAPI
omp_get_dynamic(
    void
    );

_OMPIMP int _OMPAPI
omp_in_parallel(
    void
    );

_OMPIMP void _OMPAPI
omp_set_nested(
    int _Nested
    );

_OMPIMP int _OMPAPI
omp_get_nested(
    void
    );

_OMPIMP void _OMPAPI
omp_init_lock(
    omp_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_destroy_lock(
    omp_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_set_lock(
    omp_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_unset_lock(
    omp_lock_t * _Lock
    );

_OMPIMP int _OMPAPI
omp_test_lock(
    omp_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_init_nest_lock(
    omp_nest_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_destroy_nest_lock(
    omp_nest_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_set_nest_lock(
    omp_nest_lock_t * _Lock
    );

_OMPIMP void _OMPAPI
omp_unset_nest_lock(
    omp_nest_lock_t * _Lock
    );

_OMPIMP int _OMPAPI
omp_test_nest_lock(
    omp_nest_lock_t * _Lock
    );

_OMPIMP double _OMPAPI
omp_get_wtime(
    void
    );

_OMPIMP double _OMPAPI
omp_get_wtick(
    void
    );


#if _XBOX_VER >= 200

//-----------------------------------------------------------------------------
// 
// xomp_set_cpu_order 
//
// Sets the CPU order to which tasks will be allocated. For example, it 
// can be used to manually load balance, or keep split loops on one 
// physical core to maintain and exploit L1 cache locality.
//
// Returns the current cpu order.
//
// The Xbox-specific implementation of OpenMP uses a threadpool. 
// The pool has exactly the same number of threads as CPUs.
// This cuts down on thread pool memory footprint and context switch
// overhead.  However, it means allocating multiple tasks to a single CPU
// may cause deadlocks if the multiple tasks need to synchronize with one another.
//
// You cannot change the CPU of the first task - it will
// always go to the spawning thread, and will not consume a thread from 
// the pool.
//
// If you set num threads > number processors, the runtime 
// will FORK extra threads.  This is needed to maintain OpenMP semantics. 
//
// The cpu numbers will be modulo the number of logical processors 
// in the devkit
//
// Note that it is possible to create a deadlock if you
// allocate two tasks to a single cpu and explicitly 
// synchronize between them, since they'll run serially on one thread.
//
// The default order is 0,1,2,3,4,5
//

typedef struct
{
    unsigned char order[6]; 
    unsigned char _padding[2]; // keep us in an __int64
} xomp_cpu_order_t;

_OMPIMP xomp_cpu_order_t _OMPAPI
xomp_set_cpu_order(
    xomp_cpu_order_t _new_cpu_order
    );



//-----------------------------------------------------------------------------
// 
// xomp_set_tracer 
//
// Set debug trace level.  0xFFFF is max verbosity
//

#define XOMP_TRACE_ALL       0xFFFF
#define XOMP_TRACE_HEAP      0x0001
#define XOMP_TRACE_HELPERAPI 0x0002
#define XOMP_TRACE_USERAPI   0x0004
#define XOMP_TRACE_ERRORS    0x0008
#define XOMP_TRACE_TASKS     0x0010
#define XOMP_TRACE_DLL       0x0020
#define XOMP_TRACE_PERF      0x0040

_OMPIMP void _OMPAPI
xomp_set_trace(
    unsigned int uTrace
    );



#endif

#if defined(__cplusplus)
}
#endif
