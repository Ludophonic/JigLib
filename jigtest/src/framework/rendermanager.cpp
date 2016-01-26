//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file rendermanager.cpp 
//                     
//==============================================================
#include "rendermanager.hpp"
#include "renderobject.hpp"
#include "viewport.hpp"
#include "camera.hpp"
#include "graphics.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tRenderManager
//==============================================================
tRenderManager::tRenderManager() : mWireFrame(false)
{
  TRACE_METHOD_ONLY(ONCE_1);
}

tRenderManager::~tRenderManager()
{
  TRACE_METHOD_ONLY(ONCE_1);
  int i;
  for (i = 0 ; i < (int) mObjects.size() ; ++i)
    mObjects[i]->SetRenderManager(0);
}

//==============================================================
// AddObject
//==============================================================
void tRenderManager::AddObject(tRenderObject * object)
{
  TRACE_METHOD_ONLY(FRAME_1);
  if (mObjects.end() == find(mObjects.begin(), mObjects.end(), object))
    mObjects.push_back(object);
  else
    TRACE("Warning: tried to add object %p to render manager "
          "but it's already registered\n", object);
  object->SetRenderManager(this);
}

//==============================================================
// RemoveObject
//==============================================================
bool tRenderManager::RemoveObject(tRenderObject * object)
{
  TRACE_METHOD_ONLY(FRAME_1);
  object->SetRenderManager(0);
  tObjects::iterator it = 
    find(mObjects.begin(), mObjects.end(), object);
  if (mObjects.end() == it)
    return false;
  mObjects.erase(it);
  return true;
}

//==============================================================
// RenderAllObjects
//==============================================================
void tRenderManager::RenderAllObjects(class tViewport & currentViewport)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int num = mObjects.size();
  int i;

  if (mWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
  }

  for (i = 0 ; i < num ; ++i)
  {
    tSaveGLMatrixState state;
    if (mObjects[i]->GetType() == tRenderObject::BACKGROUND)
    {
      GLTRANSLATE(currentViewport.GetCamera()->GetPosition().x, 
                  currentViewport.GetCamera()->GetPosition().y, 
                  currentViewport.GetCamera()->GetPosition().z);
    }
    mObjects[i]->Render(tRenderObject::NORMAL);
  }
  
}

