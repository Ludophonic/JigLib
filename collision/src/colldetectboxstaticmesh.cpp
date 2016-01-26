//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxstaticmesh.cpp 
//                     
//==============================================================
#include "colldetectboxstaticmesh.hpp"
#include "mathsmisc.hpp"
#include "box.hpp"
#include "trianglemesh.hpp"
#include "triangle.hpp"
#include "intersection.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

enum {MAX_PTS_PER_BOX_PAIR = 8};


//========================================================
// AddPoint
// if pt is less than Sqrt(combinationDistanceSq) from one of the
// others the original is replaced with the mean of it
// and pt, and false is returned. true means that pt was
// added to pts
//========================================================
static bool AddPoint(tFixedVector<tVector3, MAX_PTS_PER_BOX_PAIR> & pts,
                     const tVector3 & pt,
                     tScalar combinationDistanceSq)
{
  for (unsigned i = pts.Size() ; i-- != 0 ; )
  {
    if (PointPointDistanceSq(pts[i], pt) < combinationDistanceSq)
    {
      pts[i] = 0.5f * (pts[i] + pt);
      return false;
    }
  }
  pts.PushBack(pt);
  return true;
}

//==============================================================
// GetBoxTriangleIntersectionPoints
// Pushes intersection points onto the back of pts. Returns the
// number of points found.
// Points that are close together (compared to 
// combinationDistance) get combined
//==============================================================
static unsigned GetBoxTriangleIntersectionPoints(
  tFixedVector<tVector3, MAX_PTS_PER_BOX_PAIR> & pts,
  const tBox & box,
  const tTriangle& triangle,
  tScalar combinationDistance)
{
  // first intersect each edge of the box with the triangle
  const tBox::tEdge* edges = box.GetAllEdges();
  tVector3 boxPts[8];
  box.GetCornerPoints(boxPts);

  tScalar tS;
  unsigned iEdge;
  for (iEdge = 0 ; iEdge < 12 ; ++iEdge)
  {
    const tBox::tEdge& edge = edges[iEdge];
    tSegment seg(boxPts[edge.ind0], boxPts[edge.ind1] - boxPts[edge.ind0]);
    if (SegmentTriangleIntersection(&tS, 0, 0, seg, triangle))
    {
      AddPoint(pts, seg.GetPoint(tS), Sq(combinationDistance));
    }
  }

  tVector3 pos, n;
  // now each edge of the triangle with the box
  for (iEdge = 0 ; iEdge < 3 ; ++iEdge)
  {
    const tVector3 pt0 = triangle.GetPoint(iEdge);
    const tVector3 pt1 = triangle.GetPoint((iEdge + 1) % 3);
    if (box.SegmentIntersect(tS, pos, n, tSegment(pt0, pt1 - pt0)))
      AddPoint(pts, pos, Sq(combinationDistance));
    if (box.SegmentIntersect(tS, pos, n, tSegment(pt1, pt0 - pt1)))
      AddPoint(pts, pos, Sq(combinationDistance));
  }

  return pts.Size();
}

//========================================================
// Disjoint Returns true if disjoint.  Returns false if intersecting,
// and sets the overlap depth, d scaled by the axis length
//========================================================
static bool Disjoint(tScalar & d,
                     const tVector3 & axis, 
                     const tBox & box, 
                     const tTriangle & triangle,
                     tScalar collTolerance)
{
  tScalar min0, max0, min1, max1;

  box.GetSpan(min0, max0, axis);
  triangle.GetSpan(min1, max1, axis);

  if (min0 > (max1 + collTolerance + SCALAR_TINY) || 
      min1 > (max0 + collTolerance + SCALAR_TINY))
    return true;

  if ( (max0 > max1) && (min1 > min0) )
  {
    // triangle is inside - choose the min dist to move it out
    d = Min(max0 - min1, max1 - min0);
  }
  else if ( (max1 > max0) && (min0 > min1) )
  {
    // box is inside - choose the min dist to move it out
    d = Min(max1 - min0, max0 - min1);
  }
  else
  {
    // objects overlap
    d  = (max0 < max1) ? max0 : max1;
    d -= (min0 > min1) ? min0 : min1;
  }

  return false;
}

