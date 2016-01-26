//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file body.inl 
//                     
//==============================================================
// only to be included by body.hpp

//==============================================================
// LimitVel
//==============================================================
inline void tBody::LimitVel()
{
  Limit(mTransformRate.velocity.x, -mVelMax, mVelMax);
  Limit(mTransformRate.velocity.y, -mVelMax, mVelMax);
  Limit(mTransformRate.velocity.z, -mVelMax, mVelMax);
}

//==============================================================
// LimitAngVel
//==============================================================
inline void tBody::LimitAngVel()
{
  tScalar fX = Abs(mTransformRate.angVelocity.x) / mAngVelMax;
  tScalar fY = Abs(mTransformRate.angVelocity.y) / mAngVelMax;
  tScalar fZ = Abs(mTransformRate.angVelocity.z) / mAngVelMax;
  tScalar f = Max(fX, fY, fZ);
  if (f > 1.0f)
    mTransformRate.angVelocity /= f;
}

//==============================================================
// GetVelocity
//==============================================================
inline tVector3 tBody::GetVelocity(const tVector3& relPos) const
{
  return tVector3(
    mTransformRate.velocity[0] + mTransformRate.angVelocity[1]*relPos[2] - mTransformRate.angVelocity[2]*relPos[1],
    mTransformRate.velocity[1] + mTransformRate.angVelocity[2]*relPos[0] - mTransformRate.angVelocity[0]*relPos[2],
    mTransformRate.velocity[2] + mTransformRate.angVelocity[0]*relPos[1] - mTransformRate.angVelocity[1]*relPos[0]);
}

//==============================================================
// GetVelocityAux
//==============================================================
inline tVector3 tBody::GetVelocityAux(const tVector3& relPos) const
{
  return tVector3(
    mTransformRateAux.velocity[0] + mTransformRateAux.angVelocity[1]*relPos[2] - mTransformRateAux.angVelocity[2]*relPos[1],
    mTransformRateAux.velocity[1] + mTransformRateAux.angVelocity[2]*relPos[0] - mTransformRateAux.angVelocity[0]*relPos[2],
    mTransformRateAux.velocity[2] + mTransformRateAux.angVelocity[0]*relPos[1] - mTransformRateAux.angVelocity[1]*relPos[0]);
}


//==============================================================
// DoMovementActivations
//==============================================================
inline void tBody::DoMovementActivations()
{
  TRACE_METHOD_ONLY(FRAME_2);
  Assert(tPhysicsSystem::GetCurrentPhysicsSystem());
  if (mBodiesToBeActivatedOnMovement.empty())
    return;
  if ( (mTransform.position - mStoredPositionForActivation).GetLengthSq() < mSqDeltaPosThreshold)
    return;
  
  const size_t numBodies = mBodiesToBeActivatedOnMovement.size();
  for (size_t i = 0 ; i < numBodies ; ++i)
    tPhysicsSystem::GetCurrentPhysicsSystem()->ActivateObject(mBodiesToBeActivatedOnMovement[i]);
  mBodiesToBeActivatedOnMovement.resize(0);
}

//==============================================================
// add_movement_activation
//==============================================================
inline void tBody::AddMovementActivation(const tVector3 & pos, 
                                         tBody * otherBody)
{
  const size_t numBodies = mBodiesToBeActivatedOnMovement.size();
  size_t i;
  for (i = 0 ; i < numBodies ; ++i)
  {
    if (mBodiesToBeActivatedOnMovement[i] == otherBody)
      return; // already made a note of this body
  }
  if (numBodies == 0)
    mStoredPositionForActivation = pos;
  mBodiesToBeActivatedOnMovement.push_back(otherBody);
}

//==============================================================
// TryToFreeze
//==============================================================
inline void tBody::TryToFreeze(tScalar dt)
{
  if (!mAllowFreezing || mImmovable || !IsActive())
    return;
  
  if ((mTransform.position - mLastPositionForDeactivation).GetLengthSq() > 
      mSqDeltaPosThreshold)
  {
    mLastPositionForDeactivation = mTransform.position;
    mInactiveTime = 0.0f;
    return;
  }
// ugly - use quaternions
  tMatrix33 deltaMat = mTransform.orientation - mLastOrientationForDeactivation;
  if ( (deltaMat.GetLook().GetLengthSq() > mSqDeltaOrientThreshold ) ||
       (deltaMat.GetLeft().GetLengthSq() > mSqDeltaOrientThreshold ) ||
       (deltaMat.GetUp().GetLengthSq()   > mSqDeltaOrientThreshold ) )
  {
    mLastOrientationForDeactivation = mTransform.orientation;
    mInactiveTime = 0.0f;
    return;
  }

// check the thresholds as well
  if ( GetShouldBeActive() )
  {
    // let the inactivity timer continue
    return;
  }
  
  mInactiveTime += dt;
  
  if (mInactiveTime > mDeactivationTime)
  {
// sleep!
    mLastOrientationForDeactivation = mTransform.orientation;
    mLastPositionForDeactivation = mTransform.position;
    SetInactive();
  }
}

