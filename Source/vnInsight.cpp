
#include "vnInsight.h"

//
// Our hash size determines the degree of quantization of the data.
// Larger hash sizes enable less quantization and a greater per-pixel 
// precision.
//

#define VN_INSIGHT_DEFAULT_HASH_SIZE                (32)

//
// The thumbnail size is less important than the hash size, but can
// provide greater accuracy for very large images. This value should
// rarely be larger than 8 due to the likelihood of overflowing our
// 16 bit signed storage during the transformation step.
//

#define VN_INSIGHT_DEFAULT_THUMB_SIZE               (16)

#define VN_INSIGHT_MAX_THUMB_SIZE                   (2900)

#if ( ( ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) / ( VN_INSIGHT_DEFAULT_THUMB_SIZE * VN_INSIGHT_DEFAULT_THUMB_SIZE ) ) > 32 )
#error "Default hash size is too large. Decrease the hash size or increase the thumb size to remedy."
#endif

INT32 vnComputeBlockAverage( CONST CVImage & pInput )
{
    if ( VN_PARAM_CHECK )
    {
        if ( !VN_IS_IMAGE_VALID( pInput ) || VN_IMAGE_FORMAT_R32S != pInput.QueryFormat() )
        {
            vnPostError( VN_ERROR_INVALIDARG );

            return 0;
        }
    }

    //
    // Traverse the upper left 1/16th block and compute an average, ignoring
    // the DC coefficient.
    //

    INT64 iAverage      = 0;
    UINT32 uiBlockWidth = pInput.QueryWidth() >> 2;

    for ( UINT32 j = 0; j < uiBlockWidth; j++ )
    {
        for ( UINT32 i = 0; i < uiBlockWidth; i++ )
        {
            if ( 0 == i && 0 == j ) continue;

            iAverage += *( reinterpret_cast<INT32 *>( pInput.QueryData() + pInput.BlockOffset( i, j ) ) );
        }
    }

    return ( iAverage / ( ( uiBlockWidth * uiBlockWidth ) - 1 ) );
}

