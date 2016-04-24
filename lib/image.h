#ifndef LIB_IMAGE_H
#define LIB_IMAGE_H

#include <cmath>
#include <fftw3.h>

#include "image_jpeg.h"
#include "filter.h"

#define fftshift(in, x, y) fftswap(in, x, y)//circshift(in, x, y, floor(x/2), floor(y/2))
#define fftishift(in, x, y) circshift(in, x, y, ceil(x/2), ceil(y/2))

typedef unsigned long long u64;
typedef unsigned int uint;
typedef unsigned short u16;
typedef unsigned char u8;

enum Interpolation
  {
    nearestneighbor, bilinear, bicubic
  };

template<class ty>
void circshift(ty *in, int xdim, int ydim, int xshift, int yshift)
{
  ty * out = new ty[xdim * ydim];
  for(int i=0; i<xdim; ++i)
  {
    int ii=(i+xshift) % xdim;
    if (ii < 0 ) ii = xdim + ii;
    for (int j=0; j <ydim; ++j)
    {
      int jj = (j+yshift)%ydim;
      if (jj < 0) jj = ydim + jj;
      out[ii * ydim + jj][0] = in[i * ydim + j][0];
      out[ii * ydim + jj][1] = in[i * ydim + j][1];
    }
  }
  memcpy(in, out, sizeof(ty) * xdim * ydim);
  delete [] out; out = 0;
}

// todo : operator overloading on a single pixel (for convenience).
template<typename T>
struct Pixel
{
public:
  T r; T g; T b; T a;
  
  Pixel() : r(0), g(0), b(0) {};
  Pixel(T red, T green, T blue) : r(red), g(green), b(blue) {}
  ~Pixel() {}
  
  void set(T rd, T gn, T bu) { r = rd; g = gn; b = bu; }
  void set(const Pixel& p) { r = p.r; g = p.g; b = p.b; }
  void sqrt() { r = std::sqrt(r); g = std::sqrt(g); b = std::sqrt(b); }
  Pixel operator*(const T& scale);
  Pixel operator+(const Pixel& other);
  Pixel operator*(const Pixel& other);
  Pixel operator-(const Pixel& other);
  Pixel operator+=(const Pixel& other);
  Pixel operator/(const Pixel& other);
  Pixel operator/=(const Pixel& other);
};

// pixel operator overrides
template<typename T>
Pixel<T> Pixel<T>::operator*(const T& scale)
{
  return (Pixel<T>(r*scale, g*scale, b*scale));
}
template<typename T>
Pixel<T> Pixel<T>::operator*(const Pixel& other)
{
  return (Pixel<T>(r * other.r, g * other.g, b * other.b));
}
template<typename T>
Pixel<T> Pixel<T>::operator+(const Pixel& other)
{
  return (Pixel<T>(r + other.r, g + other.g, b + other.b));
}
template<typename T>
Pixel<T> Pixel<T>::operator-(const Pixel& other)
{
  return (Pixel<T>(r - other.r, g - other.g, b - other.b));
}

template<typename T>
Pixel<T> Pixel<T>::operator+=(const Pixel& other)
{
  return (Pixel<T>(r+other.r, g + other.g, b + other.b));
}

template<typename T>
Pixel<T> Pixel<T>::operator/(const Pixel& other)
{
  return (Pixel<T>(r/other.r, g/other.g, b/other.b));
}

