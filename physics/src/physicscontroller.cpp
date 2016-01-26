//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file physicscontroller.cpp 
//                     
//==============================================================
#include "physicscontroller.hpp"
#include "physicssystem.hpp"
#include "trace.hpp"

using namespace JigLib;

//==============================================================
// tPhysicsController
//==============================================================
tPhysicsController::tPhysicsController()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mControllerEnabled = false;
}

//==============================================================
// tPhysicsController
//==============================================================
tPhysicsController::~tPhysicsController()
{
  TRACE_METHOD_ONLY(ONCE_2);
  DisableController();
}

//==============================================================
// Enable
//==============================================================
void tPhysicsController::EnableController()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (true == mControllerEnabled) return;
  mControllerEnabled = true;
  tPhysicsSystem::GetCurrentPhysicsSystem()->AddController(this);
}

//==============================================================
// Disable
//==============================================================
void tPhysicsController::DisableController()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (false == mControllerEnabled) return;
  mControllerEnabled = false;
  tPhysicsSystem::GetCurrentPhysicsSystem()->RemoveController(this);
}
