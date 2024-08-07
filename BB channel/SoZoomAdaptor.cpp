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
#include "Inventor/elements/SoViewVolumeElement.h"
#include "Inventor/elements/SoViewportRegionElement.h"
#include "Inventor/actions/SoAction.h"
#include "Inventor/actions/SoGetMatrixAction.h"
#include "Inventor/SbRotation.h"
#include "Inventor/SbMatrix.h"
#include "Inventor/elements/SoModelMatrixElement.h"

SO_NODE_SOURCE(SoZoomAdaptor)

SoZoomAdaptor::SoZoomAdaptor()
{
    SO_NODE_CONSTRUCTOR(SoZoomAdaptor);
    SO_NODE_ADD_FIELD(m_scaleVector, ({ 1.0f,1.0f,1.0f }));
    SO_NODE_ADD_FIELD(m_isEqualFactor, (true));
}

SoZoomAdaptor::~SoZoomAdaptor()
{
}

void SoZoomAdaptor::initClass()
{
    SO_NODE_INIT_CLASS(SoZoomAdaptor, SoTransformation, AUTO_ZOOM);
    SO_ENABLE(SoGetMatrixAction, SoViewVolumeElement);
}

SbVec3f SoZoomAdaptor::getScaleVecFactor(SoAction* action) const
{
    SbVec3f scaleVec = { m_scaleVector.getValue()[0] ,m_scaleVector.getValue()[1] ,m_scaleVector.getValue()[2] };

    SbViewVolume viewVolume = SoViewVolumeElement::get(action->getState());
    float aspectRatio = SoViewportRegionElement::get(action->getState()).getViewportAspectRatio();
    float scale = viewVolume.getWorldToScreenScale(SbVec3f(0.f, 0.f, 0.f), 0.1f) / (5 * aspectRatio);
    for (int i = 0; i < 3; ++i)
    {
        scaleVec[i] *= scale;
    }

    return scaleVec;
}

void SoZoomAdaptor::GLRender(SoGLRenderAction* action)
{
    SoZoomAdaptor::doAction((SoAction*)action);
    inherited::GLRender(action);
}

void SoZoomAdaptor::doAction(SoAction* action)
{
    SbVec3f vecFactor = this->getScaleVecFactor(action);

    if (!m_isEqualFactor.getValue())
    {
        vecFactor[1] = 1.0f;
    }

    auto state = action->getState();
    SbRotation rot, scalingOrit;
    SbVec3f scaling, trsl;
    SbMatrix matrix = SoModelMatrixElement::get(action->getState());
    matrix.getTransform(trsl, rot, scaling, scalingOrit);
    matrix.multVecMatrix(SbVec3f(0, 0, 0), trsl);
    matrix.setTransform(trsl, rot, SbVec3f(vecFactor[0], vecFactor[1], vecFactor[2]));
    SoModelMatrixElement::set(state, this, matrix);
}

void SoZoomAdaptor::getMatrix(SoGetMatrixAction* action)
{
    SbVec3f vecFactor = this->getScaleVecFactor(action);

    if (!m_isEqualFactor.getValue())
    {
        vecFactor[1] = 1.0f;
    }

    SbMatrix& matrix = action->getMatrix();
    SbRotation rot, scalingOrit;
    SbVec3f scaling, trsl;
    matrix.getTransform(trsl, rot, scaling, scalingOrit);
    matrix.multVecMatrix(SbVec3f(0, 0, 0), trsl);
    matrix.setTransform(trsl, rot, SbVec3f(vecFactor[0], vecFactor[1], vecFactor[2]));
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
