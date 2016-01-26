//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file texture.hpp 
//                     
//==============================================================
#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include "graphics.hpp"

#include <string>


/// Texture read in from file - provides a high and low quality texture,
/// The ability to set the repeat/clamp modes, and the ability to
/// transform the original RGBA image
class tRGBAFileTexture
{
public:
  enum tEdge {CLAMP, CLAMP_TO_EDGE, REPEAT};
  enum tType {RGBA, LUM}; // Use the original image, or convert it?
  tRGBAFileTexture(const std::string & file, 
                   bool useMipmap = true,
                   tEdge edge = REPEAT,
                   tType type = RGBA);
  ~tRGBAFileTexture();
  /// 0 is returned if the texture couldn't be generated
  GLuint GetLowTexture() const {return mTextureLow;}
  GLuint GetHighTexture() const {return mTextureHigh;}
  int getW() const {return mW;}
  int getH() const {return mH;}
private:
  int mW, mH;
  GLuint mTextureLow, mTextureHigh; // two resolutions
};

/// very simple texture manager - provides reference counted file 
/// textures (ignoring the extra parameters)

/// Note that you will be sharing the texture that is returned - do not delete it
tRGBAFileTexture * AllocateRGBAFileTexture(const std::string & file, 
                                           bool useMipmap = true,
                                           tRGBAFileTexture::tEdge edge = tRGBAFileTexture::REPEAT,
                                           tRGBAFileTexture::tType type = tRGBAFileTexture::RGBA);

/// use this to free a texture returned from the above fn
void FreeRGBAFileTexture(tRGBAFileTexture * texture);

#endif
