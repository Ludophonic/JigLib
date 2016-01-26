//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintvelocity.hpp 
//                     
//==============================================================
#ifndef JIGCONSTRAINT_VELOCITY_HPP
#define JIGCONSTRAINT_VELOCITY_HPP

#include "../physics/include/constraint.hpp"
#include "../maths/include/vector3.hpp"

namespace JigLib
{
  class tBody;

  /// constraints a velocity to be a certain value - either in world 
  /// or body (by transforming the velocity direction) coordinates
  class tConstraintVelocity : public tConstraint
  {
  public:
    enum tReferenceFrame {WORLD, BODY};
    tConstraintVelocity(tBody * body, 
                        tReferenceFrame frame,
                        const tVector3 * vel,
                        const tVector3 * angVel);

    tConstraintVelocity();
    ~tConstraintVelocity();

    void Initialise(tBody * body, 
                    tReferenceFrame frame,
                    const tVector3 * vel,
                    const tVector3 * angVel);

  private:
    /// apply impulses to satisfy the constraint.
    void PreApply(tScalar dt);
    bool Apply(tScalar dt);
    void Destroy();

  private:
    tBody * mBody;
    tReferenceFrame mFrame;
    tVector3 mVel;
    tVector3 mAngVel;
    tVector3 mVelRate;
    tVector3 mAngVelRate;
    tVector3 mTargetVel;
    tVector3 mTargetAngVel;
    bool mDoVel;
    bool mDoAngVel;
  };
}

#endif
