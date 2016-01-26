//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file carobject.cpp 
//                     
//==============================================================
#include "carobject.hpp"
#include "debugconfig.hpp"

using namespace JigLib;

//==============================================================
// tCarObject
//==============================================================
tCarObject::tCarObject(bool FWDrive,
                       bool RWDrive,
                       tScalar maxSteerAngle,
                       tScalar steerRate,
                       tScalar wheelSideFriction,
                       tScalar wheelFwdFriction,
                       tScalar wheelTravel,
                       tScalar wheelRadius,
                       tScalar wheelZOffset,
                       tScalar wheelRestingFrac,
                       tScalar wheelDampingFrac,
                       int wheelNumRays,
                       tScalar driveTorque,
                       tScalar gravity)
  :
  mCar(FWDrive, RWDrive, maxSteerAngle, steerRate, 
       wheelSideFriction, wheelFwdFriction, wheelTravel, wheelRadius, 
       wheelZOffset, wheelRestingFrac, wheelDampingFrac,
       wheelNumRays, driveTorque, gravity)
{
  SetMass(1.0f);
  
  mDisplayListNum = 0;
}

//==============================================================
// tCarObject
//==============================================================
tCarObject::~tCarObject()
{
}

//==============================================================
// SetProperties
//==============================================================
void tCarObject::SetProperties(JigLib::tScalar elasticity, 
                               JigLib::tScalar staticFriction,
                               JigLib::tScalar dynamicFriction)
{
  mCar.GetChassis().GetSkin().SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// SetDensity
//==============================================================
void tCarObject::SetDensity(JigLib::tScalar density)
{
  tCollisionSkin & boxSkin = mCar.GetChassis().GetSkin();
  tScalar totalVolume = 0.0f;
  for (unsigned i = 0 ; i < boxSkin.GetNumPrimitives() ; ++i)
  {
    const tBox &box = boxSkin.GetPrimitiveLocal(i)->GetBox();
    totalVolume += box.GetVolume();
  }
  tScalar mass = density * totalVolume;
  SetMass(mass);
}

//==============================================================
// SetMass
//==============================================================
void tCarObject::SetMass(JigLib::tScalar mass)
{
  GetBody()->SetMass(mass);
  
  tVector3 min, max;
  mCar.GetChassis().GetDims(min, max);
  
  const tVector3 & sides = max - min;
  tScalar Ixx = (1.0f / 12.0f) * mass * 
    (Sq(sides.y) + Sq(sides.z));
  tScalar Iyy = (1.0f / 12.0f) * mass * 
    (Sq(sides.x) + Sq(sides.z));
  tScalar Izz = (1.0f / 12.0f) * mass * 
    (Sq(sides.x) + Sq(sides.y));
  
  GetBody()->SetBodyInertia(Ixx, Iyy, Izz);
  mCar.SetupDefaultWheels();
}

//==============================================================
// SetRenderPosition
//==============================================================
void tCarObject::SetRenderPosition(JigLib::tScalar renderFraction)
{
  TRACE_METHOD_ONLY(FRAME_2);
  tBody & body = mCar.GetChassis().GetBody();
  if (renderFraction < SCALAR_TINY)
  {
    mRenderPosition = body.GetOldPosition();
    mRenderOrientation = body.GetOldOrientation();
  }
  else if (renderFraction > (1.0f - SCALAR_TINY))
  {
    mRenderPosition = body.GetPosition();
    mRenderOrientation = body.GetOrientation();
  }
  else
  {
    mRenderPosition = 
      body.GetPosition() * renderFraction + 
      body.GetOldPosition() * (1.0f - renderFraction);
    // should really use quaternions and SLERP... but maybe if the
    // object is spinning so fast then it doesn't matter if it renders
    // a bit wrong...
    mRenderOrientation = 
      body.GetOrientation() * renderFraction + 
      body.GetOldOrientation() * (1.0f - renderFraction);
  }
}

//==============================================================
// Render
//==============================================================
void tCarObject::Render(tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  
  if (mDisplayListNum == 0)
  {
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    GLCOLOR4(RangedRandom(0.2f, 1.0f), 
             RangedRandom(0.2f, 1.0f),
             RangedRandom(0.2f, 1.0f), 
             0.5f);
    
    tCollisionSkin & boxSkin = mCar.GetChassis().GetSkin();
    for (unsigned iBox = 0 ; iBox < boxSkin.GetNumPrimitives() ; ++iBox)
    {
      const tBox &box = boxSkin.GetPrimitiveLocal(iBox)->GetBox();
      const tVector3 & sides = box.GetSideLengths();
      const tVector3 origin = box.GetPos();
      
      tVector3 BRD = origin + ElementMult(sides, tVector3(0, 0, 0));
      tVector3 BRU = origin + ElementMult(sides, tVector3(0, 0, 1));
      tVector3 BLD = origin + ElementMult(sides, tVector3(0, 1, 0));
      tVector3 BLU = origin + ElementMult(sides, tVector3(0, 1, 1));
      tVector3 FRD = origin + ElementMult(sides, tVector3(1, 0, 0));
      tVector3 FRU = origin + ElementMult(sides, tVector3(1, 0, 1));
      tVector3 FLD = origin + ElementMult(sides, tVector3(1, 1, 0));
      tVector3 FLU = origin + ElementMult(sides, tVector3(1, 1, 1));
      tVector3 fwd = tVector3::Look();
      tVector3 left = tVector3::Left();
      tVector3 up = tVector3::Up();
      tVector3 back = -fwd;
      tVector3 right = -left;
      tVector3 down = -up;
      
      glBegin(GL_QUADS);
      
      GLNORMAL3V(fwd.GetData());
      GLVERTEX3V(FLD.GetData());
      GLVERTEX3V(FLU.GetData());
      GLVERTEX3V(FRU.GetData());
      GLVERTEX3V(FRD.GetData());
      
      GLNORMAL3V(back.GetData());
      GLVERTEX3V(BLU.GetData());
      GLVERTEX3V(BLD.GetData());
      GLVERTEX3V(BRD.GetData());
      GLVERTEX3V(BRU.GetData());
      
      GLNORMAL3V(left.GetData());
      GLVERTEX3V(BLD.GetData());
      GLVERTEX3V(BLU.GetData());
      GLVERTEX3V(FLU.GetData());
      GLVERTEX3V(FLD.GetData());
      
      GLNORMAL3V(right.GetData());
      GLVERTEX3V(BRU.GetData());
      GLVERTEX3V(BRD.GetData());
      GLVERTEX3V(FRD.GetData());
      GLVERTEX3V(FRU.GetData());
      
      GLNORMAL3V(up.GetData());
      GLVERTEX3V(BLU.GetData());
      GLVERTEX3V(BRU.GetData());
      GLVERTEX3V(FRU.GetData());
      GLVERTEX3V(FLU.GetData());
      
      GLNORMAL3V(down.GetData());
      GLVERTEX3V(BRD.GetData());
      GLVERTEX3V(BLD.GetData());
      GLVERTEX3V(FLD.GetData());
      GLVERTEX3V(FRD.GetData());
      
      glEnd();
    }
    glEndList();
    
    // generate a wheel
    mWheelDisplayListNum = glGenLists(1);
    glNewList(mWheelDisplayListNum, GL_COMPILE);
    
    const tWheel & wheel = mCar.GetWheels()[0];
    tScalar radius = wheel.GetRadius();
    tScalar width = radius * 0.5f;
    tScalar width2 = 0.5f * width;
    
    const int numSeg = 16;
    GLCOLOR3(1.0f, 0.0f, 0.0f);
    for (int i = 0 ; i < numSeg ; ++i)
    {
      tScalar ang0 = i * 360.0f / numSeg;
      tScalar ang1 = (i + 1) * 360.0f / numSeg;
      
      if (i == (numSeg / 2))
        GLCOLOR3(0.0f, 0.0f, 1.0f);
      
      glBegin(GL_TRIANGLES);
      
      GLNORMAL3(0.0f, 1.0f, 0.0f);
      GLVERTEX3(0.0f, width2, 0.0f);
      GLVERTEX3(radius * CosDeg(ang1), width2, radius * SinDeg(ang1));
      GLVERTEX3(radius * CosDeg(ang0), width2, radius * SinDeg(ang0));
      
      GLNORMAL3(0.0f, -1.0f, 0.0f);
      GLVERTEX3(0.0f, -width2, 0.0f);
      GLVERTEX3(radius * CosDeg(ang0), -width2, radius * SinDeg(ang0));
      GLVERTEX3(radius * CosDeg(ang1), -width2, radius * SinDeg(ang1));
      
      glEnd();
      
      glBegin(GL_QUADS);
      GLNORMAL3(CosDeg(ang0), 0.0f, SinDeg(ang0));
      GLVERTEX3(radius * CosDeg(ang0), -width2, radius * SinDeg(ang0));
      GLVERTEX3(radius * CosDeg(ang0), width2, radius * SinDeg(ang0));
      GLNORMAL3(CosDeg(ang1), 0.0f, SinDeg(ang1));
      GLVERTEX3(radius * CosDeg(ang1), width2, radius * SinDeg(ang1));
      GLVERTEX3(radius * CosDeg(ang1), -width2, radius * SinDeg(ang1));
      glEnd();
    }
    glEndList();
  }
  
  if (tDebugConfig::mRenderCollisionBoundingBoxes)
    RenderWorldBoundingBox();
  
  ApplyTransformation(mRenderPosition, mRenderOrientation);
  tBody & body = mCar.GetChassis().GetBody();
  bool translucent = !body.IsActive();
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  glCallList(mDisplayListNum);
  
  for (unsigned iWheel = 0 ; iWheel < mCar.GetWheels().Size() ; ++iWheel)
  {
    const tWheel & wheel = mCar.GetWheels()[iWheel];
    tSaveGLMatrixState state;
    
    tVector3 actualPos = wheel.GetPos() + wheel.GetDisplacement() * wheel.GetLocalAxisUp();
    GLTRANSLATE(actualPos[0], actualPos[1], actualPos[2]);
    
    GLROTATE(wheel.GetSteerAngle(), 0.0f, 0.0f, 1.0f);
    GLROTATE(wheel.GetAxisAngle(), 0.0f, 1.0f, 0.0f);
    glCallList(mWheelDisplayListNum);
  }
  
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
    glDisable(GL_BLEND);
}

//==============================================================
// GetRenderBoundingSphere
//==============================================================
const JigLib::tSphere & tCarObject::GetRenderBoundingSphere() const
{
  const tCollisionSkin & boxSkin = mCar.GetChassis().GetSkin();
  const tAABox & box = boxSkin.GetWorldBoundingBox();
  const tVector3 & sides = box.GetSideLengths();
  // ugly...
  static tSphere sphere;
  sphere.SetPos(box.GetCentre());
  sphere.SetRadius(sides.GetLength());
  return sphere;
}

//==============================================================
// GetRenderPosition
//==============================================================
const JigLib::tVector3 & tCarObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const JigLib::tMatrix33 & tCarObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}
