//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file planeobject.hpp 
//                     
//==============================================================
#ifndef PLANEOBJECT_HPP
#define PLANEOBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"

#include "jiglib.hpp"

/// This is a game-object - i.e. a plane that has no body but it has
/// collision, and can be rendered.
class tPlaneObject : public tObject
{
public:
  tPlaneObject(const JigLib::tPlane & plane);
  ~tPlaneObject();
  
  void SetProperties(JigLib::tScalar elasticity, 
                     JigLib::tScalar staticFriction,
                     JigLib::tScalar dynamicFriction);
  
  JigLib::tBody * GetBody() {return 0;}
  JigLib::tCollisionSkin * GetCollisionSkin() {return &mCollisionSkin;}
  
  // inherited from tRenderObject
  void Render(tRenderType renderType);
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
private:
  JigLib::tCollisionSkin mCollisionSkin;
  class tRGBAFileTexture * mTexture;
  GLuint mDisplayListNum;
};

#endif
