#pragma once

#include <Inventor/nodes/SoNode.h>
#include <Inventor/SbName.h>
#include <Inventor/nodes/SoGroup.h>

class SoSwitchToChild : public SoNode
{
    SO_NODE_HEADER(SoSwitchToChild);

public:
    SoSwitchToChild();

    virtual SbBool affectsState() const;

    static void exitClass();

    virtual void GLRender(SoGLRenderAction* action);

    static void initClass();

    int32_t toWhichChild;

    SbName switchName;

    SoGroup* searchRange;

protected:
    virtual ~SoSwitchToChild();

private:

};

