//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspheresphere.cpp 
//                     
//==============================================================
#include "colldetectspheresphere.hpp"
#include "sphere.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSphereSphere
//==============================================================
tCollDetectSphereSphere::tCollDetectSphereSphere()
  :
  tCollDetectFunctor("SphereSphere", tPrimitive::SPHERE, tPrimitive::SPHERE)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectSphereSphere::CollDetect(const tCollDetectInfo &info,
                                         tScalar collTolerance,
                                         tCollisionFunctor & collisionFunctor) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  
  const tVector3& body0Pos = info.skin0->GetOwner() ? info.skin0->GetOwner()->GetOldPosition() : tVector3::Zero();
  const tVector3& body1Pos = info.skin1->GetOwner() ? info.skin1->GetOwner()->GetOldPosition() : tVector3::Zero();

  // todo - proper swept test
  const tSphere & oldSphere0 = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetSphere();
  const tSphere & newSphere0 = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetSphere();
  const tSphere & oldSphere1 = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetSphere();
  const tSphere & newSphere1 = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetSphere();

  tVector3 oldDelta = oldSphere0.GetPos() - oldSphere1.GetPos();
  tVector3 newDelta = newSphere0.GetPos() - newSphere1.GetPos();

  tScalar oldDistSq = oldDelta.GetLengthSq();
  tScalar newDistSq = newDelta.GetLengthSq();

  tScalar radSum = newSphere0.GetRadius() + newSphere1.GetRadius();

  if (Min(oldDistSq, newDistSq) < Sq(radSum + collTolerance))
  {
    tScalar oldDist = Sqrt(oldDistSq);
    tScalar depth = radSum - oldDist;
    if (oldDist > SCALAR_TINY)
    {
      oldDelta /= oldDist;
    }
    else
    {
      // TODO - make this not random...!
      oldDelta = RotationMatrix(RangedRandom(0.0f, 360.0f), tVector3::Up()) * tVector3::Look();
    }
    tVector3 worldPos = oldSphere1.GetPos() + 
      (oldSphere1.GetRadius() - 0.5f * depth) * oldDelta;
    tCollPointInfo collInfo(worldPos - body0Pos, worldPos - body1Pos, depth);
    
    collisionFunctor.CollisionNotify(
      info,
      oldDelta,
      &collInfo,
      1);
  }
}

