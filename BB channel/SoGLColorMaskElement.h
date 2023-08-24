#pragma once

#include "SoColorMaskElement.h"

class SoGLColorMaskElement : public SoColorMaskElement {
    typedef SoColorMaskElement inherited;

    SO_ELEMENT_HEADER(SoGLColorMaskElement);
public:
    static void initClass(void);
protected:
    virtual ~SoGLColorMaskElement();

public:
    virtual void init(SoState* state);
    virtual void push(SoState* state);
    virtual void pop(SoState* state, const SoElement* prevTopElement);

protected:
    virtual void setElt(SbBool red, SbBool green, SbBool blue, SbBool alpha);

private:

    SoState* state; // needed to test for OpenGL extension
    void updategl(void);
};
