    
#include "InventorEx.h"

#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoFullPath.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
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
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>

#include <GL/gl.h>

#include "SoGradientBackground.h"
#include "SoColorMask.h"
#include "SoColorMaskElement.h"
#include "SoGLColorMaskElement.h"
#include "SoSwitchToChild.h"
#include "SoDeferredRender.h"

#include "utils.h"


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
        // plugin
        {"_loadPickAndWrite", std::bind(&InventorEx::loadPickAndWrite, this)},
        {"_loadErrorHandle", std::bind(&InventorEx::loadErrorHandle, this)},
        {"_loadGLCallback", std::bind(&InventorEx::loadGLCallback, this)},
        {"_loadBackground", std::bind(&InventorEx::loadBackground, this)},
    };

    m_delayedLoadNames = {
        "_loadPickAndWrite"
    };

    m_app = new QApplication(argc, argv);
    // Initializes Quarter (and implicitly also Coin and Qt
    Quarter::init();
    // Remember to initialize the custom node!
    SoOITNode::initClass();  
    SoGradientBackground::initClass();
    SoColorMaskElement::initClass();
    SoGLColorMaskElement::initClass();
    SoColorMask::initClass();
    SoSwitchToChild::initClass();
    SoDeferredRender::initClass();

    m_mainwin = new QMainWindow();
    m_mainwin->resize(WINDOWWIDTH, WINDOWHEIGHT);

    // Create a QuarterWidget for displaying a Coin scene graph
    m_viewer = new QuarterWidget(m_mainwin);
    //set default navigation mode file
    m_viewer->setNavigationModeFile();
    m_mainwin->setCentralWidget(m_viewer);

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
    m_viewer = new QuarterWidget(m_mainwin);

    m_viewer->setNavigationModeFile();
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
    // Create a perspective camera and add it to the scene graph
    SoPerspectiveCamera* myCamera = new SoPerspectiveCamera;
    m_root->addChild(myCamera);

    // Add a directional light to the scene graph
    m_root->addChild(new SoDirectionalLight);

    // Add a material node with a red color to the scene graph
    SoMaterial* myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0); // Red
    m_root->addChild(myMaterial);

    // Create a Text3 object, and connect to the realTime field
    SoText3* myText = new SoText3;
    m_root->addChild(myText);
    myText->string.connectFrom(SoDB::getGlobalField("realTime"));
    //SoElapsedTime* myCounter = new SoElapsedTime;
    //myCounter->speed = -1;
    //myText->string.connectFrom(&myCounter->timeOut);
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
    // shape
    cubeBehind(m_root);
    SoMaterial* faceMaterial = (SoMaterial*)SoNode::getByName(SbName("FaceMaterial"));
    faceMaterial->diffuseColor.setValue(1.0, 1.0, 0.0);
    faceMaterial->transparency = 0.0;
    cubeFront(m_root);
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
    renderViewer = m_viewer;
    m_viewer->setBackgroundColor(QColor(0.8f, 0.8f, 0.8f));
}


SO_NODE_SOURCE(SoOITNode);

SoOITNode::SoOITNode() 
{
    SO_NODE_CONSTRUCTOR(SoOITNode);
}

void SoOITNode::initClass() 
{
    SO_NODE_INIT_CLASS(SoOITNode, SoSeparator, "Separator");
    SO_ENABLE(SoGLRenderAction, SoCacheElement);
}

void SoOITNode::GLRender(SoGLRenderAction* action) 
{
    // 1. 绑定 OIT 资源 (例如头指针纹理，片段列表缓冲区等)

    // 2. 执行 Build Phase 渲染，渲染所有子节点

    // 3. 执行 Resolve Phase 渲染

    // 4. 解绑 OIT 资源

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

    SoState* state = action->getState();
    SoGLLazyElement* lazyElt = (SoGLLazyElement*)SoLazyElement::getInstance(state);
    lazyElt->reset(state, (SoLazyElement::DIFFUSE_MASK) | (SoLazyElement::LIGHT_MODEL_MASK));
}

void SoOITNode::GLRenderBelowPath(SoGLRenderAction* action) 
{
    this->GLRender(action);
    SoSeparator::GLRenderBelowPath(action);
}

void SoOITNode::setupOITResources() {
    // Initialize OpenGL resources for OIT.
    // For instance, shader compilation, VBO generation, etc.
    // You can refer to the OpenGL Red Book example for details.
}

void SoOITNode::cleanupOITResources() {
    // Cleanup OpenGL resources.
    // This includes deleting shaders, VBOs, textures, etc.
}

void InventorEx::oit()
{
    buildFloor();

    SoOITNode* oitNode = new SoOITNode;
    m_root->addChild(oitNode);
    renderCamera = new SoPerspectiveCamera;
    renderCamera->position.setValue(0.0f, 0.0f, 5.0f);
    renderCamera->heightAngle = (float)(M_PI / 2.0f);
    renderCamera->nearDistance = 2.0f;
    renderCamera->farDistance = 12.0f;
    oitNode->addChild(renderCamera);// 为什么放root下不行
    //oitNode->addChild(new SoCube);
    buildScene(oitNode);
    renderViewer = m_viewer;
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

    renderViewer = m_viewer;
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
    CREATE_NODE(SoCylinder, cylinder);

    std::vector<std::pair<SoGroup*, SoNode*>> relationships =
    {
        {m_root, cylinder},
    };

    for (const auto& relationship : relationships)
    {
        ADD_CHILD(relationship.first, relationship.second);
    }

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