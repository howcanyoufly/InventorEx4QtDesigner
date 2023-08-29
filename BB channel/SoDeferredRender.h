#pragma once
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>

class SoDeferredRender : public SoSeparator {
    typedef SoSeparator inherited;

    SO_NODE_HEADER(SoDeferredRender);

public:
    static void initClass(void);
    SoDeferredRender(void);

    virtual void GLRender(SoGLRenderAction* action);
    virtual void GLRenderBelowPath(SoGLRenderAction* action);
    virtual void GLRenderInPath(SoGLRenderAction* action);
    virtual void GLRenderOffPath(SoGLRenderAction* action);

protected:
    virtual ~SoDeferredRender();
};