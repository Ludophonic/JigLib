//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectsphereheightmap.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTSPHEREHEIGHTMAP_HPP
#define JIGCOLLDETECTSPHEREHEIGHTMAP_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectSphereHeightmap : public tCollDetectFunctor
  {
  public:
    tCollDetectSphereHeightmap();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
