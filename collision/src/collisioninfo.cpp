//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisioninfo.cpp 
//                     
//==============================================================
#include "collisioninfo.hpp"
#include "collisionsystem.hpp"


using namespace std;
using namespace JigLib;

vector<tCollisionInfo *> tCollisionInfo::mFreeInfos;

//==============================================================
// Init
//==============================================================
void tCollisionInfo::Init(const tCollDetectInfo &info,
                          const tVector3 & dirToBody0,
                          const tCollPointInfo * pointInfos,
                          unsigned numPointInfos)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  mSkinInfo = info;
  mDirToBody0 = dirToBody0;

  Assert(info.skin0);
  Assert(info.skin1);

  tMaterialTable::tMaterialID ID0 = info.skin0->GetMaterialID(info.iPrim0);
  tMaterialTable::tMaterialID ID1 = info.skin1->GetMaterialID(info.iPrim1);
  
  const tMaterialTable &matTable = info.skin0->GetCollisionSystem()->GetMaterialTable();
  if (ID0 == tMaterialTable::USER_DEFINED || ID1 == tMaterialTable::USER_DEFINED)
  {
    const tMaterialProperties &prop0 = info.skin0->GetMaterialProperties(info.iPrim0);
    const tMaterialProperties &prop1 = info.skin1->GetMaterialProperties(info.iPrim1);
    mMatPairProperties.mRestitution = prop0.mElasticity * prop1.mElasticity;
    mMatPairProperties.mStaticFriction = prop0.mStaticRoughness * prop1.mStaticRoughness;
    mMatPairProperties.mDynamicFriction = prop0.mDynamicRoughness * prop1.mDynamicRoughness;
  }
  else
  {
    mMatPairProperties = matTable.GetPairProperties(ID0, ID1); 
  }
  
  if (numPointInfos > MAX_COLLISION_POINTS)
    numPointInfos = MAX_COLLISION_POINTS;
  mPointInfo.Resize(numPointInfos);
  for (unsigned i = 0 ; i < numPointInfos ; ++i)
    mPointInfo[i] = pointInfos[i];
}

//==============================================================
// Destroy
//==============================================================
void tCollisionInfo::Destroy()
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  mSkinInfo.skin0 = (tCollisionSkin *) 0xDEADBEAF;
  mSkinInfo.skin1 = mSkinInfo.skin0;
}

//==============================================================
// GetCollisionInfo
//==============================================================
tCollisionInfo & tCollisionInfo::GetCollisionInfo(const tCollDetectInfo &info,
                                                  const tVector3 & dirToBody0,
                                                  const tCollPointInfo * pointInfos,
                                                  unsigned numPointInfos)
{
  TRACE_FUNCTION_ONLY(MULTI_FRAME_2);
  if (mFreeInfos.empty())
    mFreeInfos.push_back(new tCollisionInfo);
  tCollisionInfo *collInfo = mFreeInfos.back();
  Assert(0 != collInfo);
  collInfo->Init(info, dirToBody0, pointInfos, numPointInfos);
  mFreeInfos.pop_back();
  return *collInfo;
}

//==============================================================
// FreeCollisionInfo
//==============================================================
void tCollisionInfo::FreeCollisionInfo(tCollisionInfo & info)
{
  TRACE_FUNCTION_ONLY(MULTI_FRAME_2);
  info.Destroy();
  mFreeInfos.push_back(&info);
}




