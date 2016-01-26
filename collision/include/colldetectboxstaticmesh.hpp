//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxstaticmesh.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTBOXSTATICMESH_HPP
#define JIGCOLLDETECTBOXSTATICMESH_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectBoxStaticMesh : public tCollDetectFunctor
  {
  public:
    tCollDetectBoxStaticMesh();
    
    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;
  private:
    void CollDetectOverlap(const tCollDetectInfo &info,
                           tScalar collTolerance,
                           tCollisionFunctor & collisionFunctor) const;
    void CollDetectSweep(const tCollDetectInfo &info,
                         tScalar collTolerance,
                         tCollisionFunctor & collisionFunctor) const;
  };
}

#endif
