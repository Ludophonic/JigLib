//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherestaticmesh.cpp 
//                     
//==============================================================
#include "colldetectspherestaticmesh.hpp"
#include "sphere.hpp"
#include "trianglemesh.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "intersection.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSphereStaticMesh
//==============================================================
tCollDetectSphereStaticMesh::tCollDetectSphereStaticMesh()
  :
  tCollDetectFunctor("SphereMesh", tPrimitive::SPHERE, tPrimitive::TRIANGLEMESH)
{
}

//====================================================================
// CollDetectOverlap
//====================================================================
void tCollDetectSphereStaticMesh::CollDetectSphereStaticMeshOverlap(const tSphere& oldSphere,
                                                                    const tSphere& newSphere,
                                                                    const tTriangleMesh& mesh,
                                                                    const tCollDetectInfo &info,
                                                                    tScalar collTolerance,
                                                                    tCollisionFunctor & collisionFunctor)
{
  const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

  tScalar sphereTolR = collTolerance + newSphere.GetRadius();
  tScalar sphereTolR2 = Sq(sphereTolR);

  static std::vector<tCollPointInfo> collPts;
  collPts.resize(0);
  tVector3 collNormal(0.0f);

  tAABox sphereBox(true);
  sphereBox.AddSphere(newSphere);
  static std::vector<unsigned> potentialTriangles;
  const unsigned numTriangles = mesh.GetTrianglesIntersectingtAABox(potentialTriangles, sphereBox);

  for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
  {
    const tIndexedTriangle& meshTriangle = mesh.GetTriangle(potentialTriangles[iTriangle]);
    tScalar distToCentre = PointPlaneDistance(newSphere.GetPos(), meshTriangle.GetPlane());
    if (distToCentre <= 0.0f)
      continue;
    if (distToCentre >= sphereTolR)
      continue;
    unsigned i0, i1, i2;
    meshTriangle.GetVertexIndices(i0, i1, i2);

    tTriangle triangle(mesh.GetVertex(i0), mesh.GetVertex(i1), mesh.GetVertex(i2));

    tScalar s, t;
    tScalar newD2 = PointTriangleDistanceSq(&s, &t, newSphere.GetPos(), triangle);
    if (newD2 < sphereTolR2)
    {
      // have overlap - but actually report the old intersection
      tScalar oldD2 = PointTriangleDistanceSq(&s, &t, oldSphere.GetPos(), triangle);
      tScalar dist = Sqrt(oldD2);
      tScalar depth = oldSphere.GetRadius() - dist;
      tVector3 collisionN = dist > SCALAR_TINY ? 
        (oldSphere.GetPos() - triangle.GetPoint(s, t)).GetNormalisedSafe() : triangle.GetNormal();
      // since impulse get applied at the old position
      tVector3 pt = oldSphere.GetPos() - oldSphere.GetRadius() * collisionN;
      collPts.push_back(tCollPointInfo(pt - body0Pos, pt - body1Pos, depth)) ;
      collNormal += collisionN;
    }
  }
  if (!collPts.empty())
  {
    collNormal.NormaliseSafe();
    collisionFunctor.CollisionNotify(
      info,
      collNormal,
      &collPts[0],
      collPts.size());
  }
}


//====================================================================
// CollDetectOverlap
//====================================================================
void tCollDetectSphereStaticMesh::CollDetectOverlap(const tCollDetectInfo &info,
                                                    tScalar collTolerance,
                                                    tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  // todo - proper swept test
  const tSphere & oldSphere = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetSphere();
  const tSphere & newSphere = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetSphere();

  // todo - proper swept test
  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();

  CollDetectSphereStaticMeshOverlap(oldSphere, newSphere, mesh, info, collTolerance, collisionFunctor);
}

