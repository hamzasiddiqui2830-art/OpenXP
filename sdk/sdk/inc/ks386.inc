;
; Copyright (c) OpenXP.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;

;  Pointer size in bytes
SizeofPointer equ 4

;  Process State Enumerated Type Values
ProcessInMemory equ 0
ProcessOutOfMemory equ 1
ProcessInTransition equ 2

;  Thread State Enumerated Type Values
Initialized equ 0
Ready equ 1
Running equ 2
Standby equ 3
Terminated equ 4
Waiting equ 5

;  Wait Reason and Wait Type Enumerated Type Values
WrExecutive equ 0
WrMutex equ 1Dh
WrDispatchInt equ 1Fh
WrQuantumEnd equ 1Eh
WrEventPair equ 0Eh
WaitAny equ 1
WaitAll equ 0

;  Apc State Structure Offset Definitions
AsApcListHead equ 0
AsProcess equ 10h
AsKernelApcInProgress equ 14h
AsKernelApcPending equ 15h
AsUserApcPending equ 16h

;  Bugcheck Code Definitions
APC_INDEX_MISMATCH equ 1
ATTEMPTED_SWITCH_FROM_DPC equ 0B8h
DATA_BUS_ERROR equ 2Eh
DATA_COHERENCY_EXCEPTION equ 55h
HAL1_INITIALIZATION_FAILED equ 61h
INSTRUCTION_BUS_ERROR equ 2Fh
INSTRUCTION_COHERENCY_EXCEPTION equ 56h
INTERRUPT_EXCEPTION_NOT_HANDLED equ 3Dh
INTERRUPT_UNWIND_ATTEMPTED equ 3Ch
INVALID_AFFINITY_SET equ 3
INVALID_DATA_ACCESS_TRAP equ 4
IRQL_GT_ZERO_AT_SYSTEM_SERVICE equ 4Ah
IRQL_NOT_LESS_OR_EQUAL equ 0Ah
KMODE_EXCEPTION_NOT_HANDLED equ 1Eh
NMI_HARDWARE_FAILURE equ 80h
NO_USER_MODE_CONTEXT equ 0Eh
PAGE_FAULT_WITH_INTERRUPTS_OFF equ 49h
PANIC_STACK_SWITCH equ 2Bh
SPIN_LOCK_INIT_FAILURE equ 81h
SYSTEM_EXIT_OWNED_MUTEX equ 39h
SYSTEM_SERVICE_EXCEPTION equ 3Bh
SYSTEM_UNWIND_PREVIOUS_USER equ 3Ah
TRAP_CAUSE_UNKNOWN equ 12h
UNEXPECTED_KERNEL_MODE_TRAP equ 7Fh
HARDWARE_INTERRUPT_STORM equ 0F2h
RECURSIVE_MACHINE_CHECK equ 0FBh
RECURSIVE_NMI equ 111h

;  Breakpoint type definitions
DBG_STATUS_CONTROL_C equ 1
STATUS_ASSERTION_FAILURE equ 0C0000420h

;  Client Id Structure Offset Definitions
CidUniqueProcess equ 0
CidUniqueThread equ 4

;  Critical Section Structure Offset Definitions
CsDebugInfo equ 0
CsLockCount equ 4
CsRecursionCount equ 8
CsOwningThread equ 0Ch
CsLockSemaphore equ 10h
CsSpinCount equ 14h

;  Critical Section Debug Information Structure Offset Definitions
CsType equ 0
CsCreatorBackTraceIndex equ 2
CsCriticalSection equ 4
CsProcessLocksList equ 8
CsEntryCount equ 10h
CsContentionCount equ 14h

;  Exception Record Offset, Flag, and Enumerated Type Definitions
EXCEPTION_NONCONTINUABLE equ 1
EXCEPTION_UNWINDING equ 2
EXCEPTION_EXIT_UNWIND equ 4
EXCEPTION_STACK_INVALID equ 8
EXCEPTION_NESTED_CALL equ 10h
EXCEPTION_TARGET_UNWIND equ 20h
EXCEPTION_COLLIDED_UNWIND equ 40h
EXCEPTION_UNWIND equ 66h
EXCEPTION_EXECUTE_HANDLER equ 1
EXCEPTION_CONTINUE_SEARCH equ 0
EXCEPTION_CONTINUE_EXECUTION equ 0FFFFFFFFh
EXCEPTION_CHAIN_END equ 0FFFFFFFFh
FIXED_NTVDMSTATE_LINEAR equ 714h

ExceptionContinueExecution equ 0
ExceptionContinueSearch equ 1
ExceptionNestedException equ 2
ExceptionCollidedUnwind equ 3

ErExceptionCode equ 0
ErExceptionFlags equ 4
ErExceptionRecord equ 8
ErExceptionAddress equ 0Ch
ErNumberParameters equ 10h
ErExceptionInformation equ 14h
ExceptionRecordLength equ 50h

;  Fast Mutex Structure Offset Definitions
FmCount equ 0
FmOwner equ 4
FmContention equ 8
FmGate equ 0Ch
FmOldIrql equ 1Ch

;  Interrupt Priority Request Level Definitions
PASSIVE_LEVEL equ 0
APC_LEVEL equ 1
DISPATCH_LEVEL equ 2
CLOCK1_LEVEL equ 1Ch
CLOCK2_LEVEL equ 1Ch
IPI_LEVEL equ 1Dh
POWER_LEVEL equ 1Eh
PROFILE_LEVEL equ 1Bh
HIGH_LEVEL equ 1Fh

