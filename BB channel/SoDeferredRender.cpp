
#include "SoDeferredRender.h"

#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/actions/SoGLRenderAction.h>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else // __APPLE__
#include <GL/gl.h>
#endif // __APPLE__
#include "utils.h"

SO_NODE_SOURCE(SoDeferredRender);

/*!
  Constructor.
*/
SoDeferredRender::SoDeferredRender()
    : isUsingNewAction(FALSE)
{
    SO_NODE_CONSTRUCTOR(SoDeferredRender);
    SO_NODE_ADD_FIELD(clearDepthBuffer, (FALSE));
}

SoDeferredRender::SoDeferredRender(SbBool bClearDepthBuffer)
    : isUsingNewAction(FALSE)
{
    SO_NODE_CONSTRUCTOR(SoDeferredRender);
    this->clearDepthBuffer.setValue(bClearDepthBuffer);
}

/*!
  Destructor.
*/
SoDeferredRender::~SoDeferredRender()
{
}

// Doc in superclass.
/*!
  \copybrief SoBase::initClass(void)
*/
void
SoDeferredRender::initClass(void)
{
    SO_NODE_INIT_CLASS(SoDeferredRender, SoSeparator, "Separator");
}

// Doc in superclass.
void
SoDeferredRender::GLRender(SoGLRenderAction* action)
{
    switch (action->getCurPathCode()) {
    case SoAction::NO_PATH:
    case SoAction::BELOW_PATH:
        this->GLRenderBelowPath(action);
        break;
    case SoAction::OFF_PATH:
        // do nothing. Separator will reset state.
        break;
    case SoAction::IN_PATH:
        this->GLRenderInPath(action);
        break;
    }
}

// Doc in superclass.
void
SoDeferredRender::GLRenderBelowPath(SoGLRenderAction* action)
{
    if (isUsingNewAction)
    {   // 正常渲染
        inherited::GLRenderBelowPath(action);
    }
    else if (action->isRenderingDelayedPaths())
    {   // 从delayedpaths进入
        if (clearDepthBuffer.getValue())
            glClear(GL_DEPTH_BUFFER_BIT);
        SoGLRenderAction::TransparencyType transparencyType = action->getTransparencyType();
        if (transparencyType == SoGLRenderAction::DELAYED_ADD ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_ADD ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD ||
            transparencyType == SoGLRenderAction::DELAYED_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_LAYERS_BLEND)
        {   // delayed透明模式下的路径使用新action通道
            SoGLRenderAction newAction(action->getViewportRegion());
            copyGLRenderAction(*action, newAction);
            SoPath* newPath = action->getCurPath()->copy();
            newPath->ref();
            isUsingNewAction = TRUE;
            newAction.apply(newPath);
            isUsingNewAction = FALSE;
            newPath->unref();
        }
        else
        {
            inherited::GLRenderBelowPath(action);
        }
    }
    else
    {
        SoCacheElement::invalidate(action->getState());
        action->addDelayedPath(action->getCurPath()->copy());
    }
}

// Doc in superclass.
void SoDeferredRender::GLRenderInPath(SoGLRenderAction* action)
{
    if (isUsingNewAction)
    {   // 正常渲染
        inherited::GLRenderInPath(action);
    }
    else if (action->isRenderingDelayedPaths())
    {   // 从delayedpaths进入
        if (clearDepthBuffer.getValue())
            glClear(GL_DEPTH_BUFFER_BIT);
        SoGLRenderAction::TransparencyType transparencyType = action->getTransparencyType();
        if (transparencyType == SoGLRenderAction::DELAYED_ADD ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_ADD ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD ||
            transparencyType == SoGLRenderAction::DELAYED_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND ||
            transparencyType == SoGLRenderAction::SORTED_LAYERS_BLEND)
        {   // delayed透明模式下的路径使用新action通道
            SoGLRenderAction newAction(action->getViewportRegion());
            copyGLRenderAction(*action, newAction);
            SoPath* newPath = action->getCurPath()->copy();
            newPath->ref();
            isUsingNewAction = TRUE;
            newAction.apply(newPath);
            isUsingNewAction = FALSE;
            newPath->unref();
        }
        else
        {
            inherited::GLRenderInPath(action);
        }
    }
    else
    {
        SoCacheElement::invalidate(action->getState());
        action->addDelayedPath(action->getCurPath()->copy());
    }
}

// Doc in superclass.
void
SoDeferredRender::GLRenderOffPath(SoGLRenderAction*)
{
    // should never render, this is a separator node
}
