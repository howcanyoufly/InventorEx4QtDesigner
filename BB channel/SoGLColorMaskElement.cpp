
#include "SoGLColorMaskElement.h"

#include <cassert>
#include <cstdlib>

#include <Inventor/C/tidbits.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoGLDriverDatabase.h>

#ifdef __GNUC__
#define COIN_UNUSED_ARG(x) x __attribute__((__unused__))
#else
#define COIN_UNUSED_ARG(x) x
#endif

SO_ELEMENT_SOURCE(SoGLColorMaskElement);

void
SoGLColorMaskElement::initClass(void)
{
    SO_ELEMENT_INIT_CLASS(SoGLColorMaskElement, inherited);
}

/*!
  Destructor.
*/

SoGLColorMaskElement::~SoGLColorMaskElement(void)
{
}

void
SoGLColorMaskElement::init(SoState* stateptr)
{
    inherited::init(stateptr);
    this->state = stateptr;
}

void
SoGLColorMaskElement::push(SoState* stateptr)
{
    SoGLColorMaskElement* prev = (SoGLColorMaskElement*)this->getNextInStack();

    this->red = prev->red;
    this->green = prev->green;
    this->blue = prev->blue;
    this->alpha = prev->alpha;
    this->state = stateptr;
    // capture previous element since we might or might not change the
    // GL state in set/pop
    prev->capture(stateptr);
}

//! FIXME: write doc.

void
SoGLColorMaskElement::pop(SoState* COIN_UNUSED_ARG(stateptr), const SoElement* prevTopElement)
{
    const SoGLColorMaskElement* prev = (const SoGLColorMaskElement*)prevTopElement;

    if (this->red != prev->red || this->green != prev->green || this->blue != prev->blue || this->alpha != prev->alpha) 
    {
        this->updategl();
    }
}

void
SoGLColorMaskElement::setElt(SbBool red, SbBool green, SbBool blue, SbBool alpha)
{
    if (red != this->red || green != this->green || blue != this->blue || alpha != this->alpha) 
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
        this->updategl();
    }
}

void
SoGLColorMaskElement::updategl(void)
{
    glColorMask(this->red, this->green, this->blue, this->alpha);
}

