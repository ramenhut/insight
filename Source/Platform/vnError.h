
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnError.h
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

#ifndef __VN_ERROR_H__
#define __VN_ERROR_H__

#include "vnPlatform.h"
#include "vnProfile.h"
#include "vnStandard.h"

#define VN_ERROR_SHIFT_RETURN(x)                                    ( ( x >> 8 ) - 1 )
#define VN_ERROR_CREATE_STRING(x)                                   ( (CHAR*) #x )

//
// Standard Imagine errors. Note that these errors must align with 
// VN_SUCCESS and VN_FAILURE.
//

#define VN_ERROR_INVALIDARG                                         ( 1L << 8 )
#define VN_ERROR_NOTIMPL                                            ( 2L << 8 )
#define VN_ERROR_OUTOFMEMORY                                        ( 3L << 8 )
#define VN_ERROR_UNDEFINED                                          ( 4L << 8 )
#define VN_ERROR_HARDWAREFAIL                                       ( 5L << 8 )
#define VN_ERROR_INVALID_INDEX                                      ( 6L << 8 )
#define VN_ERROR_CAPACITY_LIMIT                                     ( 7L << 8 )
#define VN_ERROR_INVALID_RESOURCE                                   ( 8L << 8 )
#define VN_ERROR_OPERATION_TIMEDOUT                                 ( 9L << 8 )
#define VN_ERROR_EXECUTION_FAILURE                                  ( 10L << 8 )
#define VN_ERROR_PERMISSION_DENIED                                  ( 11L << 8 )
#define VN_ERROR_IO_FAILURE                                         ( 12L << 8 )
#define VN_ERROR_RESOURCE_UNREACHABLE                               ( 13L << 8 )
#define VN_ERROR_SYSTEM_FAILURE                                     ( 14L << 8 )
#define VN_ERROR_NOT_READY                                          ( 15L << 8 )
#define VN_ERROR_OPERATION_COMPLETED                                ( 16L << 8 )
#define VN_ERROR_RESOURCE_UNUSED                                    ( 17L << 8 )

#define VN_SUCCESS_OPERATION_COMPLETED                              ( 1L )

//
// Simple error interface
//

inline UINT32 vnPostErrorInternal( UINT32 uiError, CONST CHAR * szFunctionName, CHAR * szFileName, UINT32 uiLine )
{
    
#ifdef VN_DEBUG

    //
    // This is fairly defensive - we will halt all execution in the event of an error.
    // We should consider adding an additional profile/scheme for the shipping version,
    // so that we can maintain debug spew (but not catastrophic breaks) in release builds.
    //

    vnDebugBreak();

#endif

    //
    // We display the spew after the debug break so that we preserve the stack as much as 
    // possible for the debugger. Developers may simply step past this break to see the 
    // spew.
    //
        
    CHAR * szShortenedPath = szFileName;
        
    for ( INT32 i = (INT32) strlen( szFileName ); i >= 0; i-- )
    {
        if ( szFileName[ i ] == '/' ) break;
            
        szShortenedPath = &szFileName[ i ];
    }

    VN_ERR( "*** VN-RIP *** %s @%s in %s:%i", szFunctionName, szShortenedPath, uiLine );
      
    return uiError;
}

#define vnPostError( x ) vnPostErrorInternal( x, __VN_FUNCTION__, (CHAR *) __FILE__, __LINE__ )

#endif // __VN_ERROR_H__