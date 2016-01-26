//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file carobject.hpp 
//                     
//==============================================================
#ifndef CAROBJECT_HPP
#define CAROBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"

#include "jiglib.hpp"

/// This is a game-object - i.e. a car that can be rendered.
class tCarObject : public tObject
{
public:
  tCarObject(bool FWDrive,
             bool RWDrive,
             JigLib::tScalar maxSteerAngle,
             JigLib::tScalar steerRate,
             JigLib::tScalar wheelSideFriction,
             JigLib::tScalar wheelFwdFriction,
             JigLib::tScalar wheelTravel,
             JigLib::tScalar wheelRadius,
             JigLib::tScalar wheelZOffset,
             JigLib::tScalar wheelRestingFrac,
             JigLib::tScalar wheelDampingFrac,
             int             wheelNumRays,
             JigLib::tScalar driveTorque,
             JigLib::tScalar gravity);
  ~tCarObject();
  void SetProperties(JigLib::tScalar elasticity, 
                     JigLib::tScalar staticFriction,
                     JigLib::tScalar dynamicFriction);
  void SetDensity(JigLib::tScalar density);
  void SetMass(JigLib::tScalar mass);
  
  JigLib::tCar & GetCar() {return mCar;}
  
  // inherited from tObject 
  JigLib::tBody * GetBody() {return &mCar.GetChassis().GetBody();}
  
  // inherited from tObject - interpolated between the physics old and
  // current positions.
  void SetRenderPosition(JigLib::tScalar renderFraction);
  
  // inherited from tRenderObject
  void Render(tRenderType renderType);
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
private:
  JigLib::tCar mCar;
  
  JigLib::tVector3 mRenderPosition;
  JigLib::tMatrix33 mRenderOrientation;
  
  GLuint mDisplayListNum;
  GLuint mWheelDisplayListNum;
};

#endif
