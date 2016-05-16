#include "defs.h"
#include "image.h"

// image library methods
extern "C"
{
   ImageLibWIN32_API Image * DECL image_instance()
  {
    return new Image();
  }

   ImageLibWIN32_API bool DECL load(char* filename, Image * handle)
  {
    return handle->load(filename);
  }

    ImageLibWIN32_API bool DECL save(char * filename, uint quality, Image * handle)
  {
    return handle->save(filename, quality);
  }

    ImageLibWIN32_API bool DECL init(int w, int h, int c, Image * handle)
  { 
    return handle->init(w, h, c);
  }

    ImageLibWIN32_API int DECL comps(Image * handle)
  {
    return handle->comps();
  }

    ImageLibWIN32_API int DECL width(Image * handle)
  {
    return handle->width();
  }

    ImageLibWIN32_API int DECL height(Image * handle)
  {
    return handle->height();
  }

    ImageLibWIN32_API int DECL size(Image * handle)
  {
    return handle->size();
  }

    ImageLibWIN32_API bool DECL rotate90(Image * handle)
  {
    return handle->rotate90();
  }

    ImageLibWIN32_API void DECL sharpen(int size, Image * handle)
  {
    handle->sharpen(size);
  }

    ImageLibWIN32_API void DECL sobel(Image * handle)
  {
    handle->sobel();
  }

    ImageLibWIN32_API void DECL emboss(int ksize, Image * handle)
  {
    handle->emboss(ksize);
  }

    ImageLibWIN32_API void DECL gaussian_smooth(int ksize, float sigma, Image* handle)
  {
    handle->gauss(ksize, sigma);
  }

    ImageLibWIN32_API void DECL median(int r, Image * handle)
  {
    handle->median(r);
  }

    ImageLibWIN32_API bool DECL texture_data(unsigned char * gldata, int size, Image * handle)
  {
    return handle->get_texture_data(gldata, size);
  }

    ImageLibWIN32_API bool DECL gamma_correct(float gr, float gg, float gb, float max, float scale, float bias, Image * handle)
  {
    return handle->gamma_correct(gr, gg, gb, max, scale, bias);
  }

    ImageLibWIN32_API bool DECL gray_scale(Image * handle)
  {
    return handle->convert_gs();
  }
  
    ImageLibWIN32_API bool DECL threshold(float val, Image* handle)
  {
    return handle->threshold(val);
  }

    ImageLibWIN32_API bool DECL fliph(Image * handle)
  {
    return handle->fliph();
  }

    ImageLibWIN32_API bool DECL flipv(Image * handle)
  {
    return handle->flipv();
  }

    ImageLibWIN32_API bool DECL binning(int b, Image * handle)
  {
    return handle->binning(b);
  }
  
    ImageLibWIN32_API bool DECL nonlocal_means(int r, int sz, Image * handle)
  {
    return handle->nonlocal_means(r, sz);
  }

    ImageLibWIN32_API bool DECL mean_filter(int r, Image * handle)
  {
    return handle->mean_filter(r);
  }

    ImageLibWIN32_API bool DECL usharp_mask(int r, float s, int C, int t, Image * handle)
  {
    return handle->unsharp_mask(r, s, C, t);
  }
  
    ImageLibWIN32_API bool DECL adjust_contrast(int C, Image * handle)
  {
    return handle->adjust_contrast(C);
  }

    ImageLibWIN32_API bool DECL update_from_histo2(int min, int max, Image * handle)
  {
    return handle->update_from_histo(min, max);
  }
    ImageLibWIN32_API bool DECL update_from_histo(int min, int max, float  minr, float  maxr,
			 float  ming, float  maxg, float  minb, float  maxb, Image * handle)
  {
    return handle->update_from_histo( min,  max,   minr,   maxr,   ming,   maxg,   minb,   maxb);
  }
}