IFDEF NT_UP
    SYNCH_LEVEL equ 2
ELSE
    SYNCH_LEVEL equ 1Bh
ENDIF

;  Large Integer Structure Offset Definitions
LiLowPart equ 0
LiHighPart equ 4

;  List Entry Structure Offset Definitions
LsFlink equ 0
LsBlink equ 4

;  String Structure Offset Definitions
StrLength equ 0
StrMaximumLength equ 2
StrBuffer equ 4

;  System Time Structure Offset Definitions
StLowTime equ 0
StHigh1Time equ 4
StHigh2Time equ 8

;  User Shared Data Structure Offset Definitions
UsTickCountMultiplier equ 4
UsInterruptTime equ 8
UsSystemTime equ 14h
UsTimeZoneBias equ 20h
UsImageNumberLow equ 2Ch
UsImageNumberHigh equ 2Eh
UsNtSystemRoot equ 30h
UsMaxStackTraceDepth equ 238h
UsCryptoExponent equ 23Ch
UsTimeZoneId equ 240h
UsLargePageMinimum equ 244h
UsReserved2 equ 248h
UsNtProductType equ 264h
UsProductTypeIsValid equ 268h
UsNtMajorVersion equ 26Ch
UsNtMinorVersion equ 270h
UsProcessorFeatures equ 274h
UsReserved1 equ 2B4h
UsReserved3 equ 2B8h
UsTimeSlip equ 2BCh
UsAlternativeArchitecture equ 2C0h
UsSystemExpirationDate equ 2C8h
UsSuiteMask equ 2D0h
UsKdDebuggerEnabled equ 2D4h
UsActiveConsoleId equ 2D8h
UsDismountCount equ 2DCh
UsComPlusPackage equ 2E0h
UsLastSystemRITEventTickCount equ 2E4h
UsNumberOfPhysicalPages equ 2E8h
UsSafeBootMode equ 2ECh
UsTraceLogging equ 2F0h
UsTestRetInstruction equ 2F8h
UsSystemCall equ 300h
UsSystemCallReturn equ 304h
UsSystemCallPad equ 308h
UsTickCount equ 320h
UsTickCountQuad equ 320h
UsWow64SharedInformation equ 334h

;  Time Structure Offset Definitions
TmLowTime equ 0
TmHighTime equ 4

;  Thread Switch Counter Offset Definitions
TwFindAny equ 0
TwFindIdeal equ 4
TwFindLast equ 8
TwIdleAny equ 0Ch
TwIdleCurrent equ 10h
TwIdleIdeal equ 14h
TwIdleLast equ 18h
TwPreemptAny equ 1Ch
TwPreemptCurrent equ 20h
TwPreemptLast equ 24h
TwSwitchToIdle equ 28h

;  Status Code Definitions
STATUS_ACCESS_VIOLATION equ 0C0000005h
STATUS_ARRAY_BOUNDS_EXCEEDED equ 0C000008Ch
STATUS_BAD_COMPRESSION_BUFFER equ 0C0000242h
STATUS_BREAKPOINT equ 80000003h
STATUS_CALLBACK_POP_STACK equ 0C0000423h
STATUS_DATATYPE_MISALIGNMENT equ 80000002h
STATUS_FLOAT_DENORMAL_OPERAND equ 0C000008Dh
STATUS_FLOAT_DIVIDE_BY_ZERO equ 0C000008Eh
STATUS_FLOAT_INEXACT_RESULT equ 0C000008Fh
STATUS_FLOAT_INVALID_OPERATION equ 0C0000090h
STATUS_FLOAT_OVERFLOW equ 0C0000091h
STATUS_FLOAT_STACK_CHECK equ 0C0000092h
STATUS_FLOAT_UNDERFLOW equ 0C0000093h
STATUS_FLOAT_MULTIPLE_FAULTS equ 0C00002B4h
STATUS_FLOAT_MULTIPLE_TRAPS equ 0C00002B5h
STATUS_GUARD_PAGE_VIOLATION equ 80000001h
STATUS_ILLEGAL_FLOAT_CONTEXT equ 0C000014Ah
STATUS_ILLEGAL_INSTRUCTION equ 0C000001Dh
STATUS_INSTRUCTION_MISALIGNMENT equ 0C00000AAh
STATUS_INVALID_HANDLE equ 0C0000008h
STATUS_INVALID_LOCK_SEQUENCE equ 0C000001Eh
STATUS_INVALID_OWNER equ 0C000005Ah
STATUS_INVALID_PARAMETER equ 0C000000Dh
STATUS_INVALID_PARAMETER_1 equ 0C00000EFh
STATUS_INVALID_SYSTEM_SERVICE equ 0C000001Ch
STATUS_INTEGER_DIVIDE_BY_ZERO equ 0C0000094h
STATUS_INTEGER_OVERFLOW equ 0C0000095h
STATUS_IN_PAGE_ERROR equ 0C0000006h
STATUS_KERNEL_APC equ 100h
STATUS_LONGJUMP equ 80000026h
STATUS_NO_CALLBACK_ACTIVE equ 0C0000258h
STATUS_NO_EVENT_PAIR equ 0C000014Eh
STATUS_PRIVILEGED_INSTRUCTION equ 0C0000096h
STATUS_SINGLE_STEP equ 80000004h
STATUS_STACK_BUFFER_OVERRUN equ 0C0000409h
STATUS_STACK_OVERFLOW equ 0C00000FDh
STATUS_SUCCESS equ 0
STATUS_THREAD_IS_TERMINATING equ 0C000004Bh
STATUS_TIMEOUT equ 102h
STATUS_UNWIND equ 0C0000027h
STATUS_UNWIND_CONSOLIDATE equ 80000029h
STATUS_USER_APC equ 0C0h
STATUS_WAKE_SYSTEM_DEBUGGER equ 80000007h

