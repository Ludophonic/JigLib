//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulestaticmesh.cpp 
//                     
//==============================================================
#include "colldetectcapsulestaticmesh.hpp"
#include "colldetectspherestaticmesh.hpp"
#include "capsule.hpp"
#include "trianglemesh.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectCapsuleStaticMesh
//==============================================================
tCollDetectCapsuleStaticMesh::tCollDetectCapsuleStaticMesh() :
tCollDetectFunctor("CapsuleMesh", tPrimitive::CAPSULE, tPrimitive::TRIANGLEMESH)
{
}

//==============================================================
// CollDetectCapsuleStaticMeshOverlap
//==============================================================
void tCollDetectCapsuleStaticMesh::CollDetectCapsuleStaticMeshOverlap(const tCapsule& oldCapsule,
                                                                      const tCapsule& newCapsule,
                                                                      const tTriangleMesh& mesh,
                                                                      const tCollDetectInfo &info,
                                                                      tScalar collTolerance,
                                                                      tCollisionFunctor & collisionFunctor)
{
  const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

  tScalar capsuleTolR = collTolerance + newCapsule.GetRadius();
  tScalar capsuleTolR2 = Sq(capsuleTolR);

  static std::vector<tCollPointInfo> collPts;
  collPts.resize(0);
  tVector3 collNormal(0.0f);

  tAABox capsuleBox(true);
  capsuleBox.AddCapsule(newCapsule);
  static std::vector<unsigned> potentialTriangles;
  const unsigned numTriangles = mesh.GetTrianglesIntersectingtAABox(potentialTriangles, capsuleBox);

  for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
  {
    const tIndexedTriangle& meshTriangle = mesh.GetTriangle(potentialTriangles[iTriangle]);
    const tVector3& capsuleStart = newCapsule.GetPos();
    const tVector3  capsuleEnd = newCapsule.GetEnd();
    tScalar distToStart = PointPlaneDistance(capsuleStart, meshTriangle.GetPlane());
    tScalar distToEnd = PointPlaneDistance(capsuleEnd, meshTriangle.GetPlane());
    if (distToStart > capsuleTolR && distToEnd > capsuleTolR)
      continue;
    if (distToStart < 0.0f && distToEnd < 0.0f)
      continue;

    unsigned i0, i1, i2;
    meshTriangle.GetVertexIndices(i0, i1, i2);

    tTriangle triangle(mesh.GetVertex(i0), mesh.GetVertex(i1), mesh.GetVertex(i2));
    tSegment seg(capsuleStart, capsuleEnd - capsuleStart);

    tScalar tS, tT0, tT1;
    tScalar d2 = SegmentTriangleDistanceSq(&tS, &tT0, &tT1, seg, triangle);

    if (d2 < capsuleTolR2)
    {
      const tVector3& oldCapsuleStart = oldCapsule.GetPos();
      const tVector3  oldCapsuleEnd = oldCapsule.GetEnd();
      tSegment oldSeg(oldCapsuleStart, oldCapsuleEnd - oldCapsuleStart);
      d2 = SegmentTriangleDistanceSq(&tS, &tT0, &tT1, oldSeg, triangle);
      // report result from old position
      tScalar dist = Sqrt(d2);
      tScalar depth = oldCapsule.GetRadius() - dist;
      tVector3 pt = triangle.GetPoint(tT0, tT1);
      tVector3 collisionN = d2 > SCALAR_TINY ? 
        (oldSeg.GetPoint(tS) - pt).GetNormalisedSafe() : 
        meshTriangle.GetPlane().GetN();
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

//==============================================================
// CollDetectOverlap
//==============================================================
void tCollDetectCapsuleStaticMesh::CollDetectOverlap(const tCollDetectInfo &info,
                                         tScalar collTolerance,
                                         tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  const tCapsule & oldCapsule = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetCapsule();
  const tCapsule & newCapsule = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetCapsule();

  // todo - proper swept test
  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();

  CollDetectCapsuleStaticMeshOverlap(oldCapsule, newCapsule, mesh, info, collTolerance, collisionFunctor);
}

//====================================================================
// CollDetectCapsuleStaticMeshSweep
//====================================================================
void tCollDetectCapsuleStaticMesh::CollDetectCapsuleStaticMeshSweep(const tCapsule& oldCapsule,
                                                                    const tCapsule& newCapsule,
                                                                    const tTriangleMesh& mesh,
                                                                    const tCollDetectInfo &info,
                                                                    tScalar collTolerance,
                                                                    tCollisionFunctor & collisionFunctor)
{
  // really use a swept test - or overlap?
  tVector3 delta = newCapsule.GetPos() - oldCapsule.GetPos();
  if (delta.GetLengthSq() < Sq(0.5f * newCapsule.GetRadius()))
  {
    CollDetectCapsuleStaticMeshOverlap(oldCapsule, newCapsule, mesh, info, collTolerance, collisionFunctor);
  }
  else
  {
    tScalar capsuleLen = oldCapsule.GetLength();
    tScalar capsuleRadius = oldCapsule.GetRadius();
    int nSpheres = 2 + (int) (capsuleLen / (2.0f * oldCapsule.GetRadius()));
    for (int iSphere = 0 ; iSphere < nSpheres ; ++iSphere)
    {
      tScalar offset = ((tScalar) iSphere) * capsuleLen / (nSpheres - 1.0f);
      tSphere oldSphere(oldCapsule.GetPos() + oldCapsule.GetOrient().GetLook() * offset, capsuleRadius);
      tSphere newSphere(newCapsule.GetPos() + newCapsule.GetOrient().GetLook() * offset, capsuleRadius);
      tCollDetectSphereStaticMesh::CollDetectSphereStaticMeshSweep(
        oldSphere, newSphere, mesh, info, collTolerance, collisionFunctor);
    }
  }
}

//====================================================================
// CollDetectSweep
//====================================================================
void tCollDetectCapsuleStaticMesh::CollDetectSweep(const tCollDetectInfo &info,
                                         tScalar collTolerance,
                                         tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  const tCapsule & oldCapsule = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetCapsule();
  const tCapsule & newCapsule = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetCapsule();

  // todo - proper swept test
  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();

  CollDetectCapsuleStaticMeshSweep(oldCapsule, newCapsule, mesh, info, collTolerance, collisionFunctor);
}

//====================================================================
// CollDetect
//====================================================================
void tCollDetectCapsuleStaticMesh::CollDetect(const tCollDetectInfo &infoOrig,
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
