
#ifdef WIN32
#include <windows.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else // __APPLE__
#include <GL/gl.h>
#endif // __APPLE__
#include "SoColorMask.h"
#include "Inventor/misc/SoState.h"
#include <Inventor/actions/SoCallbackAction.h>
#include "Inventor/actions/SoGLRenderAction.h"
#include "Inventor/elements/SoOverrideElement.h"
#include "SoColorMaskElement.h"
#include "SoGLColorMaskElement.h"

SO_NODE_SOURCE(SoColorMask);

SoColorMask::SoColorMask() :
    SoNode(),
    red(),
    green(),
    blue(),
    alpha()
{
    SO_NODE_CONSTRUCTOR(SoColorMask);

    SO_NODE_ADD_FIELD(red, (TRUE));
    SO_NODE_ADD_FIELD(green, (TRUE));
    SO_NODE_ADD_FIELD(blue, (TRUE));
    SO_NODE_ADD_FIELD(alpha, (TRUE));
}

SoColorMask::~SoColorMask()
{

}

void SoColorMask::GLRender(SoGLRenderAction* action)
{
    SoColorMask::doAction((SoAction*)action);
}

SbBool SoColorMask::affectsState() const
{
    return true;
}

void SoColorMask::doAction(SoAction* action)
{
    //SoState* state = action->getState();

    //if (SoOverrideElement::getPolygonOffsetOverride(state)) return;

    SbBool redVal;
    SbBool greenVal;
    SbBool blueVal;
    SbBool alphaVal;

    redVal = this->red.getValue();
    greenVal = this->green.getValue();
    blueVal = this->blue.getValue();
    alphaVal = this->alpha.getValue();

    SoColorMaskElement::set(action->getState(), this, redVal, greenVal, blueVal, alphaVal);

    //if (this->isOverride()) {
    //    SoOverrideElement::setPolygonOffsetOverride(state, this, TRUE);
    //}
}

void SoColorMask::callback(SoCallbackAction* action)
{
    SoColorMask::doAction((SoAction*)action);
}

void SoColorMask::initClass()
{
    SO_NODE_INIT_CLASS(SoColorMask, SoNode, "Node");

    SO_ENABLE(SoCallbackAction, SoColorMaskElement);
    SO_ENABLE(SoGLRenderAction, SoGLColorMaskElement);
}

void SoColorMask::exitClass()
{

}


