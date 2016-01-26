//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file imageloader.hpp 
//                     
//==============================================================
#ifndef IMAGE_FROM_FILE_HPP
#define IMAGE_FROM_FILE_HPP

#include "jiglib.hpp"

struct tPixel
{
  unsigned char r, g, b, a;
};

/// Works out the image type from the name (last 3 characters).
/// data is resized and populated. Currently support PNG and 
/// uncompressed SGI/RGB format
bool LoadImage(JigLib::tArray2D<tPixel> & data,
               const char * fileName);

#endif
