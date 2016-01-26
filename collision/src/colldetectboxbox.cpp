//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxbox.cpp 
//                     
//==============================================================
#include "colldetectboxbox.hpp"
#include "mathsmisc.hpp"
#include "fixedvector.hpp"
#include "body.hpp"
#include "plane.hpp"
#include "overlap.hpp"

#include <vector>
using namespace std;
using namespace JigLib;

enum {MAX_PTS_PER_BOX_PAIR = 9};

struct tContactPoint
{
  tContactPoint(const tVector3 &pos = tVector3::Zero()) : pos(pos), count(1) {}
  tVector3 pos;
  tScalar count;
};

//==============================================================
// tCollDetectBoxBox
//==============================================================
tCollDetectBoxBox::tCollDetectBoxBox() : 
tCollDetectFunctor("BoxBox", tPrimitive::BOX, tPrimitive::BOX)
{
}

//========================================================
// Disjoint Returns true if disjoint.  Returns false if intersecting,
// and sets the overlap depth, d scaled by the axis length
//========================================================
static bool Disjoint(tScalar & d,
                     const tVector3 & axis, 
                     const tBox & box0, 
                     const tBox & box1,
                     tScalar collTolerance)
{
  tScalar min0, max0, min1, max1;
  
  box0.GetSpan(min0, max0, axis);
  box1.GetSpan(min1, max1, axis);
  
  if (min0 > (max1 + collTolerance + SCALAR_TINY) || 
      min1 > (max0 + collTolerance + SCALAR_TINY))
    return true;
  
  if ( (max0 > max1) && (min1 > min0) )
  {
    // box1 is inside - choose the min dist to move it out
    d = Min(max0 - min1, max1 - min0);
  }
  else if ( (max1 > max0) && (min0 > min1) )
  {
    // box0 is inside - choose the min dist to move it out
    d = Min(max1 - min0, max0 - min1);
  }
  else
  {
    // boxes overlap
    d  = (max0 < max1) ? max0 : max1;
    d -= (min0 > min1) ? min0 : min1;
  }
  
  return false;
}

//==============================================================
// GetSupportPoint
//==============================================================
static void GetSupportPoint(tVector3 & p,
                            const tBox & box,
                            const tVector3 &axis)
{
  tScalar as = Dot(axis, box.GetOrient()[0]);
  tScalar au = Dot(axis, box.GetOrient()[1]);
  tScalar ad = Dot(axis, box.GetOrient()[2]);
  
  const tScalar threshold = SCALAR_TINY;
  
  p = box.GetCentre();
  
  if (as < -threshold)
    p += box.GetOrient()[0] * (0.5f * box.GetSideLengths().x);
  else if (as >= threshold)
    p -= box.GetOrient()[0] * (0.5f * box.GetSideLengths().x);
  
  if (au < -threshold)
    p += box.GetOrient()[1] * (0.5f * box.GetSideLengths().y);
  else if (au > threshold)
    p -= box.GetOrient()[1] * (0.5f * box.GetSideLengths().y);
  
  if (ad < -threshold)
    p += box.GetOrient()[2] * (0.5f * box.GetSideLengths().z);
  else if (ad > threshold)
    p -= box.GetOrient()[2] * (0.5f * box.GetSideLengths().z);
}

//========================================================
// AddPoint
// if pt is less than Sqrt(combinationDistanceSq) from one of the
// others the original is replaced with the mean of it
// and pt, and false is returned. true means that pt was
// added to pts
//========================================================
static inline bool AddPoint(tFixedVector<tContactPoint, MAX_PTS_PER_BOX_PAIR> & pts,
                            const tVector3 & pt,
                            tScalar combinationDistanceSq)
{
  for (unsigned i = pts.Size() ; i-- != 0 ; )
  {
    if (PointPointDistanceSq(pts[i].pos, pt) < combinationDistanceSq)
    {
      pts[i].pos = (pts[i].count * pts[i].pos + pt) / (pts[i].count + 1);
      pts[i].count += 1;
      return false;
    }
  }
  pts.PushBack(tContactPoint(pt));
  return true;
}

