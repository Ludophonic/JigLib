//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsuleplane.cpp 
//                     
//==============================================================
#include "colldetectcapsuleplane.hpp"
#include "capsule.hpp"
#include "plane.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectCapsulePlane
//==============================================================
tCollDetectCapsulePlane::tCollDetectCapsulePlane()
  :
  tCollDetectFunctor("CapsulePlane", tPrimitive::CAPSULE, tPrimitive::PLANE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectCapsulePlane::CollDetect(const tCollDetectInfo &infoOrig,
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

  // todo - proper swept test
  const tCapsule & oldCapsule = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetCapsule();
  const tCapsule & newCapsule = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetCapsule();

  const tPlane & oldPlane = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetPlane();
  const tPlane & newPlane = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetPlane();

  tFixedVector<tCollPointInfo, 2> pts;

  // the start
  {
    tScalar oldDist = PointPlaneDistance(oldCapsule.GetPos(), oldPlane);
    tScalar newDist = PointPlaneDistance(newCapsule.GetPos(), newPlane);
    if (Min(newDist, oldDist) < collTolerance + newCapsule.GetRadius())
    {
      tScalar oldDepth = oldCapsule.GetRadius() - oldDist;
      // calc the world position based on the old position(s)
      tVector3 worldPos = oldCapsule.GetPos() - oldCapsule.GetRadius() * oldPlane.GetN();
      pts.PushBack(tCollPointInfo(worldPos - body0Pos, worldPos - body1Pos, oldDepth));
    }
  }
  // the end
  {
    tVector3 oldEnd = oldCapsule.GetEnd();
    tVector3 newEnd = newCapsule.GetEnd();
    tScalar oldDist = PointPlaneDistance(oldEnd, oldPlane);
    tScalar newDist = PointPlaneDistance(newEnd, newPlane);
    if (Min(newDist, oldDist) < collTolerance + newCapsule.GetRadius())
    {
      tScalar oldDepth = oldCapsule.GetRadius() - oldDist;
      // calc the world position based on the old position(s)
      tVector3 worldPos = oldEnd - oldCapsule.GetRadius() * oldPlane.GetN();
      pts.PushBack(tCollPointInfo(worldPos - body0Pos, worldPos - body1Pos, oldDepth));
    }

    if (!pts.Empty())
    {
      collisionFunctor.CollisionNotify(
        info,
        oldPlane.GetN(),
        &pts[0],
        pts.Size());
    }
  }
}

