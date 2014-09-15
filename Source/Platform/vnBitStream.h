
//
// Copyright (c) 2002-2014 Joe Bertolami. All Right Reserved.
//
// vnBitStream.h
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

#ifndef __VN_BIT_STREAM_H__	
#define __VN_BIT_STREAM_H__	

#include "vnBase.h"

//
// Note that the BitStream is sized along byte boundaries, but
// reading and writing of bits is along bit boundaries.
//

class CVBitStream
{
	UINT32		m_uiReadIndex;			// bit index
	UINT32		m_uiWriteIndex;			// bit index
	UINT8 *	    m_DataStore;
    UINT32      m_DataCapacity;

protected:

	//
	// Copy methods return the number of bits written/read.
	//
	// Note that it is the caller's job to ensure the safety of the operation.
	//

	UINT32 AlignedCopy( UINT8 * pbyDest, UINT32 uiDestBitOffset, UINT8 * pbySource, UINT32 uiSourceBitOffset, UINT32 uiBitCountToCopy );
	UINT32 UnalignedCopy( UINT8 * pbyDest, UINT32 uiDestBitOffset, UINT8 * pbySource, UINT32 uiSourceBitOffset, UINT32 uiBitCountToCopy );

public:

    CVBitStream();
    CVBitStream( CONST CVBitStream & rvalue );
	CVBitStream( IN VOID * pbyBytes, UINT32 uiSize );
    virtual ~CVBitStream();

	//
	// Our size interfaces accept and return lengths in bits (not bytes!)
	//

    virtual UINT8 *          QueryData() CONST;
	virtual UINT32           QueryCapacity() CONST;			
    virtual UINT32			 QueryOccupancy() CONST;
    virtual UINT32			 ResizeCapacity( UINT32 uiNewSizeInBits );
 
	//
	// Seek will adjust our read index. There is deliberately no way to 
	// adjust the write index.
	//

	virtual VN_STATUS		 Seek( UINT32 uiBitOffset );
	virtual VN_STATUS		 Assign( CONST CVBitStream & rvalue );
    virtual VN_STATUS		 Assign( IN VOID * pbyBytes, UINT32 uiSize );
    virtual VN_STATUS		 Clear();
    virtual VN_STATUS		 Empty(); 

	virtual BOOL			 IsEmpty() CONST;
    virtual BOOL			 IsFull() CONST; 

    virtual VN_STATUS        WriteByte( UINT8 uiValue );
	virtual VN_STATUS		 WriteBit( UINT8 uiValue );
    virtual VN_STATUS        WriteBytes( VOID * pData, UINT32 uiByteCount );
    virtual VN_STATUS		 WriteBits( VOID * pData, UINT32 uiBitCount );

    virtual VN_STATUS        ReadByte( VOID * pData );
	virtual VN_STATUS		 ReadBit( VOID * pData );
    virtual VN_STATUS        ReadBytes( VOID * pData, INOUT UINT32 * puiByteCount );
    virtual VN_STATUS		 ReadBits( VOID * pData, INOUT UINT32 * puiBitCount );
     
	virtual CVBitStream &	 operator = ( CONST CVBitStream & rvalue );
    virtual BOOL             operator == ( CONST CVBitStream & rvalue ) CONST;
    virtual BOOL             operator != ( CONST CVBitStream & rvalue ) CONST;
};

#endif // __VN_BIT_STREAM_H__
