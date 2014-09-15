
#include "vnImagine.h"

//
// (!) Note: this transform is very slow but flexible. For optimized versions consult the full Imagine Framework (v1.05+)
// 

VN_STATUS vnTransformLine( IN UINT8 * pInput, UINT32 uiSrcStride, UINT32 uiCount, INT32 * pOutput, UINT32 uiDestStride )
{
    if ( VN_PARAM_CHECK )
	{
		if ( !pInput || 0 == uiSrcStride || !pOutput || 0 == uiDestStride )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

    for ( UINT8 i = 0; i < uiCount; i++ )
	{
		INT32 iiTotal	    = 0;             
		INT32 * pOutputCell = pOutput + i * uiDestStride;

		for ( UINT8 k = 0; k < uiCount; k++ )
		{
			UINT8 * pInputCell = pInput + k * uiSrcStride;

            iiTotal += (*pInputCell) * cos( ( ( 2 * k + 1 ) * i * VN_PI ) / ( 2 * uiCount ) );
		}

		if ( 0 == i ) iiTotal = iiTotal * vnSqrt( 1.0f / uiCount );
		else		  iiTotal = iiTotal * vnSqrt( 2.0f / uiCount ); 

		(*pOutputCell) = iiTotal;
	}

	return VN_SUCCESS;
}

VN_STATUS vnTransformLine( IN INT32 * pInput, UINT32 uiSrcStride, UINT32 uiCount, INT32 * pOutput, UINT32 uiDestStride )
{
    if ( VN_PARAM_CHECK )
	{
		if ( !pInput || 0 == uiSrcStride || !pOutput || 0 == uiDestStride )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

    for ( UINT8 i = 0; i < uiCount; i++ )
	{
		INT32 iiTotal	    = 0;             
		INT32 * pOutputCell = pOutput + i * uiDestStride;

		for ( UINT8 k = 0; k < uiCount; k++ )
		{
			INT32 * pInputCell = pInput + k * uiSrcStride;

            iiTotal += (*pInputCell) * cos( ( ( 2 * k + 1 ) * i * VN_PI ) / ( 2 * uiCount ) );
		}

		if ( 0 == i ) iiTotal = iiTotal * vnSqrt( 1.0f / uiCount );
		else		  iiTotal = iiTotal * vnSqrt( 2.0f / uiCount ); 

		(*pOutputCell) = iiTotal;
	}

	return VN_SUCCESS;
}

VN_STATUS vnTransformImage( CONST CVImage & pSrcImage, OUT CVImage ** pOutput )
{
    if ( VN_PARAM_CHECK )
	{
		if ( !VN_IS_IMAGE_VALID( pSrcImage ) || !pOutput )
		{
			return vnPostError( VN_ERROR_INVALIDARG );
		}
	}

    INT32 * pScratchBlock = new INT32[ pSrcImage.QueryWidth() * pSrcImage.QueryHeight() ];

    if ( !pScratchBlock )
    {
        return vnPostError( VN_ERROR_OUTOFMEMORY );
    }

    //
    // Create our destination image as a single channel 16 bit format.
    //

    if ( VN_FAILED( vnCreateImage( VN_IMAGE_FORMAT_R32S, pSrcImage.QueryWidth(), pSrcImage.QueryHeight(), pOutput ) ) )
    {
        delete [] pScratchBlock;

        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
	// Horizontal DCT-II
	// 

	for ( UINT32 j = 0; j < pSrcImage.QueryHeight(); j++ )
	{
        UINT8 * pSrcLine  = pSrcImage.QueryData() + pSrcImage.BlockOffset( 0, j );
        INT32 * pDestLine = pScratchBlock + j * pSrcImage.QueryWidth();

		if ( VN_FAILED( vnTransformLine( pSrcLine, 1, pSrcImage.QueryWidth(), pDestLine, 1 ) ) )
		{
            delete [] pScratchBlock;

            vnDestroyImage( *pOutput );

			return vnPostError( VN_ERROR_EXECUTION_FAILURE );
		}
	}

	//
	// Vertical DCT-II
	//

	for ( UINT32 i = 0; i < pSrcImage.QueryWidth(); i++ )
	{
        INT32 * pSrcLine  = pScratchBlock + i;
        INT32 * pDestLine = reinterpret_cast<INT32 *>( (*pOutput)->QueryData() + (*pOutput)->BlockOffset( i, 0 ) );
        
        if ( VN_FAILED( vnTransformLine( pSrcLine, pSrcImage.QueryWidth(), pSrcImage.QueryHeight(), pDestLine, (*pOutput)->QueryWidth() ) ) )
		{
            delete [] pScratchBlock;

            vnDestroyImage( *pOutput );

			return vnPostError( VN_ERROR_EXECUTION_FAILURE );
		}
	}

    //
    // Cleanup
    //

    delete [] pScratchBlock;

	return VN_SUCCESS;
}

