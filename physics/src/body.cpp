//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file body.cpp 
//                     
//==============================================================
#include "body.hpp"
#include "collisionskin.hpp"
#include "constraint.hpp"

using namespace JigLib;
using namespace std;

tScalar tBody::mVelMax = SCALAR(100.0f);
tScalar tBody::mAngVelMax = SCALAR(50.0f);


//==============================================================
// tBody
//==============================================================
tBody::tBody()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBodiesToBeActivatedOnMovement.reserve(8);
  mBodyEnabled = false;
  mCollSkin = 0;
  
  SetMass(SCALAR(1.0f));
  SetBodyInertia(SCALAR(1.0f), SCALAR(1.0f), SCALAR(1.0f));
  
  mTransform.position.SetTo(SCALAR(0.0f));
  SetOrientation(tMatrix33::Identity());
  mTransformRate.SetToZero();
  mTransformRateAux.SetToZero();
  
  mImmovable = false;
  mOrigImmovable = false;
  mDoShockProcessing = true;

  mForce.SetTo(SCALAR(0.0f));
  mTorque.SetTo(SCALAR(0.0f));
  
  mVelChanged = true;

  mActivity = ACTIVE;
  mInactiveTime = SCALAR(0.0f);
  mDeactivationTime = SCALAR(1.0f);
  SetActivityThreshold(SCALAR(0.5f), SCALAR(30.0f));
  SetDeactivationThreshold(SCALAR(0.1f), SCALAR(0.2f));
  mAllowFreezing = true;
  mLastPositionForDeactivation = mTransform.position;
  mLastOrientationForDeactivation = mTransform.orientation;
  
  CopyCurrentStateToOld();
}

//==============================================================
// ~tBody
//==============================================================
tBody::~tBody()
{
  TRACE_METHOD_ONLY(ONCE_2);
  // notify all the constraints. Use a copy of our list, since they'll
  // try to modify it
  vector<tConstraint *> constraints = mConstraints;
  for (unsigned i = 0 ; i < constraints.size() ; ++i)
  {
    constraints[i]->Destroy();
  }

  if (mCollSkin)
    mCollSkin->SetOwner(0);

  // don't care if this fails
  DisableBody();
}


//==============================================================
// SetMass
//==============================================================
void tBody::SetMass(tScalar mass)
{
  mMass = mass;
  mInvMass = SafeInvScalar(mMass);
  SetForceToGravity();
}

//==============================================================
// SetInvMass
//==============================================================
void tBody::SetInvMass(tScalar invMass)
{
  mInvMass = invMass;
  mMass = SafeInvScalar(mInvMass);
  SetForceToGravity();
}

//==============================================================
// SetBodyInertia
//==============================================================
void tBody::SetBodyInertia(const tMatrix33 &bodyInertia)
{
  mBodyInertia = bodyInertia;
  mBodyInvInertia = bodyInertia.GetInverted();
}


//==============================================================
// SetBodyInertia
//==============================================================
void tBody::SetBodyInertia(tScalar Ixx, tScalar Iyy, tScalar Izz)
{
  mBodyInertia.SetTo(SCALAR(0.0f));
  mBodyInertia(0, 0) = Ixx;
  mBodyInertia(1, 1) = Iyy;
  mBodyInertia(2, 2) = Izz;
  
  mBodyInvInertia.SetTo(SCALAR(0.0f));
  mBodyInvInertia(0, 0) = SafeInvScalar(Ixx);
  mBodyInvInertia(1, 1) = SafeInvScalar(Iyy);
  mBodyInvInertia(2, 2) = SafeInvScalar(Izz);
}

//==============================================================
// SetBodyInvInertia
//==============================================================
void tBody::SetBodyInvInertia(tScalar invIxx, 
                              tScalar invIyy, 
                              tScalar invIzz)
{
  mBodyInvInertia.SetTo(SCALAR(0.0f));
  mBodyInvInertia(0, 0) = invIxx;
  mBodyInvInertia(1, 1) = invIyy;
  mBodyInvInertia(2, 2) = invIzz;
  
  mBodyInertia.SetTo(SCALAR(0.0f));
  mBodyInertia(0, 0) = SafeInvScalar(invIxx);
  mBodyInertia(1, 1) = SafeInvScalar(invIyy);
  mBodyInertia(2, 2) = SafeInvScalar(invIzz);
}

