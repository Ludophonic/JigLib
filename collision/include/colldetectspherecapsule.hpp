//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherecapsule.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTSPHERECAPSULE_HPP
#define JIGCOLLDETECTSPHERECAPSULE_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectSphereCapsule : public tCollDetectFunctor
  {
  public:
    tCollDetectSphereCapsule();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
