/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    Acledit.c

Abstract:

    This Module implements the Acl rtl editing functions that are defined in
    ntseapi.h

Author:

    Gary Kimura     (GaryKi)    9-Nov-1989

Environment:

    Pure Runtime Library Routine

Revision History:

--*/

#include <ntrtlp.h>
#include <seopaque.h>

#define FirstAce(Acl) ((PVOID)((PUCHAR)(Acl) + sizeof(ACL)))
#define NextAce(Ace) ((PVOID)((PUCHAR)(Ace) + ((PACE_HEADER)(Ace))->AceSize))
#define LongAligned(ptr)  (LongAlign(ptr) == ((PVOID)(ptr)))
#define WordAligned(ptr)  (WordAlign(ptr) == ((PVOID)(ptr)))

VOID
RtlpAddData (
    IN PVOID From,
    IN ULONG FromSize,
    IN PVOID To,
    IN ULONG ToSize
    );

VOID
RtlpDeleteData (
    IN PVOID Data,
    IN ULONG RemoveSize,
    IN ULONG TotalSize
    );

#if defined(ALLOC_PRAGMA) && defined(NTOS_KERNEL_RUNTIME)
NTSTATUS
RtlpAddKnownAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid,
    IN UCHAR NewType
    );

NTSTATUS
RtlpAddKnownObjectAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN GUID *ObjectTypeGuid OPTIONAL,
    IN GUID *InheritedObjectTypeGuid OPTIONAL,
    IN PSID Sid,
    IN UCHAR NewType
    );

#pragma alloc_text(PAGE,RtlCreateAcl)
#pragma alloc_text(PAGE,RtlValidAcl)
#pragma alloc_text(PAGE,RtlQueryInformationAcl)
#pragma alloc_text(PAGE,RtlSetInformationAcl)
#pragma alloc_text(PAGE,RtlAddAce)
#pragma alloc_text(PAGE,RtlDeleteAce)
#pragma alloc_text(PAGE,RtlGetAce)
#pragma alloc_text(PAGE,RtlAddCompoundAce)
#pragma alloc_text(PAGE,RtlpAddKnownAce)
#pragma alloc_text(PAGE,RtlpAddKnownObjectAce)
#pragma alloc_text(PAGE,RtlAddAccessAllowedAce)
#pragma alloc_text(PAGE,RtlAddAccessAllowedAceEx)
#pragma alloc_text(PAGE,RtlAddAccessDeniedAce)
#pragma alloc_text(PAGE,RtlAddAccessDeniedAceEx)
#pragma alloc_text(PAGE,RtlAddAuditAccessAce)
#pragma alloc_text(PAGE,RtlAddAuditAccessAceEx)
#pragma alloc_text(PAGE,RtlAddAccessAllowedObjectAce)
#pragma alloc_text(PAGE,RtlAddAccessDeniedObjectAce)
#pragma alloc_text(PAGE,RtlAddAuditAccessObjectAce)
#pragma alloc_text(PAGE,RtlFirstFreeAce)
#pragma alloc_text(PAGE,RtlpAddData)
#pragma alloc_text(PAGE,RtlpDeleteData)
#endif

