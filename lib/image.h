#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

#include <fftw3.h>

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
  void set(const Pixel& p) { r = p.r; g = p.g; b = p.b; }
};

struct Gamma
{
  float r; float g; float b;
};

class Image
{
  int _height, _width, _comps, _size;
  Pixel<float>** data;
  Image_JPEG * jpeg_handle;
  Filter<float> * filter;
  Gamma gammas;
 public:
  Image(uint w, uint h);
  Image();
  ~Image();

  static Image * get_instance();
  bool load(char * filename);
  bool save(char * filename, uint quality);
  bool parse_type(char * ext);
  bool set(unsigned char * data);

  // size info
  bool init(int w, int h, int c);
  bool clear();
  int comps();
  int width();
  int height();
  int size();

  // pixel access/pointer handles
  Pixel<float> ** img_data() { return data; }
  Image_JPEG * get_handle() {return jpeg_handle;}
  Pixel<float>* pixel(int idx) { return data[idx]; }
  void set(int idx, Pixel<float>& p) { data[idx]->set(p.r, p.g, p.b); }
  void set_red(int idx, float v) { data[idx]->r = v; }
  void set_green(int idx, float v) { data[idx]->g = v; }
  void set_blue(int idx, float v) { data[idx]->b = v; }
  bool get_texture_data(unsigned char * gldata, int size);
  
  // utilities/algorithms 
  void clamp(float& r, float& g, float& b, float scale=1, float bias=0, float min=0, float max=255);
  int wrap(int i, int max);
  bool convolve(float* kernel, int kdim, float scale=1, float bias=0, float min=0, float max=255);
  bool convolve(Pixel<float> ** &result, float* kernel, int kdim, float scale=1, float bias=0, float min=0, float max=255);
  Pixel<float> select_median(Pixel<float> ** pixel_list, int sz);
  bool threshold(float val);

  // size/geometry manipulations
  bool crop();
  bool fliph();
  bool flipv();
  bool subimage();
  bool rotate90();

  // color operations
  bool invert();
  float output_intensity(float I, float G, float max);
  float input_intensity(float I, float G, float max);
  bool gamma_correct(float gr, float gg, float gb, float max, float scale, float bias);
  bool convert_gs();
  
  // filter operations
  void sharpen(int ksize);
  void sobel();
  void emboss(int ksize);
  void median(int r);
  void gauss(int ksize, float sigma);

  // ffts
  bool forward_fft();
};

#endif
