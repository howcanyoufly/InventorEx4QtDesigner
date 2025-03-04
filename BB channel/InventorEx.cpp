
#include "InventorEx.h"

#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoFullPath.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoTransformSeparator.h>
#include <Inventor/nodes/SoShuttle.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDepthBuffer.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoShaderProgram.h>
#include <Inventor/nodes/SoVertexShader.h>
#include <Inventor/nodes/SoFragmentShader.h>
#include <Inventor/nodes/SoShaderParameter.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoGeometryShader.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoReorganizeAction.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>

#include <GL/gl.h>
#include <QOpenGLTexture>

#include "SoGradientBackground.h"
#include "SoColorMask.h"
#include "SoColorMaskElement.h"
#include "SoGLColorMaskElement.h"
#include "SoSwitchToChild.h"
#include "SoDeferredRender.h"
#include "SoZoomAdaptor.h"

#include "utils.h"

#include "QThreadPool"
#include "Inventor/events/SoLocation2Event.h"


#define CREATE_NODE(type, name) \
    type* name = new type; \
    name->setName(#name);

#define ADD_CHILD(parent, child) \
    parent->addChild(child);

#define WINDOWWIDTH 1200
#define WINDOWHEIGHT 900

static int s_renderCountForDebug = 0;

void errorHandlerCB(const SoError* err, void* data);
void mousePressCB(void* userData, SoEventCallback* eventCB);
SbBool writePickedPath(SoNode* root, const SbViewportRegion& viewport, const SbVec2s& cursorPosition);
void enableColorMask(void*, SoAction* action);
void disableColorMask(void*, SoAction* action);

InventorEx::InventorEx(int argc, char** argv)
    : m_reset(false)
{
    // 初始化函数映射
    m_functions = {
        // function
        {"sphere", std::bind(&InventorEx::sphere, this)},
        {"cube", std::bind(&InventorEx::cube, this)},
        {"engineSpin", std::bind(&InventorEx::engineSpin, this)},
        {"globalFlds", std::bind(&InventorEx::globalFlds, this)},
        {"heartSpline", std::bind(&InventorEx::heartSpline, this)},
        {"transformOrdering", std::bind(&InventorEx::transformOrdering, this)},
        {"cameras", std::bind(&InventorEx::cameras, this)},
        {"lights", std::bind(&InventorEx::lights, this)},
        {"referenceCount", std::bind(&InventorEx::referenceCount, this)},
        {"indexedFaceSet", std::bind(&InventorEx::indexedFaceSet, this)},
        {"indexedLineSet", std::bind(&InventorEx::indexedLineSet, this)},
        {"twoCube", std::bind(&InventorEx::twoCube, this)},
        {"pickAction", std::bind(&InventorEx::pickAction, this)},
        {"glCallback", std::bind(&InventorEx::glCallback, this)},
        {"oit", std::bind(&InventorEx::oit, this)},
        {"simpleDepthTest", std::bind(&InventorEx::simpleDepthTest, this)},
        {"whyNotRerender", std::bind(&InventorEx::whyNotRerender, this)},
        {"hiddenLine", std::bind(&InventorEx::hiddenLine, this)},
        {"wireframe", std::bind(&InventorEx::wireframe, this)},
        {"pointInCube", std::bind(&InventorEx::pointInCube, this)},
        {"showRotationCenter", std::bind(&InventorEx::showRotationCenter, this)},
        {"colorMaskTest", std::bind(&InventorEx::colorMaskTest, this)},
        {"cylinderIV", std::bind(&InventorEx::cylinder, this)},
        {"previewPointForward", std::bind(&InventorEx::previewPointForward, this)},
        {"twoSideFace", std::bind(&InventorEx::twoSideFace, this)},
        {"cylinderGL", std::bind(&InventorEx::cylinderGL, this)},
        {"deferredRender", std::bind(&InventorEx::deferredRender, this)},
        {"flat", std::bind(&InventorEx::flat, this)},
        {"switchToPathTraversal", std::bind(&InventorEx::switchToPathTraversal, this)},
        {"auxViewport", std::bind(&InventorEx::auxViewport, this)},
        {"actStateOfDelayList", std::bind(&InventorEx::actStateOfDelayList, this)},
        {"traversalPerformance", std::bind(&InventorEx::traversalPerformance, this)},
        {"isDelayRenderNessery", std::bind(&InventorEx::isDelayRenderNessery, this)},
        {"twoSideLightInDelayRender", std::bind(&InventorEx::twoSideLightInDelayRender, this)},
        {"gnomon", std::bind(&InventorEx::gnomon, this)},
        {"depthRange", std::bind(&InventorEx::depthRange, this)},
        {"clearDepthBuffer", std::bind(&InventorEx::clearDepthBuffer, this)},
        {"depthConflict", std::bind(&InventorEx::depthConflict, this)},
        {"modelView", std::bind(&InventorEx::modelView, this)},
        {"twoSideDiscover", std::bind(&InventorEx::twoSideDiscover, this)},
        {"oneSideCorrect", std::bind(&InventorEx::oneSideCorrect, this)},
        {"glTwoSide", std::bind(&InventorEx::glTwoSide, this)},
        {"staticWireframe", std::bind(&InventorEx::staticWireframe, this)},
        {"removeAllChildren", std::bind(&InventorEx::removeAllChildren, this)},
        {"renderImage", std::bind(&InventorEx::renderImage, this)},
        {"refMain", std::bind(&InventorEx::refMain, this)},
        {"bigSphere", std::bind(&InventorEx::bigSphere, this)},
        {"dotLine", std::bind(&InventorEx::dotLine, this)},
        {"dotCircle", std::bind(&InventorEx::dotCircle, this)},
        {"fitPlane", std::bind(&InventorEx::fitPlane, this)},
        {"performance", std::bind(&InventorEx::performance, this)},
        {"pointSet", std::bind(&InventorEx::pointSet, this)},
        {"customPolygonOffset", std::bind(&InventorEx::customPolygonOffset, this)},
        {"lightTest", std::bind(&InventorEx::lightTest, this)},
        {"levelOfDetail", std::bind(&InventorEx::levelOfDetail, this)},
        {"OBB", std::bind(&InventorEx::OBB, this)},
        {"outline", std::bind(&InventorEx::outline, this)},
        {"outputBuffer", std::bind(&InventorEx::outputBuffer, this)},
        {"cylinderFace", std::bind(&InventorEx::cylinderFace, this)},
        {"gravitationalWell", std::bind(&InventorEx::gravitationalWell, this)},
        {"2outline", std::bind(&InventorEx::outline2, this)},
        {"shaderProgram", std::bind(&InventorEx::shaderProgram, this)},
        {"shaderHightlight", std::bind(&InventorEx::shaderHightlight, this)},
        {"zfighting", std::bind(&InventorEx::zfighting, this)},
        {"materialIndex", std::bind(&InventorEx::materialIndex, this)},
        {"divideTransp", std::bind(&InventorEx::divideTransp, this)},
        {"text2", std::bind(&InventorEx::text2, this)},
        {"twoLine", std::bind(&InventorEx::twoLine, this)},
        {"drtest", std::bind(&InventorEx::drtest, this)},
        {"autoZoom", std::bind(&InventorEx::autoZoom, this)},
        {"billBoard", std::bind(&InventorEx::billBoard, this)},
        {"texture", std::bind(&InventorEx::texture, this)},
        {"transTypeNone", std::bind(&InventorEx::transTypeNone, this)},
        {"transLine", std::bind(&InventorEx::transLine, this)},
        {"shaderParam", std::bind(&InventorEx::shaderParam, this)},
        {"ring", std::bind(&InventorEx::ring, this)},
        {"section", std::bind(&InventorEx::section, this)},
        {"getWorldToScreenScale", std::bind(&InventorEx::getWorldToScreenScale, this)},
        {"dynamicCSYS", std::bind(&InventorEx::dynamicCSYS, this)},
        {"meshRect", std::bind(&InventorEx::meshRect, this)},
        // plugin
        {"_loadPickAndWrite1", std::bind(&InventorEx::loadPickAndWrite, this)},
        {"_loadErrorHandle", std::bind(&InventorEx::loadErrorHandle, this)},
        {"_loadGLCallback", std::bind(&InventorEx::loadGLCallback, this)},
        {"_loadBackground", std::bind(&InventorEx::loadBackground, this)},
        {"_loadPickAndWrite2", std::bind(&InventorEx::loadPickAndWrite2, this)},
    };

    m_delayedLoadNames = {
        "_loadPickAndWrite",
        "_loadPickAndWrite2"
    };

    m_app = new QApplication(argc, argv);
    // Initializes Quarter (and implicitly also Coin and Qt
    Quarter::init();
    // Remember to initialize the custom node!
    SoGradientBackground::initClass();
    SoColorMaskElement::initClass();
    SoGLColorMaskElement::initClass();
    SoColorMask::initClass();
    SoSwitchToChild::initClass();
    SoDeferredRender::initClass();
    SoZoomAdaptor::initClass();
    qRegisterMetaType<PickResult>("PickResult");

    m_mainwin = new QMainWindow();
    m_mainwin->resize(WINDOWWIDTH, WINDOWHEIGHT);

    // Create a QuarterWidget for displaying a Coin scene graph
#ifdef USE_NAVIQUARTER
    m_viewer = new NavQuarterWidget::Tf::NavigationWidget(m_mainwin);
    m_viewer->AddTestModel();
#else
    m_viewer = new QuarterWidget(m_mainwin);
    //set default navigation mode file
    m_viewer->setNavigationModeFile();
#endif
    m_mainwin->setCentralWidget(m_viewer);

#ifdef USE_NAVIQUARTER
    m_mainwin->show();
    m_app->exec();
#endif

    // root
    m_root = new SoSeparator;
    m_root->ref();
}

InventorEx::~InventorEx()
{
    delete m_viewer;
    m_viewer = nullptr;

    delete m_mainwin;
    m_mainwin = nullptr;

    if (m_root)
        m_root->unref();

    Quarter::clean();
}

void InventorEx::resetScene()
{
    m_root->removeAllChildren();

    delete m_viewer;
    m_viewer = nullptr;

    delete m_mainwin;
    m_mainwin = nullptr;

    m_mainwin = new QMainWindow();
    m_mainwin->resize(WINDOWWIDTH, WINDOWHEIGHT);
#ifdef USE_NAVIQUARTER
    m_viewer = new NavQuarterWidget::Tf::NavigationWidget(m_mainwin);
#else
    m_viewer = new QuarterWidget(m_mainwin);

    m_viewer->setNavigationModeFile();
#endif
    m_mainwin->setCentralWidget(m_viewer);
}

void InventorEx::run(const std::string& funcName)
{
    std::vector<std::string> matches;

    for (const auto& pair : m_functions)
    {
        if (pair.first.find(funcName) == 0)
        {
            matches.push_back(pair.first);
        }
    }
    if (matches.size() == 0)
    {
        std::cout << "没有找到匹配的接口: " << funcName << std::endl;
        return;
    }
    if (matches.size() > 1)
    {
        std::cout << "找到多个匹配的接口，请选择其中一个:\n";
        for (const auto& match : matches)
        {
            std::cout << match << '\n';
        }
        std::string additionalInput;
        std::getline(std::cin, additionalInput);
        for (auto it = matches.begin(); it != matches.end();)
        {
            if (-1 == it->find(additionalInput))
            {
                it = matches.erase(it);
            }
            else
            {
                ++it;
            }
        }
        if (matches.size() > 1 || matches.size() == 0)
        {
            run(additionalInput);
            return;
        }
    }
    std::cout << matches[0] << std::endl;

    if (m_reset)
    {
        resetScene();
    }
    // isPlugin
    if (0 == matches[0].find("_"))
    {
        if (m_delayedLoadNames.end() != m_delayedLoadNames.find(matches[0]))
        {
            m_delayedLoadPlugins.push_back(m_functions[matches[0]]);
            std::cout << "插件在场景图完成后加载" << std::endl;
        }
        else
        {
            m_functions[matches[0]]();
            std::cout << "插件加载完毕" << std::endl;
        }
        m_reset = false;
    }
    else
    {
        m_functions[matches[0]]();
        m_viewer->setSceneGraph(m_root);
        m_viewer->show();
        for (const auto& delayedPlugin : m_delayedLoadPlugins)
        {
            delayedPlugin();
        }
        m_mainwin->show();
        m_app->exec();
        m_reset = true;
    }
}

const std::map<std::string, std::function<void(void)>>& InventorEx::getFunctions(void) const
{
    return m_functions;
}


// function
void InventorEx::sphere()
{
    //SoClipPlane* clip = new SoClipPlane;
    //clip->plane.setValue(SbPlane(SbVec3f(0, 1, 0), SbVec3f(0, 0, 0)));
    //m_root->addChild(clip);
    SoSphere* sphere = new SoSphere;
    //SoDepthBuffer* depth = new SoDepthBuffer;
    //depth->test.setValue(false);
    //m_root->addChild(depth);
    m_root->addChild(sphere);
}

void InventorEx::cube()
{
    // coin view会给一个随相机位置的默认直射光照，会与下面给定红色的直射光叠加
    SoDirectionalLight* dirLight = new SoDirectionalLight;
    dirLight->direction.setValue(0, -1, -1);
    dirLight->color.setValue(1, 0, 0);
    m_root->addChild(dirLight);

    m_root->addChild(new SoCube);
}

void InventorEx::engineSpin()
{
    // Create a perspective camera and add it to the scene graph
    SoPerspectiveCamera* myCamera = new SoPerspectiveCamera;
    m_root->addChild(myCamera);

    // Add a directional light to the scene graph
    m_root->addChild(new SoDirectionalLight);

    // Add a rotation transformation to the scene graph to rotate the cone
    SoRotationXYZ* myRotXYZ = new SoRotationXYZ;
    m_root->addChild(myRotXYZ);

    // Add a material node with a red color to the scene graph
    SoMaterial* myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0); // Red
    m_root->addChild(myMaterial);

    // Add a cone to the scene graph
    m_root->addChild(new SoCone);

    // Create an engine to rotate the cone
    SoElapsedTime* myCounter = new SoElapsedTime;// 引擎不作为节点
    myRotXYZ->axis = SoRotationXYZ::X; // Rotate about the X-axis
    myRotXYZ->angle.connectFrom(&myCounter->timeOut); // Connect the engine output to the rotation angle

}

void InventorEx::globalFlds()
{
    SoSeparator* time1 = new SoSeparator;

    SoFont* font = new SoFont;
    font->name.setValue("Courier-BoldOblique");
    font->size.setValue(10);
    m_root->addChild(font);

    // Add a material node with a red color to the scene graph
    SoMaterial* myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0); // Red
    myMaterial->transparency = 0.5;
    time1->addChild(myMaterial);

    // Create a Text3 object, and connect to the realTime field
    //SoText3* myText = new SoText3;
    SoText2* myText = new SoText2;
    time1->addChild(myText);
    myText->string.connectFrom(SoDB::getGlobalField("realTime"));
    //SoElapsedTime* myCounter = new SoElapsedTime;
    //myCounter->speed = -1;
    //myText->string.connectFrom(&myCounter->timeOut);

    SoSeparator* time2 = new SoSeparator;
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyleElement::LINES;
    drawStyle->lineWidth = 4;
    time2->addChild(drawStyle);
    SoMaterial* myMaterial2 = new SoMaterial;
    myMaterial2->diffuseColor.setValue(1.0, 1.0, 0.0);
    myMaterial2->transparency = 0.0f;
    time2->addChild(myMaterial2);
    time2->addChild(myText);

    m_root->addChild(time2);
    m_root->addChild(time1);
}

void InventorEx::heartSpline()
{
    // The control points for this curve
    float pts[7][3] = {
        { 4.0, -6.0, 6.0},
        {-4.0,  1.0, 0.0},
        {-1.5,  5.0, -6.0},
        { 0.0,  2.0, -2.0},
        { 1.5,  5.0, -6.0},
        { 4.0,  1.0, 0.0},
        {-4.0, -6.0, 6.0}
    };

    // The knot vector
    float knots[10] = { 1, 2, 3, 4, 5, 5, 6, 7, 8, 9 };

    // Set the draw style of the curve.
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyleElement::FILLED;
    drawStyle->lineWidth = 4;
    m_root->addChild(drawStyle);

    // Define the NURBS curve including the control points and a complexity.
    SoComplexity* complexity = new SoComplexity;
    SoCoordinate3* controlPts = new SoCoordinate3;
    SoNurbsCurve* curve = new SoNurbsCurve;
    complexity->value = /*0.8*/1.0;// 0.5-1.0

    controlPts->point.setValues(0, 7, pts);
    curve->numControlPoints = 7;
    curve->knotVector.setValues(0, 10, knots);

    m_root->addChild(complexity);
    m_root->addChild(controlPts);
    m_root->addChild(curve);

}

void InventorEx::transformOrdering()
{
    // Create two separators, for left and right objects
    SoSeparator* leftSep = new SoSeparator;
    SoSeparator* rightSep = new SoSeparator;
    m_root->addChild(leftSep);
    m_root->addChild(rightSep);

    // Create the transformation nodes
    SoTranslation* leftTranslation = new SoTranslation;
    SoTranslation* rightTranslation = new SoTranslation;
    SoRotationXYZ* myRotation = new SoRotationXYZ;
    SoScale* myScale = new SoScale;

    // Fill in the values for transformations
    leftTranslation->translation.setValue(-1.0, 0.0, 0.0);
    rightTranslation->translation.setValue(1.0, 0.0, 0.0);
    myRotation->angle = M_PI / 2; // 90 degrees
    myRotation->axis = SoRotationXYZ::X;
    myScale->scaleFactor.setValue(2., 1., 3.);

    // Add transforms to the scene for left and right objects
    leftSep->addChild(leftTranslation); // left graph, first translated
    leftSep->addChild(myRotation); // then rotated
    leftSep->addChild(myScale); // finally scaled

    rightSep->addChild(rightTranslation); // right graph, first translated
    rightSep->addChild(myScale); // then scaled
    rightSep->addChild(myRotation); // finally rotated

    // 设置material透明度
    //SoMaterial* myMaterial = new SoMaterial;
    //myMaterial->transparency = 1.0;
    //leftSep->addChild(myMaterial);

    // Read an object from file. (as in example 4.2.Lights)
    SoInput myInput;
    if (!myInput.openFile("../Data/temple.iv"))
        return;
    SoSeparator* fileContents = SoDB::readAll(&myInput);
    if (fileContents == NULL)
        return;

    // Add an instance of the object under each separator
    leftSep->addChild(fileContents);
    rightSep->addChild(fileContents);

}

void InventorEx::cameras()
{
    // Create a blinker node and put it in the scene. A blinker
    // switches between its children at timed intervals.
    SoBlinker* myBlinker = new SoBlinker;
    myBlinker->speed = 0.2;
    m_root->addChild(myBlinker);

    // Create three cameras. Their positions will be set later.
    // This is because the viewAll method depends on the size
    // of the render area, which has not been created yet.
    SoOrthographicCamera* orthoViewAll = new SoOrthographicCamera;
    SoPerspectiveCamera* perspViewAll = new SoPerspectiveCamera;
    SoPerspectiveCamera* perspOffCenter = new SoPerspectiveCamera;
    myBlinker->addChild(orthoViewAll);
    myBlinker->addChild(perspViewAll);
    myBlinker->addChild(perspOffCenter);

    // Create a light
    m_root->addChild(new SoDirectionalLight);

    // Read the object from a file and add to the scene
    SoInput myInput;
    if (!myInput.openFile("../Data/parkbench.iv"))
        exit(1);
    SoSeparator* fileContents = SoDB::readAll(&myInput);
    if (fileContents == NULL)
        exit(1);

    SoMaterial* myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.8, 0.23, 0.03);
    m_root->addChild(myMaterial);
    m_root->addChild(fileContents);

    // Establish camera positions. 
    // First do a viewAll on all three cameras.  
    // Then modify the position of the off-center camera.
    SbViewportRegion myRegion(m_viewer->getSoRenderManager()->getViewportRegion());
    orthoViewAll->viewAll(m_root, myRegion);
    perspViewAll->viewAll(m_root, myRegion);
    perspOffCenter->viewAll(m_root, myRegion);
    SbVec3f initialPos;
    initialPos = perspOffCenter->position.getValue();
    float x, y, z;
    initialPos.getValue(x, y, z);
    perspOffCenter->position.setValue(x + x / 2., y + y / 2., z + z / 4.);

}

void InventorEx::lights()
{
    // Add a directional light
    SoDirectionalLight* myDirLight = new SoDirectionalLight;
    myDirLight->direction.setValue(0, -1, -1);
    myDirLight->color.setValue(1, 0, 0);
    m_root->addChild(myDirLight);

    // Put the shuttle and the light below a transform separator.
    // A transform separator pushes and pops the transformation 
    // just like a separator node, but other aspects of the state 
    // are not pushed and popped. So the shuttle's translation 
    // will affect only the light. But the light will shine on 
    // the rest of the scene.
    SoTransformSeparator* myTransformSeparator = new SoTransformSeparator;
    m_root->addChild(myTransformSeparator);

    // A shuttle node translates back and forth between the two
    // fields translation0 and translation1.  
    // This moves the light.
    SoShuttle* myShuttle = new SoShuttle;
    myTransformSeparator->addChild(myShuttle);
    myShuttle->translation0.setValue(-2, -1, 3);
    myShuttle->translation1.setValue(1, 2, -3);

    // Add the point light below the transformSeparator
    SoPointLight* myPointLight = new SoPointLight;
    myTransformSeparator->addChild(myPointLight);
    myPointLight->color.setValue(0, 1, 0);

    m_root->addChild(new SoCone);

    m_viewer->setHeadlightEnabled(false);

}

