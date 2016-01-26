//==============================================================
// Copyright (C) 2004 DanmNy Chapman 
//               danmNy@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file chassis.hpp 
//                     
//==============================================================
#ifndef JIGCHASSIS_H
#define JIGCHASSIS_H

#include "../physics/include/body.hpp"
#include "../collision/include/collisionskin.hpp"

namespace JigLib
{
  /// extend tBody to allow for adding on car-specific forces (e.g.
  /// wheel/drive forces) - i.e. when we get asked to add on forces
  /// give the car the opportunity to do stuff
  class tChassisBody : public tBody
  {
  public:
    tChassisBody(class tCar * car) : mCar(car) {}
    
    /// inherited from tBody
    void AddExternalForces(tScalar dt);
    void PostPhysics(tScalar dt);
    
    class tCar * mCar;
  };
  
  /// basic rigid body to represent a single chassis - at the moment 
  /// no moving components. You can inherit from this and pass your 
  /// own version to tCar (TODO - tidy up this)
  class tChassis
  {
  public:
    /// On construction the physical/collision objects are created, but
    /// not registered
    tChassis(class tCar * car);
    
    /// Everything is automatically deregistered if necessart on destruction
    virtual ~tChassis();
    
    /// Set the dimensions of the chassis, specified by the extreme corner points.
    /// This will also call tCar::SetupDefaultWheels();
    virtual void SetDims(tVector3 min, tVector3 max);
    virtual void GetDims(tVector3 & min, tVector3 & max);
    
    /// Register with physics
    void EnableChassis();
    
    /// remove from the physics system
    void DisableChassis();
    
    const tBody & GetBody() const {return mBody;}
    tBody & GetBody() {return mBody;}
    
    const tCollisionSkin & GetSkin() const {return mCollisionSkin;}
    tCollisionSkin & GetSkin() {return mCollisionSkin;}
    
  private:
    tChassisBody mBody;
    tCollisionSkin mCollisionSkin;
    
    tVector3 mDimsMin;
    tVector3 mDimsMax;
  };
}

#endif
