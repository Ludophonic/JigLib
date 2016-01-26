//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file jiglib.hpp 
//                     
//==============================================================
/*
Copyright (c) 2007 Danny Chapman 
http://www.rowlhouse.co.uk

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

*/
#ifndef JIGLIB_HPP
#define JIGLIB_HPP

// Single header so this is all that applications need to include
// Everything uses namespace JigLib, so nothing under here explictly
// uses the "Jig" prefix.

namespace JigLib
{
  // Version 0.84. Let's face facts... we're not going to reach 1!!
#define JIGLIB_VERSION 84
  inline int GetVersion() {return JIGLIB_VERSION;}
}

#include "../include/jiglibconfig.hpp"

// utils contains misc stuff like tracing
#include "../utils/include/utils.hpp"

// maths contains extra utility maths stuff and vector/matrix classes
#include "../maths/include/maths.hpp"

// geometry defines shapes like planes, boxes etc and functions for
// comparing them
#include "../geometry/include/geometry.hpp"

// collision defines collision skins and collision structures used in
// physics
#include "../collision/include/collision.hpp"

// physics handles, errr, physics
#include "../physics/include/physics.hpp"

// Various vehicles - physics only. 
#include "../vehicles/include/vehicles.hpp"

#endif