void InventorEx::referenceCount()
{
    SoSeparator* P = new SoSeparator;
    SoSeparator* Q = new SoSeparator;
    SoSeparator* R = new SoSeparator;
    SoSeparator* S = new SoSeparator;


    P->addChild(Q);
    P->addChild(R);
    Q->addChild(S);
    R->addChild(S);

    int refCount[4] = { 0 };
    refCount[0] = P->getRefCount();
    refCount[1] = Q->getRefCount();
    refCount[2] = R->getRefCount();
    refCount[3] = S->getRefCount();
    for (int i = 0; i < 4; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl;

    P->ref();
    refCount[0] = P->getRefCount();
    refCount[1] = Q->getRefCount();
    refCount[2] = R->getRefCount();
    refCount[3] = S->getRefCount();
    for (int i = 0; i < 4; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl;

    std::cout << "0 for unref\n1 for unrefNoDelete\n2 for removeAllChildren\n3 for removeChild" << std::endl;
    int isDelete = 0;
    std::cin >> isDelete;
    switch (isDelete)
    {
        case 0:
            P->unref();
            break;
        case 1:
            P->unrefNoDelete();
            break;
        case 2:
            P->removeAllChildren();
            break;
        case 3:
            P->removeChild(Q);
            break;

    }
    refCount[0] = P->getRefCount();
    refCount[1] = Q->getRefCount();
    refCount[2] = R->getRefCount();
    refCount[3] = S->getRefCount();
    for (int i = 0; i < 4; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl << std::endl;
}

void InventorEx::indexedFaceSet()
{
    float vertexPositions[12][3] = {
        {0.0000, 1.2142, 0.7453},
        {0.0000, 1.2142, -0.7453},
        {-1.2142, 0.7453, 0.0000},
        {-0.7453, 0.0000, 1.2142},
        {0.7453, 0.0000, 1.2142},
        {1.2142, 0.7453, 0.0000},
        {0.0000, -1.2142, 0.7453},
        {-1.2142, -0.7453, 0.0000},
        {-0.7453, 0.0000, -1.2142},
        {0.7453, 0.0000, -1.2142},
        {1.2142, -0.7453, 0.0000},
        {0.0000, -1.2142, -0.7453}
    };

    int32_t indices[72] = {
        1, 2, 3, 4, 5, SO_END_FACE_INDEX,
        0, 1, 8, 7, 3, SO_END_FACE_INDEX,
        0, 2, 7, 6, 4, SO_END_FACE_INDEX,
        0, 3, 6,10, 5, SO_END_FACE_INDEX,
        0, 4,10, 9, 1, SO_END_FACE_INDEX,
        0, 5, 9, 8, 2, SO_END_FACE_INDEX,
        9, 5, 4, 6, 11, SO_END_FACE_INDEX,
       10, 4, 3, 7, 11, SO_END_FACE_INDEX,
        6, 3, 2, 8, 11, SO_END_FACE_INDEX,
        7, 2, 1, 9, 11, SO_END_FACE_INDEX,
        8, 1, 5,10, 11, SO_END_FACE_INDEX,
        6, 7, 8, 9, 10, SO_END_FACE_INDEX
    };

    float colors[12][3] = {
        {1.0, .0, 0}, {.0, .0, 1.0}, {0, .7, .7}, {.0, 1.0, 0},
        {.7, .7, 0}, {.7, .0, .7}, {0, .0, 1.0}, {.7, .0, .7},
        {.7, .7, 0}, {.0, 1.0, .0}, {0, .7, .7}, {1.0, .0, 0}
    };

    float transp[12] = {
    0.0f, 0.1f, 0.2f, 0.3f,
    0.4f, 0.5f, 0.6f, 0.7f,
    0.8f, 0.9f, 1.0f, 1.0f
    };


#ifdef IV_STRICT
    // This is the preferred code for Inventor 2.1

    // Using the new SoVertexProperty node is more efficient
    SoVertexProperty* myVertexProperty = new SoVertexProperty;

    // Define colors for the faces
    for (int i = 0; i < 12; i++)
        myVertexProperty->orderedRGBA.set1Value(i, SbColor(colors[i]).getPackedValue());
    myVertexProperty->materialBinding = SoMaterialBinding::PER_FACE;

    // Define coordinates for vertices
    myVertexProperty->vertex.setValues(0, 12, vertexPositions);

    // Define the IndexedFaceSet, with indices into
    // the vertices:
    SoIndexedFaceSet* myFaceSet = new SoIndexedFaceSet;
    myFaceSet->coordIndex.setValues(0, 72, indices);

    myFaceSet->vertexProperty.setValue(myVertexProperty);
    m_root->addChild(myFaceSet);

#else
    //SoDepthBuffer* depthBuffer = new SoDepthBuffer;
    //depthBuffer->test = false;
    //m_root->addChild(depthBuffer);
    // Define colors for the faces
    SoMaterial* myMaterials = new SoMaterial;
    myMaterials->diffuseColor.setValues(0, 12, colors);
    myMaterials->transparency.setValues(0, 12, transp);
    m_root->addChild(myMaterials);
    SoMaterialBinding* myMaterialBinding = new SoMaterialBinding;
    myMaterialBinding->value = SoMaterialBinding::PER_FACE;
    m_root->addChild(myMaterialBinding);

    // Define coordinates for vertices
    SoCoordinate3* myCoords = new SoCoordinate3;
    myCoords->point.setValues(0, 12, vertexPositions);
    m_root->addChild(myCoords);

    // Define the IndexedFaceSet, with indices into
    // the vertices:
    SoIndexedFaceSet* myFaceSet = new SoIndexedFaceSet;
    myFaceSet->coordIndex.setValues(0, 72, indices);
    m_root->addChild(myFaceSet);
#endif
}

void InventorEx::indexedLineSet()
{
    SoSeparator* sceneRoot = new SoSeparator;

    float cubeVertices[8][3] = {
        {0,0,0}, // v0
        {1,0,0}, // v1
        {1,1,0}, // v2
        {0,1,0}, // v3
        {0,0,1}, // v4
        {1,0,1}, // v5
        {1,1,1}, // v6
        {0,1,1}  // v7
    };

    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.setValues(0, 8, cubeVertices);
    sceneRoot->addChild(coords);

    int32_t cubeEdges[24] = {
        0, 1, 2, 3, 0, SO_END_LINE_INDEX,
        4, 5, 6, 7, 4, SO_END_LINE_INDEX,
        0, 4, SO_END_LINE_INDEX,
        1, 5, SO_END_LINE_INDEX,
        2, 6, SO_END_LINE_INDEX,
        3, 7, SO_END_LINE_INDEX
    };

    float edgeColors[6][3] = {
        {1,0,0},   // red
        {0,1,0},   // green
        {0,0,1},   // blue
        {1,1,0},   // yellow
        {1,0,1},   // magenta
        {0,1,1}   // cyan
    };

    // 建立 SoMaterial
    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValues(0, 6, edgeColors);
    sceneRoot->addChild(material);

    //---------------------------------
    // 4. 绑定方式: PER_FACE_INDEXED
    //---------------------------------
    // 对 IndexedLineSet 而言, PER_FACE / PER_PART 都表示
    // "按线段(从-1到-1) 绑定材质"
    // 这里只是名字不同
    SoMaterialBinding* matBinding = new SoMaterialBinding;
    matBinding->value = SoMaterialBinding::PER_FACE_INDEXED;
    // 若你想尝试 PER_PART_INDEXED, 就换成:
    //matBinding->value = SoMaterialBinding::PER_PART_INDEXED;
    sceneRoot->addChild(matBinding);

    //---------------------------------
    // 5. 建立 SoIndexedLineSet
    //---------------------------------
    SoIndexedLineSet* ils = new SoIndexedLineSet;
    // 顶点索引
    ils->coordIndex.setValues(0, 24, cubeEdges);

    // 决定材质索引 (每条边一个)
    // 所以一共12条线, 我们就写12个: 0..11
    int32_t matIndex[6];
    for (int i = 0; i < 6; i++) {
        matIndex[i] = i; // edge i用 colors[i]
    }
    ils->materialIndex.setValues(0, 6, matIndex);

    sceneRoot->addChild(ils);

    sceneRoot->ref();
    SoReorganizeAction reorganizeAct;
    reorganizeAct.apply(sceneRoot);

    SoWriteAction writeAct;
    writeAct.getOutput()->openFile("C:/Users/Admin/Documents/WorkingSpace/indexedLineSet.iv");
    writeAct.getOutput()->setBinary(FALSE);
    writeAct.apply(sceneRoot);
    writeAct.getOutput()->closeFile();
    sceneRoot->unrefNoDelete();

    // 把sceneRoot加进你主场景
    m_root->addChild(sceneRoot);
}

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
                └── staticWireFrame (SoSeparator)
                    ├── offset (SoPolygonOffset)
                    ├── faceRoot (SoSeparator)
                    |   ├── faceMaterial (SoMaterial)
                    |   └── faceSet (SoIndexedFaceSet)
                    ├── lineVisbleRoot (SoSeparator)
                    |   ├── lineMaterial (SoMaterial)
                    |   └── lineVisibleSet (SoIndexedLineSet)
                    └── lineHiddenRoot (SoSeparator)
                        └── lineHiddenSet (SoIndexedLineSet)
*/
void InventorEx::cubeBehind(SoSeparator* root)
{
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
    SoSwitch* bodySwitch = new SoSwitch;
    SoSeparator* body = new SoSeparator;
    SoScale* scale = new SoScale;
    SoSwitch* trasparencyTypeSwitch = new SoSwitch;
    SoTransparencyType* trasparencyType = new SoTransparencyType;
    SoSeparator* dataNode = new SoSeparator;
    SoCoordinate3* coords = new SoCoordinate3;
    SoSwitch* renderModeSwitch = new SoSwitch;
    SoSeparator* staticWireFrame = new SoSeparator;
    SoSeparator* faceRoot = new SoSeparator;
    SoSeparator* lineVisbleRoot = new SoSeparator;
    SoSeparator* lineHiddenRoot = new SoSeparator;
    SoPolygonOffset* offset = new SoPolygonOffset;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedLineSet* lineVisibleSet = new SoIndexedLineSet;
    SoIndexedLineSet* lineHiddenSet = new SoIndexedLineSet;
    SoMaterial* faceMaterial = new SoMaterial;
    SoMaterial* lineMaterial = new SoMaterial;
    SoCallback* colorOff = new SoCallback;
    SoCallback* colorOn = new SoCallback;

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
    renderModeSwitch->addChild(staticWireFrame);
    staticWireFrame->addChild(offset);// 虽说不是属性节点，但仍需要放在形体节点左边才起效果
    staticWireFrame->addChild(faceRoot);
    staticWireFrame->addChild(lineVisbleRoot);
    staticWireFrame->addChild(lineHiddenRoot);
    faceRoot->addChild(faceMaterial);
    //faceRoot->addChild(colorOff);
    faceRoot->addChild(faceSet);
    //faceRoot->addChild(colorOn);
    lineVisbleRoot->addChild(lineMaterial);
    lineVisbleRoot->addChild(lineVisibleSet);
    lineHiddenRoot->addChild(lineHiddenSet);

    dataNode->setName(SbName("DataNode"));

    bodySwitch->whichChild = 0;// 默认-1
    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 0;

    faceMaterial->setName(SbName("FaceMaterial"));
    faceMaterial->diffuseColor.setValue(1.0, 1.0, 0.0);
    faceMaterial->transparency = 0.0;
    lineMaterial->diffuseColor.setValue(226, 171, 137);

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    lineVisibleSet->coordIndex.setValues(0, 24, lineIndices);

    //colorOff->setCallback(disableColorMask);
    //colorOn->setCallback(enableColorMask);

}

void InventorEx::cubeFront(SoSeparator* root)
{
    cubeBehind(root);
    SoSeparator* dataNode = (SoSeparator*)SoNode::getByName(SbName("DataNode"));// Returns the last node that was registered under name.
    // transform
    SoTransform* transform = new SoTransform;
    dataNode->insertChild(transform, 0);
    transform->translation.setValue({ 0.5, 0.5, 0.5 });
}

void InventorEx::twoCube()
{
    SoExtSelection* extSelection = new SoExtSelection;
    m_root->addChild(extSelection);
    // shape
    cubeBehind(extSelection);
    SoMaterial* faceMaterial = (SoMaterial*)SoNode::getByName(SbName("FaceMaterial"));
    faceMaterial->diffuseColor.setValue(1.0, 1.0, 0.0);
    faceMaterial->transparency = 0.0;
    cubeFront(extSelection);
    faceMaterial = (SoMaterial*)SoNode::getByName(SbName("FaceMaterial"));
    faceMaterial->diffuseColor.setValue(0.0, 1.0, 1.0);
    faceMaterial->transparency = 0.0;


}

SbBool writePickedPath(SoNode* root, const SbViewportRegion& viewport, const SbVec2s& cursorPosition)
{
    SoRayPickAction pickAction(viewport);

    // Set an 8-pixel wide region around the pixel
    pickAction.setPoint(cursorPosition);
    pickAction.setRadius(8.0);// only influence the pick operation's behavior versus lines and points, and has no effect on picking of shapes / polygons.

    // Start a pick traversal
    pickAction.apply(root);
    const SoPickedPoint* pickedPoint = pickAction.getPickedPoint();
    if (pickedPoint == NULL)
        return FALSE;

    // Write out the path to the picked object
    SoWriteAction writeAction;
    writeAction.apply(pickedPoint->getPath());// 总会报warning，不知道哪里有问题，用SoDebugError接住

    return TRUE;
}

// This routine is called for every mouse button event.
void mousePressCB(void* userData, SoEventCallback* eventCB)
{
    SoSeparator* root = (SoSeparator*)userData;
    const SoEvent* event = eventCB->getEvent();

    // Check for mouse button being pressed
    if (SO_MOUSE_PRESS_EVENT(event, ANY))
    {
        const SbViewportRegion& region = eventCB->getAction()->getViewportRegion();
        writePickedPath(root, region, event->getPosition(region));
        eventCB->setHandled();
    }
}

void InventorEx::loadPickAndWrite()
{
    SoEventCallback* eventCB = new SoEventCallback;
    m_root->addChild(eventCB);
    eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressCB, m_viewer->getSoRenderManager()->getSceneGraph());
}

void errorHandlerCB(const SoError* err, void* data)
{
    std::cerr << "Caught error: " << err->getDebugString().getString() << std::endl;
}

void InventorEx::loadErrorHandle()
{
    SoDebugError::setHandlerCallback(errorHandlerCB, NULL);
}

void InventorEx::pickAction()
{

    // Read object data from a file
    SoInput sceneInput;
    if (!sceneInput.openFile("../Data/star.iv"))
        exit(1);
    SoSeparator* starObject = SoDB::readAll(&sceneInput);
    if (starObject == NULL)
        exit(1);
    sceneInput.closeFile();

    // Add two copies of the star object, one white and one red
    SoRotationXYZ* rotation = new SoRotationXYZ;
    rotation->axis.setValue(SoRotationXYZ::X);
    rotation->angle.setValue(M_PI / 2.2);// almost 90 degrees
    m_root->addChild(rotation);

    m_root->addChild(starObject);// first star object

    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValue(1.0, 0.0, 0.0);// red
    m_root->addChild(material);
    SoTranslation* translation = new SoTranslation;
    translation->translation.setValue(1., 0., 1.);
    m_root->addChild(translation);
    m_root->addChild(starObject);// second star object

    // Turn off viewing to allow picking
    // todo
    // 提前设置viewer
    m_viewer->setSceneGraph(m_root);
    m_viewer->show();

    // Add an event callback to catch mouse button presses.
    // The callback is set up later on.
    SoEventCallback* eventCB = new SoEventCallback;
    m_root->addChild(eventCB);// 加到最右边也起效
    // Set up the event callback. We want to pass the root of the
    // entire scene graph (including the camera) as the userData,
    // so we get the scene manager's version of the scene graph
    // root.
    eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId()/*eventtype*/, mousePressCB, m_viewer->getSoRenderManager()->getSceneGraph()/*可以获取camera，但必须先setSceneGraph*/);
}

float floorObj[81][3];
QuarterWidget* renderViewer = nullptr;
SoPerspectiveCamera* renderCamera;
// Build a scene with two objects and some light
void InventorEx::buildScene(SoSeparator* root)
{
    // Some light
    root->addChild(new SoLightModel);
    root->addChild(new SoDirectionalLight);

    // A red cube translated to the left and down
    SoTransform* myTrans = new SoTransform;
    myTrans->translation.setValue(-2.0, -2.0, 0.0);
    root->addChild(myTrans);

    SoMaterial* myMtl = new SoMaterial;
    myMtl->diffuseColor.setValue(1.0, 0.0, 0.0);
    root->addChild(myMtl);

    root->addChild(new SoCube);

    // A blue sphere translated right
    myTrans = new SoTransform;
    myTrans->translation.setValue(4.0, 0.0, 0.0);
    root->addChild(myTrans);

    myMtl = new SoMaterial;
    myMtl->diffuseColor.setValue(0.0, 0.0, 1.0);
    root->addChild(myMtl);

    root->addChild(new SoSphere);
}
template <class Type>
inline void CoinSwap(Type& a, Type& b) { Type t = a; a = b; b = t; }

// Bitmap representations of an "X", a "Y" and a "Z" for the axis cross.
static GLubyte xbmp[] = { 0x11,0x11,0x0a,0x04,0x0a,0x11,0x11 };
static GLubyte ybmp[] = { 0x04,0x04,0x04,0x04,0x0a,0x11,0x11 };
static GLubyte zbmp[] = { 0x1f,0x10,0x08,0x04,0x02,0x01,0x1f };

void drawArrow(void)
{
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glDisable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0, 0, 0);
    glVertex3f(1.0 - 1.0 / 3, +0.5 / 4, 0);
    glVertex3f(1.0 - 1.0 / 3, -0.5 / 4, 0);
    glVertex3f(1.0, 0, 0);
    glVertex3f(1.0 - 1.0 / 3, 0, +0.5 / 4);
    glVertex3f(1.0 - 1.0 / 3, 0, -0.5 / 4);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(1.0 - 1.0 / 3, +0.5 / 4, 0);
    glVertex3f(1.0 - 1.0 / 3, 0, +0.5 / 4);
    glVertex3f(1.0 - 1.0 / 3, -0.5 / 4, 0);
    glVertex3f(1.0 - 1.0 / 3, 0, -0.5 / 4);
    glEnd();
} // drawArrow()

void drawAxisCross(void)
{
    // Store GL state.
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    GLfloat depthrange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthrange);
    GLdouble projectionmatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glDepthRange(0, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND); // Kills transparency.

    // Set the viewport in the OpenGL canvas. Dimensions are calculated
    // as a percentage of the total canvas size.
    //SbVec2s view = viewer->getGLSize();
    SbVec2s view = renderViewer->getSoRenderManager()->getSize();
    const int pixelarea =
        //int(float(this->axiscrossSize)/100.0f * So@Gui@Min(view[0], view[1]));
        int(25.0 / 100.0f * std::min(view[0], view[1]));
#if 0 // middle of canvas
    SbVec2s origin(view[0] / 2 - pixelarea / 2, view[1] / 2 - pixelarea / 2);
#endif // middle of canvas
#if 1 // lower right of canvas
    SbVec2s origin(view[0] - pixelarea, 0);
#endif // lower right of canvas
    glViewport(origin[0], origin[1], pixelarea, pixelarea);

    // Set up the projection matrix.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float NEARVAL = 0.1f;
    const float FARVAL = 10.0f;
    const float dim = NEARVAL * tan(M_PI / 8.0f); // FOV is 45?(45/360 = 1/8)
    glFrustum(-dim, dim, -dim, dim, NEARVAL, FARVAL);


    // Set up the model matrix.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    SbMatrix mx;
    SoCamera* cam = renderCamera;

    // If there is no camera (like for an empty scene, for instance),
    // just use an identity rotation.
    if (cam) { mx = cam->orientation.getValue(); }
    else { mx = SbMatrix::identity(); }

    mx = mx.inverse();
    mx[3][2] = -3.5; // Translate away from the projection point (along z axis).
    glLoadMatrixf((float*)mx);


    // Find unit vector end points.
    SbMatrix px;
    glGetFloatv(GL_PROJECTION_MATRIX, (float*)px);
    SbMatrix comb = mx.multRight(px);

    SbVec3f xpos;
    comb.multVecMatrix(SbVec3f(1, 0, 0), xpos);
    xpos[0] = (1 + xpos[0]) * view[0] / 2;
    xpos[1] = (1 + xpos[1]) * view[1] / 2;
    SbVec3f ypos;
    comb.multVecMatrix(SbVec3f(0, 1, 0), ypos);
    ypos[0] = (1 + ypos[0]) * view[0] / 2;
    ypos[1] = (1 + ypos[1]) * view[1] / 2;
    SbVec3f zpos;
    comb.multVecMatrix(SbVec3f(0, 0, 1), zpos);
    zpos[0] = (1 + zpos[0]) * view[0] / 2;
    zpos[1] = (1 + zpos[1]) * view[1] / 2;


    // Render the cross.
    {
        glLineWidth(2.0);

        enum { XAXIS, YAXIS, ZAXIS };
        int idx[3] = { XAXIS, YAXIS, ZAXIS };
        float val[3] = { xpos[2], ypos[2], zpos[2] };

        // Bubble sort.. :-}
        if (val[0] < val[1])
        {
            CoinSwap(val[0], val[1]); CoinSwap(idx[0], idx[1]);
        }
        if (val[1] < val[2])
        {
            CoinSwap(val[1], val[2]); CoinSwap(idx[1], idx[2]);
        }
        if (val[0] < val[1])
        {
            CoinSwap(val[0], val[1]); CoinSwap(idx[0], idx[1]);
        }

        for (int i = 0; i < 3; i++)
        {
            glPushMatrix();
            if (idx[i] == XAXIS)
            {                       // X axis.
                glColor3f(0.500f, 0.125f, 0.125f);
            }
            else if (idx[i] == YAXIS)
            {                // Y axis.
                glRotatef(90, 0, 0, 1);
                glColor3f(0.125f, 0.500f, 0.125f);
            }
            else
            {                                     // Z axis.
                glRotatef(-90, 0, 1, 0);
                glColor3f(0.125f, 0.125f, 0.500f);
            }
            drawArrow();
            glPopMatrix();
        }
    }

    // Render axis notation letters ("X", "Y", "Z").
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, view[0], 0, view[1], -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLint unpack;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glColor3fv(SbVec3f(0.8f, 0.8f, 0.0f).getValue());

    glRasterPos2d(xpos[0], xpos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, xbmp);
    glRasterPos2d(ypos[0], ypos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, ybmp);
    glRasterPos2d(zpos[0], zpos[1]);
    glBitmap(8, 7, 0, 0, 0, 0, zbmp);

    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
    glPopMatrix();

    // Reset original state.

    // FIXME: are these 3 lines really necessary, as we push
    // GL_ALL_ATTRIB_BITS at the start? 20000604 mortene.
    glDepthRange(depthrange[0], depthrange[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projectionmatrix);

    glPopAttrib();
} // drawAxisCross()

// Build the floor that will be rendered using OpenGL.
void InventorEx::buildFloor()
{
    int a = 0;

    for (float i = -5.0; i <= 5.0; i += 1.25)
    {
        for (float j = -5.0; j <= 5.0; j += 1.25, a++)
        {
            floorObj[a][0] = j;
            floorObj[a][1] = 0.0;
            floorObj[a][2] = i;
        }
    }
}

// Draw the lines that make up the floor, using OpenGL
void drawFloor()
{
    int i;

    glBegin(GL_LINES);
    for (i = 0; i < 4; i++)
    {
        glVertex3fv(floorObj[i * 18]);
        glVertex3fv(floorObj[(i * 18) + 8]);
        glVertex3fv(floorObj[(i * 18) + 17]);
        glVertex3fv(floorObj[(i * 18) + 9]);
    }

    glVertex3fv(floorObj[i * 18]);
    glVertex3fv(floorObj[(i * 18) + 8]);
    glEnd();

    glBegin(GL_LINES);
    for (i = 0; i < 4; i++)
    {
        glVertex3fv(floorObj[i * 2]);
        glVertex3fv(floorObj[(i * 2) + 72]);
        glVertex3fv(floorObj[(i * 2) + 73]);
        glVertex3fv(floorObj[(i * 2) + 1]);
    }
    glVertex3fv(floorObj[i * 2]);
    glVertex3fv(floorObj[(i * 2) + 72]);
    glEnd();
}

// Callback routine to render the floor using OpenGL
void callbackRoutine(void*, SoAction* action)
{
    // only render the floor during GLRender actions:
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
        return;

    s_renderCountForDebug++;
    glPushMatrix();
    glTranslatef(0.0f, -3.0f, 0.0f);
    glColor3f(0.0f, 0.7f, 0.0f);
    glLineWidth(2);
    glDisable(GL_LIGHTING);  // so we don't have to set normals
    drawFloor();
    drawAxisCross();
    glEnable(GL_LIGHTING);
    glLineWidth(1);
    glPopMatrix();

    //With Inventor 2.1, it's necessary to reset SoGLLazyElement after
    //making calls (such as glColor3f()) that affect material state.
    //In this case, the diffuse color and light model are being modified,
    //so the logical-or of DIFFUSE_MASK and LIGHT_MODEL_MASK is passed 
    //to SoGLLazyElement::reset().  
    //Additional information can be found in the publication
    // "Open Inventor 2.1 Porting and Performance Tips"

    /*
    SoLazyElement类：
    SoLazyElement类用于处理材质和形状的属性。与其名称相对应，SoGLLazyElement是一个在将内容发送到OpenGL时懒惰的元素。
    除非调用SoGLLazyElement::send()，否则更改不会被发送到OpenGL。这意味着你可以多次改变某些属性的状态，但状态只会被发送到OpenGL一次。
    创建Coin中的新形状节点时：
    当你在Coin中创建一个新的形状节点时，修改OpenGL的散射颜色是一个常见的操作。你可以使用几种方法将颜色发送到OpenGL。
    1.如果你不打算在节点外部使用颜色，你可以使用纯OpenGL发送它。
    2.你还可以在元素中设置颜色，然后使用SoGLLazyElement::send(state, SoLazyElement::DIFFUSE_MASK)强制发送。
    3.但是，当创建一个扩展形状节点时，建议在堆栈上创建一个SoMaterialBundle实例。
    如果在使用新颜色更新SoLazyElement之后创建此实例，则在调用SoMaterialBundle::sendFirst()时，新颜色将被发送到OpenGL。
    这次调用还会更新所有其他懒惰的OpenGL状态。
    实际上，创建形状节点时，必须使用SoMaterialBundle::sendFirst()或调用SoGLLazyElement::send(state, SoLazyElement::ALL_MASK)。
    */
    // 如果你决定使用glColor*()将颜色发送到OpenGL，你应该通过调用SoGLLazyElement::reset(state, SoLazyElement::DIFFUSE_MASK)通知SoGLLazyElement。
    // 这将通知SoGLLazyElement当前的OpenGL散射颜色未知。
    SoState* state = action->getState();
    SoGLLazyElement* lazyElt = (SoGLLazyElement*)SoLazyElement::getInstance(state);
    lazyElt->reset(state, (SoLazyElement::DIFFUSE_MASK) | (SoLazyElement::LIGHT_MODEL_MASK));

}

void InventorEx::glCallback()
{
    // Initialize Inventor utilities
    buildFloor();

    // Build a simple scene graph, including a camera and
    // a SoCallback node for performing some GL rendering.

    //SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
    renderCamera = new SoPerspectiveCamera;
    renderCamera->position.setValue(0.0f, 0.0f, 5.0f);
    renderCamera->heightAngle = (float)(M_PI / 2.0f);  // 90 degrees
    renderCamera->nearDistance = 2.0f;
    renderCamera->farDistance = 12.0f;
    m_root->addChild(renderCamera);

    SoCallback* callback = new SoCallback;
    callback->setCallback(callbackRoutine);
    m_root->addChild(callback);

    buildScene(m_root);

    // Initialize an Inventor Win RenderArea and draw the scene.
#ifndef USE_NAVIQUARTER
    renderViewer = m_viewer;
#endif
    m_viewer->setBackgroundColor(QColor(0.8f, 0.8f, 0.8f));
}

void InventorEx::oit()
{

}

void InventorEx::loadGLCallback()
{
    buildFloor();

    renderCamera = new SoPerspectiveCamera;
    renderCamera->position.setValue(0.0f, 0.0f, 5.0f);
    renderCamera->heightAngle = (float)(M_PI / 2.0f);  // 90 degrees
    renderCamera->nearDistance = 2.0f;
    renderCamera->farDistance = 12.0f;
    m_root->addChild(renderCamera);

    SoCallback* callback = new SoCallback;
    callback->setCallback(callbackRoutine);
    m_root->addChild(callback);
#ifndef USE_NAVIQUARTER
    renderViewer = m_viewer;
#endif
}

void InventorEx::loadBackground()
{
    // create a gradient background
    SoSeparator* background = new SoSeparator;
    SoBaseColor* color = new SoBaseColor;
    SoOrthographicCamera* orthocam = new SoOrthographicCamera;

    color->rgb.set1Value(0, SbColor(0.05, 0.05, 0.2)); // 深蓝色
    color->rgb.set1Value(1, SbColor(0.0, 0.0, 0.0));   // 黑色

    orthocam->height.setValue(1.0);
    orthocam->viewportMapping = SoCamera::LEAVE_ALONE;
    orthocam->nearDistance.setValue(0.0);
    orthocam->farDistance.setValue(2.0);
    orthocam->position = SbVec3f(0.0f, 0.0f, 1.0f);

    SoMaterialBinding* mb = new SoMaterialBinding;
    mb->value = SoMaterialBinding::PER_VERTEX_INDEXED;

    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.set1Value(0, SbVec3f(-0.5f, -0.5f, 0.0f));
    coords->point.set1Value(1, SbVec3f(0.5f, -0.5f, 0.0f));
    coords->point.set1Value(2, SbVec3f(0.5f, 0.5f, 0.0f));
    coords->point.set1Value(3, SbVec3f(-0.5f, 0.5f, 0.0f));

    SoIndexedFaceSet* ifs = new SoIndexedFaceSet;
    ifs->coordIndex.set1Value(0, 0);
    ifs->coordIndex.set1Value(1, 1);
    ifs->coordIndex.set1Value(2, 2);
    ifs->coordIndex.set1Value(3, 3);
    ifs->coordIndex.set1Value(4, -1);

    ifs->materialIndex.set1Value(0, 0);
    ifs->materialIndex.set1Value(1, 0);
    ifs->materialIndex.set1Value(2, 1);
    ifs->materialIndex.set1Value(3, 1);
    ifs->materialIndex.set1Value(4, -1);

    SoLightModel* lm = new SoLightModel;
    lm->model = SoLightModel::BASE_COLOR;

    // 画星星
    SoCoordinate3* starCoords = new SoCoordinate3;
    SoPointSet* stars = new SoPointSet;
    const int STAR_COUNT = 200;
    for (int i = 0; i < STAR_COUNT; i++) {
        float x = (rand() % 1000 - 500) / 1000.0f;
        float y = (rand() % 1000 - 500) / 1000.0f;
        starCoords->point.set1Value(i, SbVec3f(x, y, 0.0f));
    }

    // 路灯
    SoMaterial* lightMaterial = new SoMaterial;
    lightMaterial->diffuseColor.setValue(1.0, 0.5, 0.0); // 橙色
    lightMaterial->transparency.setValue(0.7);  // 根据需要调整透明度

    SoCone* lightCone = new SoCone;
    lightCone->bottomRadius.setValue(0.5);
    lightCone->height.setValue(0.5);

    background->addChild(orthocam);
    background->addChild(lm);
    background->addChild(color);
    background->addChild(mb);
    background->addChild(coords);
    background->addChild(ifs);

    // 添加星星
    background->addChild(starCoords);
    background->addChild(stars);

    // 添加路灯的光芒
    background->addChild(lightMaterial);
    background->addChild(lightCone);

    (void)m_viewer->getSoRenderManager()->addSuperimposition(background, SoRenderManager::Superimposition::BACKGROUND);// 前置(void)用于显式地忽略函数返回值
}


void enableColorMask(void*, SoAction* action)
{
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
        return;

    glPopAttrib();
    s_renderCountForDebug++;
};


void disableColorMask(void*, SoAction* action)
{
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
        return;

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    s_renderCountForDebug++;
}


void InventorEx::simpleDepthTest()
{
    float pts[6][3] = {
        // face
        { 0.0, 0.0, 1.0 },
        { 1.0, 0.0, 1.0 },
        { 1.0, 1.0, 1.0 },
        { 0.0, 1.0, 1.0 },
        // line
        { 0.5,-1.0, 0.0 },
        { 0.5, 2.0, 0.0 },
    };
    int32_t faceIndices[8] = {
        0, 1, 2, SO_END_FACE_INDEX,
        0, 2, 3, SO_END_FACE_INDEX,
    };

    int32_t lineIndices[3] = {
        4, 5, SO_END_LINE_INDEX
    };

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    SoCallback* colorOff = new SoCallback;
    SoCallback* colorOn = new SoCallback;

    m_root->addChild(coords);
    m_root->addChild(colorOff);
    m_root->addChild(faceSet);
    m_root->addChild(colorOn);
    m_root->addChild(lineSet);

    coords->point.setValues(0, 6, pts);
    faceSet->coordIndex.setValues(0, 8, faceIndices);
    lineSet->coordIndex.setValues(0, 3, lineIndices);
    colorOff->setCallback(disableColorMask);
    colorOn->setCallback(enableColorMask);

}

void InventorEx::whyNotRerender()
{
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

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    SoCallback* colorOff = new SoCallback;
    SoCallback* colorOn = new SoCallback;
    SoIndexedFaceSet* faceSet2 = new SoIndexedFaceSet;
    SoIndexedLineSet* lineSet2 = new SoIndexedLineSet;
    SoCallback* colorOff2 = new SoCallback;
    SoCallback* colorOn2 = new SoCallback;
    SoTranslation* translation = new SoTranslation;

    m_root->renderCaching = SoSeparator::OFF;
    m_root->addChild(coords);
    m_root->addChild(colorOff);
    m_root->addChild(faceSet);
    m_root->addChild(colorOn);
    m_root->addChild(lineSet);
    m_root->addChild(translation);
    m_root->addChild(colorOff2);
    m_root->addChild(faceSet2);
    m_root->addChild(colorOn2);
    m_root->addChild(lineSet2);

    m_root->addChild(new SoSphere);

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    lineSet->coordIndex.setValues(0, 24, lineIndices);

    colorOff->setCallback(disableColorMask);
    colorOn->setCallback(enableColorMask);

    faceSet2->coordIndex.setValues(0, 48, faceIndices);
    lineSet2->coordIndex.setValues(0, 24, lineIndices);

    colorOff2->setCallback(disableColorMask);
    colorOn2->setCallback(enableColorMask);

    translation->translation.setValue(1.0, 1.0, 1.0);

}

void InventorEx::hiddenLine()
{
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

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    SoIndexedFaceSet* faceSet2 = new SoIndexedFaceSet;
    SoIndexedLineSet* lineSet2 = new SoIndexedLineSet;
    SoTranslation* translation = new SoTranslation;
    SoMaterial* material = new SoMaterial;

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    lineSet->coordIndex.setValues(0, 24, lineIndices);
    translation->translation.setValue(0.5, 0.5, 0.5);
    faceSet2->coordIndex.setValues(0, 48, faceIndices);
    lineSet2->coordIndex.setValues(0, 24, lineIndices);
    material->transparency = 0.5;

    m_root->addChild(new SoGradientBackground);

    SoSeparator* firstPassSeparator = new SoSeparator;
    m_root->addChild(firstPassSeparator);

    // For the first pass, use a ColorMask with all fields to FALSE in order to
    // draw only into the depth buffer
    SoColorMask* colorMask = new SoColorMask;
    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;
    firstPassSeparator->addChild(colorMask);
    // todo: depthOffset is similar to SoPolygonOffset but this node uses a different algorithm
    // based on Lengyel's method from Game Programming Gems and improved by VSG
    SoPolygonOffset* depthOffset = new SoPolygonOffset;
    firstPassSeparator->addChild(depthOffset);
    firstPassSeparator->addChild(coords);
    firstPassSeparator->addChild(material);// 没效果
    firstPassSeparator->addChild(faceSet);
    firstPassSeparator->addChild(translation);
    firstPassSeparator->addChild(faceSet2);

    SoSeparator* secondPassSeparator = new SoSeparator;
    m_root->addChild(secondPassSeparator);

    // For the second pass, draw the model in BASE_COLOR mode and DrawStyle LINES
    SoColorMask* colorMask2 = new SoColorMask;
    secondPassSeparator->addChild(colorMask2);

    SoLightModel* lightModel = new SoLightModel;
    lightModel->model = SoLightModel::BASE_COLOR;
    secondPassSeparator->addChild(lightModel);


    secondPassSeparator->addChild(coords);
    secondPassSeparator->addChild(lineSet);
    secondPassSeparator->addChild(translation);
    secondPassSeparator->addChild(lineSet2);
}

