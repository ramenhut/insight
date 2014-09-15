
#include "vnImagine.h"

VN_STATUS vnDesaturateLine( IN UINT8 * pInput, UINT32 uiPixelCount, UINT8 * pOutput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !pInput || 0 == uiPixelCount || !pOutput )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    //
    // We generate a grayscale image through desaturation. This method is preferred because
    // it favors the color channels in a human perceptual manner.
    //

    for ( UINT32 iX = 0; iX < uiPixelCount; iX++ )
    {
        UINT8 * pSrcPixel   = pInput + iX * 3;
        UINT8 * pDestPixel  = pOutput + iX;

        INT16 iSrcRed   = pSrcPixel[0];
        INT16 iSrcGreen = pSrcPixel[1];
        INT16 iSrcBlue  = pSrcPixel[2];

        pDestPixel[0] = ( ( iSrcRed * 0x4CC ) + ( iSrcGreen * 0x970 ) + ( iSrcBlue * 0x1C2 ) ) >> 12; 
    }

    return VN_SUCCESS;
}

VN_STATUS vnDesaturateImage( CONST CVImage & pSrcImage, INOUT CVImage ** pDestImage )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !VN_IS_IMAGE_VALID( pSrcImage ) || !pDestImage )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    //
    // Create our destination image as a single channel 8 bit format.
    //

    if ( VN_FAILED( vnCreateImage( VN_IMAGE_FORMAT_R8, pSrcImage.QueryWidth(), pSrcImage.QueryHeight(), pDestImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Traverse our image, unpacking each pixel block, performing the conversion, and then
    // packing the result into our destination image.
    //

    for ( UINT32 iY = 0; iY < pSrcImage.QueryHeight(); iY++ )
    {
        UINT8 * pSrcLine  = pSrcImage.QueryData() + iY * pSrcImage.RowPitch();
        UINT8 * pDestLine = (*pDestImage)->QueryData() + iY * (*pDestImage)->RowPitch();

        if ( VN_FAILED( vnDesaturateLine( pSrcLine, pSrcImage.QueryWidth(), pDestLine ) ) )
        {
            return vnPostError( VN_ERROR_EXECUTION_FAILURE );
        }
    }

    return VN_SUCCESS;
}
