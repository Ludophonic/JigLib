//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file heightmapobject.cpp 
//                     
//==============================================================
#include "heightmapobject.hpp"
#include "imageloader.hpp"
#include "appconfig.hpp"
#include "texture.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tHeightmapObject
//==============================================================
tHeightmapObject::tHeightmapObject(const tHeightmap & heightmap)
{
  TRACE_METHOD_ONLY(ONCE_2);
  
  mTexture = AllocateRGBAFileTexture("data/terrain.png");

  mCollisionSkin.AddPrimitive(heightmap, tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(0);
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(1.0f, 0.4f, 0.3f));
  
  mDisplayListNum = 0;
}

//==============================================================
// tHeightmapObject
//==============================================================
tHeightmapObject::tHeightmapObject(const string & fileName, 
                                   tScalar dx, tScalar dy, tScalar zMin, tScalar zMax)
{
  mTexture = AllocateRGBAFileTexture("data/terrain.png");

  tArray2D<tPixel> image(0, 0);
  tArray2D<tScalar> heights(0, 0);
  bool result = LoadImage(image, fileName.c_str());

  if ( (result == false) || 
       (image.GetNx() == 0) ||
       (image.GetNy() == 0) )
  {
    TRACE("Failed to load heightmap %s\n", fileName.c_str());
    heights.Resize(3, 3);
    for (unsigned i = 0 ; i < 3 ; ++i)
      for (unsigned j = 0 ; j < 3 ; ++j)
        heights(i, j) = 0.0f;
  }
  else
  {
    unsigned nx = image.GetNx();
    unsigned ny = image.GetNy();
    heights.Resize(nx, ny);
    for (unsigned i = 0 ; i < nx ; ++i)
      for (unsigned j = 0 ; j < ny ; ++j)
        heights(i, j) = zMin + (image(i, j).r / 255.0f) * (zMax - zMin);
  }
  heights.GaussianFilter(1, 2);

  tHeightmap heightmap(heights, 0.0f, 0.0f, dx, dy);
  mCollisionSkin.AddPrimitive(heightmap, tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(0);
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(1.0f, 0.4f, 0.3f));
  
  mDisplayListNum = 0;
}


//==============================================================
// ~tHeightmapObject
//==============================================================
tHeightmapObject::~tHeightmapObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
  FreeRGBAFileTexture(mTexture);
  mTexture = 0;
}

//==============================================================
// SetProperties
//==============================================================
void tHeightmapObject::SetProperties(tScalar elasticity, tScalar staticFriction, tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// GetRenderBoundingHeightmap
//==============================================================
const tSphere & tHeightmapObject::GetRenderBoundingSphere() const
{
  // fudge this a little...
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tHeightmapObject::GetRenderPosition() const
{
  return tVector3::Zero();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tHeightmapObject::GetRenderOrientation() const
{
  return tMatrix33::Identity();
}

//==============================================================
// Render
//==============================================================
void tHeightmapObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  
  if (mDisplayListNum == 0)
  { 
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    const tHeightmap & heightmap = mCollisionSkin.GetPrimitiveLocal(0)->GetHeightmap();
    
    int nx = heightmap.GetNx();
    int ny = heightmap.GetNy();
    
    int i, j, j0;
    for (j0 = 0 ; j0 < (ny - 1) ; ++j0)
    {
      glBegin(GL_TRIANGLE_STRIP);
      for (i = 0 ; i < nx ; ++i)
      {
        int offset;
        for (offset = 1 ; offset >= 0 ; --offset)
        {
          j = j0 + offset;
          
          tVector3 pos;
          tVector3 normal;
          heightmap.GetSurfacePosAndNormal(pos, normal, i, j);
          GLNORMAL3V(normal.GetData());
          GLVERTEX3V(pos.GetData());
        }
      }
      glEnd();
    }

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
    GLCOLOR4(0.6f, 0.6f, 0.6f, 1.0f);
  }
  glCallList(mDisplayListNum);

  if (tAppConfig::mTextureLevel > 0 && mTexture)
  {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }

}

