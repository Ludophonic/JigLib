//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file primitive.cpp 
//                     
//==============================================================
#include "primitive.hpp"
#include "aabox.hpp"
#include "box.hpp"
#include "capsule.hpp"
#include "heightmap.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "trianglemesh.hpp"

using namespace JigLib;


const class tAABox &tPrimitive::GetBoundingBox() const 
{
  return tAABox::HugeBox();
}
