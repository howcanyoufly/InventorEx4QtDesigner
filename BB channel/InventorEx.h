#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>

#include <Inventor/nodes/SoSeparator.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

using namespace SIM::Coin3D::Quarter;

class InventorEx {
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
    void cubeFront();
    void cubeBehind();
    void twoCube();
    void pickAction();

    // plugins
    void loadPickAndWrite();
    void loadErrorHandle();

    std::map<std::string, std::function<void(void)>> m_functions;
    std::set<std::string> m_delayedLoadNames;
    std::vector<std::function<void(void)>> m_delayedLoadPlugins;

    QApplication* m_app;
    QMainWindow* m_mainwin;
    QuarterWidget* m_viewer;
    SoSeparator* m_root;
    bool m_reset;
};
