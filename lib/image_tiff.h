#ifndef LIB_IMAGE_TIFF_H
#define LIB_IMAGE_TIFF_H

#include <stdio.h>
extern "C"
{
#include <tiff.h>
#include <tiffio.h>  
}

#define BIT8 8
#define BIT16 16
#define BIT32 32

class Image_TIFF
{
  int _width;
  int _height;
  int _comps;
  int _size;
  int _depth;
  int _bytes;
  int _order;
  uint8 * gs_data_8;
  uint16* gs_data_16;
  uint32* gs_data_32;
  /*TiffRGB<uint8>  ** data_8;
  TiffRGB<uint16> ** data_16;
  TiffRGB<uint32> ** data_32;
  */
  uint8 * data_8;
  uint16 * data_16;
  uint32 * data_32;
  TIFF * tiff_handle;
  
 public:
  Image_TIFF(int w, int h);
  Image_TIFF();
  ~Image_TIFF();

  /*allocation/deallocation routines*/
  bool init_gs();
  bool clear_gs();
  bool init_rgb();
  bool clear_rgb();

  /*main io methods/wrappers*/
  bool load_tiff(char * filename);
  bool save_tiff(char * filename);
  bool read_gs();
  bool read_rgb();

  /*utility/access information*/
  int width() { return _width; }
  int height() { return _height; }
  int comps() { return _comps; }
  int depth() { return _depth; }
  int bytes() { return _bytes; }
  int size() { return _size; }
};


#endif
