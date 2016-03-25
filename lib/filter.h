#ifndef LIB_FILTER_H
#define LIB_FILTER_H


enum FilterType
  {
    None,
    Sharpen3x3, Sharpen5x5, Sharpen7x7,
    Sobel,
    Emboss3x3 , Emboss5x5 , Emboss7x7,
    Median3x3 , Median5x5 , Median7x7
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
};


#endif
