#ifndef _WRK_DEBUGSUP_MSVC_COMPAT_H_
#define _WRK_DEBUGSUP_MSVC_COMPAT_H_

#include "../mm/mi.h"

#if defined(_MSC_VER) && defined(_X86_)

static __inline BOOLEAN
WrkMsvcWsOwned (
    IN PMMSUPPORT WsInfo
    )
{
    PETHREAD Thread = PsGetCurrentThread ();

    if (WsInfo == &MmSystemCacheWs) {
        return (Thread->OwnsSystemWorkingSetExclusive != 0) ||
               (Thread->OwnsSystemWorkingSetShared != 0);
    }

    if (WsInfo->Flags.SessionSpace != 0) {
        return (Thread->OwnsSessionWorkingSetExclusive != 0) ||
               (Thread->OwnsSessionWorkingSetShared != 0);
    }

    return (Thread->ApcState.Process == (PKPROCESS)PsGetCurrentProcess ()) &&
           ((Thread->OwnsProcessWorkingSetExclusive != 0) ||
            (Thread->OwnsProcessWorkingSetShared != 0));
}

static __inline PKTHREAD
WrkMsvcWsOwner (
    IN PEX_PUSH_LOCK Lock
    )
{
    PMMSUPPORT WsInfo;

    WsInfo = CONTAINING_RECORD (Lock, MMSUPPORT, WorkingSetMutex);

    return WrkMsvcWsOwned (WsInfo) ? &PsGetCurrentThread ()->Tcb : NULL;
}

#ifdef LOCK_WORKING_SET
#undef LOCK_WORKING_SET
#endif
#define LOCK_WORKING_SET(WSINFO) do {                                      \
    PETHREAD _Thread = PsGetCurrentThread ();                             \
    KeEnterGuardedRegionThread (&_Thread->Tcb);                           \
    ExAcquirePushLockExclusive (&(WSINFO)->WorkingSetMutex);              \
    if ((WSINFO) == &MmSystemCacheWs) {                                   \
        _Thread->OwnsSystemWorkingSetExclusive = 1;                      \
    } else if ((WSINFO)->Flags.SessionSpace != 0) {                       \
        _Thread->OwnsSessionWorkingSetExclusive = 1;                     \
    } else {                                                              \
        _Thread->OwnsProcessWorkingSetExclusive = 1;                     \
    }                                                                     \
} while (0)

#ifdef UNLOCK_WORKING_SET
#undef UNLOCK_WORKING_SET
#endif
#define UNLOCK_WORKING_SET(WSINFO) do {                                   \
    PETHREAD _Thread = PsGetCurrentThread ();                             \
    ASSERT (WrkMsvcWsOwned (WSINFO));                                    \
    if ((WSINFO) == &MmSystemCacheWs) {                                   \
        _Thread->OwnsSystemWorkingSetExclusive = 0;                      \
    } else if ((WSINFO)->Flags.SessionSpace != 0) {                       \
        _Thread->OwnsSessionWorkingSetExclusive = 0;                     \
    } else {                                                              \
        _Thread->OwnsProcessWorkingSetExclusive = 0;                     \
    }                                                                     \
    ExReleasePushLockExclusive (&(WSINFO)->WorkingSetMutex);              \
    KeLeaveGuardedRegionThread (&_Thread->Tcb);                           \
} while (0)

#ifdef MM_WS_LOCK_ASSERT
#undef MM_WS_LOCK_ASSERT
#endif
#define MM_WS_LOCK_ASSERT(WSINFO) ASSERT (WrkMsvcWsOwned (WSINFO))

#ifdef KeGetOwnerGuardedMutex
#undef KeGetOwnerGuardedMutex
#endif
#define KeGetOwnerGuardedMutex(LOCK) WrkMsvcWsOwner (LOCK)

#endif /* _MSC_VER && _X86_ */

#endif /* _WRK_DEBUGSUP_MSVC_COMPAT_H_ */