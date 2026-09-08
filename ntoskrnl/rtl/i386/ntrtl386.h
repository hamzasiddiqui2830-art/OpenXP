/*++

Module Name:

    ntrtl386.h

Abstract:

    i386-specific private RTL declarations.

    You may only use WRK-derived code if you agree to the terms of the
    Windows Research Kernel Source Code License agreement.

--*/

#ifndef _NTRTL386_
#define _NTRTL386_

EXCEPTION_DISPOSITION
RtlpExecuteHandlerForException(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN ULONG EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    );

EXCEPTION_DISPOSITION
RtlpExecuteHandlerForUnwind(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN ULONG EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    );

BOOLEAN
RtlpIsFrameInBounds(
    IN OUT PULONG LowLimit,
    IN ULONG StackFrame,
    IN OUT PULONG HighLimit
    );

#endif /* _NTRTL386_ */