NTSTATUS
RtlCreateAcl (
    IN PACL Acl,
    IN ULONG AclLength,
    IN ULONG AclRevision
    )
{
    RTL_PAGED_CODE();

    if (AclLength < sizeof(ACL)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (AclRevision < MIN_ACL_REVISION || AclRevision > MAX_ACL_REVISION) {
        return STATUS_INVALID_PARAMETER;
    }

    if (AclLength > MAX_USTRING) {
        return STATUS_INVALID_PARAMETER;
    }

    Acl->AclRevision = (UCHAR)AclRevision;
    Acl->Sbz1 = 0;
    Acl->AclSize = (USHORT)(AclLength & 0xfffc);
    Acl->AceCount = 0;
    Acl->Sbz2 = 0;
    return STATUS_SUCCESS;
}

BOOLEAN
RtlValidAcl (
    IN PACL Acl
    )
{
    RTL_PAGED_CODE();

    try {
        PACE_HEADER Ace;
        PISID Sid;
        PISID Sid2;
        ULONG i;

        if (!ValidAclRevision(Acl) || !WordAligned(&Acl->AclSize) ||
            Acl->AclSize < sizeof(ACL)) {
            return FALSE;
        }

        Ace = FirstAce(Acl);
        for (i = 0; i < Acl->AceCount; i++) {
            if ((PUCHAR)Ace + sizeof(ACE_HEADER) >
                (PUCHAR)Acl + Acl->AclSize) {
                return FALSE;
            }

            if (!WordAligned(&Ace->AceSize) || Ace->AceSize < sizeof(ACE_HEADER) ||
                (PUCHAR)Ace + Ace->AceSize > (PUCHAR)Acl + Acl->AclSize) {
                return FALSE;
            }

            if (IsKnownAceType(Ace)) {
                if (!LongAligned(Ace->AceSize) ||
                    Ace->AceSize < sizeof(KNOWN_ACE) - sizeof(ULONG) +
                                   sizeof(SID) - sizeof(ULONG)) {
                    return FALSE;
                }

                Sid = (PISID)&(((PKNOWN_ACE)Ace)->SidStart);
                if (Sid->Revision != SID_REVISION ||
                    Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                    Ace->AceSize < sizeof(KNOWN_ACE) - sizeof(ULONG) +
                                   SeLengthSid(Sid)) {
                    return FALSE;
                }
            } else if (IsCompoundAceType(Ace)) {
                if (Acl->AclRevision < ACL_REVISION3 || !LongAligned(Ace->AceSize) ||
                    Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) + sizeof(SID) ||
                    ((PKNOWN_COMPOUND_ACE)Ace)->CompoundAceType != COMPOUND_ACE_IMPERSONATION) {
                    return FALSE;
                }

                Sid = (PISID)&(((PKNOWN_COMPOUND_ACE)Ace)->SidStart);
                if (Sid->Revision != SID_REVISION ||
                    Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                    Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) +
                                   SeLengthSid(Sid) + sizeof(SID)) {
                    return FALSE;
                }

                Sid2 = (PISID)((PUCHAR)Sid + SeLengthSid(Sid));
                if (Sid2->Revision != SID_REVISION ||
                    Sid2->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                    Ace->AceSize < sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) +
                                   SeLengthSid(Sid) + SeLengthSid(Sid2)) {
                    return FALSE;
                }
            } else if (IsObjectAceType(Ace)) {
                ULONG GuidSize = 0;

                if (Acl->AclRevision < ACL_REVISION4 || !LongAligned(Ace->AceSize) ||
                    Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG)) {
                    return FALSE;
                }

                if (RtlObjectAceObjectTypePresent(Ace)) {
                    GuidSize += sizeof(GUID);
                }
                if (RtlObjectAceInheritedObjectTypePresent(Ace)) {
                    GuidSize += sizeof(GUID);
                }

                if (Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG) +
                                   GuidSize + sizeof(SID)) {
                    return FALSE;
                }

                Sid = (PISID)RtlObjectAceSid(Ace);
                if (Sid->Revision != SID_REVISION ||
                    Sid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES ||
                    Ace->AceSize < sizeof(KNOWN_OBJECT_ACE) - sizeof(ULONG) +
                                   GuidSize + SeLengthSid(Sid)) {
                    return FALSE;
                }
            }

            Ace = NextAce(Ace);
        }

        return TRUE;
    } except(EXCEPTION_EXECUTE_HANDLER) {
        return FALSE;
    }
}

