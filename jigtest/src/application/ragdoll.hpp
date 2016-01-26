//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file ragdoll.hpp 
//                     
//==============================================================
#ifndef RAGDOLL_HPP
#define RAGDOLL_HPP

#include "object.hpp"

#include "jiglib.hpp"

class tRagdoll : public tObject
{
public:
  /// complex ragdoll has more limbs/joints
  enum tRagdollType {SIMPLE, COMPLEX};
  tRagdoll(class tRenderManager * render,
           tRagdollType type,
           float density);
  ~tRagdoll();
  
  JigLib::tBody * GetBody();
  void SetRenderPosition(JigLib::tScalar renderFraction);

  void Render(tRenderType renderType) {}
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
  /// move everything so that the torso is at the specified position
  void MoveTorso(const JigLib::tVector3 & pos);
  
  /// rotate everything
  void ApplyRotationToTorso(const JigLib::tMatrix33 & mat);

  /// Makes everything inactive/frozen 
  void PutToSleep();

  virtual void SetPhysicsPosition(const JigLib::tVector3& pos, const JigLib::tMatrix33& orient);
  virtual void SetPhysicsVelocity(const JigLib::tVector3& vel);

private:
  // indices into m_components
  enum tLimb_id {
    TORSO, 
    HEAD, 
    UPPER_LEG_LEFT,
    UPPER_LEG_RIGHT, 
    LOWER_LEG_LEFT, 
    LOWER_LEG_RIGHT, 
    UPPER_ARM_LEFT,
    UPPER_ARM_RIGHT,
    LOWER_ARM_LEFT,
    LOWER_ARM_RIGHT,
    FOOT_LEFT,
    FOOT_RIGHT,
    HAND_LEFT,
    HAND_RIGHT,
    HIPS,
    NUM_LIMBS
  };
  
  enum tJoint_id
  {
    NECK,
    SHOULDER_LEFT,
    SHOULDER_RIGHT,
    ELBOW_LEFT,
    ELBOW_RIGHT,
    HIP_LEFT,
    HIP_RIGHT,
    KNEE_LEFT,
    KNEE_RIGHT,
    WRIST_LEFT,
    WRIST_RIGHT,
    ANKLE_LEFT,
    ANKLE_RIGHT,
    SPINE,
    NUM_JOINTS
  };

  tObject * mLimbs[NUM_LIMBS];
  JigLib::tHingeJoint mJoints[NUM_JOINTS];

  unsigned mNumLimbs;
  unsigned mNumJoints;
};

#endif
