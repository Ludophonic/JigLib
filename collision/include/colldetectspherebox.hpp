//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherebox.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTSPHEREBOX_HPP
#define JIGCOLLDETECTSPHEREBOX_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectSphereBox : public tCollDetectFunctor
  {
  public:
    tCollDetectSphereBox();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