//==============================================================
// GetAABox2EdgeIntersectionPoint
// The AABox has a corner at the origin and size sides
//==============================================================
static unsigned GetAABox2EdgeIntersectionPoints(
  tFixedVector<tContactPoint, MAX_PTS_PER_BOX_PAIR> &pts,
  const tVector3 &sides,
  const tBox &box,
  const tVector3 &edgePt0, 
  const tVector3 &edgePt1,
  const tMatrix33 &origBoxOrient,
  const tVector3 &origBoxPos,
  const tVector3 &dirToAABB,
  tScalar combinationDistanceSq)
{
  // The AABox faces are aligned with the world directions. Loop 
  // over the 3 directions and do the two tests. We know that the
  // AABox has a corner at the origin
  tVector3 edgeDir = (edgePt1 - edgePt0).NormaliseSafe();
  unsigned num = 0;
  for (unsigned iDir = 3 ; iDir-- != 0 ; )
  {
    // skip edge/face tests if nearly parallel
    if (Abs(edgeDir[iDir]) < 0.1f)
      continue;
    const unsigned jDir = (iDir + 1) % 3;
    const unsigned kDir = (iDir + 2) % 3;
    // one plane goes through the origin, one is offset
    const tScalar faceOffsets[] = {0.0f, sides[iDir]};
    for (unsigned iFace = 2 ; iFace-- != 0 ; )
    {
      // distance of each point from to the face plane
      const tScalar dist0 = edgePt0[iDir] - faceOffsets[iFace];
      const tScalar dist1 = edgePt1[iDir] - faceOffsets[iFace];
      tScalar frac = -1.0f;
      if (dist0 * dist1 < -SCALAR_TINY)
        frac = -dist0 / (dist1 - dist0);
      else if (Abs(dist0) < SCALAR_TINY)
        frac = 0.0f;
      else if (Abs(dist1) < SCALAR_TINY)
        frac = 1.0f;
      if (frac >= 0.0f)
      {
        Assert(frac <= 1.0f);
        tVector3 pt = (1.0f - frac) * edgePt0 + frac * edgePt1;
        // check the point is within the face rectangle
        if ((pt[jDir] > -SCALAR_TINY) && 
            (pt[jDir] < sides[jDir] + SCALAR_TINY) && 
            (pt[kDir] > -SCALAR_TINY) && 
            (pt[kDir] < sides[kDir] + SCALAR_TINY) )
        {
          // woohoo got a point
          tVector3 pos;
          ApplyTransformation(pos, origBoxPos, origBoxOrient, pt);
          AddPoint(pts, pos, combinationDistanceSq);
          if (++num == 2)
            return num;
        }
      }
    }
  }
  return num;
}

//==============================================================
// GetAABox2BoxEdgesIntersectionPoints
// Pushes intersection points (in world space) onto the back of pts.
// Intersection is between an AABox faces and an orientated box's
// edges. orient and pos are used to transform the points from the
// AABox frame back into the original frame.
//==============================================================
static unsigned GetAABox2BoxEdgesIntersectionPoints(
  tFixedVector<tContactPoint, MAX_PTS_PER_BOX_PAIR> & pts,
  const tVector3 & sides,
  const tBox & box,
  const tMatrix33 & origBoxOrient,
  const tVector3 & origBoxPos,
  const tVector3 &dirToAABB,
  tScalar combinationDistanceSq)
{
  unsigned num = 0;
  
  static tVector3 boxPts[8];
  box.GetCornerPoints(boxPts);
  const tBox::tEdge * edges = box.GetAllEdges();
  
  for (unsigned iEdge = 0 ; iEdge < 12 ; ++iEdge)
  {
    const tVector3 & edgePt0 = boxPts[edges[iEdge].ind0];
    const tVector3 & edgePt1 = boxPts[edges[iEdge].ind1];
    num += GetAABox2EdgeIntersectionPoints(pts, 
                                           sides, 
                                           box, 
                                           edgePt0, edgePt1,
                                           origBoxOrient, origBoxPos,
                                           dirToAABB,
                                           combinationDistanceSq);
    // Don't think we can get more than 8... and anyway if we get too many 
    // then the penetration must be so bad who cares about the details?
    if (num >= 8)
      return num;
  }
  return num;
}

