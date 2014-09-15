
//
// Copyright (c) 2009-2014 Joe Bertolami. All Right Reserved.
//
// vnImagine.h
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
//   Imagine ("Image Engine") is a 256 bit image processing library that provides
//   optimized routines for a very large assortment of operations. 
//
//   Although Insight leverages Imagine, it is not fully included in this source 
//   release. Instead, this release contains a compact version that provides only 
//   the minimal functionality required for Insight. To download the full version
//   of Imagine (which is not required for Insight), visit the website below.  
//
//  Additional Information:
//
//   For more information, visit http://www.bertolami.com.
//

#ifndef __VN_IMAGE_H__
#define __VN_IMAGE_H__

#include "../Platform/vnBase.h"
#include "../Platform/vnMath.h"

#define VN_IMAGE_FORMAT                     UINT32
#define VN_IMAGE_FORMAT_NONE                (0x00000000)
#define VN_IMAGE_FORMAT_R8                  (0x00200000)
#define VN_IMAGE_FORMAT_R8G8B8              (0x00208200)
#define VN_IMAGE_FORMAT_R32S                (0x10800000)

#define VN_IMAGE_MAX_CHANNEL_COUNT          (4)
#define VN_IMAGE_CHANNEL_MASK               (0x3F)
#define VN_IMAGE_CHANNEL_0_SHIFT            (0x12)
#define VN_IMAGE_CHANNEL_1_SHIFT            (0x0C)
#define VN_IMAGE_CHANNEL_2_SHIFT            (0x06)
#define VN_IMAGE_CHANNEL_3_SHIFT            (0x00)

#define VN_IMAGE_CHANNEL_COUNT( x )         ( !!( ( (x) >> VN_IMAGE_CHANNEL_0_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              !!( ( (x) >> VN_IMAGE_CHANNEL_1_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              !!( ( (x) >> VN_IMAGE_CHANNEL_2_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              !!( ( (x) >> VN_IMAGE_CHANNEL_3_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) )

#define VN_IMAGE_PIXEL_RATE( x )            ( ( ( (x) >> VN_IMAGE_CHANNEL_0_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              ( ( (x) >> VN_IMAGE_CHANNEL_1_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              ( ( (x) >> VN_IMAGE_CHANNEL_2_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) + \
                                              ( ( (x) >> VN_IMAGE_CHANNEL_3_SHIFT ) & VN_IMAGE_CHANNEL_MASK ) )


#define VN_IS_IMAGE_VALID(x)                ( (x).QueryFormat()         != VN_IMAGE_FORMAT_NONE &&  \
                                              (x).QueryBitsPerPixel()   != 0 &&                     \
                                              (x).QueryWidth()          != 0 &&                     \
                                              (x).QueryHeight()         != 0 )

class VN_NONVIRTUAL CVImage
{
    friend VN_STATUS vnCreateImage( VN_IMAGE_FORMAT format, UINT32 uiWidthInBlocks, UINT32 uiHeightInBlocks, CVImage ** pOutImage );

    friend VN_STATUS vnDestroyImage( CVImage * pInImage );

private:

    VN_IMAGE_FORMAT             m_uiImageFormat;

    //
    // As a result of the diversity in supported image formats, we treat our 
    // data set as a collection of blocks, which may either contain one or more 
    // channels of color, or otherwise encoded information.
    //
    // When speaking logically about the image however, we operate in terms of pixels.
    //

    UINT32                      m_uiWidthInPixels;
    UINT32                      m_uiHeightInPixels;
    UINT32                      m_uiBitsPerPixel;
    UINT8                       m_uiChannelCount;
    UINT8 *                     m_pbyDataBuffer;
    UINT32                      m_uiDataCapacity;

private:

    //
    // Allocation management
    //

    VN_STATUS                   Allocate( UINT32 uiSize );
    VN_STATUS                   Deallocate();

    //
    // A CVImage is considered uninitialized if its m_uiRTTI field is set to zero
    // (no format). Images must have an empty data buffer in this scenario.
    //

    VN_STATUS                   SetFormat( VN_IMAGE_FORMAT format );

    //
    // SetDimension will automatically manage the memory of the object. This is the 
    // primary interface that should be used for reserving memory for the image. Note
    // that the image must contain a valid format prior to calling SetDimension.
    //
   
    VN_STATUS                   SetDimension( UINT32 uiNewWidth, UINT32 uiNewHeight );

private:

    CVImage();				
    virtual ~CVImage();		

public:

    //
    // Query interfaces
    //

