//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file jigtestapp.cpp 
//                     
//==============================================================

#include "jigtestapp.hpp"
#include "appconfig.hpp"
#include "debugconfig.hpp"
#include "sphereobject.hpp"
#include "capsuleobject.hpp"
#include "boxobject.hpp"
#include "boxsphereobject.hpp"
#include "planeobject.hpp"
#include "heightmapobject.hpp"
#include "meshobject.hpp"
#include "carobject.hpp"
#include "characterobject.hpp"
#include "ragdoll.hpp"
#include "newton.hpp"
#include "boxchain.hpp"
#include "spherechain.hpp"
#include "viewport.hpp"
#include "camera.hpp"
#include "contactrender.hpp"
#include "skybox.hpp"

#include <algorithm>

using namespace JigLib;
using namespace std;

//==============================================================
// DisplayLicense
//==============================================================
void DisplayLicense()
{
  TRACE("JigTest Copyright 2007 Danny Chapman: danny@rowlhouse.freeserve.co.uk\n");
  TRACE("This comes with ABSOLUTELY NO WARRANTY;\n");
  TRACE("This is free software, and you are welcome to redistribute it\n");
  TRACE("under certain conditions; see the GNU General Public License\n");
  TRACE("Version compiled " __TIME__ " " __DATE__ "\n");
}

//==============================================================
// tJigTestApp
//==============================================================
tJigTestApp::tJigTestApp(int & argc, char * argv[])
  :
  tSDLApplicationBase(argc, argv, "jigtest.cfg", ::DisplayLicense),
  mCarAI(100.0f)
{
  TRACE_METHOD_ONLY(ONCE_1);
  mCameraTargetIndex = -1;
  mCameraMoveDir = MOVE_NONE;
  mCameraMoveVel.SetTo(0.0f);
  mCameraMode = CAMERA_FIXED;
  mPhysicsDuration = 0.0f;
  mPaused = false;
  mPauseAfterNextPhysicsStep = false;
  mCollisionSystem = 0;
  mSkybox = 0;
  mPicking = false;
  mDoMovie = false;
  mQuitIterations = 0;
  mPhysicsCounter = 0;
}

//==============================================================
// ~tJigTestApp
//==============================================================
tJigTestApp::~tJigTestApp()
{
  TRACE_METHOD_ONLY(ONCE_1);

  unsigned i;
  for (i = 0 ; i < mObjects.size() ; ++i)
    delete mObjects[i];
  for (i = 0 ; i < mCameras.size() ; ++i)
    delete mCameras[i];
  for (i = 0 ; i < mViewports.size() ; ++i)
    delete mViewports[i];

  delete mCollisionSystem;
}

//==============================================================
// HandleFPSMouseMotion
//==============================================================
void tJigTestApp::HandleFPSMouseMotion(const SDL_MouseMotionEvent & event)
{
  TRACE_METHOD_ONLY(FRAME_1);
  static bool handlingWarp = false;
  if (handlingWarp)
  {
    handlingWarp = false;
    return;
  }

  if (mCharacterObjects.empty())
    return;

  tCharacterObject* character = mCharacterObjects[0];
  tBody* body = character->GetBody();
  if (!body) return;

  int x, y;
  SDL_GetMouseState(&x, &y);
  SDL_Surface* surface = SDL_GetVideoSurface();
  if (surface)
  {
    int midX = surface->w / 2;
    int midY = surface->h / 2;

    handlingWarp = true;
    SDL_WarpMouse(midX, midY);

    float bearingScale = 30.0f;
    float deltaBearing = (bearingScale * (x - midX)) / midX;

    tVector3 origLook = body->GetOrientation().GetLook();
    origLook.z = 0.0f;

    tVector3 newLook = Matrix33Gamma(-deltaBearing) * origLook;
    newLook.NormaliseSafe();

    body->SetOrientation(tMatrix33(newLook, Cross(tVector3::Up(), newLook), tVector3::Up()));
  }
}


//====================================================================
// HandleFPSKeyDown
//====================================================================
void tJigTestApp::HandleFPSKeyDown( const SDL_keysym & keysym )
{
  TRACE_METHOD_ONLY(FRAME_1);
  if (mCharacterObjects.empty())
    return;

  tCharacterObject* character = mCharacterObjects[0];
  switch( keysym.sym ) 
  {
  case SDLK_F1:
    DoScreenshot();
    break;
  case SDLK_F2:
    mDoMovie = !mDoMovie;
    break;
  case SDLK_ESCAPE:
    ExitMainLoop();
    break;
  case SDLK_y:
    RemoveAllObjects(true);
    break;
  case SDLK_t:
    RestartExperiment();
    break;
  case SDLK_w: character->ControlFwd(true); break;
  case SDLK_a: character->ControlLeft(true); break;
  case SDLK_s: character->ControlBack(true); break;
  case SDLK_d: character->ControlRight(true); break;
  case SDLK_SPACE: character->ControlJump(); break;
  default: break;
  }
}

//====================================================================
// HandleFPSKeyUp
//====================================================================
void tJigTestApp::HandleFPSKeyUp( const SDL_keysym & keysym )
{
  TRACE_METHOD_ONLY(FRAME_1);
  if (mCharacterObjects.empty())
    return;

  tCharacterObject* character = mCharacterObjects[0];

  switch( keysym.sym ) 
  {
  case SDLK_w: character->ControlFwd(false); break;
  case SDLK_a: character->ControlLeft(false); break;
  case SDLK_s: character->ControlBack(false); break;
  case SDLK_d: character->ControlRight(false); break;
  default: break;
  }
}

//==============================================================
// HandleKeyDown
//==============================================================
void tJigTestApp::HandleKeyDown( const SDL_keysym & keysym )
{
  TRACE_METHOD_ONLY(FRAME_1);

  if (!mCharacterObjects.empty())
  {
    HandleFPSKeyDown(keysym);
    return;
  }

  SDLMod keyMod = SDL_GetModState();

  switch( keysym.sym ) 
  {
  case SDLK_F1:
    DoScreenshot();
    break;
  case SDLK_F2:
    mDoMovie = !mDoMovie;
    break;
  case SDLK_w:
    mCameraMoveDir |= MOVE_FWD;
    break;
  case SDLK_s:
    mCameraMoveDir |= MOVE_BACK;
    break;
  case SDLK_a:
    mCameraMoveDir |= MOVE_LEFT;
    break;
  case SDLK_d:
    mCameraMoveDir |= MOVE_RIGHT;
    break;
  case SDLK_e:
    mCameraMoveDir |= MOVE_UP;
    break;
  case SDLK_q:
    mCameraMoveDir |= MOVE_DOWN;
    break;
  case SDLK_1:
    mCameraMode = (mCameraMode == CAMERA_FIXED ? CAMERA_FOLLOW : CAMERA_FIXED);
    break;
  case SDLK_r:
    tAppConfig::mRenderEnable = !tAppConfig::mRenderEnable;
    break;
  case SDLK_k:
    tDebugConfig::mRenderCollisionContacts = 
      (tDebugConfig::mRenderCollisionContacts + 1) %3;
    break;
  case SDLK_b:
    tDebugConfig::mRenderCollisionBoundingBoxes = 
      !tDebugConfig::mRenderCollisionBoundingBoxes;
    break;
  case SDLK_v:
    tDebugConfig::mIndicateFrozenObjects = 
      !tDebugConfig::mIndicateFrozenObjects;
    break;
  case SDLK_f:
    tAppConfig::mEnableFreezing = !tAppConfig::mEnableFreezing;
    mPhysics.EnableFreezing(tAppConfig::mEnableFreezing);
    break;
  case SDLK_h:
    tAppConfig::mDoShockStep = !tAppConfig::mDoShockStep;
    mPhysics.SetDoShockStep(tAppConfig::mDoShockStep);
    break;
  case SDLK_p:
    mPaused = !mPaused;
    break;
  case SDLK_o:
    mPaused = false;
    mPauseAfterNextPhysicsStep = true;
    break;
  case SDLK_EQUALS:
    mCameraTargetIndex += 1;
    break;
  case SDLK_MINUS:
    mCameraTargetIndex -= 1;
    break;
  case SDLK_SPACE:
    FireObject();
    break;
  case SDLK_x:
    CreateBox();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_SLASH:
    CreateCompound();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_z:
    CreateSphere();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_n:
    CreateCapsule();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_c:
    CreateCar();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_BACKSLASH:
  case SDLK_LESS:
    CreateRagdoll();
    if (keyMod & KMOD_CTRL)
      FireLastCreatedObject();
    break;
  case SDLK_m:
    CreateCharacter();
    break;
  case SDLK_y:
    RemoveAllObjects(true);
    break;
  case SDLK_t:
    RestartExperiment();
    break;
  case SDLK_LEFT:
    ControlMainObject(CONTROL_LEFT, true);
    break;
  case SDLK_RIGHT:
    ControlMainObject(CONTROL_RIGHT, true);
    break;
  case SDLK_UP:
    ControlMainObject(CONTROL_UP, true);
    break;
  case SDLK_DOWN:
    ControlMainObject(CONTROL_DOWN, true);
    break;
  case SDLK_ESCAPE:
    ExitMainLoop();
    break;
  case SDLK_PAGEDOWN:
    tAppConfig::mTextureLevel -= 1;
    Limit(tAppConfig::mTextureLevel, -1, 1);
    mRenderManager.SetWireFrameMode(tAppConfig::mTextureLevel == -1);
    break;
  case SDLK_PAGEUP:
    tAppConfig::mTextureLevel += 1;
    Limit(tAppConfig::mTextureLevel, -1, 1);
    mRenderManager.SetWireFrameMode(tAppConfig::mTextureLevel == -1);
    break;
  default:
    break;
  }
  SetSleepingParams();
}

//==============================================================
// HandleKeyUp
//==============================================================
void tJigTestApp::HandleKeyUp( const SDL_keysym & keysym )
{
  TRACE_METHOD_ONLY(FRAME_1);

  if (!mCharacterObjects.empty())
  {
    HandleFPSKeyUp(keysym);
    return;
  }

  switch( keysym.sym ) 
  {
  case SDLK_w:
    mCameraMoveDir &= ~MOVE_FWD;
    break;
  case SDLK_s:
    mCameraMoveDir &= ~MOVE_BACK;
    break;
  case SDLK_a:
    mCameraMoveDir &= ~MOVE_LEFT;
    break;
  case SDLK_d:
    mCameraMoveDir &= ~MOVE_RIGHT;
    break;
  case SDLK_e:
    mCameraMoveDir &= ~MOVE_UP;
    break;
  case SDLK_q:
    mCameraMoveDir &= ~MOVE_DOWN;
    break;
  case SDLK_LEFT:
    ControlMainObject(CONTROL_LEFT, false);
    break;
  case SDLK_RIGHT:
    ControlMainObject(CONTROL_RIGHT, false);
    break;
  case SDLK_UP:
    ControlMainObject(CONTROL_UP, false);
    break;
  case SDLK_DOWN:
    ControlMainObject(CONTROL_DOWN, false);
    break;
  default:
    break;
  }
}


