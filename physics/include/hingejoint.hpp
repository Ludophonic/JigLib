//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file hingejoint.hpp 
//                     
//==============================================================
#ifndef JIGHINGE_JOINT_HPP
#define JIGHINGE_JOINT_HPP

#include "../physics/include/joint.hpp"
#include "../physics/include/constraintpoint.hpp"
#include "../physics/include/constraintmaxdistance.hpp"

namespace JigLib
{
  
  class tBody;
  /// implements a simple hinge between two rigid bodies. The bodies
  /// should be in a suitable configuration when this joint is
  /// created.
  class tHingeJoint : public tJoint
  {
  public:
    // default constructor so you can initialise this joint later.
    tHingeJoint();

    /// At the moment the destructor deregisters the constraints.
    ~tHingeJoint();

    /// TODO at the moment the bodies should be unrotated when this is
    /// called...  No angle limit if fwd > MAX_HINGE_ANGLE_LIMIT after
    /// calling initialise the joint will be functional -
    /// i.e. registered... so the bodies should be registered too!  If
    /// damping > 0 then the joint will be damped such that the torque
    /// is equal to -damping * relative-rotation of the bodies
    /// involved. The actual value is clamped to prevent instability.
    enum {MAX_HINGE_ANGLE_LIMIT = 150};
    void Initialise(tBody * body0, tBody * body1, 
                    const tVector3 & hingeAxis, 
                    const tVector3 & hingePosRel0,
                    const tScalar hingeHalfWidth,
                    const tScalar hingeFwdAngle,
                    const tScalar hingeBckAngle,
                    const tScalar sidewaysSlack,
                    const tScalar damping = -1.0f);

    /// Register the constraints
    void EnableHinge();

    /// deregister the constraints
    void DisableHinge();

    bool GetHingeEnabled() const {return mHingeEnabled;}

    /// Just remove the limit constraint
    void Break();

    /// Just enable the limit constraint
    void Mend();

    /// Are we broken
    bool IsBroken() const {return mBroken;}

    const tVector3 & GetHingePosRel0() const { return mHingePosRel0; }

    /// We can be asked to apply an extra torque to body0 (and
    /// opposite to body1) each time step.
    void SetExtraTorque(tScalar torque) {mExtraTorque = torque;}

  private:
    /// Inherited from cJigController
    void UpdateController(tScalar dt);

  private:
    tVector3 mHingeAxis;
    tVector3 mHingePosRel0;
    tBody * mBody0;
    tBody * mBody1;
    bool mUsingLimit;
    bool mHingeEnabled;
    bool mBroken;
    tScalar mDamping;
    tScalar mExtraTorque; // allow extra torque applied per update
    tConstraintPoint mMidPointConstraint;
    tConstraintMaxDistance mSidePointConstraints[2];
    tConstraintMaxDistance mMaxDistanceConstraint;
  };
}

#endif