//==============================================================
// set_orientation
//==============================================================
inline void tBody::SetOrientation(const tMatrix33 & orient) 
{ 
  mTransform.orientation = orient; 
  mInvOrientation = mTransform.orientation.GetTranspose();
  mWorldInvInertia = mTransform.orientation * mBodyInvInertia * mInvOrientation;
  mWorldInertia = mTransform.orientation * mBodyInertia * mInvOrientation;
}

#ifdef DEBUG
#define CHECK_RIGID_BODY
#endif

void DummyFnForMSVC();

//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void tBody::ApplyWorldImpulse(const tVector3 & impulse)
{
  if (mImmovable)
    return;
#ifdef CHECK_RIGID_BODY
  tVector3 origVelocity = mTransformRate.velocity;
#endif
  AddScaleVector3(mTransformRate.velocity, mTransformRate.velocity, mInvMass, impulse);
  mVelChanged = true;
#ifdef CHECK_RIGID_BODY
  if (!mTransformRate.velocity.IsSensible())
  {
    TRACE("tVector3 is not sensible after impulse: this = %p\n", this);
    origVelocity.Show("orig vel");
    impulse.Show("impulse");
    mTransformRate.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
  }
#endif
}

//==============================================================
// ApplyNegativeWorldImpulse
//==============================================================
inline void tBody::ApplyNegativeWorldImpulse(const tVector3 & impulse)
{
#ifdef CHECK_RIGID_BODY
  ApplyWorldImpulse(-impulse);
#else
  if (mImmovable)
    return;
  AddScaleVector3(mTransformRate.velocity, mTransformRate.velocity, mInvMass, impulse);
  mVelChanged = true;
#endif
}

//==============================================================
// ApplyWorldImpulseAux
//==============================================================
inline void tBody::ApplyWorldImpulseAux(const tVector3 & impulse)
{
  if (mImmovable)
    return;
#ifdef CHECK_RIGID_BODY
  tVector3 origVelocity = mTransformRateAux.velocity;
#endif
  AddScaleVector3(mTransformRateAux.velocity, mTransformRateAux.velocity, mInvMass, impulse);
  mVelChanged = true;
#ifdef CHECK_RIGID_BODY
  if (!mTransformRateAux.velocity.IsSensible())
  {
    TRACE("tVector3 is not sensible after aux impulse: this = %p\n", this);
    origVelocity.Show("orig vel");
    impulse.Show("impulse");
    mTransformRateAux.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
  }
#endif
}

//==============================================================
// ApplyNegativeWorldImpulseAux
//==============================================================
inline void tBody::ApplyNegativeWorldImpulseAux(const tVector3 & impulse)
{
#ifdef CHECK_RIGID_BODY
  ApplyWorldImpulseAux(-impulse);
#else
  if (mImmovable)
    return;
  AddScaleVector3(mTransformRateAux.velocity, mTransformRateAux.velocity, mInvMass, impulse);
  mVelChanged = true;
#endif
}

//====================================================================
// ApplyBodyWorldImpulse
//====================================================================
inline void tBody::ApplyBodyWorldImpulse(const tVector3 & impulse, const tVector3 & delta)
{
  if (mImmovable)
    return;
#ifdef CHECK_RIGID_BODY
  tVector3 origVelocity = mTransformRate.velocity;
  tVector3 origAngVel = mTransformRate.angVelocity;
#endif
  AddScaleVector3(mTransformRate.velocity, mTransformRate.velocity, mInvMass, impulse);
  mTransformRate.angVelocity += mWorldInvInertia * Cross(delta, impulse);

  mVelChanged = true;
#ifdef CHECK_RIGID_BODY
  if (!mTransformRate.angVelocity.IsSensible())
  {
    TRACE("rotation is not sensible after impulse: this = %p\n", this);
    origAngVel.Show("orig vel");
    impulse.Show("impulse");
    mTransformRate.angVelocity.Show("rotation");
    while (1) {DummyFnForMSVC();}
  }
  if (!mTransformRate.velocity.IsSensible())
  {
    TRACE("mTransformRate.velocity is not sensible after impulse: this = %p\n", this);
    origVelocity.Show("orig vel");
    impulse.Show("impulse");
    mTransformRate.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
  }
#endif
}

