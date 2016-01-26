//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisioninfo.hpp 
//                     
//==============================================================
#ifndef JIGCOLLISIONINFO_HPP
#define JIGCOLLISIONINFO_HPP
#include "../maths/include/vector3.hpp"
#include "../maths/include/matrix33.hpp"
#include "../collision/include/collisionskin.hpp"
#include "../collision/include/materials.hpp"
#include "../utils/include/fixedvector.hpp"

#include <vector>
#include <set>

namespace JigLib
{
  // ideally this would be defined within tCollisionInfo but MSVC6
  // barfs!
  class tCollPointInfo
  {
  public:
    tCollPointInfo() {}
    tCollPointInfo(const tVector3& R0, const tVector3& R1, tScalar initialPenetration) 
      : mInitialPenetration(initialPenetration), 
        mR0(R0), mR1(R1), 
        mMinSeparationVel(0.0f), 
        mDenominator(0.0f),
        mAccumulatedNormalImpulse(0.0f),
        mAccumulatedNormalImpulseAux(0.0f),
        mAccumulatedFrictionImpulse(0.0f) {}
    /// Estimated Penetration before the objects collide (can be -ve)
    tScalar mInitialPenetration; 
    tVector3 mR0; ///< positions relative to body 0 (in world space)
    tVector3 mR1; ///< positions relative to body 1 (if there is a body1)
    /// Used by physics to cache desired minimum separation velocity
    /// in the normal direction
    tScalar mMinSeparationVel;
    /// Used by physics to cache value used in calculating impulse
    tScalar mDenominator; 
    /// Used by physics to accumulated the normal impulse
    tScalar mAccumulatedNormalImpulse;
    tScalar mAccumulatedNormalImpulseAux;
    tVector3 mAccumulatedFrictionImpulse;
    /// Used by physics to cache the world position (not really
    /// needed? pretty useful in debugging!)
    tVector3 mPosition;
  };

  /// For sorting by penetration depth using std::sort
  inline bool LessPD(const tCollPointInfo& info0, const tCollPointInfo& info1)
  {
    return info0.mInitialPenetration < info1.mInitialPenetration;
  }
  inline bool MorePD(const tCollPointInfo& info0, const tCollPointInfo& info1)
  {
    return info0.mInitialPenetration > info1.mInitialPenetration;
  }

  /// details about which parts of the skins are colliding
  struct tCollDetectInfo
  {
    tCollDetectInfo(tCollisionSkin *skin0 = 0, tCollisionSkin *skin1 = 0, unsigned iPrim0 = 0, unsigned iPrim1 = 0) : skin0(skin0), skin1(skin1), iPrim0(iPrim0), iPrim1(iPrim1) {}
    tCollisionSkin * skin0; 
    tCollisionSkin * skin1;
    unsigned iPrim0; ///< index into skin0 primitive
    unsigned iPrim1; ///< index into skin1 primitive
  };

  /// contains all the details about a collision between two skins,
  /// each of which may be associated with a tBody.  Each collision
  /// can have a number of points associated with it
  class tCollisionInfo
  {
  public:
    enum {MAX_COLLISION_POINTS = 10};
    
    /// tCollisionInfos will be given out from a pool.  If more than
    /// MAX_COLLISION_POINTS are passed in, the input positions will
    /// be silently truncated!
    static tCollisionInfo & GetCollisionInfo(
      const tCollDetectInfo &info, 
      const tVector3 & dirToBody0,
      const tCollPointInfo * pointInfos, ///< array of point infos
      unsigned numPointInfos     ///< size of point info array
      );

    /// Return this info to the pool
    static void FreeCollisionInfo(tCollisionInfo & info);
    
    /// gets set to true after we've been processed, and to false when the body
    /// we're asociated with has been affected by another constraint/collision
    bool mSatisfied;

    /// both skins are always set. skin0 will always have a body
    /// skin1 may or may not have a body
    tCollDetectInfo mSkinInfo;
    
    /// normalised direction in world space pointing towards body 0
    /// from the other body
    tVector3 mDirToBody0;
    
    /// Should be set during collision detection, but could be changed
    /// subsequently
    tMaterialPairProperties mMatPairProperties;
    
    tFixedVector<tCollPointInfo, MAX_COLLISION_POINTS> mPointInfo;
    
  private:
    tCollisionInfo() {}
    void Init(const tCollDetectInfo &info, 
              const tVector3 & dirToBody0,
              const tCollPointInfo * pointInfos, ///< array of point infos
              unsigned numPointInfos     ///< size of point info array
      );
    void Destroy();
    
    static std::vector<tCollisionInfo *> mFreeInfos;
  };
}

#endif
