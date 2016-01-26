//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file viewport.cpp 
//                     
//==============================================================
#include "viewport.hpp"

#include "camera.hpp"
#include "rendermanager.hpp"

#include "graphics.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tViewport
//==============================================================
tViewport::tViewport(class tRenderManager * renderManager,
                     class tCamera * camera,
                     JigLib::tScalar offsetX, JigLib::tScalar offsetY,
                     JigLib::tScalar width, JigLib::tScalar height)
  :
  mRenderManager(renderManager),
  mCamera(camera),
  mOffsetX(offsetX), mOffsetY(offsetY),
  mWidth(width), mHeight(height)
{
  TRACE_METHOD_ONLY(ONCE_1);
}

//==============================================================
// ~tViewport
//==============================================================
tViewport::~tViewport()
{
  TRACE_METHOD_ONLY(ONCE_1);
}

//==============================================================
// ApplyViewport
//==============================================================
void tViewport::ApplyViewport() const
{
  TRACE_METHOD_ONLY(FRAME_1);
  int w, h;
  GetWindowSize(w, h);
  tScalar windowW = (tScalar) w;
  tScalar windowH = (tScalar) h;
  
  // setup the projection for this viewport
  glViewport((GLsizei) (mOffsetX * windowW), 
             (GLsizei) (mOffsetY * windowH), 
             (GLsizei) (mWidth * windowW), 
             (GLsizei) (mHeight * windowH));

  // get the camera to look in the right direction etc
  tScalar aspectRatio = (windowW * mWidth)/(windowH * mHeight);
  mCamera->Apply(aspectRatio);
}

//==============================================================
// Render
//==============================================================
void tViewport::Render()
{
  TRACE_METHOD_ONLY(FRAME_1);
  
  ApplyViewport();
  
  glMatrixMode(GL_MODELVIEW);
  
  // Lighting - TODO move this
  GLfloat amb = 0.1f;
  static GLfloat white_light[]=  {1.0f,1.0f,1.0f,1.0f};
  static GLfloat ambient_light[]={amb,amb,amb,1.0f};
  static GLfloat light_pos[] = {-1000.0f, -1000.0f, 800.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0,GL_AMBIENT,ambient_light);
  glEnable(GL_LIGHT0);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  glEnable(GL_LIGHTING);
  
  // move this too
  GLfloat spec = 0.0f;
  GLfloat diff = 0.0f;
  static GLfloat mat_specular[]={spec,spec,spec,1.0f};
  static GLfloat mat_diffuse[]=  {diff,diff,diff,1.0f};
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glEnable(GL_COLOR_MATERIAL);
  
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  
  glEnable(GL_CULL_FACE);
  
  // now get things drawn
  if (mRenderManager)
    mRenderManager->RenderAllObjects(*this);
  else
    TRACE("Warning - viewport doesn't have a render manager!\n");
}

//==============================================================
// GetPositionAndSize
//==============================================================
void tViewport::GetPositionAndSize(tVector2& pos, tVector2& size) const
{
  pos.x = mOffsetX;
  pos.y = mOffsetY;
  size.x = mWidth;
  size.y = mHeight;
}
