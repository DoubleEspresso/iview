#include <stdio.h>
#include <ctype.h>

#include "image.h"
#include "stringutils.h"

Image::Image()
{
}

Image::Image(uint w, uint h) : height(h), width(w), im_type(NONE), data(0)
{ 
}

Image::~Image() {}

bool Image::load(char * filename, void*& iptr)
{
  //StringUtils su;
  //char * ext = su.get_file_extension(filename);
  //if (ext == 0) return false;
  // parse image type
  //if (!parse_type(ext)) return false;
  //im_type = JPEG;
  //if (im_type == JPEG)
  //{
  Image_JPEG * im = new Image_JPEG();
  iptr = im;
  return im->load_jpeg(filename);
}

bool Image::save(char * filename, uint quality)
{
  Image_JPEG * im = new Image_JPEG();
  return im->save_jpeg(filename, quality);
}


bool Image::parse_type(char * ext)
{

  if (ext == 0) return false;
  int i=0;
  while( ext[i])
    {
      ext[i] = tolower(ext[i]); ++i;
    }
  
  if (i == 4 && ext[0] == 'j' && ext[1] == 'p' && ext[2] == 'e' && ext[3] == 'g') im_type = JPEG;
  else if (i == 3 && ext[0] == 'j' && ext[1] == 'p' && ext[2] == 'g') im_type = JPEG;
  else return false;

  return true;
}
