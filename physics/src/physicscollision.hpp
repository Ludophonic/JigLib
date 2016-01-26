//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file physicscollision.hpp 
//                     
//==============================================================
#ifndef JIGPHYSICSCOLLISION_HPP
#define JIGPHYSICSCOLLISION_HPP

#include "body.hpp"
#include "collisionsystem.hpp"
#include "collisioninfo.hpp"

#include <vector>
// helpers for physics when it calls collision detection
namespace JigLib
{
  /// ensures that skin 0 is non-zero, even though skin1 might be
  class tBasicCollisionFunctor : public tCollisionFunctor
  {
  public:
    tBasicCollisionFunctor(
      std::vector<tCollisionInfo *> & colls)
      : mColls(colls) {}

    void CollisionNotify(const tCollDetectInfo &collDetectInfo, 
                         const tVector3 & dirToBody0,
                         const tCollPointInfo * pointInfos,
                         unsigned numPointInfos)
      {
        Assert(collDetectInfo.skin0);
        Assert(collDetectInfo.skin1);
        tCollisionInfo * info = 0;
        // if more than one point, add another that is in the middle - collision
        if ( collDetectInfo.skin0 && (collDetectInfo.skin0->GetOwner() != 0) )
        {
          info = &tCollisionInfo::GetCollisionInfo(
            collDetectInfo,
            dirToBody0, 
            pointInfos, 
            numPointInfos);
          mColls.push_back(info);
          collDetectInfo.skin0->GetCollisions().push_back(info);
          if ( collDetectInfo.skin1 && (collDetectInfo.skin1->GetOwner()) )
            collDetectInfo.skin1->GetCollisions().push_back(info);
        }
        else if ( collDetectInfo.skin1 && (collDetectInfo.skin1->GetOwner() != 0) )
        {
          info = &tCollisionInfo::GetCollisionInfo(
            collDetectInfo,
            -dirToBody0, 
            pointInfos, 
            numPointInfos);
          mColls.push_back(info);
          collDetectInfo.skin1->GetCollisions().push_back(info);
          if ( collDetectInfo.skin0 && (collDetectInfo.skin0->GetOwner()) )
            collDetectInfo.skin0->GetCollisions().push_back(info);
        }
        else
        {
          TRACE("warning: collision detected with both skin bodies 0\n");
          return;
        }
      }
    std::vector<tCollisionInfo *> & mColls;
  };

  class tFrozenCollisionPredicate : public tCollisionSkinPredicate2
  {
  public:
    tFrozenCollisionPredicate(tBody * body) 
      : mBody(body) {Assert(mBody);}

    bool ConsiderSkinPair(
      tCollisionSkin * skin0,
      tCollisionSkin * skin1) const
      {
        if ( skin0->GetOwner() && (skin0->GetOwner() != mBody) )
          if (!skin0->GetOwner()->IsActive())
            return true;
        if ( skin1->GetOwner() && (skin1->GetOwner() != mBody) )
          if (!skin1->GetOwner()->IsActive())
            return true;
        return false;
      }
    tBody * mBody;
  };
}


#endif
