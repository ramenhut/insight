
#include "vnImagine.h"

//
// (!) Note: See the Imagine framework for a float-free implementation of this filter.
//

UINT8 vnCoverageSampleVertical( CONST CVImage & pSrcImage,FLOAT32 fX, FLOAT32 fY, FLOAT32 fRadius )
{
    if ( VN_PARAM_CHECK )
    {
        if ( 0 == fRadius || !VN_IS_IMAGE_VALID( pSrcImage ) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    FLOAT32 fSampleCount = 0;
    FLOAT32 fMaxDistance = fRadius;    
    INT32 iRadius        = fRadius + 1.0f;
    INT32 iResult        = 0;

    //
    // Scan the kernel space adding up the pixel values
    //

    for ( INT32 j = -iRadius + 1; j <= iRadius; j++ )
    {
        INT32 iX = fX;
        INT32 iY = fY + j;

        if ( iX < 0 || iY < 0 || iX > pSrcImage.QueryWidth() - 1 || iY > pSrcImage.QueryHeight() - 1 )
        {
             continue;
        }

        UINT8 * pSrcPixel   = pSrcImage.QueryData() + pSrcImage.BlockOffset( iX, iY );

        FLOAT32 fYDelta     = fY - iY;
        FLOAT32 fDistance   = fabs( fYDelta );
        FLOAT32 fWeight     = 0.0f;

        //
        // Since we're minifying, we can compute a simple distance based weighted average 
        // using our calculated radius (fRadius)
        //

        fDistance     = VN_MIN2( fMaxDistance, fDistance );
        fWeight       = 1.0f - fDistance / fMaxDistance;
        iResult      += fWeight * pSrcPixel[0];
        fSampleCount += fWeight;
    }

    //
    // Normalize our sum back to the valid pixel range
    //

    return ( iResult / fSampleCount );
}

UINT8 vnCoverageSampleHorizontal( CONST CVImage & pSrcImage, FLOAT32 fX, FLOAT32 fY, FLOAT32 fRadius )
{
    if ( VN_PARAM_CHECK )
    {
        if ( 0 == fRadius || !VN_IS_IMAGE_VALID( pSrcImage ) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    FLOAT32 fSampleCount = 0;
    FLOAT32 fMaxDistance = fRadius;    
    INT32 iRadius        = fRadius + 1.0f;
    INT32 iResult        = 0;
 
    //
    // Scan the kernel space adding up the pixel values
    //

    for ( INT32 i = -iRadius + 1; i <= iRadius; i++ )
    {
        INT32 iX = fX + i;
        INT32 iY = fY;

        if ( iX < 0 || iY < 0 || iX > pSrcImage.QueryWidth() - 1 || iY > pSrcImage.QueryHeight() - 1 )
        {
             continue;
        }

        UINT8 * pSrcPixel   = pSrcImage.QueryData() + pSrcImage.BlockOffset( iX, iY );

        FLOAT32 fXDelta     = fX - iX;
        FLOAT32 fDistance   = fabs( fXDelta );
        FLOAT32 fWeight     = 0.0f;

        //
        // Since we're minifying, we can compute a simple distance based weighted average 
        // using our calculated radius (fRadius)
        //

        fDistance     = VN_MIN2( fMaxDistance, fDistance );
        fWeight       = 1.0f - fDistance / fMaxDistance;
        iResult      += fWeight * pSrcPixel[0];
        fSampleCount += fWeight;
    }

    //
    // Normalize our sum back to the valid pixel range
    //

    return ( iResult / fSampleCount );
}

VN_STATUS vnResizeImageSeparable( CONST CVImage & pSrcImage, FLOAT32 fHRatio, FLOAT32 fVRatio, INOUT CVImage * pDestImage )
{
    //
    // We rely upon coverage filtering because it allows us to perform very large
    // resolution changes without suffering from precision, range, and sampling issues.
    //

    CVImage * tempImage;

    if ( VN_FAILED( vnCreateImage( pSrcImage.QueryFormat(), pDestImage->QueryWidth(), pSrcImage.QueryHeight(), &tempImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Perform the horizontal filter sampling.
    //

    for ( UINT32 j = 0; j < pSrcImage.QueryHeight(); j++ )
    for ( UINT32 i = 0; i < pDestImage->QueryWidth(); i++ )
    {
        UINT8 * pOutputData = tempImage->QueryData() + tempImage->BlockOffset( i, j );

        //
        // Determine the sub-pixel location of our *target* (i,j) coordinate, in the space
        // of our source image.
        //

        pOutputData[0] = vnCoverageSampleHorizontal( pSrcImage, i * fHRatio, j, fHRatio );
    }

    //
    // Perform the vertical filter sampling.
    //

    for ( UINT32 j = 0; j < pDestImage->QueryHeight(); j++ )
    for ( UINT32 i = 0; i < pDestImage->QueryWidth(); i++ )
    {
        UINT8 * pOutputData = pDestImage->QueryData() + pDestImage->BlockOffset( i, j );

        //
        // Determine the sub-pixel location of our *target* (i,j) coordinate, in the space
        // of our temp image.
        //

        pOutputData[0] = vnCoverageSampleVertical( *tempImage, i, j * fVRatio, fVRatio );
    }

    vnDestroyImage( tempImage );

    return VN_SUCCESS;
}

VN_STATUS vnResizeImage( CONST CVImage & pSrcImage, UINT32 uiWidth, UINT32 uiHeight, INOUT CVImage ** pDestImage )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !VN_IS_IMAGE_VALID( pSrcImage ) || 0 == uiWidth || 0 == uiHeight || !pDestImage )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    //
    // Verify whether resampling is actually necessary
    //

    if ( uiWidth == pSrcImage.QueryWidth() && uiHeight == pSrcImage.QueryHeight() )
    {
        return vnCloneImage( pSrcImage, pDestImage );
    }

    //
    // Create our destination image.
    //

    if ( VN_FAILED( vnCreateImage( pSrcImage.QueryFormat(), uiWidth, uiHeight, pDestImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Prepare to perform our resample. This is perhaps the most important part of our resizer -- 
    // the calculation of our image ratios. These ratios are responsible for mapping between our 
    // integer pixel locations of the source image and our float sub-pixel coordinates within the
    // source image that represent a reflection of our destination pixels. 
    //                                           

    FLOAT32 fHorizRatio = ( 1 == uiWidth  ? 1.0f : static_cast<FLOAT32>( pSrcImage.QueryWidth() - 1 )  / ( uiWidth - 1 ) );
    FLOAT32 fVertRatio  = ( 1 == uiHeight ? 1.0f : static_cast<FLOAT32>( pSrcImage.QueryHeight() - 1 ) / ( uiHeight - 1 ) );

    //
    // Our resize filter is separable, so we perform it in the horizontal space first, and 
    // then in the vertical.
    //

    return vnResizeImageSeparable( pSrcImage, fHorizRatio, fVertRatio, *pDestImage );
}