//==============================================================
// UpdateVelocity
//==============================================================
void tBody::UpdateVelocity(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  if (mImmovable || !IsActive())
    return;

#ifdef CHECK_RIGID_BODY
  tVector3 origVelocity = mTransformRate.velocity;
  tVector3 origAngVel = mTransformRate.angVelocity;
#endif
  
  mTransformRate.velocity += (dt * mInvMass) * mForce ;
  // don't quite get this - calculating angMom from angVel, then applying torque to that, then
  // converting back just results in the simple equation anyway. The extra term just produces
  // weirdness...
//#define INCLUDE_GYROSCOPIC_TERM
#ifdef INCLUDE_GYROSCOPIC_TERM
  tVector3 angMom = mWorldInertia * mTransformRate.angVelocity;
  mTransformRate.angVelocity += mWorldInvInertia * dt * (mTorque - Cross(mTransformRate.angVelocity, mWorldInertia * mTransformRate.angVelocity));
//  mTransformRate.angVelocity += mWorldInvInertia * (dt * mTorque);
  TRACE("%5.2f %5.2f %5.2f\n", angMom.x, angMom.y, angMom.z);
#else
  mTransformRate.angVelocity += mWorldInvInertia * (dt * mTorque);
#endif

  /// TODO implement rotational friction properly
//  if (mCollSkin && 
//      (mCollSkin->GetCollisions().size() >= 1) &&
//      (Abs(Dot(mCollSkin->GetCollisions()[0]->mDirToBody0, mTransformRate.angVelocity.GetNormalisedSafe())) > SCALAR(0.6f)) )
//    mTransformRate.angVelocity *= SCALAR(0.99f);
  if (mCollSkin && mCollSkin->GetCollisions().size() >= 1)
    mTransformRate.angVelocity *= SCALAR(0.99f);
#ifdef CHECK_RIGID_BODY
  // check the result, and roll-back if needed
  if (!mTransformRate.velocity.IsSensible())
  {
    TRACE("Velocity is not sensible: this = %p\n", this);
    origVelocity.Show("orig vel");
    mForce.Show("force");
    mTransformRate.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
    mTransformRate.velocity = origVelocity;
  }
  
  if (!mTransformRate.angVelocity.IsSensible())
  {
    TRACE("rotation is not sensible: this = %p\n-", this);
    mTransformRate.angVelocity.Show("rotation");
    origAngVel.Show("orig");
    mTorque.Show("torque");
    mWorldInvInertia.Show("inv world inertia");
    while (1) {DummyFnForMSVC();}
    mTransformRate.angVelocity = origAngVel;
  }
#endif
}

//==============================================================
// UpdatePosition
//==============================================================
void tBody::UpdatePosition(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  if (mImmovable || !IsActive())
    return;
  
#ifdef CHECK_RIGID_BODY
  // in case something goes wrong...
  tVector3 origPosition = mTransform.position;
  tMatrix33 origOrientation = mTransform.orientation;
#endif

  tVector3 angMomBefore = mWorldInertia * mTransformRate.angVelocity;
  ApplyTransformRate(mTransform, mTransformRate, dt);

  mInvOrientation = mTransform.orientation.GetTranspose();
  // recalculate the world inertia
  mWorldInvInertia = mTransform.orientation * mBodyInvInertia * mInvOrientation;
  mWorldInertia = mTransform.orientation * mBodyInertia * mInvOrientation;
  
  // conservation of momentum
  mTransformRate.angVelocity = mWorldInvInertia * angMomBefore;

  TRACE_FILE_IF(MULTI_FRAME_1)
    mTransform.position.Show("Body position");
  
#ifdef CHECK_RIGID_BODY
  // check the result, and roll-back if needed
  // hmmm probably due to velocity/ration being screwed, so reset them
  if (!mTransform.position.IsSensible())
  {
    TRACE("Position is not sensible: this = %p", this);
    mTransform.position.Show("position");
    origPosition.Show("orig position");
    mTransformRate.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
    mTransform.position = origPosition;
    mTransform.orientation = origOrientation;
    mTransformRate.velocity.SetTo(SCALAR(0.0f));
    mTransformRate.angVelocity.SetTo(SCALAR(0.0f));
  }
  if (!mTransform.orientation.IsSensible())
  {
    TRACE("Orientation is not sensible: this = %p", this);
    mTransform.orientation.Show("orientation");
    origOrientation.Show("orig orientation");
    mTransformRate.angVelocity.Show("ang vel");
    while (1) {DummyFnForMSVC();}
    mTransform.orientation = origOrientation;
    mTransform.position = origPosition;
    mTransformRate.velocity.SetTo(SCALAR(0.0f));
    mTransformRate.angVelocity.SetTo(SCALAR(0.0f));
  }
#endif
  
  
  tCollisionSkin * collSkin = GetCollisionSkin();
  if ( collSkin )
    collSkin->SetTransform(mOldTransform, mTransform);
}

