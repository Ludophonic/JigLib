# Microsoft Developer Studio Project File - Name="jiglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jiglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jiglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jiglib.mak" CFG="jiglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jiglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jiglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jiglib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Gi /O2 /Ob2 /I "include" /I "collision\include" /I "core\include" /I "geometry\include" /I "maths\include" /I "physics\include" /I "utils\include" /I "vehicles\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "RELEASE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\JigLibRelease.lib"

!ELSEIF  "$(CFG)" == "jiglib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gi /GX /Zi /Od /I "include" /I "collision\include" /I "core\include" /I "geometry\include" /I "maths\include" /I "physics\include" /I "utils\include" /I "vehicles\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\JigLibDebug.lib"

!ENDIF 

# Begin Target

# Name "jiglib - Win32 Release"
# Name "jiglib - Win32 Debug"
# Begin Group "collision_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\collision\include\colldetectboxbox.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectboxheightmap.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectboxplane.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectboxstaticmesh.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectcapsulebox.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectcapsulecapsule.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectcapsuleheightmap.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectcapsuleplane.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectcapsulestaticmesh.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectspherebox.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectspherecapsule.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectsphereheightmap.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectsphereplane.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectspheresphere.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\colldetectspherestaticmesh.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collision.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collisioninfo.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collisionskin.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collisionsystem.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collisionsystembrute.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\collisionsystemgrid.hpp
# End Source File
# Begin Source File

SOURCE=.\collision\include\materials.hpp
# End Source File
# End Group
# Begin Group "collision_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\collision\src\colldetectboxbox.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectboxheightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectboxplane.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectboxstaticmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectcapsulebox.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectcapsulecapsule.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectcapsuleheightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectcapsuleplane.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectcapsulestaticmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectspherebox.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectspherecapsule.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectsphereheightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectsphereplane.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectspheresphere.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\colldetectspherestaticmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\collisioninfo.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\collisionskin.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\collisionsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\collisionsystembrute.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\collisionsystemgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\collision\src\materials.cpp
# End Source File
# End Group
# Begin Group "geometry_include"

# PROP Default_Filter ""
# Begin Group "primitives-inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\geometry\include\aabox.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\aabox.inl
# End Source File
# Begin Source File

SOURCE=.\geometry\include\box.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\capsule.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\heightmap.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\plane.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\primitive.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\sphere.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\trianglemesh.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\geometry\include\distance.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\distance.inl
# End Source File
# Begin Source File

SOURCE=.\geometry\include\geometry.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\indexedtriangle.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\intersection.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\intersection.inl
# End Source File
# Begin Source File

SOURCE=.\geometry\include\line.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\octree.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\overlap.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\overlap.inl
# End Source File
# Begin Source File

SOURCE=.\geometry\include\plane.inl
# End Source File
# Begin Source File

SOURCE=.\geometry\include\rectangle.hpp
# End Source File
# Begin Source File

SOURCE=.\geometry\include\triangle.hpp
# End Source File
# End Group
# Begin Group "geometry_src"

# PROP Default_Filter ""
# Begin Group "primitives-src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\geometry\src\aabox.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\box.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\capsule.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\heightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\primitive.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\trianglemesh.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\geometry\src\distance.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\indexedtriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\intersection.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\line.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\octree.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\overlap.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\src\triangle.cpp
# End Source File
# End Group
# Begin Group "maths_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\maths\include\criticaldamping.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\maths.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\mathsmisc.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\matrix33.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\matrix33.inl
# End Source File
# Begin Source File

SOURCE=.\maths\include\precision.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\transform3.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\vector2.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\vector3.hpp
# End Source File
# Begin Source File

SOURCE=.\maths\include\vector3.inl
# End Source File
# End Group
# Begin Group "maths_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\maths\src\matrix33.cpp
# End Source File
# Begin Source File

SOURCE=.\maths\src\vector3.cpp
# End Source File
# End Group
# Begin Group "physics_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\physics\include\body.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\body.inl
# End Source File
# Begin Source File

SOURCE=.\physics\include\constraint.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\constraintmaxdistance.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\constraintpoint.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\constraintvelocity.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\constraintworldpoint.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\hingejoint.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\joint.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\physics.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\physicscontroller.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\include\physicssystem.hpp
# End Source File
# End Group
# Begin Group "physics_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\physics\src\body.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\constraint.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\constraintmaxdistance.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\constraintpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\constraintvelocity.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\constraintworldpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\hingejoint.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\physicscollision.hpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\physicscontroller.cpp
# End Source File
# Begin Source File

SOURCE=.\physics\src\physicssystem.cpp

!IF  "$(CFG)" == "jiglib - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "jiglib - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "utils_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utils\include\array2d.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\assert.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\configfile.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\fixedvector.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\time.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\timer.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\trace.hpp
# End Source File
# Begin Source File

SOURCE=.\utils\include\utils.hpp
# End Source File
# End Group
# Begin Group "utils_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utils\src\array2d.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\src\assert.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\src\configfile.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\src\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\src\trace.cpp
# End Source File
# End Group
# Begin Group "vehicles_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vehicles\include\car.hpp
# End Source File
# Begin Source File

SOURCE=.\vehicles\include\chassis.hpp
# End Source File
# Begin Source File

SOURCE=.\vehicles\include\vehicles.hpp
# End Source File
# Begin Source File

SOURCE=.\vehicles\include\wheel.hpp
# End Source File
# End Group
# Begin Group "vehicles_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vehicles\src\car.cpp
# End Source File
# Begin Source File

SOURCE=.\vehicles\src\chassis.cpp
# End Source File
# Begin Source File

SOURCE=.\vehicles\src\wheel.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\include\jiglib.hpp
# End Source File
# Begin Source File

SOURCE=.\include\jiglibconfig.hpp
# End Source File
# End Target
# End Project