VN_STATUS vnPublishHashValue( CONST CVImage & pInput, INT32 iAverage, UINT32 uiHashSize, CVBitStream * pOutStream )
{
    if ( !pOutStream || pOutStream->IsFull() || 0 == pOutStream->QueryCapacity() || !VN_IS_IMAGE_VALID( pInput ) )
    {
        return vnPostError( VN_ERROR_INVALIDARG );
    }

    //
    // Traverse the upper left 1/16th block and write out a quantized series
    // of bits. We do this carefully considering the range of our DCT values,
    // so that we quantize around the tighest range possible.
    //

    UINT32 uiBlockWidth       = pInput.QueryWidth() >> 2;
    UINT32 uiHashBitsPerPixel = ( uiHashSize << 3 ) / ( uiBlockWidth * uiBlockWidth );
    UINT32 uiMaxDCTValue      = 255 * pInput.QueryWidth() * pInput.QueryHeight();
    UINT32 uiTwiceDCTMax      = uiMaxDCTValue << 1;
    UINT32 uiQdiv             = uiTwiceDCTMax >> ( uiHashBitsPerPixel - 1 );
    
    if ( 0 == uiHashBitsPerPixel )
    {
        // 
        // There aren't enough bits to cover a thumbnail of this size. We need to 
        // either increase the hash size or decrease the thumbnail size until there
        // is at least one hash bit dedicated to each pixel here.
        //

        VN_MSG("Error: invalid hash and thumb size values.");

        return vnPostError( VN_ERROR_INVALID_RESOURCE );
    }

    if ( uiHashBitsPerPixel > 32 )
    {
        // 
        // Our hash size is too large for our thumbnail. Decrease the hash size or 
        // increase the thumbnail size to remedy this. Insight will continue, but 
        // using a 32 bpp hash.
        //

        VN_MSG("Warning: hash size is too large and will result in an unquantized hash.");

        uiHashBitsPerPixel = 32;
    }

    if ( uiHashBitsPerPixel > vnLog2( uiTwiceDCTMax << 1 ) + 1 )
    {
        UINT32 uiSuggestedHashSize = VN_MAX2( 1, ( uiBlockWidth * uiBlockWidth * ( vnLog2( uiTwiceDCTMax << 1 ) + 1 ) ) >> 3 );

        VN_MSG("Warning: requested hash size is much larger than necessary, given thumbnail dimensions");
        VN_MSG("         Suggested hash size for these parameters is %i bytes", uiSuggestedHashSize );
        VN_MSG("");
        VN_MSG("Warning: results will not be accurate given the prevelance of zero padding!");
    }

    if ( 0 == uiQdiv )
    {
        uiQdiv = 1;

        VN_MSG("Warning: performing an unquantized hash (shrink the hash size to correct this).");
    }

    //
    //  Note: Our DCT values will have a range of +- uiMaxDCTValue and we dedicate
    //        uiHashBitsPerPixel bits to each of the ( uiBlockWidth * uiBlockWidth )
    //        pixels in our image subsection.
    //
    //  For each pixel:
    //
    //  1. We subtract our average value from the pixel value. Since our values will 
    //     range from -uiMaxDCTValue to +uiMaxDCTValue, our results will range from
    //     2 * -uiMaxDCTValue to 2 * +uiMaxDCTValue.
    //
    //  2. We shift our value into the positive range by adding 2 * uiMaxDCTValue to
    //     the pixel. Our results now range from 0 to 4 * uiMaxDCTValue.
    //
    //  3. Lastly we derive a quantization divisor ( uiQdiv) that is based on our range of
    //     values and the number of bits we've dedicated to each pixel. This is essentially
    //     defined as ( 4 * uiMaxDCTValue ) / ( 2^uiHashBitsPerPixel ).
    //

    for ( UINT32 j = 0; j < uiBlockWidth; j++ )
    {
        for ( UINT32 i = 0; i < uiBlockWidth; i++ )
        {
            INT32 iValue = *( reinterpret_cast<INT32 *>( pInput.QueryData() + pInput.BlockOffset( i, j ) ) );

                  iValue = iValue - iAverage;          
                  iValue = iValue + uiTwiceDCTMax;          
                  iValue = iValue / uiQdiv;
            
            for ( UINT32 k = 0; k < uiHashBitsPerPixel; k++ )
            {
                pOutStream->WriteBit( iValue & 0x1 );

                iValue = iValue >> 0x1;
            }
        }
    }

    return VN_SUCCESS;
}

