//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file appconfig.cpp
//                     
//==============================================================
#include "appconfig.hpp"

using namespace JigLib;

int tAppConfig::mCollisionSystemType = 0;
tScalar tAppConfig::mCollisionGridSize = 10.0f;
bool tAppConfig::mEnableFreezing = true;
int tAppConfig::mNumCollisionIterations = 4;
int tAppConfig::mNumContactIterations = 4;
int tAppConfig::mNumPenetrationRelaxationTimesteps = 7;
tScalar tAppConfig::mAllowedPenetration = 0.0001f;
bool tAppConfig::mDoShockStep = false;
tScalar tAppConfig::mPhysicsFrequency = 100.0f;
std::string tAppConfig::mSolverType("accumulated");
int tAppConfig::mMaxPhysicsStepsPerFrame = 2;
bool tAppConfig::mNullPhysicsUpdate = false;
tVector3 tAppConfig::mGravity(0.0f, 0.0f, 0.0f);
tScalar tAppConfig::mProjectileSpeed = 40.0f;
tScalar tAppConfig::mPhysicsCollToll = 0.005f;
tScalar tAppConfig::mPhysicsVelThreshold = 0.4f;
tScalar tAppConfig::mPhysicsAngVelThreshold = 10.0f;
tScalar tAppConfig::mPhysicsPosThreshold = 0.1f;
tScalar tAppConfig::mPhysicsOrientThreshold = 0.2f;
tScalar tAppConfig::mPhysicsDeactivationTime = 1.0f;
bool tAppConfig::mRenderEnable = true;
int tAppConfig::mTextureLevel = 0;
bool tAppConfig::mRenderInterpolate = true;
tScalar tAppConfig::mCamSpeed = 10.0f;
tScalar tAppConfig::mObjectControllerSpeed = 10.0f;
std::string tAppConfig::mSkybox("none");  
int tAppConfig::mMovieInterval = 2;
