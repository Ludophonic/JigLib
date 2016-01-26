//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxbox.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTBOXBOX_HPP
#define JIGCOLLDETECTBOXBOX_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectBoxBox : public tCollDetectFunctor
  {
  public:
    tCollDetectBoxBox();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
