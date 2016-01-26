//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspheresphere.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTSPHERESPHERE_HPP
#define JIGCOLLDETECTSPHERESPHERE_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectSphereSphere : public tCollDetectFunctor
  {
  public:
    tCollDetectSphereSphere();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
