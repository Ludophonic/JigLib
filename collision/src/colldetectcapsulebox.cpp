//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectcapsulebox.cpp 
//                     
//==============================================================
#include "colldetectcapsulebox.hpp"
#include "capsule.hpp"
#include "box.hpp"
#include "mathsmisc.hpp"
#include "distance.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectCapsuleBox
//==============================================================
tCollDetectCapsuleBox::tCollDetectCapsuleBox()
  :
  tCollDetectFunctor("CapsuleBox", tPrimitive::CAPSULE, tPrimitive::BOX)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectCapsuleBox::CollDetect(const tCollDetectInfo &infoOrig,
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
  const tSegment oldSeg(oldCapsule.GetPos(), oldCapsule.GetLength() * oldCapsule.GetOrient().GetLook());
  const tSegment newSeg(newCapsule.GetPos(), newCapsule.GetLength() * newCapsule.GetOrient().GetLook());

  tScalar radius = oldCapsule.GetRadius();

  const tBox & oldBox = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetBox();
  const tBox & newBox = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetBox();

  tScalar oldSegT;
  tScalar oldBoxT0, oldBoxT1, oldBoxT2;
  tScalar oldDistSq = SegmentBoxDistanceSq(&oldSegT, &oldBoxT0, &oldBoxT1, &oldBoxT2, oldSeg, oldBox);
  tScalar newSegT;
  tScalar newBoxT0, newBoxT1, newBoxT2;
  tScalar newDistSq = SegmentBoxDistanceSq(&newSegT, &newBoxT0, &newBoxT1, &newBoxT2, newSeg, newBox);
        
  if (Min(oldDistSq, newDistSq) < Sq(radius + collTolerance))
  {
    tVector3 segPos = oldSeg.GetPoint(oldSegT);
    tVector3 boxPos = oldBox.GetCentre() + oldBoxT0 * oldBox.GetOrient()[0] + 
      oldBoxT1 * oldBox.GetOrient()[1] + oldBoxT2 * oldBox.GetOrient()[2];

    tScalar dist = Sqrt(oldDistSq);
    tScalar depth = radius - dist;

    tVector3 dir;
    if (dist > SCALAR_TINY)
    {
      dir = (segPos - boxPos).NormaliseSafe();
    }
    else if ( (segPos - oldBox.GetCentre()).GetLength() > SCALAR_TINY )
    {
      dir = (segPos - oldBox.GetCentre()).NormaliseSafe();
    }
    else
    {
      // todo - make this not random
      dir = RotationMatrix(RangedRandom(0.0f, 360.0f), tVector3::Up()) * tVector3::Look();
    }
    Assert(IsFinite(depth));
    tCollPointInfo collInfo(boxPos - body0Pos, boxPos - body1Pos, depth);
    
    collisionFunctor.CollisionNotify(
      info,
      dir,
      &collInfo,
      1);
  }
}
