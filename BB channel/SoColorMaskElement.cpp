
#include "SoColorMaskElement.h"

#include "SbBasicP.h"

#include <cassert>

SO_ELEMENT_SOURCE(SoColorMaskElement);


void
SoColorMaskElement::initClass(void)
{
    SO_ELEMENT_INIT_CLASS(SoColorMaskElement, inherited);
}

SoColorMaskElement::~SoColorMaskElement(void)
{
}

void
SoColorMaskElement::init(SoState* state)
{
    inherited::init(state);
    SoColorMaskElement::getDefault(this->red,
        this->green,
        this->blue,
        this->alpha);
}

void
SoColorMaskElement::set(SoState* state, SoNode* node, SbBool red, SbBool green, SbBool blue, SbBool alpha)
{
    SoColorMaskElement* elem = coin_safe_cast<SoColorMaskElement*>
        (
            SoReplacedElement::getElement(state, classStackIndex, node)
        );
    if (elem) {
        elem->setElt(red, green, blue, alpha);
    }
}

void
SoColorMaskElement::get(SoState* state, SbBool& red, SbBool& green, SbBool& blue, SbBool& alpha)
{
    const SoColorMaskElement* elem =
        coin_assert_cast<const SoColorMaskElement*>
        (
            SoElement::getConstElement(state, classStackIndex)
        );

    red = elem->red;
    green = elem->green;
    blue = elem->blue;
    alpha = elem->alpha;
}

void
SoColorMaskElement::setElt(SbBool red, SbBool green, SbBool blue, SbBool alpha)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

void
SoColorMaskElement::getDefault(SbBool& red, SbBool& green, SbBool& blue, SbBool& alpha)
{
    red = TRUE;
    green = TRUE;
    blue = TRUE;
    alpha = TRUE;
}
