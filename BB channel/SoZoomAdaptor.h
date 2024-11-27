/*========================================================================
Copyright (c) 2023
Unpublished - All rights reserved

==========================================================================
File description:
Provide AutoZoom Function, maintaining objects' scale

==========================================================================
Date            Name        Description of Change
2023/06/08      WY          Init
2023/10/07      WH          Modify scale
==========================================================================*/
#ifndef SO_ZOOM_ADAPTOR_H
#define SO_ZOOM_ADAPTOR_H
#include <Inventor/nodes/SoTransformation.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFVec3f.h>

class SoZoomAdaptor : public SoTransformation
{
    typedef SoTransformation inherited;
    SO_NODE_HEADER(SoZoomAdaptor);

public:
    static void initClass();
    SoZoomAdaptor();

protected:
    virtual ~SoZoomAdaptor();
    virtual void doAction(SoAction* action);
    virtual void getPrimitiveCount(SoGetPrimitiveCountAction* action);
    virtual void getMatrix(SoGetMatrixAction* action);
    virtual void GLRender(SoGLRenderAction* action);
    virtual void getBoundingBox(SoGetBoundingBoxAction* action);
    virtual void callback(SoCallbackAction* action);
    virtual void pick(SoPickAction* action);

    float getScaleFactor(SoAction*) const;

public:
    SoSFBool m_isEqualFactor;
    SoSFVec3f m_scaleVector;
};
#endif // SO_ZOOM_ADAPTOR_H
