//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystemgrid.cpp 
//                     
//==============================================================
#include "collisionsystemgrid.hpp"
#include "collisionskin.hpp"
#include "body.hpp"
#include "line.hpp"

using namespace JigLib;
using namespace std;

namespace JigLib
{
  /// doubly linked list used to contain all the skins in a grid box
  class tGridEntry
  {
  public:
    tGridEntry(tCollisionSkin * skin = 0) : 
      mSkin(skin), mPrev(0), mNext(0), mGridIndex(-2) {}

    tCollisionSkin * mSkin;
    tGridEntry * mPrev;
    tGridEntry * mNext;
    int mGridIndex;
  };

  /// Removes the entry by updating its neighbours. Also zaps the prev/next
  /// pointers in the entry, to help debugging
  void RemoveGridEntry(tGridEntry * entry)
  {
    Assert(entry);
    // only the first entry should have prev = 0, and that shouldn't be removed
    Assert(entry->mPrev);
    // link the previous to the next (may be 0)
    entry->mPrev->mNext = entry->mNext;
    // link the next (if it exists) to the previous.
    if (entry->mNext)
      entry->mNext->mPrev = entry->mPrev;
    // tidy up this entry
    entry->mPrev = entry->mNext = 0;
    entry->mGridIndex = -2;
  }

  /// Inserts an entry after prev, updating all links
  void InsertGridEntryAfter(tGridEntry * entry, tGridEntry * prev)
  {
    Assert(entry);
    Assert(prev);
    tGridEntry * next = prev->mNext;
    prev->mNext = entry;
    entry->mPrev = prev;
    entry->mNext = next;
    if (next)
      next->mPrev = entry;
    entry->mGridIndex = prev->mGridIndex;
  }
}

//==============================================================
// tCollisionSystemGrid
//==============================================================
tCollisionSystemGrid::tCollisionSystemGrid(
  unsigned nx, unsigned ny, unsigned nz,
  tScalar dx, tScalar dy, tScalar dz)
{
  TRACE_METHOD_ONLY(ONCE_1);
  mDetecting = false;

  mNx = nx; mNy = ny; mNz = nz;
  mDx = dx; mDy = dy; mDz = dz;
  mSizeX = mNx * mDx;
  mSizeY = mNy * mDy;
  mSizeZ = mNz * mDz;
  mMinDelta = Min(mDx, mDy, mDz);

  mGridEntries.resize(nx * ny * nz);
  mGridBoxes.resize(nx * ny * nz);
  for (unsigned i = 0 ; i < mGridEntries.size() ; ++i)
  {
    mGridEntries[i] = new tGridEntry;
    mGridEntries[i]->mGridIndex = i;
  }
  mOverflowEntries = new tGridEntry;
  mOverflowEntries->mGridIndex = -1;

  for (unsigned iX = 0 ; iX < mNx ; ++iX)
  {
    for (unsigned iY = 0 ; iY < mNy ; ++iY)
    {
      for (unsigned iZ = 0 ; iZ < mNz ; ++iZ)
      {
        int index = CalcIndex(iX, iY, iZ);
        Assert(index >= 0);
        tAABox & box = mGridBoxes[index];
        box.AddPoint(tVector3(iX * mDx, iY * mDy, iZ * mDz));
        box.AddPoint(tVector3(iX * mDx, iY * mDy, iZ * mDz) + 
                     tVector3(mDx, mDy, mDz));
      }
    }
  }
}

//==============================================================
// ~tCollisionSystemGrid
// todo delete the grid entries
//==============================================================
tCollisionSystemGrid::~tCollisionSystemGrid()
{
  TRACE_METHOD_ONLY(ONCE_1);
  int i;
  for (i = 0 ; i < (int) mSkins.size() ; ++i)
    mSkins[i]->SetCollisionSystem(0);
}

//========================================================
// tCollisionSystemGrid
//========================================================
inline int tCollisionSystemGrid::CalcIndex(int i, int j, int k) const
{
  Assert(i >= 0 && j >= 0 && k >= 0);
  int I = i % mNx;
  int J = j % mNy;
  int K = k % mNz;
  int result = I + mNx * J + (mNx + mNy) * K;
  Assert(result < (int) mGridEntries.size());
  return result;
}