//==============================================================
// UpdatePositionWithAux
//==============================================================
void tBody::UpdatePositionWithAux(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  if (mImmovable || !IsActive())
  {
    mTransformRateAux.SetToZero();
    return;
  }

#ifdef CHECK_RIGID_BODY
  // in case something goes wrong...
  tVector3 origPosition = mTransform.position;
  tMatrix33 origOrientation = mTransform.orientation;
#endif

  tPhysicsSystem * physics = tPhysicsSystem::GetCurrentPhysicsSystem();
  int ga = physics->GetMainGravityAxis();
  if (ga != -1)
  {
    mTransformRateAux.velocity[(ga+1)%3] *= 0.1f;
    mTransformRateAux.velocity[(ga+2)%3] *= 0.1f;
  }
  tVector3 angMomBefore = mWorldInertia * mTransformRate.angVelocity;
  ApplyTransformRate(mTransform, mTransformRate + mTransformRateAux, dt);
  mTransformRateAux.SetToZero();

  mInvOrientation = mTransform.orientation.GetTranspose();
  // recalculate the world inertia
  mWorldInvInertia = mTransform.orientation * mBodyInvInertia * mInvOrientation;
  mWorldInertia = mTransform.orientation * mBodyInertia * mInvOrientation;
  
  // conservation of momentum
  mTransformRate.angVelocity = mWorldInvInertia * angMomBefore;

  TRACE_FILE_IF(MULTI_FRAME_1)
    mTransform.position.Show("Body position");
  
#ifdef CHECK_RIGID_BODY
  // check the result, and roll-back if needed
  // hmmm probably due to velocity/rotation being screwed, so reset them
  if (!mTransform.position.IsSensible())
  {
    TRACE("Position is not sensible: this = %p\n", this);
    mTransform.position.Show("position");
    origPosition.Show("orig position");
    mTransformRate.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
    mTransform.position = origPosition;
    mTransform.orientation = origOrientation;
    mTransformRate.velocity.SetTo(SCALAR(0.0f));
    mTransformRate.angVelocity.SetTo(SCALAR(0.0f));
  }
  if (!mTransform.orientation.IsSensible())
  {
    TRACE("Orientation is not sensible: this = %p\n", this);
    mTransform.orientation.Show("orientation");
    origOrientation.Show("orig orientation");
    mTransformRate.angVelocity.Show("ang vel");
    while (1) {DummyFnForMSVC();}
    mTransform.orientation = origOrientation;
    mTransform.position = origPosition;
    mTransformRate.velocity.SetTo(SCALAR(0.0f));
    mTransformRate.angVelocity.SetTo(SCALAR(0.0f));
  }
#endif
  
  
  tCollisionSkin * collSkin = GetCollisionSkin();
  if ( collSkin )
    collSkin->SetTransform(mOldTransform, mTransform);
}


//==============================================================
// StateToStr
//==============================================================
const char * StateToStr(tBody::tActivity state)
{
  switch (state)
  {
  case tBody::ACTIVE: return "ACTIVE";
  case tBody::INACTIVE: return "INACTIVE";
  }
  return "Invalid state";
}

//==============================================================
// SetDeactivationTime
//==============================================================
void tBody::SetDeactivationTime(tScalar seconds) 
{
  mDeactivationTime = seconds;
}

//==============================================================
// SetActivityThreshold
//==============================================================
void tBody::SetActivityThreshold(tScalar vel, tScalar angVel)
{
  mSqVelocityActivityThreshold = Sq(vel);
  mSqAngVelActivityThreshold = Sq(Deg2Rad(angVel));
}