std::vector<InventorEx::InventorEx::ShapeData> InventorEx::generateRandomCuboids(int count, float maxSize)
{
    std::vector<ShapeData> datasets;
    srand(time(NULL));

    for (int i = 0; i < count; ++i) {
        ShapeData data;
        float startX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * maxSize;
        float startY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * maxSize;
        float startZ = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * maxSize;

        float size = 0.5f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.5f; // size between 0.5 and 1.0

        for (int j = 0; j < 8; ++j) {
            float x = startX + ((j & 1) ^ ((j & 2) >> 1) ? size : 0);
            float y = startY + ((j & 2) ? size : 0);
            float z = startZ + ((j & 4) ? size : 0);
            data.points.push_back({ x, y, z });
        }

        data.faceIndices = {
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
        data.lineIndices = {
            0, 1, 2, 3, 0, SO_END_LINE_INDEX,
            4, 5, 6, 7, 4, SO_END_LINE_INDEX,
            0, 4, SO_END_LINE_INDEX,
            1, 5, SO_END_LINE_INDEX,
            2, 6, SO_END_LINE_INDEX,
            3, 7, SO_END_LINE_INDEX
        };

        datasets.push_back(data);
    }

    return datasets;
}

#define CUBECOUNT 20
#define  USEDELAYRENDER
#ifdef USEDELAYRENDER
/*
About polygonOffset:
    With the position of the polygonOffset node being placed before the frameSwitch, the question arises: what's its effective range?
    Typically, it affects the closest shape node. If a separator node is placed behind it, the entire separator node will be influenced by the offset.
    Interestingly, though polygonOffset affects the whole frameSwitch which contains faceRoot and lineRoot both, how does it manage to serve its purpose?
    Notably, since polygon offset doesn't have an impact on non-polygonal geometry, primitives such as SoLineSet and SoPointSet are unaffected by SoPolygonOffset().

bodySwitch
│
└── body
    ├── scale
    ├── trasparencyTypeSwitch
    │   └── trasparencyType
    └── dataNode
        ├── coords
        └── renderModeSwitch
            ├── shadeWithEdge
            ├── shadeWithoutEdge
            ├── transluency
            ├── staticWireframe
            │   ├── polygonOffset
            │   ├── faceRoot
            │   │   ├── colorMask (SoColorMask)
            │   │   └── faceSet
            │   └── lineRoot (SoDeferredRender)
            │       ├── lineVisibleRoot
            │       │   └── [*]lineSet
            │       └── lineHiddenSwitch
            │           └── lineHiddenRoot
            │               ├── depthbuffer
            │               ├── wireStyleSwitch
            │               │   ├── dashedLinestyle
            │               │   └── dimColor
            │               └── [*]lineSet
            └── wireframeWithoutHidden

    [*]: shared node
*/
SoSwitch* InventorEx::assembleBodyScene(const ShapeData& data)
{
    CREATE_NODE(SoSwitch, bodySwitch)
        CREATE_NODE(SoSeparator, body)
        CREATE_NODE(SoScale, scale)
        CREATE_NODE(SoSwitch, trasparencyTypeSwitch)
        CREATE_NODE(SoTransparencyType, trasparencyType)
        CREATE_NODE(SoSeparator, dataNode)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoSwitch, renderModeSwitch)
        CREATE_NODE(SoSeparator, shadeWithEdge)
        CREATE_NODE(SoSeparator, shadeWithoutEdge)
        CREATE_NODE(SoSeparator, transluency)
        CREATE_NODE(SoSeparator, staticWireframe)
        CREATE_NODE(SoSeparator, wireframeWithoutHidden)
        CREATE_NODE(SoPolygonOffset, polygonOffset)
        CREATE_NODE(SoSeparator, faceRoot)
        CREATE_NODE(SoMaterial, faceMaterial)
        CREATE_NODE(SoColorMask, colorMask)
        CREATE_NODE(SoIndexedFaceSet, faceSet)
        CREATE_NODE(SoDeferredRender, lineRoot)
        CREATE_NODE(SoSeparator, lineVisibleRoot)
        CREATE_NODE(SoMaterial, lineVisibleMaterial)
        CREATE_NODE(SoSwitch, lineHiddenSwitch)
        CREATE_NODE(SoSeparator, lineHiddenRoot)
        CREATE_NODE(SoMaterial, lineHiddenMaterial)
        CREATE_NODE(SoDepthBuffer, depthbuffer)
        CREATE_NODE(SoSwitch, wireStyleSwitch)
        CREATE_NODE(SoDrawStyle, dashedLinestyle)
        CREATE_NODE(SoBaseColor, dimColor)
        CREATE_NODE(SoIndexedLineSet, lineSet)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {bodySwitch, body},
        {body, scale},
        {body, trasparencyTypeSwitch},
        {body, dataNode},
        {trasparencyTypeSwitch, trasparencyType},
        {dataNode, coords},
        {dataNode, renderModeSwitch},
        {renderModeSwitch, shadeWithEdge},
        {renderModeSwitch, shadeWithoutEdge},
        {renderModeSwitch, transluency},
        {renderModeSwitch, staticWireframe},
        {renderModeSwitch, wireframeWithoutHidden},
        {staticWireframe, polygonOffset},
        {staticWireframe, faceRoot},
        {staticWireframe, lineRoot},
        {faceRoot, faceMaterial},
        {faceRoot, colorMask},
        {faceRoot, faceSet},
        {lineRoot, lineVisibleRoot},
        {lineRoot, lineHiddenSwitch},
        {lineVisibleRoot, lineVisibleMaterial},
        {lineVisibleRoot, lineSet},
        {lineHiddenSwitch, lineHiddenRoot},
        {lineHiddenRoot, colorMask},
        {lineHiddenRoot, lineHiddenMaterial},
        {lineHiddenRoot, depthbuffer},
        {lineHiddenRoot, wireStyleSwitch},
        {wireStyleSwitch, dashedLinestyle},
        {wireStyleSwitch, dimColor},
        {lineHiddenRoot, lineSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    bodySwitch->whichChild = 0;
    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 3;

    //faceMaterial->transparency = 1.0;
    lineVisibleMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);

    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;

    depthbuffer->function = SoDepthBuffer::GREATER;// 仅绘制隐藏片段

    dashedLinestyle->linePattern.setValue(0xff00);

    dimColor->rgb.setValue(0.5, 0.5, 0.5);

    coords->point.setValues(0, data.points.size(), reinterpret_cast<const float(*)[3]>(data.points.data()));
    faceSet->coordIndex.setValues(0, data.faceIndices.size(), data.faceIndices.data());
    lineSet->coordIndex.setValues(0, data.lineIndices.size(), data.lineIndices.data());

    return bodySwitch;
}

/*
    "faces are first rendered with #red, #green, #blue and #alpha masks
    to FALSE, so that only depth is written. Then, the shape is rendered in
    wireframe mode, with all masks to TRUE."

Why can't we set color masks during the individual body rendering process?
    Doing so can make the hidden effect dependent on the rendering order of the bodies.
    Consider two cubes as an example: The facets of the first cube are written to the depth buffer,
    followed by the rendering of its edges based on that depth buffer — everything appears correct up to this point.
    However, when the facets of the second cube pass the depth test and write to the depth buffer,
    they don't update the color buffer. As a result, the edges of the first cube aren't obscured by the facets of the second cube.
    Essentially, you cannot hide edges using facets that haven't been rendered yet.

m_root
│
├── SoGradientBackground
│
└── bodies
*/
void InventorEx::wireframe()
{
    std::vector<ShapeData> randomCuboids = generateRandomCuboids(CUBECOUNT/*count*/, 5.0/*size*/);

    CREATE_NODE(SoSeparator, firstPassSeparator)
        CREATE_NODE(SoSeparator, secondPassSeparator)
        CREATE_NODE(SoSeparator, bodies)
        CREATE_NODE(SoSwitchToChild, switchToFacet)
        CREATE_NODE(SoSwitchToChild, switchToEdge)
        CREATE_NODE(SoColorMask, colorMask)
        CREATE_NODE(SoLightModel, lightModel)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        //{m_root, new SoGradientBackground},
        {m_root, bodies},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }
    for (const auto& data : randomCuboids)
    {
        ADD_CHILD(bodies, assembleBodyScene(data));
    }

    std::cout << "-1 for Hidden\n0 for Dashed\n1 for Dim" << std::endl;
    int option = -1;
    std::cin >> option;
    if (-1 != option)
    {
        std::vector<SoSwitch*> switchVec;
        switchVec = searchNodes<SoSwitch>(bodies, "lineHiddenSwitch");
        for (auto& node : switchVec)
        {
            node->whichChild = 0;
        }

        switchVec = searchNodes<SoSwitch>(bodies, "wireStyleSwitch");
        for (auto& node : switchVec)
        {
            node->whichChild = option;
        }
    }
}
#else
/*
bodySwitch
│
└── body
    ├── scale
    ├── trasparencyTypeSwitch
    │   └── trasparencyType
    └── dataNode
        ├── coords
        └── renderModeSwitch
            ├── shadeWithEdge
            ├── shadeWithoutEdge
            ├── transluency
            ├── staticWireframe
            │   ├── polygonOffset
            │   └── frameSwitch
            │       ├── faceRoot
            │       │   ├── faceStyle
            │       │   ├── faceNormal
            │       │   ├── normalBinding
            │       │   ├── materialSwitch
            │       │   │   └── transparentMaterial
            │       │   └── faceSet
            │       └── lineRoot
            │           ├── lineVisibleRoot
            │           │   └── [*]lineSet
            │           └── lineHiddenSwitch
            │               └── lineHiddenRoot
            │                   ├── depthbuffer
            │                   ├── wireStyleSwitch
            │                   │   ├── dashedLinestyle
            │                   │   └── dimColor
            │                   └── [*]lineSet
            └── wireframeWithoutHidden

    [*]: shared node
*/
SoSwitch* InventorEx::assembleBodyScene(const ShapeData& data)
{
    CREATE_NODE(SoSwitch, bodySwitch)
        CREATE_NODE(SoSeparator, body)
        CREATE_NODE(SoScale, scale)
        CREATE_NODE(SoSwitch, trasparencyTypeSwitch)
        CREATE_NODE(SoTransparencyType, trasparencyType)
        CREATE_NODE(SoSeparator, dataNode)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoSwitch, renderModeSwitch)
        CREATE_NODE(SoSeparator, shadeWithEdge)
        CREATE_NODE(SoSeparator, shadeWithoutEdge)
        CREATE_NODE(SoSeparator, transluency)
        CREATE_NODE(SoSeparator, staticWireframe)
        CREATE_NODE(SoSeparator, wireframeWithoutHidden)
        CREATE_NODE(SoSwitch, frameSwitch)
        CREATE_NODE(SoSeparator, faceRoot)
        CREATE_NODE(SoPolygonOffset, polygonOffset)
        CREATE_NODE(SoDrawStyle, faceStyle)
        CREATE_NODE(SoNormal, faceNormal)
        CREATE_NODE(SoNormalBinding, normalBinding)
        CREATE_NODE(SoSwitch, materialSwitch)
        CREATE_NODE(SoMaterial, transparentMaterial)
        CREATE_NODE(SoIndexedFaceSet, faceSet)
        CREATE_NODE(SoSeparator, lineRoot)
        CREATE_NODE(SoSeparator, lineVisibleRoot)
        CREATE_NODE(SoMaterial, lineVisibleMaterial)
        CREATE_NODE(SoSwitch, lineHiddenSwitch)
        CREATE_NODE(SoSeparator, lineHiddenRoot)
        CREATE_NODE(SoColorMask, colorMask)
        CREATE_NODE(SoDepthBuffer, depthbuffer)
        CREATE_NODE(SoSwitch, wireStyleSwitch)
        CREATE_NODE(SoDrawStyle, dashedLinestyle)
        CREATE_NODE(SoBaseColor, dimColor)
        CREATE_NODE(SoIndexedLineSet, lineSet)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {bodySwitch, body},
        {body, scale},
        {body, trasparencyTypeSwitch},
        {body, dataNode},
        {trasparencyTypeSwitch, trasparencyType},
        {dataNode, coords},
        {dataNode, renderModeSwitch},
        {renderModeSwitch, shadeWithEdge},
        {renderModeSwitch, shadeWithoutEdge},
        {renderModeSwitch, transluency},
        {renderModeSwitch, staticWireframe},
        {renderModeSwitch, wireframeWithoutHidden},
        {staticWireframe, polygonOffset},
        {staticWireframe, frameSwitch},
        {frameSwitch, faceRoot},
        {frameSwitch, lineRoot},
        {faceRoot, faceStyle},
        {faceRoot, faceNormal},
        {faceRoot, normalBinding},
        {faceRoot, materialSwitch},
        {faceRoot, faceSet},
        {materialSwitch, transparentMaterial},
        {lineRoot, lineVisibleRoot},
        {lineRoot, lineHiddenSwitch},
        {lineVisibleRoot, lineVisibleMaterial},
        {lineVisibleRoot, lineSet},
        {lineHiddenSwitch, lineHiddenRoot},
        {lineHiddenRoot, colorMask},
        {lineHiddenRoot, depthbuffer},
        {lineHiddenRoot, wireStyleSwitch},
        {wireStyleSwitch, dashedLinestyle},
        {wireStyleSwitch, dimColor},
        {lineHiddenRoot, lineSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    bodySwitch->whichChild = 0;
    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 3;
    frameSwitch->whichChild = 0;// for adaptive view

    lineVisibleMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);
    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;

    depthbuffer->function = SoDepthBuffer::NOTEQUAL;// 仅绘制隐藏片段

    dashedLinestyle->linePattern.setValue(0xff00);

    dimColor->rgb.setValue(0.5, 0.5, 0.5);

    coords->point.setValues(0, data.points.size(), reinterpret_cast<const float(*)[3]>(data.points.data()));
    faceSet->coordIndex.setValues(0, data.faceIndices.size(), data.faceIndices.data());
    lineSet->coordIndex.setValues(0, data.lineIndices.size(), data.lineIndices.data());

    return bodySwitch;
}

/*
m_root
│
├── SoGradientBackground
│
├── firstPassSeparator
│   ├── colorMask
│   ├── switchToFacet
│   └── [*]bodies
│
└── secondPassSeparator
    ├── lightModel
    ├── switchToEdge
    └── [*]bodies

    [*]: shared node
*/
void InventorEx::wireframe()
{
    std::vector<ShapeData> randomCuboids = generateRandomCuboids(CUBECOUNT/*count*/, 5.0/*size*/);

    CREATE_NODE(SoSeparator, firstPassSeparator)
        CREATE_NODE(SoSeparator, secondPassSeparator)
        CREATE_NODE(SoSeparator, bodies)
        CREATE_NODE(SoSwitchToChild, switchToFacet)
        CREATE_NODE(SoSwitchToChild, switchToEdge)
        CREATE_NODE(SoColorMask, colorMask)
        CREATE_NODE(SoLightModel, lightModel)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        //{m_root, new SoGradientBackground},
        {m_root, firstPassSeparator},
        {m_root, secondPassSeparator},
        {firstPassSeparator, colorMask},
        {firstPassSeparator, switchToFacet},
        {firstPassSeparator, bodies},
        {secondPassSeparator, lightModel},
        {secondPassSeparator, switchToEdge},
        {secondPassSeparator, bodies},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }
    for (const auto& data : randomCuboids)
    {
        ADD_CHILD(bodies, assembleBodyScene(data));
    }

    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;

    switchToFacet->switchName = "frameSwitch";
    switchToFacet->toWhichChild = 0;
    switchToFacet->searchRange = bodies;

    switchToEdge->switchName = "frameSwitch";
    switchToEdge->toWhichChild = 1;
    switchToEdge->searchRange = bodies;

    lightModel->model = SoLightModel::BASE_COLOR;// 渲染将只使用当前材质的漫反射颜色和透明度

    std::cout << "-1 for Hidden\n0 for Dashed\n1 for Dim" << std::endl;
    int option = -1;
    std::cin >> option;
    if (-1 != option)
    {
        std::vector<SoSwitch*> switchVec;
        switchVec = searchNodes<SoSwitch>(bodies, "lineHiddenSwitch");
        for (auto& node : switchVec)
        {
            node->whichChild = 0;
        }

        switchVec = searchNodes<SoSwitch>(bodies, "wireStyleSwitch");
        for (auto& node : switchVec)
        {
            node->whichChild = option;
        }
    }
}
#endif

void InventorEx::pointInCube()
{
    CREATE_NODE(SoSeparator, separator)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(/*SoCube*/SoSphere, point)
        CREATE_NODE(SoMaterial, material)
        CREATE_NODE(SoDepthBuffer, depthbuffer)
        CREATE_NODE(SoLightModel, lightModel)
        CREATE_NODE(SoShapeHints, shapeHints)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        //{m_root, cube},
        {m_root, separator},
        {separator, depthbuffer},
        {separator, lightModel},
        {separator, material},
        //{separator, shapeHints},
        {separator, point},
    };

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    cube->width = 10;
    cube->height = 10;
    cube->depth = 10;
    //point->width = 2;
    //point->height = 2;
    //point->depth = 2;
    point->radius = 1.0f;

    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    material->diffuseColor.setValue(1.0, 0.0, 0.0);
    material->specularColor.setValue(1.0, 0.0, 0.0);
    depthbuffer->function = SoDepthBuffer::ALWAYS;
    std::cout << "0 for Base_Color\n1 for Phong" << std::endl;
    int model;
    std::cin >> model;
    lightModel->model = (SoLightModel::Model)model;
}

// tode: 添加反向矩阵
void InventorEx::showRotationCenter()
{
    CREATE_NODE(SoSeparator, separator)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoSphere, point)
        CREATE_NODE(SoDepthBuffer, depthBuffer)
        CREATE_NODE(SoComplexity, complexity)
        CREATE_NODE(SoMaterial, material)
        CREATE_NODE(SoLightModel, lightModel)
        CREATE_NODE(SoShapeHints, shapeHints)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, cube},
        {m_root, separator},
        {separator, depthBuffer},
        {separator, lightModel},
        {separator, material},
        {separator, shapeHints},
        {separator, point},
    };

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    cube->width = 10;
    cube->height = 10;
    cube->depth = 10;
    point->radius = 1;

    complexity->value = 1;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    material->emissiveColor = SbColor(1, 0, 0);
    material->transparency = 0.8;
    depthBuffer->function = SoDepthBuffer::ALWAYS;
}


void InventorEx::colorMaskTest()
{
    CREATE_NODE(SoSeparator, sep)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoSphere, sphere)
        CREATE_NODE(SoCone, cone)
        CREATE_NODE(SoColorMask, colorMask)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, sep},
        {m_root, sphere},
        {sep, colorMask},
        {sep, cone},
        {sep, cube},
    };

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    cube->width = 1.0;
    cube->height = 1.0;
    cube->depth = 1.0;
    sphere->radius = 1.4;
    cone->bottomRadius = 1.0;
    cone->height = 2.0;

    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;
}

void InventorEx::cylinder()
{
    CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoCylinder, cylinder)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, shapeHints},
        {m_root, cylinder},
    };

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::SOLID;

    cylinder->radius = 1.0;
    cylinder->height = 2.0;

}

void glClearDepthBuffer(void* userdata, SoAction* action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void InventorEx::previewPointForward()
{
    CREATE_NODE(SoSeparator, separator)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoCube, point)
        CREATE_NODE(SoMaterial, material)
        CREATE_NODE(SoCallback, cleanDepthBuffCB)
        CREATE_NODE(SoPolygonOffset, polygonOffset)
        CREATE_NODE(SoAnnotation, annotation)
        CREATE_NODE(SoComplexity, complexity)
        CREATE_NODE(SoDepthBuffer, depthBuffer)
        CREATE_NODE(SoDepthBuffer, depthBufferOver)
        CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoDeferredRender, defferedRender)


        int method = 0;
    float factor = 0.0f;
    float units = 0.0f;
    std::cout << "0 for cleanDepthBuffer  1 for depthRange  2 for Annotation  3 for Two-sided Lighting  4 for Deffed Render" << std::endl;
    std::cin >> method;
    switch (method)
    {
        case 0:
            m_root->addChild(cube);
            m_root->addChild(separator);
            material->diffuseColor.setValue(1.0, 0.0, 0.0);
            separator->addChild(material);
            separator->addChild(cleanDepthBuffCB);
            separator->addChild(point);
            cleanDepthBuffCB->setCallback(glClearDepthBuffer);
            break;
        case 1:
            m_root->addChild(depthBuffer);
            m_root->addChild(cube);
            m_root->addChild(separator);
            material->diffuseColor.setValue(1.0, 0.0, 0.0);
            separator->addChild(material);
            separator->addChild(depthBufferOver);
            separator->addChild(point);
            depthBuffer->range.setValue(0.1, 1.0);
            depthBufferOver->range.setValue(0, 0.1);
            break;
        case 2:
            m_root->addChild(cube);
            m_root->addChild(annotation);
            complexity->value = 1;
            material->emissiveColor = SbColor(1, 0, 0);
            annotation->addChild(complexity);
            annotation->addChild(material);
            annotation->addChild(point);
        case 3:
            m_root->addChild(cube);
            m_root->addChild(separator);
            //material->diffuseColor.setValue(1.0, 0.0, 0.0);
            material->emissiveColor = SbColor(1, 0, 0);
            material->transparency = 0.8;
            separator->addChild(material);
            separator->addChild(shapeHints);
            separator->addChild(depthBuffer);
            separator->addChild(point);
            depthBuffer->test = false;
            shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
            shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
            break;
        case 4:
            m_root->addChild(cube);
            m_root->addChild(defferedRender);
            material->diffuseColor.setValue(1.0, 0.0, 0.0);
            defferedRender->addChild(material);
            defferedRender->addChild(point);
            defferedRender->clearDepthBuffer = TRUE;
            break;
    }

    cube->width = 1.0;
    cube->height = 1.0;
    cube->depth = 1.0;
    point->width = 0.2;
    point->height = 0.2;
    point->depth = 0.2;
}

void InventorEx::twoSideFace()
{
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
        4, 5, 6, SO_END_FACE_INDEX,
    };
    int32_t faceIndicesInward[48] = {
        0, 1, 2, SO_END_FACE_INDEX,
        4, 6, 5, SO_END_FACE_INDEX
    };

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedFaceSet* faceSetInward = new SoIndexedFaceSet;
    SoMaterial* material = new SoMaterial;
    SoDepthBuffer* depthBuffer = new SoDepthBuffer;
    SoShapeHints* shapeHints = new SoShapeHints;
    SoTranslation* translation = new SoTranslation;

    m_root->addChild(material);
    m_root->addChild(shapeHints);
    m_root->addChild(coords);
    m_root->addChild(depthBuffer);
    m_root->addChild(faceSet);
    //m_root->addChild(faceSetInward);
    m_root->addChild(translation);
    m_root->addChild(faceSet);

    material->diffuseColor.setValue(1, 0, 0);
    material->transparency = 0.8;
    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    faceSetInward->coordIndex.setValues(0, 8, faceIndicesInward);
    translation->translation.setValue(0, 0, 0.5);

    std::cout << "背面剔除 ON?: " << std::endl;
    bool option = false;
    std::cin >> option;
    if (option)
    {
        shapeHints->shapeType = SoShapeHints::SOLID;
        shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    }
    std::cout << "双面光 ON?(双面光ON与背面剔除ON矛盾): " << std::endl;
    std::cin >> option;
    if (option)
    {
        shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
        shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    }
    std::cout << "depth test: " << std::endl;
    std::cin >> option;
    depthBuffer->test = option;
}

void drawCylinderOutline(void*, SoAction* action)
{
    float radius = 1.0f;
    float height = 4.0f;
    int segments = 20;
    // Draw top circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0 * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, height, z);
    }
    glEnd();

    // Draw bottom circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0 * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // Draw the side lines
    glBegin(GL_LINES);
    glVertex3f(radius, 0.0f, 0.0f);  // bottom right
    glVertex3f(radius, height, 0.0f);  // top right

    glVertex3f(-radius, 0.0f, 0.0f);  // bottom left
    glVertex3f(-radius, height, 0.0f);  // top left
    glEnd();
}

void InventorEx::cylinderGL()
{
    SoCallback* cb = new SoCallback;
    cb->setCallback(drawCylinderOutline);
    m_root->addChild(cb);
}

void InventorEx::deferredRender()
{
    float pts[4][3] = {
    { -5.0, -5.0, 1.0 },
    { 5.0, -5.0, 1.0 },
    { 5.0, 5.0, 1.0 },
    { -5.0, 5.0, 1.0 },
    };
    int32_t faceIndices[8] = {
    0, 1, 2, SO_END_FACE_INDEX,
    0, 2, 3, SO_END_FACE_INDEX,
    };


    CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoDeferredRender, firstFloor)
        CREATE_NODE(SoDeferredRender, secondFloor)
        CREATE_NODE(SoMaterial, redMaterial)
        CREATE_NODE(SoMaterial, blueMaterial)
        CREATE_NODE(SoTranslation, translation)
        CREATE_NODE(SoMaterial, secondFloorMaterial)
        CREATE_NODE(SoCube, littleCube)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoIndexedFaceSet, lens)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, cube},
        {m_root, firstFloor},
        {m_root, secondFloor},
        {firstFloor, redMaterial},
        {firstFloor, littleCube},
        {firstFloor, translation},
        {firstFloor, blueMaterial},
        {firstFloor, littleCube},
        {secondFloor, coords},
        {secondFloor, secondFloorMaterial},
        {secondFloor, shapeHints},
        {secondFloor, lens},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    firstFloor->clearDepthBuffer = TRUE;
    secondFloor->clearDepthBuffer = TRUE;
    redMaterial->diffuseColor.setValue(1, 0, 0);
    blueMaterial->diffuseColor.setValue(0, 0, 1);
    translation->translation.setValue(0.1, 0.1, 0.1);
    secondFloorMaterial->diffuseColor.setValue(0, 1, 0);
    secondFloorMaterial->transparency = 0.8;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    littleCube->width = 0.2;
    littleCube->height = 0.2;
    littleCube->depth = 0.2;

    coords->point.setValues(0, 4, pts);
    lens->coordIndex.setValues(0, 8, faceIndices);
}


// question1: 双面光照时背面没有透明效果(即使是SORTED_OBJECT_BLEND)（延迟渲染无法排序？）
// question2: 选择与拾取模式下，是否可以选到期望层

// question1: 透明物体存在错误的遮挡效果
// 延迟渲染的方式代码体量小，代价也低，缺点就是无法正确的处理透明排序，简单的来说，透明排序也是一种延迟渲染，与deferredRender相矛盾
// 透明物体的渲染效果由渲染顺序和深度测试共同决定，coin中的SORTED_OBJECT_BLEND是通过****关闭深度写入****后对透明物体指定渲染顺序来实现的
// GLRenderAction先遍历节点，在遍历过程中向transList、sortedTransList和delayList添加节点
// 透明排序添加必须依赖于形体节点的访问，而deferredRender中在组节点处就跳出了，这是第一点矛盾
// 第二点矛盾是，traversal、sorttranspobjpaths和delayedpaths在流程中有固有的先后顺序，延迟渲染的透明物体也不进行handleTransparency，coin在设计中就是把两者分离的

// answer1: 假定我们把一些透明物体和不透明物体放在新的一层，这一层的深度缓冲区是全新的，我们希望尽可能地将这一层正确地渲染（考虑SORTED_OBJECT_BLEND）
//         1. 毫无疑问的，我们需要先渲染不透明物体，这一步要开启深度测试与深度写入，不要求渲染顺序
//         2. 在上一步构建的深度缓冲中，渲染透明物体，指定从后向前的渲染顺序是关键，深度测试开启而深度写入关闭
//         新的课题是：如何控制一个组节点下的形体的渲染顺序
//         想法1：在deferredRender中使用新的action，新action不将delayedpaths下子树视为延迟渲染，可以正常渲染处理透明排序
//               这个方法有新action难以设置state的困难，既没有直接赋值接口，从头遍历属性节点也不现实
//         想法2：给delayedpaths编号，在外部计算透明顺序，使用switchToPath控制渲染顺序

void InventorEx::flat()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoDeferredRender, firstFloor)
        CREATE_NODE(SoDeferredRender, secondFloor)
        CREATE_NODE(SoSeparator, redFace)
        CREATE_NODE(SoSeparator, greenFace)
        CREATE_NODE(SoMaterial, redMaterial)
        CREATE_NODE(SoMaterial, greenMaterial)
        CREATE_NODE(SoMaterial, secondFloorMaterial)
        CREATE_NODE(SoTranslation, redTranslation)
        CREATE_NODE(SoTranslation, greenTranslation)
        CREATE_NODE(SoTranslation, secondFloorTranslation)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoIndexedFaceSet, face)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, coords},
        {m_root, shapeHints},
        {m_root, face},
        {m_root, firstFloor},
        {m_root, secondFloor},
        {firstFloor, redFace},
        {firstFloor, greenFace},
        {redFace, redMaterial},
        {redFace, redTranslation},
        {redFace, face},
        {greenFace, greenMaterial},
        {greenFace, greenTranslation},
        {greenFace, face},
        {secondFloor, secondFloorMaterial},
        {secondFloor, secondFloorTranslation},
        {secondFloor, face},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    std::cout << "Clear DepthBuffer?: ";
    bool option = false;
    std::cin >> option;
    firstFloor->clearDepthBuffer = option;
    secondFloor->clearDepthBuffer = option;

    redMaterial->diffuseColor.setValue(1, 0, 0);
    redMaterial->transparency.setValue(0.8);
    greenMaterial->diffuseColor.setValue(0, 1, 0);
    greenMaterial->transparency.setValue(0.8);
    secondFloorMaterial->diffuseColor.setValue(0, 0, 1);
    secondFloorMaterial->transparency.setValue(0.8);

    redTranslation->translation.setValue(0, 0, 0.2);
    greenTranslation->translation.setValue(0, 0, 0.4);
    secondFloorTranslation->translation.setValue(0, 0, 0.6);

    coords->point.setValues(0, 8, pts);
    face->coordIndex.setValues(0, 4, faceIndices);
}

void actionSwitchTo(void* userdata, SoAction* action)
{
    SoPath* path = (SoPath*)userdata;
    //action->switchToPathTraversal(path);
    action->apply(path);
}

void InventorEx::switchToPathTraversal()
{
    // figure out how to store and restore state

    CREATE_NODE(SoSeparator, sep1)
        CREATE_NODE(SoMaterial, material1)
        CREATE_NODE(SoSphere, sphere)
        CREATE_NODE(SoSeparator, sepAdd)
        CREATE_NODE(SoSeparator, sep2)
        CREATE_NODE(SoMaterial, material2)
        CREATE_NODE(SoTranslation, trans)
        CREATE_NODE(SoCallback, cb)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        //{m_root, sep1},
        {m_root, sep2},
        {sep1, material1},
        {sep1, sepAdd},
        {sepAdd, sphere},
        {sep2, material2},
        {sep2, trans},
        {sep2, cb},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    material1->diffuseColor.setValue(1, 0, 0);
    material2->diffuseColor.setValue(0, 0, 1);
    trans->translation.setValue(1, 1, 1);

    m_viewer->setSceneGraph(m_root);

    // view会添加camera和light，这里m_root并不是真正的根节点，可以通过m_viewer->getSoRenderManager()->getSceneGraph()获取
    // 对于Recursive repaint问题，使用未被引用的节点树
    // 但是都解决不掉fulllength越界的问题
    SoPath* path = new SoPath();
    path->ref();
    path->append(sep1);
    path->append(sepAdd);
    path->append(sphere);
    std::cout << "length: " << path->getLength() << std::endl;
    std::cout << "full length: " << ((SoFullPath*)path)->getLength() << std::endl;
    cb->setCallback(actionSwitchTo, path);
}