//==============================================================
// HandleMouseButtonDown
//==============================================================
void tJigTestApp::HandleMouseButtonDown( const SDL_MouseButtonEvent & event )
{
  if (!GetPhysicsSystem())
    return;

  if (mObjectPositionController.GetConstraintEnabled() && mObjectPositionController.GetBody())
    mObjectPositionController.GetBody()->SetActive();

  if ( mPicking && (event.button == 4 ||  event.button == 5) )
  {
    if (event.button == 4)
      mPickDist *= 1.1f;
    else if (event.button == 5)
      mPickDist *= 0.9f;
    SDL_MouseMotionEvent motionEvent;
    motionEvent.x = event.x;
    motionEvent.y = event.y;
    HandleMouseMotion( motionEvent );
    return;
  }
  else if (event.button == 2 && mPicking)
  {
    if (tBody* body = mObjectPositionController.GetBody())
    {
      mObjectPositionController.DisableConstraint();
      mObjectPositionDamperController.DisableConstraint();
      const tVector3& camPos = mCameras[0]->GetPosition();
      tVector3 dir = (body->GetPosition() - camPos).GetNormalisedSafe();
      body->SetVelocity(tAppConfig::mProjectileSpeed * dir);

      mPicking = false;
      return;
    }
  }

  tVector2 mousePt(event.x, event.y);
  // get the object from physics
  tViewport* viewport = 0;
  unsigned nViewports = mViewports.size();
  unsigned i;
  for (i = 0 ; i < nViewports ; ++i)
  {
    viewport = mViewports[i];
    tVector2 pos, size;
    viewport->GetPositionAndSize(pos, size);
    if (mousePt.x > pos.x && 
        mousePt.x < pos.x + size.x &&
        mousePt.y > pos.y && 
        mousePt.y < pos.y + size.y)
    {
      break;
    }
  }
  if (!viewport)
    return;

  tVector3 worldPos = viewport->GetCamera()->Unproject(mousePt, viewport);
  const tVector3& camPos = viewport->GetCamera()->GetPosition();
  tVector3 dir = (worldPos - camPos).NormaliseSafe();
  worldPos = camPos + 100.0f * dir;

  tCollisionSystem* coll = GetPhysicsSystem()->GetCollisionSystem();
  if (!coll)
    return;

  tScalar frac = 0;
  tCollisionSkin * skin = 0;
  tVector3 pos;
  tVector3 normal;
  tSegment seg(camPos, worldPos - camPos); 

  bool result = coll->SegmentIntersect(frac, skin, pos, normal, seg, 0);

  if (!result || !skin)
  {
    TRACE("No collision skin to pick\n");
    return;
  }

  tBody* body = skin->GetOwner();
  if (!body)
  {
    TRACE("got collision skin but no owner\n");
    return;
  }

  if (event.button == 1)
  {
    for (i = 0 ; i < mObjects.size() ; ++i)
    {
      if (mObjects[i]->GetBody() == body)
      {
        mCameraTargetIndex = (int) i;
        break;
      }
    }
  }
  else if (event.button == 3)
  {
    mPickDist = (pos - camPos).GetLength();

    // Need to get the position in body coords
    tVector3 delta = pos - body->GetPosition();
    delta = body->GetOrientation().GetTranspose() * delta;
    mObjectPositionController.Initialise(body, delta, pos);
    mObjectPositionDamperController.Initialise(body, tConstraintVelocity::BODY, 0, &tVector3::Zero());
    mObjectPositionController.EnableConstraint();
    mObjectPositionDamperController.EnableConstraint();
    viewport->GetCamera()->Update(1);
    mPicking = true;
  }
}

//==============================================================
// HandleMouseButtonUp
//==============================================================
void tJigTestApp::HandleMouseButtonUp( const SDL_MouseButtonEvent & event )
{
  if (event.button == 2 || event.button == 3)
  {
    if (mObjectPositionController.GetConstraintEnabled() && mObjectPositionController.GetBody())
      mObjectPositionController.GetBody()->SetActive();
    mObjectPositionController.DisableConstraint();
    mObjectPositionDamperController.DisableConstraint();
    mPicking = false;
  }
}

//==============================================================
// HandleMouseMotion
//==============================================================
void tJigTestApp::HandleMouseMotion( const SDL_MouseMotionEvent & event )
{
  TRACE_METHOD_ONLY(FRAME_1);

  if (!mCharacterObjects.empty())
  {
    HandleFPSMouseMotion(event);
    return;
  }

  tVector2 mousePt(event.x, event.y);
  if (mPicking)
  {
    if (mObjectPositionController.GetConstraintEnabled() && mObjectPositionController.GetBody())
      mObjectPositionController.GetBody()->SetActive();
    const tViewport* viewport = 0;
    unsigned nViewports = mViewports.size();
    unsigned i;
    for (i = 0 ; i < nViewports ; ++i)
    {
      viewport = mViewports[i];
      tVector2 pos, size;
      viewport->GetPositionAndSize(pos, size);
      if (mousePt.x > pos.x && 
          mousePt.x < pos.x + size.x &&
          mousePt.y > pos.y && 
          mousePt.y < pos.y + size.y)
      {
        break;
      }
    }
    if (!viewport)
      return;
    tVector3 worldPos = viewport->GetCamera()->Unproject(mousePt, viewport);
    const tVector3& camPos = viewport->GetCamera()->GetPosition();

    tVector3 dir = (worldPos - camPos).NormaliseSafe();
    tVector3 constraintPos = camPos + mPickDist * dir;

    mObjectPositionController.SetWorldPosition(constraintPos);
  }
}

//==============================================================
// ControlMainObject
//==============================================================
void tJigTestApp::ControlMainObject(tObjectControl control, bool down)
{
  for (int i = Max(mCameraTargetIndex, 0) ; i < (int) mObjects.size() ; ++i)
  {
    tObject * obj = mObjects[i];
    if (obj->GetBody())
    {
      mCameraTargetIndex = i;
      break;
    }
  }
  if (mCameraTargetIndex < 0)
    return;

  if (!mCarObjects.empty())
  {
    int ind = mCameraTargetIndex;
    if (mObjects[ind] == mCarObjects[0])
    {
      ControlMainCar(control, down);
      return;
    }
  }

  if (!down)
  {
    mObjectController.DisableConstraint();
    return;
  }

  if (mObjects.empty())
    return;

  tScalar speed = tAppConfig::mObjectControllerSpeed;
  tVector3 vel;
  const tMatrix33 & orient = mCameras[0]->GetOrient();
  switch (control)
  {
  case CONTROL_UP: vel = speed * orient.GetUp(); break;
  case CONTROL_DOWN: vel = -speed * orient.GetLook(); break;
  case CONTROL_LEFT: vel = speed * orient.GetLeft(); break;
  case CONTROL_RIGHT: vel = -speed * orient.GetLeft(); break;
  }
  int ind = mCameraTargetIndex;
  if (ind < (int) mObjects.size())
  {
    for (int i = Max(ind, 0) ; i < (int) mObjects.size() ; ++i)
    {
      tObject * obj = mObjects[i];
      if (obj->GetBody())
      {
        tBody * body = obj->GetBody();
        mCameraTargetIndex = i;

        mObjectController.Initialise(body, tConstraintVelocity::WORLD, &vel, 0);
        mObjectController.EnableConstraint();
        return;
      }
    }
  }
}

//==============================================================
// ControlMainCar
//==============================================================
void tJigTestApp::ControlMainCar(tObjectControl control, bool down)
{
  if (mCarObjects.empty())
    return;
  tCarObject * mainCar = mCarObjects[0];

  mainCar->GetBody()->SetActive();

  switch(control)
  {
  case CONTROL_UP:
    mainCar->GetCar().SetAccelerate(down ? 1.0f : 0.0f);
    break;
  case CONTROL_DOWN:
    mainCar->GetCar().SetAccelerate(down ? -1.0f : 0.0f);
    break;
  case CONTROL_LEFT:
    mainCar->GetCar().SetSteer(down ? 1.0f : 0.0f);
    break;
  case CONTROL_RIGHT:
    mainCar->GetCar().SetSteer(down ? -1.0f : 0.0f);
    break;
  default:
    break;
  }
}

//==============================================================
// FireLastCreatedObject
//==============================================================
void tJigTestApp::FireLastCreatedObject()
{
  int origIndex = mCameraTargetIndex;
  mCameraTargetIndex = mObjects.size() - 1;
  FireObject();
  mCameraTargetIndex = origIndex;
}


//==============================================================
// FireObject
//==============================================================
void tJigTestApp::FireObject()
{
  if (mObjects.empty())
    return;
  int ind = mCameraTargetIndex;
  if (ind < (int) mObjects.size())
  {
    for (int i = Max(ind, 0) ; i < (int) mObjects.size() ; ++i)
    {
      tObject * obj = mObjects[i];
      if (obj->GetBody())
      {
        mCameraTargetIndex = i;
        tVector3 camPos = mCameras[0]->GetPosition();
        tVector3 dir = (mCameras[0]->GetTarget() - camPos).GetNormalised();
        obj->SetPhysicsPosition(camPos + 0.001f * dir, obj->GetBody()->GetOrientation());
        obj->SetPhysicsVelocity(tAppConfig::mProjectileSpeed * dir);
        return;
      }
    }
  }
}

