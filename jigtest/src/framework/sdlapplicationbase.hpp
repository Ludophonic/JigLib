//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file sdlapplicationbase.hpp 
//                     
//==============================================================
#ifndef SDLAPPLICATIONBASE_HPP
#define SDLAPPLICATIONBASE_HPP

#include "graphics.hpp"
#include "jiglib.hpp"

#include <string>

/// This is a base class for a SDL-based application. The application
/// just implements the relevant virtual functions. It should handle
/// deletion
class tSDLApplicationBase
{
public:
  /// constructor generates a config file (which can be subsequently
  /// used by the application) and sorts out tracing.  success
  /// indicates whether or not the application initialised enough to
  /// be useful.
  tSDLApplicationBase(int & argc, char * argv[], 
                      std::string configFileName,
                      void (* licenseFn)(void) = 0);
  
  /// Derived class should clean up!
  virtual ~tSDLApplicationBase();
  
  /// creates a double buffered window according to the parameters in
  /// the config file, except the name is passed in (so it can be
  /// hard-coded). Returns true/false to indicate if all is well
  virtual bool Initialise(const std::string app_name);
  
  /// start the main loop - this may return. Should have already
  /// called Initialise.
  virtual void StartMainLoop();
  
  /// Call this to signal to the base class that the main loop should
  /// exit at the next available time.
  void ExitMainLoop() {mExitMainLoop = true;}
  
  /// Return the config file opened at creation
  JigLib::tConfigFile & GetConfigFile() {return *mConfigFile;}
  
  // application can over-ride these. Default implementations do
  // nothing.
  virtual void HandleKeyDown( const SDL_keysym & keysym ) {}
  virtual void HandleKeyUp( const SDL_keysym & keysym ) {}
  virtual void HandleMouseMotion( const SDL_MouseMotionEvent & event ) {}
  virtual void HandleMouseButtonDown( const SDL_MouseButtonEvent & event ) {}
  virtual void HandleMouseButtonUp( const SDL_MouseButtonEvent & event ) {}
  virtual void ProcessMainEvent() {}
  
  /// only x and y are set in the mouse position
  const JigLib::tVector3 & GetMousePos() const {return mMousePos;}
  
  /// default implementations do something sensible - though you might
  /// want to over-ride
  virtual void HandleReShape(int newWidth, int newHeight);
  
  /// The time at the start of this frame - constant within a frame
  JigLib::tTime GetStartOfFrameTime() const {return mStartOfFrameTime;}
  
  /// Time at the start of the previous frame
  JigLib::tTime GetOldStartOfFrameTime() const {return mOldStartOfFrameTime;}
  
  /// The current time - will/may increment as the frame processing
  /// progresses (depending on the system accuracy)
  JigLib::tTime GetTimeNow() const ;
  
  /// The FPS, averaged over a suitable period.
  JigLib::tScalar GetFPS() const {return mFPS;}
  
  /// flag indicates if FPS has just been changed (may want to print it out)
  bool GetJustCalculatedNewFPS() const {return mNewFPS;}

  /// Helper - dumps the current window
  void DoScreenshot();
  
private:
  /// Displays the default application license details (should be GNU
  /// GPL!)
  void DisplayLicense();
  
  /// Loads the main configuration
  void LoadConfig(int & argc, char * argv[], 
                  std::string configFileName);
  
  /// internally distribute events to virtual functions.
  void ProcessEvents();
  
  /// internally do some things that need to be done at the start of
  /// each round of the event loop
  void InternalHandleStartOfLoop();
  
  void InternalHandleResize(const SDL_ResizeEvent & event);

  /// the current window/screen dims
  int mWindowWidth, mWindowHeight;
  
  /// the config file - will be non-zero
  JigLib::tConfigFile * mConfigFile;
  
  /// the current mouse position (only first two vals used)
  JigLib::tVector3 mMousePos;
  
  /// exit from the main loop
  bool mExitMainLoop;
  
  // Stuff to calculate the FPS/time etc
  /// The time (in seconds) of the frame start
  JigLib::tTime mStartOfFrameTime;
  /// From the last frame
  JigLib::tTime mOldStartOfFrameTime;
  /// How often to calculate FPS
  JigLib::tTime mFPSInterval;
  /// Last time we calculated FPS
  JigLib::tTime mLastStoredTime;
  /// How many frames since mLastStoredTime
  int mFPSCounter;
  /// the actual FPS
  JigLib::tScalar mFPS;
  /// just got a new one
  bool mNewFPS;
};

#endif
