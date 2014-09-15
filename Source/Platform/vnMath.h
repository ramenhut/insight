
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnMath.h
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

#ifndef __VN_MATH_H__
#define __VN_MATH_H__

#include "vnBase.h"
#include "math.h"

#define	VN_USE_FAST_32BIT_LOG2						(1)

//
// (!) Note: we rely upon inlined methods for calls that are type specific. 
//			 for all others, we use macros, which suggest type agnosticism.
//
 
#define VN_PI										( 3.14159262f )
#define VN_2PI										( 6.28318524f )
#define VN_INFINITY									( 1.0e15f )
#define VN_EPSILON									( 1.0e-5f )
#define VN_LOG2                                     ( 0.3010299956639f ) 
#define VN_REQUIRED_BITS( n )						( vnLog2( ( n ) ) + 1 )
#define VN_ROUND_OUT( n, a )						( n < 0 ? n - a : n + a )

CONST UINT8 VN_LOG2_BYTE_LUT[] = 
{
	0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
};

inline UINT8 vnLog2( UINT8 uiValue )
{
	return VN_LOG2_BYTE_LUT[ uiValue ];
}

inline UINT8 vnLog2( UINT16 uiValue )
{
	if ( uiValue <= VN_MAX_UINT8 ) 
	{
		return VN_LOG2_BYTE_LUT[ uiValue ];
	}

	return 8 + VN_LOG2_BYTE_LUT[ uiValue >> 8 ];
}

inline UINT32 vnLog2( UINT32 uiValue )
{

#ifdef VN_USE_FAST_32BIT_LOG2

    if ( uiValue <= VN_MAX_UINT16 )
	{
		return vnLog2( static_cast<UINT16>( uiValue ) );
	}

	return 16 + vnLog2( static_cast<UINT16>( uiValue >> 16 ) );

#else

	//
	// We keep this method around in case we ever wish to
	// (re)generate our log table.
	//

	if ( 0 == uiValue ) return 0;

	//
	// Note that this will provide accurate values for pow-2 aligned
	// inputs. For all else, the return will be truncated to the
	// nearest integer.
	//

    UINT32 iResult = 0;

    while ( uiValue >>= 1 ) iResult++;

    return iResult;

#endif

}

inline FLOAT32 vnLog( FLOAT32 logr ) 
{ 
    return log( logr ); 
}

inline FLOAT32 vnLog2( FLOAT32 logr )
{
    return vnLog( logr ) / VN_LOG2;
}

inline FLOAT32 vnInvSqrt( FLOAT32 f )
{
    //
    // Newton-Raphson approximation with a curiously awesome initial guess
    //
    
    FLOAT32 fhalf = 0.5f * f;
    INT32 i       = *reinterpret_cast<INT32 *>( &f );
    
    i = 0x5f3759df - ( i >> 1 );
    f = *reinterpret_cast<FLOAT32 *>( &i );
    f = f * ( 1.5f - fhalf * f * f );
    
    return f;
}

inline FLOAT32 vnSqrt( FLOAT32 f )
{    
    return 1.0f / vnInvSqrt( f );
}

inline UINT32 vnSqrt( UINT32 f )
{  
    return ( 1.0f / vnInvSqrt( f ) + 0.5f );
}

inline BOOL vnIsPow2( UINT32 uiValue )
{
    return ( 0 == ( uiValue & ( uiValue - 1 ) ) );
}

inline FLOAT32 vnGreaterMultiple( FLOAT32 fValue, FLOAT32 fMultiple )
{
    FLOAT32 mod = fmod( fValue, fMultiple );
	
	if ( mod != 0.0f )
	{
        fValue += ( fMultiple - mod );
	}
	
	return fValue;
}

inline UINT32 vnGreaterMultiple( UINT32 uiValue, UINT32 uiMultiple )
{
    UINT32 mod = uiValue % uiMultiple;
	
	if ( 0 != mod )
	{
        uiValue += ( uiMultiple - mod );
	}
	
	return uiValue;
}

inline UINT32 vnAlign( UINT32 uiValue, UINT32 uiAlignment )
{
    return vnGreaterMultiple( uiValue, uiAlignment );
}

inline UINT32 vnAlign16( UINT32 uiValue )
{
    return ( uiValue & 0xF ? uiValue + ~( uiValue & 0xF ) + 1 : uiValue );
}

inline UINT32 vnAlign8( UINT32 uiValue )
{
    return ( uiValue & 0x7 ? uiValue + ~( uiValue & 0x7 ) + 1 : uiValue );
}

inline UINT32 vnAlign2( UINT32 uiValue )
{
    if ( vnIsPow2( uiValue ) )
    {
		return uiValue;
    }
	
	INT32 iPower = 0;
	
	while ( uiValue ) 
	{
		uiValue >>= 1;
		
		iPower++;
	}
	
	return 1 << iPower;
}

#endif // __VN_MATH_H__