#ifndef LIB_FILTER2_H
#define LIB_FILTER2_H

#include <math.h>
#define M_PI 3.14159265358979323846264338327950288

enum Filter2Type
  { 
    Identity, Sharpen, Sobel2,
    Emboss, Median, Gauss,
    DX, DY, LoG, Bilateral
  };

struct FilterParams
{
  float mean; 
  float sigma; 
};


namespace {

//specific math functions for filters
float gauss2(int x, int y, float s)
{    
  float r2 = x*x + y*y;
  return 1/(2*M_PI * s*s) * exp(-r2 / (2 * s * s));
}

template<typename T>
T * build_identity(int dim)
{ return 0; }

template<typename T>
T * build_sharpen(int dim)
{ return 0; }

template<typename T>
T * build_sobel(int dim)
{ return 0; }

template<typename T>
T * build_emboss(int dim)
{ return 0; }

template<typename T>
T * build_median(int dim)
{ return 0; }

template<typename T>
T * build_gauss(int dim, FilterParams* params)
{ 
  T * kernel = new T[dim*dim];
  float sum = 0; float s = 1.0; 
  if (params) s = params->sigma;
  for(int j=0, idx=0; j < dim; ++j) 
  {
  for (int i=0; i<dim; ++i, ++idx)
    {
      int row = -dim/2 + j; int col = -dim/2 + i;
      float v = gauss2(col, row, s);
      kernel[idx] = v;
      sum += v;
    }
  }
  for (int j=0; j<dim*dim; ++j) kernel[j] /= sum;
  return kernel; 
}

template<typename T>
T * build_dx(int dim)
{ return 0; }

template<typename T>
T * build_dy(int dim)
{ return 0; }

template<typename T>
T * build_log(int dim)
{ return 0; }

template<typename T>
T * build_bilateral(int dim)
{ return 0; }

template<typename T, Filter2Type type>
  T * filter(int dim, FilterParams * params = 0)
{
T * kernel;
switch(type)
  {
 case Identity:  kernel = build_identity<T>(dim); break;
 case Sharpen:   kernel = build_sharpen<T>(dim); break;
 case Sobel2:    kernel = build_sobel<T>(dim); break;
 case Emboss:    kernel = build_emboss<T>(dim); break;
 case Median:    kernel = build_median<T>(dim); break;
 case Gauss:     kernel = build_gauss<T>(dim, params); break;
 case DX:        kernel = build_dx<T>(dim); break;
 case DY:        kernel = build_dy<T>(dim); break;
 case LoG:       kernel = build_log<T>(dim); break;
 case Bilateral: kernel = build_bilateral<T>(dim); break;
  }
return kernel;
}  

}; // end namespace

#endif
