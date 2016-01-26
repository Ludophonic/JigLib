//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file skybox.hpp 
//                     
//==============================================================
#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "renderobject.hpp"
#include <string>

class tSkybox : public tRenderObject
{
public:
  /// name is used as the base for the textures
  tSkybox(const std::string & name);
  ~tSkybox();

  void Render(tRenderType renderType);
  
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  
  const JigLib::tVector3 & GetRenderPosition() const;
  
  const JigLib::tMatrix33 & GetRenderOrientation() const;

private:
  void FreeAllTextures();

  class tRGBAFileTexture * mTextures[6];
};

#endif
