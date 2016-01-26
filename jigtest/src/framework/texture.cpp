/*!
  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk

  \file texture.cpp

  \todo improve this texture implementation!
*/
#include "texture.hpp"
#include "imageloader.hpp"

#include "jiglib.hpp"

#include <list>
using namespace std;
using namespace JigLib;

//==============================================================
// tRGBAFileTexture
//==============================================================
tRGBAFileTexture::tRGBAFileTexture(const string & file, 
                                   bool useMipmap,
                                   tRGBAFileTexture::tEdge edge,
                                   tRGBAFileTexture::tType type)
{
  TRACE_FILE_IF(ONCE_2)
    TRACE("Reading %s ", file.c_str());
  JigLib::tArray2D<tPixel> image(0, 0);
  if (false == LoadImage(image, file.c_str()))
  {
    TRACE("Error reading %s\n", file.c_str());
    mTextureLow = 0;
    mTextureHigh = 0;
    mW = mH = 0;
    return;
  }

  mW = image.GetNx();
  mH = image.GetNy();

  TRACE_FILE_IF(ONCE_2)
    TRACE("%d %d\n", mW, mH);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  GLint glEdge = GL_CLAMP;
  GLenum glType = GL_RGBA;

  switch (edge)
  {
  case CLAMP: glEdge = GL_CLAMP; break;
  case CLAMP_TO_EDGE: 
//    glEdge = GL_CLAMP; break;
    glEdge = GL_CLAMP_TO_EDGE; break;
  case REPEAT: glEdge = GL_REPEAT; break;
  };


  GLubyte * data = 0;

  switch (type)
  {
  case RGBA: 
  {
    data = new GLubyte[mW*mH*4];
    for (int i = 0 ; i < mW ; ++i)
    {
      for (int j = 0 ; j < mH ; ++j)
      {
        data[4 * (i + j * mW) + 0] = image(i, j).r;
        data[4 * (i + j * mW) + 1] = image(i, j).g;
        data[4 * (i + j * mW) + 2] = image(i, j).b;
        data[4 * (i + j * mW) + 3] = image(i, j).a;
      }
    }
    glType = GL_RGBA;
    break;
  }
  case LUM:
  {
    // detail is just a "light map" - use the intensity of the original image
    data = new GLubyte[mW*mH];
    for (int i = 0 ; i < mW ; ++i)
    {
      for (int j = 0 ; j < mH ; ++j)
      {
        data[i + j * mW] = 
          (GLubyte) ((image(i, j).r + image(i, j).g + image(i, j).b) / 3.0f);
      }
    }
    glType = GL_LUMINANCE;
    break;
  }
  default:
    TRACE("Unhandled texture modifier type: %d\n", type);
    mW = mH = 0;
    return;
  }

  glGenTextures(1,&mTextureLow);
  glBindTexture(GL_TEXTURE_2D, mTextureLow);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glEdge);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glEdge);
  if (useMipmap)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D,glType,mW,mH,glType,
                      GL_UNSIGNED_BYTE, data);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, glType, mW, mH, 0, glType, 
                 GL_UNSIGNED_BYTE, data);
  }

  glGenTextures(1,&mTextureHigh);
  glBindTexture(GL_TEXTURE_2D, mTextureHigh);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glEdge);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glEdge);

  if (useMipmap)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D,glType,mW,mH,glType,
                      GL_UNSIGNED_BYTE, data);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, glType, mW, mH, 0, glType,
                 GL_UNSIGNED_BYTE, data);
  }

  delete [] data;
}

//==============================================================
// tRGBAFileTexture
//==============================================================
tRGBAFileTexture::~tRGBAFileTexture()
{
  glDeleteTextures(1, &mTextureLow);
  glDeleteTextures(1, &mTextureHigh);
}

//==============================================================
// tTextureRecord
//==============================================================
struct tTextureRecord
{
  string file;
  bool useMipmap;
  tRGBAFileTexture::tEdge edge;
  tRGBAFileTexture::tType type;
  tRGBAFileTexture * texture;
  int refCount;
};

// speed isn't an issue - anyway we don't know what key we'll be doing
// a lookup on
typedef list<tTextureRecord> tTextureRecords;
tTextureRecords textureRecords;

//==============================================================
// allocate_rgba_file_texture
//==============================================================
tRGBAFileTexture * AllocateRGBAFileTexture(const std::string & file, 
                                           bool useMipmap,
                                           tRGBAFileTexture::tEdge edge,
                                           tRGBAFileTexture::tType type)
{
  // walk through the list
  tTextureRecords::iterator it;
  for (it = textureRecords.begin() ; it != textureRecords.end() ; ++it)
  {
    if ( (it->file == file) &&
         (it->useMipmap == useMipmap) &&
         (it->edge == edge) &&
         (it->type == type) )
    {
      it->refCount += 1;
      return it->texture;
    }
  }

  // got to the end - need to add a new one
  tTextureRecord newRecord;
  newRecord.file = file;
  newRecord.useMipmap = useMipmap;
  newRecord.edge = edge;
  newRecord.type = type;
  newRecord.refCount = 1;
  newRecord.texture = new tRGBAFileTexture(file, useMipmap, edge, type);
  textureRecords.push_back(newRecord);
  return newRecord.texture;
}

//==============================================================
// FreeRGBAFileTexture
//==============================================================
void FreeRGBAFileTexture(tRGBAFileTexture * texture)
{
  if (texture == 0)
    return;
  // walk through the list
  tTextureRecords::iterator it;
  for (it = textureRecords.begin() ; it != textureRecords.end() ; ++it)
  {
    if ( it->texture == texture )
    {
      it->refCount -= 1;
      if (it->refCount == 0)
      {
        textureRecords.erase(it);
      }
      return;
    }
  }
  TRACE("ERROR: unable to find/free texture %p\n", texture);
}