//==============================================================
// ProcessMainEvent
//==============================================================
void tJigTestApp::ProcessMainEvent()
{
  TRACE_METHOD_ONLY(FRAME_1);
  if (!mPaused && GetJustCalculatedNewFPS())
  {
//    TRACE_FILE_IF(ONCE_1)
    {
      TRACE("FPS = %5.2f, ", GetFPS());
      TRACE("Physics duration = %5.2fms which is %5.2f%% of the timestep\n",
            1000.0f * mPhysicsDuration, 
            100.0f * mPhysicsDuration * tAppConfig::mPhysicsFrequency);
    }
  }

  TRACE_FILE_IF(ONCE_1)
    {
      static unsigned oldNumObjects = 0;
      if (mObjects.size() != oldNumObjects)
        TRACE("New number of objects = %d\n", mObjects.size());
      oldNumObjects = mObjects.size();
    }


  CheckGLErrors("Start of ProcessMainEvent");

  int iLoop = 0;

  if (mDoMovie)
  {
    iLoop = 1;
    if (!mPaused)
    {
      for (iLoop = 0 ; iLoop < tAppConfig::mMovieInterval ; ++iLoop)
      {
        mPhysics.Integrate(1.0f / tAppConfig::mPhysicsFrequency);
      }
      mPhysics.ResetTime(GetStartOfFrameTime());
      DoScreenshot();
    }
  }
  else
  {
    if (!mPaused)
    {
    	mPhysics.SetNullUpdate(tAppConfig::mNullPhysicsUpdate);
      // the idea is that we run physics until it progresses just beyond
      // where we want to be.
      int maxLoops = tAppConfig::mMaxPhysicsStepsPerFrame;
      while ( (iLoop < maxLoops) &&
              (GetStartOfFrameTime() > mPhysics.GetTargetTime()) )
      {
        ++iLoop;
        bool timeAvailable = false;
        tHighResTimeVal before = GetHighResTime(timeAvailable);

        mPhysics.Integrate(1.0f / tAppConfig::mPhysicsFrequency);
        ++mPhysicsCounter;

        tHighResTimeVal after = GetHighResTime(timeAvailable);
        if (timeAvailable)
        {
          static const tScalar frac = 0.05f;
          mPhysicsDuration = 
            (1.0f - frac) * mPhysicsDuration + 
            frac * GetTimeDelta(before, after);
        }

        if (mQuitIterations != 0 && mPhysicsCounter >= mQuitIterations)
        {
          TRACE("Reached %d physics updates - quitting\n", mPhysicsCounter);
          exit(0);
        }

        // single stepping?
        if (mPauseAfterNextPhysicsStep)
        {
          mPauseAfterNextPhysicsStep = false;
          mPaused = true;
          break;
        }
      }
      if (GetStartOfFrameTime() > mPhysics.GetTargetTime())
      {
        mPhysics.ResetTime(GetStartOfFrameTime());
        static tTime lastTime = tTime(0.0f);
        if (GetStartOfFrameTime() > lastTime + 1.0f)
        {
          TRACE("Warning - physics can't keep up\n");
          lastTime = GetStartOfFrameTime();
        }
      }
    }
    else
    {
    	mPhysics.SetNullUpdate(true);
      mPhysics.Integrate(1.0f / tAppConfig::mPhysicsFrequency);
      mPhysics.ResetTime(GetStartOfFrameTime());
      iLoop = 1;
    }
  }

  // update car ai
  mCarAI.Update(iLoop / tAppConfig::mPhysicsFrequency);

  // Update camera[0] speed
  if (0 == mCameras[0]->GetPositionObject())
  {
    tVector3 desiredCamVel = 
      !!(mCameraMoveDir & MOVE_FWD) * tVector3::Look() + 
      !!(mCameraMoveDir & MOVE_BACK) * -tVector3::Look() + 
      !!(mCameraMoveDir & MOVE_LEFT) * tVector3::Left() + 
      !!(mCameraMoveDir & MOVE_RIGHT) * -tVector3::Left() + 
      !!(mCameraMoveDir & MOVE_UP) * tVector3::Up() + 
      !!(mCameraMoveDir & MOVE_DOWN) * -tVector3::Up();
    desiredCamVel = tAppConfig::mCamSpeed * 
      mCameras[0]->GetOrient() * desiredCamVel;
    // cam vel is relative to a reference distance
    desiredCamVel *= (mCameras[0]->GetTarget() - mCameras[0]->GetPosition()).GetLength() / 10.0f;
    tVector3 camPos = mCameras[0]->GetPosition();
    tScalar dt = GetStartOfFrameTime() - GetOldStartOfFrameTime();
    if (mDoMovie)
      dt = 1.0f / tAppConfig::mPhysicsFrequency;
    camPos += desiredCamVel * dt;
    mCameras[0]->SetPosition(camPos);
  }

  // do rendering
  if (tAppConfig::mRenderEnable)
  {
    /// The fraction used to interpolate between old and current
    /// positions in the physics when rendering.
    tScalar renderFraction = 1.0f;

    if (tAppConfig::mRenderInterpolate)
    {
      tScalar physicsDt = mPhysics.GetTargetTime() - mPhysics.GetOldTime();
      if (physicsDt > SCALAR_TINY)
      {
        renderFraction = 
          (GetStartOfFrameTime() - mPhysics.GetOldTime()) / physicsDt;
        Limit(renderFraction, SCALAR(0.0f), SCALAR(1.0f));
        if (mPaused)
          renderFraction = 1.0f;
      }
    }

    int nObjects = mObjects.size();
    int i;
    for (i = 0 ; i < nObjects ; ++i)
      mObjects[i]->SetRenderPosition(renderFraction);

    CheckGLErrors("After setting render position");

    // Now update all our cameras and render stuff via the viewports
    if (mCameraTargetIndex >= (int) mObjects.size())
      mCameraTargetIndex = -1;
    else if (mCameraTargetIndex < -1)
      mCameraTargetIndex = mObjects.size() - 1;
    mCameras[0]->SetChase(true);
    if (mCameraTargetIndex >= 0)
    {
      if (mCameras[0]->GetTargetObject() != mObjects[mCameraTargetIndex])
        mCameras[0]->SetTargetObject(mObjects[mCameraTargetIndex]);
      if (mCameraMode == CAMERA_FIXED)
      {
        mCameras[0]->SetTrackTarget(
          0, 8.0f, 3.0f, 50.0f);
      }
      else if (mCameras[0]->GetTrackObject() != mObjects[mCameraTargetIndex])
      {
        mCameras[0]->SetTrackTarget(
          mObjects[mCameraTargetIndex], 8.0f, 3.0f, 50.0f);
        mCameras[0]->SetPositionOffset(tVector3(-7.0f, 0.0f, 0.0f));
      }
    }
    else
    {
      if (mCameras[0]->GetTargetObject() != 0)
      {
        mCameras[0]->SetTargetObject(0);
        mCameras[0]->SetPositionOffset(tVector3::Zero());
      }
    }

    // if we have a character ignore everything else
    if (!mCharacterObjects.empty())
    {
      tObject* charObject = mCharacterObjects[0];
      mCameras[0]->SetPositionObject(charObject);
      if (mCameras[0]->GetOrientationObject() != charObject)
        mCameras[0]->SetOrientationObject(charObject);
      mCameras[0]->SetPositionOffset(tVector3(-7.0f, 0.0f, 4.0f));
    }

    if (!mPicking)
    {
      int nCameras = mCameras.size();
      for (i = 0 ; i < nCameras ; ++i)
        mCameras[i]->Update(GetStartOfFrameTime() - GetOldStartOfFrameTime());
      CheckGLErrors("after camera updates");
    }

    // Clear
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int nViewports = mViewports.size();
    for (i = 0 ; i < nViewports ; ++i)
    {
      mViewports[i]->Render();
      if (tDebugConfig::mRenderCollisionContacts)
        RenderAllContacts(tDebugConfig::mRenderCollisionContacts);
      if ( (mCameraTargetIndex >= 0) && 
           (mCameraTargetIndex < (int) mObjects.size()) )
      {
        tObject * obj = mObjects[mCameraTargetIndex];
        if (mCarObjects.empty() || (mCarObjects[0] != mObjects[mCameraTargetIndex]) )
        {
          if (obj->GetBody())
            obj->RenderWorldBoundingBox(&tVector3::Look()); // look = red
        }
      }
    }
    CheckGLErrors("After vewport renders");
  }  

  SDL_GL_SwapBuffers( );

  CheckGLErrors("After buffer swap");
}

//==============================================================
// StartMainLoop
//==============================================================
void tJigTestApp::StartMainLoop()
{
  TRACE_METHOD_ONLY(FRAME_1);

  // synchronise the physics time
  mPhysics.ResetTime(GetStartOfFrameTime());

  tSDLApplicationBase::StartMainLoop();
}


