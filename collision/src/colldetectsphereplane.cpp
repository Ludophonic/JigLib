//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectsphereplane.cpp 
//                     
//==============================================================
#include "colldetectsphereplane.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSpherePlane
//==============================================================
tCollDetectSpherePlane::tCollDetectSpherePlane()
  :
  tCollDetectFunctor("SpherePlane", tPrimitive::SPHERE, tPrimitive::PLANE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectSpherePlane::CollDetect(const tCollDetectInfo &infoOrig,
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
  const tSphere & oldSphere = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetSphere();
  const tSphere & newSphere = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetSphere();

  const tPlane & oldPlane = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetPlane();
  const tPlane & newPlane = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetPlane();
  
  // consider it a contact if either old or new are touching
  tScalar oldDist = PointPlaneDistance(oldSphere.GetPos(), oldPlane);
  tScalar newDist = PointPlaneDistance(newSphere.GetPos(), newPlane);
  if (Min(newDist, oldDist) > collTolerance + newSphere.GetRadius())
    return;

  // collision - record depth using the old values
  tScalar oldDepth = oldSphere.GetRadius() - oldDist;
  
  // calc the world position based on the old position(s)
  tVector3 worldPos = oldSphere.GetPos() - 
    oldSphere.GetRadius() * oldPlane.GetN();

  tCollPointInfo collInfo(worldPos - body0Pos, worldPos - body1Pos, oldDepth);
  
  collisionFunctor.CollisionNotify(
    info,
    oldPlane.GetN(),
    &collInfo,
    1);
}