;  APC Object Structure Offset Definitions
ApType equ 0
ApSize equ 2
ApThread equ 8
ApApcListEntry equ 0Ch
ApKernelRoutine equ 14h
ApRundownRoutine equ 18h
ApNormalRoutine equ 1Ch
ApNormalContext equ 20h
ApSystemArgument1 equ 24h
ApSystemArgument2 equ 28h
ApApcStateIndex equ 2Ch
ApApcMode equ 2Dh
ApInserted equ 2Eh
ApcObjectLength equ 30h

;  DPC object Structure Offset Definitions
DpType equ 0
DpNumber equ 2
DpImportance equ 1
DpDpcListEntry equ 4
DpDeferredRoutine equ 0Ch
DpDeferredContext equ 10h
DpSystemArgument1 equ 14h
DpSystemArgument2 equ 18h
DpDpcData equ 1Ch
DpcObjectLength equ 20h

;  Device Queue Object Structure Offset Definitions
DvType equ 0
DvSize equ 2
DvDeviceListHead equ 4
DvSpinLock equ 0Ch
DvBusy equ 10h
DeviceQueueObjectLength equ 14h

;  Device Queue Entry Structure Offset Definitions
DeDeviceListEntry equ 0
DeSortKey equ 8
DeInserted equ 0Ch
DeviceQueueEntryLength equ 10h

;  Event Object Structure Offset Definitions
EvType equ 0
EvSize equ 2
EvSignalState equ 4
EvWaitListHead equ 8
EventObjectLength equ 10h

;  Event Pair Object Structure Offset Definitions
EpType equ 0
EpSize equ 2
EpEventLow equ 4
EpEventHigh equ 14h

;  Interrupt Object Structure Offset Definitions
InLevelSensitive equ 0
InLatched equ 1

InType equ 0
InSize equ 2
InInterruptListEntry equ 4
InServiceRoutine equ 0Ch
InServiceContext equ 10h
InSpinLock equ 14h
InTickCount equ 18h
InActualLock equ 1Ch
InDispatchAddress equ 20h
InVector equ 24h
InIrql equ 28h
InSynchronizeIrql equ 29h
InFloatingSave equ 2Ah
InConnected equ 2Bh
InNumber equ 2Ch
InShareVector equ 2Dh
InMode equ 30h
InServiceCount equ 34h
InDispatchCount equ 38h
InDispatchCode equ 3Ch
InterruptObjectLength equ 1E4h

NORMAL_DISPATCH_LENGTH equ 1A8h
DISPATCH_LENGTH equ 1A8h

;  Process Object Structure Offset Definitions
PrType equ 0
PrSize equ 2
PrSignalState equ 4
PrProfileListHead equ 10h
PrDirectoryTableBase equ 18h
PrLdtDescriptor equ 20h
PrIopmOffset equ 30h
PrInt21Descriptor equ 28h
PrIopl equ 32h
PrVdmTrapcHandler equ 4Ch
PrVdmObjects equ 144h
PrFlags equ 240h
PrActiveProcessors equ 34h
PrKernelTime equ 38h
PrUserTime equ 3Ch
PrReadyListHead equ 40h
PrSwapListEntry equ 48h
PrThreadListHead equ 50h
PrProcessLock equ 58h
PrAffinity equ 5Ch
PrProcessFlags equ 60h
PrBasePriority equ 64h
PrQuantumReset equ 65h
PrState equ 66h
PrStackCount equ 6Ch
KernelProcessObjectLength equ 78h
ExecutiveProcessObjectLength equ 278h

;  Profile Object Structure Offset Definitions
PfType equ 0
PfSize equ 2
PfProfileListEntry equ 4
PfProcess equ 0Ch
PfRangeBase equ 10h
PfRangeLimit equ 14h
PfBucketShift equ 18h
PfBuffer equ 1Ch
PfSegment equ 20h
PfAffinity equ 24h
PfSource equ 28h
PfStarted equ 2Ah
ProfileObjectLength equ 2Ch

;  Queue Object Structure Offset Definitions
QuType equ 0
QuSize equ 2
QuSignalState equ 4
QuEntryListHead equ 10h
QuCurrentCount equ 18h
QuMaximumCount equ 1Ch
QuThreadListHead equ 20h
QueueObjectLength equ 28h

;  Thread Object Structure Offset Definitions
EeKernelEventPair equ 0
EtCid equ 1E4h