//==============================================================
// SetDeactivationThreshold
//==============================================================
void tBody::SetDeactivationThreshold(tScalar posThreshold,
                                     tScalar orientThreshold)
{
  mSqDeltaPosThreshold = Sq(posThreshold);
  mSqDeltaOrientThreshold = Sq(orientThreshold);
}

//==============================================================
// SetActive
//==============================================================
void tBody::SetActive(tScalar activityFactor)
{
  TRACE_METHOD_ONLY(FRAME_2);
  tPhysicsSystem * physics = tPhysicsSystem::GetCurrentPhysicsSystem();
  static bool recursing = false;
  if (physics && !recursing)
  {
    recursing = true;
    physics->ActivateObject(this);
    recursing = false;
  }
  mActivity = ACTIVE;
  mInactiveTime = (SCALAR(1.0f) - activityFactor) * mDeactivationTime;
}

//==============================================================
// SetInactive
//==============================================================
void tBody::SetInactive()
{
  if (mAllowFreezing && tPhysicsSystem::GetCurrentPhysicsSystem()->IsFreezingEnabled())
    mActivity = INACTIVE;
}

//==============================================================
// DampForDeactivation
//==============================================================
void tBody::DampForDeactivation()
{
  tScalar frac = mInactiveTime / mDeactivationTime;
  // r = 1 means don't ever damp
  // r = 0.5 means start to damp when half way
  const tScalar r = 0.5f;
  if (frac < r) return;

  tScalar scale = SCALAR(1.0f) - ((frac - r) / (SCALAR(1.0f) - r));
  Limit(scale, SCALAR(0.0f), SCALAR(1.0f));
  mTransformRate.velocity *= scale;
  mTransformRate.angVelocity *= scale;
}

//==============================================================
// SetAllowFreezing
//==============================================================
void tBody::SetAllowFreezing(bool allow)
{
  mAllowFreezing = allow;
  if (allow == false)
    SetActive();
}

//==============================================================
// MoveTo
//==============================================================
void tBody::MoveTo(const tVector3 & pos, const tMatrix33 & orientation)
{
  TRACE_METHOD_ONLY(FRAME_2);
  if (mBodyEnabled && !IsActive())
  {
    Assert(tPhysicsSystem::GetCurrentPhysicsSystem());
    tPhysicsSystem::GetCurrentPhysicsSystem()->ActivateObject(this);
  }
  SetPosition(pos);
  SetOrientation(orientation);
  SetVelocity(tVector3(SCALAR(0.0f)));
  SetAngVel(tVector3(SCALAR(0.0f)));
  CopyCurrentStateToOld();
  tCollisionSkin * collSkin = GetCollisionSkin();
  if ( collSkin )
    collSkin->SetTransform(mOldTransform, mTransform);
}

//==============================================================
// Enable
//==============================================================
void tBody::EnableBody()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (true == mBodyEnabled) return;
  mBodyEnabled = true;
  tPhysicsSystem::GetCurrentPhysicsSystem()->AddBody(this);
}

//==============================================================
// Disable
//==============================================================
void tBody::DisableBody()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (false == mBodyEnabled) return;
  mBodyEnabled = false;
  tPhysicsSystem::GetCurrentPhysicsSystem()->RemoveBody(this);
}

//==============================================================
// AddConstraint
//==============================================================
void tBody::AddConstraint(class tConstraint * constraint)
{
  if (mConstraints.end() == find(mConstraints.begin(), mConstraints.end(), constraint))
    mConstraints.push_back(constraint);
}

//==============================================================
// RemoveConstraint
//==============================================================
void tBody::RemoveConstraint(class tConstraint * constraint)
{
  std::vector<class tConstraint *>::iterator it = 
    find(mConstraints.begin(), mConstraints.end(), constraint);
  if (mConstraints.end() != it)
    mConstraints.erase(it);
}

//==============================================================
// SetImmovable
//==============================================================
void tBody::SetImmovable(bool immovable)
{
  mImmovable = immovable; 
  mOrigImmovable = mImmovable;
  SetInvMass(SCALAR(0.0f)); 
  SetBodyInvInertia(SCALAR(0.0f), SCALAR(0.0f), SCALAR(0.0f));
}

//==============================================================
// DummyFnForMSVC
//==============================================================
void JigLib::DummyFnForMSVC()
{
  // this is just so we can break on the inline functions in body.inl!
}