//==============================================================
// GetBoxBoxIntersectionPoints
// Pushes intersection points onto the back of pts. Returns the
// number of points found.
// Points that are close together (compared to 
// combinationDistance) get combined
// dirToBody0 is the collision normal towards box0
//==============================================================
static unsigned GetBoxBoxIntersectionPoints(
  tFixedVector<tContactPoint, MAX_PTS_PER_BOX_PAIR> & pts,
  const tBox & box0,
  const tBox & box1,
  const tVector3 &dirToBox0,
  tScalar combinationDistance,
  tScalar collTolerance)
{
  // first transform box1 into box0 space - there box0 has a corner
  // at the origin and faces parallel to the world planes. Then intersect
  // each of box1's edges with box0 faces, transforming each point back into
  // world space. Finally combine points
  tVector3 tol(0.5f * collTolerance);
  combinationDistance += collTolerance * 2.0f * Sqrt(3.0f);
  for (unsigned iBox = 0 ; iBox < 2 ; ++iBox)
  {
    const tBox & boxA = iBox ? box1 : box0;
    const tBox & boxB = iBox ? box0 : box1;
    
    const tMatrix33 boxAInvOrient = boxA.GetOrient().GetTranspose();
    tBox box( boxAInvOrient * (boxB.GetPos() - boxA.GetPos()), 
              boxAInvOrient * boxB.GetOrient(),
              boxB.GetSideLengths() );

    /*
    box.Expand(tol);
    box.SetPos(box.GetPos() + tol);
    */

    tVector3 dirToBoxA = iBox ? boxAInvOrient * dirToBox0 : boxAInvOrient * -dirToBox0;
    
    // if we get more than a certain number of points back from this call,
    // and iBox == 0, could probably skip the other test...
    GetAABox2BoxEdgesIntersectionPoints(
      pts, boxA.GetSideLengths()/* + tol*/, 
      box, boxA.GetOrient(), boxA.GetPos(), dirToBoxA, Sq(combinationDistance));
  }
  return pts.Size();
}

