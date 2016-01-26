//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file boxchain.cpp 
//                     
//==============================================================
#include "boxchain.hpp"

#include "rendermanager.hpp"
#include "boxobject.hpp"

#include "jiglib.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tBoxChain
//==============================================================
tBoxChain::tBoxChain(tRenderManager * renderManager,
                     unsigned chainLength,
                     const tVector3 & pos,
                     const tVector3 & boxDims,
                     tScalar boxDensity,
                     tChainType type)
{
  unsigned iBox;
  for (iBox = 0 ; iBox < chainLength ; ++iBox)
  {
    tVector3 thisPos = pos;
    thisPos.z += 0.5f * boxDims.z;
    thisPos.y -= 0.5f * chainLength * boxDims.y;
    thisPos.y += iBox * boxDims.y;
    
    tBoxObject * boxObject = new tBoxObject(boxDims, false);
    boxObject->SetProperties(0.0f, 
                             1.0f,
                             0.7f);
    boxObject->SetDensity(boxDensity);
    mBoxes.push_back(boxObject);
    boxObject->GetBody()->MoveTo(thisPos, tMatrix33::Identity());
    renderManager->AddObject(boxObject);
    /// physics will register collision for objects passed to it.
    boxObject->GetBody()->EnableBody();
  }

  // add the joints/constraints. Also disable collisions between adjacent bodies
  for (iBox = 0 ; iBox < chainLength-1 ; ++iBox)
  {
    mBoxes[iBox]->GetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(mBoxes[iBox+1]->GetBody()->GetCollisionSkin());
    mBoxes[iBox+1]->GetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(mBoxes[iBox]->GetBody()->GetCollisionSkin());
    if (type == CHAIN_BALLNSOCKET)
    {
      tConstraint * constraint = new tConstraintPoint(
        mBoxes[iBox]->GetBody(), tVector3(0.0f, 0.5f * boxDims.y, 0.0f),
        mBoxes[iBox+1]->GetBody(), tVector3(0.0f, -0.5f * boxDims.y, 0.0f),
        0.0f,
        0.0f);
      constraint->EnableConstraint();
      mConstraints.push_back(constraint);
    }
    else
    {
      tHingeJoint * joint = new tHingeJoint;
      joint->Initialise(mBoxes[iBox]->GetBody(), mBoxes[iBox+1]->GetBody(), 
                        tVector3::Look(),
                        tVector3(0.0f, 0.5f * boxDims.y, 0.0f),
                        boxDims.GetLength(), 
                        90.0f, 90.0f,
                        0.1f,
                        0.0f);
      joint->EnableHinge();
      mHinges.push_back(joint);
    }
  }
}

//==============================================================
// ~tBoxChain
//==============================================================
tBoxChain::~tBoxChain()
{
  unsigned i;
  for (i = 0 ; i < mConstraints.size() ; ++i)
    delete mConstraints[i];
  for (i = 0 ; i < mHinges.size() ; ++i)
    delete mHinges[i];
  for (i = 0 ; i < mBoxes.size() ; ++i)
    delete mBoxes[i];
}

//==============================================================
// GetBody
//==============================================================
tBody * tBoxChain::GetBody()
{
  return mBoxes[0]->GetBody();
}


//==============================================================
// SetRenderPosition
//==============================================================
void tBoxChain::SetRenderPosition(JigLib::tScalar renderFraction)
{
  unsigned numBoxes = mBoxes.size();
  for (unsigned i = 0 ; i < numBoxes ; ++i)
    mBoxes[i]->SetRenderPosition(renderFraction);
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const tSphere & tBoxChain::GetRenderBoundingSphere() const
{
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tBoxChain::GetRenderPosition() const
{
  return mBoxes[0]->GetRenderPosition();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tBoxChain::GetRenderOrientation() const
{
  return mBoxes[0]->GetRenderOrientation();
}

