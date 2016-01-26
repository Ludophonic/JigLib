//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file transform.hpp
//                     
//==============================================================
#ifndef JIGTRANSFORM_HPP
#define JIGTRANSFORM_HPP

#include "../maths/include/precision.hpp"
#include "../utils/include/trace.hpp"
#include "../utils/include/assert.hpp"
#include "../maths/include/vector3.hpp"
#include "../maths/include/matrix33.hpp"
#include "../maths/include/mathsmisc.hpp"

namespace JigLib
{
  /// Combines position and orientation for 3D operations
  class tTransform3
  {
  public:
    /// public access
    tVector3 position;
    tMatrix33 orientation;

    /// default ctor does not initialise
    tTransform3() {}
    enum tIdentity {IDENTITY};
    tTransform3(tIdentity) : position(tVector3::ZERO), orientation(tMatrix33::IDENTITY) {}
    tTransform3(const tVector3 vec, const tMatrix33 mat) : position(vec), orientation(mat) {}
    
    friend tTransform3 operator*(const tTransform3 &lhs, const tTransform3 &rhs);
    friend void MultTranform3(tTransform3 &out, const tTransform3 &lhs, const tTransform3 &rhs);
  };

  /// Combines rate of change of position and orientation in 3D
  class tTransform3Rate
  {
  public:
    tTransform3Rate() {}
    tTransform3Rate(const tVector3 &vel, const tVector3 &angVel) : velocity(vel), angVelocity(angVel) {}
    void SetToZero() {velocity.SetToZero(); angVelocity.SetToZero();}
    /// public access
    tVector3 velocity;
    tVector3 angVelocity;
  };

  /// Concatenate transforms
  inline void MultTransform3(tTransform3 &out, const tTransform3 &lhs, const tTransform3 &rhs)
  {
    out.orientation = lhs.orientation * rhs.orientation;
    ApplyTransformation(out.position, lhs.position, lhs.orientation, rhs.position);
  }

  /// Concatenate transforms
  inline tTransform3 operator*(const tTransform3 &lhs, const tTransform3 &rhs)
  {
    tTransform3 result;
    MultTransform3(result, lhs, rhs);
    return result;
  }

  inline tTransform3Rate operator+(const tTransform3Rate &rate1, const tTransform3Rate &rate2)
  {
    return tTransform3Rate(rate1.velocity + rate2.velocity, rate1.angVelocity + rate2.angVelocity);
  }

  /// Applies Euler integration to a transform
  inline void ApplyTransformRate(tTransform3 &transform, const tTransform3Rate &rate, const tScalar dt)
  {
    transform.position += dt * rate.velocity;
    static bool doDirAng = true;
    if (doDirAng)
    {
      tVector3 dir = rate.angVelocity;
      tScalar ang = dir.GetLength();
      if (ang > 0.0f)
      {
        dir /= ang;
        ang *= dt;
        tMatrix33 rot = RotationMatrix(Rad2Deg(ang), dir);
        transform.orientation = rot * transform.orientation;
      }
    }
    else
    {
      transform.orientation += dt * (tMatrix33(SCALAR(0.0), -rate.angVelocity[2],  rate.angVelocity[1],
                                       rate.angVelocity[2],          SCALAR(0.0), -rate.angVelocity[0],
                                      -rate.angVelocity[1],  rate.angVelocity[0],          SCALAR(0.0)) * transform.orientation);
    }
    transform.orientation.Orthonormalise();
  }
}

#endif
