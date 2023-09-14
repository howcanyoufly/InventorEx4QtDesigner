#pragma once
#include <iostream>
#include <vector>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include "InventorEx.h"


inline SoSeparator* readFile(const char* filename)
{
    // Open the input file
    SoInput mySceneInput;
    if (!mySceneInput.openFile(filename))
    {
        std::cerr << "Cannot open file " << filename << std::endl;
        return NULL;
    }

    // Read the whole file into the database
    SoSeparator* myGraph = SoDB::readAll(&mySceneInput);
    if (myGraph == NULL)
    {
        std::cerr << "Problem reading file" << std::endl;
        return NULL;
    }

    mySceneInput.closeFile();
    return myGraph;
}

template<typename T> 
T* searchNode(SoNode* scene)
{
    T* ret = NULL;

    SoSearchAction* sa = new SoSearchAction;
    sa->setType(T::getClassTypeId());

    sa->apply(scene);

    SoPath* path = sa->getPath();
    if (!path)
    {
        std::cerr << T::getClassTypeId().getName().getString() << " not found" << std::endl;
    }
    ret = dynamic_cast<T*>(path->getTail());

    delete sa;
    return ret;
}

template<typename T> 
std::vector<T*> searchNodes(SoNode* scene)
{
    SoSearchAction* sa = new SoSearchAction;
    sa->setType(T::getClassTypeId());
    sa->setInterest(SoSearchAction::ALL);

    sa->apply(scene);

    const SoPathList& path = sa->getPaths();
    if (path.getLength() == 0)
    {
        std::cerr << T::getClassTypeId().getName().getString() << " not found" << std::endl;
    }

    std::vector<T*> ret;
    for (size_t i = 0; i < (size_t)path.getLength(); i++)
        ret.push_back(dynamic_cast<T*>(path[i]->getTail()));

    delete sa;
    return ret;
}


// search a node by name
template<typename T> 
T* searchName(SoNode* scene, SbName name)
{
    if (!scene)
    {
        T* node = (T*)SoNode::getByName(name);
        if (node)
            return node;
    }
    else
    {
        T* ret = NULL;

        SoSearchAction* sa = new SoSearchAction;
        sa->setSearchingAll(true);
        sa->setType(T::getClassTypeId());
        sa->setName(name);

        sa->apply(scene);

        SoPath* path = sa->getPath();
        if (path)
        {
            ret = dynamic_cast<T*>(path->getTail());
            delete sa;
            return ret;
        }
        delete sa;
    }

    std::cerr << T::getClassTypeId().getName().getString() << " not found" << std::endl;
    return(NULL);
}

template<typename T>
std::vector<T*> searchNodes(SoNode* scene, SbName name)
{
    SoSearchAction* sa = new SoSearchAction;
    sa->setSearchingAll(true);
    sa->setType(T::getClassTypeId());
    sa->setName(name);
    sa->setInterest(SoSearchAction::ALL);

    sa->apply(scene);

    const SoPathList& path = sa->getPaths();
    if (path.getLength() == 0)
    {
        std::cerr << T::getClassTypeId().getName().getString() << " not found" << std::endl;
    }

    std::vector<T*> ret;
    for (size_t i = 0; i < (size_t)path.getLength(); i++)
        ret.push_back(dynamic_cast<T*>(path[i]->getTail()));

    delete sa;
    return ret;
}

typedef SoGLRenderAction::AbortCode SoGLRenderAbortCB(void* userdata);
inline void copyGLRenderAction(const SoGLRenderAction& sourceAct, SoGLRenderAction& targetAct)
{
    // Viewport and update area settings
    targetAct.setViewportRegion(sourceAct.getViewportRegion());
    SbVec2f origin, size;
    sourceAct.getUpdateArea(origin, size);
    targetAct.setUpdateArea(origin, size);

    // Abort callback settings
    SoGLRenderAbortCB* abortCallback = NULL;
    void* abortUserData = NULL;
    sourceAct.getAbortCallback(abortCallback, abortUserData);
    targetAct.setAbortCallback(abortCallback, abortUserData);

    // Transparency settings
    targetAct.setTransparencyType(sourceAct.getTransparencyType());
    targetAct.setTransparentDelayedObjectRenderType(sourceAct.getTransparentDelayedObjectRenderType());

    // Smoothing settings
    targetAct.setSmoothing(sourceAct.isSmoothing());

    // Render pass settings
    targetAct.setNumPasses(sourceAct.getNumPasses());
    targetAct.setPassUpdate(sourceAct.isPassUpdate());

    // Cache and remote rendering settings
    targetAct.setCacheContext(sourceAct.getCacheContext());
    targetAct.setRenderingIsRemote(sourceAct.getRenderingIsRemote());

    // Delayed paths and sorting settings
    targetAct.setSortedLayersNumPasses(sourceAct.getSortedLayersNumPasses());
    targetAct.setSortedObjectOrderStrategy(SoGLRenderAction::BBOX_CENTER);
    targetAct.setDelayedObjDepthWrite(sourceAct.getDelayedObjDepthWrite());
}
