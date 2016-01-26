#include "jigtestapp.hpp"

#include "jiglib.hpp"

#include <string>

using namespace std;
using namespace JigLib;

// need this to fix the reference to __ftol2 when libxml is built on VC7 but
// the app is built using vc6
//f (_MSC_VER >= 1300) && (WINVER < 0x0500)
//VC7 or later, building with pre-VC7 runtime libraries
//tern "C" long _ftol( double ); //defined by VC6 C libs
//tern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); }
//ndif


// only for MSDEV 6.0
#if (_MSC_VER == 1200) && (WINVER < 0x0500)
extern "C" long _ftol( double ); //defined by VC6 C libs
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); } 
#endif


//==============================================================
// main
//==============================================================
int main(int argc, char * argv[])
{
  // scope this to control when the app gets deleted
  {
    tJigTestApp jigTestApp(argc, argv);
    
    jigTestApp.Initialise("JigTest");
    jigTestApp.StartMainLoop();
  }
  
  TRACE_FILE_IF(ONCE_1)
    TRACE("Program exiting normally\n");
  
  return 0;
}
