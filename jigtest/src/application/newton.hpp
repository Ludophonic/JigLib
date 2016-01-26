//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file newton.hpp 
//                     
//==============================================================
#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "sphereobject.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

#include <vector>

/// Newton's cradle
class tNewton : public tObject
{
public:
  tNewton(class tRenderManager * render,
          unsigned numBalls,
          const JigLib::tVector3 & pos,
          JigLib::tScalar radius);
  ~tNewton();
  
  JigLib::tBody * GetBody();
  void SetRenderPosition(JigLib::tScalar renderFraction);

  void Render(tRenderType renderType) {}
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;
  
private:
  std::vector<tSphereObject *> mBalls;
  std::vector<JigLib::tConstraintWorldPoint *> mConstraints;
};

#endif