//==============================================================
// SetupExperiment
//==============================================================
void tJigTestApp::SetupExperiment()
{
  TRACE_METHOD_ONLY(ONCE_1);
  unsigned i;

  // clear up
  {
    RemoveAllObjects(false);
    for (i = 0 ; i < mCameras.size(); ++i)
      delete mCameras[i];
    mCameras.resize(0);
    for (i = 0 ; i < mViewports.size(); ++i)
      delete mViewports[i];
    mViewports.resize(0);

    delete mCollisionSystem;
    mCollisionSystem = 0;
  }

  switch(tAppConfig::mCollisionSystemType)
  {
  case 0:
    TRACE_FILE_IF(ONCE_1)
      TRACE("Creating tCollisionSystemBrute\n");
    mCollisionSystem = new tCollisionSystemBrute();
    break;
  case 1:
    TRACE_FILE_IF(ONCE_1)
      TRACE("Creating tCollisionSystemGrid\n");
    mCollisionSystem = new tCollisionSystemGrid(32, 32, 4, tAppConfig::mCollisionGridSize, tAppConfig::mCollisionGridSize, tAppConfig::mCollisionGridSize);
    break;
  default:
    TRACE("Collision system type %d not handled\n", tAppConfig::mCollisionSystemType);
    mCollisionSystem = new tCollisionSystemBrute();
    break;
  }
  mPhysics.SetCollisionSystem(mCollisionSystem);

  bool collisionUseSweptTests = false;
  GetConfigFile().GetValue("collision_use_swept_tests", collisionUseSweptTests);
  mCollisionSystem->SetUseSweepTests(collisionUseSweptTests);

  // create a heightmap or planes for objects to sit on
  string heightmapFilename("none");
  GetConfigFile().GetValue("heightmap_filename", heightmapFilename);
  if (heightmapFilename != string("none"))
  {
    CreateFileHeightmap(heightmapFilename);
  }
  else
  {
    bool useBumpyHeightmap = false;
    GetConfigFile().GetValue("use_bumpy_heightmap", useBumpyHeightmap);
    if (useBumpyHeightmap)
    {
      CreateBumpyHeightmap();
    }
    else
    {
      tScalar planeOffset = -2.0f;
      tScalar planeAngle = 30.0f;
      GetConfigFile().GetValue("plane_offset", planeOffset);
      GetConfigFile().GetValue("plane_angle", planeAngle);
      tScalar planeElasticity = 0.0f;
      tScalar planeStaticFriction = 0.5f;
      tScalar planeDynamicFriction = 0.3f;
      GetConfigFile().GetValue("plane_elasticity", planeElasticity);
      GetConfigFile().GetValue("plane_static_friction", planeStaticFriction);
      GetConfigFile().GetValue("plane_dynamic_friction", planeDynamicFriction);

      tPlaneObject * plane0 = new tPlaneObject(tPlane(tVector3::Up(),
                                                      tVector3(0.0f)));

      tVector3 planeUp = Matrix33Alpha(planeAngle) * tVector3::Up();
      tVector3 offset = planeOffset * tVector3::Up();
      planeUp = Matrix33Gamma(-90.0f) * planeUp;
      tPlaneObject * plane1 = new tPlaneObject(tPlane(planeUp, offset));
      planeUp = Matrix33Gamma(120.0f) * planeUp;
      tPlaneObject * plane2 = new tPlaneObject(tPlane(planeUp, offset));
      planeUp = Matrix33Gamma(120.0f) * planeUp;
      tPlaneObject * plane3 = new tPlaneObject(tPlane(planeUp, offset));

      plane0->SetProperties(planeElasticity, 
                            planeStaticFriction, 
                            planeDynamicFriction);
      plane1->SetProperties(planeElasticity, 
                            planeStaticFriction, 
                            planeDynamicFriction);
      plane2->SetProperties(planeElasticity, 
                            planeStaticFriction, 
                            planeDynamicFriction);
      plane3->SetProperties(planeElasticity, 
                            planeStaticFriction, 
                            planeDynamicFriction);

      mObjects.push_back(plane0);
      mObjects.push_back(plane1);
      mObjects.push_back(plane2);
      mObjects.push_back(plane3);
      mRenderManager.AddObject(plane0);
      mRenderManager.AddObject(plane1);
      mRenderManager.AddObject(plane2);
      mRenderManager.AddObject(plane3);
      /// need to add non-physical objects to collision
      mCollisionSystem->AddCollisionSkin(plane0->GetCollisionSkin());
      mCollisionSystem->AddCollisionSkin(plane1->GetCollisionSkin());
      mCollisionSystem->AddCollisionSkin(plane2->GetCollisionSkin());
      mCollisionSystem->AddCollisionSkin(plane3->GetCollisionSkin());
    }
  }

  bool doUnequalExp = false;
  GetConfigFile().GetValue("do_unequal_exp", doUnequalExp);
  if (doUnequalExp)
    SetupUnequalExperiment();

  // create some spheres
  int nSpheres = 1;
  tScalar sphereMinRadius = 1.0f;
  tScalar sphereMaxRadius = 1.0f;
  tScalar sphereElasticity = 0.0f;
  tScalar sphereStaticFriction = 0.5f;
  tScalar sphereDynamicFriction = 0.3f;
  tScalar sphereDensity = 500.0f;
  tScalar sphereX = 0.0f;
  tScalar sphereY = 0.0f;
  tScalar sphereZ = 0.0f;
  GetConfigFile().GetValue("sphere_x", sphereX);
  GetConfigFile().GetValue("sphere_y", sphereY);
  GetConfigFile().GetValue("sphere_z", sphereZ);
  GetConfigFile().GetValue("num_spheres", nSpheres);
  GetConfigFile().GetValue("sphere_min_radius", sphereMinRadius);
  GetConfigFile().GetValue("sphere_max_radius", sphereMaxRadius);
  GetConfigFile().GetValue("sphere_elasticity", sphereElasticity);
  GetConfigFile().GetValue("sphere_static_friction", sphereStaticFriction);
  GetConfigFile().GetValue("sphere_dynamic_friction", sphereDynamicFriction);
  GetConfigFile().GetValue("sphere_density", sphereDensity);

  for (int iSphere = 0 ; iSphere < nSpheres ; ++iSphere)
  {
    tSphereObject * sphereObject = 
      new tSphereObject(RangedRandom(sphereMinRadius, sphereMaxRadius));
    sphereObject->SetProperties(sphereElasticity, 
                                sphereStaticFriction, 
                                sphereDynamicFriction);
    sphereObject->SetDensity(sphereDensity);
    mObjects.push_back(sphereObject);
    sphereObject->GetBody()->MoveTo(
      tVector3(sphereX, sphereY, 
               sphereZ + (0.5f + iSphere) * (2.0f * sphereMaxRadius)) + 
      0.05f * sphereMaxRadius * 
      tVector3(RangedRandom(-1, 1), RangedRandom(-1, 1), 0.0f),
      tMatrix33::Identity());
    mRenderManager.AddObject(sphereObject);
    /// physics will register collision for objects passed to it.
    sphereObject->GetBody()->EnableBody();
  }

  // create a jenga stack
  int jengaStackHeight = 0;
  tScalar jengaBoxWidth = 1.0f;
  tScalar jengaBoxHeight = 0.5f;
  tScalar jengaStackX = 0.0f;
  tScalar jengaStackY = 0.0f;
  tScalar jengaStackZ = 0.0f;
  GetConfigFile().GetValue("jenga_stack_height", jengaStackHeight);
  GetConfigFile().GetValue("jenga_box_width", jengaBoxWidth);
  GetConfigFile().GetValue("jenga_box_height", jengaBoxHeight);
  GetConfigFile().GetValue("jenga_stack_x", jengaStackX);
  GetConfigFile().GetValue("jenga_stack_y", jengaStackY);
  GetConfigFile().GetValue("jenga_stack_z", jengaStackZ);

  BuildJengaStack(jengaStackHeight, 
                  tVector3(jengaBoxWidth * 3.0f,
                           jengaBoxWidth, 
                           jengaBoxHeight), 
                  tVector3(jengaStackX, jengaStackY, jengaStackZ));

  // create some boxes
  int nBoxes = 1;
  tScalar boxMinSide = 1.0f;
  tScalar boxMaxSide = 1.0f;
  tScalar boxElasticity = 0.0f;
  tScalar boxStaticFriction = 0.5f;
  tScalar boxDynamicFriction = 0.3f;
  tScalar boxDensity = 500.0f;
  tScalar boxX = 0.0f;
  tScalar boxY = 0.0f;
  tScalar boxZ = 0.0f;
  tScalar boxZSpacing = 0.0f;
  tVector3 boxScale(1, 1, 1);
  GetConfigFile().GetValue("box_x", boxX);
  GetConfigFile().GetValue("box_y", boxY);
  GetConfigFile().GetValue("box_z", boxZ);
  GetConfigFile().GetValue("box_scale_x", boxScale.x);
  GetConfigFile().GetValue("box_scale_y", boxScale.y);
  GetConfigFile().GetValue("box_scale_z", boxScale.z);
  GetConfigFile().GetValue("num_boxes", nBoxes);
  GetConfigFile().GetValue("box_min_side", boxMinSide);
  GetConfigFile().GetValue("box_max_side", boxMaxSide);
  GetConfigFile().GetValue("box_elasticity", boxElasticity);
  GetConfigFile().GetValue("box_static_friction", boxStaticFriction);
  GetConfigFile().GetValue("box_dynamic_friction", boxDynamicFriction);
  GetConfigFile().GetValue("box_density", boxDensity);
  GetConfigFile().GetValue("box_z_spacing", boxZSpacing);

  tScalar z = boxZ - tAppConfig::mAllowedPenetration;
  for (int iBox = 0 ; iBox < nBoxes ; ++iBox)
  {
    tVector3 sides = ElementMult(boxScale, tVector3(RangedRandom(boxMinSide, boxMaxSide),
                                                    RangedRandom(boxMinSide, boxMaxSide),
                                                    RangedRandom(boxMinSide, boxMaxSide)));
    tBoxObject * boxObject = new tBoxObject(sides, false);
    boxObject->SetProperties(boxElasticity, 
                             boxStaticFriction,
                             boxDynamicFriction);
    boxObject->SetDensity(boxDensity);
    mObjects.push_back(boxObject);
    boxObject->GetBody()->MoveTo(
      tVector3(boxX, boxY, z + boxZ + 0.5f * sides.z), 
      Matrix33Gamma(RangedRandom(0.0f, 0.0f)));
    z += sides.z + boxZSpacing - tAppConfig::mAllowedPenetration;
    mRenderManager.AddObject(boxObject);
    /// physics will register collision for objects passed to it.
    boxObject->GetBody()->EnableBody();
  }

  // create some capsules
  int numCapsules = 0;
  GetConfigFile().GetValue("num_capsules", numCapsules);
  for (int iCapsule = 0 ; iCapsule < numCapsules ; ++iCapsule)
  {
    tCapsuleObject * capsule = CreateCapsule();
    capsule->GetBody()->MoveTo(capsule->GetBody()->GetPosition() + 
                               iCapsule * 1.0f * tVector3::Up(), 
                               capsule->GetBody()->GetOrientation());
  }

  // create some capsules
  int numCompounds = 0;
  GetConfigFile().GetValue("num_compounds", numCompounds);
  for (int iCompound = 0 ; iCompound < numCompounds ; ++iCompound)
  {
    tBoxSphereObject * compound = CreateCompound();
    compound->GetBody()->MoveTo(compound->GetBody()->GetPosition() + 
                               iCompound * 1.0f * tVector3::Up(), 
                               compound->GetBody()->GetOrientation());
  }


  // create a wall
  int numWalls = 1;
  tScalar wallSpacing = 10.0f;
  int wallHeight = 0;
  int wallWidth = 0;
  tScalar wallBoxX = 0.5f;
  tScalar wallBoxY = 1.0f;
  tScalar wallBoxZ = 0.5f;
  tScalar wallX = 0.0f;
  tScalar wallY = 0.0f;
  tScalar wallZ = 0.0f;
  bool wallPyramid = false;
  GetConfigFile().GetValue("num_walls", numWalls);
  GetConfigFile().GetValue("wall_spacing", wallSpacing);
  GetConfigFile().GetValue("wall_stack_height", wallHeight);
  GetConfigFile().GetValue("wall_stack_width", wallWidth);
  GetConfigFile().GetValue("wall_box_x", wallBoxX);
  GetConfigFile().GetValue("wall_box_y", wallBoxY);
  GetConfigFile().GetValue("wall_box_z", wallBoxZ);
  GetConfigFile().GetValue("wall_stack_x", wallX);
  GetConfigFile().GetValue("wall_stack_y", wallY);
  GetConfigFile().GetValue("wall_stack_z", wallZ);
  GetConfigFile().GetValue("wall_pyramid", wallPyramid);

  for (int iWall = 0 ; iWall < numWalls ; ++iWall)
  {
    tVector3 offset = -iWall * wallSpacing * tVector3::Look();
    BuildWall(wallHeight, wallWidth,
              tVector3(wallBoxX, wallBoxY, wallBoxZ), 
              offset + tVector3(wallX, wallY, wallZ),
              wallPyramid);
  }
  // create some cars
  int numCars = 0;
  GetConfigFile().GetValue("num_cars", numCars);
  for (int iCar = 0 ; iCar < numCars ; ++iCar)
  {
    tCarObject * car = CreateCar();
    car->GetBody()->MoveTo(car->GetBody()->GetPosition() + 
                           iCar * 3.0f * tVector3::Up(), 
                           car->GetBody()->GetOrientation());
  }

  // create a box chain
  int boxChainLen = 0;
  GetConfigFile().GetValue("box_chain_len", boxChainLen);
  if (boxChainLen > 0)
  {
    tScalar boxChainX = 0.0f;
    tScalar boxChainY = 0.0f;
    tScalar boxChainZ = 0.0f;
    tScalar boxChainBoxX = 1.0f;
    tScalar boxChainBoxY = 2.0f;
    tScalar boxChainBoxZ = 1.0f;
    tScalar boxChainDensity = 100.0f;
    GetConfigFile().GetValue("box_chain_x", boxChainX);
    GetConfigFile().GetValue("box_chain_y", boxChainY);
    GetConfigFile().GetValue("box_chain_z", boxChainZ);
    GetConfigFile().GetValue("box_chain_box_x", boxChainBoxX);
    GetConfigFile().GetValue("box_chain_box_y", boxChainBoxY);
    GetConfigFile().GetValue("box_chain_box_z", boxChainBoxZ);
    GetConfigFile().GetValue("box_chain_density", boxChainDensity);
    tBoxChain * chain = new tBoxChain(
      &mRenderManager,
      boxChainLen,
      tVector3(boxChainX, boxChainY, boxChainZ),
      tVector3(boxChainBoxX, boxChainBoxY, boxChainBoxZ),
      boxChainDensity,
      tBoxChain::CHAIN_HINGE);
    mObjects.push_back(chain);
  }

  // create a sphere chain
  int sphereChainLen = 0;
  GetConfigFile().GetValue("sphere_chain_len", sphereChainLen);
  if (sphereChainLen > 0)
  {
    tScalar sphereChainX = 0.0f;
    tScalar sphereChainY = 0.0f;
    tScalar sphereChainZ = 0.0f;
    tScalar sphereChainRadius = 1.0f;
    tScalar sphereChainDensity = 100.0f;
    GetConfigFile().GetValue("sphere_chain_x", sphereChainX);
    GetConfigFile().GetValue("sphere_chain_y", sphereChainY);
    GetConfigFile().GetValue("sphere_chain_z", sphereChainZ);
    GetConfigFile().GetValue("sphere_chain_radius", sphereChainRadius);
    GetConfigFile().GetValue("sphere_chain_density", sphereChainDensity);
    tSphereChain * chain = new tSphereChain(
      &mRenderManager,
      sphereChainLen,
      tVector3(sphereChainX, sphereChainY, sphereChainZ),
      sphereChainRadius,
      sphereChainDensity,
      tSphereChain::CHAIN_BALLNSOCKET);
    mObjects.push_back(chain);
  }

  // create some ragdolls
  int numRagdolls = 0;
  GetConfigFile().GetValue("num_ragdolls", numRagdolls);
  bool ragdollStartAsleep = false;
  GetConfigFile().GetValue("ragdoll_start_asleep", ragdollStartAsleep);
  for (int iRagdoll = 0 ; iRagdoll < numRagdolls ; ++iRagdoll)
  {
    CreateRagdoll();
  }

  // create some newton cradles
  int numNewtons = 0;
  GetConfigFile().GetValue("num_newtons", numNewtons);
  for (int iNewton = 0 ; iNewton < numNewtons ; ++iNewton)
  {
    CreateNewton();
  }

  if (false) // test for PAL problems
  {
    std::vector<tVector3> verts(5);
    verts[0].Set(1, 0, 2);
    verts[1].Set(0, -1, 2);
    verts[2].Set(0, 0, 1);
    verts[3].Set(0, 1, 2);
    verts[4].Set(-1, 0, 2);

    std::vector<tTriangleVertexIndices> inds(4);
    inds[0].Set(2, 1, 0);
    inds[1].Set(2, 0, 3);
    inds[2].Set(1, 2, 4);
    inds[3].Set(2, 3, 4);

    tScalar minCellSize = 1.0f;
    tScalar meshElasticity = 0.0f;
    tScalar meshStaticFriction = 1.0f;
    tScalar meshDynamicFriction = 1.0f;
    int maxTrianglesPerCell = 4;

    tMeshObject* meshObject = new tMeshObject(verts, inds, maxTrianglesPerCell, minCellSize);
    meshObject->SetProperties(meshElasticity, 
      meshStaticFriction,
      meshDynamicFriction);
    mObjects.push_back(meshObject);
    mRenderManager.AddObject(meshObject);
    /// need to add non-physical objects to collision
    mCollisionSystem->AddCollisionSkin(meshObject->GetCollisionSkin());
  }


  // mesh test - todo properly
  std::string meshName("none");
  GetConfigFile().GetValue("mesh_name", meshName);
  if (meshName != "none")
  {
    tScalar meshScale = 1.0f;
    tVector3 meshOffset(0.0f);
    int maxTrianglesPerCell = 4;
    tScalar minCellSize = 1.0f;
    tScalar meshElasticity = 0.0f;
    tScalar meshStaticFriction = 0.5f;
    tScalar meshDynamicFriction = 0.3f;
    GetConfigFile().GetValue("mesh_scale", meshScale);
    GetConfigFile().GetValue("mesh_x", meshOffset.x);
    GetConfigFile().GetValue("mesh_y", meshOffset.y);
    GetConfigFile().GetValue("mesh_z", meshOffset.z);
    GetConfigFile().GetValue("mesh_max_triangles_per_cell", maxTrianglesPerCell);
    GetConfigFile().GetValue("mesh_min_cell_size", minCellSize);
    GetConfigFile().GetValue("mesh_elasticity", 
                             meshElasticity);
    GetConfigFile().GetValue("mesh_static_friction", 
                             meshStaticFriction);
    GetConfigFile().GetValue("mesh_dynamic_friction", 
                             meshDynamicFriction);

    tMeshObject* meshObject = new tMeshObject(meshName.c_str(), meshScale, maxTrianglesPerCell, minCellSize);
    meshObject->SetProperties(meshElasticity, 
                              meshStaticFriction,
                              meshDynamicFriction);
    mObjects.push_back(meshObject);
    mRenderManager.AddObject(meshObject);
    /// need to add non-physical objects to collision
    mCollisionSystem->AddCollisionSkin(meshObject->GetCollisionSkin());
  }

  // create some viewports and cameras
  int numViews = 1;
  GetConfigFile().GetValue("num_views", numViews);
  if (numViews > 2) numViews = 2;
  if (numViews < 1) numViews = 1;
  if (mObjects.size() < 2)
    numViews = 1;

  tCamera * camera0 = new tCamera;
  mCameras.push_back(camera0);
  tScalar h = numViews == 1 ? 1.0f : 0.5f;
  tViewport * viewport0 = new tViewport(&mRenderManager,
                                        camera0,
                                        0.0f, 0.0f, 
                                        h, 1.0f);
  mViewports.push_back(viewport0);

  // attach the camera to our object
  camera0->SetPosition(tVector3(-20.0f, 1.0f, 15.0f));
  camera0->SetTarget(tVector3::Zero());
  camera0->SetUp(tVector3::Up());

  // if there's a car, look at it
  if (!mCarObjects.empty())
  {
    tCarObject * car = mCarObjects[0];
    for (int i = 0 ; i < (int) mObjects.size() ; ++i)
    {
      if (mObjects[i] == car)
      {
        mCameraTargetIndex = i;
      }
    }
  }

  if (numViews == 2)
  {
    tCamera * camera1 = new tCamera;
    mCameras.push_back(camera1);
    tViewport * viewport1 = new tViewport(&mRenderManager,
                                          camera1,
                                          0.5f, 0.0f, 
                                          0.5f, 1.0f);
    mViewports.push_back(viewport1);

    // attach the camera to our object
    camera1->SetPosition(tVector3(-20.0f, 1.0f, 15.0f));
    camera1->SetTarget(tVector3::Zero());
    camera1->SetUp(tVector3::Up());

    camera1->SetTargetObject(mObjects[mObjects.size() - 1]);
  }
}

