//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherestaticmesh.hpp 
//                     
//==============================================================
#ifndef JIGCOLLDETECTSPHERESTATICMESH_HPP
#define JIGCOLLDETECTSPHERESTATICMESH_HPP

#include "../collision/include/collisionsystem.hpp"
namespace JigLib
{
  class tCollDetectSphereStaticMesh : public tCollDetectFunctor
  {
  public:
    tCollDetectSphereStaticMesh();

    void CollDetect(const tCollDetectInfo &info,
                    tScalar collTolerance,
                    tCollisionFunctor & collisionFunctor) const;

    /// Helper function that will be useful for other classes - collides
    /// a sphere against a static mesh and registers collisions based on the 
    /// skins (skin1 is the mesh, skin0 could be any type if you're using
    /// spheres to approximate it).
    static void CollDetectSphereStaticMeshSweep(const class tSphere& sphereOld,
                                                const class tSphere& sphereNew,
                                                const class tTriangleMesh& mesh,
                                                const tCollDetectInfo &info,
                                                tScalar collTolerance,
                                                tCollisionFunctor & collisionFunctor);

  private:
    void CollDetectOverlap(const tCollDetectInfo &info,
                           tScalar collTolerance,
                           tCollisionFunctor & collisionFunctor) const;

    void CollDetectSweep(const tCollDetectInfo &info,
                         tScalar collTolerance,
                         tCollisionFunctor & collisionFunctor) const;

    static void CollDetectSphereStaticMeshOverlap(const class tSphere& oldSphere,
                                                  const class tSphere& newSphere,
                                                  const class tTriangleMesh& mesh,
                                                  const tCollDetectInfo &info,
                                                  tScalar collTolerance,
                                                  tCollisionFunctor & collisionFunctor);
  };
}

#endif
