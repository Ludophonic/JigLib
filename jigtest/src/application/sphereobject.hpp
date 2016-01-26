//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file sphereobject.hpp 
//                     
//==============================================================
#ifndef SPHEREOBJECT_HPP
#define SPHEREOBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

/// This is a game-object - i.e. a sphere that has a body, has
/// collision, and can be rendered.
class tSphereObject : public tObject
{
public:
  tSphereObject(JigLib::tScalar radius);
  ~tSphereObject(); 
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
