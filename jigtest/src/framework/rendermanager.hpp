//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file rendermanager.hpp 
//                     
//==============================================================
#ifndef RENDERMANAGER_HPP
#define RENDERMANAGER_HPP

#include "jiglib.hpp"

#include <vector>

class tRenderManager
{
public:
  tRenderManager();
  ~tRenderManager();
  
  void AddObject(class tRenderObject * object);
  bool RemoveObject(class tRenderObject * object);
  
  std::vector<class tRenderObject *> GetObjects() const {
    return mObjects; }
  
  void RenderAllObjects(class tViewport & currentViewport);

  void SetWireFrameMode(bool wire) {mWireFrame = wire;}
private:
  typedef std::vector<class tRenderObject *> tObjects;
  tObjects mObjects;
  bool mWireFrame;
};

#endif
