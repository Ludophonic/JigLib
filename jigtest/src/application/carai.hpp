//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file carai.hpp 
//                     
//==============================================================
#ifndef CARAI_HPP
#define CARAI_HPP

#include "jiglib.hpp"
#include <vector>

// deals with all the cars - very simple - just makes them move around
// a bit.
class tCarAI
{
public:
  // range indicates how far away from the origin the targets are
  tCarAI(JigLib::tScalar range);
  
  void AddControlledCar(class tCarObject * car);
  
  // rv indicates success
  bool RemoveControlledCar(const tCarObject * car);
  
  void RemoveAllCars();
  
  void Update(JigLib::tScalar dt);
  
private:
  struct tCarRecord
  {
    tCarRecord() {}
    tCarRecord(class tCarObject * car, 
               const JigLib::tVector3 & target, 
               JigLib::tScalar timer) 
      :
      mCar(car), 
      mTarget(target), 
      mTimer(timer), 
      mNotReversingTimer(0),
      mReversingTimer(0), 
      mAllowReverseTurn(false) {}
    
    tCarObject * mCar;
    JigLib::tVector3 mTarget;
    JigLib::tScalar mTimer; // when < zero choose new target
    JigLib::tScalar mCrashTimer; // increments when not OK
    
    // how long we've not been reversing for
    JigLib::tScalar mNotReversingTimer;
    JigLib::tScalar mReversingTimer;
    bool mAllowReverseTurn;    
  };
  
  void ChooseNewTarget(tCarRecord & record);
  
  JigLib::tScalar mRange;
  JigLib::tScalar mMinTimer, mMaxTimer;
  
  std::vector<tCarRecord> mCarRecords;
};

#endif
