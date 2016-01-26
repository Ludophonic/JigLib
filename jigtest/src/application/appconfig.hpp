//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file appconfig.hpp 
//                     
//==============================================================
#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include "jiglib.hpp"

#include <string>

/// helper class/struct internal to tJigTestApp
class tAppConfig
{
public:
  // 0 is brute, 1 is grid
  static int mCollisionSystemType;
  static JigLib::tScalar mCollisionGridSize;
  static bool mEnableFreezing;
  static int mNumCollisionIterations;
  static int mNumContactIterations;
  static int mNumPenetrationRelaxationTimesteps;
  static JigLib::tScalar mAllowedPenetration;
  static bool mDoShockStep;
  static JigLib::tScalar mPhysicsFrequency;
  static std::string mSolverType;
  static JigLib::tVector3 mGravity;
  static JigLib::tScalar mProjectileSpeed;
  static JigLib::tScalar mPhysicsCollToll;
  static JigLib::tScalar mPhysicsVelThreshold;
  static JigLib::tScalar mPhysicsAngVelThreshold;
  static JigLib::tScalar mPhysicsPosThreshold;
  static JigLib::tScalar mPhysicsOrientThreshold;
  static JigLib::tScalar mPhysicsDeactivationTime;
  static int mMaxPhysicsStepsPerFrame;
	static bool mNullPhysicsUpdate;
  static bool mRenderEnable;
  static int mTextureLevel;
  static bool mRenderInterpolate;
  static JigLib::tScalar mCamSpeed;
  static JigLib::tScalar mObjectControllerSpeed;
  static std::string mSkybox;
  static int mMovieInterval;
};

#endif