NTSTATUS
RtlQueryInformationAcl (
    IN PACL Acl,
    OUT PVOID AclInformation,
    IN ULONG AclInformationLength,
    IN ACL_INFORMATION_CLASS AclInformationClass
    )
{
    PACL_REVISION_INFORMATION RevisionInfo;
    PACL_SIZE_INFORMATION SizeInfo;
    PVOID FirstFree;

    RTL_PAGED_CODE();

    if (!ValidAclRevision(Acl)) {
        return STATUS_INVALID_PARAMETER;
    }

    switch (AclInformationClass) {
    case AclRevisionInformation:
        if (AclInformationLength < sizeof(ACL_REVISION_INFORMATION)) {
            return STATUS_BUFFER_TOO_SMALL;
        }
        RevisionInfo = (PACL_REVISION_INFORMATION)AclInformation;
        RevisionInfo->AclRevision = Acl->AclRevision;
        break;

    case AclSizeInformation:
        if (AclInformationLength < sizeof(ACL_SIZE_INFORMATION)) {
            return STATUS_BUFFER_TOO_SMALL;
        }
        if (!RtlFirstFreeAce(Acl, &FirstFree)) {
            return STATUS_INVALID_PARAMETER;
        }

        SizeInfo = (PACL_SIZE_INFORMATION)AclInformation;
        SizeInfo->AceCount = Acl->AceCount;
        if (FirstFree == NULL) {
            SizeInfo->AclBytesInUse = Acl->AclSize;
            SizeInfo->AclBytesFree = 0;
        } else {
            SizeInfo->AclBytesInUse = (ULONG)((PUCHAR)FirstFree - (PUCHAR)Acl);
            SizeInfo->AclBytesFree = Acl->AclSize - SizeInfo->AclBytesInUse;
        }
        break;

    default:
        return STATUS_INVALID_INFO_CLASS;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RtlSetInformationAcl (
    IN PACL Acl,
    IN PVOID AclInformation,
    IN ULONG AclInformationLength,
    IN ACL_INFORMATION_CLASS AclInformationClass
    )
{
    PACL_REVISION_INFORMATION RevisionInfo;

    RTL_PAGED_CODE();

    if (!ValidAclRevision(Acl)) {
        return STATUS_INVALID_PARAMETER;
    }

    switch (AclInformationClass) {
    case AclRevisionInformation:
        if (AclInformationLength < sizeof(ACL_REVISION_INFORMATION)) {
            return STATUS_BUFFER_TOO_SMALL;
        }
        RevisionInfo = (PACL_REVISION_INFORMATION)AclInformation;
        if (RevisionInfo->AclRevision < Acl->AclRevision ||
            RevisionInfo->AclRevision > MAX_ACL_REVISION) {
            return STATUS_INVALID_PARAMETER;
        }
        Acl->AclRevision = (UCHAR)RevisionInfo->AclRevision;
        break;

    default:
        return STATUS_INVALID_INFO_CLASS;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RtlAddAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG StartingAceIndex,
    IN PVOID AceList,
    IN ULONG AceListLength
    )
{
    PVOID FirstFree;
    PACE_HEADER Ace;
    PVOID AcePosition;
    ULONG NewAceCount;
    ULONG i;
    UCHAR NewRevision;

    RTL_PAGED_CODE();

    if (!RtlValidAcl(Acl) || !RtlFirstFreeAce(Acl, &FirstFree)) {
        return STATUS_INVALID_PARAMETER;
    }

    NewRevision = (UCHAR)AceRevision > Acl->AclRevision ?
                  (UCHAR)AceRevision : Acl->AclRevision;

    for (Ace = (PACE_HEADER)AceList, NewAceCount = 0;
         (PUCHAR)Ace < (PUCHAR)AceList + AceListLength;
         Ace = (PACE_HEADER)NextAce(Ace), NewAceCount++) {
        if (Ace->AceSize < sizeof(ACE_HEADER) ||
            (PUCHAR)Ace + Ace->AceSize > (PUCHAR)AceList + AceListLength) {
            return STATUS_INVALID_PARAMETER;
        }
        if (Ace->AceType > ACCESS_MAX_MS_V2_ACE_TYPE &&
            Ace->AceType <= ACCESS_MAX_MS_V3_ACE_TYPE &&
            AceRevision < ACL_REVISION3) {
            return STATUS_INVALID_PARAMETER;
        }
        if (Ace->AceType > ACCESS_MAX_MS_V3_ACE_TYPE &&
            AceRevision < ACL_REVISION4) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    if ((PUCHAR)Ace != (PUCHAR)AceList + AceListLength) {
        return STATUS_INVALID_PARAMETER;
    }

    if (FirstFree == NULL ||
        (PUCHAR)FirstFree + AceListLength > (PUCHAR)Acl + Acl->AclSize) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    AcePosition = FirstAce(Acl);
    for (i = 0; i < StartingAceIndex && i < Acl->AceCount; i++) {
        AcePosition = NextAce(AcePosition);
    }

    RtlpAddData(AceList, AceListLength, AcePosition,
                (ULONG)((PUCHAR)FirstFree - (PUCHAR)AcePosition));
    Acl->AceCount = (USHORT)(Acl->AceCount + NewAceCount);
    Acl->AclRevision = NewRevision;
    return STATUS_SUCCESS;
}

NTSTATUS
RtlDeleteAce (
    IN OUT PACL Acl,
    IN ULONG AceIndex
    )
{
    PVOID FirstFree;
    PACE_HEADER Ace;
    ULONG i;

    RTL_PAGED_CODE();

    if (!RtlValidAcl(Acl) || AceIndex >= Acl->AceCount ||
        !RtlFirstFreeAce(Acl, &FirstFree)) {
        return STATUS_INVALID_PARAMETER;
    }

    Ace = FirstAce(Acl);
    for (i = 0; i < AceIndex; i++) {
        Ace = NextAce(Ace);
    }

    RtlpDeleteData(Ace, Ace->AceSize,
                   (ULONG)((PUCHAR)FirstFree - (PUCHAR)Ace));
    Acl->AceCount--;
    return STATUS_SUCCESS;
}

NTSTATUS
RtlGetAce (
    IN PACL Acl,
    ULONG AceIndex,
    OUT PVOID *Ace
    )
{
    ULONG i;

    RTL_PAGED_CODE();

    if (!ValidAclRevision(Acl) || AceIndex >= Acl->AceCount || Ace == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    *Ace = FirstAce(Acl);
    for (i = 0; i < AceIndex; i++) {
        if ((PUCHAR)*Ace + sizeof(ACE_HEADER) >
            (PUCHAR)Acl + Acl->AclSize) {
            return STATUS_INVALID_PARAMETER;
        }
        *Ace = NextAce(*Ace);
    }

    if ((PUCHAR)*Ace + sizeof(ACE_HEADER) >
        (PUCHAR)Acl + Acl->AclSize) {
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RtlAddCompoundAce (
    IN PACL Acl,
    IN ULONG AceRevision,
    IN UCHAR CompoundAceType,
    IN ACCESS_MASK AccessMask,
    IN PSID ServerSid,
    IN PSID ClientSid
    )
{
    PVOID FirstFree;
    USHORT AceSize;
    PKNOWN_COMPOUND_ACE GrantAce;
    UCHAR NewRevision;

    RTL_PAGED_CODE();

    if (!RtlValidSid(ServerSid) || !RtlValidSid(ClientSid)) {
        return STATUS_INVALID_SID;
    }
    if (Acl->AclRevision > ACL_REVISION4 || AceRevision < ACL_REVISION3 ||
        AceRevision > ACL_REVISION4) {
        return STATUS_REVISION_MISMATCH;
    }

    NewRevision = Acl->AclRevision > (UCHAR)AceRevision ?
                  Acl->AclRevision : (UCHAR)AceRevision;

    if (!RtlValidAcl(Acl) || !RtlFirstFreeAce(Acl, &FirstFree)) {
        return STATUS_INVALID_ACL;
    }

    AceSize = (USHORT)(sizeof(KNOWN_COMPOUND_ACE) - sizeof(ULONG) +
                       SeLengthSid(ClientSid) + SeLengthSid(ServerSid));
    if (FirstFree == NULL ||
        (PUCHAR)FirstFree + AceSize > (PUCHAR)Acl + Acl->AclSize) {
        return STATUS_ALLOTTED_SPACE_EXCEEDED;
    }

    GrantAce = (PKNOWN_COMPOUND_ACE)FirstFree;
    GrantAce->Header.AceFlags = 0;
    GrantAce->Header.AceType = ACCESS_ALLOWED_COMPOUND_ACE_TYPE;
    GrantAce->Header.AceSize = AceSize;
    GrantAce->Mask = AccessMask;
    GrantAce->CompoundAceType = CompoundAceType;
    RtlCopySid(SeLengthSid(ServerSid), (PSID)&GrantAce->SidStart, ServerSid);
    RtlCopySid(SeLengthSid(ClientSid),
               (PSID)((PCHAR)&GrantAce->SidStart + SeLengthSid(ServerSid)),
               ClientSid);
    Acl->AceCount++;
    Acl->AclRevision = NewRevision;
    return STATUS_SUCCESS;
}

NTSTATUS
RtlpAddKnownAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid,
    IN UCHAR NewType
    )
{
    PVOID FirstFree;
    USHORT AceSize;
    PKNOWN_ACE GrantAce;
    UCHAR NewRevision;
    ULONG TestedAceFlags;

    RTL_PAGED_CODE();

    if (!RtlValidSid(Sid)) {
        return STATUS_INVALID_SID;
    }
    if (Acl->AclRevision > ACL_REVISION4 || AceRevision > ACL_REVISION4) {
        return STATUS_REVISION_MISMATCH;
    }

    NewRevision = Acl->AclRevision > (UCHAR)AceRevision ?
                  Acl->AclRevision : (UCHAR)AceRevision;
    TestedAceFlags = AceFlags & ~VALID_INHERIT_FLAGS;
    if (TestedAceFlags != 0) {
        if (NewType == SYSTEM_AUDIT_ACE_TYPE) {
            TestedAceFlags &= ~(SUCCESSFUL_ACCESS_ACE_FLAG | FAILED_ACCESS_ACE_FLAG);
        }
        if (TestedAceFlags != 0) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    if (!RtlValidAcl(Acl) || !RtlFirstFreeAce(Acl, &FirstFree)) {
        return STATUS_INVALID_ACL;
    }

    AceSize = (USHORT)(sizeof(ACE_HEADER) + sizeof(ACCESS_MASK) + SeLengthSid(Sid));
    if (FirstFree == NULL ||
        (PUCHAR)FirstFree + AceSize > (PUCHAR)Acl + Acl->AclSize) {
        return STATUS_ALLOTTED_SPACE_EXCEEDED;
    }

    GrantAce = (PKNOWN_ACE)FirstFree;
    GrantAce->Header.AceFlags = (UCHAR)AceFlags;
    GrantAce->Header.AceType = NewType;
    GrantAce->Header.AceSize = AceSize;
    GrantAce->Mask = AccessMask;
    RtlCopySid(SeLengthSid(Sid), (PSID)&GrantAce->SidStart, Sid);
    Acl->AceCount++;
    Acl->AclRevision = NewRevision;
    return STATUS_SUCCESS;
}

NTSTATUS
RtlpAddKnownObjectAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN GUID *ObjectTypeGuid OPTIONAL,
    IN GUID *InheritedObjectTypeGuid OPTIONAL,
    IN PSID Sid,
    IN UCHAR NewType
    )
{
    PVOID FirstFree;
    USHORT AceSize;
    PKNOWN_OBJECT_ACE GrantAce;
    UCHAR NewRevision;
    ULONG TestedAceFlags;
    ULONG AceObjectFlags = 0;
    ULONG SidSize;
    PCHAR Where;

    RTL_PAGED_CODE();

    if (!RtlValidSid(Sid)) {
        return STATUS_INVALID_SID;
    }
    if (Acl->AclRevision > ACL_REVISION4 || AceRevision != ACL_REVISION4) {
        return STATUS_REVISION_MISMATCH;
    }

    NewRevision = Acl->AclRevision > (UCHAR)AceRevision ?
                  Acl->AclRevision : (UCHAR)AceRevision;
    TestedAceFlags = AceFlags & ~VALID_INHERIT_FLAGS;
    if (TestedAceFlags != 0) {
        if (NewType == SYSTEM_AUDIT_ACE_TYPE || NewType == SYSTEM_AUDIT_OBJECT_ACE_TYPE) {
            TestedAceFlags &= ~(SUCCESSFUL_ACCESS_ACE_FLAG | FAILED_ACCESS_ACE_FLAG);
        }
        if (TestedAceFlags != 0) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    if (!RtlValidAcl(Acl) || !RtlFirstFreeAce(Acl, &FirstFree)) {
        return STATUS_INVALID_ACL;
    }

    SidSize = SeLengthSid(Sid);
    AceSize = (USHORT)(sizeof(ACE_HEADER) + sizeof(ACCESS_MASK) + sizeof(ULONG) + SidSize);
    if (ObjectTypeGuid != NULL) {
        AceObjectFlags |= ACE_OBJECT_TYPE_PRESENT;
        AceSize += sizeof(GUID);
    }
    if (InheritedObjectTypeGuid != NULL) {
        AceObjectFlags |= ACE_INHERITED_OBJECT_TYPE_PRESENT;
        AceSize += sizeof(GUID);
    }
    if (FirstFree == NULL ||
        (PUCHAR)FirstFree + AceSize > (PUCHAR)Acl + Acl->AclSize) {
        return STATUS_ALLOTTED_SPACE_EXCEEDED;
    }

    GrantAce = (PKNOWN_OBJECT_ACE)FirstFree;
    GrantAce->Header.AceFlags = (UCHAR)AceFlags;
    GrantAce->Header.AceType = NewType;
    GrantAce->Header.AceSize = AceSize;
    GrantAce->Mask = AccessMask;
    GrantAce->Flags = AceObjectFlags;
    Where = (PCHAR)&GrantAce->SidStart;
    if (ObjectTypeGuid != NULL) {
        RtlCopyMemory(Where, ObjectTypeGuid, sizeof(GUID));
        Where += sizeof(GUID);
    }
    if (InheritedObjectTypeGuid != NULL) {
        RtlCopyMemory(Where, InheritedObjectTypeGuid, sizeof(GUID));
        Where += sizeof(GUID);
    }
    RtlCopySid(SidSize, (PSID)Where, Sid);
    Acl->AceCount++;
    Acl->AclRevision = NewRevision;
    return STATUS_SUCCESS;
}

NTSTATUS
RtlAddAccessAllowedAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    )
{
    return RtlpAddKnownAce(Acl, AceRevision, 0, AccessMask, Sid,
                            ACCESS_ALLOWED_ACE_TYPE);
}

NTSTATUS
RtlAddAccessAllowedAceEx (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    )
{
    return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                            ACCESS_ALLOWED_ACE_TYPE);
}

NTSTATUS
RtlAddAccessDeniedAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    )
{
    return RtlpAddKnownAce(Acl, AceRevision, 0, AccessMask, Sid,
                            ACCESS_DENIED_ACE_TYPE);
}

NTSTATUS
RtlAddAccessDeniedAceEx (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    )
{
    return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                            ACCESS_DENIED_ACE_TYPE);
}

NTSTATUS
RtlAddAuditAccessAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid,
    IN BOOLEAN AuditSuccess,
    IN BOOLEAN AuditFailure
    )
{
    ULONG AceFlags = 0;
    if (AuditSuccess) AceFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
    if (AuditFailure) AceFlags |= FAILED_ACCESS_ACE_FLAG;
    return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                            SYSTEM_AUDIT_ACE_TYPE);
}

NTSTATUS
RtlAddAuditAccessAceEx (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid,
    IN BOOLEAN AuditSuccess,
    IN BOOLEAN AuditFailure
    )
{
    if (AuditSuccess) AceFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
    if (AuditFailure) AceFlags |= FAILED_ACCESS_ACE_FLAG;
    return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                            SYSTEM_AUDIT_ACE_TYPE);
}

