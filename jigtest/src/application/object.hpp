//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file object.hpp 
//                     
//==============================================================
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "jiglib.hpp"
#include "renderobject.hpp"

class tObject : public tRenderObject
{
public:
  tObject();
  virtual ~tObject();
  
  virtual JigLib::tBody * GetBody() {return 0;}
  
  /// Default implementation tries to get a skin from the body, if it exists
  virtual JigLib::tCollisionSkin * GetCollisionSkin();
  
  /// derived class should use this to interpolate the render position
  /// between the old and current physics positions.
  virtual void SetRenderPosition(JigLib::tScalar renderFraction) {}
  
  /// Set the physics position (may move multiple bodies if it's a jointed structure)
  /// default just sets the body from GetBody if it returns non-zero
  virtual void SetPhysicsPosition(const JigLib::tVector3& pos, const JigLib::tMatrix33& orient);

  /// Set the physics velocity (like SetPhysicsPosition)
  virtual void SetPhysicsVelocity(const JigLib::tVector3& vel);

  /// helper
  void RenderWorldBoundingBox(const JigLib::tVector3 * col = 0);

  /// Set really basic physical properties
  virtual void SetProperties(JigLib::tScalar elasticity, 
                             JigLib::tScalar staticFriction,
                             JigLib::tScalar dynamicFriction) {}
  virtual void SetDensity(JigLib::tScalar density) {}
private:
  tObject(const tObject &);
};


#endif
