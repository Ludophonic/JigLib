//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintmaxdistance.hpp 
//                     
//==============================================================
#ifndef JIGCONSTRAINT__MAX_DISTANCE_HPP
#define JIGCONSTRAINT__MAX_DISTANCE_HPP

#include "../physics/include/constraint.hpp"
#include "../maths/include/vector3.hpp"

namespace JigLib
{
  class tBody;
  /// Constraints a point on one body to be fixed to a point on another body
  class tConstraintMaxDistance : public tConstraint
  {
  public:
    tConstraintMaxDistance(tBody * body0, const tVector3 & body0Pos,
                           tBody * body1, const tVector3 & body1Pos,
                           tScalar maxDistance);

    /// A default constructor - but make sue you call initialise
    /// before using!
    tConstraintMaxDistance();
 
    ~tConstraintMaxDistance();

    void Initialise(tBody * body0, const tVector3 & body0Pos,
                    tBody * body1, const tVector3 & body1Pos,
                    tScalar maxDistance);
  
  private:
    // inherited virtuals
    void PreApply(tScalar dt);
    bool Apply(tScalar dt);
    void Destroy();

  private:
    // configuration
    tBody * mBody0;
    tBody * mBody1;
    tVector3 mBody0Pos;
    tVector3 mBody1Pos;
    tScalar mMaxDistance;
    // stuff that gets updated
    tVector3 mR0;
    tVector3 mR1;
    tVector3 mWorldPos;
    tVector3 mCurrentRelPos0;
  };
}

#endif
