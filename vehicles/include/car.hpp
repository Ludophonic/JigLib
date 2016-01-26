//==============================================================
// Copyright (C) 2004 DanmNy Chapman 
//               danmNy@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file car.hpp 
//                     
//==============================================================
#ifndef JIGCAR_H
#define JIGCAR_H

#include "../vehicles/include/wheel.hpp"
#include "../maths/include/precision.hpp"
#include "../utils/include/fixedvector.hpp"

namespace JigLib
{
  class tCar
  {
  public:
    /// On construction the physical/collision objects are created, but
    /// not registered
    tCar(bool FWDrive,
         bool RWDrive,
         tScalar maxSteerAngle,
         tScalar steerRate,
         tScalar wheelSideFriction,
         tScalar wheelFwdFriction,
         tScalar wheelTravel,
         tScalar wheelRadius,
         tScalar wheelZOffset,
         tScalar wheelRestingFrac,
         tScalar wheelDampingFrac,
         int     wheelNumRays,
         tScalar driveTorque,
         tScalar gravity);
    
    /// Everything is automatically deregistered if necessart on destruction
    ~tCar();
    
    /// There will always be a chassis
    const class tChassis & GetChassis() const {return *mChassis;}
    class tChassis & GetChassis() {return *mChassis;}
    
    /// You can replace the default chassis with one of your own. If you do
    /// it must be created on the heap, and tCar takes ownership (and will
    /// delete it)
    void SetChassis(class tChassis * chassis);
    
    /// Register with physics
    void EnableCar();
    
    /// remove from the physics system
    void DisableCar();
    
    /// We get told to add on drive/wheel forces etc
    void AddExternalForces(tScalar dt);
    
    /// Update stuff at the end of physics
    void PostPhysics(tScalar dt);
    
    enum tWheelId {WHEEL_BR, WHEEL_FR, WHEEL_BL, WHEEL_FL, MAX_WHEELS};
    /// Allow access to all the wheels
    tFixedVector<tWheel, MAX_WHEELS> & GetWheels() {return mWheels;}
    
    /// sets up some sensible wheels based on the chassis
    void SetupDefaultWheels();
    
    /// Sets back-wheel drive
    void SetBWDrive(bool drive) {mBWDrive = drive;}
    /// Sets front-wheel drive
    void SetFWDrive(bool drive) {mFWDrive = drive;}
    
    // control - values -1/0 to 1
    void SetAccelerate(tScalar val) {mDestAccelerate = val;}
    void SetSteer(tScalar val) {mDestSteering = val;}
    void SetHBrake(tScalar val) {mHBrake = val;}
    
    int GetNumWheelsOnFloor() const;
    
  private:
    class tChassis * mChassis;
    tFixedVector<tWheel, MAX_WHEELS> mWheels;
    
    bool mFWDrive;
    bool mBWDrive;
    tScalar mMaxSteerAngle;
    tScalar mSteerRate;
    tScalar mWheelSideFriction;
    tScalar mWheelFwdFriction;
    tScalar mWheelTravel;
    tScalar mWheelRadius;
    tScalar mWheelZOffset;
    tScalar mWheelRestingFrac;
    tScalar mWheelDampingFrac;
    int     mWheelNumRays;
    tScalar mDriveTorque;
    tScalar mGravity;
    
    // control stuff
    tScalar mDestSteering; // +1 for left, -1 for right
    tScalar mDestAccelerate; // +1 for acc, -1 for brake
    
    tScalar mSteering;
    tScalar mAccelerate;
    tScalar mHBrake;
  };
}

#endif
