//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintpoint.cpp 
//                     
//==============================================================
#include "constraintpoint.hpp"
#include "body.hpp"

using namespace JigLib;
using namespace std;

static const tScalar mMaxVelMag = 20.0f;
static const tScalar minVelForProcessing = 0.01f;

//==============================================================
// tConstraintPoint
//==============================================================
tConstraintPoint::tConstraintPoint(
  tBody * body0, const tVector3 & body0Pos,
  tBody * body1, const tVector3 & body1Pos,
  tScalar allowedDistance,
  tScalar timescale)
{
  TRACE_FILE_IF(ONCE_2)
    TRACE("Creating body-body point constraint\n");
  Initialise(body0, body0Pos, body1, body1Pos,
             allowedDistance, timescale);
}

//==============================================================
// tConstraintPoint
//==============================================================
tConstraintPoint::tConstraintPoint()
  :
  mBody0(0), mBody1(0)
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// tConstraintPoint
//==============================================================
tConstraintPoint::~tConstraintPoint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (mBody0) mBody0->RemoveConstraint(this);
  if (mBody1) mBody1->RemoveConstraint(this);
  Destroy();
}

//==============================================================
// initialise
//==============================================================
void tConstraintPoint::Initialise(tBody * body0, const tVector3 & body0Pos,
                                  tBody * body1, const tVector3 & body1Pos,
                                  tScalar allowedDistance,
                                  tScalar timescale)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0Pos = body0Pos;
  mBody1Pos = body1Pos;
  mBody0 = body0;
  mBody1 = body1;
  mAllowedDistance = allowedDistance;
  mTimescale = timescale;
  if (mTimescale < SCALAR_TINY)
    mTimescale = SCALAR_TINY;
  if (mBody0) mBody0->AddConstraint(this);
  if (mBody1) mBody1->AddConstraint(this);
}

//==============================================================
// Destroy
//==============================================================
void tConstraintPoint::Destroy()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0 = mBody1 = 0;
  DisableConstraint();
}

//==============================================================
// PreApply
//==============================================================
void tConstraintPoint::PreApply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  SetUnsatisfied();
  
  MultMatrix33(mR0, mBody0->GetOrientation(), mBody0Pos);
  MultMatrix33(mR1, mBody1->GetOrientation(), mBody1Pos);
  const tVector3 worldPos0(mBody0->GetPosition() + mR0);
  const tVector3 worldPos1(mBody1->GetPosition() + mR1);
  mWorldPos = 0.5f * (worldPos0 + worldPos1);

  // add a "correction" based on the deviation of point 0
  const tVector3 deviation(worldPos0 - worldPos1);
  tScalar deviationAmount = deviation.GetLength();
  if (deviationAmount > mAllowedDistance)
  {
    mVrExtra = ((deviationAmount - mAllowedDistance) / 
                (deviationAmount * Max(mTimescale, dt))) * deviation;
  }
  else
  {
    mVrExtra.SetTo(0.0f);
  }
}

//==============================================================
// Apply
//==============================================================
bool tConstraintPoint::Apply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  SetSatisfied();

  bool body0FrozenPre = !mBody0->IsActive();
  bool body1FrozenPre = !mBody1->IsActive();
//  if (body0FrozenPre && body1FrozenPre)
//    return false;

  const tVector3 currentVel0(mBody0->GetVelocity() + 
                             Cross(mBody0->GetAngVel(), mR0));
  const tVector3 currentVel1(mBody1->GetVelocity() + 
                             Cross(mBody1->GetAngVel(), mR1));

  // add a "correction" based on the deviation of point 0
  tVector3 Vr(mVrExtra + currentVel0 - currentVel1);

  tScalar normalVel = Vr.GetLength();
  if (normalVel < minVelForProcessing)
    return false;

  // limit things
  if (normalVel > mMaxVelMag)
  {
    Vr *= mMaxVelMag / normalVel;
    normalVel = mMaxVelMag;
  }

  const tVector3 N = Vr / normalVel;

  const tScalar numerator = -normalVel;
  const tScalar denominator = mBody0->GetInvMass() + mBody1->GetInvMass() + 
    Dot(N, Cross(mBody0->GetWorldInvInertia() * (Cross(mR0, N)), mR0)) + 
    Dot(N, Cross(mBody1->GetWorldInvInertia() * (Cross(mR1, N)), mR1));

  if (denominator < SCALAR_TINY)
    return false;

  const tVector3 normalImpulse = (numerator / denominator) * N;

  if (!mBody0->GetImmovable())
    mBody0->ApplyWorldImpulse(normalImpulse, mWorldPos);
  if (!mBody1->GetImmovable())
    mBody1->ApplyWorldImpulse(-normalImpulse, mWorldPos);

  mBody0->SetConstraintsAndCollisionsUnsatisfied();
  mBody1->SetConstraintsAndCollisionsUnsatisfied();
  SetSatisfied();

  return true;
}