ThType equ 0
ThNpxIrql equ 1
ThSize equ 2
ThDebugActive equ 3
ThSignalState equ 4
ThMutantListHead equ 10h
ThInitialStack equ 18h
ThStackLimit equ 1Ch
ThKernelStack equ 20h
ThThreadLock equ 24h
ThApcState equ 28h
ThApcQueueable equ 3Fh
ThNextProcessor equ 40h
ThDeferredProcessor equ 41h
ThAdjustReason equ 42h
ThAdjustIncrement equ 43h
ThApcQueueLock equ 44h
ThContextSwitches equ 48h
ThState equ 4Ch
ThNpxState equ 4Dh
ThWaitIrql equ 4Eh
ThWaitMode equ 4Fh
ThWaitStatus equ 50h
ThWaitBlockList equ 54h
ThGateObject equ 54h
ThAlertable equ 58h
ThWaitNext equ 59h
ThWaitReason equ 5Ah
ThPriority equ 5Bh
ThEnableStackSwap equ 5Ch
ThSwapBusy equ 5Dh
ThAlerted equ 5Eh
ThWaitListEntry equ 60h
ThSwapListEntry equ 60h
ThQueue equ 68h
ThWaitTime equ 6Ch
ThCombinedApcDisable equ 70h
ThKernelApcDisable equ 70h
ThSpecialApcDisable equ 72h
ThTeb equ 74h
ThTimer equ 78h
ThThreadFlags equ 0A0h
ThWaitBlock equ 0A8h
ThSystemAffinityActive equ 0BFh
ThPreviousMode equ 0D7h
ThResourceIndex equ 0EFh
ThLargeStack equ 107h
ThQueueListEntry equ 108h
ThTrapFrame equ 110h
ThCallbackStack equ 114h
ThServiceTable equ 118h
ThApcStateIndex equ 11Ch
ThIdealProcessor equ 11Dh
ThPreempted equ 11Eh
ThProcessReadyQueue equ 11Fh
ThKernelStackResident equ 120h
ThBasePriority equ 121h
ThPriorityDecrement equ 122h
ThSaturation equ 123h
ThUserAffinity equ 124h
ThProcess equ 128h
ThAffinity equ 12Ch
ThApcStatePointer equ 130h
ThSavedApcState equ 138h
ThFreezeCount equ 14Fh
ThSuspendCount equ 150h
ThUserIdealProcessor equ 151h
ThCalloutActive equ 152h
ThIopl equ 153h
ThWin32Thread equ 154h
ThStackBase equ 158h
ThSuspendApc equ 15Ch
ThQuantum equ 15Dh
ThQuantumReset equ 15Fh
ThKernelTime equ 160h
ThTlsArray equ 180h
ThBBTData equ 184h
ThPowerState equ 18Bh
ThUserTime equ 18Ch
ThSuspendSemaphore equ 190h
ThSListFaultCount equ 1A4h
ThThreadListEntry equ 1A8h
ThSListFaultAddress equ 1B0h

KI_SLIST_FAULT_COUNT_MAXIMUM equ 400h
KTHREAD_AUTO_ALIGNMENT_BIT equ 0

KernelThreadObjectLength equ 1B8h
ExecutiveThreadObjectLength equ 250h

NPX_STATE_NOT_LOADED equ 0Ah
NPX_STATE_LOADED equ 0

;  Timer object Structure Offset Definitions
TiType equ 0
TiSize equ 2
TiInserted equ 3
TiSignalState equ 4
TiDueTime equ 10h
TiTimerListEntry equ 18h
TiDpc equ 20h
TiPeriod equ 24h
TimerObjectLength equ 28h

;  Timer Table Entry Structure Offset Definitions
TtEntry equ 0
TtTime equ 8
TIMER_ENTRY_SIZE equ 10h
TIMER_TABLE_SIZE equ 200h

;  Wait Block Structure Offset Definitions
WbWaitListEntry equ 0
WbThread equ 8
WbObject equ 0Ch
WbNextWaitBlock equ 10h
WbWaitKey equ 14h
WbWaitType equ 16h

;  Fiber Structure Offset Definitions
FbFiberData equ 0
FbExceptionList equ 4
FbStackBase equ 8
FbStackLimit equ 0Ch
FbDeallocationStack equ 10h
FbFiberContext equ 14h
FbWx86Tib equ 2E0h
FbActivationContextStackPointer equ 2E4h
FbFlsData equ 2E8h
FbGuaranteedStackBytes equ 2ECh

;  Process Environment Block Structure Offset Definitions
PeKernelCallbackTable equ 2Ch
ProcessEnvironmentBlockLength equ 230h

;  System Service Descriptor Table Structure Definitions
NUMBER_SERVICE_TABLES equ 2
SERVICE_NUMBER_MASK equ 0FFFh
SERVICE_TABLE_SHIFT equ 8
SERVICE_TABLE_MASK equ 10h
SERVICE_TABLE_TEST equ 10h

SdBase equ 0
SdCount equ 4
SdLimit equ 8
SdNumber equ 0Ch
SdLength equ 10h

;  Thread Environment Block Structure Offset Definitions
TeExceptionList equ 0
TeStackBase equ 4
TeStackLimit equ 8
TeFiberData equ 10h
TeSelf equ 18h
TeEnvironmentPointer equ 1Ch
TeClientId equ 20h
TeActiveRpcHandle equ 28h
TeThreadLocalStoragePointer equ 2Ch
TeCountOfOwnedCriticalSections equ 38h
TePeb equ 30h
TeCsrClientThread equ 3Ch
TeWOW32Reserved equ 0C0h
TeSoftFpcr equ 0C8h
TeExceptionCode equ 1A4h
TeActivationContextStackPointer equ 1A8h
TeGdiClientPID equ 6C0h
TeGdiClientTID equ 6C4h
TeGdiThreadLocalInfo equ 6C8h
TeglDispatchTable equ 7C4h
TeglReserved1 equ 0B68h
TeglReserved2 equ 0BDCh
TeglSectionInfo equ 0BE0h
TeglSection equ 0BE4h
TeglTable equ 0BE8h
TeglCurrentRC equ 0BECh
TeglContext equ 0BF0h
TeDeallocationStack equ 0E0Ch
TeTlsSlots equ 0E10h
TeTlsExpansionSlots equ 0F94h
TeLastErrorValue equ 34h
TeVdm equ 0F18h
TeInstrumentation equ 0F2Ch
TeGdiBatchCount equ 0F70h
TeGuaranteedStackBytes equ 0F78h
TeSafeThunkCall equ 0FB8h
TeFlsData equ 0FB4h
ThreadEnvironmentBlockLength equ 0FBCh
CmThreadEnvironmentBlockOffset equ 1000h
TLS_MINIMUM_AVAILABLE equ 40h
TLS_EXPANSION_SLOTS equ 400h

