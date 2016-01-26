# Microsoft Developer Studio Project File - Name="jigtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=jigtest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jigtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jigtest.mak" CFG="jigtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jigtest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "jigtest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jigtest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /Ob2 /I "..\..\include" /I "application" /I "framework" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RELEASE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release\jigtest.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 JigLibRelease.lib SDLMain.lib SDL.lib libpng.lib zlib.lib libxml2_a.lib iconv_a.lib Ws2_32.lib glu32.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\jigtest-release.exe" /libpath:"../../lib"
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "jigtest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\include" /I "application" /I "framework" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "NO_XML" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug\jigtest.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JigLibDebug.lib SDLMain.lib SDL.lib SDLmain.lib libpng.lib glu32.lib opengl32.lib libxml2_a.lib iconv_a.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"Debug\jigtest-debug.pdb" /debug /machine:I386 /out:"../jigtest-debug.exe" /pdbtype:sept /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "jigtest - Win32 Release"
# Name "jigtest - Win32 Debug"
# Begin Group "framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\framework\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\camera.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\graphics.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\imageloader.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\rendermanager.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\rendermanager.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\renderobject.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\renderobject.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\sdlapplicationbase.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\sdlapplicationbase.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\skybox.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\skybox.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\texture.hpp
# End Source File
# Begin Source File

SOURCE=.\framework\viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\framework\viewport.hpp
# End Source File
# End Group
# Begin Group "application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\application\appconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\application\appconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\application\boxchain.cpp
# End Source File
# Begin Source File

SOURCE=.\application\boxchain.hpp
# End Source File
# Begin Source File

SOURCE=.\application\boxobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\boxobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\boxsphereobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\boxsphereobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\capsuleobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\capsuleobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\carai.cpp
# End Source File
# Begin Source File

SOURCE=.\application\carai.hpp
# End Source File
# Begin Source File

SOURCE=.\application\carobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\carobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\characterobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\characterobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\contactrender.cpp
# End Source File
# Begin Source File

SOURCE=.\application\contactrender.hpp
# End Source File
# Begin Source File

SOURCE=.\application\debugconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\application\debugconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\application\heightmapobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\heightmapobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\jigtestapp.cpp
# End Source File
# Begin Source File

SOURCE=.\application\jigtestapp.hpp
# End Source File
# Begin Source File

SOURCE=.\application\main.cpp
# End Source File
# Begin Source File

SOURCE=.\application\meshobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\meshobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\newton.cpp
# End Source File
# Begin Source File

SOURCE=.\application\newton.hpp
# End Source File
# Begin Source File

SOURCE=.\application\object.cpp
# End Source File
# Begin Source File

SOURCE=.\application\object.hpp
# End Source File
# Begin Source File

SOURCE=.\application\planeobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\planeobject.hpp
# End Source File
# Begin Source File

SOURCE=.\application\ragdoll.cpp
# End Source File
# Begin Source File

SOURCE=.\application\ragdoll.hpp
# End Source File
# Begin Source File

SOURCE=.\application\spherechain.cpp
# End Source File
# Begin Source File

SOURCE=.\application\spherechain.hpp
# End Source File
# Begin Source File

SOURCE=.\application\sphereobject.cpp
# End Source File
# Begin Source File

SOURCE=.\application\sphereobject.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\jigtest.cfg
# End Source File
# Begin Source File

SOURCE=..\jigtest1.cfg
# End Source File
# Begin Source File

SOURCE=..\jigtest2.cfg
# End Source File
# Begin Source File

SOURCE=..\jigtest_castle.cfg
# End Source File
# Begin Source File

SOURCE=..\program.log
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# Begin Source File

SOURCE=..\..\todo.txt
# End Source File
# End Target
# End Project
