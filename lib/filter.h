#ifndef LIB_FILTER_H
#define LIB_FILTER_H

#include <math.h>
#include "filter2.h"

enum FilterType
  {
    None,
    Sharpen3x3, Sharpen5x5, Sharpen7x7,
    Sobel,
    Emboss3x3 , Emboss5x5 , Emboss7x7,
    Median3x3 , Median5x5 , Median7x7,
    Gauss3x3  , Gauss5x5  , Gauss7x7,
    DX3x3, DY3x3
  };

template<typename T>
class Filter
{
  T * kernel;
  int _size;
  FilterType type;
public:
  
Filter(FilterType t) : kernel(0), _size(0), type(t)
  {
    switch (t)
      {
      case None: _size = 0; break;
      case Sharpen3x3: _size = 3; break;
      case Sharpen5x5: _size = 5; break;
      case Sharpen7x7: _size = 7; break;
      case Sobel : _size = 3; break;
      case Emboss3x3: _size = 3; break;
      case Emboss5x5: _size = 5; break;
      case Emboss7x7: _size = 7; break;
      case Median3x3: _size = 3; break;
      case Median5x5: _size = 5; break;
      case Median7x7: _size = 7; break;
      case Gauss3x3: _size = 3; break;
      case Gauss5x5: _size = 5; break;
      case Gauss7x7: _size = 7; break;
      case DX3x3: _size = 3; break;
      case DY3x3: _size = 3; break;
      default: break;	
      }
    init();
  }
  
  ~Filter()
    {
      if (kernel)
	{
	  delete [] kernel; kernel = 0;
	}
    }
 
  bool init()
  {
    if (!_size) return false;
    return create_kernel();
  }

  bool create_kernel()
  {
    switch (type)
      {
      case None : break;
      case Sharpen3x3: set_sharpen3x3(); break;
      case Sharpen5x5: set_sharpen5x5(); break;
      case Sharpen7x7: set_sharpen7x7(); break;
      case Sobel: break; // handled in image.cpp (2 part filter operation)
      case Emboss3x3: set_emboss3x3(); break;
      case Emboss5x5: set_emboss5x5(); break;
      case Emboss7x7: set_emboss7x7(); break;
      case Median3x3: set_median3x3(); break;
      case Median5x5: set_median5x5(); break;
      case Median7x7: set_median7x7(); break;
      case DX3x3: set_dx3x3(); break;
      case DY3x3: set_dy3x3(); break;
      }
    return true;
  }
  
  int dim(){ return _size; }

  /***** filter definitions *****/