;  Lock Queue Structure Offset Definitions
LOCK_QUEUE_WAIT equ 1
LOCK_QUEUE_OWNER equ 2
LOCK_QUEUE_HEADER_SIZE equ 8

LockQueueDispatcherLock equ 0

LqNext equ 0
LqLock equ 4

LqhNext equ 0
LqhLock equ 4
LqhOldIrql equ 8

;  Performance Definitions
PERF_CONTEXTSWAP_OFFSET equ 4
PERF_CONTEXTSWAP_FLAG equ 4
PERF_INTERRUPT_OFFSET equ 4
PERF_INTERRUPT_FLAG equ 4000h
PERF_PROFILE_OFFSET equ 4
PERF_PROFILE_FLAG equ 2

;  Apc Record Structure Offset Definitions
ArNormalRoutine equ 0
ArNormalContext equ 4
ArSystemArgument1 equ 8
ArSystemArgument2 equ 0Ch
ApcRecordLength equ 10h

;  Processor Control Registers Structure Offset Definitions
KI_BEGIN_KERNEL_RESERVED equ 0FFDF0000h

IFDEF NT_UP
    P0PCRADDRESS equ 0FFDFF000h
    PCR equ ds:[0FFDFF000h]
ELSE
    PCR equ fs:
ENDIF

PcExceptionList equ 0
PcInitialStack equ 4
PcPerfGlobalGroupMask equ 8
PcTssCopy equ 0Ch
PcContextSwitches equ 10h
PcSetMemberCopy equ 14h
PcSelfPcr equ 1Ch
PcPrcb equ 20h
PcTeb equ 18h
PcIrql equ 24h
PcIRR equ 28h
PcIrrActive equ 2Ch
PcIDR equ 30h
PcIdt equ 38h
PcGdt equ 3Ch
PcTss equ 40h
PcNumber equ 51h
PcVdmAlert equ 54h
PcSetMember equ 48h
PcStallScaleFactor equ 4Ch
PcHal equ 94h
PcPrcbData equ 120h
ProcessorControlRegisterLength equ 0FE0h
TebPeb equ 30h
PebBeingDebugged equ 2
PebKernelCallbackTable equ 2Ch

;  Defines for user shared data
USER_SHARED_DATA equ 0FFDF0000h
MM_SHARED_USER_DATA_VA equ 07FFE0000h
USERDATA equ ds:[0FFDF0000h]
PF_XMMI_INSTRUCTIONS_AVAILABLE equ 6

;  Tss Structure Offset Definitions
TssEsp0 equ 4
TssCR3 equ 1Ch
TssEip equ 20h
TssEFlags equ 24h
TssEax equ 28h
TssEbx equ 34h
TssEcx equ 2Ch
TssEdx equ 30h
TssEsp equ 38h
TssEbp equ 3Ch
TssEsi equ 40h
TssEdi equ 44h
TssEs equ 48h
TssCs equ 4Ch
TssSs equ 50h
TssDs equ 54h
TssFs equ 58h
TssGs equ 5Ch
TssLDT equ 60h
TssIoMapBase equ 66h
TssIoMaps equ 68h
TssLength equ 20ACh

;  Gdt Descriptor Offset Definitions
KGDT_R3_DATA equ 20h
KGDT_R3_CODE equ 18h
KGDT_R0_CODE equ 8
KGDT_R0_DATA equ 10h
KGDT_R0_PCR equ 30h
KGDT_STACK16 equ 0F8h
KGDT_CODE16 equ 0F0h
KGDT_TSS equ 28h
KGDT_R3_TEB equ 38h
KGDT_DF_TSS equ 50h
KGDT_NMI_TSS equ 58h
KGDT_LDT equ 48h

;  GdtEntry Offset Definitions
KgdtBaseLow equ 2
KgdtBaseMid equ 4
KgdtBaseHi equ 7
KgdtLimitHi equ 6
KgdtLimitLow equ 0

