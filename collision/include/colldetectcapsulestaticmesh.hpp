//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulestaticmesh.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTCAPSULESTATICMESH_HPP
#define JIGCOLLDETECTCAPSULESTATICMESH_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectCapsuleStaticMesh : public tCollDetectFunctor
  {
  public:
    tCollDetectCapsuleStaticMesh();

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

    static void CollDetectCapsuleStaticMeshOverlap(const tCapsule& oldCapsule,
                                                   const tCapsule& newCapsule,
                                                   const class tTriangleMesh& mesh,
                                                   const tCollDetectInfo &info,
                                                   tScalar collTolerance,
                                                   tCollisionFunctor & collisionFunctor);

    static void CollDetectCapsuleStaticMeshSweep(const tCapsule& capsuleOld,
                                                 const class tCapsule& capsuleNew,
                                                 const class tTriangleMesh& mesh,
                                                 const tCollDetectInfo &info,
                                                 tScalar collTolerance,
                                                 tCollisionFunctor & collisionFunctor);
  };
}

#endif
