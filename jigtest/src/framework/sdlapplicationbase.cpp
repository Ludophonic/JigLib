//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file SDLApplicationbase.cpp 
//                     
//==============================================================
#include "sdlapplicationbase.hpp"
#include "graphics.hpp"
#include <stdio.h>

using namespace std;
using namespace JigLib;

void tSDLApplicationBase::LoadConfig(int & argc, char * argv[], 
                                     string configFileName)
{
  // setup config/tracing
  bool configFileOk;
  if (argc > 1)
    configFileName = string(argv[1]);
  mConfigFile = new tConfigFile(configFileName, configFileOk);
  
  if (!configFileOk)
    TRACE("Warning: Unable to open main config file: %s\n", configFileName.c_str());
  
  // initialise trace
  // set up tracing properly
  bool traceEnabled = true;
  int traceLevel = 3;
  bool traceAllStrings = true;
  vector<string> traceStrings;
  
  mConfigFile->GetValue("trace_enabled", traceEnabled);
  mConfigFile->GetValue("trace_level", traceLevel);
  mConfigFile->GetValue("trace_all_strings", traceAllStrings);
  mConfigFile->GetValues("trace_strings", traceStrings);
  
  EnableTrace(traceEnabled);
  SetTraceLevel(traceLevel);
  EnableTraceAllStrings(traceAllStrings);
  AddTraceStrings(traceStrings);
  
  TRACE_FILE_IF(ONCE_1)
    TRACE("Logging set up\n");
}


//==============================================================
// tSDLApplicationBase
//==============================================================
tSDLApplicationBase::tSDLApplicationBase(int & argc, char * argv[], 
                                         string configFileName,
                                         void (* licenseFn)(void))
{
  TRACE_METHOD_ONLY(ONCE_1);  
  
  LoadConfig(argc, argv, configFileName);
  
  if (licenseFn)
    (*licenseFn)();
  else
    DisplayLicense();
}

//==============================================================
// ~tSDLApplicationBase
//==============================================================
tSDLApplicationBase::~tSDLApplicationBase()
{
  TRACE_METHOD_ONLY(ONCE_1);  
}

//==============================================================
// Initialise
//==============================================================
bool tSDLApplicationBase::Initialise(const std::string app_name)
{
  TRACE_METHOD_ONLY(ONCE_1);  
  
  // intialise some vars
  mStartOfFrameTime = tTime(0.0f);
  mOldStartOfFrameTime = tTime(0.0f);
  mFPSInterval = tTime(1.0f);
  mLastStoredTime = tTime(0.0f);
  mFPSCounter = 0;
  mFPS = 1.0f;
  mNewFPS = true;
  
  TRACE_FILE_IF(ONCE_2)
    TRACE("Initializing SDL.\n");
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) { 
    TRACE("Could not initialize SDL: %s.\n", SDL_GetError());
    return false;
  }
  /* Clean up on exit */
  atexit(SDL_Quit);
  
  TRACE_FILE_IF(ONCE_2)
    TRACE("SDL initialized.\n");
  
  // Information about the current video settings.
  const SDL_VideoInfo* info = NULL;
  // get some video information.
  info = SDL_GetVideoInfo( );
  
  if( !info ) {
    TRACE("Video query failed: %s\n",
          SDL_GetError( ) );
    return false;
  }
  
  mWindowWidth = 800;
  mWindowHeight = 600;
  mConfigFile->GetValue("window_width", mWindowWidth);
  mConfigFile->GetValue("window_height", mWindowHeight);
  int bpp = info->vfmt->BitsPerPixel;
  
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  
  bool fullscreen = false;
  mConfigFile->GetValue("fullscreen", fullscreen);
  
  // don't allow resizing for now - textures etc get zapped
  int flags = SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0);
  SDL_Surface *screen = SDL_SetVideoMode( mWindowWidth,
                                          mWindowHeight,
                                          bpp,
                                          flags );
  if( screen == 0 ) 
  {
    TRACE("Video mode set failed: %s\n", SDL_GetError( ) );
    return false;
  }

  return true;
}

//==============================================================
// GetTimeNow
//==============================================================
tTime tSDLApplicationBase::GetTimeNow() const
{
  Uint32 millisec = SDL_GetTicks();
  return millisec * 0.001f;
}

//==============================================================
// InternalHandleStartOfLoop
//==============================================================
void tSDLApplicationBase::InternalHandleStartOfLoop()
{
  mOldStartOfFrameTime = mStartOfFrameTime;
  mStartOfFrameTime = GetTimeNow();
  if ( (mStartOfFrameTime > 0.0f) && 
       (mStartOfFrameTime < mFPSInterval) )
  {
    mFPS = mFPSCounter / mStartOfFrameTime;
  }
  
  if (mStartOfFrameTime - mLastStoredTime >= mFPSInterval)
  {
    mFPS = (tScalar) mFPSCounter;
    mLastStoredTime = mStartOfFrameTime;
    mFPSCounter = 0;
    mNewFPS = true;
  }
  else
  {
    ++mFPSCounter;
    mNewFPS = false;
  }
}


