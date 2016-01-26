//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsuleplane.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTCAPSULEPLANE_HPP
#define JIGCOLLDETECTCAPSULEPLANE_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectCapsulePlane : public tCollDetectFunctor
  {
  public:
    tCollDetectCapsulePlane();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
