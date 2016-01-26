//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file boxsphereobject.cpp 
//                     
//==============================================================
#include "boxsphereobject.hpp"
#include "debugconfig.hpp"
#include "texture.hpp"
#include "appconfig.hpp"

using namespace JigLib;

//==============================================================
// SetProperties
//==============================================================
void tBoxSphereObject::SetProperties(tScalar elasticity, 
                               tScalar staticFriction,
                               tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// tBoxSphereObject
//==============================================================
tBoxSphereObject::tBoxSphereObject(JigLib::tVector3 sides, bool randomColour)
{
  TRACE_METHOD_ONLY(ONCE_2);
  // todo handle sphere mass, and offsetting of CoG
  mCollisionSkin.AddPrimitive(tBox(-sides, 
                              tMatrix33::Identity(), 
                              sides), tMaterialTable::UNSET, 
                              tMaterialProperties(0.6f, 0.8f, 0.6f));
  tScalar radius = RangedRandom(0.02f * sides.GetLength(), 0.6f * sides.GetLength());
  tVector3 dir(RangedRandom(0.0f, 1.0f), RangedRandom(0.0f, 1.0f), RangedRandom(0.0f, 1.0f));
  dir.NormaliseSafe();
  mCollisionSkin.AddPrimitive(tSphere(dir * radius, radius), tMaterialTable::UNSET, 
                              tMaterialProperties(0.6f, 0.8f, 0.6f));

  mCollisionSkin.SetOwner(&mBody);
  
  mBody.SetCollisionSkin(&mCollisionSkin);
  SetMass(1.0f);
  
  mDisplayListNum = 0;
  mRandomColour = randomColour;
  mTexture = 0;
  mTexture = AllocateRGBAFileTexture("data/texture.png");
}

//==============================================================
// ~tBoxSphereObject
//==============================================================
tBoxSphereObject::~tBoxSphereObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
  FreeRGBAFileTexture(mTexture);
  mTexture = 0;
}

//==============================================================
// SetDensity
//==============================================================
void tBoxSphereObject::SetDensity(tScalar density, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  SetMass(density * mCollisionSkin.GetVolume(), massType);
}

//==============================================================
// SetMass
//==============================================================
void tBoxSphereObject::SetMass(tScalar mass, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  mBody.SetMass(mass);
  tPrimitive::tPrimitiveProperties primitiveProperties(massType, 
    tPrimitive::tPrimitiveProperties::MASS, mass);
  tScalar junk;
  tVector3 com;
  tMatrix33 it, itCoM;
  mCollisionSkin.GetMassProperties(primitiveProperties, junk, com, it, itCoM);

  bool alignBodyWithGeometry = true;
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
void tBoxSphereObject::SetRenderPosition(tScalar renderFraction)
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
  }
}

//==============================================================
// GetRenderBoundingBox
//==============================================================
const tSphere & tBoxSphereObject::GetRenderBoundingSphere() const
{
  const tBox & box = mCollisionSkin.GetPrimitiveLocal(0)->GetBox();
  const tVector3 & sides = box.GetSideLengths();
  // ugly...
  static tSphere sphere;
  sphere.SetPos(box.GetCentre());
  sphere.SetRadius(sides.GetLength());
  return sphere;
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tBoxSphereObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tBoxSphereObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}