//====================================================================
// DoOverlapBoxTest
//====================================================================
static bool DoOverlapBoxTriangleTest(const tBox & oldBox, 
                                     const tBox & newBox, 
                                     const tIndexedTriangle& triangle,
                                     const tTriangleMesh& mesh,
                                     const tCollDetectInfo &info,
                                     tScalar collTolerance,
                                     tCollisionFunctor & collisionFunctor)
{
  const tMatrix33 & dirs0 = newBox.GetOrient();
  tTriangle tri(
    mesh.GetVertex(triangle.GetVertexIndex(0)),
    mesh.GetVertex(triangle.GetVertexIndex(1)),
    mesh.GetVertex(triangle.GetVertexIndex(2)));
  tVector3 triEdge0 = (tri.GetPoint(1) - tri.GetPoint(0)).Normalise();  
  tVector3 triEdge1 = (tri.GetPoint(2) - tri.GetPoint(1)).Normalise();  
  tVector3 triEdge2 = (tri.GetPoint(0) - tri.GetPoint(2)).Normalise();  
  const tVector3& triNormal = triangle.GetPlane().GetN();

  // the 15 potential separating axes
  static const int numAxes = 13;
  tVector3 axes[numAxes] = 
    {
      triNormal,
      dirs0[0],
      dirs0[1],
      dirs0[2],
      Cross(dirs0[0], triEdge0),
      Cross(dirs0[0], triEdge1),
      Cross(dirs0[0], triEdge2),
      Cross(dirs0[1], triEdge0),
      Cross(dirs0[1], triEdge1),
      Cross(dirs0[1], triEdge2),
      Cross(dirs0[2], triEdge0),
      Cross(dirs0[2], triEdge1),
      Cross(dirs0[2], triEdge2),
    };

  // the overlap depths along each axis
  tScalar overlapDepths[numAxes];

  // see if the boxes are separate along any axis, and if not keep a 
  // record of the depths along each axis
  int i;
  for (i = 0 ; i < numAxes ; ++i)
  {
    overlapDepths[i] = 1.0f;
    if (Disjoint(overlapDepths[i], axes[i], newBox, tri, collTolerance))
      return false;
  }

  //-----------------------------------------------------------------
  // The box overlap, find the separation depth closest to 0.
  //-----------------------------------------------------------------
  tScalar minDepth = SCALAR_HUGE;
  int minAxis = -1;

  for(i = 0; i < numAxes; ++i)
  {
    //-----------------------------------------------------------------
    // If we can't normalise the axis, skip it
    //-----------------------------------------------------------------
    tScalar l2 = axes[i].GetLengthSq();
    if (l2 < SCALAR_TINY)
      continue;

    //-----------------------------------------------------------------
    // Normalise the separation axis and the depth
    //-----------------------------------------------------------------
    tScalar invl = 1.0f / Sqrt(l2);
    axes[i] *= invl;
    overlapDepths[i] *= invl;

    //-----------------------------------------------------------------
    // If this axis is the minimum, select it
    //-----------------------------------------------------------------
    if (overlapDepths[i] < minDepth)
    {
      minDepth = overlapDepths[i];
      minAxis = i;
    }
  }

  if (minAxis == -1)
    return false;

  //-----------------------------------------------------------------
  // Make sure the axis is facing towards the 0th box.
  // if not, invert it
  //-----------------------------------------------------------------
  tVector3 D = newBox.GetCentre() - tri.GetCentre();
  tVector3 & N = axes[minAxis];
  tScalar depth = overlapDepths[minAxis];

  if (Dot(D, N) < 0.0f)
    N.Negate();

  const tVector3& boxOldPos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& boxNewPos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetPosition() : tVector3::Zero();
  const tVector3& meshPos   = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();
  
  static tFixedVector<tVector3, MAX_PTS_PER_BOX_PAIR> pts;
  pts.Clear();

  static const tScalar combinationDist = 0.05f;
  GetBoxTriangleIntersectionPoints(pts, newBox, tri, depth + combinationDist);

  // adjust the depth 
  tVector3 delta = boxNewPos - boxOldPos;
  tScalar oldDepth = depth + Dot(delta, N);

  static tFixedVector<tCollPointInfo, MAX_PTS_PER_BOX_PAIR> collPts;
  collPts.Clear();

  // report collisions
  unsigned numPts = pts.Size();
  if (numPts > 0)
  {
    // adjust positions
    for (unsigned i = 0 ; i < numPts ; ++i)
    {
      collPts.PushBack(tCollPointInfo(pts[i] - boxNewPos, pts[i] - meshPos, oldDepth));
    }

    collisionFunctor.CollisionNotify(
      info,
      N,
      &collPts[0],
      numPts);
    return true;
  }
  else
  {
    return false;
  }
}


