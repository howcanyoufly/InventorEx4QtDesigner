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

    // Order-Independent-Transparency, a preliminary attempt£¬riddled with errors and flaws
    void oit();

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
    void previewPointForward();
    void twoSideFace();
    void auxViewport();

    void actStateOfDelayList();
    void deferredRender();
    void flat();
    void switchToPathTraversal();

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
    QuarterWidget* m_viewer;
    SoSeparator* m_root;

    bool m_reset;// function does but plugin does not
};


class SoOITNode : public SoSeparator 
{
    SO_NODE_HEADER(SoOITNode);

public:
    static void initClass();
    SoOITNode();

protected:
    virtual ~SoOITNode() {};
    virtual void GLRender(SoGLRenderAction* action) override;
    virtual void GLRenderBelowPath(SoGLRenderAction* action) override;

private:
    // OpenGL resources related to OIT like shaders, VBOs, etc.
    GLuint  list_build_program;
    GLuint  head_pointer_texture;
    // ... [Other OpenGL resources as needed]

    void setupOITResources(); // This function will initialize all OpenGL resources related to OIT.
    void cleanupOITResources(); // This function will delete and cleanup all OpenGL resources.

};