;  Processor Block Structure Offset Definitions
PbCurrentThread equ 4
PbNextThread equ 8
PbIdleThread equ 0Ch
PbNumber equ 10h
PbSetMember equ 14h
PbCpuID equ 19h
PbCpuType equ 18h
PbCpuStep equ 1Ah
PbProcessorState equ 1Ch
PbHalReserved equ 37Ch
PbLockQueue equ 418h
PbNpxThread equ 520h
PbInterruptCount equ 524h
PbKernelTime equ 528h
PbUserTime equ 52Ch
PbDpcTime equ 530h
PbDebugDpcTime equ 534h
PbInterruptTime equ 538h
PbAdjustDpcThreshold equ 53Ch
PbPageColor equ 540h
PbSkipTick equ 544h
PbNodeShiftedColor equ 548h
PbParentNode equ 54Ch
PbMultiThreadProcessorSet equ 550h
PbMultiThreadSetMaster equ 554h
PbSecondaryColorMask equ 558h
PbAlignmentFixupCount equ 578h
PbDcacheFlushCount equ 580h
PbExceptionDispatchCount equ 584h
PbFirstLevelTbFills equ 588h
PbFloatingEmulationCount equ 58Ch
PbIcacheFlushCount equ 590h
PbSecondLevelTbFills equ 594h
PbSystemCalls equ 598h
PbPPLookasideList equ 5E0h
PbPPNPagedLookasideList equ 660h
PbPPPagedLookasideList equ 760h
PbPacketBarrier equ 860h
PbReverseStall equ 864h
PbIpiFrame equ 868h
PbCurrentPacket equ 8A0h
PbTargetSet equ 8ACh
PbWorkerRoutine equ 8B0h
PbIpiFrozen equ 8B4h
PbRequestSummary equ 8E0h
PbSignalDone equ 8E4h
PbDpcListHead equ 920h
PbDpcLock equ 928h
PbDpcQueueDepth equ 92Ch
PbDpcCount equ 930h
PbDpcStack equ 948h
PbMaximumDpcQueueDepth equ 94Ch
PbDpcRequestRate equ 950h
PbMinimumDpcRate equ 954h
PbDpcInterruptRequested equ 958h
PbDpcThreadRequested equ 959h
PbDpcRoutineActive equ 95Ah
PbDpcThreadActive equ 95Bh
PbPrcbLock equ 95Ch
PbDpcLastCount equ 960h
PbTimerHand equ 964h
PbTimerRequest equ 968h
PbQuantumEnd equ 981h
PbIdleSchedule equ 983h
PbReadySummary equ 9E8h
PbDispatcherReadyListHead equ 9F0h
PbDeferredReadyListHead equ 0AF0h
PbNpxSaveArea equ 0B90h
PbChainedInterruptList equ 0B20h
PbPowerState equ 0DA0h
ProcessorBlockLength equ 0EC0h

;  Processor Power State Offset Definitions
PpIdleFunction equ 0

;  Immediate Interprocessor Command Definitions
IPI_APC equ 1
IPI_DPC equ 2
IPI_FREEZE equ 4
IPI_PACKET_READY equ 8
IPI_SYNCH_REQUEST equ 10h

;  Thread Environment Block Structure Offset Definitions (Alternate Prefixes)
TbExceptionList equ 0
TbStackBase equ 4
TbStackLimit equ 8
TbEnvironmentPointer equ 1Ch
TbVersion equ 10h
TbFiberData equ 10h
TbArbitraryUserPointer equ 14h
TbClientId equ 20h
TbThreadLocalStoragePointer equ 2Ch
TbCountOfOwnedCriticalSections equ 38h
TbSystemReserved1 equ 0CCh
TbVdm equ 0F18h
TbCsrClientThread equ 3Ch
TbGdiThreadLocalInfo equ 6C8h
TbglDispatchTable equ 7C4h
TbglSectionInfo equ 0BE0h
TbglSection equ 0BE4h
TbglTable equ 0BE8h
TbglCurrentRC equ 0BECh
TbglContext equ 0BF0h
TbWOW32Reserved equ 0C0h
TbExceptionCode equ 1A4h
TbDeallocationStack equ 0E0Ch
TbGdiBatchCount equ 0F70h

;  Time Fields (TIME_FIELDS) Structure Offset Definitions
TfSecond equ 0Ah
TfMinute equ 8
TfHour equ 6
TfWeekday equ 0Eh
TfDay equ 4
TfMonth equ 2
TfYear equ 0
TfMilliseconds equ 0Ch

;  constants for system irql and IDT vector conversion
MAXIMUM_IDTVECTOR equ 0FFh
MAXIMUM_PRIMARY_VECTOR equ 0FFh
PRIMARY_VECTOR_BASE equ 30h
RPL_MASK equ 3
MODE_MASK equ 1

;  Flags in the CR0 register
CR0_PG equ 80000000h
CR0_ET equ 10h
CR0_TS equ 8
CR0_EM equ 4
CR0_MP equ 2
CR0_PE equ 1
CR0_CD equ 40000000h
CR0_NW equ 20000000h
CR0_AM equ 40000h
CR0_WP equ 10000h
CR0_NE equ 20h

;  Flags in the CR4 register
CR4_VME equ 1
CR4_PVI equ 2
CR4_TSD equ 4
CR4_DE equ 8
CR4_PSE equ 10h
CR4_PAE equ 20h
CR4_MCE equ 40h
CR4_PGE equ 80h
CR4_FXSR equ 200h
CR4_XMMEXCPT equ 400h

;  Miscellaneous Definitions
MAXIMUM_PROCESSORS equ 20h
INITIAL_STALL_COUNT equ 64h
IRQL_NOT_GREATER_OR_EQUAL equ 9
IRQL_NOT_LESS_OR_EQUAL equ 0Ah
MUTEX_ALREADY_OWNED equ 0BFh
THREAD_NOT_MUTEX_OWNER equ 11h
SPIN_LOCK_ALREADY_OWNED equ 0Fh
SPIN_LOCK_NOT_OWNED equ 10h
BASE_PRIORITY_THRESHOLD equ 8
EVENT_PAIR_INCREMENT equ 1
LOW_REALTIME_PRIORITY equ 10h
BlackHole equ 0FFFFA000h
KERNEL_LARGE_STACK_COMMIT equ 3000h
KERNEL_STACK_SIZE equ 3000h
DOUBLE_FAULT_STACK_SIZE equ 3000h
EFLAG_SELECT equ 0C000h
BREAKPOINT_BREAK equ 0
CLOCK_QUANTUM_DECREMENT equ 3
READY_SKIP_QUANTUM equ 2
THREAD_QUANTUM equ 6
WAIT_QUANTUM_DECREMENT equ 1
ROUND_TRIP_DECREMENT_COUNT equ 10h
KI_EXCEPTION_ACCESS_VIOLATION equ 010000004h

