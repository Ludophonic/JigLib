//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintpoint.hpp 
//                     
//==============================================================
#ifndef JIGCONSTRAINT_POINT_HPP
#define JIGCONSTRAINT_POINT_HPP

#include "../physics/include/constraint.hpp"
#include "../maths/include/vector3.hpp"

namespace JigLib
{
  class tBody;
  
  /// Constraints a point on one body to be fixed to a point on another body
  class tConstraintPoint : public tConstraint
  {
  public:
    /// can set things up in the constructor...
    /// allowed_distance indicated how much the points are allowed to deviate.
    /// timescale indicates the timescale over which deviation is eliminated
    /// (suggest a few times dt - be careful if there's a variable timestep!)
    /// if timescale < 0 then the value indicates the number of dts
    tConstraintPoint(tBody * body0, const tVector3 & body0Pos,
                     tBody * body1, const tVector3 & body1Pos,
                     tScalar allowedDistance,
                     tScalar timescale);
    /// A default constructor - but make sue you call initialise
    /// before using!
    tConstraintPoint();
  
    ~tConstraintPoint();

    void Initialise(tBody * body0, const tVector3 & body0Pos,
                    tBody * body1, const tVector3 & body1Pos,
                    tScalar allowedDistance,
                    tScalar timescale);
  private:
    void PreApply(tScalar dt);
    bool Apply(tScalar dt);
    void Destroy();

  private:
    tVector3 mBody0Pos;
    tBody * mBody0;
    tVector3 mBody1Pos;
    tBody * mBody1;
    tScalar mAllowedDistance;
    tScalar mTimescale;

    // some values that we calculate once in pre_apply
    tVector3 mWorldPos; ///< average of the two joint positions
    tVector3 mR0; ///< position relative to body 0 (in world space)
    tVector3 mR1;
    tVector3 mVrExtra; ///< extra vel for restoring deviation
  };
}

#endif