//==============================================================
// LoadAppConfig
//==============================================================
void tJigTestApp::LoadAppConfig()
{
  TRACE_METHOD_ONLY(ONCE_1);

  GetConfigFile().GetValue("collision_system_type", tAppConfig::mCollisionSystemType);
  GetConfigFile().GetValue("collision_grid_size", tAppConfig::mCollisionGridSize);
  GetConfigFile().GetValue("physics_freq", tAppConfig::mPhysicsFrequency);
  GetConfigFile().GetValue("physics_num_collision_iterations", 
                           tAppConfig::mNumCollisionIterations);
  GetConfigFile().GetValue("penetration_relaxation_timesteps", 
                           tAppConfig::mNumPenetrationRelaxationTimesteps);
  GetConfigFile().GetValue("physics_num_contact_iterations", 
                           tAppConfig::mNumContactIterations);
  GetConfigFile().GetValue("allowed_penetration", tAppConfig::mAllowedPenetration);
  GetConfigFile().GetValue("physics_do_shock_step",
                           tAppConfig::mDoShockStep);
  GetConfigFile().GetValue("physics_coll_toll", tAppConfig::mPhysicsCollToll);
  GetConfigFile().GetValue("physics_gravity_x", tAppConfig::mGravity.x);
  GetConfigFile().GetValue("physics_gravity_y", tAppConfig::mGravity.y);
  GetConfigFile().GetValue("physics_gravity_z", tAppConfig::mGravity.z);
  GetConfigFile().GetValue("physics_enable_freezing", tAppConfig::mEnableFreezing);
  GetConfigFile().GetValue("start_paused", mPaused);
  GetConfigFile().GetValue("max_physics_steps_per_frame", 
                           tAppConfig::mMaxPhysicsStepsPerFrame);
  GetConfigFile().GetValue("null_physics_update", 
                           tAppConfig::mNullPhysicsUpdate);
  GetConfigFile().GetValue("physics_solver_type", tAppConfig::mSolverType);
  GetConfigFile().GetValue("movie_interval", tAppConfig::mMovieInterval);

  GetConfigFile().GetValue("indicate_frozen_objects", 
                           tDebugConfig::mIndicateFrozenObjects);
  GetConfigFile().GetValue("render_collision_bounding_boxes",
                           tDebugConfig::mRenderCollisionBoundingBoxes);
  GetConfigFile().GetValue("render_collision_contacts", 
                           tDebugConfig::mRenderCollisionContacts);

  GetConfigFile().GetValue("render_enable", tAppConfig::mRenderEnable);
  GetConfigFile().GetValue("texture_level", tAppConfig::mTextureLevel);
  GetConfigFile().GetValue("render_interpolate", tAppConfig::mRenderInterpolate);
  GetConfigFile().GetValue("cam_speed", tAppConfig::mCamSpeed);
  GetConfigFile().GetValue("object_controller_speed", tAppConfig::mObjectControllerSpeed);
  GetConfigFile().GetValue("projectile_speed", tAppConfig::mProjectileSpeed);

  GetConfigFile().GetValue("physics_vel_threshold", 
                           tAppConfig::mPhysicsVelThreshold);
  GetConfigFile().GetValue("physics_ang_vel_threshold",
                           tAppConfig::mPhysicsAngVelThreshold);
  GetConfigFile().GetValue("physics_pos_threshold", 
                           tAppConfig::mPhysicsPosThreshold);
  GetConfigFile().GetValue("physics_orient_threshold",
                           tAppConfig::mPhysicsOrientThreshold);
  GetConfigFile().GetValue("physics_deactivation_time", 
                           tAppConfig::mPhysicsDeactivationTime);

  GetConfigFile().GetValue("physics_quit_iterations", 
                           mQuitIterations);

  GetConfigFile().GetValue("skybox", 
                           tAppConfig::mSkybox);

  Limit(tAppConfig::mPhysicsFrequency, SCALAR(0.1), SCALAR(10000.0f));

  mPhysics.SetGravity(tAppConfig::mGravity);
  mPhysics.SetNumCollisionIterations(tAppConfig::mNumCollisionIterations);
  mPhysics.SetNumContactIterations(tAppConfig::mNumContactIterations);
  mPhysics.SetNumPenetrationRelaxationTimesteps(tAppConfig::mNumPenetrationRelaxationTimesteps);
  mPhysics.SetAllowedPenetration(tAppConfig::mAllowedPenetration);
  mPhysics.SetCollToll(tAppConfig::mPhysicsCollToll);
  mPhysics.SetDoShockStep(tAppConfig::mDoShockStep);
  mPhysics.EnableFreezing(tAppConfig::mEnableFreezing);
	mPhysics.SetNullUpdate(tAppConfig::mNullPhysicsUpdate);
  if (tAppConfig::mSolverType == "fast")
    mPhysics.SetSolverType(tPhysicsSystem::SOLVER_FAST);
  else if (tAppConfig::mSolverType == "normal")
    mPhysics.SetSolverType(tPhysicsSystem::SOLVER_NORMAL);
  else if (tAppConfig::mSolverType == "normal")
    mPhysics.SetSolverType(tPhysicsSystem::SOLVER_COMBINED);
  else if (tAppConfig::mSolverType == "accumulated")
    mPhysics.SetSolverType(tPhysicsSystem::SOLVER_ACCUMULATED);

  if (tAppConfig::mTextureLevel < 0)
    mRenderManager.SetWireFrameMode(true);

  delete mSkybox;
  mSkybox = new tSkybox(tAppConfig::mSkybox);
  mRenderManager.AddObject(mSkybox);
}


