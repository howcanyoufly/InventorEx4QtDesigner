
#include "SoDeferredRender.h"

#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/actions/SoGLRenderAction.h>


SO_NODE_SOURCE(SoDeferredRender);


/*!
  Constructor.
*/
SoDeferredRender::SoDeferredRender()
{
    SO_NODE_CONSTRUCTOR(SoDeferredRender);
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
    if (action->isRenderingDelayedPaths()) 
    {
        inherited::GLRenderBelowPath(action);
    } 
    else 
    {
        SoCacheElement::invalidate(action->getState());
        action->addDelayedPath(action->getCurPath()->copy());
    }
}

// Doc in superclass.
void
SoDeferredRender::GLRenderInPath(SoGLRenderAction* action)
{
    if (action->isRenderingDelayedPaths()) 
    {
        inherited::GLRenderInPath(action);
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
