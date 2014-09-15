
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnStandard.h
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

#ifndef __VN_STANDARD_H__
#define __VN_STANDARD_H__

#include "stdlib.h"
#include "stdarg.h"

#define vnZeroMemory( b, size )                     memset( b, 0, size )
#define vnCopyMemory( d, s, i )                     memcpy( d, s, i )
#define vnCompareMemory( a, b, i )                  memcmp( a, b, i )                         

#define VN_FAILED( x )                              ( !!( (x) & VN_FAILURE ) )
#define VN_ISHANDLE( x )                            ( !VN_FAILED( x ) )
#define VN_SUCCEEDED( x )                           ( !!( (x) & VN_SUCCESS ) )

#define VN_PARENT_ASSIGN( _pclass, _lptr, _rptr )   *( (_pclass *) _lptr ) = *( (_pclass *) _rptr );

#define VN_MIN2( a, b )                             ( (a) < (b) ? (a) : (b) )
#define VN_MIN3( a, b, c )                          ( (c) < (a) ? ( (c) < (b) ? (c) : (b) ) : (a) < (b) ? (a) : (b) )
#define VN_MAX2( a, b )                             ( (a) > (b) ? (a) : (b) )
#define VN_MAX3( a, b, c )                          ( (c) > (a) ? ( (c) > (b) ? (c) : (b) ) : (a) > (b) ? (a) : (b) )

#define VN_VARG(fmt)                                va_list argptr;                                 \
                                                    char text[VN_MAX_PATH] = {0};                   \
                                                    va_start( argptr, fmt );                        \
                                                    vsprintf( text, fmt, argptr );                  \
                                                    va_end( argptr );      

#define KB                                          ( UINT32(1024) )
#define MB                                          ( KB * KB )
#define GB                                          ( MB * KB )

#define INLINE                                      inline
#define CONST                                       const
#define RESTRICT                                    __restrict

#define IN                                          // pointer parameter represents an input
#define OUT                                         // deprecated
#define INOUT                                       // pointer target will be modified, not allocated
#define OWNED                                       // pointer must be managed by owner
#define VN_NONVIRTUAL                               // class cannot contain virtual functions

//
// Expressed constants
//

#define	VN_MIN_PER_DAY                              1440
#define	VN_BYTE_TO_FLOAT                            0.003921568627450980392156862745098   // ( 1.0 / 255.0 ) 
#define VN_ONE_SEC_IN_MS                            ( 1000 )
#define VN_ONE_MINUTE_IN_MS                         ( VN_ONE_SEC_IN_MS * 60 )
#define VN_FIVE_MINUTES_IN_MS                       ( VN_ONE_MINUTE_IN_MS * 5 )

#define VN_MAX_STRLEN                               256
#define VN_MAX_PATH                                 512
#define VN_MAX_ERRLEN                               256

#define VN_MAX_INT64                                ( 0x7FFFFFFFFFFFFFFF )
#define VN_MAX_INT32                                ( 0x7FFFFFFF )
#define VN_MAX_INT16                                ( 0x7FFF )
#define VN_MAX_INT8                                 ( 0x7F )

#define VN_MAX_UINT64                               ( 0xFFFFFFFFFFFFFFFF )
#define VN_MAX_UINT32                               ( 0xFFFFFFFF )
#define VN_MAX_UINT16                               ( 0xFFFF )
#define VN_MAX_UINT8                                ( 0xFF )

#define VN_INFINITE                                 ( VN_MAX_INT32 )

#define VN_MIN_INT64                                ( -VN_MAX_INT64 - 1 )
#define VN_MIN_INT32                                ( -VN_MAX_INT32 - 1 )
#define VN_MIN_INT16                                ( -VN_MAX_INT16 - 1 )
#define VN_MIN_INT8                                 ( -VN_MAX_INT8 - 1 )

#define VN_STATUS                                   UINT32
#define VN_HANDLE                                   VOID*
#define VN_SUCCESS                                  0x000000FF
#define VN_FAILURE                                  0xFFFFFF00
#define VN_INVALID_HANDLE                           0xFFFFFFFF

#define VN_TEMPLATE_T                               template <class T>
#define VN_TEMPLATE_SPEC                            template <>
#define VN_TEMPLATE_PARTIAL_SPEC                    VN_TEMPLATE_T 

#endif // __VN_STANDARD_H__