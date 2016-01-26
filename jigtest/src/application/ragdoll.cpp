//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file ragdoll.cpp 
//                     
//==============================================================
#include "ragdoll.hpp"
#include "boxobject.hpp"
#include "capsuleobject.hpp"
#include "sphereobject.hpp"
#include "renderManager.hpp"

#include "jiglib.hpp"

using namespace JigLib;

//==============================================================
// disable_collisions
//==============================================================
static void DisableCollisions(tBody * rb0, tBody * rb1)
{
  if (!rb0->GetCollisionSkin() || !rb1->GetCollisionSkin())
    return;
  rb0->GetCollisionSkin()->GetNonCollidables().push_back(
    rb1->GetCollisionSkin());
  rb1->GetCollisionSkin()->GetNonCollidables().push_back(
    rb0->GetCollisionSkin());
}

//==============================================================
// Ragdoll
//==============================================================
tRagdoll::tRagdoll(tRenderManager * render,
                   tRagdollType type,
                   float density)
{
  TRACE_METHOD_ONLY(ONCE_2);

  if (COMPLEX == type)
  {
    mNumLimbs = NUM_LIMBS;
    mNumJoints = NUM_JOINTS;
  }
  else
  {
    mNumLimbs = NUM_LIMBS - 5;
    mNumJoints = NUM_JOINTS - 5;
  }

  mLimbs[HEAD]            = new tSphereObject(0.15f);
  mLimbs[UPPER_LEG_LEFT]  = new tCapsuleObject(0.08f, 0.3f, Matrix33Beta(90.0f));
  mLimbs[UPPER_LEG_RIGHT] = new tCapsuleObject(0.08f, 0.3f, Matrix33Beta(90.0f));
  mLimbs[LOWER_LEG_LEFT]  = new tCapsuleObject(0.08f, 0.3f, Matrix33Beta(90.0f));
  mLimbs[LOWER_LEG_RIGHT] = new tCapsuleObject(0.08f, 0.3f, Matrix33Beta(90.0f));
  mLimbs[UPPER_ARM_LEFT]  = new tCapsuleObject(0.07f, 0.2f, Matrix33Gamma(90.0f));
  mLimbs[UPPER_ARM_RIGHT] = new tCapsuleObject(0.07f, 0.2f, Matrix33Gamma(90.0f));
  mLimbs[LOWER_ARM_LEFT]  = new tCapsuleObject(0.06f, 0.2f, Matrix33Gamma(90.0f));
  mLimbs[LOWER_ARM_RIGHT] = new tCapsuleObject(0.06f, 0.2f, Matrix33Gamma(90.0f));
  if (COMPLEX == type)
  {
    mLimbs[FOOT_LEFT]       = new tSphereObject(0.07f);
    mLimbs[FOOT_RIGHT]      = new tSphereObject(0.07f);
    mLimbs[HAND_LEFT]       = new tSphereObject(0.05f);
    mLimbs[HAND_RIGHT]      = new tSphereObject(0.05f);
    mLimbs[TORSO]           = new tBoxObject(tVector3(0.2f,  0.35f, 0.4f), true);
    mLimbs[HIPS]            = new tBoxObject(tVector3(0.2f,  0.35f, 0.2f), true);
  }
  else
  {
    mLimbs[TORSO]           = new tBoxObject(tVector3(0.2f,  0.35f, 0.6f), true);
    mLimbs[FOOT_LEFT] = mLimbs[FOOT_RIGHT] = mLimbs[HAND_LEFT] = mLimbs[HAND_RIGHT] = mLimbs[HIPS] = 0;
  }

  mLimbs[HEAD]           ->GetBody()->SetPosition(tVector3(0.03f, 0.0f, 0.5f));
  mLimbs[UPPER_LEG_LEFT] ->GetBody()->SetPosition(tVector3(0.0f, 0.12f, -0.4f));
  mLimbs[UPPER_LEG_RIGHT]->GetBody()->SetPosition(tVector3(0.0f,-0.12f, -0.4f));
  mLimbs[LOWER_LEG_LEFT] ->GetBody()->SetPosition(tVector3(0.0f, 0.12f, -0.7f));
  mLimbs[LOWER_LEG_RIGHT]->GetBody()->SetPosition(tVector3(0.0f,-0.12f, -0.7f));
  mLimbs[UPPER_ARM_LEFT] ->GetBody()->SetPosition(tVector3(0.0f, 0.25f, 0.25f));
  mLimbs[UPPER_ARM_RIGHT]->GetBody()->SetPosition(tVector3(0.0f,-0.25f, 0.25f));
  mLimbs[LOWER_ARM_LEFT] ->GetBody()->SetPosition(tVector3(0.0f, 0.5f, 0.25f));
  mLimbs[LOWER_ARM_RIGHT]->GetBody()->SetPosition(tVector3(0.0f,-0.5f, 0.25f));
  if (COMPLEX == type)
  {
    mLimbs[FOOT_LEFT]      ->GetBody()->SetPosition(tVector3(0.13f, 0.12f, -0.85f));
    mLimbs[FOOT_RIGHT]     ->GetBody()->SetPosition(tVector3(0.13f,-0.12f, -0.85f));
    mLimbs[HAND_LEFT]      ->GetBody()->SetPosition(tVector3(0.0f, 0.72f, 0.25f));
    mLimbs[HAND_RIGHT]     ->GetBody()->SetPosition(tVector3(0.0f,-0.72f, 0.25f));
    mLimbs[TORSO]          ->GetBody()->SetPosition(tVector3(0.0f, 0.0f, 0.2f));
    mLimbs[HIPS]           ->GetBody()->SetPosition(tVector3(0.0f, 0.0f,-0.1f));
  }
  else
  {
    mLimbs[TORSO]          ->GetBody()->SetPosition(tVector3(0.0f, 0.0f, 0.0f));
  }

  // set up the hinge joints. 
  tScalar halfWidth = 0.2f;
  tScalar sidewaysSlack = 0.1f;
  tScalar damping = 0.5f;
  
  if (COMPLEX == type)
  {
    mJoints[SPINE].Initialise(mLimbs[HIPS]->GetBody(), 
                              mLimbs[TORSO]->GetBody(), 
                              tVector3(0.0f, 1.0f, 0.0f), 
                              tVector3(0.0f, 0.0f, 0.1f),
                              halfWidth, 
                              70.0f, 30.0f,
                              3.0f * sidewaysSlack,
                              damping);

    mJoints[NECK].Initialise(mLimbs[TORSO]->GetBody(), 
                             mLimbs[HEAD]->GetBody(), 
                             tVector3(0.0f, 1.0f, 0.0f), 
                             tVector3(-0.05f, 0.0f, 0.25f),
                             halfWidth, 
                             50.0f, 20.0f,
                             3 * sidewaysSlack,
                             damping);

    mJoints[SHOULDER_LEFT].Initialise(mLimbs[TORSO]->GetBody(), 
                                      mLimbs[UPPER_ARM_LEFT]->GetBody(), 
                                      tVector3(1.0f, 0.0f, 0.0f), 
                                      tVector3(0.0f, 0.15f, 0.05f), 
                                      halfWidth,
                                      30.0f, 75.0f,
                                      0.7f, 
                                      damping);

    mJoints[SHOULDER_RIGHT].Initialise(mLimbs[TORSO]->GetBody(), 
                                       mLimbs[UPPER_ARM_RIGHT]->GetBody(), 
                                       tVector3(1.0f, 0.0f, 0.0f), 
                                       tVector3(0.0f,-0.15f, 0.05f), 
                                       halfWidth, 
                                       75.0f, 30.0f,
                                       0.7f, 
                                       damping);

    mJoints[HIP_LEFT].Initialise(mLimbs[HIPS]->GetBody(), 
                                 mLimbs[UPPER_LEG_LEFT]->GetBody(), 
                                 tVector3(0.0f, 1.0f, 0.0f), 
                                 tVector3(0.0f, 0.12f,-0.05f), 
                                 halfWidth, 
                                 10.0f, 60.0f, 
                                 0.4f, 
                                 damping);

    mJoints[HIP_RIGHT].Initialise(mLimbs[HIPS]->GetBody(),
                                  mLimbs[UPPER_LEG_RIGHT]->GetBody(), 
                                  tVector3(0.0f, 1.0f, 0.0f), 
                                  tVector3(0.0f,-0.12f,-0.05f), 
                                  halfWidth, 
                                  10.0f, 60.0f, 
                                  0.4f, 
                                  damping);

    mJoints[ANKLE_LEFT].Initialise(mLimbs[LOWER_LEG_LEFT]->GetBody(), 
                                   mLimbs[FOOT_LEFT]->GetBody(), 
                                   tVector3(0.0f, 1.0f, 0.0f), 
                                   tVector3(0.0f, 0.15f,-0.15f), 
                                   halfWidth, 
                                   30.0f, 10.0f,
                                   0.01f,
                                   damping);

    mJoints[ANKLE_RIGHT].Initialise(mLimbs[LOWER_LEG_RIGHT]->GetBody(), 
                                    mLimbs[FOOT_RIGHT]->GetBody(), 
                                    tVector3(0.0f, 1.0f, 0.0f), 
                                    tVector3(0.0f,-0.15f,-0.15f), 
                                    halfWidth, 
                                    30.0f, 10.0f, 
                                    0.01f, 
                                    damping);

    mJoints[WRIST_LEFT].Initialise(mLimbs[LOWER_ARM_LEFT]->GetBody(), 
                                   mLimbs[HAND_LEFT]->GetBody(), 
                                   tVector3(0.0f, 0.0f, 1.0f), 
                                   tVector3(0.0f, 0.12f, 0.0f), 
                                   halfWidth, 
                                   45.0f, 70.0f,
                                   0.01f, 
                                   damping);

    mJoints[WRIST_RIGHT].Initialise(mLimbs[LOWER_ARM_RIGHT]->GetBody(), 
                                    mLimbs[HAND_RIGHT]->GetBody(), 
                                    tVector3(0.0f, 0.0f, 1.0f), 
                                    tVector3(0.0f, -0.12f, 0.0f), 
                                    halfWidth, 
                                    70.0f, 45.0f, 
                                    0.01f,
                                    damping);
  }
  else
  {
    mJoints[NECK].Initialise(mLimbs[TORSO]->GetBody(), 
                             mLimbs[HEAD]->GetBody(), 
                             tVector3(0.0f, 1.0f, 0.0f), 
                             tVector3(-0.05f, 0.0f, 0.25f),
                             halfWidth, 
                             50.0f, 20.0f,
                             3 * sidewaysSlack,
                             damping);

    mJoints[SHOULDER_LEFT].Initialise(mLimbs[TORSO]->GetBody(), 
                                      mLimbs[UPPER_ARM_LEFT]->GetBody(), 
                                      tVector3(1.0f, 0.0f, 0.0f), 
                                      tVector3(0.0f, 0.15f, 0.25f), 
                                      halfWidth,
                                      30.0f, 75.0f,
                                      0.7f, 
                                      damping);

    mJoints[SHOULDER_RIGHT].Initialise(mLimbs[TORSO]->GetBody(), 
                                       mLimbs[UPPER_ARM_RIGHT]->GetBody(), 
                                       tVector3(1.0f, 0.0f, 0.0f), 
                                       tVector3(0.0f,-0.15f, 0.25f), 
                                       halfWidth, 
                                       75.0f, 30.0f,
                                       0.7f, 
                                       damping);

    mJoints[HIP_LEFT].Initialise(mLimbs[TORSO]->GetBody(), 
                                 mLimbs[UPPER_LEG_LEFT]->GetBody(), 
                                 tVector3(0.0f, 1.0f, 0.0f), 
                                 tVector3(0.0f, 0.12f,-0.25f), 
                                 halfWidth, 
                                 10.0f, 60.0f, 
                                 0.4f, 
                                 damping);

    mJoints[HIP_RIGHT].Initialise(mLimbs[TORSO]->GetBody(),
                                  mLimbs[UPPER_LEG_RIGHT]->GetBody(), 
                                  tVector3(0.0f, 1.0f, 0.0f), 
                                  tVector3(0.0f,-0.12f,-0.25f), 
                                  halfWidth, 
                                  10.0f, 60.0f, 
                                  0.4f, 
                                  damping);

  }

  
  mJoints[KNEE_LEFT].Initialise(mLimbs[UPPER_LEG_LEFT]->GetBody(), 
                                mLimbs[LOWER_LEG_LEFT]->GetBody(), 
                                tVector3(0.0f, 1.0f, 0.0f), 
                                tVector3(0.0f, 0.0f,-0.15f), 
                                halfWidth, 
                                100.0f, 0.0f, 
                                sidewaysSlack, 
                                damping);

  mJoints[KNEE_RIGHT].Initialise(mLimbs[UPPER_LEG_RIGHT]->GetBody(), 
                                 mLimbs[LOWER_LEG_RIGHT]->GetBody(), 
                                 tVector3(0.0f, 1.0f, 0.0f), 
                                 tVector3(0.0f, 0.0f,-0.15f), 
                                 halfWidth, 
                                 100.0f, 0.0f,
                                 sidewaysSlack, 
                                 damping);

  mJoints[ELBOW_LEFT].Initialise(mLimbs[UPPER_ARM_LEFT]->GetBody(), 
                                 mLimbs[LOWER_ARM_LEFT]->GetBody(), 
                                 tVector3(0.0f, 0.0f, 1.0f), 
                                 tVector3(0.0f, 0.13f, 0.0f), 
                                 halfWidth,
                                 0.0f, 130.0f,
                                 sidewaysSlack, 
                                 damping);

  mJoints[ELBOW_RIGHT].Initialise(mLimbs[UPPER_ARM_RIGHT]->GetBody(),
                                  mLimbs[LOWER_ARM_RIGHT]->GetBody(), 
                                  tVector3(0.0f, 0.0f, 1.0f), 
                                  tVector3(0.0f,-0.13f, 0.0f),
                                  halfWidth, 
                                  130.0f, 0.0f,
                                  sidewaysSlack,
                                  damping);
  
  // disable some collisions between adjacent pairs
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[UPPER_LEG_LEFT]->GetBody());
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[UPPER_LEG_RIGHT]->GetBody());
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[UPPER_ARM_LEFT]->GetBody());
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[UPPER_ARM_RIGHT]->GetBody());
  DisableCollisions(mLimbs[UPPER_LEG_LEFT]->GetBody(), mLimbs[LOWER_LEG_LEFT]->GetBody());
  DisableCollisions(mLimbs[UPPER_LEG_RIGHT]->GetBody(), mLimbs[LOWER_LEG_RIGHT]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_LEFT]->GetBody(), mLimbs[LOWER_ARM_LEFT]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_RIGHT]->GetBody(), mLimbs[LOWER_ARM_RIGHT]->GetBody());
  if (COMPLEX == type)
  {
    DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[HIPS]->GetBody());
    DisableCollisions(mLimbs[LOWER_LEG_LEFT]->GetBody(), mLimbs[FOOT_LEFT]->GetBody());
    DisableCollisions(mLimbs[LOWER_LEG_RIGHT]->GetBody(), mLimbs[FOOT_RIGHT]->GetBody());
    DisableCollisions(mLimbs[LOWER_ARM_LEFT]->GetBody(), mLimbs[HAND_LEFT]->GetBody());
    DisableCollisions(mLimbs[LOWER_ARM_RIGHT]->GetBody(), mLimbs[HAND_RIGHT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[UPPER_LEG_LEFT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[UPPER_LEG_RIGHT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[UPPER_ARM_LEFT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[UPPER_ARM_RIGHT]->GetBody());
  }

  // he's not double-jointed...
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[LOWER_LEG_LEFT]->GetBody());
  DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[LOWER_LEG_RIGHT]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_LEFT]->GetBody(), mLimbs[UPPER_ARM_RIGHT]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_RIGHT]->GetBody(), mLimbs[UPPER_ARM_LEFT]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_LEFT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[UPPER_ARM_RIGHT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[UPPER_LEG_LEFT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[UPPER_LEG_RIGHT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[LOWER_LEG_LEFT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[LOWER_LEG_RIGHT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[LOWER_LEG_LEFT]->GetBody(), mLimbs[HEAD]->GetBody());
  DisableCollisions(mLimbs[LOWER_LEG_RIGHT]->GetBody(), mLimbs[HEAD]->GetBody());
  if (COMPLEX == type)
  {
    DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[FOOT_LEFT]->GetBody());
    DisableCollisions(mLimbs[TORSO]->GetBody(), mLimbs[FOOT_RIGHT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[FOOT_LEFT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[FOOT_RIGHT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[LOWER_LEG_LEFT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[LOWER_LEG_RIGHT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[LOWER_ARM_LEFT]->GetBody());
    DisableCollisions(mLimbs[HIPS]->GetBody(), mLimbs[LOWER_ARM_RIGHT]->GetBody());
    DisableCollisions(mLimbs[FOOT_LEFT]->GetBody(), mLimbs[HAND_LEFT]->GetBody());
    DisableCollisions(mLimbs[FOOT_RIGHT]->GetBody(), mLimbs[HAND_RIGHT]->GetBody());
    DisableCollisions(mLimbs[FOOT_LEFT]->GetBody(), mLimbs[HEAD]->GetBody());
    DisableCollisions(mLimbs[FOOT_RIGHT]->GetBody(), mLimbs[HEAD]->GetBody());
    DisableCollisions(mLimbs[FOOT_LEFT]->GetBody(), mLimbs[UPPER_ARM_LEFT]->GetBody());
    DisableCollisions(mLimbs[FOOT_RIGHT]->GetBody(), mLimbs[UPPER_ARM_RIGHT]->GetBody());
    DisableCollisions(mLimbs[FOOT_LEFT]->GetBody(), mLimbs[UPPER_ARM_RIGHT]->GetBody());
    DisableCollisions(mLimbs[FOOT_RIGHT]->GetBody(), mLimbs[UPPER_ARM_LEFT]->GetBody());
    DisableCollisions(mLimbs[FOOT_LEFT]->GetBody(), mLimbs[UPPER_LEG_LEFT]->GetBody());
    DisableCollisions(mLimbs[FOOT_RIGHT]->GetBody(), mLimbs[UPPER_LEG_RIGHT]->GetBody());
  }
  unsigned i;
  for (i = 0 ; i < mNumLimbs ; ++i)
  {
    mLimbs[i]->SetProperties(0.2f, 3.0f, 2.0f);
    mLimbs[i]->SetDensity(500.0f);
    mLimbs[i]->GetBody()->SetActivityThreshold(2.0f, 180.0f);
    mLimbs[i]->GetBody()->SetDoShockProcessing(false);
    mLimbs[i]->GetBody()->EnableBody();
    render->AddObject(mLimbs[i]);
  }
  for (i = 0 ; i < mNumJoints ; ++i)
  {
    mJoints[i].EnableHinge();
  }
}

//==============================================================
// ~tRagdoll
//==============================================================
tRagdoll::~tRagdoll()
{
  TRACE_METHOD_ONLY(ONCE_2);
  for (unsigned i = 0 ; i < NUM_LIMBS ; ++i)
  {
    delete mLimbs[i];
    mLimbs[i] = 0;
  }
}


//==============================================================
// move_torso
//==============================================================
void tRagdoll::MoveTorso(const tVector3 & pos)
{
  TRACE_METHOD_ONLY(ONCE_2);
  tVector3 delta = pos - mLimbs[TORSO]->GetBody()->GetPosition();
  unsigned i;
  for (i = 0 ; i < mNumLimbs ; ++i)
  {
    tVector3 origPos = mLimbs[i]->GetBody()->GetPosition();
    mLimbs[i]->GetBody()->MoveTo(origPos + delta, 
                                 mLimbs[i]->GetBody()->GetOrientation());
  }
}

//==============================================================
// apply_rotation_to_torso
//==============================================================
void tRagdoll::ApplyRotationToTorso(const tMatrix33 & rotationMatrix)
{
  TRACE_METHOD_ONLY(ONCE_2);
  unsigned i;
  const tVector3 & origTorsoPos = mLimbs[TORSO]->GetBody()->GetPosition();
  for (i = 0 ; i < mNumLimbs ; ++i)
  {
    tVector3 delta = mLimbs[i]->GetBody()->GetPosition() - origTorsoPos;
    delta = rotationMatrix * delta;
    tMatrix33 orientation = mLimbs[i]->GetBody()->GetOrientation();
    orientation = rotationMatrix * orientation;
    tVector3 origPos = mLimbs[i]->GetBody()->GetPosition();
    mLimbs[i]->GetBody()->MoveTo(origTorsoPos + delta, orientation);
  }
}

//==============================================================
// GetBody
//==============================================================
tBody * tRagdoll::GetBody()
{
  return mLimbs[TORSO]->GetBody();
}

//==============================================================
// SetRenderPosition
//==============================================================
void tRagdoll::SetRenderPosition(JigLib::tScalar renderFraction)
{
  for (unsigned i = 0 ; i < mNumLimbs ; ++i)
    mLimbs[i]->SetRenderPosition(renderFraction);
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const tSphere & tRagdoll::GetRenderBoundingSphere() const
{
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tRagdoll::GetRenderPosition() const
{
  return mLimbs[TORSO]->GetRenderPosition();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tRagdoll::GetRenderOrientation() const
{
  return mLimbs[TORSO]->GetRenderOrientation();
}

//==============================================================
// PutToSleep
//==============================================================
void tRagdoll::PutToSleep()
{
  for (unsigned i = 0 ; i < mNumLimbs ; ++i)
  {
    mLimbs[i]->GetBody()->SetInactive();
  }
}

//====================================================================
// SetPhysicsPosition
//====================================================================
void tRagdoll::SetPhysicsPosition(const JigLib::tVector3& pos, const JigLib::tMatrix33& orient)
{
  MoveTorso(pos);
}

//====================================================================
// SetPhysicsVelocity
//====================================================================
void tRagdoll::SetPhysicsVelocity(const JigLib::tVector3& vel)
{
  for (unsigned i = 0 ; i < mNumLimbs ; ++i)
  {
    mLimbs[i]->GetBody()->SetVelocity(vel);
  }
}
