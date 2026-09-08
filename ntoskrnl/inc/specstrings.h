/*++

Module Name:

    specstrings.h

Abstract:

    NTOSKRNL compatibility shim for the legacy WRK SpecStrings header.
    Modern MSVC provides the legacy __in/__out/__ecount family through
    sal.h. The WRK copy redeclares those macros with older SAL primitives,
    which conflicts with current MSVC and turns into C4005 under /WX.

    Keep the WRK implementation for non-MSVC builds. On MSVC, use the
    compiler's SAL definitions and provide only WRK-specific compatibility
    aliases that are not supplied by sal.h.

--*/

#ifndef NTOSKRNL_SPECSTRINGS_H
#define NTOSKRNL_SPECSTRINGS_H

#if defined(_MSC_VER)

#include <sal.h>

#ifndef __specstrings
#define __specstrings
#endif

/*
 * Windows native headers still use these legacy SpecStrings entry-point
 * annotations. Modern sal.h does not provide the old __kernel_entry family,
 * so keep them as no-op compatibility annotations for the WRK build.
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

/* WRK-specific aliases which are not part of the modern SAL surface. */
#ifndef __out_awcount
#define __out_awcount(expr,size) __out_bcount(size)
#endif
#ifndef __in_awcount
#define __in_awcount(expr,size) __in_bcount(size)
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

#else

#include "../../sdk/sdk/inc/specstrings.h"

#endif

#endif /* NTOSKRNL_SPECSTRINGS_H */
