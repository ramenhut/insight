
#include "vnImagine.h"

VN_STATUS vnCreateImage( VN_IMAGE_FORMAT format, UINT32 uiWidth, UINT32 uiHeight, OUT CVImage ** pOutImage )
{
    if ( VN_PARAM_CHECK )
    {
        if ( 0 == uiWidth || 0 == uiHeight )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }

        if ( !pOutImage )
        {
            return vnPostError( VN_ERROR_INVALIDARG );
        }
    }

    (*pOutImage) = new CVImage;

    if ( !(*pOutImage) )
    {
        return vnPostError( VN_ERROR_OUTOFMEMORY );
    }

    if ( VN_FAILED( (*pOutImage)->SetFormat( format ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    if ( VN_FAILED( (*pOutImage)->SetDimension( uiWidth, uiHeight ) ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    return VN_SUCCESS;
}

VN_STATUS vnDestroyImage( CVImage * pInImage )
{
    if ( !pInImage )
    {
        return VN_SUCCESS;
    }

    if ( VN_FAILED( pInImage->Deallocate() ) )
    {
        return vnPostError( VN_ERROR_EXECUTION_FAILURE );
    }

    delete pInImage;

    return VN_SUCCESS;
}