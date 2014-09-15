Insight: Perceptual Hashing Engine
=======

Insight is a simple perceptual hashing engine that enables fuzzy image comparison through the estimation of visual similarity. This system detects structural similarities and is invariant to scale and rotation, so it is particularly useful for certain kinds of tasks including:

* Reverse image search
* Pose estimation
* Data forensics
* Image reconstruction

This is accomplished by generating a pair of digital fingerprints of two files in a manner that is resiliant to minor changes. These fingerprints can then be compared in order to form an estimate of similarity.

For more information about Insight and how it works, visit [bertolami.com](http://bertolami.com/index.php?engine=blog&content=posts&detail=perceptual-hashing).

Using Insight

=======



The primary interface to Insight is the CompareImages function:



    FLOAT32 vnCompareImages( CONST CVImage & pA,
 
                            CONST CVImage & pB );



This function will return a value in the interval of [0,1] that indicates the percentage of similarity between the two input images.



CVImage objects are simple wrappers around a formatted memory buffer. To create a new image, call vnCreateImage(), passing in the dimensions and desired image format (currently only VN_IMAGE_FORMAT_R8G8B8 is supported by Insight). Once this call completes, the image object is allocated and ready to be filled with your image data. You can supply this data by copying it to the address indicated by CVImage::QueryData(). Don't forget to call vnDestroyImage() when you've finished with your image object.