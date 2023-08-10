#ifndef SOWIN_FULLVIEWER_H
#define SOWIN_FULLVIEWER_H

// 

/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

#include <Inventor/Win/viewers/SoWinViewer.h>

class SoWinPopupMenu;

// *************************************************************************

class SOWIN_DLL_API SoWinFullViewer : public SoWinViewer {
  SOWIN_OBJECT_ABSTRACT_HEADER(SoWinFullViewer, SoWinViewer);

public:
  enum BuildFlag {
    BUILD_NONE       = 0x00,
    BUILD_DECORATION = 0x01,
    BUILD_POPUP      = 0x02,
    BUILD_ALL        = (BUILD_DECORATION | BUILD_POPUP)
  };

  void setDecoration(const SbBool on);
  SbBool isDecoration(void) const;

  void setPopupMenuEnabled(const SbBool on);
  SbBool isPopupMenuEnabled(void) const;

  HWND getAppPushButtonParent(void) const;
  void addAppPushButton(HWND newButton);
  void insertAppPushButton(HWND newButton, int index);
  void removeAppPushButton(HWND oldButton);
  int findAppPushButton(HWND oldButton) const;
  int lengthAppPushButton(void) const;

  HWND getRenderAreaWidget(void) const;

  virtual void setViewing(SbBool on);

  virtual void setComponentCursor(const SoWinCursor & cursor);

protected:
  SoWinFullViewer(HWND parent,
                    const char * name,
                    SbBool embed,
                    BuildFlag flag,
                    Type type,
                    SbBool build);
  ~SoWinFullViewer();

  virtual void sizeChanged(const SbVec2s & size);

  HWND buildWidget(HWND parent);

  virtual void buildDecoration(HWND parent);
  virtual HWND buildLeftTrim(HWND parent);
  virtual HWND buildBottomTrim(HWND parent);
  virtual HWND buildRightTrim(HWND parent);
  HWND buildAppButtons(HWND parent);
  HWND buildViewerButtons(HWND parent);
  virtual void createViewerButtons(HWND parent, SbPList * buttonlist);

  virtual void buildPopupMenu(void);
  virtual void setPopupMenuString(const char * title);
  virtual void openPopupMenu(const SbVec2s position);

  virtual void leftWheelStart(void);
  virtual void leftWheelMotion(float);
  virtual void leftWheelFinish(void);
  float getLeftWheelValue(void) const;
  void setLeftWheelValue(const float value);

  virtual void bottomWheelStart(void);
  virtual void bottomWheelMotion(float);
  virtual void bottomWheelFinish(void);
  float getBottomWheelValue(void) const;
  void setBottomWheelValue(const float value);

  virtual void rightWheelStart(void);
  virtual void rightWheelMotion(float);
  virtual void rightWheelFinish(void);
  float getRightWheelValue(void) const;
  void setRightWheelValue(const float value);

  void setLeftWheelString(const char * const name);
  HWND getLeftWheelLabelWidget(void) const;
  void setBottomWheelString(const char * const name);
  HWND getBottomWheelLabelWidget(void) const;
  void setRightWheelString(const char * const name);
  const char * getRightWheelString() const;
  HWND getRightWheelLabelWidget(void) const;

  virtual SbBool processSoEvent(const SoEvent * const event);

protected:
  HWND leftWheel;
  HWND rightWheel;
  HWND bottomWheel;

  HWND leftDecoration;
  HWND rightDecoration;
  HWND bottomDecoration;

  HWND leftWheelLabel;
  char * leftWheelStr;
  float leftWheelVal;

  HWND rightWheelLabel;
  char * rightWheelStr;
  float rightWheelVal;

  HWND bottomWheelLabel;
  char * bottomWheelStr;
  float bottomWheelVal;

  SoWinPopupMenu * prefmenu;

private:
  // Private class for implementation hiding. The idiom we're using is
  // a variant of what is known as the "Cheshire Cat", and is also
  // described as the "Bridge" pattern in "Design Patterns" by Gamma
  // et al (aka The Gang Of Four).
  class SoWinFullViewerP * pimpl;

  friend class SoGuiFullViewerP;
  friend class SoWinFullViewerP;



// FIXME: get rid of non-templatized code. 20020108 mortene.

#ifdef __COIN_SOXT__ // FIXME: get rid of non-templatized code. 20020108 mortene.
protected:
  Widget buildFunctionsSubmenu(Widget popup);
  Widget buildDrawStyleSubmenu(Widget popup);

  char * popupTitle;
  SbBool popupEnabled;
  SbPList * viewerButtonWidgets;
#endif // __COIN_SOXT__
};

// *************************************************************************

#endif // ! SOWIN_FULLVIEWER_H
