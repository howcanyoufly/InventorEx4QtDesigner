#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSubNode.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

using namespace SIM::Coin3D::Quarter;

class InventorEx 
{
public:
    InventorEx(int argc, char** argv);
    virtual ~InventorEx();

    void resetScene();
    void run(std::string& funcName);
    const std::map<std::string, std::function<void(void)>>& getFunctions() const;

private:
    // 参考示例
    void superScene();

    // functions 节点树
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
    void cubeFront(SoSeparator* root);
    void cubeBehind(SoSeparator* root);
    void twoCube();
    void pickAction();

    void buildScene(SoSeparator* root);
    void buildFloor();
    void glCallback();
    
    void oit();

    // plugins
    void loadPickAndWrite();
    void loadErrorHandle();
    void loadGLCallback();
    void loadBackground(void);


    std::map<std::string, std::function<void(void)>> m_functions;
    std::set<std::string> m_delayedLoadNames;
    std::vector<std::function<void(void)>> m_delayedLoadPlugins;

    QApplication* m_app;
    QMainWindow* m_mainwin;
    QuarterWidget* m_viewer;
    SoSeparator* m_root;
    bool m_reset;
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

private:
    void renderGround();
    void renderAxes();
};