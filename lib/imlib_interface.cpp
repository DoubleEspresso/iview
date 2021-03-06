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

  void gaussian_smooth(int ksize, float sigma, Image* handle)
  {
    handle->gauss(ksize, sigma);
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
  
  bool threshold(float val, Image* handle)
  {
    return handle->threshold(val);
  }

  bool fliph(Image * handle)
  {
    return handle->fliph();
  }

  bool flipv(Image * handle)
  {
    return handle->flipv();
  }

  bool binning(int b, Image * handle)
  {
    return handle->binning(b);
  }
  
  bool nonlocal_means(int r, int sz, Image * handle)
  {
    return handle->nonlocal_means(r, sz);
  }

  bool mean_filter(int r, Image * handle)
  {
    return handle->mean_filter(r);
  }

  bool usharp_mask(int r, float s, int C, int t, Image * handle)
  {
    return handle->unsharp_mask(r, s, C, t);
  }
  
  bool adjust_contrast(int C, Image * handle)
  {
    return handle->adjust_contrast(C);
  }

  bool update_from_histo2(int min, int max, Image * handle)
  {
    return handle->update_from_histo(min, max);
  }
  bool update_from_histo(int min, int max, float  minr, float  maxr,
			 float  ming, float  maxg, float  minb, float  maxb, Image * handle)
  {
    return handle->update_from_histo( min,  max,   minr,   maxr,   ming,   maxg,   minb,   maxb);
  }
}