//==============================================================
// Initialise
//==============================================================
bool tJigTestApp::Initialise(const std::string app_name)
{
  TRACE_METHOD_ONLY(ONCE_1);

  bool retval = tSDLApplicationBase::Initialise(app_name);

  LoadAppConfig();

  SetupExperiment();

  SetSleepingParams();

  return retval;
}

//==============================================================
// SetSleepingParams
//==============================================================
void tJigTestApp::SetSleepingParams()
{
  TRACE_METHOD_ONLY(ONCE_3);
  for (unsigned i = 0 ; i < mObjects.size() ; ++i)
  {
    if (mObjects[i]->GetBody())
    {
      mObjects[i]->GetBody()->SetActivityThreshold(
        tAppConfig::mPhysicsVelThreshold,
        tAppConfig::mPhysicsAngVelThreshold);
      mObjects[i]->GetBody()->SetDeactivationThreshold(
        tAppConfig::mPhysicsPosThreshold,
        tAppConfig::mPhysicsOrientThreshold);
      mObjects[i]->GetBody()->SetDeactivationTime(
        tAppConfig::mPhysicsDeactivationTime);
    }
  }
}


//==============================================================
// CreateSphere
//==============================================================
tSphereObject * tJigTestApp::CreateSphere()
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar sphereMinRadius = 1.0f;
  tScalar sphereMaxRadius = 1.0f;
  tScalar sphereElasticity = 0.0f;
  tScalar sphereStaticFriction = 0.5f;
  tScalar sphereDynamicFriction = 0.3f;
  tScalar sphereDensity = 500.0f;
  tScalar sphereX = 0.0f;
  tScalar sphereY = 0.0f;
  tScalar sphereZ = 0.0f;
  GetConfigFile().GetValue("sphere_x", sphereX);
  GetConfigFile().GetValue("sphere_y", sphereY);
  GetConfigFile().GetValue("sphere_z", sphereZ);
  GetConfigFile().GetValue("sphere_min_radius", sphereMinRadius);
  GetConfigFile().GetValue("sphere_max_radius", sphereMaxRadius);
  GetConfigFile().GetValue("sphere_elasticity", sphereElasticity);
  GetConfigFile().GetValue("sphere_static_friction", sphereStaticFriction);
  GetConfigFile().GetValue("sphere_dynamic_friction", sphereDynamicFriction);
  GetConfigFile().GetValue("sphere_density", sphereDensity);

  tSphereObject * sphereObject = 
    new tSphereObject(RangedRandom(sphereMinRadius, sphereMaxRadius));
  sphereObject->SetProperties(sphereElasticity, 
                              sphereStaticFriction, 
                              sphereDynamicFriction);
  sphereObject->SetDensity(sphereDensity);
  mObjects.push_back(sphereObject);
  sphereObject->GetBody()->MoveTo(
    tVector3(sphereX, sphereY, sphereZ + 0.5f * 2.0f * sphereMaxRadius),
    tMatrix33::Identity());
  mRenderManager.AddObject(sphereObject);
  sphereObject->GetBody()->EnableBody();
  return sphereObject;
}


//====================================================================
// CreateCharacter
//====================================================================
tCharacterObject * tJigTestApp::CreateCharacter()
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar radius = 0.5f;
  tScalar height = 2.0f;
  tCharacterObject * characterObject = 
    new tCharacterObject(radius, height);

  mObjects.push_back(characterObject);
  mCharacterObjects.push_back(characterObject);

  characterObject->GetBody()->MoveTo(
    tVector3(0.0f, 0.0f, 0.0f),
    tMatrix33::Identity());
  mRenderManager.AddObject(characterObject);
  characterObject->GetBody()->EnableBody();
  return characterObject;
}

//==============================================================
// CreateCapsule
//==============================================================
tCapsuleObject * tJigTestApp::CreateCapsule()
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar capsuleMinRadius = 1.0f;
  tScalar capsuleMaxRadius = 1.0f;
  tScalar capsuleMinLength = 2.0f;
  tScalar capsuleMaxLength = 2.0f;
  tScalar capsuleElasticity = 0.0f;
  tScalar capsuleStaticFriction = 0.5f;
  tScalar capsuleDynamicFriction = 0.3f;
  tScalar capsuleDensity = 500.0f;
  tScalar capsuleX = 0.0f;
  tScalar capsuleY = 0.0f;
  tScalar capsuleZ = 0.0f;
  GetConfigFile().GetValue("capsule_x", capsuleX);
  GetConfigFile().GetValue("capsule_y", capsuleY);
  GetConfigFile().GetValue("capsule_z", capsuleZ);
  GetConfigFile().GetValue("capsule_min_radius", capsuleMinRadius);
  GetConfigFile().GetValue("capsule_max_radius", capsuleMaxRadius);
  GetConfigFile().GetValue("capsule_min_length", capsuleMinLength);
  GetConfigFile().GetValue("capsule_max_length", capsuleMaxLength);
  GetConfigFile().GetValue("capsule_elasticity", capsuleElasticity);
  GetConfigFile().GetValue("capsule_static_friction", capsuleStaticFriction);
  GetConfigFile().GetValue("capsule_dynamic_friction", capsuleDynamicFriction);
  GetConfigFile().GetValue("capsule_density", capsuleDensity);

  tCapsuleObject * capsuleObject = 
    new tCapsuleObject(RangedRandom(capsuleMinRadius, capsuleMaxRadius),
                       RangedRandom(capsuleMinLength, capsuleMaxLength));
  capsuleObject->SetProperties(capsuleElasticity, 
                               capsuleStaticFriction, 
                               capsuleDynamicFriction);
  capsuleObject->SetDensity(capsuleDensity);
  mObjects.push_back(capsuleObject);
  capsuleObject->GetBody()->MoveTo(
    tVector3(capsuleX, capsuleY, capsuleZ + 0.5f * 2.0f * capsuleMaxRadius),
    Matrix33Gamma(RangedRandom(0.0f, 360.0f)));
  mRenderManager.AddObject(capsuleObject);
  capsuleObject->GetBody()->EnableBody();
  return capsuleObject;
}

//==============================================================
// CreateBox
//==============================================================
tBoxObject * tJigTestApp::CreateBox()
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar boxMinSide = 1.0f;
  tScalar boxMaxSide = 1.0f;
  tScalar boxElasticity = 0.0f;
  tScalar boxStaticFriction = 0.5f;
  tScalar boxDynamicFriction = 0.3f;
  tScalar boxDensity = 500.0f;
  tScalar boxX = 0.0f;
  tScalar boxY = 0.0f;
  tScalar boxZ = 0.0f;
  tVector3 boxScale(1, 1, 1);
  GetConfigFile().GetValue("box_x", boxX);
  GetConfigFile().GetValue("box_y", boxY);
  GetConfigFile().GetValue("box_z", boxZ);
  GetConfigFile().GetValue("box_scale_x", boxScale.x);
  GetConfigFile().GetValue("box_scale_y", boxScale.y);
  GetConfigFile().GetValue("box_scale_z", boxScale.z);
  GetConfigFile().GetValue("box_min_side", boxMinSide);
  GetConfigFile().GetValue("box_max_side", boxMaxSide);
  GetConfigFile().GetValue("box_elasticity", boxElasticity);
  GetConfigFile().GetValue("box_static_friction", boxStaticFriction);
  GetConfigFile().GetValue("box_dynamic_friction", boxDynamicFriction);
  GetConfigFile().GetValue("box_density", boxDensity);

  tBoxObject * boxObject = new tBoxObject(
    ElementMult(boxScale, tVector3(RangedRandom(boxMinSide, boxMaxSide),
                                   RangedRandom(boxMinSide, boxMaxSide),
                                   RangedRandom(boxMinSide, boxMaxSide))), true);
  boxObject->SetProperties(boxElasticity, 
                           boxStaticFriction,
                           boxDynamicFriction);
  boxObject->SetDensity(boxDensity);
  mObjects.push_back(boxObject);
  boxObject->GetBody()->MoveTo(
    tVector3(boxX, boxY, boxZ + 0.5f * boxMaxSide), 
    Matrix33Gamma(RangedRandom(0.0f, 360.0f)));
  mRenderManager.AddObject(boxObject);
  /// physics will register collision for objects passed to it.
  boxObject->GetBody()->EnableBody();
  return boxObject;
}

