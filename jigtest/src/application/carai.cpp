//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file carai.cpp
//                     
//==============================================================
#include "carai.hpp"
#include "carobject.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tCarAI
//==============================================================
tCarAI::tCarAI(tScalar range) : mRange(range), 
                                mMinTimer(10.0f), 
                                mMaxTimer(30.0f)
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// add_controlled_car
//==============================================================
void tCarAI::AddControlledCar(tCarObject * car)
{
  mCarRecords.push_back(tCarRecord(car,
                                   tVector3(RangedRandom(-mRange, mRange),
                                            RangedRandom(-mRange, mRange),
                                            RangedRandom(-mRange, mRange)),
                                   RangedRandom(mMinTimer, mMaxTimer)));
}

//==============================================================
// remove_controlled_car
//==============================================================
bool tCarAI::RemoveControlledCar(const tCarObject * car)
{
  vector<tCarRecord>::iterator it;
  for (it = mCarRecords.begin() ; 
       it != mCarRecords.end() ;
       ++it)
  {
    if (it->mCar == car)
    {
      mCarRecords.erase(it);
      return true;
    }
  }
  return false;
}


//==============================================================
// choose_new_target
//==============================================================
void tCarAI::ChooseNewTarget(tCarRecord & record)
{
  record.mTarget = tVector3(RangedRandom(-mRange, mRange),
                            RangedRandom(-mRange, mRange),
                            RangedRandom(-mRange, mRange));
}


//==============================================================
// update_controls
//==============================================================
void tCarAI::Update(tScalar dt)
{
  int index;
  
  for (index = 0 ; index < (int) mCarRecords.size() ; ++index)
  {
    // update target according to time
    mCarRecords[index].mTimer -= dt;
    if (mCarRecords[index].mTimer < 0.0f)
    {
      TRACE_FILE_IF(ONCE_2)
        TRACE("choosing new target\n");
      
      ChooseNewTarget(mCarRecords[index]);
      mCarRecords[index].mTimer = RangedRandom(mMinTimer, mMaxTimer);
    }
    
    // check for crashing
    tCarObject * car = mCarRecords[index].mCar;
    if (car->GetBody()->GetOrientation().GetCol(2)[2] < 0.5f)
    {
      mCarRecords[index].mCrashTimer += dt;
    }
    else
    {
      mCarRecords[index].mCrashTimer = 0.0f;
    }
    
    if (mCarRecords[index].mCrashTimer > 4.0f)
    {
      TRACE("AI crash of car %p\n", car);
      // choose a new target in case the target was in a bad place
      ChooseNewTarget(mCarRecords[index]);
      
      car->GetBody()->MoveTo(car->GetBody()->GetPosition() + 
                             tVector3(RangedRandom(-2.0f, 2.0f), 
                                      RangedRandom(-2.0f, 2.0f),
                                      2.0f),
                             tMatrix33::Identity());
      mCarRecords[index].mCrashTimer = 0.0f;
    }
    
    // steering
    const tVector3 & target = mCarRecords[index].mTarget;
    
    while ( (car->GetBody()->GetPosition() - target).GetLength() < 10.0f)
    {
      TRACE_FILE_IF(ONCE_3)
        TRACE("choosing new target due to proximity\n");
      
      ChooseNewTarget(mCarRecords[index]);
    }
    
    tVector3 curDir = car->GetBody()->GetOrientation().GetCol(0);
    curDir[2] = 0.0f;
    curDir.Normalise();
    
    tScalar curHeading = ATan2Deg(curDir[1], curDir[0]);
    
    tVector3 dirToTarget = target - car->GetBody()->GetPosition();
    tScalar targetHeading = ATan2Deg(dirToTarget[1], dirToTarget[0]);
    
    tScalar targetOffset = targetHeading - curHeading;
    
    while (targetOffset < -180.0f)
      targetOffset += 360.0f;
    while (targetOffset > 180.0f)
      targetOffset -= 360.0f;
    
    tScalar steerFrac = targetOffset / 45.0f;
    Limit(steerFrac, SCALAR(-1.0f), SCALAR(1.0f));
    
    // speed
    tScalar idealSpeed = 20.0f;
    tScalar curSpeed = car->GetBody()->GetVelocity().GetLength();
    tScalar accFrac = (idealSpeed - curSpeed) / 5.0f;
    Limit(accFrac, SCALAR(-1.0f), SCALAR(1.0f)); 
    
    // reverse turn if dest is directly behind
    tScalar fwdSpeed = Dot(car->GetBody()->GetVelocity(), curDir);
    if (mCarRecords[index].mAllowReverseTurn)
    {
      if ( ( (fwdSpeed > 0.0f) && (Abs(targetOffset) > 179.0f) ) ||
           ( (fwdSpeed <= 0.0f) && (Abs(targetOffset) > 90.0f) ) )
      {
        accFrac = -1.0f;
        mCarRecords[index].mReversingTimer += dt;
        mCarRecords[index].mNotReversingTimer = 0.0f;
        if (fwdSpeed < -5.0f)
          steerFrac *= -1.0f;
        else
          steerFrac = 0.0f;
      }
      else
      {
        mCarRecords[index].mNotReversingTimer += dt;
      }
    }
    else
    {
      mCarRecords[index].mNotReversingTimer += dt;
      mCarRecords[index].mReversingTimer = 0.0f;
    }
    
    if (mCarRecords[index].mNotReversingTimer > 3.0f)
      mCarRecords[index].mAllowReverseTurn = true;
    else if (mCarRecords[index].mReversingTimer > 10.0f)
      mCarRecords[index].mAllowReverseTurn = false;
    
    
    if (car->GetCar().GetNumWheelsOnFloor() == 2)
      accFrac = -1.0;
    
    // apply steering
    car->GetCar().SetSteer(steerFrac);
    
    // apply acceleration
    car->GetCar().SetAccelerate(accFrac);
  }
}

//==============================================================
// RemoveAllCars
//==============================================================
void tCarAI::RemoveAllCars()
{
  mCarRecords.resize(0);
}
