//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file characterobject.cpp 
//                     
//==============================================================
#include "characterobject.hpp"
#include "debugconfig.hpp"

using namespace JigLib;

//==============================================================
// SetProperties
//==============================================================
void tCharacterObject::SetProperties(tScalar elasticity, tScalar staticFriction, tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// tCharacterObject
//==============================================================
tCharacterObject::tCharacterObject(tScalar radius, tScalar length)
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar capLen = length - 2.0f * radius;
  if (capLen < 0.0f) capLen = 0.0f;
  tCapsule capsule(tVector3(0.0f, 0.0f, radius), Matrix33Beta(-90.0f), radius, capLen);
  mCollisionSkin.AddPrimitive(capsule, tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(&mBody);
  SetProperties(0.0f, 2.0f, 2.0f);
  
  mBody.SetCollisionSkin(&mCollisionSkin);
  SetDensity(600.0f);
  
  mBodyAngle = 0.0f;
  mLookUpAngle = 0.0f;

  mMoveFwdSpeed = 10.0f;
  mMoveBackSpeed = 3.0f;
  mMoveSideSpeed = 5.0f;
  mJumpSpeed = 6.0f;

  mBody.mDesiredFwdSpeed = 0.0f;
  mBody.mDesiredLeftSpeed = 0.0f;
  mBody.mJumpSpeed = 0.0f;

  mBody.mFwdSpeed = mBody.mFwdSpeedRate = 0.0f;
  mBody.mLeftSpeed = mBody.mLeftSpeedRate = 0.0f;
  mBody.mTimescale = 0.2f;

  mMoveDir = MOVE_NONE;
  mDoJump = false;

  mDisplayListNum = 0;
}

//==============================================================
// ~tCharacterObject
//==============================================================
tCharacterObject::~tCharacterObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// SetDensity
//==============================================================
void tCharacterObject::SetDensity(tScalar density, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  SetMass(density * mCollisionSkin.GetVolume(), massType);
}

//==============================================================
// SetMass
//==============================================================
void tCharacterObject::SetMass(tScalar mass, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  mBody.SetMass(mass);
  mBody.SetBodyInvInertia(0.0f, 0.0f, 0.0f);
}


//==============================================================
// SetRenderPosition
//==============================================================
void tCharacterObject::SetRenderPosition(tScalar renderFraction)
{
  TRACE_METHOD_ONLY(FRAME_2);
  if (renderFraction < SCALAR_TINY)
  {
    mRenderPosition = mBody.GetOldPosition();
    mRenderOrientation = mBody.GetOldOrientation();
  }
  else if (renderFraction > (1.0f - SCALAR_TINY))
  {
    mRenderPosition = mBody.GetPosition();
    mRenderOrientation = mBody.GetOrientation();
  }
  else
  {
    mRenderPosition = 
      mBody.GetPosition() * renderFraction + 
      mBody.GetOldPosition() * (1.0f - renderFraction);
    // should really use quaternions and SLERP... but maybe if the
    // object is spinning so fast then it doesn't matter if it renders
    // a bit wrong...
    mRenderOrientation = 
      mBody.GetOrientation() * renderFraction + 
      mBody.GetOldOrientation() * (1.0f - renderFraction);
//    mRenderOrientation.Orthonormalise();
  }
}

//==============================================================
// GetRenderBoundingCharacter
//==============================================================
const tSphere & tCharacterObject::GetRenderBoundingSphere() const
{
  // fudge this a little...
  const tCapsule & capsule = mCollisionSkin.GetPrimitiveLocal(0)->GetCapsule();
  static tSphere sphere;
  sphere.SetPos(capsule.GetPos() + 0.5f * capsule.GetLength() * capsule.GetOrient().GetLook());
  sphere.SetRadius(capsule.GetRadius() + 0.5f * capsule.GetLength());
  return sphere;
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tCharacterObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tCharacterObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}

//==============================================================
// Render
//==============================================================
void tCharacterObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  const tCapsule & capsule = mCollisionSkin.GetPrimitiveLocal(0)->GetCapsule();
  
  if (mDisplayListNum == 0)
  {
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    GLCOLOR4(RangedRandom(0.2f, 1.0f), 
             RangedRandom(0.2f, 1.0f),
             RangedRandom(0.2f, 1.0f), 
             0.5f);

    unsigned num = 12;

    GLTRANSLATE(0.0f, 0.0f, capsule.GetRadius());
    gluSphere(GetGLUQuadric(), 
              capsule.GetRadius(), 
              num, num);

    gluCylinder(GetGLUQuadric(), capsule.GetRadius(), capsule.GetRadius(), capsule.GetLength(), num, num);

    GLTRANSLATE(0.0f, 0.0f, capsule.GetLength());
    gluSphere(GetGLUQuadric(), 
              capsule.GetRadius(), 
              num, num);

    glEndList();
  }
  
  if (tDebugConfig::mRenderCollisionBoundingBoxes)
    RenderWorldBoundingBox();
  
  ApplyTransformation(mRenderPosition, mRenderOrientation);
  bool translucent = !mBody.IsActive();
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  glCallList(mDisplayListNum);
  
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
    glDisable(GL_BLEND);
}

