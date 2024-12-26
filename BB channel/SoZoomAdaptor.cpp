/*========================================================================
Copyright (c) 2023
Unpublished - All rights reserved

==========================================================================
File description:
Implement of SoZoomAdaptor

==========================================================================
Date            Name        Description of Change
2023/06/08      WY          Init
2023/10/07      WH          Modify scale
==========================================================================*/
#include "SoZoomAdaptor.h"
#include <windows.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbMatrix.h>

SO_NODE_SOURCE(SoZoomAdaptor)

SoZoomAdaptor::SoZoomAdaptor()
{
    SO_NODE_CONSTRUCTOR(SoZoomAdaptor);
    SO_NODE_ADD_FIELD(m_pixelType, (DESIGNED));

    SO_NODE_DEFINE_ENUM_VALUE(PixelType, DESIGNED);
    SO_NODE_DEFINE_ENUM_VALUE(PixelType, REAL);
    SO_NODE_SET_SF_ENUM_TYPE(m_pixelType, PixelType);

}

SoZoomAdaptor::~SoZoomAdaptor()
{
}

void SoZoomAdaptor::initClass()
{
    SO_NODE_INIT_CLASS(SoZoomAdaptor, SoTransformation, AUTO_ZOOM);
    SO_ENABLE(SoGetMatrixAction, SoViewVolumeElement);
}

float SoZoomAdaptor::getScaleFactor(SoAction* action) const
{
    SbViewportRegion view = SoViewportRegionElement::get(action->getState());
    SbVec2s size = view.getWindowSize();

    SbViewVolume viewVolume = SoViewVolumeElement::get(action->getState());

    float factorOnePixel = 1.0f;
    if (DESIGNED == m_pixelType.getValue())
    {
        HDC hdc = GetDC(NULL);
        int vertRes = GetDeviceCaps(hdc, VERTRES);
        ReleaseDC(NULL, hdc);
        factorOnePixel = vertRes / 1080.0f;
    }

    return viewVolume.getWorldToScreenScale(SbVec3f(0.f, 0.f, 0.f), factorOnePixel / size[0]);
}

void SoZoomAdaptor::GLRender(SoGLRenderAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
    inherited::GLRender(action);
}

void SoZoomAdaptor::doAction(SoAction* action)
{
    float scale = getScaleFactor(action);

    auto state = action->getState();
    SbRotation rot, scalingOrit;
    SbVec3f scaling, trsl;
    SbMatrix matrix = SoModelMatrixElement::get(action->getState());
    matrix.getTransform(trsl, rot, scaling, scalingOrit);
    matrix.multVecMatrix(SbVec3f(0, 0, 0), trsl);
    matrix.setTransform(trsl, rot, SbVec3f(scale, scale, scale));
    SoModelMatrixElement::set(state, this, matrix);
}

void SoZoomAdaptor::getMatrix(SoGetMatrixAction* action)
{
    float scale = getScaleFactor(action);

    SbMatrix& matrix = action->getMatrix();
    SbRotation rot, scalingOrit;
    SbVec3f scaling, trsl;
    matrix.getTransform(trsl, rot, scaling, scalingOrit);
    matrix.multVecMatrix(SbVec3f(0, 0, 0), trsl);
    matrix.setTransform(trsl, rot, SbVec3f(scale, scale, scale));
    action->getInverse() = matrix.inverse();
}

void SoZoomAdaptor::callback(SoCallbackAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
}

void SoZoomAdaptor::getBoundingBox(SoGetBoundingBoxAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
}

void SoZoomAdaptor::pick(SoPickAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
}

void SoZoomAdaptor::getPrimitiveCount(SoGetPrimitiveCountAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
}