//==============================================================
// Render
//==============================================================
void tBoxSphereObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  
  if (mDisplayListNum == 0)
  {
    const tBox & box = mCollisionSkin.GetPrimitiveLocal(0)->GetBox();
    const tVector3 & sides = box.GetSideLengths();
    const tVector3 origin = box.GetPos();
    
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    if (0)
    {
    }
    else if (mRandomColour)
    {
      GLCOLOR4(RangedRandom(0.2f, 1.0f), 
               RangedRandom(0.2f, 1.0f),
               RangedRandom(0.2f, 1.0f), 
               0.5f);
    }
    else
    {
      tScalar wavelength = 5.0f;
      const tVector3 p = TWO_PI * mBody.GetPosition() / wavelength;
      tVector3 col(
        Sin(1.0f * p.x + 1.3f * p.y + 0.6f * p.z),
        Sin(1.4f * p.x + 1.0f * p.y + 0.75f * p.z),
        Sin(0.96f * p.x + 1.13f * p.y + 1.0f * p.z) );
      col = tVector3(0.5f) + 0.5f * col;
      GLCOLOR4(col.x, col.y, col.z, 0.5f);
    }
    tVector3 BRD = origin + ElementMult(sides, tVector3(0, 0, 0));
    tVector3 BRU = origin + ElementMult(sides, tVector3(0, 0, 1));
    tVector3 BLD = origin + ElementMult(sides, tVector3(0, 1, 0));
    tVector3 BLU = origin + ElementMult(sides, tVector3(0, 1, 1));
    tVector3 FRD = origin + ElementMult(sides, tVector3(1, 0, 0));
    tVector3 FRU = origin + ElementMult(sides, tVector3(1, 0, 1));
    tVector3 FLD = origin + ElementMult(sides, tVector3(1, 1, 0));
    tVector3 FLU = origin + ElementMult(sides, tVector3(1, 1, 1));
    tVector3 fwd = tVector3::Look();
    tVector3 left = tVector3::Left();
    tVector3 up = tVector3::Up();
    tVector3 back = -fwd;
    tVector3 right = -left;
    tVector3 down = -up;
    
    tScalar minDim = Min(sides);
    tScalar texX = sides.x / minDim;
    tScalar texY = sides.y / minDim;
    tScalar texZ = sides.z / minDim;
    
    glBegin(GL_QUADS);
    
    GLNORMAL3V(fwd.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(FLD.GetData());
    GLTEXCOORD2(0, texZ);
    GLVERTEX3V(FLU.GetData());
    GLTEXCOORD2(texY, texZ);
    GLVERTEX3V(FRU.GetData());
    GLTEXCOORD2(texY, 0);
    GLVERTEX3V(FRD.GetData());
    
    GLNORMAL3V(back.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(BLU.GetData());
    GLTEXCOORD2(0, texZ);
    GLVERTEX3V(BLD.GetData());
    GLTEXCOORD2(texY, texZ);
    GLVERTEX3V(BRD.GetData());
    GLTEXCOORD2(texY, 0);
    GLVERTEX3V(BRU.GetData());
    
    GLNORMAL3V(left.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(BLD.GetData());
    GLTEXCOORD2(0, texZ);
    GLVERTEX3V(BLU.GetData());
    GLTEXCOORD2(texX, texZ);
    GLVERTEX3V(FLU.GetData());
    GLTEXCOORD2(texX, 0);
    GLVERTEX3V(FLD.GetData());
    
    GLNORMAL3V(right.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(BRU.GetData());
    GLTEXCOORD2(0, texZ);
    GLVERTEX3V(BRD.GetData());
    GLTEXCOORD2(texX, texZ);
    GLVERTEX3V(FRD.GetData());
    GLTEXCOORD2(texX, 0);
    GLVERTEX3V(FRU.GetData());
    
    GLNORMAL3V(up.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(BLU.GetData());
    GLTEXCOORD2(0, texY);
    GLVERTEX3V(BRU.GetData());
    GLTEXCOORD2(texX, texY);
    GLVERTEX3V(FRU.GetData());
    GLTEXCOORD2(texX, 0);
    GLVERTEX3V(FLU.GetData());
    
    GLNORMAL3V(down.GetData());
    GLTEXCOORD2(0, 0);
    GLVERTEX3V(BRD.GetData());
    GLTEXCOORD2(0, texY);
    GLVERTEX3V(BLD.GetData());
    GLTEXCOORD2(texX, texY);
    GLVERTEX3V(FLD.GetData());
    GLTEXCOORD2(texX, 0);
    GLVERTEX3V(FRD.GetData());
    
    glEnd();
    
    const tSphere & sphere = mCollisionSkin.GetPrimitiveLocal(1)->GetSphere();
    GLTRANSLATE(sphere.GetPos()[0], sphere.GetPos()[1], sphere.GetPos()[2]);
    gluSphere(GetGLUQuadric(), 
              mCollisionSkin.GetPrimitiveLocal(1)->GetSphere().GetRadius(), 
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
  
  if (tAppConfig::mTextureLevel > 0 && mTexture)
  {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, mTexture->GetHighTexture());
    GLCOLOR4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  glCallList(mDisplayListNum);
  
  if (tAppConfig::mTextureLevel > 0 && mTexture)
  {
    glDisable(GL_TEXTURE_2D);
  }

  if (translucent && tDebugConfig::mIndicateFrozenObjects)
    glDisable(GL_BLEND);
}

