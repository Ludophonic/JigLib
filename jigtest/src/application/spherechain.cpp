//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file spherechain.cpp 
//                     
//==============================================================
#include "spherechain.hpp"

#include "rendermanager.hpp"
#include "sphereobject.hpp"

#include "jiglib.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tSphereChain
//==============================================================
tSphereChain::tSphereChain(tRenderManager * renderManager,
                           unsigned chainLength,
                           const tVector3 & pos,
                           const tScalar radius,
                           tScalar sphereDensity,
                           tChainType type)
{
  unsigned iSphere;
  for (iSphere = 0 ; iSphere < chainLength ; ++iSphere)
  {
    tVector3 thisPos = pos;
    thisPos.z += radius;
    thisPos.y -= chainLength * radius;
    thisPos.y += 2.0f * iSphere * radius;
    
    tSphereObject * sphereObject = new tSphereObject(radius);
    sphereObject->SetProperties(0.0f, 
                                1.0f,
                                0.7f);
    sphereObject->SetDensity(sphereDensity);
    mSpheres.push_back(sphereObject);
    sphereObject->GetBody()->MoveTo(thisPos, tMatrix33::Identity());
    renderManager->AddObject(sphereObject);
    /// physics will register collision for objects passed to it.
    sphereObject->GetBody()->EnableBody();
  }

  // add the joints/constraints. Also disable collisions between adjacent bodies
  for (iSphere = 0 ; iSphere < chainLength-1 ; ++iSphere)
  {
    mSpheres[iSphere]->GetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(mSpheres[iSphere+1]->GetBody()->GetCollisionSkin());
    mSpheres[iSphere+1]->GetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(mSpheres[iSphere]->GetBody()->GetCollisionSkin());
    if (type == CHAIN_BALLNSOCKET)
    {
      tConstraint * constraint = new tConstraintPoint(
        mSpheres[iSphere]->GetBody(), tVector3(0.0f, 0.0f, 0.0f),
        mSpheres[iSphere+1]->GetBody(), tVector3(0.0f, -2.0f * radius, 0.0f),
        0.0f,
        0.0f);
      constraint->EnableConstraint();
      mConstraints.push_back(constraint);
    }
    else
    {
      tHingeJoint * joint = new tHingeJoint;
      joint->Initialise(mSpheres[iSphere]->GetBody(), mSpheres[iSphere+1]->GetBody(), 
                        tVector3::Look(),
                        tVector3(0.0f, 0.0f, 0.0f),
                        radius, 
                        90.0f, 90.0f,
                        0.0f,
                        0.1f);
      joint->EnableHinge();
      mHinges.push_back(joint);
    }
  }
}

//==============================================================
// ~tSphereChain
//==============================================================
tSphereChain::~tSphereChain()
{
  unsigned i;
  for (i = 0 ; i < mConstraints.size() ; ++i)
    delete mConstraints[i];
  for (i = 0 ; i < mHinges.size() ; ++i)
    delete mHinges[i];
  for (i = 0 ; i < mSpheres.size() ; ++i)
    delete mSpheres[i];
}

//==============================================================
// GetBody
//==============================================================
tBody * tSphereChain::GetBody()
{
  return mSpheres[0]->GetBody();
}


//==============================================================
// SetRenderPosition
//==============================================================
void tSphereChain::SetRenderPosition(JigLib::tScalar renderFraction)
{
  unsigned numSpheres = mSpheres.size();
  for (unsigned i = 0 ; i < numSpheres ; ++i)
    mSpheres[i]->SetRenderPosition(renderFraction);
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const tSphere & tSphereChain::GetRenderBoundingSphere() const
{
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tSphereChain::GetRenderPosition() const
{
  return mSpheres[0]->GetRenderPosition();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tSphereChain::GetRenderOrientation() const
{
  return mSpheres[0]->GetRenderOrientation();
}

