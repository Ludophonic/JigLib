//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file jigtestapp.hpp 
//                     
//==============================================================
#ifndef JIGTESTAPP_HPP
#define JIGTESTAPP_HPP

#include "sdlapplicationbase.hpp"
#include "rendermanager.hpp"
#include "carai.hpp"

#include "jiglib.hpp"

#include <vector>
#include <string>

class tJigTestApp : public tSDLApplicationBase
{
public:
  tJigTestApp(int & argc, char * argv[]);
  ~tJigTestApp();
  
  // inherited from tSDLApplicationBase
  class JigLib::tPhysicsSystem * GetPhysicsSystem() {return &mPhysics;}
  bool Initialise(const std::string app_name);
  void StartMainLoop();
  
  // event handlers
  void HandleKeyDown(const SDL_keysym & keysym );
  void HandleKeyUp(const SDL_keysym & keysym );
  void HandleFPSKeyDown(const SDL_keysym & keysym );
  void HandleFPSKeyUp(const SDL_keysym & keysym );
  void HandleFPSMouseMotion(const SDL_MouseMotionEvent & event);
  void HandleMouseMotion(const SDL_MouseMotionEvent & event);
  void HandleMouseButtonDown(const SDL_MouseButtonEvent & event);
  void HandleMouseButtonUp(const SDL_MouseButtonEvent & event);
  void ProcessMainEvent();
  
  void AddObject(class tObject * object);

private:
  void LoadAppConfig();
  
  /// height is the number of levels
  /// pos is the position of the centre of the bottom of the stack
  void BuildJengaStack(int height, 
                       const JigLib::tVector3 & boxDims,
                       const JigLib::tVector3 & pos);
  
  void BuildWall(int height, 
                 int width,
                 const JigLib::tVector3 & boxDims,
                 const JigLib::tVector3 & pos,
                 bool pyramid);

  void CreateBumpyHeightmap();
  
  void CreateFileHeightmap(const std::string & fileName);

  void SetupExperiment();
  
  void SetupUnequalExperiment();
  
  void FireObject();
  
  /// Fires the last created object, but doesn't change the camera focus
  void FireLastCreatedObject();

  void RemoveAllObjects(bool onlyPhysical);
  
  void RestartExperiment();
  
  void SetSleepingParams();

  class tBoxObject * CreateBox();
  
  class tBoxSphereObject * CreateCompound();
  
  class tSphereObject * CreateSphere();
  
  class tCapsuleObject * CreateCapsule();

  class tCarObject * CreateCar();
  
  class tCharacterObject * CreateCharacter();

  class tRagdoll * CreateRagdoll();

  class tNewton * CreateNewton();

  enum tObjectControl {CONTROL_UP, CONTROL_DOWN, CONTROL_LEFT, CONTROL_RIGHT};
  void ControlMainObject(tObjectControl control, bool down);
  void ControlMainCar(tObjectControl control, bool down);
  
  enum tMoveDir {MOVE_NONE  = 1 << 0,
                 MOVE_FWD   = 1 << 1,
                 MOVE_BACK  = 1 << 2,
                 MOVE_LEFT  = 1 << 3,
                 MOVE_RIGHT = 1 << 4,
                 MOVE_UP    = 1 << 5,
                 MOVE_DOWN  = 1 << 6};
  
  /// Our physics system
  JigLib::tPhysicsSystem mPhysics;
  
  /// create the collision system on the heap since we might want to
  /// choose the type at run-time (from config)
  JigLib::tCollisionSystem * mCollisionSystem;
  
  /// The thing we use to manager all renderable objects
  tRenderManager mRenderManager;
  
  /// could have a number of viewports...
  std::vector<class tViewport *> mViewports;
  
  /// could have a number of cameras, for example one per viewport
  std::vector<class tCamera *> mCameras;
  
  /// Used to move objects by constraining their velocity
  JigLib::tConstraintVelocity mObjectController;

  /// Used to move objects by constraining their position
  JigLib::tConstraintWorldPoint mObjectPositionController;
  /// damps velocity on the position-controlled object
  JigLib::tConstraintVelocity mObjectPositionDamperController;

  /// all our things
  typedef std::vector<class tObject *> tObjects;
  tObjects mObjects;
  
  /// Additional list for cars so that the user can control them
  typedef std::vector<class tCarObject *> tCarObjects;
  tCarObjects mCarObjects;
  
  /// Additional list for characters so that the user can control them
  typedef std::vector<class tCharacterObject *> tCharacterObjects;
  tCharacterObjects mCharacterObjects;
  
  /// index into mObjects for the camera target. if -1 then stops following
  int mCameraTargetIndex;
  
  /// For camera movement
  unsigned mCameraMoveDir;
  /// actual camera vel
  JigLib::tVector3 mCameraMoveVel;

  enum tCameraMode {CAMERA_FIXED, CAMERA_FOLLOW};
  tCameraMode mCameraMode;
  
  /// the average duration of each physics step
  JigLib::tScalar mPhysicsDuration;
  
  /// pause physics
  bool mPaused;
  
  /// allow single-stepping
  bool mPauseAfterNextPhysicsStep;
  
  /// AI controlled cars
  tCarAI mCarAI;

  /// maybe have a skybox
  class tSkybox * mSkybox;

  /// true if we're picking/moving an object with the mouse
  bool mPicking;

  /// When picking this is the original distance from the camera to the object
  JigLib::tScalar mPickDist;

  /// Record a movie?
  bool mDoMovie;

  /// if non-zero then quit after this many physics updates -
  /// e.g. when profiling
  int mQuitIterations;

  /// Counts the number of physics updates we've done
  int mPhysicsCounter;
};

#endif
