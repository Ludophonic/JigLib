//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file sphereobject.cpp 
//                     
//==============================================================
#include "sphereobject.hpp"
#include "debugconfig.hpp"

using namespace JigLib;

//==============================================================
// SetProperties
//==============================================================
void tSphereObject::SetProperties(tScalar elasticity, tScalar staticFriction, tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// tSphereObject
//==============================================================
tSphereObject::tSphereObject(tScalar radius)
{
  TRACE_METHOD_ONLY(ONCE_2);
  
  mCollisionSkin.AddPrimitive(tSphere(tVector3(0.0f * radius), radius), tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(&mBody);
  SetProperties(0.6f, 0.8f, 0.6f);
  
  mBody.SetCollisionSkin(&mCollisionSkin);
  SetMass(1.0f);
  
  mDisplayListNum = 0;
}

//==============================================================
// ~tSphereObject
//==============================================================
tSphereObject::~tSphereObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// SetDensity
//==============================================================
void tSphereObject::SetDensity(tScalar density, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  SetMass(density * mCollisionSkin.GetVolume(), massType);
}

//==============================================================
// SetMass
//==============================================================
void tSphereObject::SetMass(tScalar mass, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  mBody.SetMass(mass);
  tPrimitive::tPrimitiveProperties primitiveProperties(massType, 
    tPrimitive::tPrimitiveProperties::MASS, mass);
  tScalar junk;
  tVector3 com;
  tMatrix33 it, itCoM;
  mCollisionSkin.GetMassProperties(primitiveProperties, junk, com, it, itCoM);

  bool alignBodyWithGeometry = false;
  if (alignBodyWithGeometry)
  {
    // move body to geometry
    mBody.MoveTo(mBody.GetPosition() + com, mBody.GetOrientation());
    mCollisionSkin.ApplyLocalTransform(tTransform3(-com, tMatrix33::Identity()));
    mBody.SetBodyInertia(itCoM);
  }
  else
  {
    mBody.SetBodyInertia(it);
  }
}


//==============================================================
// SetRenderPosition
//==============================================================
void tSphereObject::SetRenderPosition(tScalar renderFraction)
{
  TRACE_METHOD_ONLY(FRAME_2);
  if (renderFraction < SCALAR_TINY)
  {
    mRenderPosition = mBody.GetOldPosition();
    mRenderOrientation = mBody.GetOldOrientation();
  }
  else if (renderFraction > (1.0f - SCALAR_TINY))
  {
    mRenderPosition = mBody.GetPosition();
    mRenderOrientation = mBody.GetOrientation();
  }
  else
  {
    mRenderPosition = 
      mBody.GetPosition() * renderFraction + 
      mBody.GetOldPosition() * (1.0f - renderFraction);
    // should really use quaternions and SLERP... but maybe if the
    // object is spinning so fast then it doesn't matter if it renders
    // a bit wrong...
    mRenderOrientation = 
      mBody.GetOrientation() * renderFraction + 
      mBody.GetOldOrientation() * (1.0f - renderFraction);
//    mRenderOrientation.Orthonormalise();
  }
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const tSphere & tSphereObject::GetRenderBoundingSphere() const
{
  // fudge this a little...
  return mCollisionSkin.GetPrimitiveNewWorld(0)->GetSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tSphereObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tSphereObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}

//==============================================================
// Render
//==============================================================
void tSphereObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  
  if (mDisplayListNum == 0)
  {
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    GLCOLOR4(RangedRandom(0.2f, 1.0f), 
             RangedRandom(0.2f, 1.0f),
             RangedRandom(0.2f, 1.0f), 
             0.5f);
    const tSphere &sphere = mCollisionSkin.GetPrimitiveLocal(0)->GetSphere();
    GLTRANSLATE(sphere.GetPos().x, sphere.GetPos().y, sphere.GetPos().z);
    gluSphere(GetGLUQuadric(), 
              sphere.GetRadius(), 
              12, 12);
    glEndList();
  }
  
  if (tDebugConfig::mRenderCollisionBoundingBoxes)
    RenderWorldBoundingBox();
  
  ApplyTransformation(mRenderPosition, mRenderOrientation);
  bool translucent = !mBody.IsActive();
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  glCallList(mDisplayListNum);
  
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
    glDisable(GL_BLEND);
}

