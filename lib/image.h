#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

typedef unsigned int uint;

namespace
{
  enum ImageType { NONE, JPEG, TIFF, PNG };
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
  bool rotate();

  // filters etc.

  // ffts
};

#endif
