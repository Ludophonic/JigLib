//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file chassis.cpp 
//                     
//==============================================================
#include "chassis.hpp"
#include "car.hpp"
#include "trace.hpp"

using namespace JigLib;

//==============================================================
// AddExternalForces
//==============================================================
void tChassisBody::AddExternalForces(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  Assert(mCar);
  ClearForces();
  SetForceToGravity();
  mCar->AddExternalForces(dt);
}

//==============================================================
// UpdatePosition
//==============================================================
void tChassisBody::PostPhysics(tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  Assert(mCar);
  mCar->PostPhysics(dt);
}


//==============================================================
// tChassis
//==============================================================
tChassis::tChassis(class tCar * car)
  :
  mBody(car),
  mCollisionSkin(&mBody)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody.SetCollisionSkin(&mCollisionSkin);
  
  tScalar length = 4.0f;
  tScalar width = 2.0f;
  tScalar height = 1.5f;
  
  tVector3 min(-0.5f * length, -width * 0.5f, 0.0f);
  tVector3 max(0.5f * length, width * 0.5f, height);
  
  SetDims(min, max);
}
//==============================================================
// tChassis
//==============================================================
tChassis::~tChassis()
{
  TRACE_METHOD_ONLY(ONCE_2);
  DisableChassis();
}
//==============================================================
// SetDims
//==============================================================
void tChassis::SetDims(tVector3 min, tVector3 max)
{
  TRACE_METHOD_ONLY(ONCE_3);
  mDimsMin = min;
  mDimsMax = max;
  tVector3 sides = max - min;

  // ratio of top/bottom depths
  tScalar topBotRatio = 0.4f;
  
  // the bottom box
  tVector3 max1 = max;
  max1.z -= topBotRatio * sides.z;
  tBox box1(min,
            tMatrix33::Identity(),
            max1 - min);
  
  // the top box
  tVector3 min2 = min;
  min2.z += topBotRatio * sides.z;
  tVector3 max2 = max;
  min2.x += sides.x * 0.05f;
  max2.x -= sides.x * 0.3f;
  min2.y *= 0.9f;
  max2.y *= 0.9f;
  
  tBox box2(min2,
            tMatrix33::Identity(),
            max2 - min2);
  
  mCollisionSkin.RemoveAllPrimitives();
  mCollisionSkin.AddPrimitive(box1, tMaterialTable::NORMAL_NORMAL, tMaterialProperties(0.3f, 0.5f, 0.3f));
  mCollisionSkin.AddPrimitive(box2, tMaterialTable::NORMAL_NORMAL, tMaterialProperties(0.3f, 0.5f, 0.3f));
  
  mBody.mCar->SetupDefaultWheels();
}

//==============================================================
// GetDims
//==============================================================
void tChassis::GetDims(tVector3 & min, tVector3 & max)
{
  min = mDimsMin;
  max = mDimsMax;
}

//==============================================================
// Register
//==============================================================
void tChassis::EnableChassis()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody.EnableBody();
}
//==============================================================
// DeRegister
//==============================================================
void tChassis::DisableChassis()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mBody.DisableBody();
}