    UINT32                      QueryWidth() CONST;            // the width of the image in pixels
    UINT32                      QueryHeight() CONST;           // the height of the image in pixels    
    UINT8 *                     QueryData() CONST;             // base pointer of the image data   
    UINT8                       QueryBitsPerPixel() CONST;     // channel-specific block size    
    UINT8                       QueryChannelCount() CONST;     // number of valid channels 
    VN_IMAGE_FORMAT             QueryFormat() CONST;

public:

    //
    // Row Pitch
    //
    // RowPitch is the byte delta between two adjacent rows of pixels in the image.
    // This function takes alignment into consideration and may provide a value that
    // is greater than the byte width of the visible image. 
    //

    UINT32                      RowPitch() CONST;
    
    //
    // Slice Pitch
    //
    // SlicePitch is the byte size of the entire image. This size may extend beyond the
    // edge of the last row and column of the image, due to alignment and tiling 
    // requirements on certain platforms.
    //

    UINT32                      SlicePitch() CONST;

    //
    // Block Offset
    //
    // Block offset returns the byte offset from the start of the image to pixel (i,j).
    // Formats are required to use byte aligned pixel rates, so this function will always
    // point to the start of a pixel block.
    //

    UINT32                      BlockOffset( UINT32 i, UINT32 j ) CONST;
};

//
// CVImage Creator
//
// This is the sole interface for creating new CVImage objects. We require the use of this function
// to guarantee that image creation is high-level atomic (soft RAII). Thus, the consumer is responsible
// for allocation of the out image pointer (and the interface will manage the internal memory).
//

VN_STATUS vnCreateImage( VN_IMAGE_FORMAT format, UINT32 uiWidth, UINT32 uiHeight, OUT CVImage ** pOutImage );

//
// CVImage Destructor
//
// CVImage objects will destroy themselves automatically, but we encourage the use of this discreet
// 'destructor' to allow for future internal memory management.
//

VN_STATUS vnDestroyImage( INOUT CVImage * pInImage );


//
// CloneImage Operator
//
//   CloneImage is a simple shorthand for creating an identical copy of an image.
//
// Parameters:
// 
//   pSrcImage:  The read-only source image to copy.
//
//   pDestImage: an uninitialized pointer to an image object. Upon successful
//               return, this object will be fully initialized and contain a 
//               clone of pSrcImage.
//
// Supported Image Formats: 
//    
//   All image formats are supported. 
//

inline VN_STATUS vnCloneImage( CONST CVImage & pSrcImage, OUT CVImage ** RESTRICT pDestImage )
{
    if ( VN_PARAM_CHECK )
    {
        if ( VN_IMAGE_FORMAT_NONE == pSrcImage.QueryFormat() || !pDestImage )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }

        if ( &pSrcImage == *pDestImage )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    if ( VN_FAILED( vnCreateImage( pSrcImage.QueryFormat(), pSrcImage.QueryWidth(), pSrcImage.QueryHeight(), pDestImage ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    vnCopyMemory( (*pDestImage)->QueryData(), pSrcImage.QueryData(), pSrcImage.SlicePitch() );

    return VN_SUCCESS;
}

//
// ResizeImage Operator
//
//   ResizeImage resamples the image to the specified dimensions using a simple coverage kernel.
//
// Parameters:
// 
//   pSrcImage:  The read-only source image to resize.
//
//   uiWidth:    The destination width to target.
//
//   uiHeight:   The destination height to target.
//
//   pDestImage: a pointer to an image object. Upon successful return, this object will
//               contain a resized view of the source image.
//

VN_STATUS vnResizeImage( CONST CVImage & pSrcImage, UINT32 uiWidth, UINT32 uiHeight, CVImage ** pDestImage );

//
// DesaturateImage Operator
//
//   DesaturateImage performs a simple desaturation of the entire image.
//
// Parameters:
// 
//   pSrcImage:   The read-only source RGB8 image to desaturate.
//
//   pDestImage: a pointer to an image object. Upon successful return, this object will
//               contain a desaturated single channel copy of the source image.
//

VN_STATUS vnDesaturateImage( CONST CVImage & pSrcImage, INOUT CVImage ** pDestImage );

//
// TransformImage Operator
//
//   TransformImage performs a slow (but flexible) DCT-II transformation on the image data.
//
// Parameters:
// 
//   pSrcImage:   The read-only source R8 image to transform.
//
//   pDestImage: a pointer to an image object. Upon successful return, this object will
//               contain transform coefficients in a single channel R32S image of 
//               matching size to the source.
//

VN_STATUS vnTransformImage( CONST CVImage & pSrcImage, OUT CVImage ** pOutput );

#endif // __VN_IMAGE_H__