//==============================================================
// CollDetect
//==============================================================
void tCollDetectBoxBox::CollDetect(const tCollDetectInfo &info,
                                   tScalar collTolerance,
                                   tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  const tBox &box0 = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetBox();
  const tBox &box1 = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetBox();

  const tBox &oldBox0 = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetBox();
  const tBox &oldBox1 = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetBox();

  const tMatrix33 & dirs0 = box0.GetOrient();
  const tMatrix33 & dirs1 = box1.GetOrient();
  
  // the 15 potential separating axes
  static const unsigned numAxes = 15;
  tVector3 axes[numAxes] = {
    dirs0[0],
    dirs0[1],
    dirs0[2],
    dirs1[0],
    dirs1[1],
    dirs1[2],
    Cross(dirs0[0], dirs1[0]),
    Cross(dirs0[0], dirs1[1]),
    Cross(dirs0[0], dirs1[2]),
    Cross(dirs0[1], dirs1[0]),
    Cross(dirs0[1], dirs1[1]),
    Cross(dirs0[1], dirs1[2]),
    Cross(dirs0[2], dirs1[0]),
    Cross(dirs0[2], dirs1[1]),
    Cross(dirs0[2], dirs1[2]),
  };
  
  // the overlap depths along each axis
  tScalar overlapDepths[numAxes];
  
  // see if the boxes are separate along any axis, and if not keep a 
  // record of the depths along each axis
  unsigned i;
  for (i = 0 ; i < numAxes ; ++i)
  {
    // If we can't normalise the axis, skip it
    tScalar l2 = axes[i].GetLengthSq();
    if (l2 < SCALAR_TINY)
      continue;
    overlapDepths[i] = SCALAR_HUGE;
    if (Disjoint(overlapDepths[i], axes[i], box0, box1, collTolerance))
      return;
  }
  
  //-----------------------------------------------------------------
  // The box overlap, find the separation depth closest to 0.
  //-----------------------------------------------------------------
  tScalar minDepth = SCALAR_HUGE;
  int minAxis = -1;
  
  for(i = 0; i < numAxes; ++i)
  {
    // If we can't normalise the axis, skip it
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
    return;
  
  //-----------------------------------------------------------------
  // Make sure the axis is facing towards the 0th box.
  // if not, invert it
  //-----------------------------------------------------------------
  tVector3 D = box1.GetCentre() - box0.GetCentre();
  tVector3 & N = axes[minAxis];
  tScalar depth = overlapDepths[minAxis];
  
  if (Dot(D, N) > 0.0f)
    N.Negate();
  
  tScalar combinationDist = 
    0.05f * Min(Min(box0.GetSideLengths()), Min(box1.GetSideLengths()));
  
  /// the contact points
  bool contactPointsFromOld = true;
  tFixedVector<tContactPoint, MAX_PTS_PER_BOX_PAIR> pts;
  if (depth > -SCALAR_TINY)
    GetBoxBoxIntersectionPoints(pts, oldBox0, oldBox1, N, combinationDist, collTolerance);
  unsigned numPts = pts.Size();
  if (numPts == 0)
  {
    contactPointsFromOld = false;
    GetBoxBoxIntersectionPoints(pts, box0, box1, N, combinationDist, collTolerance);
  }
  numPts = pts.Size();

  const tVector3& body0OldPos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1OldPos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body0NewPos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetPosition() : tVector3::Zero();
  const tVector3& body1NewPos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetPosition() : tVector3::Zero();

  const tVector3 bodyDelta = (body0NewPos - body0OldPos) - (body1NewPos - body1OldPos);
  const tScalar bodyDeltaLen = Dot(bodyDelta, N);
  tScalar oldDepth = depth + bodyDeltaLen;

  tFixedVector<tCollPointInfo, MAX_PTS_PER_BOX_PAIR> collPts;
  collPts.Clear();

  tVector3 SATPoint(0.0f);
  switch(minAxis)
  {
    //-----------------------------------------------------------------
    // Box0 face, Box1 Corner collision
    //-----------------------------------------------------------------
  case 0:
  case 1:
  case 2:
  {
    //-----------------------------------------------------------------
    // Get the lowest point on the box1 along box1 normal
    //-----------------------------------------------------------------
    GetSupportPoint(SATPoint, box1, -N);
    break;
  }
  //-----------------------------------------------------------------
  // We have a Box2 corner/Box1 face collision
  //-----------------------------------------------------------------
  case 3:
  case 4:
  case 5:
  {
    //-----------------------------------------------------------------
    // Find with vertex on the triangle collided
    //-----------------------------------------------------------------
    GetSupportPoint(SATPoint, box0, N);
    break;
  }
  //-----------------------------------------------------------------
  // We have an edge/edge colliiosn
  //-----------------------------------------------------------------
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  { 
    {
      //-----------------------------------------------------------------
      // Retrieve which edges collided.
      //-----------------------------------------------------------------
      int i = minAxis-6;
      int ia = i / 3;
      int ib = i - ia * 3;
      Assert(ia >= 0);
      Assert(ia < 3);
      Assert(ib >= 0);
      Assert(ib < 3);
      //-----------------------------------------------------------------
      // find two P0, P1 point on both edges. 
      //-----------------------------------------------------------------
      tVector3 P0;
      tVector3 P1;
      GetSupportPoint( P0, box0, N);
      GetSupportPoint( P1, box1, -N);
      
      //-----------------------------------------------------------------
      // Find the edge intersection. 
      //-----------------------------------------------------------------
      
      //-----------------------------------------------------------------
      // plane along N and F, and passing through PB
      //-----------------------------------------------------------------
      tVector3 planeNormal = Cross(N, box1.GetOrient()[ib]);
      tScalar planeD = Dot(planeNormal, P1);
      
      //-----------------------------------------------------------------
      // find the intersection t, where Pintersection = P0 + t*box edge dir
      //-----------------------------------------------------------------
      tScalar div = Dot(box0.GetOrient()[ia], planeNormal);
      
      //-----------------------------------------------------------------
      // plane and ray colinear, skip the intersection.
      //-----------------------------------------------------------------
      if (Abs(div) < SCALAR_TINY)
        return;
      
      tScalar t = (planeD - Dot(P0,  planeNormal)) / div;
      
      //-----------------------------------------------------------------
      // point on edge of box0
      //-----------------------------------------------------------------
      P0 += box0.GetOrient()[ia] * t;
      
      SATPoint = (P0 + (0.5f * depth) * N);
    }
    break;
  }
  default:
    Assert(!"Impossible switch");
  }

  // distribute the depth according to the distance to the SAT point
  if (numPts > 0)
  {
    tScalar minDist = SCALAR_HUGE;
    tScalar maxDist = -SCALAR_HUGE;
    for (i = 0 ; i < numPts ; ++i)
    {
      tScalar dist = PointPointDistance(pts[i].pos, SATPoint);
      if (dist < minDist)
        minDist = dist;
      if (dist > maxDist)
        maxDist = dist;
    }
    if (maxDist < minDist + SCALAR_TINY)
      maxDist = minDist + SCALAR_TINY;

    // got some intersection points
    for (i = 0 ; i < numPts ; ++i)
    {
      static tScalar minDepthScale = 0.0f;
      tScalar dist = PointPointDistance(pts[i].pos, SATPoint);
      tScalar depthScale = (dist - minDist) / (maxDist - minDist);
      tScalar depth = (1.0f - depthScale) * oldDepth + minDepthScale * depthScale * oldDepth;
      if (contactPointsFromOld)
      {
        collPts.PushBack(tCollPointInfo(pts[i].pos - body0OldPos, 
                                        pts[i].pos - body1OldPos, 
                                        depth));
      }
      else
      {
        collPts.PushBack(tCollPointInfo(pts[i].pos - body0NewPos, 
                                        pts[i].pos - body1NewPos, 
                                        depth));
      }
    }
  }
  else
  {
    collPts.PushBack(tCollPointInfo(SATPoint - body0NewPos, 
                                    SATPoint - body1NewPos, oldDepth));

  }

  // report collisions
  //  TRACE("numPts = %d\n", numPts);
  collisionFunctor.CollisionNotify(
    info,
    N,
    &collPts[0],
    collPts.Size());
}


