//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file planeobject.cpp 
//                     
//==============================================================
#include "planeobject.hpp"
#include "appconfig.hpp"
#include "texture.hpp"

using namespace JigLib;

//==============================================================
// tPlaneObject
//==============================================================
tPlaneObject::tPlaneObject(const tPlane & plane)
{
  TRACE_METHOD_ONLY(ONCE_2);
  
  mTexture = AllocateRGBAFileTexture("data/terrain.png");

  mCollisionSkin.AddPrimitive(plane, tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(0);
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(1.0f, 0.4f, 0.3f));
  
  mDisplayListNum = 0;
}

//==============================================================
// ~tPlaneObject
//==============================================================
tPlaneObject::~tPlaneObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
  FreeRGBAFileTexture(mTexture);
  mTexture = 0;
}

//==============================================================
// SetProperties
//==============================================================
void tPlaneObject::SetProperties(tScalar elasticity, tScalar staticFriction, tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// GetRenderBoundingPlane
//==============================================================
const tSphere & tPlaneObject::GetRenderBoundingSphere() const
{
  // fudge this a little...
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tPlaneObject::GetRenderPosition() const
{
  return tVector3::Zero();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tPlaneObject::GetRenderOrientation() const
{
  return tMatrix33::Identity();
}

//==============================================================
// Render
//==============================================================
void tPlaneObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  
  if (mDisplayListNum == 0)
  { 
    // calculate a point on the plane
    const tPlane & plane = mCollisionSkin.GetPrimitiveLocal(0)->GetPlane();
    tVector3 point = (-PointPlaneDistance(tVector3::Zero(), plane)) * plane.GetN();
    tVector3 planeUp = plane.GetN();
    tVector3 planeLook;
    if (Dot(planeUp, tVector3::Up()) < 0.5f)
      planeLook = Cross(tVector3::Up(), planeUp).GetNormalised();
    else
      planeLook = Cross(tVector3::Look(), planeUp).GetNormalised();
    tVector3 planeLeft = Cross(planeUp, planeLook).GetNormalised();
    
    tMatrix33 mat(planeLook, planeLeft, planeUp);
    
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    ApplyTransformation(point, mat);
    gluDisk(GetGLUQuadric(), 
            0.0f, 1000.0f, // todo work out max radius somehow...
            8, 8);
    glEndList();
  }
  
  if (tAppConfig::mTextureLevel > 0 && mTexture)
  {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    GLCOLOR4(1.0f, 1.0f, 1.0f, 1.0f);

    GLfloat delta = 50.0f;
    GLfloat plane_x[] = {1.0f / delta, 0, 0, -0.5f};
    GLfloat plane_y[] = {0, 1.0f / delta, 0, -0.5f};
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane_x);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, plane_y);

    glBindTexture(GL_TEXTURE_2D, mTexture->GetHighTexture());
  }
  else
  {
    //    GLCOLOR4(RangedRandom(0.0f, 1.0f), RangedRandom(0.0f, 1.0f),RangedRandom(0.0f, 1.0f), 1.0f);
    GLCOLOR4(0.4f, 0.5f, 0.6f, 1.0f);
  }
  glCallList(mDisplayListNum);
  if (tAppConfig::mTextureLevel > 0 && mTexture)
  {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }
}

