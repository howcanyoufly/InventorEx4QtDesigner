#pragma once
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSwitch.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <NavigationWidget.h>

using namespace SIM::Coin3D::Quarter;

class InventorEx
{
public:
    struct ShapeData
    {
        std::vector<std::array<float, 3>> points;
        std::vector<int32_t> faceIndices;
        std::vector<int32_t> lineIndices;
    };

public:
    InventorEx(int argc, char** argv);
    virtual ~InventorEx();

    // Public API
    void run(const std::string& funcName);
    void resetScene();
    const std::map<std::string, std::function<void(void)>>& getFunctions() const;

private:
    // functions ½ÚµãÊ÷
    // inventor mentor exercises
    void sphere();
    void cube();
    void engineSpin();
    void globalFlds();
    void heartSpline();
    void transformOrdering();
    void cameras();
    void lights();
    void referenceCount();
    void indexedFaceSet();

    // hiddenline discovery
    void cubeFront(SoSeparator* root);
    void cubeBehind(SoSeparator* root);
    void twoCube();
    void pickAction();

    // glcallback
    void buildScene(SoSeparator* root);
    void buildFloor();
    void glCallback();

    // Order-Independent-Transparency
    void oit();// not implement yet

    // hiddenline discovery2
    void simpleDepthTest();
    void whyNotRerender();
    void hiddenLine();

    // wireframe implement
    SoSwitch* assembleBodyScene(const ShapeData& data);
    std::vector<ShapeData> generateRandomCuboids(int count, float maxSize);// count for quentity and maxSize for volume
    void wireframe();
    void colorMaskTest();

    // preview point forward and error performance when deep test set false
    void pointInCube();
    void showRotationCenter();
    void previewPointForward();
    void twoSideFace();
    void auxViewport();// not implement yet
    void deferredRender();
    void flat();

    // discover PATH and its traversal
    void actStateOfDelayList();
    void switchToPathTraversal();// something wrong
    SoSeparator* usePathAssemble(std::vector<ShapeData> randomCuboids, bool usePath = true);
    SoSeparator* assembleEasyBody(const ShapeData& data, bool usePath = false);
    void traversalPerformance();

    // multi-flat render
    void isDelayRenderNessery();
    void twoSideLightInDelayRender();

    // about gnomon
    void gnomon();

    void depthRange();
    void clearDepthBuffer();
    void depthConflict();
    void modelView();

    void twoSideDiscover();
    void oneSideCorrect();
    void glTwoSide();

    void replaceGroup();
    void staticWireframe();
    void removeAllChildren();
    void renderImage();
    SoSeparator* refInterface();
    void refMain();
    void bigSphere();
    void dotLine();
    void dotCircle();
    void fitPlane();
    SoSwitch* assembleBodySceneShader(const ShapeData& data);
    void performance();
    SoSeparator* assembleSingleBodyScene(const std::vector<ShapeData>& cuboids);
    void pointSet();
    void customPolygonOffset();
    void lightTest();
    void levelOfDetail();
    void OBB();
    void outline();
    void outputBuffer();
    void cylinderFace();
    void gravitationalWell();
    void outline2();
    void autoZoom();
    void shaderProgram();
    void shaderHightlight();
    void zfighting();
    void terminate();
    void materialIndex();
    void divideTransp();
    void text2();
    void twoLine();
    void drtest();
    // SoLazyElement, SoState, SoGLRenderAction and openGL state


    // cylinder outline
    void cylinder();
    void cylinderGL();

    // plugins
    void loadPickAndWrite();
    void loadErrorHandle();
    void loadGLCallback();
    void loadBackground();


    std::map<std::string, std::function<void(void)>> m_functions;
    std::set<std::string> m_delayedLoadNames;
    std::vector<std::function<void(void)>> m_delayedLoadPlugins;

    QApplication* m_app;
    QMainWindow* m_mainwin;


    //#define USE_NAVIQUARTER
#ifdef USE_NAVIQUARTER
    NavQuarterWidget::Tf::NavigationWidget* m_viewer;
#else
    QuarterWidget* m_viewer;
#endif
    SoSeparator* m_root;

    bool m_reset;// function does but plugin does not
    std::vector<ShapeData> m_randomCuboids;
};
