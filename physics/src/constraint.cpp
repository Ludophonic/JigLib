//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraint.cpp 
//                     
//==============================================================
#include "constraint.hpp"
#include "physicssystem.hpp"
#include "trace.hpp"

using namespace JigLib;

//==============================================================
// tConstraint
//==============================================================
tConstraint::tConstraint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  mConstraintEnabled = false;
}

//==============================================================
// ~tConstraint
//==============================================================
tConstraint::~tConstraint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  DisableConstraint();
}


//==============================================================
// Enable
//==============================================================
void tConstraint::EnableConstraint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (true == mConstraintEnabled) return;
  mConstraintEnabled = true;
  tPhysicsSystem::GetCurrentPhysicsSystem()->AddConstraint(this);
}

//==============================================================
// Disable
//==============================================================
void tConstraint::DisableConstraint()
{
  TRACE_METHOD_ONLY(ONCE_2);
  if (0 == tPhysicsSystem::GetCurrentPhysicsSystem()) return;
  if (false == mConstraintEnabled) return;
  mConstraintEnabled = false;
  tPhysicsSystem::GetCurrentPhysicsSystem()->RemoveConstraint(this);
}


