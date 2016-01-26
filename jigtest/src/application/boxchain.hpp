//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file boxchain.hpp 
//                     
//==============================================================
#ifndef BOXCHAIN_HPP
#define BOXCHAIN_HPP

#include "object.hpp"

#include "jiglib.hpp"

#include <vector>

class tBoxChain : public tObject
{
public:
  enum tChainType {CHAIN_HINGE, CHAIN_BALLNSOCKET};
  
  tBoxChain(class tRenderManager * render,
            unsigned chainLength,
            const JigLib::tVector3 & pos,
            const JigLib::tVector3 & boxDims,
            JigLib::tScalar boxDensity,
            tChainType type);
  
  ~tBoxChain();

  JigLib::tBody * GetBody();
  void SetRenderPosition(JigLib::tScalar renderFraction);

  void Render(tRenderType renderType) {}
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;

private:
  std::vector<class tBoxObject *> mBoxes;
  std::vector<class JigLib::tHingeJoint *> mHinges;
  std::vector<class JigLib::tConstraint *> mConstraints;
};

#endif