//====================================================================
// CollDetectSphereStaticMeshSweep
//====================================================================
void tCollDetectSphereStaticMesh::CollDetectSphereStaticMeshSweep(const tSphere& oldSphere,
                                                                  const tSphere& newSphere,
                                                                  const tTriangleMesh& mesh,
                                                                  const tCollDetectInfo &info,
                                                                  tScalar collTolerance,
                                                                  tCollisionFunctor & collisionFunctor)
{
  // really use a swept test - or overlap?
  tVector3 delta = newSphere.GetPos() - oldSphere.GetPos();
  if (delta.GetLengthSq() < Sq(0.5f * newSphere.GetRadius()))
  {
    CollDetectSphereStaticMeshOverlap(oldSphere, newSphere, mesh, info, collTolerance, collisionFunctor);
  }
  else
  {
    const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
    const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

    tScalar sphereTolR = collTolerance + oldSphere.GetRadius();
    tScalar sphereTolR2 = Sq(sphereTolR);

    static std::vector<tCollPointInfo> collPts;
    collPts.resize(0);
    tVector3 collNormal(0.0f);

    tAABox sphereBox(true);
    sphereBox.AddSphere(oldSphere);
    sphereBox.AddSphere(newSphere);

    static std::vector<unsigned> potentialTriangles;
    const unsigned numTriangles = mesh.GetTrianglesIntersectingtAABox(potentialTriangles, sphereBox);

    for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
    {
      // first test the old sphere for being on the wrong side
      const tIndexedTriangle& meshTriangle = mesh.GetTriangle(potentialTriangles[iTriangle]);
      tScalar distToCentreOld = PointPlaneDistance(oldSphere.GetPos(), meshTriangle.GetPlane());
      if (distToCentreOld <= 0.0f)
        continue;
      // now test the new sphere for being clear
      tScalar distToCentreNew = PointPlaneDistance(newSphere.GetPos(), meshTriangle.GetPlane());
      if (distToCentreNew > sphereTolR)
        continue;

      unsigned i0, i1, i2;
      meshTriangle.GetVertexIndices(i0, i1, i2);

      tTriangle triangle(mesh.GetVertex(i0), mesh.GetVertex(i1), mesh.GetVertex(i2));

      // If the old sphere is intersecting, just use that result
      tScalar s, t;
      tScalar d2 = PointTriangleDistanceSq(&s, &t, oldSphere.GetPos(), triangle);
      if (d2 < sphereTolR2)
      {
        tScalar dist = Sqrt(d2);
        tScalar depth = oldSphere.GetRadius() - dist;
        tVector3 triangleN = triangle.GetNormal();
        tVector3 collisionN = dist > SCALAR_TINY ? (oldSphere.GetPos() - triangle.GetPoint(s, t)).GetNormalisedSafe() : triangleN;
        // since impulse gets applied at the old position
        tVector3 pt = oldSphere.GetPos() - oldSphere.GetRadius() * collisionN;
        collPts.push_back(tCollPointInfo(pt - body0Pos, pt - body1Pos, depth));
        collNormal += collisionN;
      }
      else if (distToCentreNew < distToCentreOld)
      {
        // old sphere is not intersecting - do a sweep, but only if the sphere is moving into the
        // triangle
        tVector3 pt, N;
        tScalar depth;
        if (SweptSphereTriangleIntersection(pt, N, depth, 
                                            oldSphere, newSphere, triangle, 
                                            &distToCentreOld, &distToCentreNew, 
                                            EDGE_ALL, CORNER_ALL))
        {
          // collision point etc must be relative to the old position because that's
          //where the impulses are applied
          tScalar dist = Sqrt(d2);
          tScalar depth = oldSphere.GetRadius() - dist;
          tVector3 triangleN = triangle.GetNormal();
          tVector3 collisionN = dist > SCALAR_TINY ? (oldSphere.GetPos() - triangle.GetPoint(s, t)).GetNormalisedSafe() : triangleN;
          // since impulse gets applied at the old position
          tVector3 pt = oldSphere.GetPos() - oldSphere.GetRadius() * collisionN;
          collPts.push_back(tCollPointInfo(pt - body0Pos, pt - body1Pos, depth));
          collNormal += collisionN;
        }
      }
    }
    if (!collPts.empty())
    {
      collNormal.NormaliseSafe();
      collisionFunctor.CollisionNotify(
        info,
        collNormal,
        &collPts[0],
        collPts.size());
    }
  }
}



//====================================================================
// CollDetectSweep
//====================================================================
void tCollDetectSphereStaticMesh::CollDetectSweep(const tCollDetectInfo &info,
                                                  tScalar collTolerance,
                                                  tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  // todo - proper swept test
  const tSphere & oldSphere = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetSphere();
  const tSphere & newSphere = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetSphere();

  // todo - proper swept test
  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();

  CollDetectSphereStaticMeshSweep(oldSphere, newSphere, mesh, info, collTolerance, collisionFunctor);
}


//====================================================================
// CollDetect
//====================================================================
void tCollDetectSphereStaticMesh::CollDetect(const tCollDetectInfo &infoOrig,
                                         tScalar collTolerance,
                                         tCollisionFunctor & collisionFunctor) const
{
  // get the skins in the order that we're expecting
  tCollDetectInfo info(infoOrig);
  if (info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetType() == mType1)
  {
    Swap(info.skin0, info.skin1); 
    Swap(info.iPrim0, info.iPrim1);
  }

  if (info.skin0->GetCollisionSystem() && info.skin0->GetCollisionSystem()->GetUseSweepTests())
    CollDetectSweep(info, collTolerance, collisionFunctor);
  else
    CollDetectOverlap(info, collTolerance, collisionFunctor);
}
