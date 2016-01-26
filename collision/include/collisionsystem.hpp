//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystem.hpp 
//                     
//==============================================================
#ifndef JIGCOLLISIONSYSTEM_HPP
#define JIGCOLLISIONSYSTEM_HPP

#include "../collision/include/collisioninfo.hpp"
#include "../collision/include/materials.hpp"

#include <vector>
#include <string>

/// Currently this class introduces a circular dependency between
/// tPhysicsSystem and tCollisionSystem - i.e. both use the other (via
/// tBody). Not sure if this is really nice...

namespace JigLib
{
  /// The user of tCollisionSystem creates an object derived from
  /// tCollisionFunctor and passes it in to
  /// tCollisionSystem::DetectCollisions. For ever collision detected
  /// the functor gets called so that the user can decide if they want
  /// to keep the collision.
  class tCollisionFunctor
  {
  public:
    virtual ~tCollisionFunctor() {}
    /// Skins are passed back because there maybe more than one skin
    /// per body, and the user can always get the body from the skin.
    virtual void CollisionNotify(const tCollDetectInfo &collDetectInfo,
                                 const tVector3 & dirToBody0,
                                 const tCollPointInfo * pointInfos,
                                 unsigned numPointInfos) = 0;
  };

  /// The user can create an object derived from this and pass it in
  /// to tCollisionSystem::DetectCollisions to indicate whether a pair
  /// of skins should be considered. 
  class tCollisionSkinPredicate2
  {
  public:
    virtual ~tCollisionSkinPredicate2() {}
    virtual bool ConsiderSkinPair(class tCollisionSkin * skin0,
                                  class tCollisionSkin * skin1) const = 0;
  };

  /// The user can create an object derived from this and pass it in
  /// to the ray/segment intersection functions to indicate whether certain
  /// skins should be considered. 
  class tCollisionSkinPredicate1
  {
  public:
    virtual ~tCollisionSkinPredicate1() {}
    virtual bool ConsiderSkin(class tCollisionSkin * skin0) const = 0;
  };

  /// Used during setup - allow the creator to register functors to do
  /// the actual collision detection. Each functor inherits from this
  /// - has a name to help debugging!  The functor has to be able to
  /// handle the primitivs being passed to it in either order.
  class tCollDetectFunctor
  {
  public:
    tCollDetectFunctor(const std::string & name,
                       unsigned primType0, 
                       unsigned primType1) : 
      mName(name), mType0(primType0), mType1(primType1) {}
    virtual ~tCollDetectFunctor() {}
    
    virtual void CollDetect(const tCollDetectInfo &info,
                            tScalar collTolerance,
                            tCollisionFunctor & collisionFunctor) const = 0;
    std::string mName;
    unsigned mType0, mType1;
  };

  /// Interface to a class that will contain a list of all the
  /// collision objects in the world, and it will provide ways of
  /// detecting collisions between other objects and these collision
  /// objects.
  class tCollisionSystem
  {
  public:
    /// The implementation of the derived class should have a flag
    /// that gets set during collision detection to make sure that
    /// skins don't get added/removed during that.
    tCollisionSystem();
    virtual ~tCollisionSystem();

    /// don't add skins whilst doing detection!
    virtual void AddCollisionSkin(class tCollisionSkin * skin) = 0;

    /// don't remove skins whilst doing detection!
    virtual bool RemoveCollisionSkin(class tCollisionSkin * skin) = 0;

    /// whenever a skin changes position it will call this to let us
    /// update our internal state
    virtual void CollisionSkinMoved(const class tCollisionSkin * skin) {}

    /// Detects all collisions between the body and all the registered
    /// collision skins (which should have already had their
    /// positions/bounding volumes etc updated).  For each potential
    /// pair of skins then the predicate (if it exists) will be called
    /// to see whether or not to continue. If the skins are closer
    /// than collTolerance (+ve value means report objects that aren't
    /// quite colliding) then the functor will get called.
    /// You can't just loop over all your bodies calling this, because 
    /// that will double-detect collisions. Use DetectAllCollisions for 
    /// that.
    virtual void DetectCollisions(
				  class tBody & body,
				  tCollisionFunctor & collisionFunctor,
				  const tCollisionSkinPredicate2 * collisionPredicate,
				  tScalar collTolerance) = 0;

    /// As DetectCollisions but detects for all bodies, testing each pair 
    /// only once
    virtual void DetectAllCollisions(
				     const std::vector<class tBody *> & bodies,
				     tCollisionFunctor & collisionFunctor,
				     const tCollisionSkinPredicate2 * collisionPredicate,
				     tScalar collTolerance) = 0;

    /// type0/1 could be from tCollisionSkinType or they could be
    /// larger values. The collision detection table will get extended
    /// as necessary. You only need to register the function once
    /// (i.e. not for type0, type1 then type1, type 0).
    void RegisterCollDetectFunctor(tCollDetectFunctor & f);

    /// Get the previously registered function for the pair type. May
    /// return 0.
    inline tCollDetectFunctor * GetCollDetectFunctor(unsigned type0,
                                                     unsigned type1);

    /// Intersect a segment with the world. If non-zero the predicate
    /// allows certain skins to be excluded
    virtual bool SegmentIntersect(
				  tScalar & frac, 
				  tCollisionSkin *& skin, 
				  tVector3 & pos, 
				  tVector3 & normal, 
				  const class tSegment & seg, 
				  const tCollisionSkinPredicate1 * collisionPredicate) = 0;

    /// Sets whether collision tests should use sweep or overlap
    void SetUseSweepTests(bool use) {mUseSweepTests = use;}

    bool GetUseSweepTests() const {return mUseSweepTests;}
    
    const tMaterialTable &GetMaterialTable() const {return mMaterialTable;}
    tMaterialTable &GetMaterialTable() {return mMaterialTable;}

  private:
    std::vector< std::vector<tCollDetectFunctor *> > mDetectionFunctors;

    bool mUseSweepTests;
    tMaterialTable mMaterialTable;
  };


  //==============================================================
  // GetCollDetectFunction
  //==============================================================
  inline tCollDetectFunctor * tCollisionSystem::GetCollDetectFunctor(
								     unsigned type0,
								     unsigned type1)
  {
    if (type0 >= mDetectionFunctors.size())
      return 0;
    std::vector<tCollDetectFunctor *> & fs = mDetectionFunctors[type0];
    if (type1 >= fs.size())
      return 0;
    return fs[type1];
  }

}

#endif
