#ifndef LIB_IMAGE_JPEG_H
#define LIB_IMAGE_JPEG_H

#include <stdio.h>
extern "C"
{
#include "jinclude.h"
#include "jerror.h"
#include "jpeglib.h"
}
#include <setjmp.h>


typedef unsigned int uint;
struct jpeg_err
{
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;  
};
typedef struct jpeg_err * err_ptr;


class Image_JPEG
{
  int img_width;
  int img_height;
  int img_comps;
  unsigned char * data;
  
 public:
  Image_JPEG(int w, int h);
  Image_JPEG();
  ~Image_JPEG();
  
  bool load_jpeg(char * fname);
  bool save_jpeg(char * fname, uint quality);
  int width() { return img_width; }
  int height() { return img_height; }
  int comps() { return img_comps; }
  int size() { return img_width * img_height * img_comps; }
  void copy_data(unsigned char * indata, int w, int h, int cmps);
  unsigned char * img_data() { return data; } 
};
#endif
