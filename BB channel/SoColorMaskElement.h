#pragma once

#include <Inventor/elements/SoReplacedElement.h>

class SoColorMaskElement : public SoReplacedElement {
    typedef SoReplacedElement inherited;

    SO_ELEMENT_HEADER(SoColorMaskElement);

public:
    static void initClass(void);
protected:
    virtual ~SoColorMaskElement();

public:
    virtual void init(SoState* state);
    static void set(SoState* state, SoNode* node, SbBool red, SbBool green, SbBool blue, SbBool alpha);
    static void get(SoState* state, SbBool& red, SbBool& green, SbBool& blue, SbBool& alpha);
    static void getDefault(SbBool& red, SbBool& green, SbBool& blue, SbBool& alpha);

protected:
    SbBool red;
    SbBool green;
    SbBool blue;
    SbBool alpha;

    virtual void setElt(SbBool red, SbBool green, SbBool blue, SbBool alpha);
};
