//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file colldetectboxheightmap.cpp 
//                     
//==============================================================
#include "colldetectboxheightmap.hpp"
#include "mathsmisc.hpp"
#include "heightmap.hpp"
#include "body.hpp"

using namespace std;
using namespace JigLib;

//==============================================================
// tCollDetectBoxHeightmap
//==============================================================
tCollDetectBoxHeightmap::tCollDetectBoxHeightmap() :
tCollDetectFunctor("BoxHeightmap", tPrimitive::BOX, tPrimitive::HEIGHTMAP)
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// operator
//==============================================================
void tCollDetectBoxHeightmap::CollDetect(const tCollDetectInfo &infoOrig,
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
  const tBox & oldBox = info.skin0->GetPrimitiveOldWorld(info.iPrim0)->GetBox();
  const tBox & newBox = info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetBox();

  const tHeightmap & oldHeightmap = info.skin1->GetPrimitiveOldWorld(info.iPrim1)->GetHeightmap();
  const tHeightmap & newHeightmap = info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetHeightmap();

  static tVector3 oldPts[8];
  oldBox.GetCornerPoints(oldPts);
  static tVector3 newPts[8];
  newBox.GetCornerPoints(newPts);
  
  static tFixedVector<tCollPointInfo, 8> collPts;
  collPts.Clear();

  tVector3 collNormal(0.0f);
  
  for (unsigned i = 0 ; i < 8 ; ++i)
  {
    const tVector3 & newPt = newPts[i];

    tScalar newDist;
    tVector3 normal;
    newHeightmap.GetHeightAndNormal(newDist, normal, newPt);
    
    if (newDist < collTolerance)
    {
      const tVector3 & oldPt = oldPts[i];
      tScalar oldDist = oldHeightmap.GetHeight(oldPt);
      collPts.PushBack(tCollPointInfo(oldPt - body0Pos, 
                                      oldPt - body1Pos, -oldDist));
      collNormal += normal;
    }
  }
  if (!collPts.Empty())
  {
    collNormal.NormaliseSafe();
    collisionFunctor.CollisionNotify(
      info,
      collNormal,
      &collPts[0],
      collPts.Size());
  }
}

