#ifndef SOWIN_RENDERAREA_H
#define SOWIN_RENDERAREA_H

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

#include <Inventor/SbColor.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/SoSceneManager.h>
#if HAVE_SOSCENEMANAGER_GETBACKGROUNDCOLORRGBA
#include <Inventor/SbColorRGBA.h>
#endif

#include <Inventor/Win/SoWinGLWidget.h>

class SbColor;
class SoNode;
class SoSelection;

class SoWinDevice;
// SoWinRenderAreaP is only used in the "friend class" statement in
// the class definition, so this shouldn't really be necessary. But
// the OSF1/cxx compiler complains if it's left out.
class SoWinRenderAreaP;

typedef SbBool SoWinRenderAreaEventCB(void * closure, MSG* event);

// *************************************************************************

class SOWIN_DLL_API SoWinRenderArea : public SoWinGLWidget {
  SOWIN_OBJECT_HEADER(SoWinRenderArea, SoWinGLWidget);

public:
  SoWinRenderArea(HWND parent = NULL,
                    const char * name = NULL,
                    SbBool embed = TRUE,
                    SbBool mouseInput = TRUE,
                    SbBool keyboardInput = TRUE);
  ~SoWinRenderArea();

  virtual void setSceneGraph(SoNode * scene);
  virtual SoNode * getSceneGraph(void);
  void setOverlaySceneGraph(SoNode * scene);
  SoNode * getOverlaySceneGraph(void);

  void setBackgroundColor(const SbColor & color);
  const SbColor & getBackgroundColor(void) const;
#if HAVE_SOSCENEMANAGER_GETBACKGROUNDCOLORRGBA
  void setBackgroundColorRGBA(const SbColorRGBA & color);
  const SbColorRGBA & getBackgroundColorRGBA(void) const;
#endif
  void setBackgroundIndex(int idx);
  int getBackgroundIndex(void) const;
  void setOverlayBackgroundIndex(int idx);
  int getOverlayBackgroundIndex(void) const;
  void setColorMap(int start, int num, const SbColor * colors);
  void setOverlayColorMap(int start, int num, const SbColor * colors);
  void setViewportRegion(const SbViewportRegion & newRegion);
  const SbViewportRegion & getViewportRegion(void) const;
  void setTransparencyType(SoGLRenderAction::TransparencyType type);
  SoGLRenderAction::TransparencyType getTransparencyType(void) const;
  void setAntialiasing(SbBool smoothing, int numPasses);
  void getAntialiasing(SbBool & smoothing, int & numPasses) const;
  void setClearBeforeRender(SbBool enable, SbBool zbEnable = TRUE);
  SbBool isClearBeforeRender(void) const;
  SbBool isClearZBufferBeforeRender(void) const;
  void setClearBeforeOverlayRender(SbBool enable);
  SbBool isClearBeforeOverlayRender(void) const;
  void setAutoRedraw(SbBool enable);
  SbBool isAutoRedraw(void) const;
  void setRedrawPriority(uint32_t priority);
  uint32_t getRedrawPriority(void) const;
  static uint32_t getDefaultRedrawPriority(void);
  void render(void);
  void renderOverlay(void);
  void scheduleRedraw(void);
  void scheduleOverlayRedraw(void);
  void redrawOnSelectionChange(SoSelection * selection);
  void redrawOverlayOnSelectionChange(SoSelection * selection);
  void setEventCallback(SoWinRenderAreaEventCB * func, void * user = NULL);
  void setSceneManager(SoSceneManager * manager);
  SoSceneManager * getSceneManager(void) const;
  void setOverlaySceneManager(SoSceneManager * manager);
  SoSceneManager * getOverlaySceneManager(void) const;
  void setGLRenderAction(SoGLRenderAction * action);
  SoGLRenderAction * getGLRenderAction(void) const;
  void setOverlayGLRenderAction(SoGLRenderAction * action);
  SoGLRenderAction * getOverlayGLRenderAction(void) const;

  SbBool sendSoEvent(const SoEvent * event);

  void registerDevice(SoWinDevice * device);
  void unregisterDevice(SoWinDevice * device);


protected:
  SoWinRenderArea(HWND parent,
                    const char * name,
                    SbBool embed,
                    SbBool mouseInput,
                    SbBool keyboardInput,
                    SbBool build);

  virtual void redraw(void);
  virtual void actualRedraw(void);
  virtual void redrawOverlay(void);
  virtual void actualOverlayRedraw(void);

  virtual SbBool processSoEvent(const SoEvent * const event);
  virtual void processEvent(MSG* event);
  virtual void initGraphic(void);
  virtual void initOverlayGraphic(void);
  virtual void sizeChanged(const SbVec2s & size);
  virtual void widgetChanged(HWND widget);
  virtual void afterRealizeHook(void);

  HWND buildWidget(HWND parent);

  virtual const char * getDefaultWidgetName(void) const;
  virtual const char * getDefaultTitle(void) const;
  virtual const char * getDefaultIconTitle(void) const;

  virtual SbBool glScheduleRedraw(void);

private:
  class SoWinRenderAreaP * pimpl;
  friend class SoWinRenderAreaP;
};

// *************************************************************************

#endif // ! SOWIN_RENDERAREA_H