static void renderAuxViewport(void* userdata, SoAction* action)
{
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
        return;

    SoGLRenderAction* glAction = static_cast<SoGLRenderAction*>(action);

    // 保存当前OpenGL状态
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glPushMatrix();

    // 获取窗口大小
    const SbViewportRegion& vp = glAction->getViewportRegion();
    int windowWidth = vp.getWindowSize()[0];
    int windowHeight = vp.getWindowSize()[1];

    // 设置OpenGL视口到窗口的右下角
    glViewport((GLint)(0.7 * windowWidth), 0,
               (GLsizei)(0.3 * windowWidth), (GLsizei)(0.3 * windowHeight));


    // 从当前的action状态中获取相机矩阵
    SbMatrix modelMatrix = SoViewingMatrixElement::get(glAction->getState());
    SbMatrix projMatrix = SoProjectionMatrixElement::get(glAction->getState());

    // 创建一个与主相机相反的旋转
    SbRotation flipRotation(SbVec3f(0, 1, 0), M_PI);
    SbMatrix flipMatrix;
    flipMatrix.setRotate(flipRotation);

    // 将翻转旋转应用到模型矩阵
    modelMatrix.multRight(flipMatrix);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((const GLfloat*)projMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((const GLfloat*)modelMatrix);

    // 渲染球体
    SoSphere* sphere = static_cast<SoSphere*>(userdata);
    sphere->GLRender(glAction);

    // 恢复OpenGL状态
    glPopMatrix();
    glPopAttrib();

    SoState* state = action->getState();
    SoGLLazyElement* lazyElt = (SoGLLazyElement*)SoLazyElement::getInstance(state);
    lazyElt->reset(state, (SoLazyElement::DIFFUSE_MASK) | (SoLazyElement::LIGHT_MODEL_MASK));
}

// 参考SoViewport
void InventorEx::auxViewport()// Not implemented yet
{
    SoSphere* sphere = new SoSphere; // 一个简单的球体
    m_root->addChild(sphere);

    SoCallback* auxViewportCallback = new SoCallback;

    auxViewportCallback->setCallback(renderAuxViewport, sphere);
    m_root->addChild(auxViewportCallback);
}

/*
void cubeDrag()
{
    // 获取当前视口区域
    const SbViewportRegion region = eventCB->getAction()->getViewportRegion();
    // 获取场景的透视相机
    SoPerspectiveCamera* camera = (SoPerspectiveCamera*)root->getChild(0);
    // 获取当前的位置事件
    const SoLocation2Event* locEvent = (const SoLocation2Event*)event;
    // 获取相机的当前旋转
    SbRotation rotation = camera->orientation.getValue();

    // 计算场景移动后与上一次场景的差值
    SbVec3f siteMove = siteAfterMove - lastSite;

    // 获取视口的像素大小
    SbVec2s viewportSize = region.getViewportSizePixels();
    // 获取相机的视图体积
    SbViewVolume viewVolume = camera->getViewVolume();

    // 计算相机与场景之间的距离
    SbVec3f distanceCameraSiteV = camera->position.getValue() - site;
    float distanceCameraSite = distanceCameraSiteV.length();

    // 将上次的鼠标位置转换为屏幕坐标
    float lastMouseXinScreen = float(lastMouseX) / float(viewportSize[0]);
    float lastMouseYinScreen = float(lastMouseY) / float(viewportSize[1]);
    SbVec2f lastMouseInScreen(lastMouseXinScreen, lastMouseYinScreen);
    // 将上次的鼠标屏幕坐标转换为3D世界坐标
    SbVec3f lastMouseInWorld = viewVolume.getPlanePoint(distanceCameraSite, lastMouseInScreen);

    // 获取当前鼠标位置
    int mouseX = locEvent->getPosition()[0];
    int mouseY = locEvent->getPosition()[1];
    // 将当前鼠标位置转换为屏幕坐标
    float mouseXinScreen = float(mouseX) / float(viewportSize[0]);
    float mouseYinScreen = float(mouseY) / float(viewportSize[1]);
    SbVec2f mouseInScreen(mouseXinScreen, mouseYinScreen);

    // 计算鼠标在屏幕上的移动量
    SbVec2f moveOnScreen = mouseInScreen - lastMouseInScreen;

    // 计算屏幕上的移动与normal的关系
    SbVec2f normal2D(normal_project_camera[0], normal_project_camera[1]);
    float dir = moveOnScreen.dot(normal2D) / (moveOnScreen.length() * normal2D.length());
    // 计算移动的长度
    float lengthAlongNormal = moveOnScreen.dot(normal2D) / normal2D.length();
    SbVec2f mouseMoved = lastMouseInScreen + lengthAlongNormal * normal2D.normalized();

    // 将鼠标的移动转换为3D世界坐标
    SbVec3f mouseInWorldMoved = viewVolume.getPlanePoint(distanceCameraSite, mouseMoved);

    // 计算3D世界中的移动量
    SbVec3f move = mouseInWorldMoved - lastMouseInWorld;

    // 计算normal在相机坐标系下的投影
    SbVec3f normalOnCamera;
    rotation.inverse().multVec(normal, normalOnCamera);
    // 计算场景在屏幕上的移动
    SbVec3f siteMoveOnScreen;
    rotation.inverse().multVec(siteMove, siteMoveOnScreen);
    // 计算移动量与normal的夹角
    float Cosine = sqrt(normalOnCamera[0] * normalOnCamera[0] + normalOnCamera[1] * normalOnCamera[1]) / normalOnCamera.length();

    // 计算实际的移动距离
    float distance = move.length() / Cosine;

    // 根据方向调整距离
    if (dir < 0) distance *= -1;
    distance = normal2D.length() < 0.1 ? 0 : distance;
    SbVec3f offset = distance * normal;

    // 更新所有相关顶点的位置
    for (int i = 0; i < face_vertex_num; ++i)
    {
        vertex_index = map_vertex_index[i];
        for (auto index : vertex_index)
        {
            vertices[index][0] += offset[0];
            vertices[index][1] += offset[1];
            vertices[index][2] += offset[2];
        }
    }

    // 创建和设置新的3D坐标点
    SoCoordinate3* newCoords = new SoCoordinate3;
    newCoords->point.setValues(0, face_vertex_num, vertices);
    // 更新旧的3D坐标点
    SoCoordinate3* coord = (SoCoordinate3*)root->getChild(4);
    coord->point.setValues(0, face_vertex_num, vertices);

    // 更新上次的鼠标位置和场景位置
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    lastSite += offset;

    // 输出场景位置用于调试
    std::cout << "site:" << lastSite[0] << "," << lastSite[1] << "," << lastSite[2] << std::endl;

    return;
}
*/

// 证明了path.GLRender(glrenderaction)应用的是Path所处位置的状态
void InventorEx::actStateOfDelayList()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoDeferredRender, firstFloor)
        CREATE_NODE(SoDeferredRender, secondFloor)
        CREATE_NODE(SoMaterial, firstFloorMaterial)
        CREATE_NODE(SoMaterial, firstFloorMaterialPlus)
        CREATE_NODE(SoMaterial, secondFloorMaterial)
        CREATE_NODE(SoTranslation, firstFloorTranslation)
        CREATE_NODE(SoTranslation, secondFloorTranslation)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoIndexedFaceSet, face)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, coords},
        {m_root, shapeHints},
        {m_root, face},
        {m_root, firstFloorMaterial},
        {m_root, firstFloorTranslation},
        {m_root, firstFloor},
        {m_root, secondFloor},
        {firstFloor, face},
        {firstFloor, firstFloorMaterialPlus},
        {firstFloor, firstFloorTranslation},
        {firstFloor, face},
        {secondFloor, secondFloorMaterial},
        {secondFloor, secondFloorTranslation},
        {secondFloor, face},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    firstFloor->clearDepthBuffer = TRUE;
    secondFloor->clearDepthBuffer = TRUE;

    firstFloorMaterial->diffuseColor.setValue(1, 0, 0);
    firstFloorMaterial->transparency.setValue(0.8);
    firstFloorMaterialPlus->diffuseColor.setValue(0, 1, 0);
    firstFloorMaterialPlus->transparency.setValue(0.8);
    secondFloorMaterial->diffuseColor.setValue(0, 0, 1);
    secondFloorMaterial->transparency.setValue(0.8);

    firstFloorTranslation->translation.setValue(0, 0, 0.2);
    secondFloorTranslation->translation.setValue(0, 0, 0.4);

    coords->point.setValues(0, 8, pts);
    face->coordIndex.setValues(0, 4, faceIndices);
}


SoSeparator* InventorEx::assembleEasyBody(const ShapeData& data, bool usePath)
{
    SoSeparator* face = NULL;
    if (usePath)
        face = new SoDeferredRender;
    else
        face = new SoSeparator;

    CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoIndexedFaceSet, faceSet)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {face, coords},
        {face, faceSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    coords->point.setValues(0, data.points.size(), reinterpret_cast<const float(*)[3]>(data.points.data()));
    faceSet->coordIndex.setValues(0, data.faceIndices.size(), data.faceIndices.data());

    return face;
}

SoSeparator* InventorEx::usePathAssemble(std::vector<ShapeData> randomCuboids, bool usePath)
{
    CREATE_NODE(SoSeparator, bodies)
        CREATE_NODE(SoScale, scale)
        CREATE_NODE(SoSwitch, trasparencyTypeSwitch)
        CREATE_NODE(SoTransparencyType, trasparencyType)
        CREATE_NODE(SoSeparator, dataNode)
        CREATE_NODE(SoSwitch, renderModeSwitch)
        CREATE_NODE(SoSeparator, shadeWithEdge)
        CREATE_NODE(SoSeparator, shadeWithoutEdge)
        CREATE_NODE(SoSeparator, transluency)
        CREATE_NODE(SoSeparator, staticWireframe)
        CREATE_NODE(SoSeparator, wireframeWithoutHidden)
        CREATE_NODE(SoSeparator, faceRoot)
        CREATE_NODE(SoPolygonOffset, polygonOffset)
        CREATE_NODE(SoDrawStyle, faceStyle)
        CREATE_NODE(SoNormal, faceNormal)
        CREATE_NODE(SoNormalBinding, normalBinding)
        CREATE_NODE(SoSwitch, materialSwitch)
        CREATE_NODE(SoMaterial, transparentMaterial)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {bodies, scale},
        {bodies, trasparencyTypeSwitch},
        {bodies, dataNode},
        {trasparencyTypeSwitch, trasparencyType},
        {dataNode, renderModeSwitch},
        {renderModeSwitch, shadeWithEdge},
        {renderModeSwitch, shadeWithoutEdge},
        {renderModeSwitch, transluency},
        {renderModeSwitch, staticWireframe},
        {renderModeSwitch, wireframeWithoutHidden},
        {staticWireframe, polygonOffset},
        {staticWireframe, faceRoot},
        {faceRoot, faceStyle},
        {faceRoot, faceNormal},
        {faceRoot, normalBinding},
        {faceRoot, materialSwitch},
        {materialSwitch, transparentMaterial},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    for (const auto& data : randomCuboids)
    {
        ADD_CHILD(faceRoot, assembleEasyBody(data, usePath));
    }

    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 3;

    return bodies;
}

void InventorEx::traversalPerformance()
{
    std::cout << "use path?: " << std::endl;
    bool option = false;
    std::cin >> option;

    if (0 == m_randomCuboids.size())
        m_randomCuboids = generateRandomCuboids(20000, 5.0);
    std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, usePathAssemble(m_randomCuboids, option)},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }
}

void InventorEx::isDelayRenderNessery()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoIndexedFaceSet, faceSet)
        CREATE_NODE(SoTranslation, translation)
        CREATE_NODE(SoSeparator, redTransFace)
        CREATE_NODE(SoSeparator, greenFace)
        CREATE_NODE(SoSeparator, blueTransFace)
        CREATE_NODE(SoMaterial, redMaterial)
        CREATE_NODE(SoMaterial, greenMaterial)
        CREATE_NODE(SoMaterial, blueMaterial)
        CREATE_NODE(SoCallback, cb)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, coords},
        {m_root, shapeHints},
        {m_root, redTransFace},
        {m_root, translation},
        {m_root, greenFace},
        {m_root, translation},
        {m_root, blueTransFace},
        {redTransFace, redMaterial},
        {redTransFace, faceSet},
        {greenFace, greenMaterial},
        {greenFace, faceSet},
        {blueTransFace, cb},
        {blueTransFace, blueMaterial},
        {blueTransFace, faceSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    redMaterial->diffuseColor.setValue(1, 0, 0);
    redMaterial->transparency.setValue(0.5);
    greenMaterial->diffuseColor.setValue(0, 1, 0);
    blueMaterial->diffuseColor.setValue(0, 0, 1);
    blueMaterial->transparency.setValue(0.5);

    translation->translation.setValue(0, 0, 0.2);

    cb->setCallback(glClearDepthBuffer);

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 4, faceIndices);
}

void InventorEx::twoSideLightInDelayRender()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoDeferredRender, deferredRender)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoIndexedFaceSet, faceSet)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, deferredRender},
        {deferredRender, coords},
        {deferredRender, shapeHints},
        {deferredRender, faceSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 4, faceIndices);

    deferredRender->renderCaching = SoSeparator::OFF;
    m_root->renderCaching = SoSeparator::OFF;
}

const char* gnomonGeometry = "\
#Inventor V2.1 ascii\n\
\
DEF GnomonGeom Separator { \
  PickStyle { style UNPICKABLE } \
  DrawStyle { style FILLED } \
  LightModel { model PHONG } \
  MaterialBinding { value OVERALL } \
  Complexity { value .2 } \
  ShapeHints { vertexOrdering COUNTERCLOCKWISE shapeType SOLID } \
  Font { name \"Arial : Bold\" size 15 } \
  Separator { \
    Material { \
      diffuseColor    [ 0.5 0 0 ] \
      emissiveColor   [ 0.5 0 0 ] \
    } \
    RotationXYZ { axis Z angle -1.570796327 } \
    Cylinder { height 2 radius .06 } \
    Translation { translation 0 1 0 } \
    Cone { bottomRadius .18 height .3 } \
    Translation { translation 0 .16 0 } \
    Text2 { string \"X\" } \
  } \
  Separator { \
    Material { \
      diffuseColor    [ 0 0.5 0 ] \
      emissiveColor   [ 0 0.5 0 ] \
    } \
    Cylinder { height 2 radius .06 } \
    Translation { translation 0 1 0 } \
    Cone { bottomRadius .18 height .3 } \
    Translation { translation 0 .16 0 } \
    Text2 { string \"Y\" } \
  } \
  Material { \
    diffuseColor    [ 0 0 0.5 ] \
    emissiveColor   [ 0 0 0.5 ] \
  } \
  RotationXYZ { axis X angle 1.570796327 } \
  Cylinder { height 2 radius .06 } \
  Translation { translation 0 1 0 } \
  Cone { bottomRadius .18 height .3 } \
  Translation { translation 0 .16 0 } \
  Text2 { string \"Z\" } \
} ";

SoSeparator* makeGnomon()
{
    SoSeparator* pRoot = new SoSeparator(1);
    SoSwitch* pSwitch = new SoSwitch(2);
    SoCallback* pCallb = new SoCallback;
    SoPerspectiveCamera* pCam = new SoPerspectiveCamera;
    pRoot->ref();
    pRoot->setName("Gnomon");

    // Switch node will allow us turn the gnomon on and off
    pSwitch->setName("GnomonSwitch");
    pRoot->addChild(pSwitch);


    SoInput in;
    in.setBuffer((void*)gnomonGeometry, (size_t)strlen(gnomonGeometry));
    SoNode* node;
    SbBool ok = SoDB::read(&in, node);
    if (ok && node != NULL) {
        pSwitch->addChild(node);
        pSwitch->whichChild = SO_SWITCH_ALL;
    }

    pRoot->unrefNoDelete();
    return pRoot;
}

void InventorEx::gnomon()
{
    m_root->addChild(makeGnomon());
}

void InventorEx::depthRange()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoSeparator, firstFloor)
        CREATE_NODE(SoSeparator, secondFloor)
        CREATE_NODE(SoSeparator, redFace)
        CREATE_NODE(SoSeparator, greenFace)
        CREATE_NODE(SoMaterial, redMaterial)
        CREATE_NODE(SoMaterial, greenMaterial)
        CREATE_NODE(SoMaterial, secondFloorMaterial)
        CREATE_NODE(SoTranslation, redTranslation)
        CREATE_NODE(SoTranslation, greenTranslation)
        CREATE_NODE(SoTranslation, secondFloorTranslation)
        CREATE_NODE(SoDepthBuffer, groudFloorDepthBuffer);
    CREATE_NODE(SoDepthBuffer, firstFloorDepthBuffer);
    CREATE_NODE(SoDepthBuffer, secondFloorDepthBuffer);
    CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoIndexedFaceSet, face)
        CREATE_NODE(SoCube, cube)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, coords},
        {m_root, shapeHints},
        {m_root, groudFloorDepthBuffer},
        {m_root, face},
        {m_root, firstFloor},
        {m_root, secondFloor},
        {firstFloor, firstFloorDepthBuffer},
        {firstFloor, redFace},
        {firstFloor, greenFace},
        {redFace, redMaterial},
        {redFace, redTranslation},
        {redFace, face},
        {greenFace, greenMaterial},
        {greenFace, greenTranslation},
        {greenFace, face},
        {secondFloor, secondFloorDepthBuffer},
        {secondFloor, secondFloorMaterial},
        {secondFloor, secondFloorTranslation},
        {secondFloor, face},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    redMaterial->diffuseColor.setValue(1, 0, 0);
    redMaterial->transparency.setValue(0.8);
    greenMaterial->diffuseColor.setValue(0, 1, 0);
    greenMaterial->transparency.setValue(0.8);
    secondFloorMaterial->diffuseColor.setValue(0, 0, 1);
    secondFloorMaterial->transparency.setValue(0.8);

    redTranslation->translation.setValue(0, 0, 0.2);
    greenTranslation->translation.setValue(0, 0, 0.4);
    secondFloorTranslation->translation.setValue(0, 0, 0.6);

    groudFloorDepthBuffer->range.setValue(0.1, 1.0);
    firstFloorDepthBuffer->range.setValue(0.05, 0.1);
    secondFloorDepthBuffer->range.setValue(0, 0.05);

    coords->point.setValues(0, 8, pts);
    face->coordIndex.setValues(0, 4, faceIndices);
}

#define CLEARTIMES 1E+3
void clearDepthBufferManyTimes(void* userdata, SoAction* action)
{
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
        return;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < CLEARTIMES; ++i)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Done in " << duration.count() << " seconds" << std::endl;
}

void InventorEx::clearDepthBuffer()
{
    wireframe();
    SoCallback* cb = new SoCallback;
    // glClear耗时与view中形体个数无关
    cb->setCallback(clearDepthBufferManyTimes);
    m_root->addChild(cb);
}

void InventorEx::depthConflict()
{
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
    int32_t faceIndices[8] = {
        4, 5, 6, SO_END_FACE_INDEX
    };

    CREATE_NODE(SoShapeHints, shapeHints)
        CREATE_NODE(SoSeparator, firstFloor)
        CREATE_NODE(SoSeparator, secondFloor)
        CREATE_NODE(SoSeparator, redFace)
        CREATE_NODE(SoSeparator, greenFace)
        CREATE_NODE(SoMaterial, redMaterial)
        CREATE_NODE(SoMaterial, greenMaterial)
        CREATE_NODE(SoMaterial, secondFloorMaterial)
        CREATE_NODE(SoTranslation, redTranslation)
        CREATE_NODE(SoTranslation, greenTranslation)
        CREATE_NODE(SoTranslation, secondFloorTranslation)
        CREATE_NODE(SoDepthBuffer, groudFloorDepthBuffer);
    CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoIndexedFaceSet, face)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, coords},
        {m_root, shapeHints},
        {m_root, groudFloorDepthBuffer},
        {m_root, face},
        {m_root, firstFloor},
        {m_root, secondFloor},
        {firstFloor, redFace},
        {firstFloor, greenFace},
        {redFace, redMaterial},
        {redFace, redTranslation},
        {redFace, face},
        {greenFace, greenMaterial},
        {greenFace, greenTranslation},
        {greenFace, face},
        {secondFloor, secondFloorMaterial},
        {secondFloor, secondFloorTranslation},
        {secondFloor, face},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    redMaterial->diffuseColor.setValue(1, 0, 0);
    greenMaterial->diffuseColor.setValue(0, 1, 0);
    secondFloorMaterial->diffuseColor.setValue(0, 0, 1);

    redTranslation->translation.setValue(0, 0, 0.000001);// 0.000001
    greenTranslation->translation.setValue(0, 0, 0.000002);
    secondFloorTranslation->translation.setValue(0, 0, 0.000003);

    bool useRange = false;
    std::cout << "useRange: " << std::endl;
    std::cin >> useRange;
    if (useRange)
        groudFloorDepthBuffer->range.setValue(0.99, 1.0);

    coords->point.setValues(0, 8, pts);
    face->coordIndex.setValues(0, 4, faceIndices);
}

SoDeferredRender* createWcsAxis()
{
    const char* wcsAxisGeo = R"(
#Inventor V2.1 ascii

DEF AxisGeo Separator {
  DrawStyle { style FILLED }
  LightModel { model PHONG }
  MaterialBinding { value OVERALL }
  Complexity { value .2 }
  ShapeHints { vertexOrdering COUNTERCLOCKWISE shapeType SOLID }

  Separator {
    Material { 
      diffuseColor [0.5 0.125 0.125] 
      emissiveColor [0.5 0.125 0.125] 
    }
    Cylinder { height 1 radius .02 }
    Translation { translation 0 0.5 0 }
    Separator {  # Arrowhead for X-axis
      Cone { bottomRadius 0.125 height 0.3333 }
      Translation { translation 0 0.3333 0 }
      IndexedFaceSet {
          vertexProperty DEF vProp VertexProperty {
              vertex [
                  0 0.25 0,
                  0 0 0.25,
                  0 -0.25 0,
                  0 0 -0.25
              ]
          }
          coordIndex [
              0, 1, 2, 3, -1
          ]
      }
    }
    Translation { translation 0 0.5 0 }
    Text2 { string "X" }
  }

  Separator {
    Material { 
      diffuseColor [0.125 0.5 0.125] 
      emissiveColor [0.125 0.5 0.125] 
    }
    RotationXYZ { axis Z angle -1.570796327 }
    Cylinder { height 1 radius .02 }
    Translation { translation 0 0.5 0 }
    Separator {  # Arrowhead for Y-axis
      Cone { bottomRadius 0.125 height 0.3333 }
      Translation { translation 0 0.3333 0 }
      IndexedFaceSet {
        vertexProperty USE vProp
        coordIndex [
          0, 1, 2, 3, -1
        ]
      }
    }
    Translation { translation 0 0.5 0 }
    Text2 { string "Y" }
  }

  Separator {
    Material { 
      diffuseColor [0.125 0.125 0.5] 
      emissiveColor [0.125 0.125 0.5] 
    }
    RotationXYZ { axis X angle 1.570796327 }
    Cylinder { height 1 radius .02 }
    Translation { translation 0 0.5 0 }
    Separator {  # Arrowhead for Z-axis
      Cone { bottomRadius 0.125 height 0.3333 }
      Translation { translation 0 0.3333 0 }
      IndexedFaceSet {
        vertexProperty USE vProp
        coordIndex [
          0, 1, 2, 3, -1
        ]
      }
    }
    Translation { translation 0 0.5 0 }
    Text2 { string "Z" }
  }
}
)";

    SoDeferredRender* pDeferredRender = new SoDeferredRender(true);
    pDeferredRender->ref();

    SoInput in;
    in.setBuffer((void*)wcsAxisGeo, (size_t)strlen(wcsAxisGeo));
    SoNode* pNode = NULL;
    if (SoDB::read(&in, pNode) && pNode)
    {
        pDeferredRender->addChild(pNode);
    }

    pDeferredRender->unrefNoDelete();
    return pDeferredRender;
}

SoDeferredRender* directionSelectionAxis()
{
    return NULL;

}

SoDeferredRender* rotationCenterPoint()
{
    return NULL;
}

SoDeferredRender* previewPointsAndAxes()
{
    return NULL;
}

SoDeferredRender* naviCube()
{
    return NULL;
}

void InventorEx::modelView()
{
    CREATE_NODE(SoSeparator, backgroundRoot);
    CREATE_NODE(SoSeparator, modelSpace);
    CREATE_NODE(SoSeparator, overlay);
    CREATE_NODE(SoSwitch, wcsAxis);
    CREATE_NODE(SoSwitch, directionSelectionAxis);
    CREATE_NODE(SoSwitch, rotationCenterPoint);
    CREATE_NODE(SoSwitch, previewPointsAndAxes);
    CREATE_NODE(SoSwitch, naviCube);

    std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, backgroundRoot},
        {m_root, overlay},
        {overlay, wcsAxis},
        {overlay, directionSelectionAxis},
        {overlay, rotationCenterPoint},
        {overlay, previewPointsAndAxes},
        {overlay, naviCube},
        {backgroundRoot, new SoGradientBackground},
        {wcsAxis, createWcsAxis()}

    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    wcsAxis->whichChild = 0;

    wireframe();
}

void glTwoSideLight(void* userdata, SoAction* action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    }
}

void InventorEx::twoSideDiscover()
{
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
        4, 5, 6, SO_END_FACE_INDEX,
    };
    float norms[8][3] = {
        {0.0, 0.0, -1.0},
        {0.0, 0.0, -1.0},
        {0.0, 0.0, -1.0},
        {0.0, 0.0, -1.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
    };
    SoLightModel* lightModel = new SoLightModel;
    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoMaterial* material = new SoMaterial;
    SoShapeHints* shapeHints = new SoShapeHints;
    SoCallback* cb = new SoCallback;
    SoNormal* normals = new SoNormal;
    normals->vector.setValues(0, 8, norms);
    SoNormalBinding* normalBinding = new SoNormalBinding;
    normalBinding->value = SoNormalBinding::PER_VERTEX;

    //m_root->addChild(lightModel);
    m_root->addChild(cb);
    //m_root->addChild(shapeHints);
    m_root->addChild(material);
    m_root->addChild(coords);
    m_root->addChild(normals);
    m_root->addChild(normalBinding);
    m_root->addChild(faceSet);

    m_root->renderCaching = SoSeparator::OFF;

    material->diffuseColor.setValue(1, 0, 0);

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->creaseAngle = 0.0;

    cb->setCallback(glTwoSideLight);

    lightModel->model = SoLightModel::BASE_COLOR;

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 8, faceIndices);
}

void InventorEx::oneSideCorrect()
{
    float pts[3][3] = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 1.0, 0.0 },
    };
    int32_t faceIndices[48] = {
        0, 1, 2, SO_END_FACE_INDEX,
    };

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoMaterial* material = new SoMaterial;
    SoShapeHints* shapeHints = new SoShapeHints;

    m_root->addChild(material);
    m_root->addChild(shapeHints);
    m_root->addChild(coords);
    m_root->addChild(faceSet);

    material->diffuseColor.setValue(1, 0, 0);

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    coords->point.setValues(0, 3, pts);
    faceSet->coordIndex.setValues(0, 4, faceIndices);
}

void glTwoSideFace(void*, SoAction* action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        // 允许自定义 OpenGL 代码与 Coin3D 共存
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        // 设置双面光照
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glEnd();

        // 恢复 Coin3D 的 OpenGL 状态
        glPopAttrib();
    }
}

void InventorEx::glTwoSide()
{
    SoCallback* cb = new SoCallback;
    cb->setCallback(glTwoSideFace);
    m_root->addChild(cb);

}

void InventorEx::replaceGroup()
{
    SoSeparator* pGroup = new SoSeparator;
    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(1, 0, 0);

    pGroup->addChild(mat);
    m_root->addChild(pGroup);
}

void InventorEx::staticWireframe()
{
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

    CREATE_NODE(SoSeparator, m_root)
        CREATE_NODE(SoSeparator, staticWireframe)
        CREATE_NODE(SoSeparator, faceRoot)
        CREATE_NODE(SoSeparator, lineRoot)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoTransparencyType, transType)
        CREATE_NODE(SoDrawStyle, faceDrawStyle)
        CREATE_NODE(SoDrawStyle, lineDrawStyle)
        CREATE_NODE(SoMaterial, faceMaterial)
        CREATE_NODE(SoMaterial, lineMaterial)
        CREATE_NODE(SoIndexedFaceSet, faces)
        CREATE_NODE(SoIndexedLineSet, lines)
        CREATE_NODE(SoPolygonOffset, polygonOffset)

        // 添加子节点
        m_root->addChild(transType);
    m_root->addChild(coords);
    m_root->addChild(staticWireframe);

    staticWireframe->addChild(polygonOffset);
    staticWireframe->addChild(faceRoot);
    staticWireframe->addChild(lineRoot);

    faceRoot->addChild(faceDrawStyle);
    faceRoot->addChild(faceMaterial);
    faceRoot->addChild(faces);

    lineRoot->addChild(lineDrawStyle);
    lineRoot->addChild(lineMaterial);
    lineRoot->addChild(lines);

    // 设置属性
    transType->value = SoTransparencyType::NONE;
    coords->point.setValues(0, 8, pts);

    faceDrawStyle->style = SoDrawStyle::FILLED;
    faceMaterial->transparency = 1.0f;
    faces->coordIndex.setValues(0, 48, faceIndices);

    lineDrawStyle->style = SoDrawStyle::LINES;
    lineMaterial->diffuseColor.setValue(51.0f / 255.0f, 51.0f / 255.0f, 51.0f / 255.0f);
    lines->coordIndex.setValues(0, 24, lineIndices);
}


void InventorEx::removeAllChildren()
{
    SoSeparator* Q = new SoSeparator;
    SoSeparator* R = new SoSeparator;
    SoSeparator* S = new SoSeparator;

    Q->addChild(S);
    R->addChild(S);

    int refCount[3] = { 0 };
    refCount[0] = Q->getRefCount();
    refCount[1] = R->getRefCount();
    refCount[2] = S->getRefCount();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl;

    Q->ref();
    refCount[0] = Q->getRefCount();
    refCount[1] = R->getRefCount();
    refCount[2] = S->getRefCount();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl;

    std::cout << "0 for unref\n1 for unrefNoDelete\n2 for removeAllChildren\n3 for removeThenUnref" << std::endl;
    int isDelete = 0;
    std::cin >> isDelete;
    switch (isDelete)
    {
        case 0:
            Q->unref();
            break;
        case 1:
            Q->unrefNoDelete();
            break;
        case 2:
            Q->removeAllChildren();
            break;
        case 3:
            Q->removeAllChildren();
            Q->unref();
            break;
    }
    refCount[0] = Q->getRefCount();
    refCount[1] = R->getRefCount();
    refCount[2] = S->getRefCount();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << refCount[i] << ' ';
    }
    std::cout << std::endl << std::endl;
}


QImage g_img;
void glReadPixelsCallback(void*, SoAction* action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        // 首先判断动作是否为SoGLRenderAction
        SoGLRenderAction* glRenderAction = dynamic_cast<SoGLRenderAction*>(action);
        if (glRenderAction)
        {
            // 获取与此渲染动作相关联的视口区域
            const SbViewportRegion& vp = glRenderAction->getViewportRegion();
            SbVec2s size = vp.getViewportSizePixels();
            int width = size[0];
            int height = size[1];

            // 接下来你可以使用这个尺寸来调用glReadPixels
            QImage img(QSize(width, height), QImage::Format_RGB32);
            glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
            g_img = img;
            g_img.save("../Data/image.png", "PNG");
        }
    }
}

void glDrawPixelsCallback(void*, SoAction* action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        // 首先判断动作是否为SoGLRenderAction
        SoGLRenderAction* glRenderAction = dynamic_cast<SoGLRenderAction*>(action);
        if (glRenderAction)
        {
            // 获取与此渲染动作相关联的视口区域
            const SbViewportRegion& vp = glRenderAction->getViewportRegion();
            SbVec2s size = vp.getViewportSizePixels();
            int width = size[0];
            int height = size[1];

            // 接下来你可以使用这个尺寸来调用glReadPixels
            glDrawPixels(width, height, GL_BGRA, GL_UNSIGNED_BYTE, g_img.bits());
        }

        glEnable(GL_DEPTH_TEST);
    }
}

