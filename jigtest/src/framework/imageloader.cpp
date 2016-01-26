/*!
  Sss - a slope soaring simulater.
  Copyright (C) 2003 Danny Chapman - flight@rowlhouse.freeserve.co.uk

  \file image_from_file.h
*/
#include "imageloader.hpp"

#include <stdio.h>
#include <stdlib.h>

#define WITH_PNG

#ifdef WITH_PNG

extern "C" {
#include <png.h>
}

#include <string>

using namespace std;
using namespace JigLib;

bool LoadRGBAImage(tArray2D<tPixel> & data,
                   const char * fileName);
bool LoadPNGImage(tArray2D<tPixel> & data,
                  const char * fileName);


bool LoadImage(JigLib::tArray2D<tPixel> & data,
               const char * fileName)
{
  string file(fileName);
  if ( (file.find(".png") != file.npos) ||
       (file.find(".PNG") != file.npos))
  {
    TRACE_FILE_IF(ONCE_3)
      TRACE("Loading PNG image: %s\n", fileName);
    return LoadPNGImage(data, fileName);
  }
  else if ( (file.find(".rgb") != file.npos) ||
            (file.find(".RGB") != file.npos) ) 
  {
    TRACE_FILE_IF(ONCE_3)
      TRACE("Loading SGI RGB image: %s\n", fileName);
    return LoadRGBAImage(data, fileName);
  }
  TRACE("Invalid image file format: %s\n", fileName);
  data.Resize(0, 0);
  return false;
}

//======================== RGB stuff ===========================
inline unsigned short int getshort(FILE * inf)
{
  unsigned char buf[2];
  fread(buf,2,1,inf);
  return (unsigned short int) (buf[0]<<8)+(buf[1]<<0);
}

inline unsigned char getrgbchar(FILE * inf)
{
  unsigned char buf;
  fread(&buf,1,1,inf);
  return buf;
}


//==============================================================
// LoadRGBAImage
//==============================================================
bool LoadRGBAImage(tArray2D<tPixel> & data,
                   const char * fileName)
{
  data.Resize(0, 0);

  unsigned char *temp;
  FILE *image_in;
  unsigned char input_char;
  unsigned short int input_short;
  unsigned char header[512];
  unsigned long int loop;
  int width, height;
  
  if ( (image_in = fopen(fileName, "rb")) == NULL) 
    return false;

  input_short=getshort(image_in);
  if (input_short == 0x01da)
  {
    input_char=getrgbchar(image_in);
    if (input_char == 0)
    {
      input_char=getrgbchar(image_in);
      input_short=getshort(image_in);
      if (input_short == 3)
      {
        input_short=getshort(image_in);
        width=input_short;
        input_short=getshort(image_in);
        height=input_short;
        // if it looks like this isn't a trivial texture, display a
        // warning - loading the large textures can be slow.
        static bool done_warning = false;
        if ( (done_warning == false) && ( (width > 256) || (height > 256) ) )
        {
          TRACE_FILE_IF(ONCE_2)
            TRACE("[Large texture - may take a few seconds]");
          done_warning = true;
        }

        // how many bytes per pixel
        input_short=getshort(image_in);
        int bytes_per_pixel = input_short;
        
        if ( (bytes_per_pixel == 3) ||
             (bytes_per_pixel == 4) )
        {
          data.Resize(width, height);
          temp  = new unsigned char[width * height];

          fread(header,sizeof(unsigned char),500,image_in);
          fread(temp, sizeof(unsigned char), width * height, image_in);
          for (loop=0;loop<(unsigned long int)(width * height);loop++)
          {
            int i = loop % width;
            int j = loop - (i * width);
            data(0, j).r = temp[loop];
          }
          fread(temp, sizeof(unsigned char), width * height, image_in);
          for (loop=0;loop<(unsigned long int)(width * height);loop++)
          {
            int i = loop % width;
            int j = loop - (i * width);
            data(0, j).g = temp[loop];
          }
          fread(temp, sizeof(unsigned char), width * height, image_in);
          for (loop=0;loop<(unsigned long int)(width * height);loop++)
          {
            int i = loop % width;
            int j = loop - (i * width);
            data(0, j).b = temp[loop];
          }
          // alpha?
          if (bytes_per_pixel == 4)
          {
            fread(temp, sizeof(unsigned char), width * height, image_in);
            for (loop=0;loop<(unsigned long int)(width * height);loop++)
            {
              int i = loop % width;
              int j = loop - (i * width);
              data(0, j).a = temp[loop];
            }
          }
          else
          {
            for (loop=0;loop<(unsigned long int)(width * height);loop++)
            {
              int i = loop % width;
              int j = loop - (i * width);
              data(0, j).a = temp[loop];
            }
          }
          delete [] temp;
          fclose(image_in);
          return true;  
        }
        else
        {
          TRACE("This file isn't a 3 or 4 channel RGB file.\n");
          fclose(image_in);
          return false;
        }
      }    
      else
      {
        TRACE("Not a useable RGB file.\n");
        fclose(image_in);
        return false;
      }
    }
    else
    {
      TRACE("RLE encoded SGI files are not supported.\n");
      fclose(image_in);
      return false;
    }
  }
  else
  {
    TRACE("File %s doesn't appear to be an SGI rgb file!\n",fileName);
    fclose(image_in);
    return false;
  }
}

