//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintworldpoint.cpp 
//                     
//==============================================================
#include "constraintworldpoint.hpp"
#include "body.hpp"

using namespace JigLib;

static const tScalar minVelForProcessing = 0.001f;

//==============================================================
// tConstraintWorldPoint
//==============================================================
tConstraintWorldPoint::tConstraintWorldPoint(tBody * body,
                                             const tVector3 & pointOnBody,
                                             const tVector3 & worldPosition)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = 0;
  Initialise(body, pointOnBody, worldPosition);
}

//==============================================================
// tConstraintWorldPoint
//==============================================================
tConstraintWorldPoint::tConstraintWorldPoint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = 0;
  Initialise(0, tVector3(0, 0, 0), tVector3(0, 0, 0));
}

//==============================================================
// tConstraintWorldPoint
//==============================================================
tConstraintWorldPoint::~tConstraintWorldPoint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (mBody) mBody->RemoveConstraint(this);
}

//==============================================================
// Initiliase
//==============================================================
void tConstraintWorldPoint::Initialise(tBody * body,
                                       const tVector3 & pointOnBody,
                                       const tVector3 & worldPosition)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = body;
  mPointOnBody = pointOnBody;
  mWorldPosition = worldPosition;
  if (mBody) mBody->AddConstraint(this);
}

//==============================================================
// Destroy
//==============================================================
void tConstraintWorldPoint::Destroy()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = 0;
  DisableConstraint();
}

//==============================================================
// Apply
//==============================================================
bool tConstraintWorldPoint::Apply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  SetSatisfied();
  
  const tVector3 worldPos = 
    mBody->GetPosition() + mBody->GetOrientation() * mPointOnBody;
  const tVector3 R = worldPos - mBody->GetPosition();
  const tVector3 currentVel = 
    mBody->GetVelocity() + Cross(mBody->GetAngVel(), R);

  // add an extra term to get us back to the original position
  tVector3 desiredVel;

  const tScalar allowedDeviation = 0.01f;
  const tScalar timescale = 4.0f * dt;
  tVector3 deviation = worldPos - mWorldPosition;
  tScalar deviationDistance = deviation.GetLength();
  if (deviationDistance > allowedDeviation)
  {
    tVector3 deviationDir = deviation / deviationDistance;
    desiredVel = ((allowedDeviation - deviationDistance) / timescale) * 
      deviationDir;
  }
  else
  {
    desiredVel.SetTo(0.0f);
  }

  // stop velocities pushing us through geometry
  if (mBody->GetCollisionSkin())
  {
    const std::vector<class tCollisionInfo *>& collisions = mBody->GetCollisionSkin()->GetCollisions();
    unsigned num = collisions.size();
    for (unsigned i = 0 ; i < num ; ++i)
    {
      const tCollisionInfo* collInfo = collisions[i];
      if (collInfo->mSkinInfo.skin1->GetOwner() == 0)
      {
        const tVector3 dir = collInfo->mDirToBody0.GetNormalisedSafe();
        tScalar dot = Dot(desiredVel, dir);
        if (dot < 0.0f)
          desiredVel -= dot * dir;
      }
    }
  }

  // need an impulse to take us from the current vel to the desired vel
  tVector3 N = currentVel - desiredVel;
  tScalar normalVel = N.GetLength();
  if (normalVel < minVelForProcessing)
    return false;

  N /= normalVel;

  tScalar denominator = mBody->GetInvMass() + 
    Dot(N, Cross(mBody->GetWorldInvInertia() * (Cross(R, N)), R));

  if (denominator < SCALAR_TINY)
    return false;

  tScalar normalImpulse = -normalVel / denominator;

  mBody->ApplyWorldImpulse(normalImpulse * N, worldPos);

  mBody->SetConstraintsAndCollisionsUnsatisfied();
  SetSatisfied();

  return true;
}

