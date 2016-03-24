#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

#include "image_jpeg.h"

typedef unsigned long long u64;
typedef unsigned int uint;
typedef unsigned short u16;
typedef unsigned char u8;


/*
template<typename T>
class Image2
{
  T * data;
  uint width, height, stride;

 public:
  Image2(uint w, uint h);
  Image2(Image2<T>& src);
  Image2(T * indata);
  Image2();
  ~Image2();

  // overloads
  Image2<T>& operator=(const Image2<T>& other) const;
  Image2<T>& operator=(const T * indata) const;

  bool load(const char * filename);
  bool save(const char * filename);

  bool multiply(T s);
  bool add(const Image2<T>& other);
  bool sub(const Image2<T>& other);
};
*/

class Image
{
  int _height, _width, _comps, _size;
  unsigned char* data;
  Image_JPEG * jpeg_handle;

 public:
  Image(uint w, uint h);
  Image();
  ~Image();

  static Image * get_instance();
  bool load(char * filename);
  bool save(char * filename, uint quality);
  bool parse_type(char * ext);
  bool set(unsigned char * data);

  Image_JPEG * get_handle() {return jpeg_handle;} 
  int comps();
  int width();
  int height();
  int size();

  // image operations
  bool crop();
  bool fliph();
  bool flipv();
  bool subimage();
  bool rotate90();

  bool clear();
  bool invert(); // color  

  char convolve3(const char * image, float * kernel);
  char convolve5();
  char convolve7();

  // filters etc.
  int wrap(int stride, int row, int col, int drow, int dcol);
  void sharpen();

  // ffts
};

#endif
