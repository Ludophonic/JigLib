//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraintworldpoint.hpp 
//                     
//==============================================================
#ifndef JIGCONSTRAINT_WORLD_POINT_HPP
#define JIGCONSTRAINT_WORLD_POINT_HPP

#include "../physics/include/constraint.hpp"
#include "../maths/include/vector3.hpp"

namespace JigLib
{
  class tBody;
  /// Constrains a point within a rigid body to remain at a fixed
  /// world point
  class tConstraintWorldPoint : public tConstraint
  {
  public:
    /// pointOnBody is in body coords
    tConstraintWorldPoint(tBody * body,
                          const tVector3 & pointOnBody,
                          const tVector3 & worldPosition);

    tConstraintWorldPoint();

    ~tConstraintWorldPoint();

    void Initialise(tBody * body,
                    const tVector3 & pointOnBody,
                    const tVector3 & worldPosition);

    void SetWorldPosition(const tVector3& worldPos) {mWorldPosition = worldPos;}

    tBody* GetBody() const {return mBody;}

  private:
    bool Apply(tScalar dt);
    void Destroy();

  private:
    tBody * mBody;
    tVector3 mPointOnBody;
    tVector3 mWorldPosition;
  };
}

#endif
