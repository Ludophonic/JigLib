//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxplane.cpp 
//                     
//==============================================================
#include "colldetectboxplane.hpp"
#include "mathsmisc.hpp"
#include "fixedvector.hpp"
#include "body.hpp"
#include "distance.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectBoxPlane
//==============================================================
tCollDetectBoxPlane::tCollDetectBoxPlane()
  :
  tCollDetectFunctor("BoxPlane", tPrimitive::BOX, tPrimitive::PLANE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectBoxPlane::CollDetect(const tCollDetectInfo &infoOrig,
                                     tScalar collTolerance,
                                     tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  // get the skins in the order that we're expecting
  tCollDetectInfo info(infoOrig);
  if (info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetType() == mType1)
  {
    Swap(info.skin0, info.skin1); 
    Swap(info.iPrim0, info.iPrim1);
  }
  
  const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

  const tBox & oldBox = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetBox();
  const tBox & newBox = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetBox();

  const tPlane & oldPlane = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetPlane();
  const tPlane & newPlane = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetPlane();
      
  // quick check
  tScalar centreDist = PointPlaneDistance(newBox.GetCentre(), newPlane);
  if (centreDist > collTolerance + newBox.GetBoundingRadiusAboutCentre())
    return;

  static tVector3 newPts[8];
  newBox.GetCornerPoints(newPts);
  static tVector3 oldPts[8];
  oldBox.GetCornerPoints(oldPts);

  static tFixedVector<tCollPointInfo, 8> collPts;
  collPts.Clear();
  
  for (unsigned i = 0 ; i < 8 ; ++i)
  {
    const tVector3 & newPt = newPts[i];
    const tVector3 & oldPt = oldPts[i];
    tScalar oldDepth = -PointPlaneDistance(oldPt, oldPlane);
    tScalar newDepth = -PointPlaneDistance(newPt, newPlane);
    if (Max(oldDepth, newDepth) > -collTolerance)
      collPts.PushBack(tCollPointInfo(oldPt - body0Pos, oldPt - body1Pos, oldDepth));
  }

  if (!collPts.Empty())
  {
    collisionFunctor.CollisionNotify(
      info,
      oldPlane.GetN(),
      &collPts[0],
      collPts.Size());
  }
}

