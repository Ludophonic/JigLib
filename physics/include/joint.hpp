//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file joint.hpp 
//                     
//==============================================================
#ifndef JIGJOINT_HPP
#define JIGJOINT_HPP

#include "../physics/include/physicscontroller.hpp"

namespace JigLib
{
  
  /// virtual base class for all joints
  /// All joints are expected to do the following in their constructor:
  /// 1. create whatever constraints are necessary
  /// 2. register these constraints with the physics engine
  class tJoint : public tPhysicsController
  {
  public:
    virtual ~tJoint() {}
  
  private:
    /// Inherited from cJigController
    void UpdateController(tScalar dt) {}
  };

}

#endif
