//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsuleheightmap.cpp 
//                     
//==============================================================
#include "colldetectcapsuleheightmap.hpp"
#include "capsule.hpp"
#include "heightmap.hpp"
#include "mathsmisc.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectCapsuleHeightmap
//==============================================================
tCollDetectCapsuleHeightmap::tCollDetectCapsuleHeightmap()
  :
  tCollDetectFunctor("CapsuleHeightmap", tPrimitive::CAPSULE, tPrimitive::HEIGHTMAP)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectCapsuleHeightmap::CollDetect(const tCollDetectInfo &infoOrig,
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

  const tHeightmap & oldHeightmap = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetHeightmap();
  const tHeightmap & newHeightmap = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetHeightmap();
      
  tFixedVector<tCollPointInfo, 2> pts;
  tVector3 averageNormal(0.0f);
  
  // the start
  {
    tScalar oldDist, newDist;
    tVector3 normal;
    oldHeightmap.GetHeightAndNormal(oldDist, normal, oldCapsule.GetPos());
    newHeightmap.GetHeightAndNormal(newDist, normal, newCapsule.GetPos());
    if (Min(newDist, oldDist) < collTolerance + newCapsule.GetRadius())
    {
      tScalar oldDepth = oldCapsule.GetRadius() - oldDist;
      // calc the world position based on the old position(s)
      tVector3 worldPos = oldCapsule.GetPos() - oldCapsule.GetRadius() * normal;
      pts.PushBack(tCollPointInfo(worldPos - body0Pos, worldPos - body1Pos, oldDepth));
      averageNormal += normal;
    }
  }
  // the end
  {
    tVector3 oldEnd = oldCapsule.GetEnd();
    tVector3 newEnd = newCapsule.GetEnd();
    tScalar oldDist, newDist;
    tVector3 normal;
    oldHeightmap.GetHeightAndNormal(oldDist, normal, oldEnd);
    newHeightmap.GetHeightAndNormal(newDist, normal, newEnd);
    if (Min(newDist, oldDist) < collTolerance + newCapsule.GetRadius())
    {
      tScalar oldDepth = oldCapsule.GetRadius() - oldDist;
      // calc the world position based on the old position(s)
      tVector3 worldPos = oldEnd - oldCapsule.GetRadius() * normal;
      pts.PushBack(tCollPointInfo(worldPos - body0Pos, worldPos - body1Pos, oldDepth));
      averageNormal += normal;
    }
  }

  if (!pts.Empty())
  {
    averageNormal.NormaliseSafe();

    collisionFunctor.CollisionNotify(
      info,
      averageNormal,
      &pts[0],
      pts.Size());
  }
}

