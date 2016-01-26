//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file hingejoint.cpp 
//                     
//==============================================================
#include "hingejoint.hpp"
#include "body.hpp"
#include "constraintpoint.hpp"
#include "constraintmaxdistance.hpp"
#include "physics.hpp"

using namespace JigLib;

//========================================================
// tHingeJoint
//========================================================
tHingeJoint::tHingeJoint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0 = mBody1 = 0;
  mHingeEnabled = false;
  mUsingLimit = false;
  mBroken = false;
  mExtraTorque = 0.0f;
  mDamping = 0.0f;
}

//========================================================
// ~tHingeJoint
//========================================================
tHingeJoint::~tHingeJoint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  DisableHinge();
}

//========================================================
// EnableHinge
//========================================================
void tHingeJoint::EnableHinge()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (mHingeEnabled)
    return;

  if (mBody0)
  {
    mMidPointConstraint.EnableConstraint();
    mSidePointConstraints[0].EnableConstraint();
    mSidePointConstraints[1].EnableConstraint();
    if (mUsingLimit && !mBroken)
      mMaxDistanceConstraint.EnableConstraint();
    EnableController();
  }
  mHingeEnabled = true;
}

//========================================================
// DisableHinge
//========================================================
void tHingeJoint::DisableHinge()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (!mHingeEnabled)
    return;

  if (mBody0)
  {
    mMidPointConstraint.DisableConstraint();
    mSidePointConstraints[0].DisableConstraint();
    mSidePointConstraints[1].DisableConstraint();
    if (mUsingLimit && !mBroken)
      mMaxDistanceConstraint.DisableConstraint();
    DisableController();
  }
  mHingeEnabled = false;
}

//========================================================
// Break
//========================================================
void tHingeJoint::Break()
{
  TRACE_METHOD_ONLY(ONCE_3);
  if (mBroken)
    return;
  if (mUsingLimit)
    mMaxDistanceConstraint.DisableConstraint();
  mBroken = true;
}

//========================================================
// Mend
//========================================================
void tHingeJoint::Mend()
{
  TRACE_METHOD_ONLY(ONCE_3);
  if (!mBroken)
    return;
  if (mUsingLimit)
    mMaxDistanceConstraint.EnableConstraint();
  mBroken = false;
}

