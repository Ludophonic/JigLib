//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file camera.cpp 
//                     
//==============================================================
#include "camera.hpp"
#include "viewport.hpp"
#include "renderobject.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

using namespace JigLib;

//==============================================================
// tCamera
//==============================================================
tCamera::tCamera() 
  :
  mPosition(0),
  mOffset(0),
  mOffsetAbsolute(false),
  mTarget(0),
  mUp(tVector3::Up()),
  mOrient(tMatrix33::IDENTITY),
  mTrackObject(0),
  mTrackDist(0),
  mTrackDeltaHeight(0),
  mTrackSpeed(0),
  mTrackPosition(0),
  mPositionObject(0),
  mTargetObject(0),
  mUpObject(0),
  mOrientationObject(0)
{
  TRACE_METHOD_ONLY(ONCE_1);
  mVel.SetTo(0.0f);
  mClipNear = 0.1f;
  mClipFar = 5000.0f;
  mFOV = 75.0f;
}

//==============================================================
// ~tCamera
//==============================================================
tCamera::~tCamera()
{
  TRACE_METHOD_ONLY(ONCE_1);
}

//==============================================================
// SetTrackTarget
//==============================================================
void tCamera::SetTrackTarget(tRenderObject * trackObject, 
                             tScalar dist, 
                             tScalar deltaHeight,
                             tScalar speed)
{
  mTrackObject = trackObject;
  mTrackDist = dist;
  mTrackDeltaHeight = deltaHeight;
  mTrackSpeed = speed;
  
  mVel.SetTo(0.0f);
}

//==============================================================
// Apply
//==============================================================
void tCamera::Apply(tScalar aspectRatio)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 
    mFOV,
    aspectRatio,
    mClipNear, mClipFar);

  TRACE_METHOD_ONLY(FRAME_1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  gluLookAt(mPosition[0], mPosition[1], mPosition[2],
            mTarget[0], mTarget[1], mTarget[2],
            mUp[0], mUp[1], mUp[2]);
}

//==============================================================
// update
//==============================================================
void tCamera::Update(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  // update the camera properties
  if (mTrackObject)
  {
    tVector3 idealPos = mTrackObject->GetRenderPosition();
    idealPos += tVector3::Up() * mTrackDeltaHeight;
    mUp = tVector3::Up();
    if (mTargetObject)
      mTarget = mTargetObject->GetRenderPosition();
      
    if (mChase)
    {
      tScalar idealDist = mOffset.GetLength();
      idealPos += idealDist * (mPosition - idealPos).GetNormalisedSafe();
    }
    else
    {
      if (mOffsetAbsolute)
        idealPos += mOffset;
      else
        idealPos += mTrackObject->GetRenderOrientation() * mOffset;
    }
    SmoothCD(mPosition, mVel, dt, idealPos, 0.2f);
  }
  else 
  {
    if (mPositionObject)
    {
      if (mOffsetAbsolute)
        mPosition = mPositionObject->GetRenderPosition() + mOffset;
      else
        mPosition = mPositionObject->GetRenderPosition() + 
          mPositionObject->GetRenderOrientation() * mOffset;
    }
    if (mOrientationObject)
    {
      mTarget = mPosition + 
        mOrientationObject->GetRenderOrientation().GetCol(0);
      mUp = mOrientationObject->GetRenderOrientation().GetCol(2);
    }
    else
    {
      if (mTargetObject)
        mTarget = mTargetObject->GetRenderPosition();
      if (mUpObject)
        mUp = mUpObject->GetRenderOrientation().GetCol(2);
    }
  }
  
  mOrient = tMatrix33((mTarget - mPosition).GetNormalised(),
                      Cross(mUp, mTarget - mPosition).GetNormalised(),
                      mUp);
}

//==============================================================
// set_position
//==============================================================
void tCamera::SetPosition(const tVector3 & position)
{
  TRACE_METHOD_ONLY(FRAME_2);
  mPosition = position;
}

//==============================================================
// set_target
//==============================================================
void tCamera::SetTarget(const tVector3 & target)
{
  TRACE_METHOD_ONLY(FRAME_2);
  mTarget = target;
}

//==============================================================
// set_up
//==============================================================
void tCamera::SetUp(const tVector3 & up)
{
  TRACE_METHOD_ONLY(FRAME_2);
  mUp = up;
}

//==============================================================
// set_position_object
//==============================================================
void tCamera::SetPositionObject(tRenderObject * positionObject)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mPositionObject = positionObject;
}

//==============================================================
// set_target_object
//==============================================================
void tCamera::SetTargetObject(tRenderObject * targetObject)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mTargetObject = targetObject;
}

//==============================================================
// set_up_object
//==============================================================
void tCamera::SetUpObject(tRenderObject * upObject)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mUpObject = upObject;
}

//==============================================================
// set_orientation_object
//==============================================================
void tCamera::SetOrientationObject(tRenderObject * orientationObject)
{
  TRACE_METHOD_ONLY(ONCE_1);
  mOrientationObject = orientationObject;
}

//==============================================================
// Unproject
// See the OpenGL red book, page 152
//==============================================================
tVector3 tCamera::Unproject(const tVector2& screenPt, const tViewport* viewport) const
{
  if (!viewport)
    return tVector3(0, 0, 0);

  int x = (int) screenPt.x;
  int y = (int) screenPt.y;

  GLint vp[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLint realy;         // convertet y coord
  GLdouble wx, wy, wz; // return value

  viewport->ApplyViewport();

  glGetIntegerv(GL_VIEWPORT, vp);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  realy = vp[3] - (GLint) y - 1;
  int result = gluUnProject((GLdouble) x, (GLdouble) realy, 1.0, mvmatrix, projmatrix, vp, &wx, &wy, &wz);
  if (result == GL_TRUE)
  {
    return tVector3((tScalar) wx, (tScalar) wy, (tScalar) wz);
  }
  else
  {
    TRACE("Failed to unproject point\n");
    return tVector3(0, 0, 0);
  }
}

