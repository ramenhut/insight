
#include "vnBitStream.h"
#include "vnMath.h"

CVBitStream::CVBitStream()
{
	m_uiReadIndex  = 0;
	m_uiWriteIndex = 0;
    m_DataStore    = 0;
    m_DataCapacity = 0;
}

CVBitStream::CVBitStream( CONST CVBitStream & rvalue )
{
	if ( VN_FAILED( Assign( rvalue ) ) )
	{
		vnPostError( VN_ERROR_EXECUTION_FAILURE );
	}
}

CVBitStream::CVBitStream( IN VOID * pbyBytes, UINT32 uiSize )
{
	if ( VN_FAILED( Assign( pbyBytes, uiSize ) ) )
	{
		vnPostError( VN_ERROR_EXECUTION_FAILURE );
	}
}

CVBitStream::~CVBitStream()
{
    if ( VN_FAILED( Clear() ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }
}

UINT8 * CVBitStream::QueryData() CONST
{
    return m_DataStore;
}

UINT32 CVBitStream::QueryCapacity() CONST
{
	return m_DataCapacity << 3;
}

UINT32 CVBitStream::QueryOccupancy() CONST
{
	return m_uiWriteIndex - m_uiReadIndex;
}

UINT32 CVBitStream::ResizeCapacity( UINT32 uiNewSizeInBits )
{
	if ( VN_PARAM_CHECK )
	{
		if ( 0 == uiNewSizeInBits )
		{
			VN_ERR("Invalid bitstream capacity requested (%i).", uiNewSizeInBits );

			vnPostError( VN_ERROR_INVALIDARG );

			return 0;
		}
	}

    if ( VN_FAILED( Clear() ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

	UINT32 uiByteSize = vnAlign( uiNewSizeInBits, 8 ) >> 3;

    m_DataStore       = new UINT8[ uiByteSize ];

    if ( !m_DataStore )
    {
        VN_ERR("Failed to allocate %i bytes for a bitstream.", uiByteSize );

		vnPostError( VN_ERROR_EXECUTION_FAILURE );

		return 0;
    }

    m_DataCapacity = uiByteSize;

	return uiNewSizeInBits;
}
 
VN_STATUS CVBitStream::Assign( CONST CVBitStream & rvalue )
{
    //
	// We copy the data into our own buffer and adjust 
	// our read/write pointers appropriately.
	//

    if ( VN_FAILED( Assign( rvalue.m_DataStore, rvalue.m_DataCapacity ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

	m_uiReadIndex  = rvalue.m_uiReadIndex;
	m_uiWriteIndex = rvalue.m_uiWriteIndex;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::Assign( IN VOID * pbyBytes, UINT32 uiSize )
{
    if ( VN_PARAM_CHECK )
    {
        if ( 0 == uiSize || !pbyBytes )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    if ( VN_FAILED( Clear() ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

	//
	// We copy the data into our own buffer and adjust 
	// our read/write pointers appropriately.
	//

    m_DataStore = new UINT8[ uiSize ];

    if ( !m_DataStore )
    {
        return vnPostError( VN_ERROR_OUTOFMEMORY );
    }

    vnCopyMemory( m_DataStore, pbyBytes, uiSize );

	m_uiReadIndex  = 0;
	m_uiWriteIndex = uiSize << 3;
    m_DataCapacity = uiSize;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::Seek( UINT32 uiBitOffset )
{
	if ( uiBitOffset >= m_uiWriteIndex )
	{
		uiBitOffset = m_uiWriteIndex;
	}
	
	m_uiReadIndex = uiBitOffset;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::Clear()
{
	Empty();

    delete [] m_DataStore;

    m_DataStore    = 0;
    m_DataCapacity = 0;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::Empty()
{
	m_uiWriteIndex = 0;
	m_uiReadIndex  = 0;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::WriteBit( UINT8 uiValue )
{
	if ( m_uiWriteIndex + 1 > QueryCapacity() )
	{
		VN_WRN("Bitstream write capacity reached.");

		return VN_ERROR_CAPACITY_LIMIT;
	}

	//
	// Determine the current byte to write to
	//

	UINT32 uiTargetByte = m_uiWriteIndex >> 3;
	UINT8  uiTargetBit  = m_uiWriteIndex % 8;

	//
	// Pull the correct byte from our data store, update it, and then store it.
	// Note that we do not guarantee that unused buffer memory was zero filled,
	// thus we safely clear the write bit.
	//

	UINT8 * uiData = &( m_DataStore[ uiTargetByte ] );

	(*uiData) = ( (*uiData) & ~( 0x1 << uiTargetBit ) ) | ( uiValue & 0x1 ) << uiTargetBit;

	m_uiWriteIndex++;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::WriteByte( UINT8 uiValue )
{
    if ( m_uiWriteIndex + 8 > QueryCapacity() )
	{
		VN_WRN("Bitstream write capacity reached.");

		return VN_ERROR_CAPACITY_LIMIT;
	}

    //
	// Determine the current byte to write to
	//

	UINT32 uiTargetByte = m_uiWriteIndex >> 3;
	UINT8  uiTargetBit  = m_uiWriteIndex % 8;

    if ( 0 == uiTargetBit )
    {
        //
        // This is a byte aligned write, so we perform it at byte level.
        //

        UINT8 * uiData = &( m_DataStore[ uiTargetByte ] );

	    (*uiData) = uiValue;

        m_uiWriteIndex += 8;
    }
    else
    {
        //
        // Slower byte unaligned write.
        //

        for ( UINT8 i = 0; i < 8; i++ )
        {
            WriteBit( ( uiValue >> i ) & 0x1 );
        }
    }

    return VN_SUCCESS;
}

VN_STATUS CVBitStream::ReadBit( VOID * pData )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pData )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	if ( m_uiReadIndex >= m_uiWriteIndex )
	{
		VN_WRN("Requested read operation on an empty bitstream.");

		return vnPostError( VN_ERROR_INVALID_RESOURCE );
	}

	//
	// Determine the current byte to read from
	//

	UINT32 uiTargetByte = m_uiReadIndex >> 3;
	UINT8  uiTargetBit  = m_uiReadIndex % 8;

	//
	// Pull the correct byte from our data store. Note that we 
	// preserve the high bits of pbyData.
	//

	(*pbyData) &= 0xFE;
	(*pbyData) |= ( ( m_DataStore[ uiTargetByte ] ) >> uiTargetBit ) & 0x1;

	m_uiReadIndex++;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::ReadByte( VOID * pData )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pbyData )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	if ( m_uiReadIndex + 8 > m_uiWriteIndex )
	{
		VN_WRN("Insufficient data to complete read request.");

		return VN_ERROR_INVALID_RESOURCE;
	}

	//
	// Determine the current byte to read from
	//

	UINT32 uiTargetByte = m_uiReadIndex >> 3;
	UINT8  uiTargetBit  = m_uiReadIndex % 8;

    if ( 0 == ( uiTargetBit % 8 ) )
    {
        (*pbyData) = m_DataStore[ uiTargetByte ];

        m_uiReadIndex += 8;
    }
    else
    {
        //
        // Slower byte unaligned read.
        //

        UINT8 uiTemp = 0;

        for ( UINT8 i = 0; i < 8; i++ )
        {
            uiTemp <<= 0x1;

            ReadBit( &uiTemp );
        }

        (*pbyData) = uiTemp;
    }

	return VN_SUCCESS;
}

UINT32 CVBitStream::AlignedCopy( UINT8 * pbyDest, UINT32 uiDestBitOffset, UINT8 * pbySource, UINT32 uiSourceBitOffset, UINT32 uiBitCountToCopy )
{
	if ( VN_PARAM_CHECK )
	{
		if ( !pbyDest || 0 != ( uiDestBitOffset % 8 ) || !pbySource || 0 != ( uiSourceBitOffset % 8 ) || 0 == ( uiBitCountToCopy >> 3 ) )
		{
			vnPostError( VN_ERROR_INVALIDARG );

			return 0;
		}
	}

	UINT32 uiDestByteOffset		= uiDestBitOffset >> 3;
	UINT32 uiSourceByteOffset	= uiSourceBitOffset >> 3;
	UINT32 uiBytesCopied		= ( uiBitCountToCopy >> 3 );

	vnCopyMemory( pbyDest + uiDestByteOffset, pbySource + uiSourceByteOffset, uiBytesCopied );

	return ( uiBytesCopied << 3 );
}

UINT32 CVBitStream::UnalignedCopy( UINT8 * pbyDest, UINT32 uiDestOffset, UINT8 * pbySource, UINT32 uiSourceOffset, UINT32 uiBitCountToCopy )
{
	if ( VN_PARAM_CHECK )
	{
		if ( !pbyDest || 0 == uiBitCountToCopy || !pbySource )
		{
			vnPostError( VN_ERROR_INVALIDARG );

			return 0;
		}
	}

	UINT32 uiSourceCopyLimit = uiSourceOffset + uiBitCountToCopy;

	//
	// Perform an unaligned copy of our data.
	//

	while ( uiSourceOffset < uiSourceCopyLimit )
	{
		UINT32 uiTargetByte = uiDestOffset >> 3;
		UINT8  uiTargetBit  = uiDestOffset % 8;
		UINT32 uiSourceByte = uiSourceOffset >> 3;
		UINT8  uiSourceBit  = uiSourceOffset % 8;

		UINT32 uiBitsLeft  = uiSourceCopyLimit - uiSourceOffset;

		//
		// We traverse our buffer and perform copies in as large of increments as possible.
		//

		UINT8 uiWriteCapacity   = VN_MIN2( 8 - uiTargetBit, 8 - uiSourceBit );
		UINT8 uiWriteCount	    = VN_MIN2( uiWriteCapacity, uiBitsLeft );
		UINT8 uiWriteFillMask	= ( 0x1 << uiWriteCount ) - 1;

		UINT8 * pTargetData		= &( pbyDest[ uiTargetByte ]);
		UINT8 * pSourceData		= &( pbySource[ uiSourceByte ] );

		(*pTargetData)			= ( (*pTargetData) & ~( uiWriteFillMask << uiTargetBit ) );
		(*pTargetData)		   |= ( ( (*pSourceData) & ( uiWriteFillMask << uiSourceBit ) ) >> uiSourceBit ) << uiTargetBit;

		uiSourceOffset		   += uiWriteCount;
		uiDestOffset		   += uiWriteCount;
	}

	return uiBitCountToCopy;
}

VN_STATUS CVBitStream::WriteBits( VOID * pData, UINT32 uiBitCount )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pbyData || 0 == uiBitCount )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	if ( m_uiWriteIndex + uiBitCount > QueryCapacity() )
	{
		VN_WRN("Bitstream write capacity reached.");

		return VN_ERROR_CAPACITY_LIMIT;
	}

	UINT32 uiBitsCopied = 0;

	if ( 0 == ( m_uiWriteIndex % 8 ) && ( uiBitCount >= 8 ) )
	{
		//
		// We can perform a (partial) fast copy because our source and destination 
		// are byte aligned. Note that this may leave trailing bits that still need 
		// to be copied below.
		//

		uiBitsCopied = AlignedCopy( m_DataStore, m_uiWriteIndex, pbyData, 0, uiBitCount );

		if ( !uiBitsCopied )
		{
			return vnPostError( VN_ERROR_EXECUTION_FAILURE );
		}
	}

	//
	// Perform unaligned copies of our data
	//

	if ( uiBitsCopied < uiBitCount )
	{
		uiBitsCopied += UnalignedCopy( m_DataStore, m_uiWriteIndex + uiBitsCopied, pbyData, uiBitsCopied, uiBitCount - uiBitsCopied );
	}

	m_uiWriteIndex += uiBitsCopied;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::WriteBytes( VOID * pData, UINT32 uiByteCount )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pbyData || 0 == uiByteCount )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	if ( m_uiWriteIndex + ( uiByteCount << 3 ) > QueryCapacity() )
	{
		VN_WRN("Bitstream write capacity reached.");

		return VN_ERROR_CAPACITY_LIMIT;
	}

	if ( 0 == ( m_uiWriteIndex % 8 ) )
	{
		//
		// We can perform a fast copy because our source is byte aligned. 
		//

        UINT8 * pDest = m_DataStore + ( m_uiWriteIndex >> 3 );

		vnCopyMemory( pDest, pbyData, uiByteCount );

        m_uiWriteIndex += uiByteCount << 3;
	}
    else
    {
        WriteBits( pbyData, uiByteCount << 3 );
    }

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::ReadBits( VOID * pData, INOUT UINT32 * puiBitCount )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pbyData || !puiBitCount || 0 == (*puiBitCount) )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	//
	// We attempt to read *puiBitCount bits and replace it with the number
	// actually read.
	//

	if ( m_uiReadIndex + *puiBitCount > m_uiWriteIndex )
	{
		(*puiBitCount) = m_uiWriteIndex - m_uiReadIndex;
	}

	UINT32 uiBitsCopied = 0;

	if ( 0 == ( m_uiReadIndex % 8 ) && ( (*puiBitCount) >= 8 ) )
	{
		//
		// We can perform a (partial) fast copy because our source and destination 
		// are byte aligned. Note that this may leave trailing bits that still need 
		// to be copied below.
		//

		uiBitsCopied = AlignedCopy( pbyData, 0, m_DataStore, m_uiReadIndex, (*puiBitCount) );

		if ( !uiBitsCopied )
		{
			return vnPostError( VN_ERROR_EXECUTION_FAILURE );
		}
	}

	//
	// Perform unaligned copies of our data
	//

	if ( uiBitsCopied < (*puiBitCount) )
	{
		uiBitsCopied += UnalignedCopy( pbyData, uiBitsCopied, m_DataStore, m_uiReadIndex + uiBitsCopied, (*puiBitCount) - uiBitsCopied );
	}

	m_uiReadIndex += uiBitsCopied;

	return VN_SUCCESS;
}

VN_STATUS CVBitStream::ReadBytes( VOID * pData, INOUT UINT32 * puiByteCount )
{
    UINT8 * pbyData = reinterpret_cast<UINT8 *>( pData );

	if ( VN_PARAM_CHECK )
	{
		if ( !pbyData || !puiByteCount || 0 == (*puiByteCount) )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

	//
	// We attempt to read *puiBitCount bits and replace it with the number
	// actually read.
	//

	if ( m_uiReadIndex + ((*puiByteCount) << 3 ) > m_uiWriteIndex )
	{
		(*puiByteCount) = ( m_uiWriteIndex - m_uiReadIndex ) >> 3;

        if ( !(*puiByteCount) )
        {
            return VN_ERROR_CAPACITY_LIMIT;
        }
	}
	
	if ( 0 == ( m_uiReadIndex % 8 ) )
	{
		//
		// We can perform a fast copy because our source is byte aligned. 
		//

        UINT8 * pSrc = m_DataStore + ( m_uiReadIndex >> 3 );

		vnCopyMemory( pbyData, pSrc, *puiByteCount );

        m_uiReadIndex += (*puiByteCount) << 3;
	}
    else
    {
        //
        // We don't bother checking uiBitCount after the read because we've
        // already checked the capacity limits.
        //

        UINT32 uiBitCount = (*puiByteCount) << 3;

        ReadBits( pbyData, &uiBitCount );
    }

	return VN_SUCCESS;
}

BOOL CVBitStream::IsEmpty() CONST
{
	return ( m_uiWriteIndex == m_uiReadIndex );
}

BOOL CVBitStream::IsFull() CONST
{
	return ( m_uiWriteIndex == QueryCapacity() );
}

CVBitStream & CVBitStream::operator = ( CONST CVBitStream & rvalue )
{
	if ( VN_FAILED( Assign( rvalue ) ) )
	{
		vnPostError( VN_ERROR_EXECUTION_FAILURE );
	}

	return (*this);
}

BOOL CVBitStream::operator == ( CONST CVBitStream & rvalue ) CONST
{
    //
    // Two bitstreams are only identical if their entire (deep) state is
    // the same.
    //

	if ( m_uiWriteIndex != rvalue.m_uiWriteIndex ) return FALSE;
	if ( m_uiReadIndex  != rvalue.m_uiReadIndex )  return FALSE;
	if ( m_DataCapacity != rvalue.m_DataCapacity ) return FALSE;

    for ( UINT32 i = 0; i < m_DataCapacity; i++ )
    {
        if ( m_DataStore[ i ] != rvalue.m_DataStore[ i ] )
        {
            return FALSE;
        }
    }

	return TRUE;
}

BOOL CVBitStream::operator != ( CONST CVBitStream & rvalue ) CONST
{
	return !( (*this) == rvalue );
}