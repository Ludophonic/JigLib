//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file spherechain.hpp 
//                     
//==============================================================
#ifndef SPHERECHAIN_HPP
#define SPHERECHAIN_HPP

#include "object.hpp"

#include "jiglib.hpp"

#include <vector>

class tSphereChain : public tObject
{
public:
  enum tChainType {CHAIN_HINGE, CHAIN_BALLNSOCKET};
  
  tSphereChain(class tRenderManager * render,
               unsigned chainLength,
               const JigLib::tVector3 & pos,
               JigLib::tScalar radius,
               JigLib::tScalar boxDensity,
               tChainType type);
  
  ~tSphereChain();

  JigLib::tBody * GetBody();
  void SetRenderPosition(JigLib::tScalar renderFraction);

  void Render(tRenderType renderType) {}
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;

private:
  std::vector<class tSphereObject *> mSpheres;
  std::vector<class JigLib::tHingeJoint *> mHinges;
  std::vector<class JigLib::tConstraint *> mConstraints;
};

#endif
