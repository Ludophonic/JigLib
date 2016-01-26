//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file camera.hpp 
//                     
//==============================================================
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "jiglib.hpp"

// this is a kind of "does-everything" camera - should really split it
// up to contain different types - e.g. orthogrphic, follow etc.
class tCamera
{
public:
  tCamera();
  ~tCamera();
  
  /// Update the camera position etc (e.g. follow the target)
  void Update(JigLib::tScalar dt);
  
  /// set up the perspective view for the current parameters
  void Apply(JigLib::tScalar aspectRatio);
  
  void SetPosition(const JigLib::tVector3 & position);
  /// absolute if the offset is in world space (i.e. an absolute offset)
  void SetPositionOffset(const JigLib::tVector3 & offset, bool absolute=false)
    {mOffset = offset; mOffsetAbsolute = absolute;} // when using target object
  
  void SetTarget(const JigLib::tVector3 & target);
  void SetUp(const JigLib::tVector3 & up);
  
  void SetTrackTarget(class tRenderObject * targetObject,
                      JigLib::tScalar dist,
                      JigLib::tScalar deltaHeight, 
                      JigLib::tScalar speed);
  
  void SetChase(bool chase) {mChase = chase;}

  const JigLib::tVector3 & GetPosition() const {return mPosition;}
  const JigLib::tVector3 & GetTarget() const {return mTarget;}
  const JigLib::tVector3 & GetUp() const {return mUp;}
  
  const JigLib::tMatrix33 & GetOrient() const {return mOrient;}
  
  void SetPositionObject(class tRenderObject * positionObject);
  // if orientation object is set this over-rides target/up objects
  void SetOrientationObject(class tRenderObject * orientationObject);
  void SetTargetObject(class tRenderObject * targetObject);
  void SetUpObject(class tRenderObject * upObject);
  
  const class tRenderObject * GetTargetObject() const {
    return mTargetObject;}
  const class tRenderObject * GetPositionObject() const {
    return mPositionObject;}
  const class tRenderObject * GetTrackObject() const {
    return mTrackObject;}
  const class tRenderObject * GetOrientationObject() const {
    return mOrientationObject;}

  /// converts a screen position into a world position at the far plane
  JigLib::tVector3 Unproject(const JigLib::tVector2& screenPt, const class tViewport* viewport) const;
  
private:
  JigLib::tScalar mFOV, mClipNear, mClipFar;

  JigLib::tVector3 mPosition;
  JigLib::tVector3  mOffset;
  bool mOffsetAbsolute;
  JigLib::tVector3 mTarget;
  JigLib::tVector3  mUp;
  
  JigLib::tMatrix33 mOrient;
  
  class tRenderObject * mTrackObject;
  JigLib::tScalar mTrackDist, mTrackDeltaHeight, mTrackSpeed;
  JigLib::tVector3 mTrackPosition;
  /// Flag - if we should chase, or else use the orientation of the track object
  bool mChase;
  
  // if any of these objects are zero, then the property stays fixed
  class tRenderObject * mPositionObject;
  class tRenderObject * mTargetObject;
  class tRenderObject * mUpObject;
  class tRenderObject * mOrientationObject;
  
  // working stuff
  JigLib::tVector3 mVel;
};

#endif
