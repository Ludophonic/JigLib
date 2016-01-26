//==============================================================
// Copyright (C) 2004 DanmNy Chapman 
//               danmNy@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file wheel.hpp 
//                     
//==============================================================
#ifndef JIGWHEEL_HPP
#define JIGWHEEL_HPP

#include "../maths/include/vector3.hpp"

namespace JigLib
{
  class tWheel
  {
  public:
    tWheel();
    void Setup(class tCar * car,
               const tVector3 & pos, ///< position relative to car, in car's space
               const tVector3 & axisUp, ///< in car's space
               tScalar spring,  ///< force per suspension offset
               tScalar travel,  ///< suspension travel upwards
               tScalar inertia, ///< inertia about the axel
               tScalar radius,
               tScalar sideFriction,
               tScalar fwdFriction,
               tScalar damping,
               int numRays);
    
    bool GetOnFloor() const {return mLastOnFloor;}
    
    // Adds the forces die to this wheel to the parent. Return value indicates if it's
    // on the ground.
    bool AddForcesToCar(tScalar dt);
    
    /// Updates the rotational state etc
    void Update(tScalar dt);
    
    /// sets everything that varies to a default
    void Reset();
    
    /// get steering angle in degrees
    tScalar GetSteerAngle() const {return mSteerAngle;}
    void SetSteerAngle(tScalar steer) {mSteerAngle = steer;}
    
    /// lock/unlock the wheel
    void SetLock(bool lock) {mLocked = lock;}
    
    /// power
    void AddTorque(tScalar torque) {mDriveTorque += torque;}
    
    /// the basic origin position
    const tVector3 & GetPos() const {return mPos;}
    
    /// the suspension axis in the car's frame
    const tVector3 & GetLocalAxisUp() const {return mAxisUp;}
    
    /// wheel radius
    tScalar GetRadius() const {return mRadius;}
    
    /// the displacement along our up axis
    tScalar GetDisplacement() const {return mDisplacement;}
    
    tScalar GetAxisAngle() const {return mAxisAngle;}
    
  private:
    class tCar * mCar;
    
    /// local mount position
    tVector3 mPos;
    tVector3 mAxisUp;
    tScalar mSpring;
    tScalar mTravel;
    tScalar mInertia;
    tScalar mRadius;
    tScalar mSideFriction;
    tScalar mFwdFriction;
    tScalar mDamping;
    int     mNumRays;
    
    // things that change 
    tScalar mAngVel;
    tScalar mSteerAngle;
    tScalar mTorque;
    tScalar mDriveTorque;
    tScalar mAxisAngle;
    tScalar mDisplacement; // = mTravel when fully compressed
    tScalar mUpSpeed; // speed relative to the car
    bool   mLocked;
    // last frame stuff
    tScalar mLastDisplacement;
    bool   mLastOnFloor;
    
    /// used to estimate the friction
    tScalar mAngVelForGrip;
  };
  
}
#endif