//==============================================================
// CreateCompound
//==============================================================
tBoxSphereObject * tJigTestApp::CreateCompound()
{
  TRACE_METHOD_ONLY(ONCE_2);
  tScalar compoundMinSide = 1.0f;
  tScalar compoundMaxSide = 1.0f;
  tScalar compoundElasticity = 0.0f;
  tScalar compoundStaticFriction = 0.5f;
  tScalar compoundDynamicFriction = 0.3f;
  tScalar compoundDensity = 500.0f;
  tScalar compoundX = 0.0f;
  tScalar compoundY = 0.0f;
  tScalar compoundZ = 0.0f;
  tVector3 compoundScale(1, 1, 1);
  GetConfigFile().GetValue("compound_x", compoundX);
  GetConfigFile().GetValue("compound_y", compoundY);
  GetConfigFile().GetValue("compound_z", compoundZ);
  GetConfigFile().GetValue("compound_scale_x", compoundScale.x);
  GetConfigFile().GetValue("compound_scale_y", compoundScale.y);
  GetConfigFile().GetValue("compound_scale_z", compoundScale.z);
  GetConfigFile().GetValue("compound_min_side", compoundMinSide);
  GetConfigFile().GetValue("compound_max_side", compoundMaxSide);
  GetConfigFile().GetValue("compound_elasticity", compoundElasticity);
  GetConfigFile().GetValue("compound_static_friction", compoundStaticFriction);
  GetConfigFile().GetValue("compound_dynamic_friction", compoundDynamicFriction);
  GetConfigFile().GetValue("compound_density", compoundDensity);

  tBoxSphereObject * compoundObject = new tBoxSphereObject(
    ElementMult(compoundScale, tVector3(RangedRandom(compoundMinSide, compoundMaxSide),
                                   RangedRandom(compoundMinSide, compoundMaxSide),
                                   RangedRandom(compoundMinSide, compoundMaxSide))), true);
  compoundObject->SetProperties(compoundElasticity, 
                           compoundStaticFriction,
                           compoundDynamicFriction);
  compoundObject->SetDensity(compoundDensity);
  mObjects.push_back(compoundObject);
  compoundObject->GetBody()->MoveTo(
    tVector3(compoundX, compoundY, compoundZ + 0.5f * compoundMaxSide), 
    Matrix33Gamma(RangedRandom(0.0f, 360.0f)));
  mRenderManager.AddObject(compoundObject);
  /// physics will register collision for objects passed to it.
  compoundObject->GetBody()->EnableBody();
  return compoundObject;
}


//==============================================================
// SetupUnequalExperiment
//==============================================================
void tJigTestApp::SetupUnequalExperiment()
{
  TRACE_METHOD_ONLY(ONCE_1);
  tScalar boxElasticity = 0.0f;
  tScalar boxStaticFriction = 0.5f;
  tScalar boxDynamicFriction = 0.3f;
  GetConfigFile().GetValue("box_elasticity", boxElasticity);
  GetConfigFile().GetValue("box_static_friction", boxStaticFriction);
  GetConfigFile().GetValue("box_dynamic_friction", boxDynamicFriction);

  tBoxObject * boxObject = new tBoxObject(tVector3(0.1f, 12.0f, 6.0f), true);
  boxObject->SetProperties(boxElasticity, 
                           boxStaticFriction,
                           boxDynamicFriction);
  boxObject->SetDensity(100.0f);
  mObjects.push_back(boxObject);
  boxObject->GetBody()->MoveTo(tVector3(0.0f, 0.0f, 3.0f), 
                               tMatrix33::Identity());
  mRenderManager.AddObject(boxObject);
  boxObject->GetBody()->EnableBody();

  boxObject = new tBoxObject(tVector3(4.0f, 5.0f, 2.0f), true);
  boxObject->SetProperties(boxElasticity, 
                           boxStaticFriction,
                           boxDynamicFriction);
  boxObject->SetDensity(100.0f);
  mObjects.push_back(boxObject);
  boxObject->GetBody()->MoveTo(tVector3(0.0f, 0.0f, 10.0f), 
                               tMatrix33::Identity());
  mRenderManager.AddObject(boxObject);
  boxObject->GetBody()->EnableBody();
}

//==============================================================
// BuildJengaStack
//==============================================================
void tJigTestApp::BuildJengaStack(int height, 
                                  const tVector3 & boxDims,
                                  const tVector3 & pos)
{
  TRACE_METHOD_ONLY(ONCE_1);
  tScalar boxElasticity = 0.0f;
  tScalar boxStaticFriction = 0.5f;
  tScalar boxDynamicFriction = 0.3f;
  tScalar boxDensity = 500.0f;
  GetConfigFile().GetValue("box_elasticity", boxElasticity);
  GetConfigFile().GetValue("box_static_friction", boxStaticFriction);
  GetConfigFile().GetValue("box_dynamic_friction", boxDynamicFriction);
  GetConfigFile().GetValue("box_density", boxDensity);

  for (int iH = 0 ; iH < height ; ++iH)
  {
    tVector3 thisPos(pos);
    thisPos.z += (iH + 0.5f) * (boxDims.z - tAppConfig::mAllowedPenetration) - tAppConfig::mAllowedPenetration;
    tVector3 leftDir = tVector3::Left();
    tVector3 fwdDir = tVector3::Look();
    tMatrix33 rot(tMatrix33::IDENTITY);
    if (iH % 2)
    {
      rot = Matrix33Gamma(90.0f);
      leftDir = rot * leftDir;
      fwdDir = rot * fwdDir;
    }

    for (int iBox = -1 ; iBox <= 1 ; ++iBox)
    {
      tBoxObject * boxObject = new tBoxObject(boxDims, false);
      boxObject->SetProperties(boxElasticity, 
                               boxStaticFriction,
                               boxDynamicFriction);
      boxObject->SetDensity(boxDensity);
      mObjects.push_back(boxObject);
      boxObject->GetBody()->MoveTo(
        thisPos + (tScalar) iBox * leftDir * boxDims.y,
        rot);
      mRenderManager.AddObject(boxObject);
      /// physics will register collision for objects passed to it.
      boxObject->GetBody()->EnableBody();
    }
  }
}

//==============================================================
// BuildJengaStack
//==============================================================
void tJigTestApp::BuildWall(int height, 
                            int width,
                            const tVector3 & boxDims,
                            const tVector3 & pos,
                            bool pyramid)
{
  TRACE_METHOD_ONLY(ONCE_1);
  tScalar boxElasticity = 0.0f;
  tScalar boxStaticFriction = 0.5f;
  tScalar boxDynamicFriction = 0.3f;
  tScalar boxDensity = 500.0f;
  GetConfigFile().GetValue("box_elasticity", boxElasticity);
  GetConfigFile().GetValue("box_static_friction", boxStaticFriction);
  GetConfigFile().GetValue("box_dynamic_friction", boxDynamicFriction);
  GetConfigFile().GetValue("box_density", boxDensity);

  for (int iH = 0 ; iH < height ; ++iH)
  {
    for (int iW = 0 ; iW < width ; ++iW)
    {
      tVector3 thisPos(pos);
      thisPos.z += (iH + 0.5f) * (boxDims.z - tAppConfig::mAllowedPenetration) - tAppConfig::mAllowedPenetration;
      thisPos.y += iW * boxDims.y - 0.5f * width;
      if (iH % 2)
      {
        if (iW == width-1)
          continue;
        thisPos.y += 0.5f * boxDims.y;
      }

      if (pyramid)
      {
        int iH2 = iH / 2;
        if (iW < iH2)
          continue;
        int maxW = width - (iH2 + 1);
        if (iH % 2)
          maxW -= 1;
        if (iW > maxW)
          continue;
      }
      else
      {
        if ( (iH == height - 1) &&
             (iH % 2 == 0) &&
             ( (iW == 0) || (iW == width - 1) ) )
          continue;
      }      
      tBoxObject * boxObject = new tBoxObject(boxDims, false);
      boxObject->SetProperties(boxElasticity, 
                               boxStaticFriction,
                               boxDynamicFriction);
      boxObject->SetDensity(boxDensity);
      mObjects.push_back(boxObject);
      boxObject->GetBody()->MoveTo(thisPos, tMatrix33::Identity());
      mRenderManager.AddObject(boxObject);
      /// physics will register collision for objects passed to it.
      boxObject->GetBody()->EnableBody();
    }
  }
}

//==============================================================
// RemoveAllObjects
//==============================================================
void tJigTestApp::RemoveAllObjects(bool onlyPhysical)
{
  TRACE_METHOD_ONLY(ONCE_1);

  mObjectController.DisableConstraint();
  mCameraTargetIndex = -1;

  unsigned i;
  unsigned nCameras = mCameras.size();
  for (i = 0 ; i < nCameras ; ++i)
  {
    mCameras[i]->SetTargetObject(0);
    mCameras[i]->SetOrientationObject(0);
    mCameras[i]->SetPositionObject(0);
    mCameras[i]->SetTrackTarget(0, 0, 0, 0);
  }
  for (i = 0 ; i < mObjects.size() ; ++i)
  {
    if (!onlyPhysical || mObjects[i]->GetBody())
    {
      TRACE("Deleting object %d\n", i);
      delete mObjects[i];
      mObjects[i] = 0;
      TRACE("Done deleting object %d\n", i);
    }
  }
  tObjects::iterator pos = remove(mObjects.begin(),
                                  mObjects.end(),
                                  (tObject *) 0); 
  mObjects.erase(pos, mObjects.end());

  mCarAI.RemoveAllCars();
  mCarObjects.resize(0);
  mCharacterObjects.resize(0);
}

//==============================================================
// RestartExperiment
//==============================================================
void tJigTestApp::RestartExperiment()
{
  TRACE_METHOD_ONLY(ONCE_1);
  SetupExperiment();
}