//==============================================================
// StartMainLoop
//==============================================================
void tSDLApplicationBase::StartMainLoop()
{
  mExitMainLoop = false;
  
  while( false == mExitMainLoop ) 
  {
    InternalHandleStartOfLoop();
    // do all the basic stuff like keyboard, mouse etc, callint the
    // app.
    ProcessEvents();
    // get the app to do it's own stuff - e.g. physics, rendering
    ProcessMainEvent();
  }
  
}

//==============================================================
// InternalHandleResize
//==============================================================
void tSDLApplicationBase::InternalHandleResize(const SDL_ResizeEvent & event)
{
  bool fullscreen = false;
  mConfigFile->GetValue("fullscreen", fullscreen);
  mWindowWidth = event.w;
  mWindowHeight = event.h;

  const SDL_VideoInfo* info = SDL_GetVideoInfo( );
  if( !info ) {
    TRACE("Video query failed: %s\n",
          SDL_GetError( ) );
    return;
  }
  int bpp = info->vfmt->BitsPerPixel;
  
  int flags = SDL_RESIZABLE | SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0);
  SDL_Surface *screen = SDL_SetVideoMode( mWindowWidth,
                                          mWindowHeight,
                                          bpp,
                                          flags );
  if( screen == 0 ) 
  {
    TRACE("Video mode set failed: %s\n", SDL_GetError( ) );
  }
}


//==============================================================
// ProcessEvents
//==============================================================
void tSDLApplicationBase::ProcessEvents()
{
  SDL_Event event;
  
  while( SDL_PollEvent( &event ) ) 
  {
    switch( event.type ) {
    case SDL_KEYDOWN:
      HandleKeyDown( event.key.keysym );
      break;
    case SDL_KEYUP:
      HandleKeyUp( event.key.keysym );
      break;
    case SDL_MOUSEMOTION:
      HandleMouseMotion( event.motion );
      break;
    case SDL_MOUSEBUTTONDOWN:
      HandleMouseButtonDown( event.button );
      break;
    case SDL_MOUSEBUTTONUP:
      HandleMouseButtonUp( event.button );
      break;
    case SDL_VIDEORESIZE:
      InternalHandleResize( event.resize );
      break;
    case SDL_QUIT:
      /* Handle quit requests (like Ctrl-c). */
      exit(0);
      break;
    }
  }
}

//==============================================================
// HandleReShape
//==============================================================
void tSDLApplicationBase::HandleReShape(int newWidth, int newHeight)
{
}


//==============================================================
// DisplayLicense
//==============================================================
void tSDLApplicationBase::DisplayLicense()
{
  TRACE("Application Copyright 2004 Danny Chapman: danny@rowlhouse.freeserve.co.uk\n");
  TRACE("Application comes with ABSOLUTELY NO WARRANTY;\n");
  TRACE("This is free software, and you are welcome to redistribute it\n");
  TRACE("under certain conditions; see the GNU General Public License\n");
}



//===========================================================
// Code to write out screen shots
//==========================================================
static void FlipVertical(unsigned char *data, int w, int h)
{
  int x, y, i1, i2;
  unsigned char temp;
  for (x=0;x<w;x++){
    for (y=0;y<h/2;y++){
      i1 = (y*w + x)*3; // this pixel
      i2 = ((h - y - 1)*w + x)*3; // its opposite (across x-axis)

      // swap pixels
      temp = data[i1];
      data[i1] = data[i2];
      data[i2] = temp;

      i1++; i2++;
      temp = data[i1];
      data[i1] = data[i2];
      data[i2] = temp;

      i1++; i2++;
      temp = data[i1];
      data[i1] = data[i2];
      data[i2] = temp;
    }
  }
}


//==============================================================
// writeFrameBuffer
//==============================================================
static void WriteFrameBuffer(char *filename)
{
  FILE *fp = fopen(filename, "wb");
  int width, height;
  GetWindowSize(width, height);
  
  int data_size = width * height * 3;
  unsigned char *framebuffer = 
    (unsigned char *) malloc(data_size * sizeof(unsigned char));
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
  FlipVertical(framebuffer, width, height);

  fprintf(fp, "P6\n%d %d\n%d\n", width, height, 255);
  fwrite(framebuffer, data_size, 1, fp);
  fclose(fp);
  free(framebuffer);
}

//==============================================================
// DoScreenshot
//==============================================================
void tSDLApplicationBase::DoScreenshot()
{
  static int count = 0;
  static char screen_file[] = "screenshot-00000.ppm";
  sprintf(screen_file, "screenshot-%05d.ppm", count++);
//  printf("%s\n", movie_file);
  WriteFrameBuffer(screen_file);
}



