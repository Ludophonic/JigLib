//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file newton.cpp
//                     
//==============================================================
#include "newton.hpp"
#include "renderManager.hpp"

using namespace std;
using namespace JigLib;

//========================================================
// tNewton
//========================================================
tNewton::tNewton(class tRenderManager * render,
                 unsigned numBalls,
                 const JigLib::tVector3 & pos,
                 JigLib::tScalar radius)
{
  unsigned i;
  tScalar yOffset = 2.0f * radius;
  tScalar zOffset = 5.0f * radius;
  Assert(numBalls > 0);
  mBalls.resize(numBalls);
  mConstraints.resize(numBalls*2);
  for (i = 0 ; i < numBalls ; ++i)
  {
    mBalls[i] = new tSphereObject(radius);
    mBalls[i]->GetBody()->MoveTo(pos + i * 2.0f * radius * tVector3(1.0f, 0.0f, 0.0f), tMatrix33::Identity());
    mBalls[i]->SetProperties(1.0f, 0.0f, 0.0f);
    mBalls[i]->SetDensity(1000.0f);
    mBalls[i]->GetBody()->SetActivityThreshold(2.0f, 180.0f);
    mBalls[i]->GetBody()->SetDoShockProcessing(false);
    mBalls[i]->GetBody()->EnableBody();
    mBalls[i]->GetBody()->SetAllowFreezing(false);
    render->AddObject(mBalls[i]);

    unsigned i0 = i*2;
    unsigned i1 = i0 + 1;
    mConstraints[i0] = new tConstraintWorldPoint(
      mBalls[i]->GetBody(), 
      tVector3(0.0f, yOffset, zOffset),
      mBalls[i]->GetBody()->GetPosition() + tVector3(0.0f, yOffset, zOffset));

    mConstraints[i1] = new tConstraintWorldPoint(
      mBalls[i]->GetBody(), 
      tVector3(0.0f, -yOffset, zOffset),
      mBalls[i]->GetBody()->GetPosition() + tVector3(0.0f, -yOffset, zOffset));

    mConstraints[i0]->EnableConstraint();
    mConstraints[i1]->EnableConstraint();
  }

  mBalls[0]->GetBody()->MoveTo(
    pos + zOffset * tVector3(-1.0f, 0.0f, 1.0f), 
    Matrix33Beta(90.0f));
//  mBalls[0]->GetBody()->SetVelocity(tVector3(-10 * radius, 0.0f, 0.0f));

}

//========================================================
// tNewton
//========================================================
tNewton::~tNewton()
{
  unsigned i;
  for (i = 0 ; i < mConstraints.size() ; ++i)
  {
    delete mConstraints[i];
  }
  for (i = 0 ; i < mBalls.size() ; ++i)
  {
    delete mBalls[i];
  }
}

//========================================================
// GetBody
//========================================================
JigLib::tBody * tNewton::GetBody()
{
  return mBalls[0]->GetBody();
}

//========================================================
// SetRenderPosition
//========================================================
void tNewton::SetRenderPosition(JigLib::tScalar renderFraction)
{
  for (unsigned i = 0 ; i < mBalls.size() ; ++i)
    mBalls[i]->SetRenderPosition(renderFraction);
}

//========================================================
// GetRenderBoundingSphere
//========================================================
const JigLib::tSphere & tNewton::GetRenderBoundingSphere() const
{
  return tSphere::HugeSphere();
}

//========================================================
// GetRenderPosition
//========================================================
const JigLib::tVector3 & tNewton::GetRenderPosition() const
{
  return mBalls[0]->GetRenderPosition();
}

//========================================================
// GetRenderOrientation
//========================================================
const JigLib::tMatrix33 & tNewton::GetRenderOrientation() const
{
  return mBalls[0]->GetRenderOrientation();
}
