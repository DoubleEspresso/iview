#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

#include "image_jpeg.h"
#include "filter.h"

typedef unsigned long long u64;
typedef unsigned int uint;
typedef unsigned short u16;
typedef unsigned char u8;

template<typename T>
struct Pixel
{
public:
  T r; T g; T b; T a;
  
  Pixel() {};
  Pixel(T red, T green, T blue) : r(red), g(green), b(blue) {}
  ~Pixel() {}
  
  void set(T rd, T gn, T bu) { r = rd; g = gn; b = bu; }
};


class Image
{
  int _height, _width, _comps, _size;
  Pixel<float>** data;
  Image_JPEG * jpeg_handle;
  Filter<float> * filter;
 public:
  Image(uint w, uint h);
  Image();
  ~Image();

  static Image * get_instance();
  bool load(char * filename);
  bool save(char * filename, uint quality);
  bool parse_type(char * ext);
  bool set(unsigned char * data);

  // utility
  bool init(int w, int h, int c);
  bool clear();
  int comps();
  int width();
  int height();
  int size();
  Pixel<float> ** img_data() { return data; }
  void clamp(float& r, float& g, float& b, float scale, float bias);
  Image_JPEG * get_handle() {return jpeg_handle;}
  void set(int idx, Pixel<float>& p) { data[idx]->set(p.r, p.g, p.b); }
  void set_red(int idx, float v) { data[idx]->r = v; }
  void set_green(int idx, float v) { data[idx]->g = v; }
  void set_blue(int idx, float v) { data[idx]->b = v; }
  Pixel<float>* get(int idx) { return data[idx]; }
  int wrap(int i, int max);

  // size/geometry manipulations
  bool crop();
  bool fliph();
  bool flipv();
  bool subimage();
  bool rotate90();

  // color operations
  bool invert();  

  // filters etc.
  void sharpen(int kernel_size);

  // ffts
};

#endif
