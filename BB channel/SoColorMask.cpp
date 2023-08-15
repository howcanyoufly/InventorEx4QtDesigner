

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else // __APPLE__
#include <GL/gl.h>
#endif // __APPLE__
#include "SoColorMask.h"

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
    //glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColorMask(red.getValue(), green.getValue(), blue.getValue(), alpha.getValue());
    //glPopAttrib();

}

SbBool SoColorMask::affectsState() const
{
    return true;
}

void SoColorMask::initClass()
{
    SO_NODE_INIT_CLASS(SoColorMask, SoNode, "Node");
}

void SoColorMask::exitClass()
{

}


