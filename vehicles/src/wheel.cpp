//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file wheel.cpp 
//                     
//==============================================================
#include "wheel.hpp"
#include "car.hpp"
#include "chassis.hpp"

#include "line.hpp"
#include "body.hpp"
#include "collisionsystem.hpp"
#include "trace.hpp"

using namespace JigLib;

//==============================================================
// tWheel
//==============================================================
tWheel::tWheel()
{
  TRACE_METHOD_ONLY(ONCE_2);
  Setup(0, tVector3::Zero(), tVector3::Up(),
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
}

//==============================================================
// Setup
//==============================================================
void tWheel::Setup(tCar * car,
                   const tVector3 & pos, 
                   const tVector3 & axisUp,
                   tScalar spring,  
                   tScalar travel,  
                   tScalar inertia, 
                   tScalar radius,
                   tScalar sideFriction,
                   tScalar fwdFriction,
                   tScalar damping,
                   int numRays)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mCar = car;
  mPos = pos;
  mAxisUp = axisUp;
  mSpring = spring;
  mTravel = travel;
  mInertia = inertia;
  mRadius = radius;
  mSideFriction = sideFriction;
  mFwdFriction = fwdFriction;
  mDamping = damping;
  mNumRays = numRays;
  Reset();
}

/// Predicate for the wheel->world intersection test
class tWheelPred : public tCollisionSkinPredicate1
{
public:
  tWheelPred(tCollisionSkin * carSkin) : mSkin(carSkin) {}
  bool ConsiderSkin(class tCollisionSkin * skin) const
    {
      return (skin != mSkin);
    }
  const tCollisionSkin * const mSkin;
};

