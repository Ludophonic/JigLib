//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file object.cpp 
//                     
//==============================================================
#include "object.hpp"
#include "jiglib.hpp"
#include "graphics.hpp"

using namespace JigLib;

//==============================================================
// tObject
//==============================================================
tObject::tObject()
  :
  tRenderObject(DYNAMIC_OBJECT)
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// ~tObject
//==============================================================
tObject::~tObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// GetCollisionSkin
//==============================================================
tCollisionSkin * tObject::GetCollisionSkin() 
{
  tBody * body = GetBody();
  if (body)
    return body->GetCollisionSkin();
  else return 0;
}

//==============================================================
// RenderWorldBoundingBox
//==============================================================
void tObject::RenderWorldBoundingBox(const tVector3 * col)
{
  TRACE_METHOD_ONLY(FRAME_3);
  tSaveGLState state;
  glDisable(GL_LIGHTING);

  tCollisionSkin * skin = GetCollisionSkin();
  if (0 == skin)
    return;
  
  const tAABox & box = skin->GetWorldBoundingBox();
  
  if (col)
    GLCOLOR3(col->x, col->y, col->z);
  else
    GLCOLOR3(1.0f, 1.0f, 1.0f);
  tVector3 sides = box.GetSideLengths();
  
  tVector3 BRD = box.GetMinPos() + ElementMult(tVector3(0, 0, 0), sides);
  tVector3 BRU = box.GetMinPos() + ElementMult(tVector3(0, 0, 1), sides);
  tVector3 BLD = box.GetMinPos() + ElementMult(tVector3(0, 1, 0), sides);
  tVector3 BLU = box.GetMinPos() + ElementMult(tVector3(0, 1, 1), sides);
  tVector3 FRD = box.GetMinPos() + ElementMult(tVector3(1, 0, 0), sides);
  tVector3 FRU = box.GetMinPos() + ElementMult(tVector3(1, 0, 1), sides);
  tVector3 FLD = box.GetMinPos() + ElementMult(tVector3(1, 1, 0), sides);
  tVector3 FLU = box.GetMinPos() + ElementMult(tVector3(1, 1, 1), sides);
  
  glBegin(GL_LINES);
  
  // back
  GLVERTEX3V(BRD.GetData()); GLVERTEX3V(BLD.GetData());
  GLVERTEX3V(BLD.GetData()); GLVERTEX3V(BLU.GetData());
  GLVERTEX3V(BLU.GetData()); GLVERTEX3V(BRU.GetData());
  GLVERTEX3V(BRU.GetData()); GLVERTEX3V(BRD.GetData());
  
  // front
  GLVERTEX3V(FRD.GetData()); GLVERTEX3V(FLD.GetData());
  GLVERTEX3V(FLD.GetData()); GLVERTEX3V(FLU.GetData());
  GLVERTEX3V(FLU.GetData()); GLVERTEX3V(FRU.GetData());
  GLVERTEX3V(FRU.GetData()); GLVERTEX3V(FRD.GetData());
  
  // other edges
  GLVERTEX3V(BRD.GetData()); GLVERTEX3V(FRD.GetData());
  GLVERTEX3V(BRU.GetData()); GLVERTEX3V(FRU.GetData());
  GLVERTEX3V(BLD.GetData()); GLVERTEX3V(FLD.GetData());
  GLVERTEX3V(BLU.GetData()); GLVERTEX3V(FLU.GetData());
  
  glEnd();

  // do axes
  if (1)
  {
    tBody * body = GetBody();
    if (body)
    {
      ApplyTransformation(body->GetPosition(), body->GetOrientation());
      tScalar delta = box.GetRadiusAboutCentre();
      glBegin(GL_LINES);
      GLCOLOR3(1.0f, 0.0f, 0.0f);
      GLVERTEX3V(tVector3::Zero().GetData());
      GLVERTEX3V((delta * tVector3::Look()).GetData());
      GLCOLOR3(0.0f, 1.0f, 0.0f);
      GLVERTEX3V(tVector3::Zero().GetData());
      GLVERTEX3V((delta * tVector3::Left()).GetData());
      GLCOLOR3(0.0f, 0.0f, 1.0f);
      GLVERTEX3V(tVector3::Zero().GetData());
      GLVERTEX3V((delta * tVector3::Up()).GetData());
      glEnd();
    }
  }
}

//====================================================================
// SetPhysicsPosition
//====================================================================
void tObject::SetPhysicsPosition(const JigLib::tVector3& pos, const JigLib::tMatrix33& orient)
{
  if (GetBody())
    GetBody()->MoveTo(pos, orient);
}

//====================================================================
// SetPhysicsVelocity
//====================================================================
void tObject::SetPhysicsVelocity(const JigLib::tVector3& vel)
{
  if (GetBody())
    GetBody()->SetVelocity(vel);
}
