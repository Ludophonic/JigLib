//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectsphereheightmap.cpp 
//                     
//==============================================================
#include "colldetectsphereheightmap.hpp"
#include "sphere.hpp"
#include "heightmap.hpp"
#include "mathsmisc.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectSphereHeightmap
//==============================================================
tCollDetectSphereHeightmap::tCollDetectSphereHeightmap()
  :
  tCollDetectFunctor("SphereHeightmap", tPrimitive::SPHERE, tPrimitive::HEIGHTMAP)
{
}


//==============================================================
// operator
//==============================================================
void tCollDetectSphereHeightmap::CollDetect(const tCollDetectInfo &infoOrig,
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

  const tHeightmap & oldHeightmap = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetHeightmap();
  const tHeightmap & newHeightmap = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetHeightmap();
      
  tScalar newDist;
  tVector3 normal;
  newHeightmap.GetHeightAndNormal(newDist, normal, newSphere.GetPos());
  if (newDist < collTolerance + newSphere.GetRadius())
  {
    tScalar oldDist = oldHeightmap.GetHeight(oldSphere.GetPos());
    tScalar depth = oldSphere.GetRadius() - oldDist;
    
    // calc the world position when it just hit. 
    tVector3 oldPt = oldSphere.GetPos() - oldSphere.GetRadius() * normal;
    tCollPointInfo ptInfo(oldPt - body0Pos, oldPt -  body1Pos, depth);
    
    collisionFunctor.CollisionNotify(
      info,
      normal,
      &ptInfo,
      1);
  }
}

