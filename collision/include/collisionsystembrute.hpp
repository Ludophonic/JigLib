//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystembrute.hpp 
//                     
//==============================================================
#ifndef JIGCOLLISIONSYSTEMBRUTE_HPP
#define JIGCOLLISIONSYSTEMBRUTE_HPP

#include "../collision/include/collisionsystem.hpp"

#include <vector>

/// Implements a "brute-force" collision system with no culling other
/// than bounding volume checks.

namespace JigLib
{
  class tCollisionSystemBrute : public tCollisionSystem
  {
  public:
    tCollisionSystemBrute();
    ~tCollisionSystemBrute();
    
    // inherited
    void AddCollisionSkin(class tCollisionSkin * skin);
    
    // inherited
    bool RemoveCollisionSkin(class tCollisionSkin * skin);
    
    // inherited
    void DetectCollisions(
      class tBody & body,
      tCollisionFunctor & collisionFunctor,
      const tCollisionSkinPredicate2 * collisionPredicate,
      tScalar collTolerance);
    
    // inherited
    void DetectAllCollisions(
      const std::vector<class tBody *> & bodies,
      tCollisionFunctor & collisionFunctor,
      const tCollisionSkinPredicate2 * collisionPredicate,
      tScalar collTolerance);
    
    bool SegmentIntersect(
      tScalar & frac, 
      tCollisionSkin *& skin, 
      tVector3 & pos, 
      tVector3 & normal, 
      const class tSegment & seg, 
      const tCollisionSkinPredicate1 * collisionPredicate);
    
  private:
    typedef std::vector<tCollisionSkin *> tSkins;
    tSkins mSkins;
    
    bool mDetecting;
  };
}

#endif
