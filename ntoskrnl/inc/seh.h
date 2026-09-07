/*
 * PROJECT:         ReactOS Kernel
 * LICENSE:         GNU GPL - See COPYING in the top level directory
 * PURPOSE:         Structured Exception Handling (SEH) Abstraction
 * PROGRAMMER:      ReactOS Team
 * 
 * DESCRIPTION:     Abstracts SEH for different compilers.
 *                  - MSVC: Uses native __try/__except/__finally
 *                  - GCC/Clang: Uses PSEH3 (Portable SEH)
 */

#pragma once

#ifndef _SEH_H_
#define _SEH_H_

#ifdef _MSC_VER
    /* 
     * Microsoft Visual C++ - Native SEH Support 
     * No extra headers or libraries needed, keywords are built-in.
     */
    
    /* Ensure standard SEH header is available for exception codes if needed */
    #include <excpt.h>

    /* Native keywords are used directly: __try, __except, __finally, __leave */
    /* We define the single-underscore versions for code compatibility if desired, 
       but standard ReactOS code usually uses __try directly or has its own macros. 
       Here we map the common ReactOS style if it differs, or just ensure availability. */
       
    /* If the codebase expects 'try'/'except' without underscores, map them. 
       However, standard NT kernel code typically uses __try. 
       Assuming the user wants compatibility with code using 'try'/'except' as seen in previous errors: */
    #ifndef try
        #define try __try
    #endif
    #ifndef except
        #define except __except
    #endif
    #ifndef finally
        #define finally __finally
    #endif
    #ifndef leave
        #define leave __leave
    #endif
    #ifndef AbnormalTermination
        #define AbnormalTermination() __abnormal_termination()
    #endif
    #ifndef ExceptionCode
        #define ExceptionCode() _exception_code()
    #endif
    #ifndef ExceptionInfo
        #define ExceptionInfo() _exception_info()
    #endif

#elif defined(__GNUC__) || defined(__clang__)
    /* 
     * GCC or Clang - Use PSEH3 (Portable SEH)
     * Requires linking against libpseh3.a
     */
    
    /* Include the PSEH3 implementation provided in the workspace */
    #include <pseh3.h>
    
    /* PSEH3 defines _SEH3_TRY, _SEH3_EXCEPT, etc. 
       We map the standard keywords to PSEH3 macros for seamless integration. */
       
    #define try         _SEH3_TRY
    #define except(x)   _SEH3_EXCEPT(x)
    #define finally     _SEH3_FINALLY
    #define leave       goto _SEH3$_l_Leave /* Handled internally by _SEH3_END scope */
    #define AbnormalTermination() _abnormal_termination()
    #define ExceptionCode() _exception_code()
    #define ExceptionInfo() _exception_info()
    
    /* 
     * NOTE: When using PSEH3, the syntax changes slightly due to macro expansion.
     * Usage:
     *   try {
     *       ...
     *   } except (EXCEPTION_EXECUTE_HANDLER) {
     *       ...
     *   }
     * 
     * The PSEH3 header handles the complex asm logic.
     */

#else
    #error "Unsupported compiler for SEH. Only MSVC, GCC, and Clang are supported."
#endif

#endif /* _SEH_H_ */
