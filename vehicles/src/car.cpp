//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file car.cpp 
//                     
//==============================================================
#include "car.hpp"
#include "chassis.hpp"
#include "trace.hpp"

using namespace JigLib;

//==============================================================
// tCar
//==============================================================
tCar::tCar(bool FWDrive,
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
           tScalar gravity)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mFWDrive = true;
  mBWDrive = true;
  mMaxSteerAngle = maxSteerAngle;
  mSteerRate = steerRate;
  mWheelSideFriction = wheelSideFriction;
  mWheelFwdFriction = wheelFwdFriction;
  mWheelTravel = wheelTravel;
  mWheelRadius = wheelRadius;
  mWheelZOffset = wheelZOffset;
  mWheelRestingFrac = wheelRestingFrac;
  mWheelDampingFrac = wheelDampingFrac;
  mWheelNumRays = wheelNumRays;
  mDriveTorque = driveTorque;
  mGravity = gravity;
  
  mChassis = 0; // see SetupDefaultWheels
  mChassis = new tChassis(this);
  SetupDefaultWheels();
  
  mDestSteering = mDestAccelerate = mSteering = mAccelerate = mHBrake = 0.0f;
}

//==============================================================
// tCar
//==============================================================
tCar::~tCar()
{
  TRACE_METHOD_ONLY(ONCE_2);
  Assert(mChassis);
  delete mChassis;
  mChassis = 0;
}

//==============================================================
// SetupDefaultWheels
//==============================================================
void tCar::SetupDefaultWheels()
{
  if (!mChassis)
    return; // happens in constructor of tChassis
  tVector3 min, max;
  mChassis->GetDims(min, max);
  
  tScalar mass = mChassis->GetBody().GetMass();
  tScalar mass4 = 0.25f * mass;
  
  tVector3 axis(tVector3::Up());
  // set the resting position to be restingFrac * mWheelTravel
  // todo how do we get gravity before the car is registered with physics?!
  tScalar spring = mass4 * mGravity / (mWheelRestingFrac * mWheelTravel);
  // inertia = 0.5 * m * r^2
  tScalar wheelMass = 0.03f * mass;
  tScalar inertia = 0.5f * Sq(mWheelRadius) * wheelMass;
  // critical damping from (d = damping, k = spring, x = displacement, v = displacement vel, a = displacement acc):
  // a + (d/m) * v + (k/m) * x = 0
  // beta = d/m   w0^2 = k/m
  // critical if sq(beta) = 4*sq(w0)
  // so d = 2 * sqrt(k*m)
  tScalar damping = 2.0f * Sqrt(spring * mass);
  damping *= 0.25f; // assume wheels act together
  damping *= mWheelDampingFrac;  // a bit bouncy
  
  // the wheels aren't quite at the corners
  min.x += 1.1f*mWheelRadius;
  max.x -= 1.2f*mWheelRadius;
  min.y += mWheelRadius * 0.35f;
  max.y -= mWheelRadius * 0.35f;
  tVector3 delta = max - min;
  
  min.z += mWheelZOffset;
  
  tVector3 BRPos(min + tVector3(0.0f, 0.0f, 0.0f));
  tVector3 FRPos(min + tVector3(delta.x, 0.0f, 0.0f));
  tVector3 BLPos(min + tVector3(0.0f, delta.y, 0.0f));
  tVector3 FLPos(min + tVector3(delta.x, delta.y, 0.0f));
  
  mWheels.Resize(4);
  mWheels[WHEEL_BR].Setup(this,
                          BRPos,
                          axis,
                          spring,
                          mWheelTravel,
                          inertia,
                          mWheelRadius,
                          mWheelSideFriction,
                          mWheelFwdFriction,
                          damping,
                          mWheelNumRays);
  mWheels[WHEEL_FR].Setup(this,
                          FRPos,
                          axis,
                          spring,
                          mWheelTravel,
                          inertia,
                          mWheelRadius,
                          mWheelSideFriction,
                          mWheelFwdFriction,
                          damping,
                          mWheelNumRays);
  mWheels[WHEEL_BL].Setup(this,
                          BLPos,
                          axis,
                          spring,
                          mWheelTravel,
                          inertia,
                          mWheelRadius,
                          mWheelSideFriction,
                          mWheelFwdFriction,
                          damping,
                          mWheelNumRays);
  mWheels[WHEEL_FL].Setup(this,
                          FLPos,
                          axis,
                          spring,
                          mWheelTravel,
                          inertia,
                          mWheelRadius,
                          mWheelSideFriction,
                          mWheelFwdFriction,
                          damping,
                          mWheelNumRays);
}

