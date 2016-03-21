#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

#include "image_jpeg.h"

typedef unsigned int uint;

namespace
{
  enum ImageType { NONE, JPEG, TIFF, PNG };
  enum Rotation { R90, R180, R270 };
}

class Image
{
  uint height;
  uint width;
  ImageType im_type;
  unsigned char* data;

 public:
  Image(uint w, uint h);
  Image();
  ~Image();

  static Image * get_instance();
  bool load(char * filename, void*& iptr);
  bool save(char * filename, uint quality);
  bool parse_type(char * ext);

  // image operations
  bool crop();
  bool translate();
  bool rotate(Image_JPEG& im, const int r );

  // filters etc.

  // ffts
};

#endif
