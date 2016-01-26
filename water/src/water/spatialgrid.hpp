#ifndef SPATIALGRID_HPP
#define SPATIALGRID_HPP

#include "jiglib.hpp"

#include "water.hpp"

//#include "windows.h"

/// Class to iterate over the particles in the vicinity of a particular position
class tSpatialGridIterator
{
public:
  /// constructor sets the internal state to be sensible,
  tSpatialGridIterator();

  /// This does two things - it does a lookup based on pos and
  /// calculates all the info it will need for subsequent traversals.
  /// Then it returns the first particle. Subsequent particles are
  /// returned by calls to GetNext. May return 0
  tParticle * FindFirst(const JigLib::tVector2 & pos,
                        const class tSpatialGrid & grid);

  /// updates the internal iterator state and returns the next
  /// particle. Returns 0 if there are no more particles.
  inline tParticle * GetNext(const class tSpatialGrid & grid);
private:
  /// an array of linked lists - each entry points to the first
  /// element of the list. Since this is a 2D grid there are 9
  /// adjacent grid cells that might have a particle
  tParticle * mParticleLists[9];

  /// The number of lists actually in use.
  int mNParticleLists;

  /// current index into mParticleLists
  int mCurrentListIndex;

  /// current particle that we've iterated to and already returned
  /// using FindFirst or GetNext
  tParticle * mCurrentParticle;
};


class tSpatialGrid
{
public:
  /// sets up the internal grid so that it ranges over the domain
  /// specified, and each grid cell is AT LEAST of size delta. This
  /// means that for every point in a cell, all other objects within a
  /// distance less than delta can be found by traversing the
  /// neighbouring cells
  tSpatialGrid(const JigLib::tVector2 & domainMin,
               const JigLib::tVector2 & domainMax,
               JigLib::tScalar delta);

  /// 
  void PopulateGrid(tParticle * particles, int nParticles);

private:
  /// let the iterator look at our privates
  friend class tSpatialGridIterator;

  /// for internal use by tSpatialGrid
  class tSpatialGridEntry
  {
  public:
    /// Store the first element in a linked list - tParticle itself
    /// contains the "next" pointer
    tParticle * mFirstParticle;
  };

  JigLib::tArray2D<tSpatialGridEntry> mGridEntries;

  /// location of the bottom left corner of the domain
  JigLib::tVector2 mDomainMin;
  /// location of the top right corner of the domain
  JigLib::tVector2 mDomainMax;
  /// size of each grid cell
  JigLib::tVector2 mDelta;
};

//==============================================================
// GetNext
//==============================================================
inline tParticle * tSpatialGridIterator::GetNext(const tSpatialGrid & grid)
{
  if (0 == mCurrentParticle)
    return 0;

  mCurrentParticle = mCurrentParticle->mNext;

  if (mCurrentParticle)
    return mCurrentParticle;

  // may be some more lists to traverse
  if (++mCurrentListIndex >= mNParticleLists)
    return 0;

  return (mCurrentParticle = mParticleLists[mCurrentListIndex]);
}


#endif