//==============================================================
// AddForcesToCar
//==============================================================
bool tWheel::AddForcesToCar(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  tVector3 force(0.0f);
  mLastDisplacement = mDisplacement;
  mDisplacement = 0.0f;
  
  tBody & carBody = mCar->GetChassis().GetBody();
  
  tVector3 worldPos = carBody.GetPosition() + carBody.GetOrientation() * mPos;
  tVector3 worldAxis = carBody.GetOrientation() * mAxisUp;
  tVector3 wheelFwd = RotationMatrix(mSteerAngle, worldAxis) * carBody.GetOrientation().GetCol(0);
  tVector3 wheelUp = worldAxis;
  tVector3 wheelLeft = Cross(wheelUp, wheelFwd).Normalise();
  wheelUp = Cross(wheelFwd, wheelLeft);
  
  // start of ray
  tScalar rayLen = 2.0f * mRadius + mTravel;
  tVector3 wheelRayEnd = worldPos - mRadius * worldAxis;
  tSegment wheelRay(wheelRayEnd + rayLen * worldAxis, -rayLen * worldAxis);
  
  Assert(tPhysicsSystem::GetCurrentPhysicsSystem());
  tCollisionSystem * collSystem = tPhysicsSystem::GetCurrentPhysicsSystem()->GetCollisionSystem();
  Assert(collSystem);
  
  // do a number of rays, and choose the deepest penetration
  const int maxNumRays = 32;
  const int numRays = Min(mNumRays, maxNumRays);
  static tScalar fracs[maxNumRays];
  static tCollisionSkin * otherSkins[maxNumRays];
  static tVector3 groundPositions[maxNumRays];
  static tVector3 groundNormals[maxNumRays];
  static tSegment segments[maxNumRays];
  
  // adjust the start position of the ray - divide the wheel into numRays+2 
  // rays, but don't use the first/last.
  tScalar deltaFwd = (2.0f * mRadius) / (numRays + 1);
  tScalar deltaFwdStart = deltaFwd;
  
  tWheelPred pred(carBody.GetCollisionSkin());
  mLastOnFloor = false;
  int bestIRay = 0;
  int iRay;
  for (iRay = 0 ; iRay < numRays ; ++iRay)
  {
    fracs[iRay] = SCALAR_HUGE;
    // work out the offset relative to the middle ray
    tScalar distFwd = (deltaFwdStart + iRay * deltaFwd) - mRadius;
    tScalar zOffset = mRadius * (1.0f - CosDeg( 90.0f * (distFwd / mRadius) ) );
    segments[iRay] = wheelRay;
    segments[iRay].mOrigin += distFwd * wheelFwd + zOffset * wheelUp;
    if (collSystem->SegmentIntersect(fracs[iRay], otherSkins[iRay], 
                                     groundPositions[iRay], groundNormals[iRay], segments[iRay], &pred))
    {
      mLastOnFloor = true;
      if (fracs[iRay] < fracs[bestIRay])
        bestIRay = iRay;
    }
  }
  if (!mLastOnFloor)
    return false;
  Assert(bestIRay < numRays);
  
  // use the best one
  const tVector3 & groundPos = groundPositions[bestIRay];
  tScalar frac = fracs[bestIRay];
  tCollisionSkin * otherSkin = otherSkins[bestIRay];
//  const tVector3 groundNormal = (worldPos - segments[bestIRay].GetEnd()).NormaliseSafe();
//  const tVector3 groundNormal = groundNormals[bestIRay];
  tVector3 groundNormal(worldAxis);
  if (numRays > 1)
  {
    for (iRay = 0 ; iRay < numRays ; ++iRay)
    {
      if (fracs[iRay] <= 1.0f)
      {
        groundNormal += (1.0f - fracs[iRay]) * (worldPos - segments[iRay].GetEnd());
      }
    }
    groundNormal.NormaliseSafe();
  }
  else
  {
    groundNormal = groundNormals[bestIRay];
  }
  
  Assert(otherSkin);
  tBody * worldBody = otherSkin->GetOwner();
  
  mDisplacement = rayLen * (1.0f - frac);
  Limit(mDisplacement, SCALAR(0.0f), mTravel);
  
  tScalar displacementForceMag = mDisplacement * mSpring;
  
  // reduce force when suspension is par to ground
  displacementForceMag *= Dot(groundNormals[bestIRay], worldAxis);
  
  // apply damping
  tScalar dampingForceMag = mUpSpeed * mDamping;
  
  tScalar totalForceMag = displacementForceMag + dampingForceMag;
  if (totalForceMag < 0.0f) totalForceMag = 0.0f;
  
  tVector3 extraForce = totalForceMag * worldAxis;
  
  force += extraForce;
  
  // side-slip friction and drive force. Work out wheel- and floor-relative coordinate frame
  tVector3 groundUp = groundNormal;
  tVector3 groundLeft = Cross(groundNormal, wheelFwd).NormaliseSafe();
  tVector3 groundFwd = Cross(groundLeft, groundUp);
  
  tVector3 wheelPointVel = carBody.GetVelocity() +
    Cross(carBody.GetAngVel(), carBody.GetOrientation() * mPos);
  
  tVector3 rimVel = mAngVel * Cross(wheelLeft, groundPos - worldPos);
  wheelPointVel += rimVel;
  
  // if sitting on another body then adjust for its velocity.
  if (worldBody)
  {
    tVector3 worldVel = worldBody->GetVelocity() + 
      Cross(worldBody->GetAngVel(), groundPos - worldBody->GetPosition());
    wheelPointVel -= worldVel;
  }
  
  // sideways forces
  const tScalar noslipVel  = 0.2f;
  const tScalar slipVel    = 0.4f;
  const tScalar slipFactor = 0.7f;
  
  const tScalar smallVel  = 3.0;
  tScalar friction = mSideFriction;
  
  tScalar sideVel = Dot(wheelPointVel, groundLeft);
  if ( (sideVel >  slipVel) || (sideVel < -slipVel) )
    friction *= slipFactor;
  else if ( (sideVel >  noslipVel) || (sideVel < -noslipVel) )
    friction *= 1.0f -  (1.0f - slipFactor) * (Abs(sideVel) - noslipVel) / (slipVel - noslipVel);
  
  if (sideVel < 0.0f)
    friction *= -1.0f;
  if (Abs(sideVel) < smallVel)
    friction *= Abs(sideVel) / smallVel;
  
  tScalar sideForce = -friction * totalForceMag;
  
  extraForce = sideForce * groundLeft;
  force += extraForce;
  
  // fwd/back forces
  friction = mFwdFriction;
  tScalar fwdVel = Dot(wheelPointVel, groundFwd);
  if ( (fwdVel >  slipVel) || (fwdVel < -slipVel) )
    friction *= slipFactor;
  else if ( (fwdVel >  noslipVel) || (fwdVel < -noslipVel) )
    friction *= 1.0f -  (1.0f - slipFactor) * (Abs(fwdVel) - noslipVel) / (slipVel - noslipVel);
  
  if (fwdVel < 0.0f)
    friction *= -1.0f;
  if (Abs(fwdVel) < smallVel)
    friction *= Abs(fwdVel) / smallVel;
  
  tScalar fwdForce = -friction * totalForceMag;
  
  extraForce = fwdForce * groundFwd;
  force += extraForce;
  
  if (!force.IsSensible())
  {
    TRACE_FILE_IF(ONCE_1)
      TRACE("Bad force in car wheel\n");
    return true;
  }

  // fwd force also spins the wheel
  tVector3 wheelCentreVel = carBody.GetVelocity() +
    Cross(carBody.GetAngVel(), carBody.GetOrientation() * mPos);
  mAngVelForGrip = Dot(wheelCentreVel, groundFwd) / mRadius;
  mTorque += -fwdForce * mRadius;
  
  // add force to car
  carBody.AddWorldForce(force, groundPos);
  // add force to the world
  if (worldBody && !worldBody->GetImmovable())
  {
    // todo get the position in the right place...
    // also limit the velocity that this force can produce by looking at the 
    // mass/inertia of the other object
    tScalar maxOtherBodyAcc = 500.0f;
    tScalar maxOtherBodyForce = maxOtherBodyAcc * worldBody->GetMass();
    if (force.GetLengthSq() > Sq(maxOtherBodyForce))
      force *= maxOtherBodyForce / force.GetLength();
    worldBody->AddWorldForce(-force, groundPos);
  }
  return true;
}

