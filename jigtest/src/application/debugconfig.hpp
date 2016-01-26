//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file debugconfig.hpp
//                     
//==============================================================
#ifndef DEBUGCONFIG_HPP
#define DEBUGCONFIG_HPP

/// global config - for debugging
class tDebugConfig
{
public:
  static bool mIndicateFrozenObjects;
  static bool mRenderCollisionBoundingBoxes;
  static int  mRenderCollisionContacts;
};

#endif
