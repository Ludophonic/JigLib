//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file renderobject.cpp 
//                     
//==============================================================
#include "renderobject.hpp"
#include "rendermanager.hpp"

using namespace JigLib;

//==============================================================
// tRenderObject
//==============================================================
tRenderObject::tRenderObject(tRenderObjectType type) 
  : 
  mType(type) 
{
  TRACE_METHOD_ONLY(ONCE_2);
  mRenderManager = 0;
}

//==============================================================
// ~tRenderObject
//==============================================================
tRenderObject::~tRenderObject() 
{
  TRACE_METHOD_ONLY(ONCE_2);
  // don't care if this fails
  if (mRenderManager)
    mRenderManager->RemoveObject(this);
}
