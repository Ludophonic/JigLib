#include "spatialgrid.hpp"
#include "water.hpp"

using namespace JigLib;

//==============================================================
// tSpatialGrid
//==============================================================
tSpatialGrid::tSpatialGrid(const JigLib::tVector2 & domainMin,
                           const JigLib::tVector2 & domainMax,
                           JigLib::tScalar delta)
{
  mDomainMin = domainMin;
  mDomainMax = domainMax;

  tVector2 range = domainMax - domainMin;
  int nx = (int) (range.x / delta);
  int ny = (int) (range.y / delta);
  Assert(nx >= 1);
  Assert(ny >= 1);

  mDelta.x = range.x / nx;
  mDelta.y = range.y / ny;

  tSpatialGridEntry emptyEntry = {0};

  mGridEntries.Resize(nx, ny);
  mGridEntries.SetTo(emptyEntry);
}


//==============================================================
// PopulateGrid
//==============================================================
void tSpatialGrid::PopulateGrid(tParticle * particles, int nParticles)
{
  int i, j, iParticle;
  const int nx = mGridEntries.GetNx();
  const int ny = mGridEntries.GetNy();
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < ny ; ++j)
    {
      mGridEntries(i, j).mFirstParticle = 0;
    }
  }

  for (iParticle = 0 ; iParticle < nParticles ; ++iParticle)
  {
    tParticle & p = particles[iParticle];
    i = (int) ((p.mR.x - mDomainMin.x) / mDelta.x);
    j = (int) ((p.mR.y - mDomainMin.y) / mDelta.y);
    Limit(i, 0, nx-1);
    Limit(j, 0, ny-1);
    tSpatialGridEntry & grid = mGridEntries(i, j);

    if (0 == grid.mFirstParticle)
    {
      grid.mFirstParticle = &p;
      p.mNext = 0;
    }
    else
    {
      // insert at the beginning
      tParticle * next = grid.mFirstParticle;
      p.mNext = next;
      grid.mFirstParticle = &p;
    }
  }
}


//==============================================================
// tSpatialGridIterator
//==============================================================
tSpatialGridIterator::tSpatialGridIterator()
{
  mNParticleLists = 0;
  mCurrentListIndex = 0;
  mCurrentParticle = 0;
}

//==============================================================
// FindFirst
//==============================================================
tParticle * tSpatialGridIterator::FindFirst(const tVector2 & pos, 
                                            const tSpatialGrid & grid)
{
  const int nx = grid.mGridEntries.GetNx();
  const int ny = grid.mGridEntries.GetNy();
  int i = (int) ((pos.x - grid.mDomainMin.x) / grid.mDelta.x);
  int j = (int) ((pos.y - grid.mDomainMin.y) / grid.mDelta.y);
  Limit(i, 0, nx-1);
  Limit(j, 0, ny-1);

  mNParticleLists = 0;
  mCurrentListIndex = 0;
  mCurrentParticle = 0;

  // set up iteration through adjacent cells
  const int di[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  const int dj[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

  for (unsigned iCell = 9 ; iCell-- != 0 ; )
  {
    const int ii = i + di[iCell];
    const int jj = j + dj[iCell];
    if (ii >= 0 && ii < nx && jj >= 0 && jj < ny)
    {
      if (grid.mGridEntries(ii, jj).mFirstParticle != 0)
      {
        mParticleLists[mNParticleLists] = grid.mGridEntries(ii, jj).mFirstParticle;
        ++mNParticleLists;
        Assert(mNParticleLists <= 9);
      }
    }
  }

  if (mNParticleLists > 0)
  {
    mCurrentParticle = mParticleLists[mCurrentListIndex];
    return mCurrentParticle;
  }
  else
  {
    return 0;
  }
}

