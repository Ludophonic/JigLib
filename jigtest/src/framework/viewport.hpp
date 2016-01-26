//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file viewport.hpp 
//                     
//==============================================================
#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP
#include "jiglib.hpp"

class tViewport
{
public:
  /// sets up a viewport that is offset from bottom left and has a
  /// width/height, expressed as a fraction of the window.  Each
  /// viewport uses a camera, though it doesn't own it, so don't
  /// delete the camera before stopping using this. You could share a
  /// camera between multiple viewports if you wanted...
  tViewport(class tRenderManager * renderManager,
            class tCamera * camera,
            JigLib::tScalar offsetX, JigLib::tScalar offsetY,
            JigLib::tScalar width, JigLib::tScalar height);
  
  ~tViewport();
  
  void SetRenderManager(class tRenderManager * renderManager) {
    mRenderManager = renderManager; }
  
  void SetCamera(class tCamera * camera) {
    mCamera = camera; }
  
  const tCamera * GetCamera() const {return mCamera;}
  tCamera * GetCamera() {return mCamera;}
  
  void SetFOV(JigLib::tScalar fov) {mFOV = fov;}
  
  void Render();

  void GetPositionAndSize(JigLib::tVector2& pos, JigLib::tVector2& size) const;

  // Applies the viewport and camera (without rendering)
  void ApplyViewport() const;
  
private:
  class tRenderManager * mRenderManager;
  class tCamera * mCamera;
  
  JigLib::tScalar mOffsetX, mOffsetY;
  JigLib::tScalar mWidth, mHeight;
  
  JigLib::tScalar mClipNear;
  JigLib::tScalar mClipFar;
  JigLib::tScalar mFOV;
};

#endif