VN_STATUS vnHashImage( CONST CVImage & pInput, UINT32 uiThumbSize, UINT32 uiHashSize, CVBitStream * pOutStream )
{
    if ( VN_PARAM_CHECK )
    {
        //
        // We arbitrarily limit the thumb and hash sizes to 64K and 2G respectively.
        //

        if ( uiThumbSize > VN_INSIGHT_MAX_THUMB_SIZE || uiHashSize > 2*GB )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }

        if ( !pOutStream || pOutStream->IsFull() || 0 == pOutStream->QueryCapacity() || !VN_IS_IMAGE_VALID( pInput ) )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }

        if ( VN_IMAGE_FORMAT_R8G8B8 != pInput.QueryFormat() )
        {
            VN_MSG("Insight currently only supports R8G8B8 images.");

            return vnPostError( VN_ERROR_INVALIDARG );
        }

        if ( pInput.QueryWidth() < 32 || pInput.QueryHeight() < 32 )
        {
            //
            // For now we only support images >= 32x32 pixels
            //

            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    CVImage * pGrayImage      = NULL;
    CVImage * pSmallImage     = NULL;
    CVImage * pTransformImage = NULL;

    //
    // Maintain our default values.
    //

    if ( 0 == uiHashSize )  uiHashSize  = VN_INSIGHT_DEFAULT_HASH_SIZE;
    if ( 0 == uiThumbSize ) uiThumbSize = VN_INSIGHT_DEFAULT_THUMB_SIZE;

    //
    // Our hash size represents the number of bits we wish to use for our output hash. 
    //

     INT32 iAverageValue  = 0;
    UINT32 uiTargetWidth  = uiThumbSize << 2;

    //
    // First we convert our image to grayscale.
    //

    if ( VN_FAILED( vnDesaturateImage( pInput, &pGrayImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Reduce our image down to (uiTargetWidth x uiTargetWidth)
    //

    if ( VN_FAILED( vnResizeImage( *pGrayImage, uiTargetWidth, uiTargetWidth, &pSmallImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Transform into frequency space, converting to 16 bpp.
    //

    if ( VN_FAILED( vnTransformImage( *pSmallImage, &pTransformImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Compute the average of the upper left (uiLeftQuadSize x uiLeftQuadSize) region
    // of our transformed image, ignoring the DC coefficient.
    //

    iAverageValue = vnComputeBlockAverage( *pTransformImage );        

    //
    // Traverse our upper-left block and write out an output bits depending upon the 
    // results of our quantization function.
    //

    if ( VN_FAILED( vnPublishHashValue( *pTransformImage, iAverageValue, uiHashSize, pOutStream ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    //
    // Cleanup
    //

    vnDestroyImage( pGrayImage );
    vnDestroyImage( pSmallImage );
    vnDestroyImage( pTransformImage );

    return VN_SUCCESS;
}

UINT64 vnHashImage64( CONST CVImage & pInput )
{
    UINT64 result = 0;
    UINT32 count  = 8;
    CVBitStream stream;

    if ( 64 != stream.ResizeCapacity( 64 ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    if ( VN_FAILED( vnHashImage( pInput, 8, 8, &stream ) ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    if ( VN_FAILED( stream.ReadBytes( &result, &count ) ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    return result;
}

FLOAT32 vnCompareImages64( CONST CVImage & pA, CONST CVImage & pB )
{
    UINT64 uiHashA = vnHashImage64( pA );
    UINT64 uiHashB = vnHashImage64( pB );

    //
    // Compute the Hamming distance of our two hashes and return 
    // the count as a representative of the degree of similarity.
    //

    UINT32  uiMatchCount  = 0;
    UINT64 uiCompareValue = uiHashA ^ uiHashB;

    while ( uiCompareValue )
    {
        uiCompareValue &= ( uiCompareValue - 1 );

        uiMatchCount++;
    }

    uiMatchCount = ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) - uiMatchCount;

    return ( uiMatchCount / ( (FLOAT32) ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) ) );
}

FLOAT32 vnCompareImages( CONST CVImage & pA, CONST CVImage & pB )
{
    CVBitStream pAStream, pBStream;

    if ( ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) != pAStream.ResizeCapacity( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    if ( ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) != pBStream.ResizeCapacity( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    if ( VN_FAILED( vnHashImage( pA, 0, 0, &pAStream ) ) || VN_FAILED( vnHashImage( pB, 0, 0, &pBStream ) ) )
    {
        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    if ( pAStream.QueryOccupancy() != pBStream.QueryOccupancy() )
    {
        //
        // Our hashing algorithm guarantees a fixed sized output. If our bitstreams
        // do not have the same occupancy, then something has gone wrong.
        //

        vnPostError( VN_ERROR_EXECUTION_FAILURE );

        return 0;
    }

    //
    // Compute the Hamming distance of our two hashes and return the count as a 
    // representative of the degree of similarity.
    //

    UINT32  uiMatchCount = 0;

    while ( !pAStream.IsEmpty() && !pBStream.IsEmpty() )
    {
        //
        // Read a bit, compare it, continue.
        //

        UINT8 uiA = 0, uiB = 0;

        pAStream.ReadBit( &uiA );
        pBStream.ReadBit( &uiB );

        uiMatchCount += ( uiA == uiB );
    }

    return ( uiMatchCount / ( (FLOAT32) ( VN_INSIGHT_DEFAULT_HASH_SIZE << 3 ) ) );
}