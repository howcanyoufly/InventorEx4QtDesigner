/*==========================================================
Copyright (c) 2022 UDS
Unpublished - All rights reserved

===========================================================
File description:
Header file reference in the project NavigationQuarterWidget
===========================================================
Date            Name                 Description of Change
2023-05-17      fjh                  Init
2023-05-30      fjh                  Add header file
2024-03-13      FJH                  Add header file
==========================================================*/
#ifndef COMMONHEADERFILES_H
#define COMMONHEADERFILES_H

#define QUARTER_DLL

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include <Inventor/scxml/SoScXMLStateMachine.h>
#include <Inventor/scxml/ScXML.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/SbPlane.h>
#include <Inventor/SbTime.h>
#include <Inventor/SbString.h>
#include <Inventor/SbImage.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTextureCoordinateCube.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFEnum.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtCore/QRectF>
#include <QtGui/QPolygon>
#include <QtGui/QPainterPath>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QWheelEvent>
#include <QtOpenGL/QGLFormat>

//#include <ImGui/imgui.h>
//#include <ImGui/QtImGui.h>

#endif // ! COMMONHEADERFILES_H