  /** sharpen filters **/
  void set_sharpen3x3()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = 0; kernel[1] = -1; kernel[2] = 0;
    kernel[3] = -1; kernel[4] = 5; kernel[5] = -1;
    kernel[6] = 0; kernel[7] = -1; kernel[8] = 0;
  }  
  void set_sharpen5x5() {}
  void set_sharpen7x7() {}

  /** sobel filter **/
  void set_sobelX()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = -1; kernel[1] = 0; kernel[2] = 1;
    kernel[3] = -2; kernel[4] = 0; kernel[5] = 2;
    kernel[6] = -1; kernel[7] = 0; kernel[8] = 1;
  }
  void set_sobelY()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = -1; kernel[1] = -2; kernel[2] = -1;
    kernel[3] = 0; kernel[4] = 0; kernel[5] = 0;
    kernel[6] = 1; kernel[7] = 2; kernel[8] = 1;
  }

  /** sobel filter **/
  void set_emboss3x3()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = -1; kernel[1] = -1; kernel[2] = 0;
    kernel[3] = -1; kernel[4] = 0; kernel[5] = 1;
    kernel[6] = 0; kernel[7] = 1; kernel[8] = 1;
  }
  void set_emboss5x5() {}
  void set_emboss7x7() {}

  /** median filter **/
  void set_median3x3() {}
  void set_median5x5() {}
  void set_median7x7() {}

  /** gaussian smoothing **/
  void set_gauss3x3(float s) 
  {
        if (!kernel) kernel = new T[9];
	kernel[0] = gauss(1, 1, s); kernel[1] = gauss(0,1,s); kernel[2] = gauss(1,1,s);
	kernel[3] = gauss(1,0,s);   kernel[4] = gauss(0,0,s); kernel[5] = kernel[3];
	kernel[6] = kernel[0];       kernel[7] = kernel[1];     kernel[8] = kernel[0];
	float sum = 0;
	for (int j=0; j<9; ++j )
	  {
	    sum += kernel[j];
	  }
	for (int j=0; j<9; ++j )
	  {
	    kernel[j] /= sum;
	  }
  }
  void set_gauss5x5(float s) 
  {
    if (kernel) { delete [] kernel; kernel = 0; }
    kernel = new T[25];
    kernel[0] = gauss(2, 2, s); kernel[1] = gauss(1,2,s); kernel[2] = gauss(0,2,s); kernel[3] = gauss(1,2,s); kernel[4] = gauss(2,2,s);
    kernel[5] = gauss(2, 1, s); kernel[6] = gauss(1,1,s); kernel[7] = gauss(0,1,s); kernel[8] = gauss(1,1,s); kernel[9] = gauss(2,1,s);
    kernel[10] = gauss(2, 0, s); kernel[11] = gauss(1,0,s); kernel[12] = gauss(0,0,s); kernel[13] = gauss(1,0,s); kernel[14] = gauss(2,0,s);
    kernel[15] = gauss(2, 1, s); kernel[16] = gauss(1,1,s); kernel[17] = gauss(0,1,s); kernel[18] = gauss(1,1,s); kernel[19] = gauss(2,1,s);
    kernel[20] = gauss(2, 2, s); kernel[21] = gauss(1,2,s); kernel[22] = gauss(0,2,s); kernel[23] = gauss(1,2,s); kernel[24] = gauss(2,2,s);

    float sum = 0;
    for (int j=0; j<25; ++j )
      {
	sum += kernel[j];
      }
    for (int j=0; j<25; ++j )
      {
	kernel[j] /= sum;
      }
  }
  void set_gauss7x7(float s) 
  {
    if (kernel) { delete [] kernel; kernel = 0; }
    kernel = new T[49];
    float sum = 0;
    for (int j=0, idx =0; j <7; ++j)
      {
	for (int i=0; i<7; ++i, ++idx)
	  {
	    int row = -7/2 + j;
	    int col = -7/2 + i;
	    float v = gauss(col, row, s);
	    kernel[j * 7 + i] = v;
	    sum += v;
	  }
      }
    for (int j=0; j<49; ++j )
      {
	kernel[j] /= sum;
      }
  }
  
  void set_dy3x3()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = -1; kernel[1] = -1; kernel[2] = -1;
    kernel[3] = 0; kernel[4] = 0; kernel[5] = 0;
    kernel[6] = 1; kernel[7] = 1; kernel[8] = 1;
  }
  
  void set_dx3x3()
  {
    if (!kernel) kernel = new T[9];
    kernel[0] = -1; kernel[1] = 0; kernel[2] = 1;
    kernel[3] = -1; kernel[4] = 0; kernel[5] = 1;
    kernel[6] = -1; kernel[7] = 0; kernel[8] = 1;
  }

  // utility methods
  T * get_kernel() { return kernel; }
  bool set_kernel(T * src, int size)
  {
    if (size <= 0) return false;
    if (kernel)
      {
	delete[] kernel; kernel = 0;
      }

    kernel = new T[size*size];
    _size = size;
    
    for (int j=0; j<_size * _size; ++j)
      {
	kernel[j] = src[j];
      }
    return true;
  }
  float gauss(int x, int y, float s)
  {    
    float r2 = x*x + y*y;
    return 1/(2*M_PI * s*s) * exp(-r2 / (2 * s * s));
  }

  // 8th order approximation to 2d gaussian
  float gauss8(int x, int y, float s)
  {    
    float r2 = x*x + y*y;
    float r4 = r2 * r2;
    float r6 = r4 * r2;
    float r8 = r4 * r4;
    float s2 = s*s; float s4 = s2 * s2; float s6 = s4 * s2; float s8 = s4*s4; float s10 = s6 * s4;
    return 1/(2*M_PI *s2) - 
      r2/(4 * M_PI * s4) + 
      r4/(16 * M_PI * s6) - 
      r6/(96*M_PI * s8) + 
      r8 / (768 * M_PI * s10);
  }

};


#endif