//========================================================
// Init
//========================================================
void tHingeJoint::Initialise(tBody * body0, tBody * body1, 
                             const tVector3 & hingeAxis, 
                             const tVector3 & hingePosRel0,
                             const tScalar hingeHalfWidth,
                             const tScalar hingeFwdAngle,
                             const tScalar hingeBckAngle,
                             const tScalar sidewaysSlack,
                             const tScalar damping)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody0 = body0;
  mBody1 = body1;
  mHingeAxis = hingeAxis;
  mHingePosRel0 = hingePosRel0;
  mUsingLimit = false;
  mDamping = damping;

  //  tScalar allowedDistance = 0.005f;
  mHingeAxis.Normalise();

  tVector3 hingePosRel1 = 
    body0->GetPosition() + hingePosRel0 - body1->GetPosition();

  // generate the two positions relative to each body
  tVector3 relPos0a = hingePosRel0 + hingeHalfWidth * hingeAxis;
  tVector3 relPos0b = hingePosRel0 - hingeHalfWidth * hingeAxis;

  tVector3 relPos1a = hingePosRel1 + hingeHalfWidth * hingeAxis;
  tVector3 relPos1b = hingePosRel1 - hingeHalfWidth * hingeAxis;

  tScalar timescale = 1.0f / 20.0f;
  tScalar allowedDistanceMid = 0.005f;
  tScalar allowedDistanceSide = sidewaysSlack * hingeHalfWidth;

  mSidePointConstraints[0].Initialise(
    body0, relPos0a, body1, relPos1a, allowedDistanceSide);
  mSidePointConstraints[1].Initialise(
    body0, relPos0b, body1, relPos1b, allowedDistanceSide);

  mMidPointConstraint.Initialise(
    body0, hingePosRel0, body1, hingePosRel1, 
    allowedDistanceMid, timescale);

  if (hingeFwdAngle <= MAX_HINGE_ANGLE_LIMIT)
  {
    // choose a direction that is perpendicular to the hinge
    tVector3 perpDir(0.0f, 0.0f, 1.0f);
    if (Dot(perpDir, hingeAxis) > 0.1f)
      perpDir.Set(0.0f, 1.0f, 0.0f);
    // now make it perpendicular to the hinge
    tVector3 sideAxis = Cross(hingeAxis, perpDir);
    perpDir = Cross(sideAxis, hingeAxis).Normalise();
    
    // the length of the "arm" TODO take this as a parameter? what's
    // the effect of changing it?
    tScalar len = 10.0f * hingeHalfWidth;
    
    // Choose a position using that dir. this will be the anchor point
    // for body 0. relative to hinge
    tVector3 hingeRelAnchorPos0 = perpDir * len;

    // anchor point for body 2 is chosen to be in the middle of the
    // angle range.  relative to hinge
    tScalar angleToMiddle = 0.5f * (hingeFwdAngle - hingeBckAngle);
    tVector3 hingeRelAnchorPos1 = 
      RotationMatrix(-angleToMiddle, hingeAxis) * hingeRelAnchorPos0;
    
    // work out the "string" length
    tScalar hingeHalfAngle = 0.5f * (hingeFwdAngle + hingeBckAngle);
    tScalar allowedDistance = len * 2.0f * SinDeg(hingeHalfAngle * 0.5f);

    tVector3 hingePos = body1->GetPosition() + hingePosRel0;
    tVector3 relPos0c = hingePos + hingeRelAnchorPos0 - body0->GetPosition();
    tVector3 relPos1c = hingePos + hingeRelAnchorPos1 - body1->GetPosition();

    mMaxDistanceConstraint.Initialise(
      body0, relPos0c, 
      body1, relPos1c,
      allowedDistance);
    mUsingLimit = true;
  }
  if (mDamping <= 0.0f)
    mDamping = -1.0f; // just make sure that a value of 0.0 doesn't mess up...
  else
    Limit(mDamping, SCALAR(0.0f), SCALAR(1.0f));
}

//========================================================
// UpdateController
//========================================================
void tHingeJoint::UpdateController(const tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_2);
  Assert(0 != mBody0);
  Assert(0 != mBody1);
  if(mDamping > 0.0f)
  {
    // Some hinges can bend in wonky ways. Derive the effective hinge axis
    // using the relative rotation of the bodies.
    tVector3 hingeAxis = mBody1->GetAngVel() - mBody0->GetAngVel();
    hingeAxis.NormaliseSafe();

    const tScalar angRot1 = Dot(mBody0->GetAngVel(), hingeAxis);
    const tScalar angRot2 = Dot(mBody1->GetAngVel(), hingeAxis);

    const tScalar avAngRot = 0.5f * (angRot1 + angRot2);

    const tScalar frac = 1.0f - mDamping;
    const tScalar newAngRot1 = avAngRot + (angRot1 - avAngRot) * frac;
    const tScalar newAngRot2 = avAngRot + (angRot2 - avAngRot) * frac;

    const tVector3 newAngVel1 = 
      mBody0->GetAngVel() + (newAngRot1 - angRot1) * hingeAxis;
    const tVector3 newAngVel2 = 
      mBody1->GetAngVel() + (newAngRot2 - angRot2) * hingeAxis;

    mBody0->SetAngVel(newAngVel1);
    mBody1->SetAngVel(newAngVel2);
  }

  // the extra torque
  if (mExtraTorque != 0.0f)
  {
    tVector3 torque1 = 
      mExtraTorque * (mBody0->GetOrientation() * mHingeAxis);
    mBody0->AddWorldTorque(torque1);
    mBody1->AddWorldTorque(-torque1);
  }
}
