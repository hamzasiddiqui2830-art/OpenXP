/*++

Module Name:

    specstrings.h

Abstract:

    NTOSKRNL compatibility implementation of the WRK SpecStrings header.

    This header is deliberately self-contained.  Do not include the host
    compiler/Windows SDK sal.h here: the WRK build supplies its own SDK and
    annotation vocabulary and must remain independent of the host SDK.

    The annotations are compile-time metadata only.  For the kernel build,
    they are represented by no-op compatibility macros unless a local
    definition is already available.

--*/

#ifndef NTOSKRNL_SPECSTRINGS_H
#define NTOSKRNL_SPECSTRINGS_H

/*
 * Base SpecStrings marker.
 */
#ifndef __specstrings
#define __specstrings
#endif

/*
 * Legacy WRK annotations.  Keep these local rather than importing the
 * compiler's sal.h, whose definitions conflict with the WRK SDK macros.
 */
#ifndef __control_entrypoint
#define __control_entrypoint(category)
#endif
#ifndef __rpc_entry
#define __rpc_entry
#endif
#ifndef __kernel_entry
#define __kernel_entry
#endif
#ifndef __gdi_entry
#define __gdi_entry
#endif
#ifndef __data_entrypoint
#define __data_entrypoint(category)
#endif
#ifndef __blocksOn
#define __blocksOn(resource)
#endif
#ifndef __range
#define __range(low,high)
#endif
#ifndef __in_range
#define __in_range(low,high)
#endif
#ifndef __out_range
#define __out_range(low,high)
#endif
#ifndef __deref_in_range
#define __deref_in_range(low,high)
#endif
#ifndef __deref_out_range
#define __deref_out_range(low,high)
#endif
#ifndef __deref_inout_range
#define __deref_inout_range(low,high)
#endif
#ifndef __field_range
#define __field_range(low,high)
#endif
#ifndef __range_max
#define __range_max(low,high)
#endif
#ifndef __range_min
#define __range_min(low,high)
#endif
#ifndef __bound
#define __bound
#endif
#ifndef __in_bound
#define __in_bound
#endif
#ifndef __out_bound
#define __out_bound
#endif
#ifndef __deref_out_bound
#define __deref_out_bound
#endif
#ifndef __assume_bound
#define __assume_bound(value)
#endif
#ifndef __analysis_assume_nullterminated
#define __analysis_assume_nullterminated(value)
#endif
#ifndef __allocator
#define __allocator
#endif
#ifndef __deallocate
#define __deallocate(kind)
#endif
#ifndef __deallocate_opt
#define __deallocate_opt(kind)
#endif
#ifndef __post_invalid
#define __post_invalid
#endif
#ifndef __post_nullnullterminated
#define __post_nullnullterminated
#endif
#ifndef __possibly_notnullterminated
#define __possibly_notnullterminated
#endif
#ifndef __field_ecount
#define __field_ecount(size)
#endif
#ifndef __field_bcount
#define __field_bcount(size)
#endif
#ifndef __field_xcount
#define __field_xcount(size)
#endif
#ifndef __field_ecount_opt
#define __field_ecount_opt(size)
#endif
#ifndef __field_bcount_opt
#define __field_bcount_opt(size)
#endif
#ifndef __field_xcount_opt
#define __field_xcount_opt(size)
#endif
#ifndef __field_ecount_part
#define __field_ecount_part(size,init)
#endif
#ifndef __field_bcount_part
#define __field_bcount_part(size,init)
#endif
#ifndef __field_xcount_part
#define __field_xcount_part(size,init)
#endif
#ifndef __field_ecount_part_opt
#define __field_ecount_part_opt(size,init)
#endif
#ifndef __field_bcount_part_opt
#define __field_bcount_part_opt(size,init)
#endif
#ifndef __field_xcount_part_opt
#define __field_xcount_part_opt(size,init)
#endif
#ifndef __field_ecount_full
#define __field_ecount_full(size)
#endif
#ifndef __field_bcount_full
#define __field_bcount_full(size)
#endif
#ifndef __field_xcount_full
#define __field_xcount_full(size)
#endif
#ifndef __field_ecount_full_opt
#define __field_ecount_full_opt(size)
#endif
#ifndef __field_bcount_full_opt
#define __field_bcount_full_opt(size)
#endif
#ifndef __field_xcount_full_opt
#define __field_xcount_full_opt(size)
#endif
#ifndef __field_nullterminated
#define __field_nullterminated
#endif
#ifndef __struct_bcount
#define __struct_bcount(size)
#endif
#ifndef __struct_xcount
#define __struct_xcount(size)
#endif

/*
 * Core legacy parameter annotations.  These are intentionally no-ops in
 * this self-contained SDK.  They preserve source compatibility without
 * importing host SAL definitions.
 */
#ifndef __in
#define __in
#endif
#ifndef __out
#define __out
#endif
#ifndef __inout
#define __inout
#endif
#ifndef __in_opt
#define __in_opt
#endif
#ifndef __out_opt
#define __out_opt
#endif
#ifndef __inout_opt
#define __inout_opt
#endif
#ifndef __in_ecount
#define __in_ecount(size)
#endif
#ifndef __in_bcount
#define __in_bcount(size)
#endif
#ifndef __out_ecount
#define __out_ecount(size)
#endif
#ifndef __out_bcount
#define __out_bcount(size)
#endif
#ifndef __inout_ecount
#define __inout_ecount(size)
#endif
#ifndef __inout_bcount
#define __inout_bcount(size)
#endif
#ifndef __in_ecount_opt
#define __in_ecount_opt(size)
#endif
#ifndef __in_bcount_opt
#define __in_bcount_opt(size)
#endif
#ifndef __out_ecount_opt
#define __out_ecount_opt(size)
#endif
#ifndef __out_bcount_opt
#define __out_bcount_opt(size)
#endif
#ifndef __inout_ecount_opt
#define __inout_ecount_opt(size)
#endif
#ifndef __inout_bcount_opt
#define __inout_bcount_opt(size)
#endif
#ifndef __in_ecount_part
#define __in_ecount_part(size,init)
#endif
#ifndef __in_bcount_part
#define __in_bcount_part(size,init)
#endif
#ifndef __out_ecount_part
#define __out_ecount_part(size,init)
#endif
#ifndef __out_bcount_part
#define __out_bcount_part(size,init)
#endif
#ifndef __inout_ecount_part
#define __inout_ecount_part(size,init)
#endif
#ifndef __inout_bcount_part
#define __inout_bcount_part(size,init)
#endif
#ifndef __in_ecount_full
#define __in_ecount_full(size)
#endif
#ifndef __in_bcount_full
#define __in_bcount_full(size)
#endif
#ifndef __out_ecount_full
#define __out_ecount_full(size)
#endif
#ifndef __out_bcount_full
#define __out_bcount_full(size)
#endif
#ifndef __inout_ecount_full
#define __inout_ecount_full(size)
#endif
#ifndef __inout_bcount_full
#define __inout_bcount_full(size)
#endif
#ifndef __in_awcount
#define __in_awcount(expr,size)
#endif
#ifndef __out_awcount
#define __out_awcount(expr,size)
#endif

#endif /* NTOSKRNL_SPECSTRINGS_H */
