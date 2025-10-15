/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       tracerecording.h
 *  Content:    Xenon Trace Recording include file
 *
 ****************************************************************************/

#ifndef _TRACE_RECORDING_H
#define _TRACE_RECORDING_H

#ifdef __cplusplus
extern "C" {
#endif

// These functions implement native (on the devkit) trace recording of
// PowerPC code. They return TRUE for success, FALSE otherwise.
// Call XTraceStartRecording to being recording a trace. Only one
// trace can be recorded at a time. Recording traces from multiple
// threads simultaneously is not supported.
// The filename should be for a writeable file on the devkit.
// Example usage: XTraceStartRecording("cache:\\trace.bin");
// If a null pointer is passed for the filename then tracing runs until
// the buffer fills up, and it can then be saved using XTraceSaveBuffer.
BOOL XTraceStartRecording(const char* filename);

// XTraceStartRecordingFunction starts trace recording but only when
// inside the specified function. Tracing starts whenever the instruction
// at the specified address is executed, and pauses whenever execution
// returns from the thread. If recordChildren is TRUE then code called
// from this function is also recorded.
BOOL XTraceStartRecordingFunction(const char* filename,
                                  void* pFunction, BOOL recordChildren);

// XTraceSetBufferSize lets you specify the size of the buffer used for trace
// recording. Normally this isn't necessary because when the buffer fills up
// it is spooled off to disk. However, when running at an elevated IRQ level,
// inside some system functions, file IO is prohibited and the buffer may
// fill up, thus stopping tracing.
// Alternately, this can be used to request a smaller buffer size in order to
// conserve memory. The old buffer size is returned. Passing in zero will just
// return the old buffer size.
// You must call XTraceSetBufferSize before you start recording a trace.
DWORD XTraceSetBufferSize(DWORD size);

// Call XTraceStopRecording to stop recording a trace. This function
// can safely be called when no trace recording is occurring.
BOOL XTraceStopRecording();

// Call XTraceSaveBuffer to save the buffer if a NULL pointer was passed for
// the filename to the start function.
BOOL XTraceSaveBuffer(const char* filename);

#ifdef __cplusplus
};
#endif

#endif /* _TRACE_RECORDING_H */