void InventorEx::renderImage()
{
    SoSphere* sphere = new SoSphere;
    m_root->addChild(sphere);

    SoCallback* glReadPixelsCB = new SoCallback;
    glReadPixelsCB->setCallback(glReadPixelsCallback);
    m_root->addChild(glReadPixelsCB);

    SoCallback* glDrawPixelsCB = new SoCallback;
    glDrawPixelsCB->setCallback(glDrawPixelsCallback);
    m_root->addChild(glDrawPixelsCB);
}

SoSeparator* InventorEx::refInterface()
{
    SoSeparator* pLocalRoot = new SoSeparator;
    pLocalRoot->addChild(new SoSphere);
    SoAction* pAction = new SoGLRenderAction(m_viewer->getSoRenderManager()->getViewportRegion());
    pAction->apply(pLocalRoot);
    return pLocalRoot;
}

void InventorEx::refMain()
{
    m_root->addChild(refInterface());
}

void InventorEx::bigSphere()
{
    SoSphere* sphere = new SoSphere;
    m_root->addChild(sphere);

    int degree = 0;
    std::cin >> degree;
    sphere->radius = pow(10, degree); // <=18 good
}

void InventorEx::dotLine()
{
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
    int32_t lineIndices[24] = {
        0, 1, 2, 3, 0, SO_END_LINE_INDEX,
        4, 5, 6, 7, 4, SO_END_LINE_INDEX,
        0, 4, SO_END_LINE_INDEX,
        1, 5, SO_END_LINE_INDEX,
        2, 6, SO_END_LINE_INDEX,
        3, 7, SO_END_LINE_INDEX
    };

    SoDeferredRender* deferredRender = new SoDeferredRender(true);
    SoDrawStyle* drawStyle = new SoDrawStyle;
    SoCoordinate3* coords = new SoCoordinate3;
    SoLineSet* lineSet = new SoLineSet;
    SoMaterial* material = new SoMaterial;

    m_root->addChild(deferredRender);
    deferredRender->addChild(drawStyle);
    deferredRender->addChild(coords);
    deferredRender->addChild(material);
    deferredRender->addChild(lineSet);

    drawStyle->style = SoDrawStyle::LINES;
    drawStyle->linePattern.setValue(0xff00);
    material->transparency = 0.1;
    coords->point.setValues(0, 8, pts);
    //lineSet->coordIndex.setValues(0, 24, lineIndices);
    lineSet->numVertices.setValue(8);
}

void InventorEx::dotCircle()
{
    SoCoordinate3* coords = new SoCoordinate3;
    const int N = 1000; // 顶点数
    const float radius = 1.0f; // 圆的半径

    // 计算圆的顶点
    for (int i = 0; i <= N; ++i) {
        float angle = 2.0f * M_PI * i / N;
        coords->point.set1Value(i, radius * cos(angle), radius * sin(angle), 0.0f);
    }

    // 创建一个线集来绘制圆
    auto* lines = new SoLineSet;
    lines->numVertices.setValue(N + 1); // 加1是为了闭合圆

    SoDrawStyle* drawStyle = new SoDrawStyle;
    SoMaterial* material = new SoMaterial;

    // 组装场景图
    SoDeferredRender* deferredRender = new SoDeferredRender(true);
    m_root->addChild(deferredRender);
    deferredRender->addChild(coords);
    deferredRender->addChild(drawStyle);
    deferredRender->addChild(material);
    deferredRender->addChild(lines);

    drawStyle->style = SoDrawStyle::LINES;
    drawStyle->linePattern.setValue(0xf0);
    material->transparency = 0.1;

}

void InventorEx::fitPlane()
{
    float pts[4][3] = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { 0.0, 1.0, 0.0 },
    };
    int32_t faceIndices[8] = {
        0, 2, 1, SO_END_FACE_INDEX,
        0, 3, 2, SO_END_FACE_INDEX
    };

    SoCoordinate3* coords = new SoCoordinate3;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoShapeHints* shapeHints = new SoShapeHints;
    SoLightModel* lightModel = new SoLightModel;

    m_root->addChild(coords);
    m_root->addChild(shapeHints);
    m_root->addChild(lightModel);
    m_root->addChild(faceSet);

    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

    lightModel->model = SoLightModel::BASE_COLOR;

    coords->point.setValues(0, 4, pts);
    faceSet->coordIndex.setValues(0, 8, faceIndices);

    SoGetBoundingBoxAction action(m_viewer->getSoRenderManager()->getViewportRegion());
    action.apply(m_root);
    SbBox3f box = action.getBoundingBox();
    SbVec3f min = box.getMin();
    SbVec3f max = box.getMax();
}

SoSwitch* InventorEx::assembleBodySceneShader(const ShapeData& data)
{
    CREATE_NODE(SoSwitch, bodySwitch)
        CREATE_NODE(SoSeparator, body)
        CREATE_NODE(SoScale, scale)
        CREATE_NODE(SoSwitch, trasparencyTypeSwitch)
        CREATE_NODE(SoTransparencyType, trasparencyType)
        CREATE_NODE(SoSeparator, dataNode)
        CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoSwitch, renderModeSwitch)
        CREATE_NODE(SoSeparator, shadeWithEdge)
        CREATE_NODE(SoPolygonOffset, polygonOffset)
        CREATE_NODE(SoSeparator, faceRoot)
        CREATE_NODE(SoMaterial, faceMaterial)
        //CREATE_NODE(SoIndexedFaceSet, faceSet)
        CREATE_NODE(SoSeparator, lineRoot)
        CREATE_NODE(SoIndexedLineSet, lineSet)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {bodySwitch, body},
        {body, scale},
        {body, trasparencyTypeSwitch},
        {body, dataNode},
        {trasparencyTypeSwitch, trasparencyType},
        {dataNode, coords},
        {dataNode, renderModeSwitch},
        {renderModeSwitch, shadeWithEdge},
        {shadeWithEdge, polygonOffset},
        {shadeWithEdge, faceRoot},
        {shadeWithEdge, lineRoot},
        {faceRoot, faceMaterial},
        //{faceRoot, faceSet},
        {lineRoot, lineSet},
    };
    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

    bodySwitch->whichChild = 0;
    trasparencyTypeSwitch->whichChild = 0;
    renderModeSwitch->whichChild = 0;

    coords->point.setValues(0, data.points.size(), reinterpret_cast<const float(*)[3]>(data.points.data()));
    //faceSet->coordIndex.setValues(0, data.faceIndices.size(), data.faceIndices.data());
    lineSet->coordIndex.setValues(0, data.lineIndices.size(), data.lineIndices.data());

    return bodySwitch;
}

void InventorEx::performance()
{
    unsigned int count = 0;
    std::cout << "count:" << std::endl;
    std::cin >> count;
    std::vector<ShapeData> randomCuboids = generateRandomCuboids(count/*count*/, 5.0/*size*/);

    CREATE_NODE(SoSeparator, bodies)

        std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, new SoGradientBackground},
        {m_root, bodies},
    };

    int option = 0;
    std::cout << "0 for bodies\n1 for body\n";
    std::cin >> option;

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }
    if (0 == option)
    {
        for (const auto& data : randomCuboids)
        {
            ADD_CHILD(bodies, assembleBodySceneShader(data));
        }
    }
    else
    {
        ADD_CHILD(bodies, assembleSingleBodyScene(randomCuboids));
    }

    std::cout << "0 for unuse\n1 for use VBOs\n";
    std::cin >> option;
    if (1 == option)
    {
        SoReorganizeAction action;
        action.apply(m_root);
    }
    bodies->renderCaching = SoSeparator::ON;
    //m_root->renderCaching = SoSeparator::OFF;

    //SoNode* copy = m_root->copy();
}

SoSeparator* InventorEx::assembleSingleBodyScene(const std::vector<ShapeData>& cuboids) {
    CREATE_NODE(SoSeparator, body);
    CREATE_NODE(SoCoordinate3, coords);
    CREATE_NODE(SoIndexedFaceSet, faceSet);
    CREATE_NODE(SoMaterial, materials);
    CREATE_NODE(SoMaterialBinding, materialBinding);

    std::vector<SbVec3f> allPoints;
    std::vector<int32_t> allFaceIndices;
    std::vector<SbColor> allColors;

    for (const auto& data : cuboids) {
        // 计算当前点的偏移量
        int currentOffset = allPoints.size();

        // 添加点
        for (const auto& point : data.points) {
            allPoints.push_back(SbVec3f(point[0], point[1], point[2]));
        }

        // 添加面索引，并为每个面添加一个颜色
        for (int idx : data.faceIndices) {
            if (idx == SO_END_FACE_INDEX) {
                allFaceIndices.push_back(SO_END_FACE_INDEX);
                // 添加随机颜色
                allColors.push_back(SbColor(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
            }
            else {
                allFaceIndices.push_back(idx + currentOffset);
            }
        }
    }

    coords->point.setValues(0, allPoints.size(), allPoints.data());
    faceSet->coordIndex.setValues(0, allFaceIndices.size(), allFaceIndices.data());
    materials->diffuseColor.setValues(0, allColors.size(), allColors.data());
    materialBinding->value = SoMaterialBinding::PER_FACE;

    body->addChild(coords);
    body->addChild(materials);
    body->addChild(materialBinding);
    body->addChild(faceSet);

    return body;
}

void InventorEx::pointSet()
{
    CREATE_NODE(SoDrawStyle, drawStyle)
        CREATE_NODE(SoPointSet, pointSet)

        SoCallback* pointSmooth = new SoCallback;
    pointSmooth->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        }
                             });

    SoCallback* pointSmoothOff = new SoCallback;
    pointSmoothOff->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            glDisable(GL_POINT_SMOOTH);
        }
                                });


    ADD_CHILD(m_root, drawStyle);
    ADD_CHILD(m_root, pointSmooth);
    ADD_CHILD(m_root, pointSmoothOff);
    ADD_CHILD(m_root, pointSet);

    drawStyle->pointSize = 200.0f;
}

void InventorEx::customPolygonOffset()
{
    float pts[2][3] =
    {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
    };

    CREATE_NODE(SoCoordinate3, coords)
        CREATE_NODE(SoLineSet, lineSet)
        CREATE_NODE(SoMaterial, materialRed)
        CREATE_NODE(SoMaterial, materialYellow)
        CREATE_NODE(SoCallback, customPolygonOffsetNode)
        CREATE_NODE(SoCallback, disablePolygonOffsetNode)

        ADD_CHILD(m_root, coords);
    ADD_CHILD(m_root, materialRed);
    ADD_CHILD(m_root, customPolygonOffsetNode);
    ADD_CHILD(m_root, lineSet);
    ADD_CHILD(m_root, disablePolygonOffsetNode)
        ADD_CHILD(m_root, materialYellow);
    ADD_CHILD(m_root, lineSet);

    materialRed->diffuseColor.setValue(1, 0, 0);
    materialYellow->diffuseColor.setValue(1, 1, 0);

    coords->point.setValues(0, 2, pts);
    lineSet->numVertices.setValue(2);

    customPolygonOffsetNode->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glPolygonOffset(-1.0f, -1.0f);
            glEnable(GL_POLYGON_OFFSET_UNITS);
        }
                                         });

    disablePolygonOffsetNode->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glDisable(GL_POLYGON_OFFSET_UNITS);
        }
                                          });
}

void InventorEx::lightTest()
{
    CREATE_NODE(SoDirectionalLight, directionalLight);
    CREATE_NODE(SoMaterial, mat);
    CREATE_NODE(SoCube, cube);

    ADD_CHILD(m_root, directionalLight);
    ADD_CHILD(m_root, mat);
    ADD_CHILD(m_root, cube);

    directionalLight->direction.setValue(0, 0, 1);
    directionalLight->color.setValue(1, 1, 0);

    mat->ambientColor.setValue(0.0, 0.0, 0.0);
    mat->diffuseColor.setValue(0.0, 1.0, 0.0);
    mat->specularColor.setValue(0.0, 0.0, 0.0);
    mat->emissiveColor.setValue(0.0, 0.0, 0.0);

    m_viewer->setHeadlightEnabled(false);
}

void mouseDragCB(void* userData, SoEventCallback* eventCB) {
    SoSeparator* scene = (SoSeparator*)userData;
    const SoEvent* event = eventCB->getEvent();

    // 检查是否为鼠标拖动事件
    if (SO_MOUSE_RELEASE_EVENT(event, ANY)) {
        // 这里更新相机的位置和方向
        // 根据事件的细节（如鼠标的位置和移动方向）来旋转相机
        // ...

        // 更新相机后，重新计算包围盒
        SoGetBoundingBoxAction action(SbViewportRegion(800, 600));
        action.setInCameraSpace(true);
        action.apply(scene);
        //SbXfBox3f xbox = action.getXfBoundingBox();
        //SbBox3f box = xbox.project();
        SbBox3f box = action.getBoundingBox();

        std::cout << "boxSize: " << box.getMax()[0] - box.getMin()[0] << ' '
            << box.getMax()[1] - box.getMin()[1] << ' '
            << box.getMax()[2] - box.getMin()[2] << std::endl;
    }
}

void InventorEx::levelOfDetail()
{
    CREATE_NODE(SoLevelOfDetail, LOD)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoTranslation, trans)
        CREATE_NODE(SoEventCallback, eventCB)

        ADD_CHILD(m_root, LOD);
    ADD_CHILD(LOD, cube);
    ADD_CHILD(LOD, trans);
    ADD_CHILD(LOD, cube);
    ADD_CHILD(m_root, eventCB);

    m_viewer->setSceneGraph(m_root);
    m_viewer->show();

    trans->translation.setValue(2, 2, 2);

    m_root->renderCaching = false;
    m_root->boundingBoxCaching = false;

    eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mouseDragCB, m_viewer->getSoRenderManager()->getSceneGraph());
}

void InventorEx::OBB()
{
    CREATE_NODE(SoOrthographicCamera, cam)
        CREATE_NODE(SoCube, cube)
        CREATE_NODE(SoTranslation, trans)
        CREATE_NODE(SoEventCallback, eventCB)

        ADD_CHILD(m_root, cam);
    ADD_CHILD(m_root, cube)
        ADD_CHILD(m_root, trans)
        ADD_CHILD(m_root, cube)
        ADD_CHILD(m_root, eventCB)

        cam->position.setValue(0.0f, 0.0f, 5.0f);
    cam->nearDistance = 2.0f;
    cam->farDistance = 12.0f;

    m_viewer->setSceneGraph(m_root);
    m_viewer->show();

    trans->translation.setValue(2, 2, 2);

    eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mouseDragCB, m_viewer->getSoRenderManager()->getSceneGraph());
}

SoSeparator* gravWellSep(int gridSize/*网格的大小*/, float spacing/*网格的间距*/, float depthFactor/*深度因子，用于放大中心的"引力井"效果*/);
SoSeparator* cylFace(float height/*高度*/, float radius/*半径*/, int numSides/*柱面侧面数量*/);

void InventorEx::outline()
{
    // 保证线框不被深度测试冲突影响
    SoPolygonOffset* polygonOffset = new SoPolygonOffset;

    // 加粗线框
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyleElement::LINES;
    drawStyle->lineWidth = 2;
    SoLightModel* lightModel = new SoLightModel;
    lightModel->model = SoLightModel::BASE_COLOR;

    // 禁用写颜色缓冲
    SoColorMask* colorMask = new SoColorMask;
    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;

    // 禁用透明度，防止透明排序影响模板写入的时机
    SoTransparencyType* transparencyType = new SoTransparencyType;
    transparencyType->value = SoTransparencyType::NONE;

    int option = 0;
    std::cout << "0:text 1:cylinder 2:gravitationalWell 3:cylinederFace" << std::endl;
    std::cin >> option;
    SoSeparator* body = new SoSeparator;
    switch (option)
    {
        case 0:
        {
            SoFont* font = new SoFont;
            font->name.setValue("Courier-BoldOblique");
            font->size.setValue(10);
            body->addChild(font);
            SoText3* text = new SoText3;
            text->string.connectFrom(SoDB::getGlobalField("realTime"));
            body->addChild(text);
            break;
        }
        case 1:
        {
            SoCylinder* cylinder = new SoCylinder;
            cylinder->radius.setValue(3.0f);
            cylinder->height.setValue(10.0f);
            body->addChild(cylinder);
            break;
        }
        case 2:
        {
            body = gravWellSep(60, 1.0, 0.7);
            break;
        }
        case 3:
        {
            body = cylFace(10.0f, 3.0f, 60);
            break;
        }
        default:
            break;
    }

    // 创建模板回调
    SoCallback* stencilInit = new SoCallback;
    stencilInit->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            // Enable stencil test.
            glEnable(GL_STENCIL_TEST);
            glClearStencil(0);
            glClear(GL_STENCIL_BUFFER_BIT);
        }
                             });

    SoCallback* stencilWrite = new SoCallback;
    stencilWrite->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            // Configure stencil test to always pass and not use the value in the stencil buffer.
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            // Set stencil operation. Increment the stencil buffer value when depth test passes.
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        }
                              });

    SoCallback* stencilTest = new SoCallback;
    stencilTest->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            // Configure stencil test to pass only if the value in the stencil buffer is 0.
            glStencilFunc(GL_EQUAL, 0, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }
                             });

    SoCallback* stencilReset = new SoCallback;
    stencilReset->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            // Reset stencil buffer value to 0.
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }
                              });

    // 绘制模板
    SoSeparator* stencilWriteSep = new SoSeparator;
    stencilWriteSep->addChild(stencilWrite);
    stencilWriteSep->addChild(colorMask);
    stencilWriteSep->addChild(transparencyType);
    stencilWriteSep->addChild(body);
    stencilWriteSep->addChild(stencilReset);

    // 使用模板绘制边框
    SoSeparator* stencilTestSep = new SoSeparator;
    stencilTestSep->addChild(stencilTest);
    stencilTestSep->addChild(drawStyle);
    stencilTestSep->addChild(lightModel);
    stencilTestSep->addChild(body);
    stencilWriteSep->addChild(stencilReset);

    // 组合场景
    m_root->addChild(stencilInit);
    m_root->addChild(polygonOffset);
    m_root->addChild(stencilWriteSep);
    m_root->addChild(stencilTestSep);

    // question:
    // 轮廓线怎么做wireframe\hiddenLine
    // answer:
    // wireframe依赖于可见线写模板结果，轮廓线依赖于可见面写模板结果，这两个模板对隐藏线和轮廓线来说是冲突的
    // 那么，轮廓线是否可以脱离当前管线，作为纹理进行绘制，因为轮廓线并不需要整个场景的深度与模板信息，只需要当前物体的
    // 是否模板可以一个写1，一个写2？
    // glStencilFunc(GL_ALWAYS, 1, 0xFF); // 设置模板测试总是通过，写入的值为1
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 深度测试通过时替换模板值

    // question:
    // 轮廓线是否需要延迟渲染
    // answer:
    // 需要，模板柱面写2得到轮廓线，轮廓线与可见线相同，写1，然后延迟渲染中根据≠1进行模板测试

    // question:
    // 轮廓线能否全面代替可见线（解决线面离散不匹配）
    // answer:
    // 不行，隐藏线怎么生成？

    // question:
    // 上下离散圆与轮廓边的匹配

    // question:
    // 轮廓线与人为定义的边重合或理论重合时，该怎么处理
    // answer:
    // edge绘制可以反过来进行模板测试=2，但仍会有离散不匹配问题

    // question:
    // 空心圆柱内侧面边界如何绘制
    // answer：
    // NX不支持绘制，导入零件的内侧面边界是内侧实体的外轮廓

    // question:
    // 其他outline的渲染方式？
    // answer:
    // 需要的只有silhouette，其他类型的outline如countour不应该在需求中

    // question:
    // wireframe模式下的双线
    // answer:
    // 中间被模板掉了

    // 有一种解决方式，把silhouette用polygonOffset去写低一个深度，然后edge和silhouette组成的深度缓冲，通过它能判断出哪些位置是圆弧段，像扫雷一样
    // 不行，离散的不同导致深度上也有很大差别，并不能实现只写低一个深度。不过，如果离散相同，silhouette和edge一定是紧贴着的，这是个可以用来判断的特征

    // 既然lineWeight变粗会使轮廓线外扩，把模板变小是否就可以了（模板能变小吗）？
    // lineWeight无论如何设置都会超出正常渲染的边界
}

void InventorEx::outputBuffer()
{
    // 创建一个黄色的球
    SoSeparator* yellowSphere = new SoSeparator;
    yellowSphere->addChild(new SoMaterial);
    yellowSphere->addChild(new SoSphere);
    ((SoMaterial*)yellowSphere->getChild(0))->diffuseColor.setValue(1, 1, 0);

    // 创建一个红色的球，在2，2，2处
    SoSeparator* redSphere = new SoSeparator;
    redSphere->ref();
    redSphere->addChild(new SoTranslation);
    redSphere->addChild(new SoMaterial);
    redSphere->addChild(new SoSphere);
    ((SoTranslation*)redSphere->getChild(0))->translation.setValue(2, 2, 2);
    ((SoMaterial*)redSphere->getChild(1))->diffuseColor.setValue(1, 0, 0);

    // 黄色的球直接添加至root，红色的球写入buffer
    m_root->addChild(yellowSphere);

    SoOutput out;
    out.setBinary(TRUE);
    out.setBuffer(malloc(1024), 1024, (void* (*)(void*, size_t))realloc);
    SoWriteAction writeAction(&out);
    writeAction.apply(redSphere);

    void* buffer;
    size_t bufferSize;
    out.getBuffer(buffer, bufferSize);

    // 从buffer中读取数据
    SoInput in;
    in.setBuffer(buffer, bufferSize);
    SoSeparator* readSphere = SoDB::readAll(&in);
    if (readSphere)
        m_root->addChild(readSphere);

    redSphere->unref();
    free(buffer);
}

void InventorEx::cylinderFace()
{
    m_root->addChild(cylFace(5.0, 2.0, 20));
}

SoSeparator* cylFace(float height/*高度*/, float radius/*半径*/, int numSides/*柱面侧面数量*/)
{
    SoSeparator* root = new SoSeparator;

    // 创建坐标节点
    SoCoordinate3* coords = new SoCoordinate3;
    root->addChild(coords);

    SoShapeHints* shapeHints = new SoShapeHints;
    root->addChild(shapeHints);
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::SOLID;

    // 计算并添加顶点坐标
    SbVec3f* vertices = new SbVec3f[2 * numSides];
    for (int i = 0; i < numSides; ++i) {
        float angle = 2.0f * M_PI * i / numSides;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices[2 * i] = SbVec3f(x, height, z);     // 顶部顶点
        vertices[2 * i + 1] = SbVec3f(x, 0.0f, z);   // 底部顶点
    }
    coords->point.setValues(0, 2 * numSides, vertices);

    // 创建索引面集
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    root->addChild(faceSet);

    // 计算并添加索引
    int* indices = new int[5 * numSides];
    for (int i = 0; i < numSides; ++i) {
        int next = (i + 1) % numSides;
        indices[5 * i] = 2 * i;
        indices[5 * i + 1] = 2 * next;
        indices[5 * i + 2] = 2 * next + 1;
        indices[5 * i + 3] = 2 * i + 1;
        indices[5 * i + 4] = SO_END_FACE_INDEX;
    }
    faceSet->coordIndex.setValues(0, 5 * numSides, indices);

    // 清理
    delete[] vertices;
    delete[] indices;

    return root;
}

void InventorEx::gravitationalWell()
{
    m_root->addChild(gravWellSep(20, 1.0, 0.7));
}

SoSeparator* gravWellSep(int gridSize/*网格的大小*/, float spacing/*网格的间距*/, float depthFactor/*深度因子，用于放大中心的"引力井"效果*/)
{
    SoSeparator* root = new SoSeparator;

    // 创建一个坐标节点
    SoCoordinate3* coords = new SoCoordinate3;
    root->addChild(coords);

    // 生成曲面上的点
    SbVec3f* points = new SbVec3f[(gridSize + 1) * (gridSize + 1)];
    for (int i = 0; i <= gridSize; ++i) {
        for (int j = 0; j <= gridSize; ++j) {
            float x = (i - gridSize / 2.0f) * spacing;
            float y = (j - gridSize / 2.0f) * spacing;
            // 计算高度值为距离中心的函数
            float distance = sqrt(x * x + y * y);
            float z = -depthFactor * distance; // 简化的"引力井"效果
            points[i * (gridSize + 1) + j] = SbVec3f(x, y, z);
        }
    }
    coords->point.setValues(0, (gridSize + 1) * (gridSize + 1), points);

    // 创建面集索引
    SoIndexedFaceSet* faces = new SoIndexedFaceSet;
    root->addChild(faces);

    // 生成面的索引
    int numFaces = gridSize * gridSize * 2; // 每个网格两个三角形
    int32_t* indices = new int32_t[numFaces * 4]; // 每个面四个索引（三个顶点加一个结束标志）
    int idx = 0;
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            // 计算四个角的索引
            int p0 = i * (gridSize + 1) + j;
            int p1 = p0 + 1;
            int p2 = p0 + (gridSize + 1);
            int p3 = p2 + 1;

            // 第一个三角形
            indices[idx++] = p0;
            indices[idx++] = p2;
            indices[idx++] = p1;
            indices[idx++] = SO_END_FACE_INDEX;

            // 第二个三角形
            indices[idx++] = p1;
            indices[idx++] = p2;
            indices[idx++] = p3;
            indices[idx++] = SO_END_FACE_INDEX;
        }
    }
    faces->coordIndex.setValues(0, numFaces * 4, indices);

    // 清理分配的内存
    delete[] points;
    delete[] indices;

    return root;
}


void InventorEx::outline2()
{
    // 加粗线框
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyleElement::LINES;
    drawStyle->lineWidth = 2;
    SoLightModel* lightModel = new SoLightModel;
    lightModel->model = SoLightModel::BASE_COLOR;

    SoPolygonOffset* polygonOffset = new SoPolygonOffset;
    polygonOffset->factor = -2.0f;
    polygonOffset->units = -2.0f;

    SoCallback* glPolygonOffsetCB = new SoCallback;
    glPolygonOffsetCB->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(2.0f, 2.0f);
        }
                                   });


    // 禁用写颜色缓冲
    SoColorMask* colorMask = new SoColorMask;
    colorMask->red = FALSE;
    colorMask->green = FALSE;
    colorMask->blue = FALSE;
    colorMask->alpha = FALSE;

    int option = 0;
    std::cout << "0:text 1:cylinder 2:gravitationalWell 3:cylinederFace" << std::endl;
    std::cin >> option;
    SoSeparator* body = new SoSeparator;
    switch (option)
    {
        case 0:
        {
            SoFont* font = new SoFont;
            font->name.setValue("Courier-BoldOblique");
            font->size.setValue(10);
            body->addChild(font);
            SoText3* text = new SoText3;
            text->string.connectFrom(SoDB::getGlobalField("realTime"));
            body->addChild(text);
            break;
        }
        case 1:
        {
            SoCylinder* cylinder = new SoCylinder;
            cylinder->radius.setValue(3.0f);
            cylinder->height.setValue(10.0f);
            body->addChild(cylinder);
            break;
        }
        case 2:
        {
            body = gravWellSep(60, 1.0, 0.7);
            break;
        }
        case 3:
        {
            body = cylFace(10.0f, 3.0f, 60);
            break;
        }
        default:
            break;
    }

    SoCallback* stencilInit = new SoCallback();
    stencilInit->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glEnable(GL_STENCIL_TEST);
            glClearStencil(0);
            glClear(GL_STENCIL_BUFFER_BIT);
        }
                             });
    m_root->addChild(stencilInit);

    SoSeparator* stencilWriteSep = new SoSeparator;
    stencilWriteSep->addChild(colorMask);
    //stencilWriteSep->addChild(polygonOffset);
    stencilWriteSep->addChild(body);
    m_root->addChild(stencilWriteSep);

    SoSeparator* stencilTestSep = new SoSeparator;
    stencilTestSep->addChild(drawStyle);
    stencilTestSep->addChild(lightModel);
    stencilTestSep->addChild(body);
    m_root->addChild(stencilTestSep);
}

void InventorEx::autoZoom()
{
    SoSeparator* testRoot = new SoSeparator;
    SoSeparator* test1 = new SoSeparator;
    SoSeparator* test2 = new SoSeparator;
    SoSeparator* test3 = new SoSeparator;

    SoSphere* sphere = new SoSphere;
    sphere->radius = 1.0f;

    SoMatrixTransform* trans = new SoMatrixTransform;
    SbMatrix mat;
    mat.setTranslate(SbVec3f(0.0f, 10.0f, 0.0f));
    trans->matrix.setValue(mat);

    SoZoomAdaptor* zoomAdaptor = new SoZoomAdaptor;

    SoMaterial* matR = new SoMaterial;
    matR->diffuseColor.setValue(1.0f, 0.0f, 0.0f);

    SoMaterial* matG = new SoMaterial;
    matG->diffuseColor.setValue(0.0f, 1.0f, 0.0f);

    SoMaterial* matB = new SoMaterial;
    matB->diffuseColor.setValue(0.0f, 0.0f, 1.0f);

    m_root->addChild(new SoOrthographicCamera);
    m_root->addChild(testRoot);
    testRoot->addChild(sphere);
    testRoot->addChild(test1);
    testRoot->addChild(test2);
    testRoot->addChild(test3);
    test1->addChild(matR);
    test1->addChild(zoomAdaptor);
    test1->addChild(sphere);
    test2->addChild(matG);
    test2->addChild(trans);
    test2->addChild(zoomAdaptor);
    test2->addChild(sphere);
    test3->addChild(matB);
    test3->addChild(zoomAdaptor);
    test3->addChild(trans);
    test3->addChild(sphere);
}

void InventorEx::shaderProgram()
{
    // 创建着色器程序节点
    SoShaderProgram* shaderProgram = new SoShaderProgram;

    // 创建并设置顶点着色器
    SoVertexShader* vertexShader = new SoVertexShader;
    vertexShader->sourceProgram.setValue("perpixel_vertex.glsl");
    shaderProgram->shaderObject.set1Value(0, vertexShader);

    // 创建并设置片元着色器
    SoFragmentShader* fragmentShader = new SoFragmentShader;
    fragmentShader->sourceProgram.setValue("perpixel_fragment.glsl");
    shaderProgram->shaderObject.set1Value(1, fragmentShader);

    // 添加着色器程序到根节点
    m_root->addChild(shaderProgram);

    // 设置材质和光照参数
    SoMaterial* material = new SoMaterial;
    material->ambientColor.setValue(0.0, 0.0, 0.0);
    material->diffuseColor.setValue(0.0, 1.0, 0.0);
    material->specularColor.setValue(1.0, 1.0, 1.0);
    material->emissiveColor.setValue(0.0, 0.0, 0.0);
    material->shininess.setValue(0.9); // 光泽度
    m_root->addChild(material);

    SoCube* cube = new SoCube;
    cube->width = 2;
    cube->height = 2;
    cube->depth = 2;

    SoSphere* sphere = new SoSphere;
    sphere->radius = 1.2;

    //SoColorMask* colorMask = new SoColorMask;
    //colorMask->red = FALSE;
    //colorMask->green = FALSE;
    //colorMask->blue = FALSE;
    //colorMask->alpha = FALSE;
    //m_root->addChild(colorMask);

    SoDirectionalLight* myDirLight = new SoDirectionalLight;
    myDirLight->direction.setValue(0, 0, -1);
    myDirLight->color.setValue(1, 0, 0);
    m_root->addChild(myDirLight);
    m_root->addChild(cube);
    m_root->addChild(sphere);

    m_viewer->setHeadlightEnabled(false);
}

