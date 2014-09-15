
//
// Copyright (c) 2009-2014 Joe Bertolami. All Right Reserved.
//
// vnInsight.h
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
//   Insight is a simple perceptual hashing engine that can be used to create a
//   measurement of similarity between two images. This source release is available 
//   primarily as a learning aid for those interested in learning more about 
//   perceptual image hashing and its uses. 
//
//   Note that Insight relies heavily on both the Vision Platform (v3), and the 
//   Imagine Library (v.105). Vision provides a foundation of basic types, data 
//   structures, and utiliy functions, as well as providing a measure of platform 
//   independence.
//
//   Imagine ("Image Engine") is a 256 bit image processing library that provides
//   optimized routines for a very large assortment of operations. 
//
//   Although Insight leverages both of these tools, they are not fully included in 
//   this source release. Instead, this release contains compact versions that provide 
//   only the minimal functionality required for Insight. To download the full versions
//   of Vision or Imagine (which are not required for Insight), visit the website below.  
//
//  Additional Information:
//
//   For more information, visit http://www.bertolami.com.
//

#ifndef __INSIGHT_H__
#define __INSIGHT_H__

#include "Platform/vnBase.h"
#include "Platform/vnBitStream.h"
#include "Imagine/vnImagine.h"

//
// (!) Note: Insight measures the Hamming distance of two perceptual hashes in order 
//           to derive the similarity of two images. Even two very different images
//           will have a number of bits in common, which may produce a higher than
//           expected similarity rating. 
//
//           Use the following table to filter the percentage results:
//
//           90 -100%  - high similarity
//           70 - 90%  - possible similarity (likely with significant distortion)
//            0 - 70%  - likely dissimilarity
//

//
// vnHashImage
//
//   Generates a perceptual hash of pInput and places it within pOutStream.
//
// Parameters:
//
//   pInput:      the source image to hash.
//   uiThumbSize: the width of the symmetric workspace thumbnail image (see notes).
//   uiHashSize:  the resultant hash size, in bytes, that is produced by this function.
//   pOutStream:  the destination that will store the output hash.
//
// Returns:
//
//   A status code indicating success or failure of the operation.
//
// Notes:
//
//   The quality of the hashing algorithm is heavily dependent upon the thumbsize and 
//   hashsize parameter values. Generally speaking, the higher these values are, the 
//   higher quality (and more accurate) the hash will be. 
//
//   If a zero is specified for either of these values, default values of (16, 8) will
//   be used.
//

VN_STATUS vnHashImage( CONST CVImage & pInput, UINT32 uiThumbSize, UINT32 uiHashSize, CVBitStream * pOutStream );

//
// vnCompareImages
//
//   Compares pA and pB and returns a rating of their similarity. 
//
// Parameters:
//
//   pA: the first of two images to compare.
//   pB: the second image to compare (against pA).
//
// Returns:
//
//   The return value is the percent of similarity.
//

FLOAT32 vnCompareImages( CONST CVImage & pA, CONST CVImage & pB );

#endif // __INSIGHT_H__