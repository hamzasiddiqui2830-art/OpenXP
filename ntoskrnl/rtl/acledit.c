/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Module Name:

    Acledit.c

Abstract:

    This Module implements the Acl rtl editing functions that are defined in
    ntseapi.h

--*/

#include <ntrtlp.h>
#include <seopaque.h>

#define FirstAce(Acl) ((PVOID)((PUCHAR)(Acl) + sizeof(ACL)))
#define NextAce(Ace) ((PVOID)((PUCHAR)(Ace) + ((PACE_HEADER)(Ace))->AceSize))
#define LongAligned(ptr) (LongAlign(ptr) == ((PVOID)(ptr)))
#define WordAligned(ptr) (WordAlign(ptr) == ((PVOID)(ptr)))

VOID RtlpAddData(IN PVOID From, IN ULONG FromSize, IN PVOID To, IN ULONG ToSize);
VOID RtlpDeleteData(IN PVOID Data, IN ULONG RemoveSize, IN ULONG TotalSize);

#if defined(ALLOC_PRAGMA) && defined(NTOS_KERNEL_RUNTIME)
#pragma alloc_text(PAGE,RtlCreateAcl)
#pragma alloc_text(PAGE,RtlValidAcl)
#pragma alloc_text(PAGE,RtlQueryInformationAcl)
#pragma alloc_text(PAGE,RtlSetInformationAcl)
#pragma alloc_text(PAGE,RtlDeleteAce)
#pragma alloc_text(PAGE,RtlGetAce)
#pragma alloc_text(PAGE,RtlFirstFreeAce)
#pragma alloc_text(PAGE,RtlpAddData)
#pragma alloc_text(PAGE,RtlpDeleteData)
#endif

NTSTATUS NTAPI
RtlCreateAcl(IN PACL Acl, IN ULONG AclLength, IN ULONG AclRevision)
{
    RTL_PAGED_CODE();
    if (AclLength < sizeof(ACL) || AclLength > MAX_USTRING) return STATUS_BUFFER_TOO_SMALL;
    if (AclRevision < MIN_ACL_REVISION || AclRevision > MAX_ACL_REVISION) return STATUS_INVALID_PARAMETER;
    Acl->AclRevision = (UCHAR)AclRevision;
    Acl->Sbz1 = 0;
    Acl->AclSize = (USHORT)(AclLength & 0xfffc);
    Acl->AceCount = 0;
    Acl->Sbz2 = 0;
    return STATUS_SUCCESS;
}

BOOLEAN NTAPI
RtlValidAcl(IN PACL Acl)
{
    PACE_HEADER Ace;
    PISID Sid, Sid2;
    ULONG i;
    RTL_PAGED_CODE();
    if (!ValidAclRevision(Acl) || !WordAligned(&Acl->AclSize) || Acl->AclSize < sizeof(ACL)) return FALSE;
    Ace = FirstAce(Acl);
    for (i = 0; i < Acl->AceCount; i++) {
        if ((PUCHAR)Ace + sizeof(ACE_HEADER) > (PUCHAR)Acl + Acl->AclSize ||
            !WordAligned(&Ace->AceSize) || Ace->AceSize < sizeof(ACE_HEADER) ||
            (PUCHAR)Ace + Ace->AceSize > (PUCHAR)Acl + Acl->AclSize) return FALSE;
        if (IsKnownAceType(Ace)) {
            if (!LongAligned(Ace->AceSize) || Ace->AceSize < sizeof(KNOWN_ACE) - sizeof(ULONG) + sizeof(SID) - sizeof(ULONG)) return FALSE;
            Sid = (PISID)&((PKNOWN_ACE)Ace)->SidStart;
            if (Sid->Revision != SID_REVISION || Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                Ace->AceSize < sizeof(KNOWN_ACE) - sizeof(ULONG) + SeLengthSid(Sid)) return FALSE;
        } else if (IsCompoundAceType(Ace)) {
            if (Acl->AclRevision < ACL_REVISION3 || !LongAligned(Ace->AceSize) ||
                Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) + sizeof(SID)) return FALSE;
            if (((PKNOWN_COMPOUND_ACE)Ace)->CompoundAceType != COMPOUND_ACE_IMPERSONATION) return FALSE;
            Sid = (PISID)&((PKNOWN_COMPOUND_ACE)Ace)->SidStart;
            if (Sid->Revision != SID_REVISION || Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) + SeLengthSid(Sid) + sizeof(SID)) return FALSE;
            Sid2 = (PISID)((PUCHAR)Sid + SeLengthSid(Sid));
            if (Sid2->Revision != SID_REVISION || Sid2->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) + SeLengthSid(Sid) + SeLengthSid(Sid2)) return FALSE;
        } else if (IsObjectAceType(Ace)) {
            ULONG GuidSize = 0;
            if (Acl->AclRevision < ACL_REVISION4 || !LongAligned(Ace->AceSize) || Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG)) return FALSE;
            if (RtlObjectAceObjectTypePresent(Ace)) GuidSize += sizeof(GUID);
            if (RtlObjectAceInheritedObjectTypePresent(Ace)) GuidSize += sizeof(GUID);
            if (Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG) + GuidSize + sizeof(SID)) return FALSE;
            Sid = (PISID)RtlObjectAceSid(Ace);
            if (Sid->Revision != SID_REVISION || Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG) + GuidSize + SeLengthSid(Sid)) return FALSE;
        }
        Ace = (PACE_HEADER)NextAce(Ace);
    }
    return TRUE;
}

