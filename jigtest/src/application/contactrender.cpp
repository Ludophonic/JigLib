//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file contactrender.cpp 
//                     
//==============================================================
#include "contactrender.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

using namespace JigLib;

//==============================================================
// RenderContact
//==============================================================
void RenderContact(const tVector3 pos,
                   const tVector3 dir,
                   tScalar impulse,
                   tScalar depth,
                   int type,
                   bool special)
{
  if (type == 1)
  {
    static const tScalar delta = 0.3f;
    static const tScalar deltaImpulse = 0.02f;
    if (special)
      GLCOLOR3(1.0f, 1.0f, 0.0f);
    else
      GLCOLOR3(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    GLVERTEX3V((pos - delta * tVector3::Look()).GetData());
    GLVERTEX3V((pos + delta * tVector3::Look()).GetData());
    GLVERTEX3V((pos - delta * tVector3::Up()).GetData());
    GLVERTEX3V((pos + delta * tVector3::Up()).GetData());
    GLVERTEX3V((pos - delta * tVector3::Left()).GetData());
    GLVERTEX3V((pos + delta * tVector3::Left()).GetData());
    glEnd();

    glLineWidth(6.0f);
    glBegin(GL_LINES);
    GLCOLOR3(1.0f, 0.0f, 0.0f);
    GLVERTEX3V(pos.GetData());
    GLVERTEX3V((pos + delta * dir).GetData());
    glEnd();

    glLineWidth(10.0f);
    glBegin(GL_LINES);
    GLCOLOR3(0.0f, 1.0f, 0.0f);
    GLVERTEX3V(pos.GetData());
    GLVERTEX3V((pos + impulse * deltaImpulse * dir).GetData());
    glEnd();
  }
  else if (type == 2)
  {
    tSaveGLMatrixState state;

    if (special)
      GLCOLOR3(1.0f, 1.0f, 0.0f);
    else
      GLCOLOR3(1.0f, 1.0f, 1.0f);

    GLTRANSLATE(pos.x, pos.y, pos.z);
    gluSphere(GetGLUQuadric(), 
              0.02f, 
              12, 12);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    GLCOLOR3(0.0f, 0.0f, 1.0f);
    GLVERTEX3V(( depth * dir).GetData());
    GLVERTEX3V((-depth * dir).GetData());
    glEnd();
  }
}

//==============================================================
// RenderAllContacts
//==============================================================
void RenderAllContacts(int type)
{
  tSaveGLState state;
  glDisable(GL_LIGHTING);

  const std::vector<tCollisionInfo *> & colls = 
    tPhysicsSystem::GetCurrentPhysicsSystem()->GetCollisions();
  unsigned numColls = colls.size();
  
  for (unsigned i = 0 ; i < numColls ; ++i)
  {
    tCollisionInfo * coll = colls[i];
    Assert(coll);
    for (unsigned iPt = 0 ; iPt < coll->mPointInfo.Size() ; ++iPt)
    {
      RenderContact(coll->mPointInfo[iPt].mPosition,
                    coll->mDirToBody0,
                    coll->mPointInfo[iPt].mAccumulatedNormalImpulse,
                    coll->mPointInfo[iPt].mInitialPenetration,
                    type, iPt == 0);
    }
  }
}

