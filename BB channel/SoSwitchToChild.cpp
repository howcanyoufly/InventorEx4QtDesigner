
#ifdef WIN32
#include <windows.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else // __APPLE__
#include <GL/gl.h>
#endif // __APPLE__

#include <Inventor/nodes/SoSubNode.h>

#include "SoSwitchToChild.h"
#include <vector>
#include <Inventor/nodes/SoSwitch.h>
#include "utils.h"

SO_NODE_SOURCE(SoSwitchToChild)

SoSwitchToChild::SoSwitchToChild() :
    SoNode(),
    toWhichChild(-1),
    searchRange(NULL)
{
    SO_NODE_CONSTRUCTOR(SoSwitchToChild);
}

SoSwitchToChild::~SoSwitchToChild()
{
}

SbBool SoSwitchToChild::affectsState() const
{
    return false;
}

void SoSwitchToChild::exitClass()
{
}

void SoSwitchToChild::GLRender(SoGLRenderAction* action)
{
    std::vector<SoSwitch*> switchVec;
    switchVec = searchNodes<SoSwitch>(searchRange, switchName);
    for (auto& node : switchVec)
    {
        node->whichChild = toWhichChild;
    }
}

void SoSwitchToChild::initClass()
{
    SO_NODE_INIT_CLASS(SoSwitchToChild, SoNode, "Node");
}
