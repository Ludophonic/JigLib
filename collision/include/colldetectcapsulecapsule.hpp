//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulecapsule.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTCAPSULECAPSULE_HPP
#define JIGCOLLDETECTCAPSULECAPSULE_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectCapsuleCapsule : public tCollDetectFunctor
  {
  public:
    tCollDetectCapsuleCapsule();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
