
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnPlatform.h
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Description:
//
//   This module is part of the Vision Basecode and has been compacted and reduced
//   for inclusion within Insight.
//
//  Additional Information:
//
//   For more information, visit http://www.bertolami.com.
//

#ifndef __VN_PLATFORM_H__
#define __VN_PLATFORM_H__

#if defined ( _WIN32 ) || defined ( _WIN64 )

    #include "windows.h"                                        // required for some types  
    #include "mmsystem.h"
    #include "direct.h"
   
        #pragma warning ( disable : 4244 )                      // conversion, possible loss of data   
        #pragma warning ( disable : 4018 )                      // signed / unsigned mismatch
        #pragma warning ( disable : 4996 )                      // deprecated interfaces
        #pragma warning ( disable : 4221 )                      // empty translation unit
        #pragma warning ( disable : 4273 )                      // inconsistent linkage

    #define VN_PLATFORM_WINDOWS                                 // building a Windows application
    #define VN_LE_FORMAT                                        // targeting a little endian format

    #if defined ( _M_IX86 )
        #define VN_ARCH_32BIT                                   // building for a 32 bit processor
        #define VN_FAMILY_X86                                   // building with an x86 ISA
    #elif defined ( _M_X64 )
        #define VN_ARCH_64BIT                                   // building for a 64 bit processor
        #define VN_FAMILY_X64                                   // building with an x64 ISA
    #endif

    //
    // Some processor families support multiple different 
    // runtimes, so we define appropriately here.
    //

    #if defined ( _WIN64 )
        #define VN_FORMAT_64BIT                                 // building a 64 bit application
    #elif defined ( _WIN32 )                                    // always defined, even for 64 bit
        #define VN_FORMAT_32BIT                                 // building a 32 bit application
    #endif

    #define VN_PLATFORM_STRING                                  "VN_PLATFORM_WINDOWS"

#elif defined ( _XENON ) || defined ( __APPLE__ )

    //
    // The compact version of the Vision Basecode included in this source release
    // does not have broad platform support. Consult the full Vision Basecode if 
    // you require support for additional platforms.
    //

    #error "This platform is not supported by this version of the Vision Basecode." \
           "Consult the full framework for expanded support."

#endif

//
// Platform specific debug support
//

#if defined ( VN_PLATFORM_WINDOWS ) || defined ( VN_PLATFORM_XBOX360 )

    #ifdef _DEBUG
        #define VN_DEBUG                    _DEBUG
    #elif defined ( DEBUG )
        #define VN_DEBUG                    DEBUG
    #endif

    #ifdef VN_DEBUG
        #define vnDebugBreak                __debugbreak
    #endif

    #define __VN_FUNCTION__                 __FUNCTION__

#endif

//
// Platform specific intrinsic types
//

#if defined ( VN_PLATFORM_WINDOWS ) || defined ( VN_PLATFORM_XBOX360 )

    #define VOID                            void                
  
    //
    // Canonical types are already defined by basetd.h
    //

#endif

//
// Platform agnostic intrinsic types
//

typedef float                               FLOAT32;             // 32-bit float
typedef double                              FLOAT64;             // 64-bit float

typedef char                                CHAR;                // ansi char
typedef wchar_t                             WCHAR;               // unicode

#endif // __VN_PLATFORM_H__