NTSTATUS NTAPI
RtlQueryInformationAcl(IN PACL Acl, OUT PVOID AclInformation, IN ULONG AclInformationLength, IN ACL_INFORMATION_CLASS AclInformationClass)
{
    PACL_REVISION_INFORMATION RevisionInfo;
    PACL_SIZE_INFORMATION SizeInfo;
    PVOID FirstFree;
    if (!ValidAclRevision(Acl)) return STATUS_INVALID_PARAMETER;
    switch (AclInformationClass) {
    case AclRevisionInformation:
        if (AclInformationLength < sizeof(ACL_REVISION_INFORMATION)) return STATUS_BUFFER_TOO_SMALL;
        RevisionInfo = (PACL_REVISION_INFORMATION)AclInformation;
        RevisionInfo->AclRevision = Acl->AclRevision;
        return STATUS_SUCCESS;
    case AclSizeInformation:
        if (AclInformationLength < sizeof(ACL_SIZE_INFORMATION) || !RtlFirstFreeAce(Acl, &FirstFree)) return STATUS_BUFFER_TOO_SMALL;
        SizeInfo = (PACL_SIZE_INFORMATION)AclInformation;
        SizeInfo->AceCount = Acl->AceCount;
        SizeInfo->AclBytesInUse = (ULONG)((PUCHAR)(FirstFree ? FirstFree : (PVOID)Acl + Acl->AclSize) - (PUCHAR)Acl);
        SizeInfo->AclBytesFree = Acl->AclSize - SizeInfo->AclBytesInUse;
        return STATUS_SUCCESS;
    default:
        return STATUS_INVALID_INFO_CLASS;
    }
}

NTSTATUS NTAPI
RtlSetInformationAcl(IN PACL Acl, IN PVOID AclInformation, IN ULONG AclInformationLength, IN ACL_INFORMATION_CLASS AclInformationClass)
{
    if (!ValidAclRevision(Acl) || AclInformationClass != AclRevisionInformation || AclInformationLength < sizeof(ACL_REVISION_INFORMATION)) return STATUS_INVALID_PARAMETER;
    Acl->AclRevision = ((PACL_REVISION_INFORMATION)AclInformation)->AclRevision;
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI
RtlGetAce(IN PACL Acl, IN ULONG AceIndex, OUT PVOID *Ace)
{
    PACE_HEADER Current;
    ULONG i;
    if (!Ace || !RtlValidAcl(Acl) || AceIndex >= Acl->AceCount) return STATUS_INVALID_PARAMETER;
    Current = FirstAce(Acl);
    for (i = 0; i < AceIndex; i++) Current = (PACE_HEADER)NextAce(Current);
    if ((PUCHAR)Current + sizeof(ACE_HEADER) > (PUCHAR)Acl + Acl->AclSize) return STATUS_INVALID_PARAMETER;
    *Ace = Current;
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI
RtlDeleteAce(IN OUT PACL Acl, IN ULONG AceIndex)
{
    PVOID Ace;
    PVOID Next;
    NTSTATUS Status;
    if (!RtlValidAcl(Acl) || AceIndex >= Acl->AceCount) return STATUS_INVALID_PARAMETER;
    Status = RtlGetAce(Acl, AceIndex, &Ace);
    if (!NT_SUCCESS(Status)) return Status;
    Next = NextAce((PACE_HEADER)Ace);
    RtlpDeleteData(Ace, (ULONG)((PUCHAR)Next - (PUCHAR)Ace), (ULONG)((PUCHAR)FirstAce(Acl) + Acl->AclSize - (PUCHAR)Ace));
    Acl->AceCount--;
    return STATUS_SUCCESS;
}

BOOLEAN NTAPI
RtlFirstFreeAce(IN PACL Acl, OUT PVOID *FirstFree)
{
    PACE_HEADER Ace;
    ULONG i;
    if (!FirstFree || !RtlValidAcl(Acl)) return FALSE;
    Ace = FirstAce(Acl);
    for (i = 0; i < Acl->AceCount; i++) {
        if ((PUCHAR)Ace + sizeof(ACE_HEADER) > (PUCHAR)Acl + Acl->AclSize) return FALSE;
        Ace = (PACE_HEADER)NextAce(Ace);
    }
    if ((PUCHAR)Ace > (PUCHAR)Acl + Acl->AclSize) return FALSE;
    *FirstFree = Ace;
    return TRUE;
}

VOID NTAPI
RtlpAddData(IN PVOID From, IN ULONG FromSize, IN PVOID To, IN ULONG ToSize)
{
    PUCHAR Dst = (PUCHAR)To;
    ULONG i;
    for (i = ToSize; i != 0; i--) Dst[i - 1 + FromSize] = Dst[i - 1];
    RtlMoveMemory(Dst, From, FromSize);
}

VOID NTAPI
RtlpDeleteData(IN PVOID Data, IN ULONG RemoveSize, IN ULONG TotalSize)
{
    PUCHAR Base = (PUCHAR)Data;
    if (TotalSize > RemoveSize) RtlMoveMemory(Base, Base + RemoveSize, TotalSize - RemoveSize);
    RtlZeroMemory(Base + (TotalSize > RemoveSize ? TotalSize - RemoveSize : 0), RemoveSize);
}
