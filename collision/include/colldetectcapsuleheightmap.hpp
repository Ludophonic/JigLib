//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsuleheightmap.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTCAPSULEHEIGHTMAP_HPP
#define JIGCOLLDETECTCAPSULEHEIGHTMAP_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectCapsuleHeightmap : public tCollDetectFunctor
  {
  public:
    tCollDetectCapsuleHeightmap();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
