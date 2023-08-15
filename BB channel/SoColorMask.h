/*================================================================================
 *** THE CONTENT OF THIS WORK IS PROPRIETARY TO FEI S.A.S, (FEI S.A.S.),       ***
 ***              AND IS DISTRIBUTED UNDER A LICENSE AGREEMENT.                ***
 ***                                                                           ***
 ***  REPRODUCTION, DISCLOSURE,  OR USE,  IN WHOLE OR IN PART,  OTHER THAN AS  ***
 ***  SPECIFIED  IN THE LICENSE ARE  NOT TO BE  UNDERTAKEN  EXCEPT WITH PRIOR  ***
 ***  WRITTEN AUTHORIZATION OF FEI S.A.S.                                      ***
 ***                                                                           ***
 ***                        RESTRICTED RIGHTS LEGEND                           ***
 ***  USE, DUPLICATION, OR DISCLOSURE BY THE GOVERNMENT OF THE CONTENT OF THIS ***
 ***  WORK OR RELATED DOCUMENTATION IS SUBJECT TO RESTRICTIONS AS SET FORTH IN ***
 ***  SUBPARAGRAPH (C)(1) OF THE COMMERCIAL COMPUTER SOFTWARE RESTRICTED RIGHT ***
 ***  CLAUSE  AT FAR 52.227-19  OR SUBPARAGRAPH  (C)(1)(II)  OF  THE RIGHTS IN ***
 ***  TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 52.227-7013.        ***
 ***                                                                           ***
 ***                   COPYRIGHT (C) 1996-2020 BY FEI S.A.S,                   ***
 ***                        BORDEAUX, FRANCE                                   ***
 ***                      ALL RIGHTS RESERVED                                  ***
 ================================================================================*/

#pragma once

#include <Inventor/nodes/SoNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/nodes/SoSubNode.h>

 /**
  * @VSGEXT Enable and disable writing of frame buffer color components
  *
  * @ingroup Nodes
  *
  * @DESCRIPTION
  *  Specifies whether individual color components in the frame buffer
  *  will be modified during rendering.
  *
  *  The color mask will affect all draw buffers.
  *
  *  "Hidden Line Rendering" is a typical use case where a SoColorMask can be
  *  useful: faces are first rendered with #red, #green, #blue and #alpha masks
  *  to FALSE, so that only depth is written. Then, the shape is rendered in
  *  wireframe mode, with all masks to TRUE.
  *
  *  To control modifying the depth buffer during rendering see SoDepthBuffer.
  *
  * @FILE_FORMAT_DEFAULT
  *    ColorMask {
  *    @TABLE_FILE_FORMAT
  *       @TR red   @TD TRUE
  *       @TR green @TD TRUE
  *       @TR blue  @TD TRUE
  *       @TR alpha @TD TRUE
  *    @TABLE_END
  *    }
  *
  * @SEE_ALSO
  *   SoDrawStyle, SoDepthBuffer
  *
  * @NODE_SINCE_OIV 10.4
  *
  */
class SoColorMask : public SoNode
{
    SO_NODE_HEADER(SoColorMask);

public:
    /**
     * Specifies whether or not the red component can be written into the
     * frame buffer. The default value is TRUE, indicating that the color
     * component can be written.
     */
    SoSFBool red;

    /**
     * Specifies whether or not the green component can be written into the
     * frame buffer. The default value is TRUE, indicating that the color
     * component can be written.
     */
    SoSFBool green;

    /**
     * Specifies whether or not the blue component can be written into the
     * frame buffer. The default value is TRUE, indicating that the color
     * component can be written.
     */
    SoSFBool blue;

    /**
     * Specifies whether or not the alpha component can be written into the
     * frame buffer. The default value is TRUE, indicating that the color
     * component can be written.
     */
    SoSFBool alpha;

    SoColorMask();

SoEXTENDER public:
    virtual void GLRender(SoGLRenderAction* action);
    virtual SbBool affectsState() const;

SoINTERNAL public:

    static void initClass();
    static void exitClass();

protected:
    virtual ~SoColorMask();
};