void InventorEx::shaderHightlight()
{
    // 创建着色器程序节点
    SoShaderProgram* shaderProgram = new SoShaderProgram();

    // 创建并设置顶点着色器
    SoVertexShader* vertexShader = new SoVertexShader();
    vertexShader->sourceProgram.setValue("vertex_shader2.glsl");
    shaderProgram->shaderObject.set1Value(0, vertexShader);

    // 创建并设置片元着色器
    SoFragmentShader* fragmentShader = new SoFragmentShader();
    fragmentShader->sourceProgram.setValue("highlight_fragment_shader2.glsl");
    shaderProgram->shaderObject.set1Value(1, fragmentShader);

    // 设置高亮颜色
    //SoShaderParameter4f* highlightColor = new SoShaderParameter4f;
    //highlightColor->name.setValue("highlightColor"); // 这里的名字要和着色器中的uniform变量名一致
    //highlightColor->value.setValue(1.0, 1.0, 0.0, 1.0); // 设置为黄色
    //fragmentShader->parameter.set1Value(0, highlightColor); // 将这个参数传递给片元着色器

    SoSeparator* defaultCube = new SoSeparator;
    SoSeparator* highlightCube = new SoSeparator;

    SoCube* cube = new SoCube;
    defaultCube->addChild(cube);

    SoMaterial* highlightMaterial = new SoMaterial;
    highlightMaterial->diffuseColor.setValue(0, 0, 1);
    SoTranslation* translation = new SoTranslation;
    translation->translation.setValue(1, 0, 0);
    highlightCube->addChild(shaderProgram); // 注意：shaderProgram 应当在设置所有参数后添加到场景中
    highlightCube->addChild(translation);
    highlightCube->addChild(highlightMaterial);
    highlightCube->addChild(cube);

    m_root->addChild(defaultCube);
    m_root->addChild(highlightCube);
}

void InventorEx::zfighting()
{
    SoDepthBuffer* depthBuffer = new SoDepthBuffer;
    depthBuffer->function = SoDepthBuffer::LESS;
    m_root->addChild(depthBuffer);

    // 创建两个材质节点
    SoMaterial* mat1 = new SoMaterial;
    mat1->diffuseColor.setValue(1, 0, 0);
    SoMaterial* mat2 = new SoMaterial;
    mat2->diffuseColor.setValue(0, 0, 1);

    // 创建两个立方体
    SoSeparator* sep1 = new SoSeparator;
    sep1->addChild(mat1);
    sep1->addChild(new SoSphere);
    SoSeparator* sep2 = new SoSeparator;
    sep2->addChild(mat2);
    sep2->addChild(new SoSphere);
    m_root->addChild(sep1);
    m_root->addChild(sep2);
}

// Callback function to terminate action
//void terminateActionCallback(void* userData, SoAction* action)
//{
//    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
//        // Terminate the action, preventing the rendering of this node and its children
//        action->setTerminated(TRUE);
//    }
//}

// 回调函数，用于决定是否继续遍历子树
static SoCallbackAction::Response renderCallback(void* userData, SoCallbackAction* action, const SoNode* node)
{
    // 检查当前的Action是否为SoGLRenderAction
    if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
    {
        // 如果是渲染Action，继续遍历子树
        return SoCallbackAction::CONTINUE;
    }
    else {
        // 如果是其他类型的Action，停止遍历这个子树
        return SoCallbackAction::PRUNE;
    }
}

void InventorEx::terminate()
{
    // Add a cube
    SoCube* cube = new SoCube;
    m_root->addChild(cube);

    SoSeparator* sep = new SoSeparator;

    // Add a callback node to terminate the action
    //SoCallback* callback = new SoCallback;
    //callback->setCallback(renderCallback);
    //sep->addChild(callback);

    // Add a sphere - this should not be rendered
    SoSphere* sphere = new SoSphere;
    sep->addChild(sphere);

    m_root->addChild(sep);

    // Add a cylinder - this should be rendered
    SoCylinder* cylinder = new SoCylinder;
    m_root->addChild(cylinder);
}

void InventorEx::materialIndex()
{
    float vertices[8][3] = {
    0.0f, 0.0f, 0.0f, // 0
    1.0f, 0.0f, 0.0f, // 1
    1.0f, 1.0f, 0.0f, // 2
    0.0f, 1.0f, 0.0f, // 3
    0.0f, 0.0f, 1.0f, // 4
    1.0f, 0.0f, 1.0f, // 5
    1.0f, 1.0f, 1.0f, // 6
    0.0f, 1.0f, 1.0f  // 7
    };

    // 使用三角形定义立方体面，每个三角形以-1结尾
    int32_t indicesTri[48] = {
        0, 1, 2, -1, 0, 2, 3, -1, // 底
        4, 7, 6, -1, 4, 6, 5, -1, // 顶
        0, 4, 5, -1, 0, 5, 1, -1, // 前
        3, 2, 6, -1, 3, 6, 7, -1, // 后
        0, 3, 7, -1, 0, 7, 4, -1, // 左
        1, 5, 6, -1, 1, 6, 2, -1  // 右
    };

    int32_t materialIndices[24] =
    {
        0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0
    };

    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    // 创建材质绑定节点
    SoMaterialBinding* materialBinding = new SoMaterialBinding;
    materialBinding->value = SoMaterialBinding::PER_FACE_INDEXED;

    // 创建颜色和透明度节点，以及默认材质
    SoMaterial* materials = new SoMaterial;
    materials->diffuseColor.setValues(0, 3, new SbColor[3]{ {0.8, 0.8, 0.8}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0} }); // 默认，红色，绿色
    materials->transparency.setValues(0, 3, new float[3] {0.0, 0.5, 0.5}); // 默认不透明，红绿透明

    // 第一个立方体和第二个立方体的顶点
    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.setValues(0, 8, vertices);

    // 第一个立方体
    SoIndexedFaceSet* cube1 = new SoIndexedFaceSet;
    cube1->coordIndex.setValues(0, 48, indicesTri);
    cube1->materialIndex.setValues(0, 12, materialIndices); // 设置特殊材料的面

    // 第二个立方体，平移后的
    SoTranslation* translation = new SoTranslation;
    translation->translation.setValue(0.5f, 0.5f, 0.0f);

    SoIndexedFaceSet* cube2 = new SoIndexedFaceSet;
    cube2->coordIndex.setValues(0, 48, indicesTri);
    cube2->materialIndex.setValues(0, 12, materialIndices + 12); // 设置特殊材料的面

    // 构建场景图
    SoSeparator* customScene = new SoSeparator;
    customScene->addChild(shapeHints);
    customScene->addChild(materials); // 添加材质
    customScene->addChild(materialBinding); // 添加材质绑定
    customScene->addChild(coords); // 添加顶点
    customScene->addChild(cube1); // 添加第一个立方体
    customScene->addChild(translation); // 添加平移，对第二个立方体生效
    customScene->addChild(cube2); // 添加第二

    m_root->addChild(customScene);

    //m_viewer->getSoRenderManager()->getGLRenderAction()->setTransparentDelayedObjectRenderType(SoGLRenderAction::NONSOLID_SEPARATE_BACKFACE_PASS);

}

void InventorEx::divideTransp()
{
    float vertices[8][3] = {
    0.0f, 0.0f, 0.0f, // 0
    1.0f, 0.0f, 0.0f, // 1
    1.0f, 1.0f, 0.0f, // 2
    0.0f, 1.0f, 0.0f, // 3
    0.0f, 0.0f, 1.0f, // 4
    1.0f, 0.0f, 1.0f, // 5
    1.0f, 1.0f, 1.0f, // 6
    0.0f, 1.0f, 1.0f  // 7
    };

    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    // 创建材质绑定节点
    SoMaterialBinding* materialBinding = new SoMaterialBinding;
    materialBinding->value = SoMaterialBinding::PER_FACE_INDEXED;

    // 创建颜色和透明度节点，以及默认材质
    SoMaterial* materials = new SoMaterial;
    materials->diffuseColor.setValues(0, 3, new SbColor[3]{ {0.8, 0.8, 0.8}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0} }); // 默认，红色，绿色
    materials->transparency.setValues(0, 3, new float[3] {0.0, 0.5, 0.5}); // 默认不透明，红绿透明

    // 第一个立方体和第二个立方体的顶点
    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.setValues(0, 8, vertices);

    // 第二个立方体，平移后的
    SoTranslation* translation = new SoTranslation;
    translation->translation.setValue(0.5f, 0.5f, 0.0f);

    // 构建场景图
    SoSeparator* customScene = new SoSeparator;

    // 定义单个面的顶点索引
    int32_t faceIndices[6][8] = {
        {0, 1, 2, -1, 0, 2, 3, -1}, // 底
        {4, 7, 6, -1, 4, 6, 5, -1}, // 顶
        {0, 4, 5, -1, 0, 5, 1, -1}, // 前
        {3, 2, 6, -1, 3, 6, 7, -1}, // 后
        {0, 3, 7, -1, 0, 7, 4, -1}, // 左
        {1, 5, 6, -1, 1, 6, 2, -1}  // 右
    };

    int32_t faceMaterialIndicesRed[12] = { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 };
    int32_t faceMaterialIndicesGreen[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0 };

    // 创建场景根节点
    m_root->addChild(shapeHints);
    m_root->addChild(materials);
    m_root->addChild(materialBinding);
    m_root->addChild(coords);

    // 添加立方体1的每个面
    for (int i = 0; i < 6; i++) {
        SoSeparator* faceSep = new SoSeparator;
        SoIndexedFaceSet* face = new SoIndexedFaceSet;
        face->coordIndex.setValues(0, 8, faceIndices[i]);
        face->materialIndex.setValues(0, 2, &faceMaterialIndicesRed[2 * i]); // 设置每个面的材质索引
        faceSep->addChild(face);
        m_root->addChild(faceSep); // 将面添加到场景中
    }

    // 第二个立方体的变换
    SoSeparator* cube2Sep = new SoSeparator;
    cube2Sep->addChild(translation); // 对第二个立方体进行平移

    // 添加立方体2的每个面
    for (int i = 0; i < 6; i++) {
        SoSeparator* faceSep = new SoSeparator;
        SoIndexedFaceSet* face = new SoIndexedFaceSet;
        face->coordIndex.setValues(0, 8, faceIndices[i]);
        face->materialIndex.setValues(0, 2, &faceMaterialIndicesGreen[2 * i]); // 设置每个面的材质索引
        faceSep->addChild(face);
        cube2Sep->addChild(faceSep); // 将面添加到场景中
    }

    m_root->addChild(cube2Sep); // 将第二个立方体添加到场景中

    //m_viewer->getSoRenderManager()->getGLRenderAction()->setNumPasses(4);
    m_viewer->getSoRenderManager()->getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_LAYERS_BLEND);
    //m_viewer->getSoRenderManager()->getGLRenderAction()->setSortedLayersNumPasses(4);
    //m_viewer->getSoRenderManager()->getGLRenderAction()->setTransparentDelayedObjectRenderType(SoGLRenderAction::NONSOLID_SEPARATE_BACKFACE_PASS);

}

void InventorEx::text2()
{
    SoText2* text = new SoText2;
    text->string.setValue("Hello, World!");
    text->justification = SoText2::CENTER;

    SoFont* textFont = new SoFont();
    textFont->name.setValue("Arial");
    textFont->size.setValue(120.0f);

    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(1, 0, 0);
    mat->transparency = 0.6;

    //SoGradientBackground* bc = new SoGradientBackground;
    //bc->color0 = SbColor(1.0f, 1.0f, 1.0f);
    //bc->color1 = SbColor(1.0f, 1.0f, 1.0f);

    //m_root->addChild(bc);
    m_root->addChild(mat);
    m_root->addChild(textFont);
    m_root->addChild(text);
}

void InventorEx::twoLine()
{
    SoDepthBuffer* depthBuffer = new SoDepthBuffer;
    depthBuffer->range.setValue(0, 0.99f);
    m_root->addChild(depthBuffer);

    // 定义坐标点
    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.set1Value(0, SbVec3f(0, 0, 0));
    coords->point.set1Value(1, SbVec3f(1, 0, 0));
    coords->point.set1Value(2, SbVec3f(3, 0, 0));
    coords->point.set1Value(3, SbVec3f(4, 0, 0));
    m_root->addChild(coords);

    // 创建材料节点（绿色虚线）
    SoMaterial* mat1 = new SoMaterial;
    mat1->diffuseColor.setValue(0, 1, 0);  // 绿色
    m_root->addChild(mat1);

    // 创建绘制样式节点（虚线）
    SoDrawStyle* style1 = new SoDrawStyle;
    style1->linePattern = 0x00FF;  // 点画线
    style1->lineWidth = 2;
    m_root->addChild(style1);

    // 创建索引线集（虚线）
    SoIndexedLineSet* ils1 = new SoIndexedLineSet;
    ils1->coordIndex.set1Value(0, 0);
    ils1->coordIndex.set1Value(1, 2);
    m_root->addChild(ils1);

    SoDepthBuffer* depthBuffer2 = new SoDepthBuffer;
    depthBuffer2->range.setValue(0, 1.0f);
    m_root->addChild(depthBuffer2);

    // 创建材料节点（白色实线）
    SoMaterial* mat2 = new SoMaterial;
    mat2->diffuseColor.setValue(1, 1, 1);  // 白色
    m_root->addChild(mat2);

    // 创建绘制样式节点（实线）
    SoDrawStyle* style2 = new SoDrawStyle;
    style2->linePattern = 0xFFFF;  // 实线
    style2->lineWidth = 2;
    m_root->addChild(style2);

    // 创建索引线集（实线）
    SoIndexedLineSet* ils2 = new SoIndexedLineSet;
    ils2->coordIndex.set1Value(0, 1);
    ils2->coordIndex.set1Value(1, 3);
    m_root->addChild(ils2);
}

void InventorEx::drtest()
{
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
    SoSeparator* body = new SoSeparator;
    SoSeparator* dataNode = new SoSeparator;
    SoCoordinate3* coords = new SoCoordinate3;
    SoSeparator* staticWireFrame = new SoSeparator;
    SoSeparator* faceRoot = new SoSeparator;
    SoSeparator* lineVisbleRoot = new SoSeparator;
    SoSeparator* lineHiddenRoot = new SoSeparator;
    SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
    SoIndexedLineSet* lineVisibleSet = new SoIndexedLineSet;
    SoMaterial* faceMaterial = new SoMaterial;
    SoMaterial* lineMaterial = new SoMaterial;

    SoDepthBuffer* depthBufferF = new SoDepthBuffer;
    depthBufferF->range.setValue(0, 1.0f);

    SoDepthBuffer* depthBufferL = new SoDepthBuffer;
    depthBufferL->range.setValue(0.5f, 0.51f);

    m_root->addChild(body);
    body->addChild(dataNode);
    dataNode->addChild(coords);
    dataNode->addChild(staticWireFrame);
    staticWireFrame->addChild(faceRoot);
    staticWireFrame->addChild(lineVisbleRoot);
    staticWireFrame->addChild(lineHiddenRoot);
    faceRoot->addChild(faceMaterial);
    faceRoot->addChild(depthBufferF);
    faceRoot->addChild(faceSet);
    lineVisbleRoot->addChild(lineMaterial);
    lineVisbleRoot->addChild(depthBufferL);
    lineVisbleRoot->addChild(lineVisibleSet);

    faceMaterial->diffuseColor.setValue(0, 1.0f, 0);
    faceMaterial->transparency = 0.0f;
    lineMaterial->diffuseColor.setValue(1.0f, 0, 0);

    coords->point.setValues(0, 8, pts);
    faceSet->coordIndex.setValues(0, 48, faceIndices);
    lineVisibleSet->coordIndex.setValues(0, 24, lineIndices);
}

void InventorEx::billBoard()
{
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->pointSize = 5.0f;
    m_root->addChild(drawStyle);
    SoPointSet* point = new SoPointSet;
    m_root->addChild(point);
}

void InventorEx::texture()
{
    // Choose a texture
    SoTexture2* textureNode = new SoTexture2;
    // A 3-by-2 array of black and white pixels; the array is
    //upside-down here (the first pixel is the lower left corner)
    unsigned char image[] = {
     255, 0,
     0, 255,
     255, 0
    };
    //Set the image field:
    textureNode->image.setValue(SbVec2s(3, 2), 1, image);
    m_root->addChild(textureNode);

    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(1, 0, 0);
    m_root->addChild(mat);

    // Make a cube
    m_root->addChild(new SoCube);
}

void InventorEx::transTypeNone()
{
    SoTransparencyType* transType = new SoTransparencyType;
    transType->value = SoTransparencyType::NONE;
    m_root->addChild(transType);

    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    m_root->addChild(shapeHints);

    //SoLightModel* lightModel = new SoLightModel;
    //lightModel->model = SoLightModel::BASE_COLOR;
    //m_root->addChild(lightModel);

    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(1, 0, 0);
    mat->transparency = 0.5f;
    m_root->addChild(mat);

    SoCube* cube = new SoCube;
    m_root->addChild(cube);

    m_viewer->getSoRenderManager()->getGLRenderAction()->setSmoothing(TRUE);
}

void InventorEx::transLine()
{
    SoMaterial* mat = new SoMaterial;
    mat->transparency.setValue(1.0f);

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
    int32_t lineIndices[24] = {
        0, 1, 2, 3, 0, SO_END_LINE_INDEX,
        4, 5, 6, 7, 4, SO_END_LINE_INDEX,
        0, 4, SO_END_LINE_INDEX,
        1, 5, SO_END_LINE_INDEX,
        2, 6, SO_END_LINE_INDEX,
        3, 7, SO_END_LINE_INDEX
    };

    SoDrawStyle* drawStyle = new SoDrawStyle;
    SoLightModel* lightModel = new SoLightModel;
    SoCoordinate3* coords = new SoCoordinate3;
    SoLineSet* lineSet = new SoLineSet;
    SoMaterial* material = new SoMaterial;

    m_root->addChild(drawStyle);
    m_root->addChild(lightModel);
    m_root->addChild(coords);
    m_root->addChild(material);
    m_root->addChild(lineSet);

    drawStyle->style = SoDrawStyle::LINES;
    drawStyle->lineWidth = 6.0f;
    //drawStyle->linePattern.setValue(0xff00);
    lightModel->model = SoLightModel::BASE_COLOR;
    material->transparency = 0.5;
    coords->point.setValues(0, 8, pts);
    //lineSet->coordIndex.setValues(0, 24, lineIndices);
    lineSet->numVertices.setValue(8);
}

SoShaderProgram* loadShader(int width, int height,
                            float a1, float a2, float b1, float b2,
                            float m1, float m2,
                            float n11, float n12, float n13,
                            float n21, float n22, float n23)
{
    SoVertexShader* vertexShader = new SoVertexShader();
    SoGeometryShader* geometryShader = new SoGeometryShader();
    SoFragmentShader* fragmentShader = new SoFragmentShader();

    vertexShader->sourceProgram =
        "vertexShader.glsl";
    geometryShader->sourceProgram =
        "geometryShader.glsl";
    fragmentShader->sourceProgram =
        "fragmentShader.glsl";

    SoShaderProgram* shaderProgram = new SoShaderProgram;
    shaderProgram->shaderObject.set1Value(0, vertexShader);
    shaderProgram->shaderObject.set1Value(1, geometryShader);
    shaderProgram->shaderObject.set1Value(2, fragmentShader);

    SoShaderParameter2f* ab1 = new SoShaderParameter2f;
    SoShaderParameter2f* ab2 = new SoShaderParameter2f;

    SoShaderParameter1i* w = new SoShaderParameter1i;
    SoShaderParameter1i* h = new SoShaderParameter1i;


    SoShaderParameter4f* param1 = new SoShaderParameter4f;
    SoShaderParameter4f* param2 = new SoShaderParameter4f;

    w->name = "width";
    w->value = width;

    h->name = "height";
    h->value = height;

    ab1->name = "ab1";
    ab1->value.setValue(SbVec2f(a1, b1));
    ab2->name = "ab2";
    ab2->value.setValue(SbVec2f(a2, b2));
    param1->name = "param1";
    param1->value.setValue(SbVec4f(m1, n11, n12, n13));
    param2->name = "param2";
    param2->value.setValue(SbVec4f(m2, n21, n22, n23));

    geometryShader->parameter.set1Value(0, ab1);
    geometryShader->parameter.set1Value(1, ab2);
    geometryShader->parameter.set1Value(2, param1);
    geometryShader->parameter.set1Value(3, param2);
    geometryShader->parameter.set1Value(4, w);
    geometryShader->parameter.set1Value(5, h);

    return shaderProgram;
}

SoShaderProgram* loadShader(int width, int height, float a, float b, float m,
                            float n1, float n2, float n3)
{
    return loadShader(width, height, a, a, b, b, m, m, n1, n2, n3, n1, n2, n3);
}

void InventorEx::shaderParam()
{
    SoShaderProgram* shaderProgram = loadShader(100, 100, 1.f, 1.f, 5.2f, 0.04f, 1.7f, 1.7f);

    m_root->addChild(shaderProgram);
}

SoSeparator* createRing()
{
    SoSeparator* root = new SoSeparator;

    // 定义圆环参数
    const int num_major = 40; // 主圆（大圆）分段数
    const int num_minor = 20; // 副圆（小圆）分段数
    const float major_radius = 1.0f; // 主圆半径
    const float minor_radius = 0.3f; // 副圆半径

    const int total_vertices = num_major * num_minor;
    SbVec3f* vertexPositions = new SbVec3f[total_vertices];

    // 计算顶点坐标
    for (int i = 0; i < num_major; ++i) {
        float theta = 2.0f * M_PI * i / num_major;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int j = 0; j < num_minor; ++j) {
            float phi = 2.0f * M_PI * j / num_minor;
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);

            float x = (major_radius + minor_radius * cosPhi) * cosTheta;
            float y = (major_radius + minor_radius * cosPhi) * sinTheta;
            float z = minor_radius * sinPhi;

            vertexPositions[i * num_minor + j] = SbVec3f(x, y, z);
        }
    }

    // 定义面索引
    const int faces = (num_major) * (num_minor);
    const int indices_per_face = 4 + 1; // 4个顶点 + 1个SO_END_FACE_INDEX
    const int total_indices = faces * indices_per_face;
    int* indices = new int[total_indices];
    int idx = 0;

    for (int i = 0; i < num_major; ++i) {
        for (int j = 0; j < num_minor; ++j) {
            int current = i * num_minor + j;
            int next = ((i + 1) % num_major) * num_minor + j;
            int nextMinor = i * num_minor + (j + 1) % num_minor;
            int nextBoth = ((i + 1) % num_major) * num_minor + (j + 1) % num_minor;

            indices[idx++] = current;
            indices[idx++] = next;
            indices[idx++] = nextBoth;
            indices[idx++] = nextMinor;
            indices[idx++] = SO_END_FACE_INDEX;
        }
    }

    // 定义颜色
    SoMaterial* myMaterials = new SoMaterial;
    myMaterials->diffuseColor.setValue(0.0f, 0.5f, 1.0f); // 蓝色
    root->addChild(myMaterials);

    // 设置材质绑定
    SoMaterialBinding* myMaterialBinding = new SoMaterialBinding;
    myMaterialBinding->value = SoMaterialBinding::OVERALL;
    root->addChild(myMaterialBinding);

    // 定义顶点坐标
    SoCoordinate3* myCoords = new SoCoordinate3;
    myCoords->point.setValues(0, total_vertices, vertexPositions);
    root->addChild(myCoords);

    // 定义索引面集
    SoIndexedFaceSet* myFaceSet = new SoIndexedFaceSet;
    myFaceSet->coordIndex.setValues(0, total_indices, indices);
    root->addChild(myFaceSet);

    // 释放内存
    delete[] vertexPositions;
    delete[] indices;

    return root;
}

void InventorEx::ring()
{
    SoClipPlane* clipPlane = new SoClipPlane;
    clipPlane->plane.setValue(SbPlane(SbVec3f(0, 1, 0), -0.9));

    SoShapeHints* twoSidedLighting = new SoShapeHints;
    twoSidedLighting->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    twoSidedLighting->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    SoShapeHints* backFaceCulling = new SoShapeHints;
    backFaceCulling->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    backFaceCulling->shapeType = SoShapeHints::SOLID;

    SoShapeHints* frontFaceCulling = new SoShapeHints;
    frontFaceCulling->vertexOrdering = SoShapeHints::CLOCKWISE;
    frontFaceCulling->shapeType = SoShapeHints::SOLID;


    m_root->addChild(clipPlane);
    m_root->addChild(twoSidedLighting);
    m_root->addChild(createRing());
}

#include <QtSvg/QSvgRenderer>

