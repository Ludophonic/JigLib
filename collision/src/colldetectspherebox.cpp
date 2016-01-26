//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectspherebox.cpp 
//                     
//==============================================================
#include "colldetectspherebox.hpp"
#include "sphere.hpp"
#include "box.hpp"
#include "mathsmisc.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSphereBox
//==============================================================
tCollDetectSphereBox::tCollDetectSphereBox()
  :
  tCollDetectFunctor("SphereBox", tPrimitive::SPHERE, tPrimitive::BOX)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectSphereBox::CollDetect(const tCollDetectInfo &infoOrig,
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

  const tBox & oldBox = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetBox();
  const tBox & newBox = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetBox();
  tVector3 oldBoxPoint;
  tVector3 newBoxPoint;
  tScalar oldDist = oldBox.GetDistanceToPoint(oldBoxPoint,
                                              oldSphere.GetPos());
  tScalar newDist = newBox.GetDistanceToPoint(newBoxPoint,
                                              newSphere.GetPos());
  // normally point will be outside
  tScalar oldDepth = oldSphere.GetRadius() - oldDist;
  tScalar newDepth = newSphere.GetRadius() - newDist;

  if (Max(oldDepth, newDepth) > -collTolerance)
  {
    tVector3 dir;
    if (oldDist < -SCALAR_TINY)
    {
      dir = (oldBoxPoint - oldSphere.GetPos() - oldBoxPoint).NormaliseSafe();
    }
    else if (oldDist > SCALAR_TINY)
    {
      dir = (oldSphere.GetPos() - oldBoxPoint).NormaliseSafe();
    }
    else
    {
      dir = (oldSphere.GetPos() - oldBox.GetCentre()).NormaliseSafe();
    }
    tCollPointInfo collInfo(oldBoxPoint - body0Pos, oldBoxPoint - body1Pos, oldDepth);
    
    collisionFunctor.CollisionNotify(
      info,
      dir,
      &collInfo,
      1);
  }
}