//==============================================================
// Update
//==============================================================
void tWheel::Update(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  if (dt <= 0.0f)
    return;
  
  tScalar origAngVel = mAngVel;
  mUpSpeed = (mDisplacement - mLastDisplacement) / Max(dt, SCALAR_TINY);
  
  if (mLocked)
  {
    mAngVel = 0.0;
    mTorque = 0.0;
  }
  else
  {
    mAngVel += mTorque * dt / mInertia;
    mTorque = 0.0;
    
    // prevent friction from reversing dir - todo do this better
    // by limiting the torque
    if ( ( (origAngVel > mAngVelForGrip) && (mAngVel < mAngVelForGrip) ) ||
         ( (origAngVel < mAngVelForGrip) && (mAngVel > mAngVelForGrip) ) )
      mAngVel = mAngVelForGrip;
    
    mAngVel += mDriveTorque * dt / mInertia;
    mDriveTorque = 0.0;
    
    const tScalar maxAngVel = 200.0;
    Limit(mAngVel, -maxAngVel, maxAngVel);
    
    mAxisAngle += Rad2Deg(dt * mAngVel);
  }
}

//==============================================================
// Reset
//==============================================================
void tWheel::Reset()
{
  TRACE_METHOD_ONLY(ONCE_3);
  mAngVel = 0.0f;
  mSteerAngle = 0.0f;
  mTorque = 0.0f;
  mDriveTorque = 0.0f;
  mAxisAngle = 0.0f;
  mDisplacement = 0.0f;
  mUpSpeed = 0.0f;
  mLocked = false;
  mLastDisplacement = 0.0f;
  mLastOnFloor = false;
  mAngVelForGrip = 0.0f;
}