//==============================================================
// ApplyNegativeBodyWorldImpulse
//==============================================================
inline void tBody::ApplyNegativeBodyWorldImpulse(const tVector3 & impulse, const tVector3 & delta)
{
#ifdef CHECK_RIGID_BODY
  ApplyBodyWorldImpulse(-impulse, delta);
#else
  if (mImmovable)
    return;
  AddScaleVector3(mTransformRate.velocity, mTransformRate.velocity, -mInvMass, impulse);
  mTransformRate.angVelocity -= mWorldInvInertia * Cross(delta, impulse);
  mVelChanged = true;
#endif
}
//====================================================================
// ApplyBodyWorldImpulseAux
//====================================================================
inline void tBody::ApplyBodyWorldImpulseAux(const tVector3 & impulse, const tVector3 & delta)
{
  if (mImmovable)
    return;
#ifdef CHECK_RIGID_BODY
  tVector3 origVelocity = mTransformRateAux.velocity;
  tVector3 origAngVel = mTransformRateAux.angVelocity;
#endif
  AddScaleVector3(mTransformRateAux.velocity, mTransformRateAux.velocity, mInvMass, impulse);
  mTransformRateAux.angVelocity += mWorldInvInertia * Cross(delta, impulse);
  /// todo flag vel changed?
  mVelChanged = true;
#ifdef CHECK_RIGID_BODY
  if (!mTransformRateAux.angVelocity.IsSensible())
  {
    TRACE("rotation is not sensible after aux impulse: this = %p\n", this);
    origAngVel.Show("orig vel");
    impulse.Show("impulse");
    mTransformRateAux.angVelocity.Show("rotation");
    while (1) {DummyFnForMSVC();}
  }
  if (!mTransformRateAux.velocity.IsSensible())
  {
    TRACE("mTransformRateAux.velocity is not sensible after impulse: this = %p\n", this);
    origVelocity.Show("orig vel");
    impulse.Show("impulse");
    mTransformRateAux.velocity.Show("velocity");
    while (1) {DummyFnForMSVC();}
  }
#endif
}

//==============================================================
// ApplyNegativeBodyWorldImpulseAux
//==============================================================
inline void tBody::ApplyNegativeBodyWorldImpulseAux(const tVector3 & impulse, const tVector3 & delta)
{
#ifdef CHECK_RIGID_BODY
  ApplyBodyWorldImpulseAux(-impulse, delta);
#else
  if (mImmovable)
    return;
  AddScaleVector3(mTransformRateAux.velocity, mTransformRateAux.velocity, -mInvMass, impulse);
  mTransformRateAux.angVelocity -= mWorldInvInertia * Cross(delta, impulse);
  /// todo falg vel changed when it's aux?
  mVelChanged = true;
#endif
}

//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void tBody::ApplyWorldImpulse(const tVector3 & impulse, 
                                     const tVector3 & pos)
{
  ApplyBodyWorldImpulse(impulse, pos - mTransform.position);
}

//==============================================================
// ApplyNegativeWorldImpulse
//==============================================================
inline void tBody::ApplyNegativeWorldImpulse(const tVector3 & impulse, 
                                             const tVector3 & pos)
{
  ApplyNegativeBodyWorldImpulse(impulse, pos - mTransform.position);
}

//==============================================================
// ApplyWorldImpulseAux
//==============================================================
inline void tBody::ApplyWorldImpulseAux(const tVector3 & impulse, 
                                        const tVector3 & pos)
{
  ApplyBodyWorldImpulseAux(impulse, pos - mTransform.position);
}

//==============================================================
// ApplyNegativeWorldImpulseAux
//==============================================================
inline void tBody::ApplyNegativeWorldImpulseAux(const tVector3 & impulse, 
                                                const tVector3 & pos)
{
  ApplyNegativeBodyWorldImpulseAux(impulse, pos - mTransform.position);
}

//==============================================================
// apply_world_ang_impulse
//==============================================================
inline void tBody::ApplyWorldAngImpulse(const tVector3 & angImpulse)
{
  if (mImmovable) return;
#ifdef CHECK_RIGID_BODY
  tVector3 origAngVel = mTransformRate.angVelocity;
#endif
  mTransformRate.angVelocity += mWorldInvInertia * angImpulse;

  mVelChanged = true;
#ifdef CHECK_RIGID_BODY
  if (!mTransformRate.angVelocity.IsSensible())
  {
    TRACE("rotation is not sensible after ang impulse: this = %p\n", this);
    origAngVel.Show("orig ang vel");
    angImpulse.Show("ang impulse");
    mTransformRate.angVelocity.Show("rotation");
    while (1) {DummyFnForMSVC();}
  }
#endif
}

//==============================================================
// ApplyBodyImpulse
//==============================================================
inline void tBody::ApplyBodyImpulse(const tVector3 & impulse)
{
  ApplyWorldImpulse(mTransform.orientation * impulse);
}

