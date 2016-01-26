//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxheightmap.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTBOXHEIGHTMAP_HPP
#define JIGCOLLDETECTBOXHEIGHTMAP_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectBoxHeightmap : public tCollDetectFunctor
  {
  public:
    tCollDetectBoxHeightmap();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