;  Trap Frame Offset Definitions and Length
TsExceptionList equ 4Ch
TsPreviousPreviousMode equ 48h
TsSegGs equ 30h
TsSegFs equ 50h
TsSegEs equ 34h
TsSegDs equ 38h
TsEdi equ 54h
TsEsi equ 58h
TsEbp equ 60h
TsEbx equ 5Ch
TsEdx equ 3Ch
TsEcx equ 40h
TsEax equ 44h
TsErrCode equ 64h
TsEip equ 68h
TsSegCs equ 6Ch
TsEflags equ 70h
TsHardwareEsp equ 74h
TsHardwareSegSs equ 78h
TsTempSegCs equ 10h
TsTempEsp equ 14h
TsDbgEbp equ 0
TsDbgEip equ 4
TsDbgArgMark equ 8
TsDbgArgPointer equ 0Ch
TsDr0 equ 18h
TsDr1 equ 1Ch
TsDr2 equ 20h
TsDr3 equ 24h
TsDr6 equ 28h
TsDr7 equ 2Ch
TsV86Es equ 7Ch
TsV86Ds equ 80h
TsV86Fs equ 84h
TsV86Gs equ 88h
KTRAP_FRAME_LENGTH equ 8Ch
KTRAP_FRAME_ALIGN equ 4
FRAME_EDITED equ 0FFF8h
EFLAGS_ALIGN_CHECK equ 40000h
EFLAGS_V86_MASK equ 20000h
EFLAGS_INTERRUPT_MASK equ 200h
EFLAGS_TF equ 100h
EFLAGS_VIF equ 80000h
EFLAGS_VIP equ 100000h
EFLAGS_USER_SANITIZE equ 03F4DD7h

;  Context Frame Offset and Flag Definitions
CONTEXT_FULL equ 10007h
CONTEXT_DEBUG_REGISTERS equ 10010h
CONTEXT_CONTROL equ 10001h
CONTEXT_FLOATING_POINT equ 10008h
CONTEXT_INTEGER equ 10002h
CONTEXT_SEGMENTS equ 10004h

CsContextFlags equ 0
CsDr0 equ 4
CsDr1 equ 8
CsDr2 equ 0Ch
CsDr3 equ 10h
CsDr6 equ 14h
CsDr7 equ 18h
CsFloatSave equ 1Ch
CsSegGs equ 8Ch
CsSegFs equ 90h
CsSegEs equ 94h
CsSegDs equ 98h
CsEdi equ 9Ch
CsEsi equ 0A0h
CsEbx equ 0A4h
CsEdx equ 0A8h
CsEcx equ 0ACh
CsEax equ 0B0h
CsEbp equ 0B4h
CsEip equ 0B8h
CsSegCs equ 0BCh
CsEflags equ 0C0h
CsEsp equ 0C4h
CsSegSs equ 0C8h
CsExtendedRegisters equ 0CCh
ContextFrameLength equ 2D0h
CONTEXT_ALIGNED_SIZE equ 2CCh

DR6_LEGAL equ 0E00Fh
DR7_LEGAL equ 0FFFF0155h
DR7_ACTIVE equ 55h
DR7_RESERVED_MASK equ 0DC00h

ErrHandler equ 4
ErrNext equ 0
ErrLength equ 8

;  Floating save area field offset definitions
FpControlWord equ 0
FpStatusWord equ 4
FpTagWord equ 8
FpErrorOffset equ 0Ch
FpErrorSelector equ 10h
FpDataOffset equ 14h
FpDataSelector equ 18h
FpRegisterArea equ 1Ch
FpCtxtCr0NpxState equ 6Ch

;  FX Floating save area field offset definitions
FxControlWord equ 0
FxStatusWord equ 2
FxTagWord equ 4
FxErrorOpcode equ 6
FxErrorOffset equ 8
FxErrorSelector equ 0Ch
FxDataOffset equ 10h
FxDataSelector equ 14h
FxMXCsr equ 18h
FxFpRegisterArea equ 20h
FpNpxSavedCpu equ 208h
FpCr0NpxState equ 20Ch

NPX_FRAME_LENGTH equ 210h

;  Processor State Frame Offset Definitions
PsContextFrame equ 0
PsSpecialRegisters equ 2CCh
SrCr0 equ 0
SrCr2 equ 4
SrCr3 equ 8
SrCr4 equ 0Ch
SrKernelDr0 equ 10h
SrKernelDr1 equ 14h
SrKernelDr2 equ 18h
SrKernelDr3 equ 1Ch
SrKernelDr6 equ 20h
SrKernelDr7 equ 24h
SrGdtr equ 2Ah
SrIdtr equ 32h
SrTr equ 38h
SrLdtr equ 3Ah
ProcessorStateLength equ 320h

;  EPROCESS
EpDebugPort equ 0CCh
EpVdmObjects equ 144h