QImage RenderSvgToImage(const QString& svgPath, const QRect& targetRect)
{
    QImage image(WINDOWWIDTH, WINDOWHEIGHT, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QSvgRenderer renderer(svgPath);
    renderer.render(&painter, targetRect);

    return image;
}
GLuint texId = 0;

void InventorEx::section()
{
    SoSeparator* ring = createRing();
    //SoSeparator* ring = new SoSeparator;

    //SoTranslation* ori = new SoTranslation;
    //ori->translation.setValue(-1.5f, -1.5f, -1.5f);

    //SoCube* cube = new SoCube;
    //cube->width = 3.0f;
    //cube->height = 3.0f;
    //cube->depth = 3.0f;

    //ring->addChild(ori);
    //ring->addChild(cube);

    SoClipPlane* clipPlane = new SoClipPlane;
    clipPlane->plane.setValue(SbPlane(SbVec3f(0, 1, 0), -0.8));

    SoClipPlane* clipPlane2 = new SoClipPlane;
    clipPlane2->plane.setValue(SbPlane(SbVec3f(1, 0, 0), 0));

    SoCallback* stencilInit = new SoCallback();
    stencilInit->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glEnable(GL_STENCIL_TEST);
            glClearStencil(0);
            glClear(GL_STENCIL_BUFFER_BIT);
        }
                             });
    m_root->addChild(stencilInit);

    SoCallback* stencilWrite1 = new SoCallback();
    stencilWrite1->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        }
                               });

    SoCallback* stencilWrite0 = new SoCallback();
    stencilWrite0->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        }
                               });

    SoCallback* stencilReset = new SoCallback();
    stencilReset->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }
                              });

    SoShapeHints* frontFaceCulling = new SoShapeHints;
    frontFaceCulling->vertexOrdering = SoShapeHints::CLOCKWISE;
    frontFaceCulling->shapeType = SoShapeHints::SOLID;

    SoShapeHints* backFaceCulling = new SoShapeHints;
    backFaceCulling->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    backFaceCulling->shapeType = SoShapeHints::SOLID;

    SoColorMask* colorMaskFalse = new SoColorMask;
    colorMaskFalse->red = FALSE;
    colorMaskFalse->green = FALSE;
    colorMaskFalse->blue = FALSE;
    colorMaskFalse->alpha = FALSE;

    SoColorMask* colorMaskTrue = new SoColorMask;
    colorMaskTrue->red = TRUE;
    colorMaskTrue->green = TRUE;
    colorMaskTrue->blue = TRUE;
    colorMaskTrue->alpha = TRUE;

    SoSeparator* clipRoot = new SoSeparator;
    m_root->addChild(clipRoot);

    clipRoot->addChild(clipPlane);
    //clipRoot->addChild(clipPlane2); // 一个圆柱，被两个裁剪面平行切出一个管状体，无法通过反面方式获取剖面

    // first pass
    clipRoot->addChild(colorMaskFalse);
    clipRoot->addChild(stencilWrite1);
    clipRoot->addChild(frontFaceCulling);
    clipRoot->addChild(ring);

    // second pass
    clipRoot->addChild(colorMaskTrue);
    clipRoot->addChild(stencilWrite0);
    clipRoot->addChild(backFaceCulling);
    clipRoot->addChild(ring);
    clipRoot->addChild(stencilReset);

    // third pass
    SoCallback* faceSet = new SoCallback();
    faceSet->setCallback([](void*, SoAction* action) {
        if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        {
            if (0 == texId)
            {
                const QString ICONDIR = "../Data/counterclockwise.svg";
                QImage image = RenderSvgToImage(ICONDIR, QRect(0, 0, WINDOWWIDTH, WINDOWHEIGHT));
                image.save("../Data/counterclockwise.png", "PNG");
                QOpenGLTexture* texture = new QOpenGLTexture(image.mirrored());
                texture->setMinificationFilter(QOpenGLTexture::Nearest);
                texture->setMagnificationFilter(QOpenGLTexture::Linear);
                texId = texture->textureId();
            }

            glPushAttrib(GL_ENABLE_BIT | GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_EQUAL, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            // based color
            glColor3f(0.89f, 0.30f, 0.35f);
            glBegin(GL_QUADS);
            glVertex2f(-1.f, -1.f);
            glVertex2f(1.f, -1.f);
            glVertex2f(1.f, 1.f);
            glVertex2f(-1.f, 1.f);
            glEnd();

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texId);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBegin(GL_QUADS);

            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            glTexCoord2f(0.f, 0.f);
            glVertex2f(-1.f, -1.f);

            glTexCoord2f(1.f, 0.f);
            glVertex2f(1.f, -1.f);

            glTexCoord2f(1.f, 1.f);
            glVertex2f(1.f, 1.f);

            glTexCoord2f(0.f, 1.f);
            glVertex2f(-1.f, 1.f);

            glEnd();

            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);

            glPopAttrib();
        }
                         });
    m_root->addChild(faceSet);

    /*!
    * \brief Callback for section line rendering with "B" approach
    *        1) Create colorFbo (with colorTex + dsRbo)
    *        2) Blit stencil from oldFbo
    *        3) In colorFbo, use stencil test => draw white(1.0) to stencil=1 area
    *        4) Final pass: read s_colorTex => do screen-space render
    */
    SoCallback* sectionLine = new SoCallback();
    sectionLine->setCallback([](void* userData, SoAction* action) {
        if (!action->isOfType(SoGLRenderAction::getClassTypeId()))
            return;

        SoGLRenderAction* glRenderAction = (SoGLRenderAction*)action;
        //---------------------------------------
        // 1) Load extension function pointers
        //---------------------------------------
        static const cc_glglue* globalGlue = nullptr;
        static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersFunc = nullptr;
        static PFNGLBINDFRAMEBUFFERPROC glBindFramebufferFunc = nullptr;
        static PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersFunc = nullptr;
        static PFNGLBLITFRAMEBUFFERPROC glBlitFramebufferFunc = nullptr;
        static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DFunc = nullptr;
        static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatusFunc = nullptr;
        static PFNGLCREATESHADERPROC glCreateShaderFunc = nullptr;
        static PFNGLCREATEPROGRAMPROC glCreateProgramFunc = nullptr;
        static PFNGLSHADERSOURCEPROC glShaderSourceFunc = nullptr;
        static PFNGLCOMPILESHADERPROC glCompileShaderFunc = nullptr;
        static PFNGLATTACHSHADERPROC glAttachShaderFunc = nullptr;
        static PFNGLLINKPROGRAMPROC glLinkProgramFunc = nullptr;
        static PFNGLDELETESHADERPROC glDeleteShaderFunc = nullptr;
        static PFNGLGENVERTEXARRAYSPROC glGenVertexArraysFunc = nullptr;
        static PFNGLBINDVERTEXARRAYPROC glBindVertexArrayFunc = nullptr;
        static PFNGLGENBUFFERSPROC glGenBuffersFunc = nullptr;
        static PFNGLBINDBUFFERPROC glBindBufferFunc = nullptr;
        static PFNGLBUFFERDATAPROC glBufferDataFunc = nullptr;
        static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayFunc = nullptr;
        static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerFunc = nullptr;
        static PFNGLUSEPROGRAMPROC glUseProgramFunc = nullptr;
        static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationFunc = nullptr;
        static PFNGLUNIFORM1IPROC glUniform1iFunc = nullptr;
        static PFNGLUNIFORM2FPROC glUniform2fFunc = nullptr;
        static PFNGLUNIFORM4FPROC glUniform4fFunc = nullptr;
        static PFNGLACTIVETEXTUREPROC glActiveTextureFunc = nullptr;
        static PFNGLGENRENDERBUFFERSPROC glGenRenderbuffersFunc = nullptr;
        static PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffersFunc = nullptr;
        static PFNGLBINDRENDERBUFFERPROC glBindRenderbufferFunc = nullptr;
        static PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorageFunc = nullptr;
        static PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbufferFunc = nullptr;
        static PFNGLDRAWBUFFERSPROC glDrawBuffersFunc = nullptr;

        if (!globalGlue)
        {
            globalGlue = cc_glglue_instance(glRenderAction->getCacheContext());
        }
        if (!glGenFramebuffersFunc)
        {
            glGenFramebuffersFunc = (PFNGLGENFRAMEBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glGenFramebuffers");
        }
        if (!glBindFramebufferFunc)
        {
            glBindFramebufferFunc = (PFNGLBINDFRAMEBUFFERPROC)cc_glglue_getprocaddress(globalGlue, "glBindFramebuffer");
        }
        if (!glDeleteFramebuffersFunc)
        {
            glDeleteFramebuffersFunc = (PFNGLDELETEFRAMEBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glDeleteFramebuffers");
        }
        if (!glBlitFramebufferFunc)
        {
            glBlitFramebufferFunc = (PFNGLBLITFRAMEBUFFERPROC)cc_glglue_getprocaddress(globalGlue, "glBlitFramebuffer");
        }
        if (!glFramebufferTexture2DFunc)
        {
            glFramebufferTexture2DFunc = (PFNGLFRAMEBUFFERTEXTURE2DPROC)cc_glglue_getprocaddress(globalGlue, "glFramebufferTexture2D");
        }
        if (!glCheckFramebufferStatusFunc)
        {
            glCheckFramebufferStatusFunc = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)cc_glglue_getprocaddress(globalGlue, "glCheckFramebufferStatus");
        }
        if (!glCreateShaderFunc)
        {
            glCreateShaderFunc = (PFNGLCREATESHADERPROC)cc_glglue_getprocaddress(globalGlue, "glCreateShader");
        }
        if (!glCreateProgramFunc)
        {
            glCreateProgramFunc = (PFNGLCREATEPROGRAMPROC)cc_glglue_getprocaddress(globalGlue, "glCreateProgram");
        }
        if (!glShaderSourceFunc)
        {
            glShaderSourceFunc = (PFNGLSHADERSOURCEPROC)cc_glglue_getprocaddress(globalGlue, "glShaderSource");
        }
        if (!glCompileShaderFunc)
        {
            glCompileShaderFunc = (PFNGLCOMPILESHADERPROC)cc_glglue_getprocaddress(globalGlue, "glCompileShader");
        }
        if (!glAttachShaderFunc)
        {
            glAttachShaderFunc = (PFNGLATTACHSHADERPROC)cc_glglue_getprocaddress(globalGlue, "glAttachShader");
        }
        if (!glLinkProgramFunc)
        {
            glLinkProgramFunc = (PFNGLLINKPROGRAMPROC)cc_glglue_getprocaddress(globalGlue, "glLinkProgram");
        }
        if (!glDeleteShaderFunc)
        {
            glDeleteShaderFunc = (PFNGLDELETESHADERPROC)cc_glglue_getprocaddress(globalGlue, "glDeleteShader");
        }
        if (!glGenVertexArraysFunc)
        {
            glGenVertexArraysFunc = (PFNGLGENVERTEXARRAYSPROC)cc_glglue_getprocaddress(globalGlue, "glGenVertexArrays");
        }
        if (!glBindVertexArrayFunc)
        {
            glBindVertexArrayFunc = (PFNGLBINDVERTEXARRAYPROC)cc_glglue_getprocaddress(globalGlue, "glBindVertexArray");
        }
        if (!glGenBuffersFunc)
        {
            glGenBuffersFunc = (PFNGLGENBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glGenBuffers");
        }
        if (!glBindBufferFunc)
        {
            glBindBufferFunc = (PFNGLBINDBUFFERPROC)cc_glglue_getprocaddress(globalGlue, "glBindBuffer");
        }
        if (!glBufferDataFunc)
        {
            glBufferDataFunc = (PFNGLBUFFERDATAPROC)cc_glglue_getprocaddress(globalGlue, "glBufferData");
        }
        if (!glEnableVertexAttribArrayFunc)
        {
            glEnableVertexAttribArrayFunc = (PFNGLENABLEVERTEXATTRIBARRAYPROC)cc_glglue_getprocaddress(globalGlue, "glEnableVertexAttribArray");
        }
        if (!glVertexAttribPointerFunc)
        {
            glVertexAttribPointerFunc = (PFNGLVERTEXATTRIBPOINTERPROC)cc_glglue_getprocaddress(globalGlue, "glVertexAttribPointer");
        }
        if (!glUseProgramFunc)
        {
            glUseProgramFunc = (PFNGLUSEPROGRAMPROC)cc_glglue_getprocaddress(globalGlue, "glUseProgram");
        }
        if (!glGetUniformLocationFunc)
        {
            glGetUniformLocationFunc = (PFNGLGETUNIFORMLOCATIONPROC)cc_glglue_getprocaddress(globalGlue, "glGetUniformLocation");
        }
        if (!glUniform1iFunc)
        {
            glUniform1iFunc = (PFNGLUNIFORM1IPROC)cc_glglue_getprocaddress(globalGlue, "glUniform1i");
        }
        if (!glUniform2fFunc)
        {
            glUniform2fFunc = (PFNGLUNIFORM2FPROC)cc_glglue_getprocaddress(globalGlue, "glUniform2f");
        }
        if (!glUniform4fFunc)
        {
            glUniform4fFunc = (PFNGLUNIFORM4FPROC)cc_glglue_getprocaddress(globalGlue, "glUniform4f");
        }
        if (!glActiveTextureFunc)
        {
            glActiveTextureFunc = (PFNGLACTIVETEXTUREPROC)cc_glglue_getprocaddress(globalGlue, "glActiveTexture");
        }
        if (!glGenRenderbuffersFunc)
        {
            glGenRenderbuffersFunc = (PFNGLGENRENDERBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glGenRenderbuffers");
        }
        if (!glDeleteRenderbuffersFunc)
        {
            glDeleteRenderbuffersFunc = (PFNGLDELETERENDERBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glDeleteRenderbuffers");
        }
        if (!glBindRenderbufferFunc)
        {
            glBindRenderbufferFunc = (PFNGLBINDRENDERBUFFERPROC)cc_glglue_getprocaddress(globalGlue, "glBindRenderbuffer");
        }
        if (!glRenderbufferStorageFunc)
        {
            glRenderbufferStorageFunc = (PFNGLRENDERBUFFERSTORAGEPROC)cc_glglue_getprocaddress(globalGlue, "glRenderbufferStorage");
        }
        if (!glFramebufferRenderbufferFunc)
        {
            glFramebufferRenderbufferFunc = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)cc_glglue_getprocaddress(globalGlue, "glFramebufferRenderbuffer");
        }
        if (!glDrawBuffersFunc)
        {
            glDrawBuffersFunc = (PFNGLDRAWBUFFERSPROC)cc_glglue_getprocaddress(globalGlue, "glDrawBuffers");
        }

        // 获取窗口尺寸
        const SbViewportRegion& vp = glRenderAction->getViewportRegion();
        int width = vp.getViewportSizePixels()[0];
        int height = vp.getViewportSizePixels()[1];

        //---------------------------------------
        // 2) 创建/更新 colorFbo (带 colorTex + dsRbo)
        //---------------------------------------
        static GLuint s_colorFbo = 0;
        static GLuint s_colorTex = 0;
        static GLuint s_dsRbo = 0;
        static int s_texWidth = 0, s_texHeight = 0;

        if (s_colorFbo == 0 || width != s_texWidth || height != s_texHeight)
        {
            // 如果存在旧资源, 先清理
            if (s_colorFbo) {
                glDeleteFramebuffersFunc(1, &s_colorFbo);
                glDeleteTextures(1, &s_colorTex);
                if (s_dsRbo) {
                    glDeleteRenderbuffersFunc(1, &s_dsRbo);
                    s_dsRbo = 0;
                }
                s_colorFbo = 0;
                s_colorTex = 0;
            }

            // (A) 创建 colorTex
            glGenTextures(1, &s_colorTex);
            glBindTexture(GL_TEXTURE_2D, s_colorTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // 分配 RGBA8
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            // (B) 创建FBO
            glGenFramebuffersFunc(1, &s_colorFbo);
            glBindFramebufferFunc(GL_FRAMEBUFFER, s_colorFbo);

            // attach colorTex => color attachment0
            glFramebufferTexture2DFunc(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D,
                                       s_colorTex,
                                       0);

            // (C) 创建深度+模板的 renderbuffer
            glGenRenderbuffersFunc(1, &s_dsRbo);
            glBindRenderbufferFunc(GL_RENDERBUFFER, s_dsRbo);
            glRenderbufferStorageFunc(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbufferFunc(GL_FRAMEBUFFER,
                                          GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER,
                                          s_dsRbo);
            glBindRenderbufferFunc(GL_RENDERBUFFER, 0);

            // 检查完整性
            GLenum stat = glCheckFramebufferStatusFunc(GL_FRAMEBUFFER);
            if (stat != GL_FRAMEBUFFER_COMPLETE) {
                SoDebugError::post("sectionLine",
                                   "colorFbo incomplete! status=0x%x", stat);
            }
            glBindFramebufferFunc(GL_FRAMEBUFFER, 0);

            // 记录宽高
            s_texWidth = width;
            s_texHeight = height;
        }

        //---------------------------------------
        // 3) Blit stencil from "旧FBO" => s_colorFbo
        //---------------------------------------
        GLint oldFbo = 0;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldFbo);
        // 这里 oldFbo 可能是0(系统FBO) 或 QOpenGLWidget->defaultFramebufferObject()

        // (A) bind read/draw
        glBindFramebufferFunc(GL_READ_FRAMEBUFFER, oldFbo);
        glBindFramebufferFunc(GL_DRAW_FRAMEBUFFER, s_colorFbo);

        // (B) 复制 stencil
        glBlitFramebufferFunc(
            0, 0, width, height,
            0, 0, width, height,
            GL_STENCIL_BUFFER_BIT,
            GL_NEAREST
        );

        // 解绑定
        glBindFramebufferFunc(GL_FRAMEBUFFER, 0);

        //---------------------------------------
        // 4) 在 s_colorFbo 里启用 stencil=1 => 画全屏白色
        //---------------------------------------
        glBindFramebufferFunc(GL_FRAMEBUFFER, s_colorFbo);
        GLenum stat = glCheckFramebufferStatusFunc(GL_FRAMEBUFFER);
        if (GL_FRAMEBUFFER_COMPLETE != stat)
        {
            SoDebugError::post("sectionLine",
                               "colorFbo bind incorrect! status=0x%x", stat);
        }

        GLint curFbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFbo);
        if (curFbo != s_colorFbo)
        {
            SoDebugError::post("myCallback", "curFbo = %d", curFbo);
        }
        glViewport(0, 0, width, height);

        // 清空 color => (0,0,0,0)
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // 启用 stencil test
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        glDisable(GL_DEPTH_TEST); // 不做深度

        glDisable(GL_LIGHTING);

        // 用固定管线方式画满屏
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // 写成白色
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(-1, -1);
        glVertex2f(1, -1);
        glVertex2f(1, 1);
        glVertex2f(-1, 1);
        glEnd();

        // restore matrix
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        // 还原状态
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glBindFramebufferFunc(GL_FRAMEBUFFER, 0);

        // 此时 s_colorTex 内部就是：stencil=1 => (1,1,1,1)，其余 => (0,0,0,0)

        //---------------------------------------
        // 5) 最后一段 pass: 在屏幕上展示 s_colorTex 
        //    (或做其他后处理，比如Canny/边缘检测等)
        //---------------------------------------
        // 这里我们就演示: "把 s_colorTex 当做灰度贴图显示"
        // 下方写了一个非常简单的 Shader (s_prog)
        static GLuint s_prog = 0;
        if (s_prog == 0) {
            // compile & link
            const char* vsCode = R"(
                #version 330 core
                layout(location=0) in vec2 aPos;
                out vec2 vUV;
                void main(){
                    gl_Position=vec4(aPos,0,1);
                    vUV = (aPos * 0.5) + vec2(0.5,0.5);
                }
            )";
            const char* fsCode = R"(
                #version 330 core

                in vec2 vUV;           // 来自顶点着色器的纹理坐标
                out vec4 FragColor;    // 输出像素颜色

                uniform sampler2D uMaskTex;    // 0/1 mask 纹理
                uniform vec2 uViewportSize;    // (width, height)
                uniform vec4 uLineColor;       // 剖面线颜色，比如(1,0,0,1)

                void main()
                {
                    // center
                    float center = texture(uMaskTex, vUV).r;

                    float px = 1.0 / uViewportSize.x;
                    float py = 1.0 / uViewportSize.y;

                    // neighbor
                    float leftVal   = texture(uMaskTex, vUV + vec2(-px, 0.0)).r;
                    float rightVal  = texture(uMaskTex, vUV + vec2( px, 0.0)).r;
                    float upVal     = texture(uMaskTex, vUV + vec2(0.0,  py)).r;
                    float downVal   = texture(uMaskTex, vUV + vec2(0.0, -py)).r;

                    // 简单判断：只要某个邻域和 center 不同 => 边缘
                    float diff = 0.0;
                    diff += abs(center - leftVal);
                    diff += abs(center - rightVal);
                    diff += abs(center - upVal);
                    diff += abs(center - downVal);

                    if(diff > 0.001)
                    {
                        // 边缘像素 => 画线颜色
                        FragColor = uLineColor;
                    }
                    else
                    {
                        // 非边缘 => 透明
                        FragColor = vec4(0,0,0,0);
                    }
                }
            )";
            // shader compile ...
            GLuint vs = glCreateShaderFunc(GL_VERTEX_SHADER);
            glShaderSourceFunc(vs, 1, &vsCode, nullptr);
            glCompileShaderFunc(vs);
            // check compile ...
            GLuint fs = glCreateShaderFunc(GL_FRAGMENT_SHADER);
            glShaderSourceFunc(fs, 1, &fsCode, nullptr);
            glCompileShaderFunc(fs);
            // link
            s_prog = glCreateProgramFunc();
            glAttachShaderFunc(s_prog, vs);
            glAttachShaderFunc(s_prog, fs);
            glLinkProgramFunc(s_prog);
            // check link ...
            glDeleteShaderFunc(vs);
            glDeleteShaderFunc(fs);
        }

        // 全屏三角形 VAO
        static GLuint s_vao = 0, s_vbo = 0;
        if (!s_vao) {
            glGenVertexArraysFunc(1, &s_vao);
            glBindVertexArrayFunc(s_vao);

            static float fsQuad[6 * 2] = {
                -1,-1,  1,-1,  1,1,
                -1,-1,  1,1,   -1,1
            };
            glGenBuffersFunc(1, &s_vbo);
            glBindBufferFunc(GL_ARRAY_BUFFER, s_vbo);
            glBufferDataFunc(GL_ARRAY_BUFFER, sizeof(fsQuad), fsQuad, GL_STATIC_DRAW);

            glEnableVertexAttribArrayFunc(0);
            glVertexAttribPointerFunc(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

            glBindBufferFunc(GL_ARRAY_BUFFER, 0);
            glBindVertexArrayFunc(0);
        }

        // 绑定默认FBO 或 Coin3D FBO 做最终显示
        glBindFramebufferFunc(GL_FRAMEBUFFER, oldFbo);
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 启动 s_prog
        glUseProgramFunc(s_prog);

        // 传送 uniform
        GLint locTex = glGetUniformLocationFunc(s_prog, "uStencilTex");
        glUniform1iFunc(locTex, 0); // 绑定到 texture unit 0
        // 视口大小
        GLint locSize = glGetUniformLocationFunc(s_prog, "uViewportSize");
        glUniform2fFunc(locSize, (float)width, (float)height);
        // 颜色
        GLint locCol = glGetUniformLocationFunc(s_prog, "uLineColor");
        // 例如红色线
        glUniform4fFunc(locCol, 0.0f, 1.0f, 0.0f, 1.0f);

        // 绑定 s_colorTex
        glActiveTextureFunc(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_colorTex);

        // 画全屏
        glBindVertexArrayFunc(s_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArrayFunc(0);

        // 还原
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgramFunc(0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

                             }, m_viewer);
    m_root->addChild(sectionLine);

    //m_viewer->getSoRenderManager()->getGLRenderAction()->setNumPasses(4);
    //m_viewer->getSoRenderManager()->getGLRenderAction()->setSmoothing(TRUE);
};

void InventorEx::getWorldToScreenScale()
{
    SoOrthographicCamera* camera = new SoOrthographicCamera;
    m_root->addChild(camera);

    SoZoomAdaptor* autoZoom = new SoZoomAdaptor;
    m_root->addChild(autoZoom);

    SoSphere* sphere = new SoSphere;
    sphere->radius = WINDOWHEIGHT / 2.f;
    m_root->addChild(sphere);
}

// Constants for dimensions
const float AXIS_LENGTH = 80.0f;
const float ARROW_HEIGHT = 16.0f;
const float TEXT_OFFSET = 6.0f;
const float ORIGIN_SPHERE_RADIUS = 8.0f;
const float AXIS_LOWER_RADIUS = 1.5f;
const float AXIS_UPPER_RADIUS = 2.5f;
const float ARROW_RADIUS = 6.0f;
const int AXIS_SEGMENTS = 20;
const float RING_RADIUS = 64.0f;
const float TUBE_RADIUS = 1.5f;
const int RING_SEGMENTS = 20;
const int TUBE_SEGMENTS = 20;
const float START_ANGLE = 0.0f;
const float END_ANGLE = M_PI_2;

// Function to create a reusable axis node (includes line and arrow)
SoSeparator* createAxisNode() {
    SoSeparator* axisNode = new SoSeparator;

    // Create coordinates for the truncated cone (axis line)
    SoCoordinate3* axisCoords = new SoCoordinate3;
    std::vector<SbVec3f> coords;

    for (int i = 0; i < AXIS_SEGMENTS; ++i) {
        float angle = 2.0f * M_PI * i / AXIS_SEGMENTS;
        float xLower = AXIS_LOWER_RADIUS * cos(angle);
        float yLower = AXIS_LOWER_RADIUS * sin(angle);
        coords.push_back(SbVec3f(xLower, yLower, 0));

        float xUpper = AXIS_UPPER_RADIUS * cos(angle);
        float yUpper = AXIS_UPPER_RADIUS * sin(angle);
        coords.push_back(SbVec3f(xUpper, yUpper, AXIS_LENGTH));
    }

    axisCoords->point.setValues(0, coords.size(), coords.data());
    axisNode->addChild(axisCoords);

    // Define faces for the truncated cone
    SoIndexedFaceSet* axisFaceSet = new SoIndexedFaceSet;
    std::vector<int32_t> indices;
    for (int i = 0; i < AXIS_SEGMENTS; ++i) {
        int next = (i + 1) % AXIS_SEGMENTS;

        // Corrected vertex order (counter-clockwise when viewed from outside)
        indices.push_back(i * 2);           // Current lower circle point
        indices.push_back(next * 2);        // Next lower circle point
        indices.push_back(next * 2 + 1);    // Next upper circle point
        indices.push_back(i * 2 + 1);       // Current upper circle point
        indices.push_back(-1);              // End of face
    }
    axisFaceSet->coordIndex.setValues(0, indices.size(), indices.data());
    axisNode->addChild(axisFaceSet);

    // Create arrowhead using SoCone
    SoCone* arrowCone = new SoCone;
    arrowCone->bottomRadius = ARROW_RADIUS;
    arrowCone->height = ARROW_HEIGHT;

    // Position and rotate the arrowhead
    SoTransform* arrowTransform = new SoTransform;
    arrowTransform->translation.setValue(0, 0, AXIS_LENGTH + ARROW_HEIGHT / 2);
    arrowTransform->rotation.setValue(SbVec3f(1, 0, 0), M_PI_2);

    SoSeparator* arrowSep = new SoSeparator;
    arrowSep->addChild(arrowTransform);
    arrowSep->addChild(arrowCone);

    axisNode->addChild(arrowSep);

    return axisNode;
}

// Function to create a reusable arc node
SoSeparator* createArcNode() {
    SoSeparator* arcNode = new SoSeparator;

    // Create coordinates for the arc
    SoCoordinate3* arcCoords = new SoCoordinate3;
    std::vector<SbVec3f> coords;

    // Generate points for the torus segment (arc with circular cross-section)
    for (int i = 0; i <= RING_SEGMENTS; ++i) {
        float ringAngle = START_ANGLE + (END_ANGLE - START_ANGLE) * i / RING_SEGMENTS;

        for (int j = 0; j <= TUBE_SEGMENTS; ++j) {
            float tubeAngle = 2.0f * M_PI * j / TUBE_SEGMENTS;
            float x = (RING_RADIUS + TUBE_RADIUS * cos(tubeAngle)) * cos(ringAngle);
            float y = (RING_RADIUS + TUBE_RADIUS * cos(tubeAngle)) * sin(ringAngle);
            float z = TUBE_RADIUS * sin(tubeAngle);
            coords.push_back(SbVec3f(x, y, z));
        }
    }

    arcCoords->point.setValues(0, coords.size(), coords.data());
    arcNode->addChild(arcCoords);

    // Define faces for the arc
    SoIndexedFaceSet* arcFaceSet = new SoIndexedFaceSet;
    std::vector<int32_t> indices;

    int ptsPerRing = TUBE_SEGMENTS + 1;

    for (int i = 0; i < RING_SEGMENTS; ++i) {
        for (int j = 0; j < TUBE_SEGMENTS; ++j) {
            int idx0 = i * ptsPerRing + j;
            int idx1 = idx0 + ptsPerRing;
            int idx2 = idx1 + 1;
            int idx3 = idx0 + 1;

            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);
            indices.push_back(idx3);
            indices.push_back(-1);
        }
    }

    arcFaceSet->coordIndex.setValues(0, indices.size(), indices.data());
    arcNode->addChild(arcFaceSet);

    return arcNode;
}

// Main function to construct the dynamic coordinate system
void InventorEx::dynamicCSYS()
{
    // Create root node
    SoSeparator* root = new SoSeparator;

    root->addChild(new SoZoomAdaptor);

    // Create origin sphere
    SoSphere* originSphere = new SoSphere;
    originSphere->radius = ORIGIN_SPHERE_RADIUS;
    root->addChild(originSphere);

    // Create reusable axis node (line and arrow)
    SoSeparator* axisNode = createAxisNode();

    // Font for axis labels
    SoFont* font = new SoFont;
    font->name.setValue("Arial");

    HDC hdc = GetDC(NULL);
    int vertRes = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(NULL, hdc);
    font->size.setValue(16 * vertRes / 1080.0f);

    // Total offset for label position
    const float LABEL_POSITION = AXIS_LENGTH + ARROW_HEIGHT + TEXT_OFFSET;

    // Create X-axis
    SoSeparator* xAxis = new SoSeparator;
    SoRotation* xRotation = new SoRotation;
    xRotation->rotation.setValue(SbVec3f(0, 1, 0), M_PI_2);
    xAxis->addChild(xRotation);
    xAxis->addChild(axisNode);

    // Add label for X-axis
    SoTransform* xTextTransform = new SoTransform;
    xTextTransform->translation.setValue(0, 0, LABEL_POSITION);

    SoText2* xAxisText = new SoText2;
    xAxisText->justification = SoText2::CENTER;
    xAxisText->string.setValue("X");

    SoSeparator* xTextSep = new SoSeparator;
    xTextSep->addChild(xTextTransform);
    xTextSep->addChild(font);
    xTextSep->addChild(xAxisText);

    xAxis->addChild(xTextSep);
    root->addChild(xAxis);

    // Create Y-axis
    SoSeparator* yAxis = new SoSeparator;
    SoRotation* yRotation = new SoRotation;
    yRotation->rotation.setValue(SbVec3f(1, 0, 0), -M_PI_2);
    yAxis->addChild(yRotation);
    yAxis->addChild(axisNode);

    // Add label for Y-axis
    SoTransform* yTextTransform = new SoTransform;
    yTextTransform->translation.setValue(0, 0, LABEL_POSITION);

    SoText2* yAxisText = new SoText2;
    yAxisText->justification = SoText2::CENTER;
    yAxisText->string.setValue("Y");

    SoSeparator* yTextSep = new SoSeparator;
    yTextSep->addChild(yTextTransform);
    yTextSep->addChild(font);
    yTextSep->addChild(yAxisText);

    yAxis->addChild(yTextSep);
    root->addChild(yAxis);

    // Create Z-axis
    SoSeparator* zAxis = new SoSeparator;
    // No rotation needed for Z-axis
    zAxis->addChild(axisNode);

    // Add label for Z-axis
    SoTransform* zTextTransform = new SoTransform;
    zTextTransform->translation.setValue(0, 0, LABEL_POSITION);

    SoText2* zAxisText = new SoText2;
    zAxisText->justification = SoText2::CENTER;
    zAxisText->string.setValue("Z");

    SoSeparator* zTextSep = new SoSeparator;
    zTextSep->addChild(zTextTransform);
    zTextSep->addChild(font);
    zTextSep->addChild(zAxisText);

    zAxis->addChild(zTextSep);
    root->addChild(zAxis);

    // Create reusable arc node
    SoSeparator* arcNode = createArcNode();

    // Create arc in XY plane
    SoSeparator* xyArc = new SoSeparator;
    // No rotation needed for XY plane
    xyArc->addChild(arcNode);
    root->addChild(xyArc);

    // Create arc in YZ plane
    SoSeparator* yzArc = new SoSeparator;
    SoRotation* yzRotation = new SoRotation;
    yzRotation->rotation.setValue(SbVec3f(1, 0, 0), M_PI_2);
    yzArc->addChild(yzRotation);
    yzArc->addChild(arcNode);
    root->addChild(yzArc);

    // Create arc in XZ plane
    SoSeparator* xzArc = new SoSeparator;
    SoRotation* xzRotation = new SoRotation;
    xzRotation->rotation.setValue(SbVec3f(0, 1, 0), -M_PI_2);
    xzArc->addChild(xzRotation);
    xzArc->addChild(arcNode);
    root->addChild(xzArc);

    // Add the constructed root node to the scene
    m_root->addChild(root);
}

std::atomic<int> gLatestRequestId;

PickResultManager::PickResultManager(QObject* parent)
    : QObject(parent)
{
    // ...
}

void PickResultManager::onPickDone(const PickResult& result)
{
    // 这里可以做任何后续操作，比如：
    // 1) 调用某个 InventorEx 的普通方法： 
    //    inventorEx->highlightPickedObject(result);
    // 2) 记录日志，弹UI提示等
    int i = 1345;
    i++;
}

MyPickTask::MyPickTask(const PickRequest& req)
    : QObject(nullptr)
    , request(req)
{
    // 让 Qt 不自动删除 QRunnable 对象(!!)：
    // 因为要用 signals/slots，一般最好禁用自动删除
    // （否则 run() 结束后对象立即被释放，信号可能还没来得及发出/处理）
    setAutoDelete(false);
}

void MyPickTask::run()
{
    PickResult result;
    result.requestId = request.requestId;
    result.hit = false;
    result.pathInfo.clear();

    if (!request.root) {
        emit pickDone(result);
        return;
    }

    // 1) 读锁
    SoDB::readlock();

    // 2) 构造pick
    SoRayPickAction pickAction(request.viewport);
    pickAction.setPoint(request.cursorPosition);
    pickAction.setRadius(30.0f);

    // [测试耗时] 假设sleep 10秒，看看是否卡主线程
    //Sleep(10000);
    //QThread::sleep(10);

    // 3) apply
    pickAction.apply(request.root);

    // 4) 获取结果
    const SoPickedPoint* pp = pickAction.getPickedPoint();
    if (pp) {
        result.hit = true;

        // 演示：用 SoWriteAction 把路径写到内存或文件
        // 这里写到一个文件做示范
        //SoWriteAction wra;
        //wra.getOutput()->openFile("C:/temp/pickPoint.iv");
        //wra.getOutput()->setBinary(FALSE);
        //wra.apply(pp->getPath());
        //wra.getOutput()->closeFile();

        // 也可以把结果存到 result.pathInfo
        result.pathInfo = QString("Picked path has been written to C:/temp/pickPoint.iv");
    }
    else {
        result.hit = false;
        result.pathInfo = QString("No object picked.");
    }

    // 5) 解锁
    SoDB::readunlock();

    // 6) 发信号
    int currentLatest = gLatestRequestId.load();
    //if (request.requestId < currentLatest) {
    //    // 说明已经出现了更新的请求
    //    // 就不再 emit pickDone
    //    this->deleteLater();
    //    return;
    //}
    emit pickDone(result);

    // 因为 setAutoDelete(false)，此时 run() 结束后对象不会自动销毁
    // 可以在发完信号后自杀：
    this->deleteLater();
}

static PickResultManager gPickManager;

void mouseReleaseCB(void* userData, SoEventCallback* eventCB)
{
    // 这里 userData 可以是 root 也可以是别的
    SoSeparator* root = static_cast<SoSeparator*>(userData);
    if (!root) return;

    const SoEvent* event = eventCB->getEvent();
    const SoLocation2Event* location2Event = dynamic_cast<const SoLocation2Event*>(event);
    if (/*SO_MOUSE_RELEASE_EVENT(event, ANY)*/location2Event)
    {
        const SbViewportRegion& region = eventCB->getAction()->getViewportRegion();
        SbVec2s pos = event->getPosition(region);

        static int globalPickID = 0;
        PickRequest req;
        req.root = root;
        req.viewport = region;
        req.cursorPosition = pos;
        req.requestId = ++globalPickID;
        gLatestRequestId.store(req.requestId);

        // 生成任务
        MyPickTask* task = new MyPickTask(req);
        // 连接信号到 gPickManager
        QObject::connect(task, &MyPickTask::pickDone,
                         &gPickManager, &PickResultManager::onPickDone,
                         Qt::QueuedConnection/*DirectConnection*/);

        // 提交线程池
        QThreadPool::globalInstance()->start(task);

        // 如果想阻止Coin对事件的进一步处理，可以 eventCB->setHandled();
    }
}

void InventorEx::loadPickAndWrite2()
{
    // 1) 创建 SoEventCallback
    SoEventCallback* eventCB = new SoEventCallback;
    m_root->addChild(eventCB);

    // 2) 给它添加回调
    //    第三个参数 userData 塞 m_root 或其它你想传递的数据
    //eventCB->addEventCallback(
    //    SoMouseButtonEvent::getClassTypeId(),
    //    mouseReleaseCB,
    //    m_root  // userData
    //);
    eventCB->addEventCallback(
        SoLocation2Event::getClassTypeId(),
        mouseReleaseCB,
        m_root  // userData
    );
}

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbVec3f.h>
#include <cmath>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbVec3f.h>

// 构造网格平面节点树
// 输入参数：
//    length, width：网格整体尺寸（XY 平面，从原点开始）
//    borderWidth：内部边框宽度
SoSeparator* CreateGridPlane(float length, float width, float borderWidth)
{
    SoSeparator* root = new SoSeparator();

    // ---------------------------
    // 1. 内部网格线（10行10列，绘制内部9条竖直和9条水平线）
    // ---------------------------
    SoSeparator* gridLinesSep = new SoSeparator();

    // 固定颜色 #D3D6DB
    SbColor gridLineColor(0xD3 / 255.0f, 0xD6 / 255.0f, 0xDB / 255.0f);
    SoMaterial* gridLineMat = new SoMaterial();
    gridLineMat->diffuseColor.setValue(gridLineColor);
    gridLinesSep->addChild(gridLineMat);

    SoDrawStyle* gridLineDrawStyle = new SoDrawStyle();
    gridLineDrawStyle->lineWidth = 1;
    gridLinesSep->addChild(gridLineDrawStyle);

    // 计算每个格子的尺寸
    float cellWidth = length / 10.0f;
    float cellHeight = width / 10.0f;

    // 绘制内部竖直网格线（i = 1 ~ 9）
    for (int i = 1; i < 10; i++)
    {
        float x = i * cellWidth;
        SoSeparator* vLineSep = new SoSeparator();
        SoCoordinate3* vCoord = new SoCoordinate3();
        vCoord->point.set1Value(0, SbVec3f(x, 0.0f, 0.0f));
        vCoord->point.set1Value(1, SbVec3f(x, width, 0.0f));
        vLineSep->addChild(vCoord);
        SoLineSet* vLine = new SoLineSet();
        vLineSep->addChild(vLine);
        gridLinesSep->addChild(vLineSep);
    }

    // 绘制内部水平网格线（j = 1 ~ 9）
    for (int j = 1; j < 10; j++)
    {
        float y = j * cellHeight;
        SoSeparator* hLineSep = new SoSeparator();
        SoCoordinate3* hCoord = new SoCoordinate3();
        hCoord->point.set1Value(0, SbVec3f(0.0f, y, 0.0f));
        hCoord->point.set1Value(1, SbVec3f(length, y, 0.0f));
        hLineSep->addChild(hCoord);
        SoLineSet* hLine = new SoLineSet();
        hLineSep->addChild(hLine);
        gridLinesSep->addChild(hLineSep);
    }

    root->addChild(gridLinesSep);

    // ---------------------------
    // 2. 内部边框（用面绘制，不与网格线重合）
    //    颜色固定 #237FE8，不透明度20%（即透明度 0.8），开启双面光照
    // ---------------------------
    SoSeparator* borderSep = new SoSeparator();

    SbColor borderColor(0x23 / 255.0f, 0x7F / 255.0f, 0xE8 / 255.0f);
    SoMaterial* borderMat = new SoMaterial();
    borderMat->diffuseColor.setValue(borderColor);
    // 设置透明度：20%不透明，即 80% 透明
    borderMat->transparency.setValue(0.8f);
    borderSep->addChild(borderMat);

    // 开启双面光照，使用 SoShapeHints（顶点顺序设为 COUNTERCLOCKWISE）
    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    borderSep->addChild(shapeHints);

    // 使用 PolygonOffset 确保边框面能遮挡网格线
    SoPolygonOffset* po = new SoPolygonOffset();
    po->factor = -1.0f;
    po->units = -1.0f;
    borderSep->addChild(po);

    SoSeparator* borderGeomSep = new SoSeparator();

    // 下边框面：从 (0,0) 到 (length,0)，高度 borderWidth
    {
        SoCoordinate3* bottomCoords = new SoCoordinate3();
        SbVec3f pts[4] = {
            SbVec3f(0.0f, 0.0f, 0.0f),
            SbVec3f(length, 0.0f, 0.0f),
            SbVec3f(length, borderWidth, 0.0f),
            SbVec3f(0.0f, borderWidth, 0.0f)
        };
        bottomCoords->point.setValues(0, 4, pts);
        borderGeomSep->addChild(bottomCoords);
        SoFaceSet* bottomFace = new SoFaceSet();
        bottomFace->numVertices.set1Value(0, 4);
        borderGeomSep->addChild(bottomFace);
    }

    // 上边框面：从 (0, width - borderWidth) 到 (length, width)
    {
        SoCoordinate3* topCoords = new SoCoordinate3();
        SbVec3f pts[4] = {
            SbVec3f(0.0f, width - borderWidth, 0.0f),
            SbVec3f(length, width - borderWidth, 0.0f),
            SbVec3f(length, width, 0.0f),
            SbVec3f(0.0f, width, 0.0f)
        };
        topCoords->point.setValues(0, 4, pts);
        borderGeomSep->addChild(topCoords);
        SoFaceSet* topFace = new SoFaceSet();
        topFace->numVertices.set1Value(0, 4);
        borderGeomSep->addChild(topFace);
    }

    // 左边框面：从 (0, borderWidth) 到 (borderWidth, width - borderWidth)
    {
        SoCoordinate3* leftCoords = new SoCoordinate3();
        SbVec3f pts[4] = {
            SbVec3f(0.0f, borderWidth, 0.0f),
            SbVec3f(borderWidth, borderWidth, 0.0f),
            SbVec3f(borderWidth, width - borderWidth, 0.0f),
            SbVec3f(0.0f, width - borderWidth, 0.0f)
        };
        leftCoords->point.setValues(0, 4, pts);
        borderGeomSep->addChild(leftCoords);
        SoFaceSet* leftFace = new SoFaceSet();
        leftFace->numVertices.set1Value(0, 4);
        borderGeomSep->addChild(leftFace);
    }

    // 右边框面：从 (length - borderWidth, borderWidth) 到 (length, width - borderWidth)
    {
        SoCoordinate3* rightCoords = new SoCoordinate3();
        SbVec3f pts[4] = {
            SbVec3f(length - borderWidth, borderWidth, 0.0f),
            SbVec3f(length, borderWidth, 0.0f),
            SbVec3f(length, width - borderWidth, 0.0f),
            SbVec3f(length - borderWidth, width - borderWidth, 0.0f)
        };
        rightCoords->point.setValues(0, 4, pts);
        borderGeomSep->addChild(rightCoords);
        SoFaceSet* rightFace = new SoFaceSet();
        rightFace->numVertices.set1Value(0, 4);
        borderGeomSep->addChild(rightFace);
    }

    borderSep->addChild(borderGeomSep);
    root->addChild(borderSep);

    // ---------------------------
    // 3. 外部边框（整个网格外框，线宽 1px，颜色 #237FE8，不透明度50%）
    // ---------------------------
    SoSeparator* outerBorderSep = new SoSeparator();

    SoMaterial* outerBorderMat = new SoMaterial();
    // 同样颜色 #237FE8
    outerBorderMat->diffuseColor.setValue(borderColor);
    // 50% 不透明，即透明度 0.5
    outerBorderMat->transparency.setValue(0.5f);
    outerBorderSep->addChild(outerBorderMat);

    SoDrawStyle* outerBorderDrawStyle = new SoDrawStyle();
    outerBorderDrawStyle->lineWidth = 1;
    outerBorderSep->addChild(outerBorderDrawStyle);

    SoCoordinate3* outerBorderCoords = new SoCoordinate3();
    // 外框顶点：从 (0,0) -> (length,0) -> (length,width) -> (0,width) 并闭合
    SbVec3f outerPts[5] = {
        SbVec3f(0.0f, 0.0f, 0.0f),
        SbVec3f(length, 0.0f, 0.0f),
        SbVec3f(length, width, 0.0f),
        SbVec3f(0.0f, width, 0.0f),
        SbVec3f(0.0f, 0.0f, 0.0f)
    };
    outerBorderCoords->point.setValues(0, 5, outerPts);
    outerBorderSep->addChild(outerBorderCoords);
    SoLineSet* outerBorderLineSet = new SoLineSet();
    outerBorderSep->addChild(outerBorderLineSet);

    root->addChild(outerBorderSep);

    return root;
}

SoSeparator* CreateMeshAnnotations(float length, float width, float borderWidth)
{
    const float ArrowHeight = 24.0f;
    const float ArrowBottom = 6.0f;
    const float BoundaryLineLenth = 40.0f;
    const float BoxWidth = 68.0f;
    const float BoxHeight = 28.0f;
    const float TextSize = 12.0f;
    const float MinLineLen = 6.0f;
    const float ArrowMinus = 4.0f;
    const float ArrowPos = BoundaryLineLenth - ArrowMinus;

    SoSeparator* annotationRoot = new SoSeparator();

    SoMaterial* mat = new SoMaterial;
    mat->diffuseColor.setValue(SbColor(0x2C / 255.0f, 0x30 / 255.0f, 0x39 / 255.0f));
    annotationRoot->addChild(mat);

    // ---------------------------
    // Prepare Ref Element
    // ---------------------------
    // 1 Arrow
    SoSeparator* arrow = new SoSeparator();

    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    arrow->addChild(shapeHints);

    SoLightModel* baseColor = new SoLightModel;
    baseColor->model = SoLightModel::BASE_COLOR;
    arrow->addChild(baseColor);

    // 指向X-的箭头
    SoCoordinate3* arrowCoord = new SoCoordinate3;
    arrowCoord->point.set1Value(0, SbVec3f(0.0f, -ArrowBottom / 2.0f, 0.0f));
    arrowCoord->point.set1Value(1, SbVec3f(0.0f, ArrowBottom / 2.0f, 0.0f));
    arrowCoord->point.set1Value(2, SbVec3f(-ArrowHeight, 0.0f, 0.0f));
    arrow->addChild(arrowCoord);

    SoFaceSet* arrowFace = new SoFaceSet;
    arrowFace->numVertices.set1Value(0, 3);
    arrow->addChild(arrowFace);

    // 2 BoundaryLine
    SoSeparator* boundaryLine = new SoSeparator();

    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->lineWidth = 1.0f;

    boundaryLine->addChild(drawStyle);

    // 与Y轴水平的尺寸界线
    SoCoordinate3* boundaryLineCoord = new SoCoordinate3;
    boundaryLineCoord->point.set1Value(0, SbVec3f(0.0f, 0.0f, 0.0f));
    boundaryLineCoord->point.set1Value(1, SbVec3f(0.0f, -BoundaryLineLenth, 0.0f));
    boundaryLine->addChild(boundaryLineCoord);
    boundaryLine->addChild(new SoLineSet);

    // ---------------------------
    //  3.InputBox
    // ---------------------------
    SoSeparator* inputBox = new SoSeparator();

    inputBox->addChild(shapeHints);
    inputBox->addChild(baseColor);

    // 绘制背景面（白色）
    SoMaterial* bgMat = new SoMaterial;
    bgMat->diffuseColor.setValue(1.0f, 1.0f, 1.0f);
    inputBox->addChild(bgMat);

    inputBox->addChild(new SoPolygonOffset);

    SoCoordinate3* boxCoord = new SoCoordinate3;
    // 中心在原点，以Y轴为水平方向
    float halfW = BoxWidth / 2.0f;
    float halfH = BoxHeight / 2.0f;
    boxCoord->point.setValues(0, 4, new SbVec3f[4]{
        SbVec3f(-halfH, -halfW, 0.0f),
        SbVec3f(halfH, -halfW, 0.0f),
        SbVec3f(halfH, halfW, 0.0f),
        SbVec3f(-halfH, halfW, 0.0f)
                              });
    inputBox->addChild(boxCoord);

    SoFaceSet* boxFace = new SoFaceSet;
    boxFace->numVertices.set1Value(0, 4);
    inputBox->addChild(boxFace);

    SbColor borderCol(0xD3 / 255.0f, 0xD6 / 255.0f, 0xDB / 255.0f);
    SoMaterial* borderMat = new SoMaterial;
    borderMat->diffuseColor.setValue(borderCol);
    inputBox->addChild(borderMat);

    inputBox->addChild(drawStyle);

    SoIndexedLineSet* borderLS = new SoIndexedLineSet;
    borderLS->coordIndex.setValues(0, 6, new int[6] {
        0, 1, 2, 3, 0, -1
                                   });
    inputBox->addChild(borderLS);


    // Bottom Annotation
    SoSeparator* bottomAnnotation = new SoSeparator;

    // 1 尺寸界线
    bottomAnnotation->addChild(boundaryLine);

    // 2 箭头
    SoSeparator* arrowRoot2 = new SoSeparator;

    SoTranslation* translation2 = new SoTranslation;
    translation2->translation.setValue(ArrowHeight, -ArrowPos, 0);
    arrowRoot2->addChild(translation2);
    arrowRoot2->addChild(arrow);

    bottomAnnotation->addChild(arrowRoot2);

    // 3 尺寸线
    SoSeparator* dimensionLine = new SoSeparator;

    SoCoordinate3* dimensionLineCoord = new SoCoordinate3;
    float bootomAnnotationLineLength = (length - ArrowHeight * 2.0f - BoxHeight) / 2.0f;
    dimensionLineCoord->point.set1Value(0, SbVec3f(ArrowHeight, -ArrowPos, 0.0f));
    dimensionLineCoord->point.set1Value(1, SbVec3f(ArrowHeight + bootomAnnotationLineLength, -ArrowPos, 0.0f));
    dimensionLine->addChild(dimensionLineCoord);
    dimensionLine->addChild(new SoLineSet);

    bottomAnnotation->addChild(dimensionLine);

    // 4 输入框
    SoSeparator* boxRoot4 = new SoSeparator;

    SoTranslation* translation4 = new SoTranslation;
    translation4->translation.setValue(length / 2.0f, -ArrowPos, 0);
    boxRoot4->addChild(translation4);
    boxRoot4->addChild(inputBox);

    // 文本
    SoMaterial* textMat = new SoMaterial;
    textMat->diffuseColor.setValue(SbColor(0xB9 / 255.0f, 0xBD / 255.0f, 0xC4 / 255.0f));
    SoFont* font = new SoFont;
    font->size = TextSize;
    SoTransform* textXform = new SoTransform;
    textXform->translation.setValue(TextSize / 3.0f, 0.0f, 0.0f);
    textXform->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI_2));
    SoText3* textBottom = new SoText3;
    textBottom->justification = SoText3::CENTER;
    textBottom->string.setValue((std::to_string((int)length).c_str()));

    boxRoot4->addChild(textXform);
    boxRoot4->addChild(textMat);
    boxRoot4->addChild(font);
    boxRoot4->addChild(textBottom);

    bottomAnnotation->addChild(boxRoot4);

    // 5 尺寸线
    SoSeparator* dimensionLineRoot5 = new SoSeparator;

    SoTranslation* translation5 = new SoTranslation;
    translation5->translation.setValue(bootomAnnotationLineLength + BoxHeight, 0, 0);
    dimensionLineRoot5->addChild(translation5);
    dimensionLineRoot5->addChild(dimensionLine);

    bottomAnnotation->addChild(dimensionLineRoot5);

    // 6 箭头
    SoSeparator* arrowRoot6 = new SoSeparator;

    SoRotation* rotation6 = new SoRotation;
    rotation6->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI));
    SoTranslation* translation6 = new SoTranslation;
    translation6->translation.setValue(length - ArrowHeight, -ArrowPos, 0);
    arrowRoot6->addChild(translation6);
    arrowRoot6->addChild(rotation6);
    arrowRoot6->addChild(arrow);

    bottomAnnotation->addChild(arrowRoot6);

    // 7 尺寸界线
    SoSeparator* boundaryLineRoot7 = new SoSeparator;

    SoTranslation* translation7 = new SoTranslation;
    translation7->translation.setValue(length, 0, 0);
    boundaryLineRoot7->addChild(translation7);
    boundaryLineRoot7->addChild(boundaryLine);

    bottomAnnotation->addChild(boundaryLineRoot7);


    // Right Annotation
    SoSeparator* rightAnnotation = new SoSeparator;

    // 8 尺寸界线
    SoSeparator* boundaryLineRoot8 = new SoSeparator;

    SoRotation* rotation8 = new SoRotation;
    rotation8->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI_2));
    SoTranslation* translation8 = new SoTranslation;
    translation8->translation.setValue(length, 0, 0);
    boundaryLineRoot8->addChild(translation8);
    boundaryLineRoot8->addChild(rotation8);
    boundaryLineRoot8->addChild(boundaryLine);

    rightAnnotation->addChild(boundaryLineRoot8);

    // 9 箭头
    SoSeparator* arrowRoot9 = new SoSeparator;

    SoRotation* rotation9 = new SoRotation;
    rotation9->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI_2));
    SoTranslation* translation9 = new SoTranslation;
    translation9->translation.setValue(length + ArrowPos, ArrowHeight, 0);
    arrowRoot9->addChild(translation9);
    arrowRoot9->addChild(rotation9);
    arrowRoot9->addChild(arrow);

    rightAnnotation->addChild(arrowRoot9);

    // 10 尺寸线
    SoSeparator* dimensionLineRoot10 = new SoSeparator;

    SoCoordinate3* dimensionLineCoord10 = new SoCoordinate3;
    float rightAnnotationLineLength = (width - ArrowHeight * 2.0f - BoxWidth) / 2.0f;
    dimensionLineCoord10->point.set1Value(0, SbVec3f(length + ArrowPos, ArrowHeight, 0.0f));
    dimensionLineCoord10->point.set1Value(1, SbVec3f(length + ArrowPos, ArrowHeight + rightAnnotationLineLength, 0.0f));
    dimensionLineRoot10->addChild(dimensionLineCoord10);
    dimensionLineRoot10->addChild(new SoLineSet);

    rightAnnotation->addChild(dimensionLineRoot10);

    // 11 输入框
    SoSeparator* boxRoot11 = new SoSeparator;

    SoTranslation* translation11 = new SoTranslation;
    translation11->translation.setValue(length + ArrowPos, width / 2.0f, 0);
    boxRoot11->addChild(translation11);
    boxRoot11->addChild(inputBox);

    // 文本
    SoText3* textRight = new SoText3;
    textRight->justification = SoText3::CENTER;
    textRight->string.setValue((std::to_string((int)width).c_str()));

    boxRoot11->addChild(textXform);
    boxRoot11->addChild(textMat);
    boxRoot11->addChild(font);
    boxRoot11->addChild(textRight);

    rightAnnotation->addChild(boxRoot11);

    // 12 尺寸线
    SoSeparator* dimensionLineRoot12 = new SoSeparator;

    SoTranslation* translation12 = new SoTranslation;
    translation12->translation.setValue(0, rightAnnotationLineLength + BoxWidth, 0);
    dimensionLineRoot12->addChild(translation12);
    dimensionLineRoot12->addChild(dimensionLineRoot10);

    rightAnnotation->addChild(dimensionLineRoot12);

    // 13 箭头
    SoSeparator* arrowRoot13 = new SoSeparator;

    SoRotation* rotation13 = new SoRotation;
    rotation13->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(-M_PI_2));
    SoTranslation* translation13 = new SoTranslation;
    translation13->translation.setValue(length + ArrowPos, width - ArrowHeight, 0);
    arrowRoot13->addChild(translation13);
    arrowRoot13->addChild(rotation13);
    arrowRoot13->addChild(arrow);

    rightAnnotation->addChild(arrowRoot13);

    // 14 尺寸界线
    SoSeparator* boundaryLineRoot14 = new SoSeparator;

    SoTranslation* translation14 = new SoTranslation;
    translation14->translation.setValue(0, width, 0);
    boundaryLineRoot14->addChild(translation14);
    boundaryLineRoot14->addChild(boundaryLineRoot8);

    rightAnnotation->addChild(boundaryLineRoot14);


    // Border Annotation
    SoSeparator* borderAnnotation = new SoSeparator;
    bool borderInternal = (borderWidth >= (2 * ArrowHeight + 2 * MinLineLen + BoxWidth));
    if (borderInternal)
    {
        // 15 尺寸界线
        SoSeparator* boundaryLineRoot15 = new SoSeparator;

        SoRotation* rotation15 = new SoRotation;
        rotation15->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(-M_PI_2));
        SoTranslation* translation15 = new SoTranslation;
        translation15->translation.setValue(0, width - borderWidth, 0);
        boundaryLineRoot15->addChild(translation15);
        boundaryLineRoot15->addChild(rotation15);
        boundaryLineRoot15->addChild(boundaryLine);

        borderAnnotation->addChild(boundaryLineRoot15);

        // 16 箭头
        SoSeparator* arrowRoot16 = new SoSeparator;

        SoRotation* rotation16 = new SoRotation;
        rotation16->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI_2));
        SoTranslation* translation16 = new SoTranslation;
        translation16->translation.setValue(-ArrowPos, width - borderWidth + ArrowHeight, 0);
        arrowRoot16->addChild(translation16);
        arrowRoot16->addChild(rotation16);
        arrowRoot16->addChild(arrow);

        borderAnnotation->addChild(arrowRoot16);

        // 17 尺寸线
        SoSeparator* dimensionLineRoot17 = new SoSeparator;

        SoCoordinate3* dimensionLineCoord17 = new SoCoordinate3;
        float borderAnnotationLineLength = (borderWidth - ArrowHeight * 2.0f - BoxWidth) / 2.0f;
        dimensionLineCoord17->point.set1Value(0, SbVec3f(-ArrowPos, width - borderWidth + ArrowHeight, 0.0f));
        dimensionLineCoord17->point.set1Value(1, SbVec3f(-ArrowPos, width - borderWidth + ArrowHeight + borderAnnotationLineLength, 0.0f));
        dimensionLineRoot17->addChild(dimensionLineCoord17);
        dimensionLineRoot17->addChild(new SoLineSet);

        borderAnnotation->addChild(dimensionLineRoot17);

        // 18 输入框
        SoSeparator* boxRoot18 = new SoSeparator;

        SoTranslation* translation18 = new SoTranslation;
        translation18->translation.setValue(-ArrowPos, width - borderWidth / 2.0f, 0);
        boxRoot18->addChild(translation18);
        boxRoot18->addChild(inputBox);

        // 文本
        SoText3* textBorder = new SoText3;
        textBorder->justification = SoText3::CENTER;
        textBorder->string.setValue((std::to_string((int)borderWidth).c_str()));

        boxRoot18->addChild(textXform);
        boxRoot18->addChild(textMat);
        boxRoot18->addChild(font);
        boxRoot18->addChild(textRight);

        borderAnnotation->addChild(boxRoot18);

        // 19 尺寸线
        SoSeparator* dimensionLineRoot19 = new SoSeparator;

        SoTranslation* translation19 = new SoTranslation;
        translation19->translation.setValue(0, borderAnnotationLineLength + BoxWidth, 0);
        dimensionLineRoot19->addChild(translation19);
        dimensionLineRoot19->addChild(dimensionLineRoot17);

        borderAnnotation->addChild(dimensionLineRoot19);

        // 20 箭头
        SoSeparator* arrowRoot20 = new SoSeparator;

        SoRotation* rotation20 = new SoRotation;
        rotation20->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(-M_PI_2));
        SoTranslation* translation20 = new SoTranslation;
        translation20->translation.setValue(-ArrowPos, width - ArrowHeight, 0);
        arrowRoot20->addChild(translation20);
        arrowRoot20->addChild(rotation20);
        arrowRoot20->addChild(arrow);

        borderAnnotation->addChild(arrowRoot20);

        // 21 尺寸界线
        SoSeparator* boundaryLineRoot21 = new SoSeparator;

        SoTranslation* translation21 = new SoTranslation;
        translation21->translation.setValue(0, borderWidth, 0);
        boundaryLineRoot21->addChild(translation21);
        boundaryLineRoot21->addChild(boundaryLineRoot15);

        borderAnnotation->addChild(boundaryLineRoot21);
    }
    else
    {
        // 15 最短尺寸线
        SoSeparator* dimensionLineRoot15 = new SoSeparator;

        SoCoordinate3* dimensionLineCoord15 = new SoCoordinate3;
        dimensionLineCoord15->point.set1Value(0, SbVec3f(-ArrowPos, width - borderWidth - ArrowHeight - MinLineLen, 0.0f));
        dimensionLineCoord15->point.set1Value(1, SbVec3f(-ArrowPos, width - borderWidth - ArrowHeight, 0.0f));
        dimensionLineRoot15->addChild(dimensionLineCoord15);
        dimensionLineRoot15->addChild(new SoLineSet);

        borderAnnotation->addChild(dimensionLineRoot15);

        // 16 箭头
        SoSeparator* arrowRoot16 = new SoSeparator;

        SoRotation* rotation16 = new SoRotation;
        rotation16->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(-M_PI_2));
        SoTranslation* translation16 = new SoTranslation;
        translation16->translation.setValue(-ArrowPos, width - borderWidth - ArrowHeight, 0);
        arrowRoot16->addChild(translation16);
        arrowRoot16->addChild(rotation16);
        arrowRoot16->addChild(arrow);

        borderAnnotation->addChild(arrowRoot16);

        // 17 尺寸界线
        SoSeparator* boundaryLineRoot17 = new SoSeparator;

        SoRotation* rotation17 = new SoRotation;
        rotation17->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(-M_PI_2));
        SoTranslation* translation17 = new SoTranslation;
        translation17->translation.setValue(0, width - borderWidth, 0);
        boundaryLineRoot17->addChild(translation17);
        boundaryLineRoot17->addChild(rotation17);
        boundaryLineRoot17->addChild(boundaryLine);

        borderAnnotation->addChild(boundaryLineRoot17);

        // 18 尺寸线
        SoSeparator* dimensionLineRoot18 = new SoSeparator;

        SoCoordinate3* dimensionLineCoord18 = new SoCoordinate3;
        dimensionLineCoord18->point.set1Value(0, SbVec3f(-ArrowPos, width - borderWidth, 0.0f));
        dimensionLineCoord18->point.set1Value(1, SbVec3f(-ArrowPos, width, 0.0f));
        dimensionLineRoot18->addChild(dimensionLineCoord18);
        dimensionLineRoot18->addChild(new SoLineSet);

        borderAnnotation->addChild(dimensionLineRoot18);

        // 19 尺寸界线
        SoSeparator* boundaryLineRoot19 = new SoSeparator;

        SoTranslation* translation19 = new SoTranslation;
        translation19->translation.setValue(0, borderWidth, 0);
        boundaryLineRoot19->addChild(translation19);
        boundaryLineRoot19->addChild(boundaryLineRoot17);

        borderAnnotation->addChild(boundaryLineRoot19);


        // 20 箭头
        SoSeparator* arrowRoot20 = new SoSeparator;

        SoRotation* rotation20 = new SoRotation;
        rotation20->rotation.setValue(SbVec3f(0, 0, 1), static_cast<float>(M_PI_2));
        SoTranslation* translation20 = new SoTranslation;
        translation20->translation.setValue(-ArrowPos, width + ArrowHeight, 0);
        arrowRoot20->addChild(translation20);
        arrowRoot20->addChild(rotation20);
        arrowRoot20->addChild(arrow);

        borderAnnotation->addChild(arrowRoot20);

        // 21 最短尺寸线
        SoSeparator* dimensionLineRoot21 = new SoSeparator;

        SoCoordinate3* dimensionLineCoord21 = new SoCoordinate3;
        dimensionLineCoord21->point.set1Value(0, SbVec3f(-ArrowPos, width + ArrowHeight, 0.0f));
        dimensionLineCoord21->point.set1Value(1, SbVec3f(-ArrowPos, width + ArrowHeight + MinLineLen, 0.0f));
        dimensionLineRoot21->addChild(dimensionLineCoord21);
        dimensionLineRoot21->addChild(new SoLineSet);

        borderAnnotation->addChild(dimensionLineRoot21);

        // 22 输入框
        SoSeparator* boxRoot22 = new SoSeparator;

        SoTranslation* translation22 = new SoTranslation;
        translation22->translation.setValue(-ArrowPos, width + ArrowHeight + MinLineLen + BoxWidth / 2.0f, 0);
        boxRoot22->addChild(translation22);
        boxRoot22->addChild(inputBox);

        // 文本
        SoText3* textBorder = new SoText3;
        textBorder->justification = SoText3::CENTER;
        textBorder->string.setValue((std::to_string((int)borderWidth).c_str()));

        boxRoot22->addChild(textXform);
        boxRoot22->addChild(textMat);
        boxRoot22->addChild(font);
        boxRoot22->addChild(textRight);

        borderAnnotation->addChild(boxRoot22);
    }

    annotationRoot->addChild(bottomAnnotation);
    annotationRoot->addChild(rightAnnotation);
    annotationRoot->addChild(borderAnnotation);

    return annotationRoot;
}

void InventorEx::meshRect()
{
    m_root->addChild(CreateGridPlane(500, 500, 10));
    m_root->addChild(CreateMeshAnnotations(500, 500, 10));
}