//==============================================================
// SetChassis
//==============================================================
void tCar::SetChassis(class tChassis * chassis)
{
  TRACE_METHOD_ONLY(ONCE_2);
  delete mChassis;
  mChassis = chassis;
}


//==============================================================
// Register
//==============================================================
void tCar::EnableCar()
{
  TRACE_METHOD_ONLY(ONCE_2);
  Assert(mChassis);
  mChassis->EnableChassis();
}

//==============================================================
// DeRegister
//==============================================================
void tCar::DisableCar()
{
  TRACE_METHOD_ONLY(ONCE_2);
  Assert(mChassis);
  mChassis->DisableChassis();
}

//==============================================================
// AddExternalForces
//==============================================================
void tCar::AddExternalForces(tScalar dt)
{
  unsigned numWheels = mWheels.Size();
  for (unsigned i = 0 ; i < numWheels ; ++i)
    mWheels[i].AddForcesToCar(dt);
}

//==============================================================
// PostPhysics
//==============================================================
void tCar::PostPhysics(tScalar dt)
{
  unsigned numWheels = mWheels.Size();
  for (unsigned i = 0 ; i < numWheels ; ++i)
    mWheels[i].Update(dt);
  
  // control inputs
  tScalar deltaAccelerate = dt * 4.0f;
  tScalar deltaSteering = dt * mSteerRate;
  
  // update the actual values
  tScalar dAccelerate = mDestAccelerate - mAccelerate;
  Limit(dAccelerate, -deltaAccelerate, deltaAccelerate);
  mAccelerate += dAccelerate;
  
  tScalar dSteering = mDestSteering - mSteering;
  Limit(dSteering, -deltaSteering, deltaSteering);
  mSteering += dSteering;
  
  // apply these inputs
  tScalar maxTorque = mDriveTorque;
  if (mFWDrive && mBWDrive)
    maxTorque *= 0.5f;
  if (mFWDrive)
  {
    mWheels[WHEEL_FL].AddTorque(maxTorque * mAccelerate);
    mWheels[WHEEL_FR].AddTorque(maxTorque * mAccelerate);
  }
  if (mBWDrive)
  {
    mWheels[WHEEL_BL].AddTorque(maxTorque * mAccelerate);
    mWheels[WHEEL_BR].AddTorque(maxTorque * mAccelerate);
  }
  
  mWheels[WHEEL_BL].SetLock(mHBrake > 0.5f);
  mWheels[WHEEL_BR].SetLock(mHBrake > 0.5f);
  
  // steering angle applies to the inner wheel. The outer one needs to match it
  unsigned inner, outer;
  if (mSteering > 0.0f)
  {
    inner = WHEEL_FL; outer = WHEEL_FR;
  }
  else
  {
    inner = WHEEL_FR; outer = WHEEL_FL;
  }
  
  tScalar alpha = Abs(mMaxSteerAngle * mSteering);
  tScalar angleSgn = mSteering > 0.0f ? 1.0f : -1.0f;
  
  mWheels[inner].SetSteerAngle(angleSgn * alpha);
  tScalar beta;
  if (alpha == 0.0f)
  {
    beta = alpha;
  }
  else
  {
    tScalar dx = (mWheels[WHEEL_FR].GetPos().x - mWheels[WHEEL_BR].GetPos().x);
    tScalar dy = (mWheels[WHEEL_FL].GetPos().y - mWheels[WHEEL_FR].GetPos().y);
    beta = ATan2Deg(dy, dx + (dy / TanDeg(alpha)));
  }
  mWheels[outer].SetSteerAngle(angleSgn* beta);
}

//==============================================================
// GetNumWheelsOnFloor
//==============================================================
int tCar::GetNumWheelsOnFloor() const
{
  int numWheels = mWheels.Size();
  int count = 0;
  for (int i = 0 ; i < numWheels ; ++i)
  {
    if (mWheels[i].GetOnFloor())
      ++count;
  }
  return count;
}