;  Machine type definitions (Temporarily)
MACHINE_TYPE_ISA equ 0
MACHINE_TYPE_EISA equ 1
MACHINE_TYPE_MCA equ 2

;  KeFeatureBits defines
KF_V86_VIS equ 1
KF_RDTSC equ 2
KF_CR4 equ 4
KF_GLOBAL_PAGE equ 10h
KF_LARGE_PAGE equ 20h
KF_CMPXCHG8B equ 80h
KF_FAST_SYSCALL equ 1000h

;  LoaderParameterBlock offsets relative to base
LpbLoadOrderListHead equ 0
LpbMemoryDescriptorListHead equ 8
LpbKernelStack equ 18h
LpbPrcb equ 1Ch
LpbProcess equ 20h
LpbThread equ 24h
LpbI386 equ 5Ch
LpbRegistryLength equ 28h
LpbRegistryBase equ 2Ch
LpbConfigurationRoot equ 30h
LpbArcBootDeviceName equ 34h
LpbArcHalDeviceName equ 38h
LpbExtension equ 58h

;  LoaderParameterExtension offsets relative to base
LpeHalpIRQLToTPR equ 40h
LpeHalpVectorToIRQL equ 44h
PAGE_SIZE equ 1000h

;  VDM equates.
VDM_INDEX_Invalid equ 0
VDM_INDEX_0F equ 1
VDM_INDEX_ESPrefix equ 2
VDM_INDEX_CSPrefix equ 3
VDM_INDEX_SSPrefix equ 4
VDM_INDEX_DSPrefix equ 5
VDM_INDEX_FSPrefix equ 6
VDM_INDEX_GSPrefix equ 7
VDM_INDEX_OPER32Prefix equ 8
VDM_INDEX_ADDR32Prefix equ 9
VDM_INDEX_INSB equ 0Ah
VDM_INDEX_INSW equ 0Bh
VDM_INDEX_OUTSB equ 0Ch
VDM_INDEX_OUTSW equ 0Dh
VDM_INDEX_PUSHF equ 0Eh
VDM_INDEX_POPF equ 0Fh
VDM_INDEX_INTnn equ 10h
VDM_INDEX_INTO equ 11h
VDM_INDEX_IRET equ 12h
VDM_INDEX_NPX equ 13h
VDM_INDEX_INBimm equ 14h
VDM_INDEX_INWimm equ 15h
VDM_INDEX_OUTBimm equ 16h
VDM_INDEX_OUTWimm equ 17h
VDM_INDEX_INB equ 18h
VDM_INDEX_INW equ 19h
VDM_INDEX_OUTB equ 1Ah
VDM_INDEX_OUTW equ 1Bh
VDM_INDEX_LOCKPrefix equ 1Ch
VDM_INDEX_REPNEPrefix equ 1Dh
VDM_INDEX_REPPrefix equ 1Eh
VDM_INDEX_CLI equ 1Fh
VDM_INDEX_STI equ 20h
VDM_INDEX_HLT equ 21h
MAX_VDM_INDEX equ 22h

;  VDM feature bits.
V86_VIRTUAL_INT_EXTENSIONS equ 1
PM_VIRTUAL_INT_EXTENSIONS equ 2

;  Selector types.
SEL_TYPE_NP equ 40h

;  Usermode callout frame definitions
CuInStk equ 0
CuTrFr equ 4
CuCbStk equ 8
CuEdi equ 0Ch
CuEsi equ 10h
CuEbx equ 14h
CuEbp equ 18h
CuRet equ 1Ch
CuOutBf equ 20h
CuOutLn equ 24h

;  VDM_PROCESS_OBJECTS
VpVdmTib equ 98h

;  Wow64 turbo dispatch system call types
ServiceNoTurbo equ 0

;  No arguments system call
Service0Arg equ 1
Service0ArgReloadState equ 2

;  One argument system call types
Service1ArgSp equ 3
Service1ArgNSp equ 4

;  Two arguments system call types
Service2ArgNSpNSp equ 5
Service2ArgNSpNSpReloadState equ 6
Service2ArgSpNSp equ 7
Service2ArgSpSp equ 8
Service2ArgNSpSp equ 9

;  Three arguments system call types
Service3ArgNSpNSpNSp equ 0Ah
Service3ArgSpSpSp equ 0Bh
Service3ArgSpNSpNSp equ 0Ch
Service3ArgSpNSpNSpReloadState equ 0Dh
Service3ArgSpSpNSp equ 0Eh
Service3ArgSpNSpSp equ 10h

;  Four arguments system call types
Service4ArgNSpNSpNSpNSp equ 11h
Service4ArgSpSpNSpNSp equ 12h
Service4ArgSpSpNSpNSpReloadState equ 13h
Service4ArgSpNSpNSpNSp equ 14h
Service4ArgSpNSpNSpNSpReloadState equ 15h
Service4ArgNSpSpNSpNSp equ 16h
Service4ArgSpSpSpNSp equ 17h

;  Special system calls
ServiceCpupTdQuerySystemTime equ 18h
ServiceCpupTdGetCurrentProcessorNumber equ 19h
ServiceCpupTdReadWriteFile equ 1Ah
ServiceCpupTdDeviceIoControlFile equ 1Bh
ServiceCpupTdRemoveIoCompletion equ 1Ch
ServiceCpupTdWaitForMultipleObjects equ 1Dh
ServiceCpupTdWaitForMultipleObjects32 equ 1Eh
Wow64ServiceTypesCount equ 1Fh