//========================================================
// CalcGridForSkin
//========================================================
void tCollisionSystemGrid::CalcGridForSkin(int & i, int & j, int & k,
                                           const tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  const tVector3 sides = skin->GetWorldBoundingBox().GetSideLengths();
  if ((sides.x > mDx) || (sides.y > mDy) || (sides.z > mDz))
  {
    TRACE("skin %p too big for gridding system - "
          "putting it into overflow list\n", skin);
    i = j = k = -1;
    return;
  }
  tVector3 min = skin->GetWorldBoundingBox().GetMinPos();
  Wrap(min.x, SCALAR(0.0f), mSizeX);
  Wrap(min.y, SCALAR(0.0f), mSizeY);
  Wrap(min.z, SCALAR(0.0f), mSizeZ);

  i = (int) (min.x / mDx) % mNx;
  j = (int) (min.y / mDy) % mNy;
  k = (int) (min.z / mDz) % mNz;
}

//========================================================
// CalcGridForSkin
//========================================================
void tCollisionSystemGrid::CalcGridForSkin(
  int & i, int & j, int & k,
  tScalar & fi, tScalar & fj, tScalar &fk,
  const tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  const tVector3 sides = skin->GetWorldBoundingBox().GetSideLengths();
  if ((sides.x > mDx) || (sides.y > mDy) || (sides.z > mDz))
  {
    TRACE_FILE_IF(MULTI_FRAME_3)
      TRACE("skin %p too big for gridding system - "
            "putting it into overflow list\n", skin);
    i = j = k = -1;
    return;
  }
  tVector3 min = skin->GetWorldBoundingBox().GetMinPos();
  Wrap(min.x, SCALAR(0.0f), mSizeX);
  Wrap(min.y, SCALAR(0.0f), mSizeY);
  Wrap(min.z, SCALAR(0.0f), mSizeZ);

  fi = min.x / mDx;
  fj = min.y / mDy;
  fk = min.z / mDz;

  i = (int) fi;
  j = (int) fj;
  k = (int) fk;

  if (i < 0) {i = 0 ; fi = SCALAR(0.0f);} 
  else if (i >= (int) mNx) {i = 0; fi = SCALAR(0.0f);}
  else fi -= (tScalar) i;

  if (j < 0) {j = 0 ; fj = SCALAR(0.0f);} 
  else if (j >= (int) mNy) {j = 0; fj = SCALAR(0.0f);}
  else fj -= (tScalar) j;

  if (k < 0) {k = 0 ; fk = SCALAR(0.0f);} 
  else if (k >= (int) mNz) {k = 0; fk = SCALAR(0.0f);}
  else fk -= (tScalar) k;
}

//========================================================
// CalcGridIndexForSkin
//========================================================
int tCollisionSystemGrid::CalcGridIndexForSkin(const tCollisionSkin * skin)
{
  int i, j, k;
  CalcGridForSkin(i, j, k, skin);
  if (i == -1)
    return -1;
  return CalcIndex(i, j, k);
}


//==============================================================
// AddCollisionSkin
//==============================================================
void tCollisionSystemGrid::AddCollisionSkin(tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDetecting);
  if (mSkins.end() == find(mSkins.begin(), mSkins.end(), skin))
    mSkins.push_back(skin);
  else
    TRACE("Warning: tried to add skin %p to tCollisionSkinGrid but "
          "it's already registered\n", skin);
  skin->SetCollisionSystem(this);

  // also do the grid stuff - for now put it on the overflow list
  tGridEntry * entry = new tGridEntry(skin);
  skin->GetExternalData().mPointer = (void *) entry;
  // add entry to the start of the list
  Assert(mOverflowEntries);
  InsertGridEntryAfter(entry, mOverflowEntries);

  // now put it in its proper place
  CollisionSkinMoved(skin);
}

//==============================================================
// RemoveCollisionSkin
//==============================================================
bool tCollisionSystemGrid::RemoveCollisionSkin(tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDetecting);
  tGridEntry * entry = (tGridEntry *) skin->GetExternalData().mPointer;
  if (entry)
  {
    RemoveGridEntry(entry);
    delete entry;
    skin->GetExternalData().mPointer = 0;
  }
  else
  {
    TRACE("Warning - skin %s being deleted without a grid entry\n", skin);
  }

  skin->SetCollisionSystem(0);
  tSkins::iterator it = find(mSkins.begin(), mSkins.end(), skin);
  if (mSkins.end() == it)
    return false;
  mSkins.erase(it);
  return true;
}