NTSTATUS
RtlAddAccessAllowedObjectAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN GUID *ObjectTypeGuid OPTIONAL,
    IN GUID *InheritedObjectTypeGuid OPTIONAL,
    IN PSID Sid
    )
{
    if (ObjectTypeGuid == NULL && InheritedObjectTypeGuid == NULL) {
        return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                               ACCESS_ALLOWED_ACE_TYPE);
    }
    return RtlpAddKnownObjectAce(Acl, AceRevision, AceFlags, AccessMask,
                                 ObjectTypeGuid, InheritedObjectTypeGuid, Sid,
                                 ACCESS_ALLOWED_OBJECT_ACE_TYPE);
}

NTSTATUS
RtlAddAccessDeniedObjectAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN GUID *ObjectTypeGuid OPTIONAL,
    IN GUID *InheritedObjectTypeGuid OPTIONAL,
    IN PSID Sid
    )
{
    if (ObjectTypeGuid == NULL && InheritedObjectTypeGuid == NULL) {
        return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                               ACCESS_DENIED_ACE_TYPE);
    }
    return RtlpAddKnownObjectAce(Acl, AceRevision, AceFlags, AccessMask,
                                 ObjectTypeGuid, InheritedObjectTypeGuid, Sid,
                                 ACCESS_DENIED_OBJECT_ACE_TYPE);
}

