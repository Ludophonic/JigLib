//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file skybox.cpp 
//                     
//==============================================================
#include "skybox.hpp"
#include "texture.hpp"
#include "appconfig.hpp"

#include "jiglib.hpp"

using namespace JigLib;
using namespace std;

enum tCubeSide {FRONT, LEFT, BACK, RIGHT, UP, DOWN, NUM_SIDES};

tVector3 viewDirs[6] = 
{
  tVector3(1, 0, 0), // front
  tVector3(0, 1, 0), // left
  tVector3(-1, 0, 0),// back
  tVector3(0, -1, 0),// right
  tVector3(0, 0, 1), // up
  tVector3(0, 0, -1) // down
};
tVector3 viewUps[6] = 
{
  tVector3(0, 0, 1),
  tVector3(0, 0, 1),
  tVector3(0, 0, 1),
  tVector3(0, 0, 1),
  tVector3(-1, 0, 0),
  tVector3(1, 0, 0)
};

//==============================================================
// tSkybox
//==============================================================
tSkybox::tSkybox(const string & name)
  :
  tRenderObject(BACKGROUND)
{
  string front = string("data/") + name + string("_front.png");
  string left = string("data/") + name + string("_left.png");
  string back = string("data/") + name + string("_back.png");
  string right = string("data/") + name + string("_right.png");
  string up = string("data/") + name + string("_up.png");
  string down = string("data/") + name + string("_down.png");

  mTextures[FRONT] = AllocateRGBAFileTexture(front, false, tRGBAFileTexture::CLAMP_TO_EDGE);
  mTextures[LEFT] = AllocateRGBAFileTexture(left, false, tRGBAFileTexture::CLAMP_TO_EDGE);
  mTextures[BACK] = AllocateRGBAFileTexture(back, false, tRGBAFileTexture::CLAMP_TO_EDGE);
  mTextures[RIGHT] = AllocateRGBAFileTexture(right, false, tRGBAFileTexture::CLAMP_TO_EDGE);
  mTextures[UP] = AllocateRGBAFileTexture(up, false, tRGBAFileTexture::CLAMP_TO_EDGE);
  mTextures[DOWN] = AllocateRGBAFileTexture(down, false, tRGBAFileTexture::CLAMP_TO_EDGE);
}

//==============================================================
// tSkybox
//==============================================================
tSkybox::~tSkybox()
{
  FreeAllTextures();
}

//==============================================================
// FreeAllTextures
//==============================================================
void tSkybox::FreeAllTextures()
{
  for (unsigned i = 0 ; i < NUM_SIDES ; ++i)
    FreeRGBAFileTexture(mTextures[i]);
}

//==============================================================
// Render
//==============================================================
void tSkybox::Render(tRenderType renderType)
{
  if (tAppConfig::mTextureLevel <= 0)
    return;
  tSaveGLState state;

  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DEPTH_TEST);

  for (unsigned i = 0 ; i < NUM_SIDES-1 ; ++i)
  {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, mTextures[i]->GetHighTexture());
    {
      tScalar d = 1000.0f;
      tVector3 p = d * viewDirs[i]; // the middle of the quad
      tVector3 left = d * Cross(viewUps[i], viewDirs[i]);
      const tVector3 up = d * viewUps[i];
      tVector3 p0 = p + left + up;
      tVector3 p1 = p + left - up;
      tVector3 p2 = p - left - up;
      tVector3 p3 = p - left + up;
      glBegin(GL_QUADS);
      GLTEXCOORD2(0.0f, 1.0f);
      GLVERTEX3V(p0.GetData());
      GLTEXCOORD2(0.0f, 0.0f);
      GLVERTEX3V(p1.GetData());
      GLTEXCOORD2(1.0f, 0.0f);
      GLVERTEX3V(p2.GetData());
      GLTEXCOORD2(1.0f, 1.0f);
      GLVERTEX3V(p3.GetData());
      glEnd();
    }
  }
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const tSphere & tSkybox::GetRenderBoundingSphere() const
{
  return tSphere::HugeSphere();
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tSkybox::GetRenderPosition() const
{
  return tVector3::Zero();
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tSkybox::GetRenderOrientation() const
{
  return tMatrix33::Identity();
}

