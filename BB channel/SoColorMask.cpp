
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
#include "Inventor/actions/SoGLRenderAction.h"
#include "Inventor/elements/SoMaterialBindingElement.h"
#include "Inventor/elements/SoLightModelElement.h"
#include "Inventor/elements/SoPolygonOffsetElement.h"
#include "Inventor/elements/SoOverrideElement.h"

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

// 怎么才能让colorMask渲染其后的节点后自动恢复
void SoColorMask::GLRender(SoGLRenderAction* action)
{
    // only draw into depth buffer
    glColorMask(red.getValue(), green.getValue(), blue.getValue(), alpha.getValue());

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