//==============================================================
// tCollDetectBoxStaticMesh
//==============================================================
tCollDetectBoxStaticMesh::tCollDetectBoxStaticMesh()
  :
  tCollDetectFunctor("BoxMesh", tPrimitive::BOX, tPrimitive::TRIANGLEMESH)
{
}

//====================================================================
// CollDetectBoxStaticMeshOverlap
//====================================================================
static bool CollDetectBoxStaticMeshOverlap(const tBox& oldBox,
                                           const tBox& newBox,
                                           const class tTriangleMesh& mesh,
                                           const tCollDetectInfo &info,
                                           tScalar collTolerance,
                                           tCollisionFunctor & collisionFunctor)
{
  tScalar boxRadius = newBox.GetBoundingRadiusAboutCentre();
  tVector3 boxCentre = newBox.GetCentre();

  tAABox boxBox(true);
  boxBox.AddBox(newBox);
  static std::vector<unsigned> potentialTriangles;
  const unsigned numTriangles = mesh.GetTrianglesIntersectingtAABox(potentialTriangles, boxBox);

  bool collision = false;
  for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
  {
    const tIndexedTriangle& meshTriangle = mesh.GetTriangle(potentialTriangles[iTriangle]);

    // quick early test
    tScalar dist = PointPlaneDistance(boxCentre, meshTriangle.GetPlane());
    if (dist > boxRadius || dist < 0.0f)
      continue;

    if (DoOverlapBoxTriangleTest(
          oldBox, newBox, meshTriangle, mesh,
          info,
          collTolerance,
          collisionFunctor))
    {
      collision = true;
    }
  }
  return collision;
}

//==============================================================
// CollDetectOverlap
//==============================================================
void tCollDetectBoxStaticMesh::CollDetectOverlap(const tCollDetectInfo &info,
                                                 tScalar collTolerance,
                                                 tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();

  const tBox & oldBox = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetBox();
  const tBox & newBox = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetBox();

  CollDetectBoxStaticMeshOverlap(oldBox, newBox, mesh, info, collTolerance, collisionFunctor);
}


//==============================================================
// CollDetectOverlap
//==============================================================
void tCollDetectBoxStaticMesh::CollDetectSweep(const tCollDetectInfo &info,
                                               tScalar collTolerance,
                                               tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);

  // todo - proper swept test
  // note - mesh is static and its triangles are in world space
  const tTriangleMesh & mesh = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetTriangleMesh();
  static std::vector<unsigned> potentialTriangles;

  const tBox & oldBox = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetBox();
  const tBox & newBox = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetBox();

  tVector3 oldCentre = oldBox.GetCentre();
  tVector3 newCentre = newBox.GetCentre();
  tVector3 delta = newCentre - oldCentre;
  tScalar boxMinLen = 0.5f * Min(newBox.GetSideLengths().x, newBox.GetSideLengths().y, newBox.GetSideLengths().z);
  int nPositions = 1 + (int) (delta.GetLength() / boxMinLen);
  // limit the max positions...
  if (nPositions > 50)
  {
    TRACE_FILE_IF(ONCE_1)
      TRACE("Warning - clamping max positions in swept box test\n");
    nPositions = 50;
  }
  if (nPositions == 1)
  {
    CollDetectBoxStaticMeshOverlap(oldBox, newBox, mesh, info, collTolerance, collisionFunctor);
  }
  else
  {
    tAABox boxBox(true);
    boxBox.AddBox(oldBox);
    boxBox.AddBox(newBox);
    static std::vector<unsigned> potentialTriangles;
    const unsigned numTriangles = mesh.GetTrianglesIntersectingtAABox(potentialTriangles, boxBox);
    if (numTriangles > 0)
    {
      for (int i = 0 ; i <= nPositions ; ++i)
      {
        tScalar frac = ((tScalar) i) / nPositions;
        tVector3 centre = oldCentre + frac * delta;
        tMatrix33 orient = (1.0f - frac) * oldBox.GetOrient() + frac * newBox.GetOrient();
        orient.Orthonormalise();
        tBox box(centre - 0.5f * orient * newBox.GetSideLengths(), orient, newBox.GetSideLengths());
        // ideally we'd break if we get one collision... but that stops us getting multiple collisions
        // when we enter a corner (two walls meeting) - can let us pass through
        CollDetectBoxStaticMeshOverlap(oldBox, box, mesh, info, collTolerance, collisionFunctor);
      }
    }
  }
}

//====================================================================
// CollDetect
//====================================================================
void tCollDetectBoxStaticMesh::CollDetect(const tCollDetectInfo &infoOrig,
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
