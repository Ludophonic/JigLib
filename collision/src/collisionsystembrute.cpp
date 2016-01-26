//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionsystembrute.cpp 
//                     
//==============================================================
#include "collisionsystembrute.hpp"
#include "collisionskin.hpp"
#include "body.hpp"
#include "line.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tCollisionSystemBrute
//==============================================================
tCollisionSystemBrute::tCollisionSystemBrute()
{
  TRACE_METHOD_ONLY(ONCE_1);
  mDetecting = false;
  
}

//==============================================================
// ~tCollisionSystemBrute
//==============================================================
tCollisionSystemBrute::~tCollisionSystemBrute()
{
  TRACE_METHOD_ONLY(ONCE_1);
  int i;
  for (i = 0 ; i < (int) mSkins.size() ; ++i)
    mSkins[i]->SetCollisionSystem(0);
}

//==============================================================
// AddCollisionSkin
//==============================================================
void tCollisionSystemBrute::AddCollisionSkin(tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(skin);
  Assert(false == mDetecting);
  if (mSkins.end() == find(mSkins.begin(), mSkins.end(), skin))
    mSkins.push_back(skin);
  else
    TRACE("Warning: tried to add skin %p to tCollisionSkinBrute but "
          "it's already registered\n", skin);
  skin->SetCollisionSystem(this);
}

//==============================================================
// RemoveCollisionSkin
//==============================================================
bool tCollisionSystemBrute::RemoveCollisionSkin(tCollisionSkin * skin)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDetecting);
  skin->SetCollisionSystem(0);
  tSkins::iterator it = find(mSkins.begin(), mSkins.end(), skin);
  if (mSkins.end() == it)
    return false;
  mSkins.erase(it);
  return true;
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
  
  for (unsigned i0 = nonColl0.size() ; i0-- != 0 ; )
  {
    if (nonColl0[i0] == skin1)
      return false;
  }
  
  for (unsigned i1 = nonColl1.size() ; i1-- != 0 ; )
  {
    if (nonColl1[i1] == skin0)
      return false;
  }
  
  return true;
}

//==============================================================
// DetectCollisions 
//==============================================================
void tCollisionSystemBrute::DetectCollisions(
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
void tCollisionSystemBrute::DetectAllCollisions(
  const vector<tBody *> & bodies,
  tCollisionFunctor & collisionFunctor,
  const tCollisionSkinPredicate2 * collisionPredicate,
  tScalar collTolerance)
{
  mDetecting = true;
  unsigned numSkins = mSkins.size();
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

    for (unsigned iSkin = 0 ; iSkin < numSkins ; ++iSkin)
    {
      info.skin1 = mSkins[iSkin];
      if (info.skin0 == info.skin1)
        continue;

      Assert(info.skin1);
      bool skinSleeping = true;
      if (info.skin1->GetOwner() && (info.skin1->GetOwner()->IsActive()))
        skinSleeping = false;
    
      // only do one per pair
      if ( (skinSleeping == false) && (info.skin1 < info.skin0) )
        continue;
    
      if ( (collisionPredicate != 0) &&
           (collisionPredicate->ConsiderSkinPair(info.skin0, info.skin1) == false) )
        continue;

      // basic bbox test
      if (OverlapTest(info.skin0->GetWorldBoundingBox(),
                      info.skin1->GetWorldBoundingBox(),
                      collTolerance))
      {
        if (CheckCollidables(info.skin0, info.skin1))
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
        }
      } // overlap test
    } // loop over mSkins
  } // loop over bodies
  
  mDetecting = false;
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tCollisionSystemBrute::SegmentIntersect(
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
  
  if (fracOut > SCALAR(1.0f))
    return false;
  Limit(fracOut, SCALAR(0.0f), SCALAR(1.0f));
  return true;
}

