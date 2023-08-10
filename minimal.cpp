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

/*!
  This example shows a minimal stand-alone
  example of a QuarterViewer without the use of UI files
 */

#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <QApplication>
#include <QMainWindow>

using namespace SIM::Coin3D::Quarter;

int
main(int argc, char** argv)
{
    QApplication app(argc, argv);
    // Initializes Quarter (and implicitly also Coin and Qt
    Quarter::init();

    // Make a dead simple scene graph by using the Coin library, only
    // containing a single yellow cone under the scenegraph root.
    SoSeparator* root = new SoSeparator;
    root->ref();

    //SoBaseColor * col = new SoBaseColor;
    //col->rgb = SbColor(1, 1, 0);
    //root->addChild(col);

    //root->addChild(new SoCone);
    float pts[8][3] = {
      { 0.0, 0.0, 0.0 },
      { 1.0, 0.0, 0.0 },
      { 1.0, 1.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      { 1.0, 0.0, 1.0 },
      { 1.0, 1.0, 1.0 },
      { 0.0, 1.0, 1.0 },
    };
    int32_t faceIndices[48] = {
        0, 2, 1, SO_END_FACE_INDEX,
        0, 3, 2, SO_END_FACE_INDEX,
        0, 1, 5, SO_END_FACE_INDEX,
        0, 5, 4, SO_END_FACE_INDEX,
        1, 2, 6, SO_END_FACE_INDEX,
        1, 6, 5, SO_END_FACE_INDEX,
        2, 3, 6, SO_END_FACE_INDEX,
        3, 7, 6, SO_END_FACE_INDEX,
        3, 4, 7, SO_END_FACE_INDEX,
        0, 4, 3, SO_END_FACE_INDEX,
        4, 5, 7, SO_END_FACE_INDEX,
        5, 6, 7, SO_END_FACE_INDEX,
    };
    int32_t lineIndices[24] = {
        0, 1, 2, 3, 0, SO_END_LINE_INDEX,
        4, 5, 6, 7, 4, SO_END_LINE_INDEX,
        0, 4, SO_END_LINE_INDEX,
        1, 5, SO_END_LINE_INDEX,
        2, 6, SO_END_LINE_INDEX,
        3, 7, SO_END_LINE_INDEX
    };

/*
==================================
ViewProviderSoShow
==================================
m_root
└── bodySwitch (SoSwitch)
    └── body (SoSeparator)
==================================
ViewProviderBody
==================================
        ├── scale (SoScale)
        ├── trasparencyTypeSwitch (SoSwitch)
        |   └── trasparencyType (SoTransparencyType)
        └── dataNode (SoSeparator)
            ├── coords (SoCoordinate3)
            └── renderModeSwitch (SoSwitch)
                ├── shadeWithEdge (SoSeparator)
                |   ├── material (SoMaterial)
                |   ├── offset (SoPolygonOffset)
                |   ├── faceRoot (SoSeparator)
                |   |   ├── faceStyle (SoDrawStyle)
                |   |   ├── faceNormal (SoNormal)
                |   |   ├── normalBinding (SoNormalBinding)
                |   |   ├── materialSwitch (SoSwitch)
                |   |   |   └── transparentMaterial (SoMaterial)
                |   |   └── faceSet (SoIndexedFaceSet)
                |   └── lineVisbleRoot (SoSeparator)
                |       └── lineVisibleSet (SoIndexedLineSet)
                ├── shadeWithoutEdge (SoSeparator)
                ├── transluency (SoSeparator)
                ├── staticWireframe (SoSeparator)
                └── wireframeWithoutHidden (SoSeparator)
*/
    SoSwitch* bodySwitch = new SoSwitch;
    SoSeparator* body = new SoSeparator;
    SoScale* scale = new SoScale;
    SoSwitch* trasparencyTypeSwitch = new SoSwitch;
    SoTransparencyType* trasparencyType = new SoTransparencyType;
    SoSeparator* dataNode = new SoSeparator;
    SoCoordinate3* coords = new SoCoordinate3;
    SoSwitch* renderModeSwitch = new SoSwitch;
    SoSeparator* shadeWithEdge = new SoSeparator;
    SoSeparator* shadeWithoutEdge = new SoSeparator;
    SoSeparator* transluency = new SoSeparator;
    SoSeparator* staticWireframe = new SoSeparator;
    SoSeparator* wireframeWithoutHidden = new SoSeparator;
    SoSeparator* faceRoot = new SoSeparator;
    SoSeparator* lineVisbleRoot = new SoSeparator;
    SoMaterial* material = new SoMaterial;
    SoPolygonOffset* offset = new SoPolygonOffset;
    SoDrawStyle* faceStyle = new SoDrawStyle;
    SoNormal* faceNormal = new SoNormal;
    SoNormalBinding* normalBinding = new SoNormalBinding;
    SoSwitch* materialSwitch = new SoSwitch;
    SoIndexedFaceSet/*SoBrepFaceSet*/* faceSet = new SoIndexedFaceSet;
    SoMaterial* transparentMaterial = new SoMaterial;
    SoIndexedLineSet* lineVisibleSet = new SoIndexedLineSet;

    // ViewProviderSoShow
    root->addChild(bodySwitch);
    bodySwitch->addChild(body);
    // ViewProviderBody
    body->addChild(scale);
    body->addChild(trasparencyTypeSwitch);
    body->addChild(dataNode);
    trasparencyTypeSwitch->addChild(trasparencyType);
    dataNode->addChild(coords);
    dataNode->addChild(renderModeSwitch);
    renderModeSwitch->addChild(shadeWithEdge);
    renderModeSwitch->addChild(shadeWithoutEdge);
    renderModeSwitch->addChild(transluency);
    renderModeSwitch->addChild(staticWireframe);
    renderModeSwitch->addChild(wireframeWithoutHidden);
    shadeWithEdge->addChild(material);
    shadeWithEdge->addChild(offset);
    shadeWithEdge->addChild(faceRoot);
    shadeWithEdge->addChild(lineVisbleRoot);
    faceRoot->addChild(faceStyle);
    faceRoot->addChild(faceNormal);
    faceRoot->addChild(normalBinding);
    faceRoot->addChild(materialSwitch);
    faceRoot->addChild(faceSet);
    materialSwitch->addChild(transparentMaterial);
    lineVisbleRoot->addChild(lineVisibleSet);

    bodySwitch->whichChild = 0;
    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 0;

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    lineVisibleSet->coordIndex.setValues(0, 24, lineIndices);


    QMainWindow* mainwin = new QMainWindow();

    // Create a QuarterWidget for displaying a Coin scene graph
    QuarterWidget* viewer = new QuarterWidget(mainwin);
    //set default navigation mode file
    viewer->setNavigationModeFile();
    mainwin->setCentralWidget(viewer);
    viewer->setSceneGraph(root);

    // Pop up the QuarterWidget
    mainwin->show();
    // Loop until exit.
    app.exec();
    // Clean up resources.
    root->unref();
    delete viewer;

    Quarter::clean();

    return 0;
}
