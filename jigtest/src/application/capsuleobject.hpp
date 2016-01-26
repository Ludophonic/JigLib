//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file capsuleobject.hpp 
//                     
//==============================================================
#ifndef CAPSULEOBJECT_HPP
#define CAPSULEOBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

/// This is a game-object - i.e. a capsule that has a body, has
/// collision, and can be rendered.
class tCapsuleObject : public tObject
{
public:
  tCapsuleObject(JigLib::tScalar radius, JigLib::tScalar length, const JigLib::tMatrix33 & orient = JigLib::tMatrix33::Identity());
  ~tCapsuleObject();
  void SetProperties(JigLib::tScalar elasticity, 
                     JigLib::tScalar staticFriction,
                     JigLib::tScalar dynamicFriction);
  void SetDensity(JigLib::tScalar density, JigLib::tPrimitive::tPrimitiveProperties::tMassDistribution massDistribution = JigLib::tPrimitive::tPrimitiveProperties::SOLID);
  void SetMass(JigLib::tScalar mass, JigLib::tPrimitive::tPrimitiveProperties::tMassDistribution massDistribution = JigLib::tPrimitive::tPrimitiveProperties::SOLID);
  
  JigLib::tBody * GetBody() {return &mBody;}
  
  // inherited from tObject - interpolated between the physics old and
  // current positions.
  void SetRenderPosition(JigLib::tScalar renderFraction);
  
  // inherited from tRenderObject
  void Render(tRenderType renderType);
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
private:
  JigLib::tBody mBody;
  JigLib::tCollisionSkin mCollisionSkin;
  
  JigLib::tVector3 mRenderPosition;
  JigLib::tMatrix33 mRenderOrientation;
  
  GLuint mDisplayListNum;
};

#endif
