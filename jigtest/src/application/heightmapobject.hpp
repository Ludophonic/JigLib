//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file heightmapobject.hpp 
//                     
//==============================================================
#ifndef HEIGHTMAPOBJECT_HPP
#define HEIGHTMAPOBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"

#include "jiglib.hpp"

#include <string>

/// This is a game-object - i.e. a heightmap that has no body but it has
/// collision, and can be rendered.
class tHeightmapObject : public tObject
{
public:
  tHeightmapObject(const JigLib::tHeightmap & heightmap);
  tHeightmapObject(const std::string & fileName, 
                   JigLib::tScalar dx, JigLib::tScalar dy, 
                   JigLib::tScalar zMin, JigLib::tScalar zMax);

  ~tHeightmapObject();
  
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
