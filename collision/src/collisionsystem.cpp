//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystem.cpp 
//                     
//==============================================================
#include "collisionsystem.hpp"
#include "jiglib.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tCollisionSystem
//==============================================================
tCollisionSystem::tCollisionSystem()
  : mUseSweepTests(false)
{
  TRACE_METHOD_ONLY(ONCE_1);
  
  static tCollDetectBoxBox boxBoxCollDetector;
  static tCollDetectBoxHeightmap boxHeightmapCollDetector;
  static tCollDetectBoxPlane boxPlaneCollDetector;
  static tCollDetectCapsuleBox capsuleBoxCollDetector;
  static tCollDetectCapsuleCapsule capsuleCapsuleCollDetector;
  static tCollDetectCapsuleHeightmap capsuleHeightmapCollDetector;
  static tCollDetectCapsulePlane capsulePlaneCollDetector;
  static tCollDetectSphereBox sphereBoxCollDetector;
  static tCollDetectSphereCapsule sphereCapsuleCollDetector;
  static tCollDetectSphereHeightmap sphereHeightmapCollDetector;
  static tCollDetectSpherePlane spherePlaneCollDetector;
  static tCollDetectSphereSphere sphereSphereCollDetector;
  static tCollDetectSphereStaticMesh sphereStaticMeshCollDetector;
  static tCollDetectCapsuleStaticMesh capsuleStaticMeshCollDetector;
  static tCollDetectBoxStaticMesh boxStaticMeshCollDetector;
  
  RegisterCollDetectFunctor(boxBoxCollDetector);
  RegisterCollDetectFunctor(boxHeightmapCollDetector);
  RegisterCollDetectFunctor(boxPlaneCollDetector);
  RegisterCollDetectFunctor(capsuleBoxCollDetector);
  RegisterCollDetectFunctor(capsuleCapsuleCollDetector);
  RegisterCollDetectFunctor(capsuleHeightmapCollDetector);
  RegisterCollDetectFunctor(capsulePlaneCollDetector);
  RegisterCollDetectFunctor(sphereBoxCollDetector);
  RegisterCollDetectFunctor(sphereCapsuleCollDetector);
  RegisterCollDetectFunctor(sphereHeightmapCollDetector);
  RegisterCollDetectFunctor(spherePlaneCollDetector);
  RegisterCollDetectFunctor(sphereSphereCollDetector);
  RegisterCollDetectFunctor(sphereStaticMeshCollDetector);
  RegisterCollDetectFunctor(capsuleStaticMeshCollDetector);
  RegisterCollDetectFunctor(boxStaticMeshCollDetector);
}

//==============================================================
// tCollisionSystem
//==============================================================
tCollisionSystem::~tCollisionSystem()
{
  TRACE_METHOD_ONLY(ONCE_1);
}

//==============================================================
// RegisterCollDetectFunction
//==============================================================
void tCollisionSystem::RegisterCollDetectFunctor(tCollDetectFunctor & f)
{
  TRACE_METHOD_ONLY(ONCE_1);
  
  unsigned type0 = f.mType0;
  unsigned type1 = f.mType1;
  unsigned maxType = Max(type0, type1);
  
  if (maxType >= mDetectionFunctors.size())
  {
    mDetectionFunctors.resize(maxType + 1);
  }
  unsigned sizeF0 = mDetectionFunctors[0].size();
  if (maxType >= sizeF0)
  {
    for (unsigned i = 0 ; i < mDetectionFunctors.size() ; ++i)
    {
      mDetectionFunctors[i].resize(maxType + 1);
    }
  }
  
  mDetectionFunctors[type0][type1] = &f;
  mDetectionFunctors[type1][type0] = &f;
}


