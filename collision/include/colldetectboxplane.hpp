//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxplane.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTBOXPLANE_HPP
#define JIGCOLLDETECTBOXPLANE_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectBoxPlane : public tCollDetectFunctor
  {
  public:
    tCollDetectBoxPlane();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