NTSTATUS
RtlAddAuditAccessObjectAce (
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN GUID *ObjectTypeGuid OPTIONAL,
    IN GUID *InheritedObjectTypeGuid OPTIONAL,
    IN PSID Sid,
    IN BOOLEAN AuditSuccess,
    IN BOOLEAN AuditFailure
    )
{
    if (AuditSuccess) AceFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
    if (AuditFailure) AceFlags |= FAILED_ACCESS_ACE_FLAG;
    if (ObjectTypeGuid == NULL && InheritedObjectTypeGuid == NULL) {
        return RtlpAddKnownAce(Acl, AceRevision, AceFlags, AccessMask, Sid,
                               SYSTEM_AUDIT_ACE_TYPE);
    }
    return RtlpAddKnownObjectAce(Acl, AceRevision, AceFlags, AccessMask,
                                 ObjectTypeGuid, InheritedObjectTypeGuid, Sid,
                                 SYSTEM_AUDIT_OBJECT_ACE_TYPE);
}

BOOLEAN
RtlFirstFreeAce (
    IN PACL Acl,
    OUT PVOID *FirstFree
    )
{
    PACE_HEADER Ace;
    ULONG i;

    RTL_PAGED_CODE();
    *FirstFree = NULL;

    for (i = 0, Ace = FirstAce(Acl); i < Acl->AceCount; i++, Ace = NextAce(Ace)) {
        if ((PUCHAR)Ace + sizeof(ACE_HEADER) >
            (PUCHAR)Acl + Acl->AclSize) {
            return FALSE;
        }
        if (Ace->AceSize < sizeof(ACE_HEADER) ||
            (PUCHAR)Ace + Ace->AceSize > (PUCHAR)Acl + Acl->AclSize) {
            return FALSE;
        }
    }

    if ((PUCHAR)Ace <= (PUCHAR)Acl + Acl->AclSize) {
        *FirstFree = Ace;
    }
    return TRUE;
}

VOID
RtlpAddData (
    IN PVOID From,
    IN ULONG FromSize,
    IN PVOID To,
    IN ULONG ToSize
    )
{
    LONG i;

    for (i = (LONG)ToSize - 1; i >= 0; i--) {
        ((PUCHAR)To)[i + FromSize] = ((PUCHAR)To)[i];
    }
    for (i = 0; (ULONG)i < FromSize; i++) {
        ((PUCHAR)To)[i] = ((PUCHAR)From)[i];
    }
}

VOID
RtlpDeleteData (
    IN PVOID Data,
    IN ULONG RemoveSize,
    IN ULONG TotalSize
    )
{
    ULONG i;

    for (i = RemoveSize; i < TotalSize; i++) {
        ((PUCHAR)Data)[i - RemoveSize] = ((PUCHAR)Data)[i];
    }
    for (i = TotalSize - RemoveSize; i < TotalSize; i++) {
        ((PUCHAR)Data)[i] = 0;
    }
}
