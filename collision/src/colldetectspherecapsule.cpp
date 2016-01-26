//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherecapsule.cpp 
//                     
//==============================================================
#include "colldetectspherecapsule.hpp"
#include "sphere.hpp"
#include "capsule.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSphereCapsule
//==============================================================
tCollDetectSphereCapsule::tCollDetectSphereCapsule()
  :
  tCollDetectFunctor("SphereCapsule", tPrimitive::SPHERE, tPrimitive::CAPSULE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectSphereCapsule::CollDetect(const tCollDetectInfo &infoOrig,
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

  const tCapsule & oldCapsule = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetCapsule();
  const tCapsule & newCapsule = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetCapsule();
  
  const tSegment oldSeg(oldCapsule.GetPos(), oldCapsule.GetLength() * oldCapsule.GetOrient().GetLook());
  const tSegment newSeg(newCapsule.GetPos(), newCapsule.GetLength() * newCapsule.GetOrient().GetLook());

  tScalar radSum = newCapsule.GetRadius() + newSphere.GetRadius();

  tScalar oldt, newt;
  tScalar oldDistSq = PointSegmentDistanceSq(&oldt, oldSphere.GetPos(), oldSeg);
  tScalar newDistSq = PointSegmentDistanceSq(&newt, newSphere.GetPos(), newSeg);
        
  if (Min(oldDistSq, newDistSq) < Sq(radSum + collTolerance))
  {
    tVector3 segPos = oldSeg.GetPoint(oldt);
    tVector3 delta = oldSphere.GetPos() - segPos;

    tScalar dist = Sqrt(oldDistSq);
    tScalar depth = radSum - dist;
    if (dist > SCALAR_TINY)
    {
      delta /= dist;
    }
    else
    {
      // TODO - make this not random...!
      delta = RotationMatrix(RangedRandom(0.0f, 360.0f), tVector3::Up()) * tVector3::Look();
    }
    tVector3 worldPos = segPos + 
      (oldCapsule.GetRadius() - 0.5f * depth) * delta;
    tCollPointInfo collInfo(worldPos - body0Pos, worldPos - body1Pos, depth);
    
    collisionFunctor.CollisionNotify(
      info,
      delta,
      &collInfo,
      1);
  }
}