//==============================================================
// ApplyNegativeBodyImpulse
//==============================================================
inline void tBody::ApplyNegativeBodyImpulse(const tVector3 & impulse)
{
  ApplyNegativeWorldImpulse(mTransform.orientation * impulse);
}

//==============================================================
// ApplyBodyImpulse
//==============================================================
inline void tBody::ApplyBodyImpulse(const tVector3 & impulse, 
                                    const tVector3 & pos)
{
  ApplyWorldImpulse(mTransform.orientation * impulse, mTransform.position + mTransform.orientation * pos);
}

//==============================================================
// ApplyNegativeBodyImpulse
//==============================================================
inline void tBody::ApplyNegativeBodyImpulse(const tVector3 & impulse, 
                                            const tVector3 & pos)
{
  ApplyNegativeWorldImpulse(mTransform.orientation * impulse, mTransform.position + mTransform.orientation * pos);
}
//==============================================================
// ApplyBodyAngImpulse
//==============================================================
inline void tBody::ApplyBodyAngImpulse(const tVector3 & angImpulse)
{
  ApplyWorldAngImpulse(mTransform.orientation * angImpulse);
}

//========================================================
// AddWorldForce
//========================================================
inline void tBody::AddWorldForce(const tVector3 & force) 
{
  if (mImmovable) return;
  mForce += force;
  mVelChanged = true;
}

//==============================================================
// AddWorldForce
//==============================================================
inline void tBody::AddWorldForce(const tVector3 & force, 
                                 const tVector3 & pos)
{
  if (mImmovable) return;
  mForce += force ;
  mTorque += Cross(pos - mTransform.position, force);
  mVelChanged = true;
}

//========================================================
// AddWorldTorque
//========================================================
inline void tBody::AddWorldTorque(const tVector3 & torque) 
{
  if (mImmovable) return;
  mTorque += torque;
  mVelChanged = true;
}

//==============================================================
// AddBodyForce
//==============================================================
inline void tBody::AddBodyForce(const tVector3 & force)
{
  AddWorldForce(mTransform.orientation * force);
}

//==============================================================
// AddBodyForce
//==============================================================
inline void tBody::AddBodyForce(const tVector3 & force, const tVector3 & pos)
{
  AddWorldForce(mTransform.orientation * force, mTransform.position + mTransform.orientation * pos);
}

//==============================================================
// AddBodyTorque
//==============================================================
inline void tBody::AddBodyTorque(const tVector3 & torque)
{
  AddWorldTorque(mTransform.orientation * torque);
}

//==============================================================
// ClearForces
//==============================================================
inline void tBody::ClearForces()
{
  mForce.SetTo(0.0f);
  mTorque.SetTo(0.0f);
}

//==============================================================
// SetForceToGravity
//==============================================================
inline void tBody::SetForceToGravity()
{
  if (tPhysicsSystem::GetCurrentPhysicsSystem())
    mForce = mMass * tPhysicsSystem::GetCurrentPhysicsSystem()->GetGravity();
  else
    mForce.SetTo(0.0f);
}

//========================================================
// AddExternalForces
//========================================================
inline void tBody::AddExternalForces(tScalar dt)
{
  mTorque.SetTo(0.0f);
  SetForceToGravity();
}


//==============================================================
// CopyCurrentStateToOld
//==============================================================
inline void tBody::CopyCurrentStateToOld()
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  mOldTransform = mTransform;
  mOldTransformRate = mTransformRate;
}

//==============================================================
// SetConstraintsUnsatisfied
//==============================================================
inline void tBody::SetConstraintsAndCollisionsUnsatisfied()
{
  for (size_t iConstraint = mConstraints.size() ; iConstraint-- != 0; )
    mConstraints[iConstraint]->SetUnsatisfied();

  if (mCollSkin)
  {
    std::vector<tCollisionInfo *> & colls = mCollSkin->GetCollisions();
    for (size_t iColl = colls.size() ; iColl-- != 0 ; )
      colls[iColl]->mSatisfied = false;
  }
}

//========================================================
// StoreState
//========================================================
inline void tBody::StoreState()
{
  mStoredTransform = mTransform;
  mStoredTransformRate = mTransformRate;
}

//========================================================
// RestoreState
//========================================================
inline void tBody::RestoreState()
{
  mTransform = mStoredTransform;
  mTransformRate = mStoredTransformRate;

  mInvOrientation = mTransform.orientation.GetTranspose();
  // recalculate the world inertia
  mWorldInvInertia = mTransform.orientation * mBodyInvInertia * mInvOrientation;
  mWorldInertia = mTransform.orientation * mBodyInertia * mInvOrientation;
}
