//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file renderobject.hpp 
//                     
//==============================================================
#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include "jiglib.hpp"

/// Base class for an object that can render itself and be registered
/// with the render manager.
class tRenderObject
{
public:
  enum tRenderObjectType {DYNAMIC_OBJECT, STATIC_OBJECT, TERRAIN, BACKGROUND};
  
  tRenderObject(tRenderObjectType type);
  virtual ~tRenderObject();
  
  tRenderObjectType GetType() const {return mType;}
  
  enum tRenderType {WIREFRAME, NORMAL, FOR_SHADOW};
  /// This should draw the object in its local reference frame
  virtual void Render(tRenderType renderType) = 0;
  
  /// return a bounding sphere - used for culling (maybe!)
  virtual const JigLib::tSphere & GetRenderBoundingSphere() const = 0;
  
  /// return a position suitable for use as a camera focus
  virtual const JigLib::tVector3 & GetRenderPosition() const = 0;
  
  /// return an orientation suitable for use when calculating camera
  /// orientation
  virtual const JigLib::tMatrix33 & GetRenderOrientation() const = 0;
  
  /// Can attach an object to a render manager after it's created -
  /// this is called by tRenderManager.
  void SetRenderManager(class tRenderManager * rm) {mRenderManager = rm;}
  
  /// May return 0
  class tRenderManager * GetRenderManager() const {return mRenderManager;}
  
private:
  tRenderObjectType mType;
  class tRenderManager * mRenderManager;
};

#endif