//======================== PNG stuff ===========================
bool LoadPNGImage(tArray2D<tPixel> & image,
                  const char * fileName)
{
  image.Resize(0, 0);

  FILE *png_file;
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 width32, height32, x, y;
  int bit_depth, color_type, interlace_type;
  int compression_type, filter_method;
  unsigned char *data;
  unsigned char *row_pointer;
  unsigned int has_alpha;
  int width, height;
  
  png_file = fopen(fileName, "rb");
  if (!png_file) 
  {
    TRACE("Error opening PNG file %s\n", fileName);
    return false;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, NULL, NULL);
  if (!png_ptr) 
  {
    TRACE("Error reading PNG data from file %s\n", fileName);
    fclose(png_file);
    return false;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) 
  {
    TRACE("Error reading PNG info from file %s\n", fileName);
    fclose(png_file);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return false;
  }

  png_init_io(png_ptr, png_file);

  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &width32, &height32,
               &bit_depth, &color_type, &interlace_type,
               &compression_type, &filter_method);
  width = width32;
  height = height32;

  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_MASK_ALPHA) 
  {
    TRACE("Has alpha\n");
    has_alpha = 1;
  }
  else 
  {
    TRACE("Has no alpha\n");
    has_alpha = 0;
  }

  data = new unsigned char[width * height * (3 + has_alpha)];

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  if (bit_depth == 16)
    png_set_strip_16(png_ptr);

  if (bit_depth < 8)
    png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  row_pointer = data;
  for (y = 0; y < (png_uint_32) height; y++) 
  {
    png_read_row(png_ptr, row_pointer, NULL);
    row_pointer += width * (3 + has_alpha);
  }
  fclose(png_file);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  // now copy the image into something that definitely has alpha, and
  // flip it as well.
  image.Resize(width, height);
  for (y = 0 ; y < (png_uint_32) height ; ++y)
  {
    for (x = 0 ; x < (png_uint_32) width ; ++x)
    {
      image(x, y).r = data[(x + (height - y - 1)*width)*(3+has_alpha) + 0];
      image(x, y).g = data[(x + (height - y - 1)*width)*(3+has_alpha) + 1];
      image(x, y).b = data[(x + (height - y - 1)*width)*(3+has_alpha) + 2];
      if (has_alpha == 1)
        image(x, y).a = data[(x + (height - y - 1)*width)*(3+has_alpha) + 3];
      else
        image(x, y).a = 255;
    }
  }
  
  delete [] data;
  
  return true;
}

#else

bool LoadImage(JigLib::tArray2D<tPixel> & data,
               const char * fileName)
{
  data.Resize(32, 32);
  return true;
}


#endif