template<typename T>
Pixel<T> Pixel<T>::operator/=(const Pixel& other)
{
  return (Pixel<T>(r/other.r, g/other.g, b/other.b));
}

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
  Image(Image& other);
  Image(Pixel<float> ** src, uint w, uint h);
  Image();
  ~Image();

  static Image * get_instance();
  bool load(char * filename);
  bool save(char * filename, uint quality);
  bool parse_type(char * ext);
  bool set(unsigned char * data);
  bool set(double * data);
  
  // size info
  bool init(int w, int h, int c);
  bool clear();
  int comps();
  int width();
  int height();
  int size();

  // pixel access/pointer handles
  Pixel<float> ** img_data() { return data; }
  Filter<float> * img_filter() { return filter; }
  Image_JPEG * get_handle() {return jpeg_handle;}
  Pixel<float>* pixel(int idx) { return data[idx]; }
  void set(int idx, Pixel<float>& p) { data[idx]->set(p.r, p.g, p.b); }
  void set(int idx, float v) { data[idx]->set(v,v,v); }
  void set(int idx, float r, float g, float b) { data[idx]->set(r,g,b); }
  void set_red(int idx, float v) { data[idx]->r = v; }
  void set_green(int idx, float v) { data[idx]->g = v; }
  void set_blue(int idx, float v) { data[idx]->b = v; }
  bool get_texture_data(unsigned char * gldata, int size);
  Pixel<float> * pixel(float x, float y) { return data[(int) (int(y) * _width + int(x))]; }
  
  // utilities/algorithms 
  void clamp(float& r, float& g, float& b, 
	     float scale=1, 
	     float bias=0, 
	     float min=0, 
	     float max=255);
  void clamp(Pixel<float>* p,
	     float scale=1,
	     float bias=0,
	     float min=0,
	     float max=255);
  int wrap(int i, int max);
  bool convolve(float* kernel, int kdim, float scale=1, float bias=0, float min=0, float max=255);
  bool convolve(Pixel<float> ** &result, float* kernel, int kdim, float scale=1, float bias=0, float min=0, float max=255);
  Pixel<float> select_median(Pixel<float> ** pixel_list, int sz);
  bool threshold(float val);
  bool swap_rows();
  bool swap_cols();
  bool swap_cols(double * d, int w, int h);
  bool swap_rows(double * d, int w, int h);
  bool swap_rows2();
  bool swap_cols2();
  void swap(int i1, int i2);
  bool swap_quadrants(); // similar to fftshift in matlab
  bool resize(int w, int h, int c = 3);
  bool on_image(int i);

  // whole image math operations
  bool sub(Image * src);
  
  // size/geometry manipulations
  bool crop();
  bool fliph();
  bool flipv();
  bool subimage(int startx, int endx, int starty, int endy);
  bool rotate90();
  bool binning(int xbin, int ybin);
  bool binning(int b);
  bool xform(float tx, float ty, float centerx, float centery, float deg, float scale);

  // color operations
  bool invert();
  float output_intensity(float I, float G, float max);
  float input_intensity(float I, float G, float max);
  bool gamma_correct(float gr, float gg, float gb, float max, float scale, float bias);
  bool convert_gs();
  template<Interpolation i>
  Pixel<float> * interpolate(float x, float y);
  bool adjust_contrast(float C);
  
  // filter operations
  void sharpen(int ksize);
  void sobel();
  void emboss(int ksize);
  void median(int r);
  void gauss(int ksize, float sigma);
  void gradientX();
  void gradientY();
  void gradientTheta(Pixel<float> ** &result);

  // basic statistical measures
  Pixel<float> mean();
  Pixel<float> stddev(Pixel<float>& mean);

  // denoising algorithms
  bool mean_filter(int r);
  bool nonlocal_means(int r, int sz);

  // unsharp masking (used to sharpen image)
  bool unsharp_mask(int r, float s, int C, int threshold = 25);    
  
  // fftw library support
  bool fft(fftw_complex* R, fftw_complex* G, fftw_complex* B);
  bool ifft(fftw_complex* R, fftw_complex* G, fftw_complex* B);
  bool convolve_fft();
  bool lowpass_filter(double r0, double n);
  bool highpass_filter(double r0, double n);
  void save_mag_image(char * fname, fftw_complex * R, fftw_complex * G, fftw_complex * B, int w, int h);
  double * build_fft_filter(double * filter, int w, int h);
  void fourier_convolve(fftw_complex *R, fftw_complex *G, fftw_complex*B, fftw_complex*C, int w, int h);
  void fourier_convolve(fftw_complex *R, fftw_complex *G, fftw_complex*B, double*C, int w, int h);
  int pow2(int i); // zero padding (not used)
};

#endif
