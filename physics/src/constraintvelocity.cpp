//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintvelocity.cpp 
//                     
//==============================================================
#include "constraintvelocity.hpp"
#include "jiglib.hpp"

using namespace JigLib;

//==============================================================
// tConstraintVelocity
//==============================================================
tConstraintVelocity::tConstraintVelocity(tBody * body, 
                                         tReferenceFrame frame,
                                         const tVector3 * vel,
                                         const tVector3 * angVel)
{
  TRACE_METHOD_ONLY(ONCE_2);
  Initialise(body, frame, vel, angVel);
}
//==============================================================
// tConstraintVelocity
//==============================================================
tConstraintVelocity::tConstraintVelocity()
{
  TRACE_METHOD_ONLY(ONCE_2);
  Initialise(0, WORLD, 0, 0);
}

//==============================================================
// tConstraintVelocity
//==============================================================
tConstraintVelocity::~tConstraintVelocity()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (mBody) mBody->RemoveConstraint(this);
  Destroy();
}

//==============================================================
// Initialise
//==============================================================
void tConstraintVelocity::Initialise(tBody * body, 
                                     tReferenceFrame frame,
                                     const tVector3 * vel,
                                     const tVector3 * angVel)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = body;
  mFrame = frame;
  mDoVel = true;
  mDoAngVel = true;

  if (vel)
    mTargetVel = *vel;
  else
    mDoVel = false;

  if (angVel)
    mTargetAngVel = *angVel;
  else
    mDoAngVel = false;

  if (mBody)
  {
    mVel = body->GetVelocity();
    mAngVel = body->GetAngVel();
  }
  else
  {
    mVel.SetTo(0.0f);
    mAngVel.SetTo(0.0f);
  }
  mVelRate.SetTo(0.0f);
  mAngVelRate.SetTo(0.0f);

  if (mBody) mBody->AddConstraint(this);
}

//==============================================================
// Destroy
//==============================================================
void tConstraintVelocity::Destroy()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody = 0;
  DisableConstraint();
}

//==============================================================
// PreApply
//==============================================================
void tConstraintVelocity::PreApply(tScalar dt)
{
  tConstraint::PreApply(dt);
  static const tScalar smoothTime = 0.2f;
  if (mDoVel)
    SmoothCD(mVel, mVelRate, dt, mTargetVel, smoothTime);
  if (mDoAngVel)
    SmoothCD(mAngVel, mAngVelRate, dt, mTargetAngVel, smoothTime);

  // Try to prevent constraining the velocity into pushing through static geometry
  if (mDoVel && mBody->GetCollisionSkin() && !mBody->GetCollisionSkin()->GetCollisions().empty())
  {
    const std::vector<class tCollisionInfo *>& collisions = mBody->GetCollisionSkin()->GetCollisions();
    unsigned num = collisions.size();
    for (unsigned i = 0 ; i < num ; ++i)
    {
      const tCollisionInfo* collInfo = collisions[i];
      if (collInfo->mSkinInfo.skin1->GetOwner() == 0)
      {
        const tVector3 dir = collInfo->mDirToBody0.GetNormalisedSafe();
        tScalar dot = Dot(mVel, dir);
        if (dot < 0.0f)
          mVel -= dot * dir;
      }
    }
  }
}

//==============================================================
// Apply
//==============================================================
bool tConstraintVelocity::Apply(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  SetSatisfied();

  if (!mBody) return false;

  tScalar frac = 0.5f;

  if (mFrame == BODY)
  {
    if (mDoVel)
      mBody->SetVelocity(frac * mBody->GetOrientation() * mVel + (1.0f - frac) * mBody->GetVelocity());
    if (mDoAngVel)
      mBody->SetAngVel(frac * mBody->GetOrientation() * mAngVel + (1.0f - frac) * mBody->GetAngVel());
  }
  else
  {
    if (mDoVel)
      mBody->SetVelocity(frac * mVel + (1.0f - frac) * mBody->GetVelocity());
    if (mDoAngVel)
      mBody->SetAngVel(frac * mAngVel + (1.0f - frac) * mBody->GetAngVel());
  }
  /// todo return false if we were already there...

  mBody->SetConstraintsAndCollisionsUnsatisfied();
  SetSatisfied();

  return true;
}