//==============================================================
// CreateBumpyHeightmap
//==============================================================
void tJigTestApp::CreateBumpyHeightmap()
{
  TRACE_METHOD_ONLY(ONCE_1);

  int nx = 128;
  int ny = 128;
  tScalar dx = 1.0f;
  tScalar dy = 1.0f;
  tScalar x0 = 0.0f;
  tScalar y0 = 0.0f;
  tScalar heightmapElasticity = 0.0f;
  tScalar heightmapStaticFriction = 0.5f;
  tScalar heightmapDynamicFriction = 0.3f;
  GetConfigFile().GetValue("heightmap_nx", nx);
  GetConfigFile().GetValue("heightmap_ny", ny);
  GetConfigFile().GetValue("heightmap_dx", dx);
  GetConfigFile().GetValue("heightmap_dy", dy);
  GetConfigFile().GetValue("heightmap_x0", x0);
  GetConfigFile().GetValue("heightmap_y0", y0);
  GetConfigFile().GetValue("heightmap_elasticity", 
                           heightmapElasticity);
  GetConfigFile().GetValue("heightmap_static_friction", 
                           heightmapStaticFriction);
  GetConfigFile().GetValue("heightmap_dynamic_friction", 
                           heightmapDynamicFriction);

  /// create the bumps
  tArray2D<tScalar> heights(nx, ny, 0.0f);

  tScalar lambda = nx * (dx / 5.0f);

  tScalar peakRadius = lambda * 2.0f;
  tScalar peakRadiusDelta = lambda;

  tScalar xMin = x0 - 0.5f * (nx - 1) * dx;
  tScalar yMin = y0 - 0.5f * (ny - 1) * dy;

  int i, j;
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < ny ; ++j)
    {
      tScalar x = xMin + i * dx ;
      tScalar y = yMin + j * dy;
      tScalar radius = Hypot(x, y);
      tScalar scale = Exp(-Sq(radius - peakRadius) / 
                          Sq(peakRadiusDelta));
      tScalar z = 0.2f * scale * lambda * 
        SinDeg(360.0f * x / lambda) * 
        SinDeg(360.0f * y / lambda);
      heights(i, j) = z;
    }
  }

  // make a bumpy bit
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < ny ; ++j)
    {
      tScalar x = xMin + i * dx ;
      tScalar y = yMin + j * dy;
      tScalar radius = Hypot(x - lambda, y - lambda);
      if (radius < lambda)
        heights(i, j) += RangedRandom(-0.1f, 0.1f);
    }
  }

  // make sure the middle is completely flat
  tScalar rMin = 50.0f;
  tScalar rMax = 70.0f;
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < ny ; ++j)
    {
      tScalar x = xMin + i * dx ;
      tScalar y = yMin + j * dy;
      tScalar radius = Hypot(x, y);
      tScalar frac = (radius - rMin) / (rMax - rMin);
      Limit(frac, SCALAR(0.0f), SCALAR(1.0f));
      if (radius < rMax)
      {
        heights(i, j) = heights(i, j) * frac;
      }
    }
  }

  /// create the actual objects
  tHeightmap heightmap(heights, x0, y0, dx, dy);
  tHeightmapObject * heightmapObject = new tHeightmapObject(heightmap);
  heightmapObject->SetProperties(heightmapElasticity, 
                                 heightmapStaticFriction,
                                 heightmapDynamicFriction);
  mObjects.push_back(heightmapObject);
  mRenderManager.AddObject(heightmapObject);

  /// need to add non-physical objects to collision
  mCollisionSystem->AddCollisionSkin(heightmapObject->GetCollisionSkin());
}

//==============================================================
// CreateFileHeightmap
//==============================================================
void tJigTestApp::CreateFileHeightmap(const string & fileName)
{
  TRACE_METHOD_ONLY(ONCE_1);

  int nx = 128;
  int ny = 128;
  tScalar dx = 1.0f;
  tScalar dy = 1.0f;
  tScalar zMin = 0.0f;
  tScalar zMax = 0.0f;
  tScalar heightmapElasticity = 0.0f;
  tScalar heightmapStaticFriction = 0.5f;
  tScalar heightmapDynamicFriction = 0.3f;
  GetConfigFile().GetValue("heightmap_nx", nx);
  GetConfigFile().GetValue("heightmap_ny", ny);
  GetConfigFile().GetValue("heightmap_dx", dx);
  GetConfigFile().GetValue("heightmap_dy", dy);
  GetConfigFile().GetValue("heightmap_zmin", zMin);
  GetConfigFile().GetValue("heightmap_zmax", zMax);
  GetConfigFile().GetValue("heightmap_elasticity", 
                           heightmapElasticity);
  GetConfigFile().GetValue("heightmap_static_friction", 
                           heightmapStaticFriction);
  GetConfigFile().GetValue("heightmap_dynamic_friction", 
                           heightmapDynamicFriction);

  /// create the actual objects
  tHeightmapObject * heightmapObject = 
    new tHeightmapObject(fileName, dx, dy, zMin, zMax);
  heightmapObject->SetProperties(heightmapElasticity, 
                                 heightmapStaticFriction,
                                 heightmapDynamicFriction);
  mObjects.push_back(heightmapObject);
  mRenderManager.AddObject(heightmapObject);
  /// need to add non-physical objects to collision
  mCollisionSystem->AddCollisionSkin(heightmapObject->GetCollisionSkin());
}


//==============================================================
// CreateCar
//==============================================================
tCarObject * tJigTestApp::CreateCar()
{
  tScalar carMinX = -2.0f;
  tScalar carMinY = -1.0f;
  tScalar carMinZ = 0.0f;
  tScalar carMaxX = 2.0f;
  tScalar carMaxY = 1.0f;
  tScalar carMaxZ = 1.5f;
  tScalar carElasticity = 0.0f;
  tScalar carStaticFriction = 0.5f;
  tScalar carDynamicFriction = 0.3f;
  tScalar carDensity = 500.0f;
  tScalar carX = 0.0f;
  tScalar carY = 0.0f;
  tScalar carZ = 0.0f;
  GetConfigFile().GetValue("car_x", carX);
  GetConfigFile().GetValue("car_y", carY);
  GetConfigFile().GetValue("car_z", carZ);
  GetConfigFile().GetValue("car_min_x", carMinX);
  GetConfigFile().GetValue("car_min_y", carMinY);
  GetConfigFile().GetValue("car_min_z", carMinZ);
  GetConfigFile().GetValue("car_max_x", carMaxX);
  GetConfigFile().GetValue("car_max_y", carMaxY);
  GetConfigFile().GetValue("car_max_z", carMaxZ);
  GetConfigFile().GetValue("car_elasticity", carElasticity);
  GetConfigFile().GetValue("car_static_friction", carStaticFriction);
  GetConfigFile().GetValue("car_dynamic_friction", carDynamicFriction);
  GetConfigFile().GetValue("car_density", carDensity);


  bool FWDrive = true;
  bool RWDrive = true;
  tScalar maxSteerAngle = 30.0f;
  tScalar steerRate = 4.0f;
  tScalar wheelSideFriction = 4.0f;
  tScalar wheelFwdFriction = 4.0f;
  tScalar wheelTravel = 0.3f;
  tScalar wheelRadius = 0.4f;
  tScalar wheelZOffset = 0.0f;
  tScalar wheelRestingFrac = 0.25f;
  tScalar wheelDampingFrac = 0.6f;
  int     wheelNumRays = 1;
  tScalar driveTorque = 800.0f;
  tScalar gravity = mPhysics.GetGravity().GetLength();
  GetConfigFile().GetValue("FWDrive", FWDrive);
  GetConfigFile().GetValue("RWDrive", RWDrive);
  GetConfigFile().GetValue("max_steer_angle", maxSteerAngle);
  GetConfigFile().GetValue("steer_rate", steerRate);
  GetConfigFile().GetValue("wheel_side_friction", wheelSideFriction);
  GetConfigFile().GetValue("wheel_fwd_friction", wheelFwdFriction);
  GetConfigFile().GetValue("wheel_travel", wheelTravel);
  GetConfigFile().GetValue("wheel_radius", wheelRadius);
  GetConfigFile().GetValue("wheel_z_offset", wheelZOffset);
  GetConfigFile().GetValue("wheel_resting_frac", wheelRestingFrac);
  GetConfigFile().GetValue("wheel_damping_frac", wheelDampingFrac);
  GetConfigFile().GetValue("wheel_num_rays", wheelNumRays);
  GetConfigFile().GetValue("drive_torque", driveTorque);

  tCarObject * carObject = new tCarObject(
    FWDrive, RWDrive, maxSteerAngle, steerRate,
    wheelSideFriction, wheelFwdFriction, 
    wheelTravel, wheelRadius, wheelZOffset, wheelRestingFrac, wheelDampingFrac,
    wheelNumRays, driveTorque, gravity);
  
  carObject->GetCar().GetChassis().SetDims(
    tVector3(carMinX, carMinY, carMinZ), 
    tVector3(carMaxX, carMaxY, carMaxZ));
  carObject->SetProperties(carElasticity, 
                           carStaticFriction,
                           carDynamicFriction);
  carObject->SetDensity(carDensity);

  carObject->GetBody()->MoveTo(
    tVector3(RangedRandom(-4.0f, 4.0f), 
             RangedRandom(-4.0f, 4.0f), 0.0f) +
    tVector3(carX, carY, carZ + 2.0f), 
    Matrix33Gamma(RangedRandom(0.0f, 0.0f)));

  // store
  mObjects.push_back(carObject);
  mCarObjects.push_back(carObject);

  // register
  mRenderManager.AddObject(carObject);
  carObject->GetCar().EnableCar();

  if (mCarObjects.size() > 1)
    mCarAI.AddControlledCar(carObject);

  return carObject;
}


//==============================================================
// CreateRagdoll
//==============================================================
tRagdoll* tJigTestApp::CreateRagdoll()
{
  tScalar ragdollX = 0.0f;
  tScalar ragdollY = 0.0f;
  tScalar ragdollZ = 0.0f;
  tScalar ragdollDensity = 500.0f;
  bool ragdollStartAsleep = false;
  bool ragdollComplex = false;
  GetConfigFile().GetValue("ragdoll_x", ragdollX);
  GetConfigFile().GetValue("ragdoll_y", ragdollY);
  GetConfigFile().GetValue("ragdoll_z", ragdollZ);
  GetConfigFile().GetValue("ragdoll_density", ragdollDensity);
  GetConfigFile().GetValue("ragdoll_start_asleep", ragdollStartAsleep);
  GetConfigFile().GetValue("ragdoll_complex", ragdollComplex);

  tRagdoll * ragdoll = new tRagdoll(
    &mRenderManager,
    ragdollComplex ? tRagdoll::COMPLEX : tRagdoll::SIMPLE,
    ragdollDensity);
  
  ragdoll->MoveTorso(
    tVector3(RangedRandom(-4.0f, 4.0f), 
             RangedRandom(-4.0f, 4.0f), 0.0f) +
    tVector3(ragdollX, ragdollY, ragdollZ));

  if (ragdollStartAsleep)
    ragdoll->PutToSleep();
  // store
  mObjects.push_back(ragdoll);
  return ragdoll;
}

//==============================================================
// CreateRagdoll
//==============================================================
tNewton* tJigTestApp::CreateNewton()
{
  tScalar newtonX = 0.0f;
  tScalar newtonY = 0.0f;
  tScalar newtonZ = 0.0f;
  tScalar newtonRadius = 1.0f;
  int balls = 5;
  GetConfigFile().GetValue("newton_x", newtonX);
  GetConfigFile().GetValue("newton_y", newtonY);
  GetConfigFile().GetValue("newton_z", newtonZ);
  GetConfigFile().GetValue("newton_radius", newtonRadius);
  GetConfigFile().GetValue("newton_balls", balls);

  static tScalar yOffset = 0.0f;

  tNewton * newton = new tNewton(
    &mRenderManager,
    balls,
    tVector3(newtonX, newtonY + yOffset, newtonZ),
    newtonRadius);

  yOffset += newtonRadius * 4.0f;
  mObjects.push_back(newton);
  return newton;
}