//========================================================
// CollisionSkinMoved
//========================================================
void tCollisionSystemGrid::CollisionSkinMoved(
  const class tCollisionSkin * skin)
{
  tGridEntry * entry = (tGridEntry *) skin->GetExternalData().mPointer;
  if (!entry)
  {
    TRACE("Warning = skin %s has grid entry 0!\n", skin);
    return;
  }

  int gridIndex = CalcGridIndexForSkin(skin);

  // see if it's moved grid
  if (gridIndex == entry->mGridIndex)
    return;

  // get the start of the list for the grid
  tGridEntry * start;
  if (gridIndex >= 0)
    start = mGridEntries[gridIndex];
  else
    start = mOverflowEntries;
  Assert(start);
  // start should just be a placeholder
  Assert(start->mPrev == 0);
  Assert(start->mSkin == 0);

  RemoveGridEntry(entry);
  InsertGridEntryAfter(entry, start);
}

//========================================================
// GetListsToCheck
//========================================================
void tCollisionSystemGrid::GetListsToCheck(
  vector<class tGridEntry *> & entries,
  const tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_3);
  entries.resize(0);

  tGridEntry * entry = (tGridEntry *) skin->GetExternalData().mPointer;
  if (!entry)
  {
    TRACE("Warning = skin %s has grid entry 0!\n", skin);
    return;
  }

  // todo - work back from the mGridIndex rather than calculating it again...
  int i, j, k;
  tScalar fi, fj, fk;
  CalcGridForSkin(i, j, k, fi, fj, fk, skin);

  if (-1 == i)
  {
    // oh dear - add everything!
    entries = mGridEntries;
    entries.push_back(mOverflowEntries);
    return;
  }

  // always add the overflow
  entries.push_back(mOverflowEntries);

  tAABox skinAABox = skin->GetWorldBoundingBox();
  tVector3 delta = skinAABox.GetSideLengths();

  int maxI = 1, maxJ = 1, maxK = 1;
  if (fi + (delta.x / mDx) < 1.0f)
    maxI = 0;
  if (fj + (delta.y / mDy) < 1.0f)
    maxJ = 0;
  if (fk + (delta.z / mDz) < 1.0f)
    maxK = 0;

  // now add the contents of all 18 grid boxes - their contents may extend beyond the bounds
  for (int di = -1 ; di <= maxI ; ++di)
  {
    for (int dj = -1 ; dj <= maxJ ; ++dj)
    {
      for (int dk = -1 ; dk <= maxK ; ++dk)
      {
        int thisIndex = CalcIndex(mNx + i + di, mNy + j + dj, mNz + k + dk);
        tGridEntry * start = mGridEntries[thisIndex];
        if (start->mNext)
          entries.push_back(start);
      }
    }
  }
}

//==============================================================
// CheckCollidables
// returns true if these two skins could collide
//==============================================================
static inline bool CheckCollidables(const tCollisionSkin * skin0,
                                    const tCollisionSkin * skin1)
{
  const vector<const tCollisionSkin *> & nonColl0 = skin0->GetNonCollidables();
  const vector<const tCollisionSkin *> & nonColl1 = skin1->GetNonCollidables();
  // most common case
  if (nonColl0.empty() && nonColl1.empty())
    return true;

  unsigned num0 = nonColl0.size();
  for (unsigned i0 = 0 ; i0 < num0 ; ++i0)
  {
    if (nonColl0[i0] == skin1)
      return false;
  }

  unsigned num1 = nonColl1.size();
  for (unsigned i1 = 0 ; i1 < num1 ; ++i1)
  {
    if (nonColl1[i1] == skin0)
      return false;
  }

  return true;
}
//==============================================================
// DetectCollisions 
//==============================================================
void tCollisionSystemGrid::DetectCollisions(
  tBody & body,
  tCollisionFunctor & collisionFunctor,
  const tCollisionSkinPredicate2 * collisionPredicate,
  tScalar collTolerance)
{
  if (!body.IsActive())
    return;

  tCollDetectInfo info;

  info.skin0 = body.GetCollisionSkin();
  if (!info.skin0)
    return;

  mDetecting = true;

  unsigned nBodyPrimitives = info.skin0->GetNumPrimitives();

  unsigned numSkins = mSkins.size();
  for (unsigned iSkin = 0 ; iSkin < numSkins ; ++iSkin)
  {
    info.skin1 = mSkins[iSkin];
    Assert(info.skin1);
    if ((info.skin0 != info.skin1) && CheckCollidables(info.skin0, info.skin1))
    {
      unsigned nPrimitives = info.skin1->GetNumPrimitives();

      for (info.iPrim0 = 0 ; info.iPrim0 < nBodyPrimitives ; ++info.iPrim0)
      {
        for (info.iPrim1 = 0 ; info.iPrim1 < nPrimitives ; ++info.iPrim1)
        {
          const tCollDetectFunctor * f = 
            GetCollDetectFunctor(info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetType(), 
            info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetType());
          if (f)
            f->CollDetect(info, collTolerance, collisionFunctor);
        }
      }
    }
  }
  mDetecting = false;
}

