//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulecapsule.cpp 
//                     
//==============================================================
#include "colldetectcapsulecapsule.hpp"
#include "capsule.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectCapsuleCapsule
//==============================================================
tCollDetectCapsuleCapsule::tCollDetectCapsuleCapsule()
  :
  tCollDetectFunctor("CapsuleCapsule", tPrimitive::CAPSULE, tPrimitive::CAPSULE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectCapsuleCapsule::CollDetect(const tCollDetectInfo &info,
                                           tScalar collTolerance,
                                           tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

  // todo - proper swept test
  const tCapsule & oldCapsule0 = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetCapsule();
  const tCapsule & newCapsule0 = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetCapsule();
  const tSegment oldSeg0(oldCapsule0.GetPos(), oldCapsule0.GetLength() * oldCapsule0.GetOrient().GetLook());
  const tSegment newSeg0(newCapsule0.GetPos(), newCapsule0.GetLength() * newCapsule0.GetOrient().GetLook());

  const tCapsule & oldCapsule1 = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetCapsule();
  const tCapsule & newCapsule1 = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetCapsule();
  const tSegment oldSeg1(oldCapsule1.GetPos(), oldCapsule1.GetLength() * oldCapsule1.GetOrient().GetLook());
  const tSegment newSeg1(newCapsule1.GetPos(), newCapsule1.GetLength() * newCapsule1.GetOrient().GetLook());

  tScalar radSum = newCapsule0.GetRadius() + newCapsule1.GetRadius();

  tScalar oldt0, oldt1;
  tScalar newt0, newt1;
  tScalar oldDistSq = SegmentSegmentDistanceSq(&oldt0, &oldt1, oldSeg0, oldSeg1);
  tScalar newDistSq = SegmentSegmentDistanceSq(&newt0, &newt1, newSeg0, newSeg1);
        
  if (Min(oldDistSq, newDistSq) < Sq(radSum + collTolerance))
  {
    tVector3 pos0 = oldSeg0.GetPoint(oldt0);
    tVector3 pos1 = oldSeg1.GetPoint(oldt1);

    tVector3 delta = pos0 - pos1;

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
    tVector3 worldPos = pos1 + 
      (oldCapsule1.GetRadius() - 0.5f * depth) * delta;
    tCollPointInfo collInfo(worldPos - body0Pos, worldPos - body1Pos, depth);

    collisionFunctor.CollisionNotify(
      info,
      delta,
      &collInfo,
      1);
  }
}

