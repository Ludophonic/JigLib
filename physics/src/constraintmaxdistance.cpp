//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintmaxdistance.hpp 
//                     
//==============================================================
#include "constraintmaxdistance.hpp"
#include "body.hpp"

using namespace JigLib;
using namespace std;

// maximum relative velocity induced at the constraint points - proportional
// to the error
static const tScalar maxVelMag = 20.0f;
static const tScalar minVelForProcessing = 0.01f;

//==============================================================
// tConstraintMaxDistance
//==============================================================
tConstraintMaxDistance::tConstraintMaxDistance(
  tBody * body0, const tVector3 & body0Pos,
  tBody * body1, const tVector3 & body1Pos,
  tScalar maxDistance)
{
  TRACE_FILE_IF(ONCE_2)
    TRACE("Creating body-body max distance constraint\n");
  Initialise(body0, body0Pos, body1, body1Pos, maxDistance);
}

//==============================================================
// tConstraintMaxDistance
//==============================================================
tConstraintMaxDistance::tConstraintMaxDistance()
  :
  mBody0(0),
  mBody1(0)
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// tConstraintMaxDistance
//==============================================================
tConstraintMaxDistance::~tConstraintMaxDistance()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (mBody0) mBody0->RemoveConstraint(this);
  if (mBody1) mBody1->RemoveConstraint(this);
  Destroy();
}

//==============================================================
// Destroy
//==============================================================
void tConstraintMaxDistance::Destroy()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0 = mBody1 = 0;
  DisableConstraint();
}

//==============================================================
// initialise
//==============================================================
void tConstraintMaxDistance::Initialise(
  tBody * body0, const tVector3 & body0Pos,
  tBody * body1, const tVector3 & body1Pos,
  tScalar maxDistance)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0Pos = body0Pos;
  mBody1Pos = body1Pos;
  mBody0 = body0;
  mBody1 = body1;
  mMaxDistance = maxDistance;
  if (mBody0) mBody0->AddConstraint(this);
  if (mBody1) mBody1->AddConstraint(this);
}

//==============================================================
// preApply
//==============================================================
void tConstraintMaxDistance::PreApply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  SetUnsatisfied();
  
  MultMatrix33(mR0, mBody0->GetOrientation(), mBody0Pos);
  MultMatrix33(mR1, mBody1->GetOrientation(), mBody1Pos);

  const tVector3 worldPos0(mBody0->GetPosition() + mR0);
  const tVector3 worldPos1(mBody1->GetPosition() + mR1);
  mWorldPos = 0.5f * (worldPos0 + worldPos1);

  // current location of point 0 relative to point 1
  SubVector3(mCurrentRelPos0, worldPos0, worldPos1);
}

//==============================================================
// apply
//==============================================================
bool tConstraintMaxDistance::Apply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  SetSatisfied();

  bool body0FrozenPre = !mBody0->IsActive();
  bool body1FrozenPre = !mBody1->IsActive();
  if (body0FrozenPre && body1FrozenPre)
    return false;

  const tVector3 currentVel0(
    mBody0->GetVelocity() + Cross(mBody0->GetAngVel(), mR0));
  const tVector3 currentVel1(
    mBody1->GetVelocity() + Cross(mBody1->GetAngVel(), mR1));

  // predict a new location
  tVector3 predRelPos0(mCurrentRelPos0 + (currentVel0 - currentVel1) * dt);

  // if the new position is out of range then clamp it
  tVector3 clampedRelPos0 = predRelPos0;
  tScalar clampedRelPos0Mag = clampedRelPos0.GetLength();
  if (clampedRelPos0Mag <= SCALAR_TINY)
    return false;
  if (clampedRelPos0Mag > mMaxDistance)
    clampedRelPos0 *= mMaxDistance / clampedRelPos0Mag;

  // now claculate desired vel based on the current pos, new/clamped
  // pos and dt
  tVector3 desiredRelVel0((clampedRelPos0 - mCurrentRelPos0) / Max(dt, SCALAR_TINY));

  // Vr is -ve the total velocity change
  tVector3 Vr((currentVel0 - currentVel1) - desiredRelVel0);

  tScalar normalVel = Vr.GetLength();

  // limit it
  if (normalVel > maxVelMag)
  {
    Vr *= (maxVelMag / normalVel);
    normalVel = maxVelMag;
  }
  else if (normalVel < minVelForProcessing)
  {
    return false;
  }

  const tVector3 N = Vr / normalVel;

  tScalar denominator = mBody0->GetInvMass() + mBody1->GetInvMass() + 
    Dot(N, Cross(mBody0->GetWorldInvInertia() * (Cross(mR0, N)), mR0)) + 
    Dot(N, Cross(mBody1->GetWorldInvInertia() * (Cross(mR1, N)), mR1));

  if (denominator < SCALAR_TINY)
    return false;

  tScalar normalImpulse = -normalVel / denominator;

  if (!mBody0->GetImmovable())
    mBody0->ApplyWorldImpulse(normalImpulse * N, mWorldPos);
  if (!mBody1->GetImmovable())
    mBody1->ApplyWorldImpulse(-normalImpulse * N, mWorldPos);

  mBody0->SetConstraintsAndCollisionsUnsatisfied();
  mBody1->SetConstraintsAndCollisionsUnsatisfied();
  SetSatisfied();

  return true;
}