//==============================================================
// DetectAllCollisions
//==============================================================
void tCollisionSystemGrid::DetectAllCollisions(
  const vector<tBody *> & bodies,
  tCollisionFunctor & collisionFunctor,
  const tCollisionSkinPredicate2 * collisionPredicate,
  tScalar collTolerance)
{
  mDetecting = true;
  unsigned numBodies = bodies.size();

  tCollDetectInfo info;

  for (unsigned iBody = 0 ; iBody < numBodies ; ++iBody)
  {
    tBody * body = bodies[iBody];
    Assert(body);
    if (!body->IsActive())
      continue;

    info.skin0 = body->GetCollisionSkin();
    if (!info.skin0)
      continue;

    static vector<tGridEntry *> lists;
    GetListsToCheck(lists, info.skin0);
    for (unsigned iList = lists.size() ; iList-- != 0 ; )
    {
      // first one is a placeholder.
      tGridEntry * entry = lists[iList];
      Assert(entry);
      for (entry = entry->mNext ; entry != 0 ; entry = entry->mNext)
      {
        info.skin1 = entry->mSkin;
        if (info.skin1 == info.skin0)
          continue;

        Assert(info.skin1);
        bool skinSleeping = true;
        if (info.skin1->GetOwner() && (info.skin1->GetOwner()->IsActive()))
          skinSleeping = false;

        // only do one per pair
        if ( (skinSleeping == false) && (info.skin1 < info.skin0) )
          continue;

        if ( (collisionPredicate != 0) &&
             (!collisionPredicate->ConsiderSkinPair(info.skin0, info.skin1)))
          continue;

        // basic bbox test
        if (OverlapTest(info.skin1->GetWorldBoundingBox(),
                        info.skin0->GetWorldBoundingBox(),
                        collTolerance))
        {
          if (CheckCollidables(info.skin1, info.skin0))
          {
            unsigned nBodyPrimitives = info.skin0->GetNumPrimitives();
            unsigned nPrimitives = info.skin1->GetNumPrimitives();

            for (info.iPrim0 = 0 ; info.iPrim0 < nBodyPrimitives ; ++info.iPrim0)
            {
              for (info.iPrim1 = 0 ; info.iPrim1 < nPrimitives ; ++info.iPrim1)
              {
                const tCollDetectFunctor * f = 
                  GetCollDetectFunctor(info.skin0->GetPrimitiveNewWorld(info.iPrim0)->GetType(), 
                  info.skin1->GetPrimitiveNewWorld(info.iPrim1)->GetType());
                if (f)
                  f->CollDetect(info, collTolerance, collisionFunctor);
              }
            }
          } // check collidables
        } // overlap test
      } // loop over entries
    } // loop over lists
  } // loop over bodies

  mDetecting = false;
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tCollisionSystemGrid::SegmentIntersect(
  tScalar & fracOut, 
  tCollisionSkin *& skinOut, 
  tVector3 & posOut, 
  tVector3 & normalOut, 
  const class tSegment & seg, 
  const tCollisionSkinPredicate1 * collisionPredicate)
{
  mDetecting = true;
  unsigned numSkins = mSkins.size();

  tAABox segAABox;
  segAABox.AddSegment(seg);

  // initialise the outputs
  fracOut = SCALAR_HUGE;
  skinOut = 0;

  // working vars
  tScalar frac;
  tVector3 pos;
  tVector3 normal;

  for (unsigned iSkin = 0 ; iSkin < numSkins ; ++iSkin)
  {
    tCollisionSkin * skin = mSkins[iSkin];
    Assert(skin);
    if ( (collisionPredicate == 0) ||
         (collisionPredicate->ConsiderSkin(skin) == true) )
    {
      // basic bbox test
      if (OverlapTest(skin->GetWorldBoundingBox(),
                      segAABox))
      {
        if (skin->SegmentIntersect(frac, pos, normal, seg))
        {
          if (frac < fracOut)
          {
            posOut = pos;
            normalOut = normal;
            skinOut = skin;
            fracOut = frac;
          }
        }

      }
    }
  }
  mDetecting = false;

  if (fracOut > 1.0f)
    return false;
  Limit(fracOut, SCALAR(0.0f), SCALAR(1.0f));
  return true;
}