//====================================================================
// ControlFwd
//====================================================================
void tCharacterObject::ControlFwd(bool start) {
  if (start) mMoveDir |= MOVE_FWD; else mMoveDir &= ~MOVE_FWD;
  UpdateControl();
}
//====================================================================
// ControlBack
//====================================================================
void tCharacterObject::ControlBack(bool start) {
  if (start) mMoveDir |= MOVE_BACK; else mMoveDir &= ~MOVE_BACK;
  UpdateControl();
}
//====================================================================
// ControlLeft
//====================================================================
void tCharacterObject::ControlLeft(bool start) {
  if (start) mMoveDir |= MOVE_LEFT; else mMoveDir &= ~MOVE_LEFT;
  UpdateControl();
}
//====================================================================
// ControlRight
//====================================================================
void tCharacterObject::ControlRight(bool start) {
  if (start) mMoveDir |= MOVE_RIGHT; else mMoveDir &= ~MOVE_RIGHT;
  UpdateControl();
}
//====================================================================
// ControlJump
//====================================================================
void tCharacterObject::ControlJump() 
{
  mDoJump = true;
  UpdateControl();
}

//====================================================================
// UpdateConstraint
//====================================================================
void tCharacterObject::UpdateControl()
{
  mBody.mDesiredFwdSpeed = 0.0f;
  mBody.mDesiredLeftSpeed = 0.0f;

  mBody.mDesiredFwdSpeed += (mMoveDir & MOVE_FWD) ? mMoveFwdSpeed : 0.0f;
  mBody.mDesiredFwdSpeed -= (mMoveDir & MOVE_BACK) ? mMoveBackSpeed : 0.0f;

  mBody.mDesiredLeftSpeed += (mMoveDir & MOVE_LEFT) ? mMoveSideSpeed : 0.0f;
  mBody.mDesiredLeftSpeed -= (mMoveDir & MOVE_RIGHT) ? mMoveSideSpeed : 0.0f;

  mBody.mJumpSpeed = mDoJump ? mJumpSpeed : 0.0f;
  mDoJump = false;

  mBody.SetActive();
}

//====================================================================
// PostPhysics
//====================================================================
void tCharacterObject::tCharacterBody::PostPhysics(tScalar dt)
{
  const tVector3& fwd = GetOrientation().GetLook();
  const tVector3& left = GetOrientation().GetLeft();
  const tVector3& up = GetOrientation().GetUp();

  bool gotFloor = false;
  const std::vector<tCollisionInfo*> collInfo = GetCollisionSkin()->GetCollisions();
  for (unsigned iColl = 0 ; iColl < collInfo.size() ; ++iColl)
  {
    tCollisionInfo* coll = collInfo[iColl];
    tVector3 N = coll->mDirToBody0;
    if (this == coll->mSkinInfo.skin1->GetOwner())
      N.Negate();

    if (Dot(N, up) > 0.7f)
    {
      gotFloor = true;
      break;
    }
  }

  tScalar timescale = mTimescale;
  if (!gotFloor)
  {
    mJumpSpeed = 0.0f;
    timescale *= 3.0f;
  }

  tScalar fwdSpeed = Dot(GetVelocity(), fwd);
  tScalar leftSpeed = Dot(GetVelocity(), left);

  SmoothCD(mFwdSpeed, mFwdSpeedRate, dt, mDesiredFwdSpeed, timescale);
  SmoothCD(mLeftSpeed, mLeftSpeedRate, dt, mDesiredLeftSpeed, timescale);

  tScalar fwdDiff = mFwdSpeed - fwdSpeed;
  tScalar leftDiff = mLeftSpeed - leftSpeed;

  SetVelocity(GetVelocity() + fwdDiff * fwd + leftDiff * left + mJumpSpeed * up);
  mJumpSpeed = 0.0f;
}
