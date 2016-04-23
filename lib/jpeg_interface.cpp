#include "image_jpeg.h"


// jpeg library routines
extern "C"
{
  Image_JPEG * jpeg_instance()
  {
    return new Image_JPEG();
  }  
  bool jpeg_load(char* filename, Image_JPEG * handle)
  {
    return handle->load_jpeg(filename);
  }
  bool jpeg_save(char * fname, uint quality, Image_JPEG * handle)
  {
    return handle->save_jpeg(fname, quality);
  }
  int jpeg_width(Image_JPEG * handle)
  {
    return handle->width();
  }
  int jpeg_height(Image_JPEG * handle)
  {
    return handle->height();
  }
  int jpeg_comps(Image_JPEG * handle)
  {
    return handle->comps();
  }
  unsigned char* jpeg_data(Image_JPEG * handle)
  {
    return handle->img_data();
  }
  int jpeg_size(Image_JPEG * handle)
  {
    return handle->size();
  }
};
