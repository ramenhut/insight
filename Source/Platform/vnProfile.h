
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnProfile.h
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

#ifndef __VN_PROFILE_H__
#define __VN_PROFILE_H__

#include "stdio.h"
#include "vnPlatform.h"

#ifdef VN_DEBUG

    #define VN_PARAM_CHECK                  (1)
    #define VN_ERR( fmt, ... )              do { printf("[VN-ERR] "); printf( fmt, ##__VA_ARGS__ ); printf("\n"); vnDebugBreak(); } while(0)
    #define VN_MSG( fmt, ... )              do { printf("[VN-MSG] "); printf( fmt, ##__VA_ARGS__ ); printf("\n"); } while(0)
    #define VN_WRN( fmt, ... )              do { printf("[VN-WRN] "); printf( fmt, ##__VA_ARGS__ ); printf("\n"); } while(0)

#else

    #define VN_PARAM_CHECK                  (0)
    #define VN_ERR( fmt, ... )                              
    #define VN_MSG( fmt, ... )              do { printf("[VN-MSG] "); printf( fmt, ##__VA_ARGS__ ); printf("\n"); } while(0)               
    #define VN_WRN( fmt, ... )  

#endif 

#endif // __VN_PROFILE_H__