#include "image.h"

// image library methods
extern "C"
{
  Image * image_instance()
  {
    return new Image();
  }

  bool load(char* filename, Image * handle)
  {
    return handle->load(filename);
  }

  bool save(char * filename, uint quality, Image * handle)
  {
    return handle->save(filename, quality);
  }

  bool init(int w, int h, int c, Image * handle)
  {
    return handle->init(w, h, c);
  }

  int comps(Image * handle)
  {
    return handle->comps();
  }

  int width(Image * handle)
  {
    return handle->width();
  }

  int height(Image * handle)
  {
    return handle->height();
  }

  int size(Image * handle)
  {
    return handle->size();
  }

  bool rotate90(Image * handle)
  {
    return handle->rotate90();
  }

  void sharpen(int size, Image * handle)
  {
    handle->sharpen(size);
  }

  void sobel(Image * handle)
  {
    handle->sobel();
  }

  void emboss(int ksize, Image * handle)
  {
    handle->emboss(ksize);
  }

  void median(int r, Image * handle)
  {
    handle->median(r);
  }

  bool texture_data(unsigned char * gldata, int size, Image * handle)
  {
    return handle->get_texture_data(gldata, size);
  }

  bool gamma_correct(float gr, float gg, float gb, float max, float scale, float bias, Image * handle)
  {
    return handle->gamma_correct(gr, gg, gb, max, scale, bias);
  }

  bool gray_scale(Image * handle)
  {
    return handle->convert_gs();
  }
}
