//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file characterobject.hpp 
//                     
//==============================================================
#ifndef CHARACTEROBJECT_HPP
#define CHARACTEROBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"

#include "jiglib.hpp"

/// This is a game-object - i.e. a character that can be rendered.
class tCharacterObject : public tObject
{
public:
  tCharacterObject(JigLib::tScalar radius, JigLib::tScalar height);
  ~tCharacterObject();
  void SetProperties(JigLib::tScalar elasticity, 
                     JigLib::tScalar staticFriction,
                     JigLib::tScalar dynamicFriction);
  void SetDensity(JigLib::tScalar density, JigLib::tPrimitive::tPrimitiveProperties::tMassDistribution massDistribution = JigLib::tPrimitive::tPrimitiveProperties::SOLID);
  void SetMass(JigLib::tScalar mass, JigLib::tPrimitive::tPrimitiveProperties::tMassDistribution massDistribution = JigLib::tPrimitive::tPrimitiveProperties::SOLID);
  

  // inherited from tObject 
  JigLib::tBody * GetBody() {return &mBody;}
  
  // inherited from tObject - interpolated between the physics old and
  // current positions.
  void SetRenderPosition(JigLib::tScalar renderFraction);
  
  // inherited from tRenderObject
  void Render(tRenderType renderType);
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
  /// If start, then set control to be forward, else stop going forward
  void ControlFwd(bool start);
  void ControlBack(bool start);
  void ControlLeft(bool start);
  void ControlRight(bool start);
  void ControlJump();

private:
  class tCharacterBody : public JigLib::tBody
  {
  public:
    virtual void PostPhysics(JigLib::tScalar dt);
    JigLib::tScalar mDesiredFwdSpeed;
    JigLib::tScalar mDesiredLeftSpeed;
    JigLib::tScalar mJumpSpeed;

    // used for smoothing
    JigLib::tScalar mFwdSpeed;
    JigLib::tScalar mLeftSpeed;
    JigLib::tScalar mFwdSpeedRate;
    JigLib::tScalar mLeftSpeedRate;
    JigLib::tScalar mTimescale;
  };

  void UpdateControl();

  tCharacterBody mBody;
  JigLib::tCollisionSkin mCollisionSkin;
  
  JigLib::tVector3 mLookDir;

  JigLib::tVector3 mRenderPosition;
  JigLib::tMatrix33 mRenderOrientation;
  
  GLuint mDisplayListNum;

  enum tMoveDir {MOVE_NONE  = 1 << 0,
                 MOVE_FWD   = 1 << 1,
                 MOVE_BACK  = 1 << 2,
                 MOVE_LEFT  = 1 << 3,
                 MOVE_RIGHT = 1 << 4};
  /// bitmask from tMoveDir
  unsigned mMoveDir;

  JigLib::tScalar mBodyAngle;   ///< in degrees - rotation around z
  JigLib::tScalar mLookUpAngle; ///< in Degrees

  JigLib::tScalar mMoveFwdSpeed;
  JigLib::tScalar mMoveBackSpeed;
  JigLib::tScalar mMoveSideSpeed;
  JigLib::tScalar mJumpSpeed;

  bool mDoJump;
};

#endif
