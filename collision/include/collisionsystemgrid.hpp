//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystemgrid.hpp 
//                     
//==============================================================
#ifndef JIGCOLLISIONSYSTEMGRID_HPP
#define JIGCOLLISIONSYSTEMGRID_HPP

#include "../collision/include/collisionsystem.hpp"

#include <vector>

namespace JigLib
{
  /// implements a collision system by dividing the world up into a wrapping
  /// grid with a certain configurable size. If objects are evenly distributed
  /// this will reduce the number of checks that need to be made.
  class tCollisionSystemGrid : public tCollisionSystem
  {
  public:
    tCollisionSystemGrid(unsigned nx, unsigned ny, unsigned nz,
                         tScalar dx, tScalar dy, tScalar dz);
    ~tCollisionSystemGrid();
    
    // inherited
    void AddCollisionSkin(class tCollisionSkin * skin);
    
    // inherited
    bool RemoveCollisionSkin(class tCollisionSkin * skin);
    
    // inherited
    void CollisionSkinMoved(const class tCollisionSkin * skin);

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
    /// calculate the array index for a given i, j, k value, which 
    /// may be > nx etc
    inline int CalcIndex(int i, int j, int k) const;

    /// Calculate the grid index that this skin should reside in
    /// if -1 is returned it means use the overflow list
    int CalcGridIndexForSkin(const tCollisionSkin * skin);

    /// Calculate the i, j, k indices - all will be set to -1 if
    /// it means use the overflow list
    /// The fractional part of the indices are returned too - so
    /// you could re-place the skin position using (i + fi) etc
    void CalcGridForSkin(int & i, int & j, int & k,
                         tScalar & fi, tScalar & fj, tScalar &fk,
                         const tCollisionSkin * skin);

    /// As the other version, but doesn't calculate the
    /// fractional parts
    void CalcGridForSkin(int & i, int & j, int & k,
                         const tCollisionSkin * skin);

    /// populates lists with the starts of the lists that need
    /// to be checked for the skin. If skin itself is in the overflow
    /// list this will return everything!
    void GetListsToCheck(std::vector<class tGridEntry *> & lists,
                         const tCollisionSkin * skin);

    /// mGridEntries is an array, indexed using CalcIndex, of
    /// lists of objects in the grid box. The first entry is just
    /// a placeholder, and will have the collisionskin and prev
    /// pointer zero.
    std::vector<class tGridEntry *> mGridEntries;

    /// Store the grid box extents
    std::vector<tAABox> mGridBoxes;

    /// This points to the start of the list of all objects that don't fit into 
    /// the gridding system, probably because they're too big. Same as for
    /// mGridEntries
    class tGridEntry * mOverflowEntries;

    unsigned mNx, mNy, mNz;
    tScalar mDx, mDy, mDz;
    /// Just cached values of mNx * mDx etc
    tScalar mSizeX, mSizeY, mSizeZ;

    /// minimum of the grid deltas
    tScalar mMinDelta;

    typedef std::vector<tCollisionSkin *> tSkins;
    tSkins mSkins;
    
    bool mDetecting;
  };
}

#endif
