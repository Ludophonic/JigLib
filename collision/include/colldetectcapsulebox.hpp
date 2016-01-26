//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulebox.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTCAPSULEBOX_HPP
#define JIGCOLLDETECTCAPSULEBOX_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectCapsuleBox : public tCollDetectFunctor
  {
  public:
    tCollDetectCapsuleBox();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
