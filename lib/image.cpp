#include <stdio.h>
#include <ctype.h>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <fstream>

#include "image.h"
#include "stringutils.h"


Image::Image() :
  _height(0), _width(0), _comps(0), 
  _size(0), data(0), jpeg_handle(0), filter(0), data_copy(0)
{
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
}

Image::Image(uint w, uint h) : 
  _height(h), _width(w), _comps(3), 
  _size(w*h), data(0), jpeg_handle(0), filter(0), data_copy(0)
{
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
  init(w,h,3);
}

Image::Image(Pixel<float> ** src, uint w, uint h) :
  _height(h), _width(w), _comps(3), 
  _size(w*h), data(0), jpeg_handle(0), filter(0), data_copy(0)
{
  if (!jpeg_handle) jpeg_handle = new Image_JPEG();
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
  data = new Pixel<float>*[_size];
  unsigned char * jpeg_d = new unsigned char [_size * _comps];
  for (int j=0,i=0; j<_size; ++j,i+=3)
    { 
      float r = src[j]->r; jpeg_d[i] = r;
      float g = src[j]->g; jpeg_d[i+1] = g;    
      float b = src[j]->b; jpeg_d[i+2] = b;
      data[j] = new Pixel<float>(r,g,b);      
    }
  jpeg_handle->copy_data(jpeg_d, w, h, _comps);
  delete [] jpeg_d; jpeg_d = 0;  
}

Image::Image(Image& other) :
  _height(other.height()), _width(other.width()), _comps(other.comps()),
  _size(other.size()), data(0), jpeg_handle(other.get_handle()), filter(other.img_filter()),
  data_copy(0)
{
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
  data = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    { 
      float r = other.pixel(j)->r;
      float g = other.pixel(j)->g;
      float b = other.pixel(j)->b;
      data[j] = new Pixel<float>(r,g,b);
    }
  
}

Image::~Image() 
{
  if (filter)
    {
      delete filter; filter = 0;
    }
  if (jpeg_handle) { delete jpeg_handle; jpeg_handle = 0; }
  if (data)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete data[j]; data[j] = 0;
	}
      delete[] data; data = 0; 
    }
  _size = _width = _height = _comps = 0;
}

bool Image::clear()
{
  if (data)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete data[j]; data[j] = 0;
	}
      delete[] data; data = 0;
    }
  //if (jpeg_handle) { delete jpeg_handle; jpeg_handle = 0; }
  return true;
}

bool Image::init(int w, int h, int c)
{
  if (_size <= 0 ) return false;
  int oldsz = _size;
  if (data) clear();
  _width = w; _height = h; _comps = c; _size = w * h;

  if (data)
    {
      for(int j=0; j<oldsz; ++j)
	{
	  delete data[j]; data[j] =0;	  
	}
      delete [] data; data = 0;
    }

  // TODO: check if mem available
  data = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      data[j] = new Pixel<float>(0,0,0);
    }
  return true;
}

bool Image::load(char * filename)
{
  //StringUtils su;
  //char * ext = su.get_file_extension(filename);
  //if (ext == 0) return false;
  // parse image type
  //if (!parse_type(ext)) return false;
  //im_type = JPEG;
  //if (im_type == JPEG)
  //{
  clear();
  if (!jpeg_handle) jpeg_handle = new Image_JPEG();
  bool ok = jpeg_handle->load_jpeg(filename);
  _width = jpeg_handle->width();
  _height = jpeg_handle->height();
  _comps = jpeg_handle->comps();
  _size = _width * _height; //jpeg_handle->size();
  init(_width, _height, _comps);

  // copy jpeg data to member pixel array
  unsigned char * tmp = jpeg_handle->img_data();
  for (int j=0, idx=0; j<_size; ++j, idx+=3)
    {
      data[j]->set(tmp[idx], tmp[idx+1], tmp[idx+2]);
    }
  return ok;
}

int Image::comps() { return _comps; }
int Image::width()  { return _width; }
int Image::height() { return _height;}
int Image::size() { return _size; }

bool Image::get_texture_data(unsigned char * gldata, int size)
{
  if (!gldata) return false;
  if (size != _size * _comps) return false;
  for (int j=0, i =0; j<_size; ++j, i+=comps())
    {
      gldata[i] = data[j]->r;
      gldata[i+1] = data[j]->g;
      gldata[i+2] = data[j]->b;
    }
  return true;
}

bool Image::save(char * filename, uint quality)
{
  if(!jpeg_handle) jpeg_handle = new Image_JPEG();//return false;
  unsigned char * tmp = new unsigned char[_width * _height * _comps];
  for (int j=0, idx=0; j < _width * _height; ++j, idx += 3)
    {
      tmp[idx] = data[j]->r; tmp[idx+1] = data[j]->g; tmp[idx+2] = data[j]->b;
    }
  jpeg_handle->copy_data(tmp, _width, _height, _comps);
  { delete [] tmp; tmp = 0; }
  return jpeg_handle->save_jpeg(filename, quality);
}


bool Image::parse_type(char * ext)
{
  return false;
  /*
  if (ext == 0) return false;
  int i=0;
  while( ext[i])
    {
      ext[i] = tolower(ext[i]); ++i;
    }
  
  if (i == 4 && ext[0] == 'j' && ext[1] == 'p' && ext[2] == 'e' && ext[3] == 'g') im_type = JPEG;
  else if (i == 3 && ext[0] == 'j' && ext[1] == 'p' && ext[2] == 'g') im_type = JPEG;
  else return false;

  return true;
  */
}


void Image::sharpen(int ksize)
{
  if (filter)
    {
      delete filter; filter = 0;
    }

  switch(ksize)
    {
    case 3: filter = new Filter<float>(Sharpen3x3); break;
    case 5: filter = new Filter<float>(Sharpen5x5); break;
    case 7: filter = new Filter<float>(Sharpen7x7); break;
    }  
  convolve(filter->get_kernel(), filter->dim());  
  if (filter) { delete filter; filter = 0; }
}

void Image::sobel()
{
  
    if (filter)
    {
      delete filter; filter = 0;
    }
      
    Pixel<float> ** resultX;
    Pixel<float> ** resultY;
    
    filter = new Filter<float>(Sobel);
    filter->set_sobelX();
    convolve(resultX, filter->get_kernel(), filter->dim());
    filter->set_sobelY();
    convolve(resultY, filter->get_kernel(), filter->dim());


    for (int j=0; j<_size; ++j)
      {
	float r1 = resultX[j]->r; float g1 = resultX[j]->g; float b1 = resultX[j]->b;
	float r2 = resultY[j]->r; float g2 = resultY[j]->g; float b2 = resultY[j]->b;
	float r3 = sqrt(r1*r1 + r2*r2); float g3 = sqrt(g1*g1 + g2*g2); float b3 = sqrt(b1*b1+b2*b2);

	clamp(r3,g3,b3);
	data[j]->set(r3,g3,b3);
      }

    // cleanup
    if (resultX)
      {
	for (int j=0; j<_size; ++j)
	  {
	    delete resultX[j]; resultX[j] = 0;
	  }
	delete[] resultX; resultX = 0;
      }

    if (resultY)
      {
	for (int j=0; j<_size; ++j)
	  {
	    delete resultY[j]; resultY[j] = 0;
	  }
	delete[] resultY; resultY = 0;
      }
    if (filter) { delete filter; filter = 0; }
}

void Image::emboss(int ksize)
{
  if (filter)
    {
      delete filter; filter = 0;
    }
  
  switch(ksize)
    {
    case 3: filter = new Filter<float>(Emboss3x3); break;
    case 5: filter = new Filter<float>(Emboss5x5); break;
    case 7: filter = new Filter<float>(Emboss7x7); break;
    }  
  convolve(filter->get_kernel(), filter->dim(), 1.0, 128.0);  
  if (filter) { delete filter; filter = 0; }
}

void Image::median(int r)
{
  Pixel<float> ** result;
  Pixel<float> ** plist;
  result = new Pixel<float>*[_size];
  plist = new Pixel<float>* [r*r];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }

  for (int j=0; j<r*r; ++j)
    {
      plist[j] = new Pixel<float>(0,0,0);
    }
  
    for (int j = 0, ci = 0; j < _height; ++j)
    {
      for (int i=0; i < _width; ++i, ++ci)
	{

	  for (int kh = 0, ki = 0; kh < r; ++kh)
	    {
	      for (int kw = 0; kw < r; ++kw, ++ki)		
		{
		  int dx = -r/2 + kw; int dy = -r/2 + kh;
		  int idx  = wrap(j+dy, _height) * _width + wrap(i+dx, _width);
		  plist[ki]->set(*data[idx]);
		}
	    }	  
	  result[ci]->set(select_median(plist, r*r));
	}
    }

    // overwrite the current image data .. necessary?
    for (int j=0; j<_size; ++j)
      {
	float r = result[j]->r; float g = result[j]->g; float b = result[j]->b;
	data[j]->set(r,g,b);
      }

    //cleanup
    if (result)
      {
	for (int j=0; j<_size; ++j)
	  {
	    if (result[j])
	      {
		delete result[j]; result[j] = 0;
	      }
	  }
	if (result)
	  {
	    delete[] result; result = 0;
	  }
      }
    if (plist)
      {
	for (int j=0; j<r*r; ++j)
	  {
	    if (plist[j])
	      {
		delete plist[j]; plist[j] = 0;
	      }
	  }
	if (plist)
	  {
	    delete[] plist; plist = 0;
	  }
      }
}

void Image::gauss(int ksize, float sigma)
{
  switch(ksize)
    {
    case 3: filter = new Filter<float>(Gauss3x3); 
      filter->set_gauss3x3(sigma); break;
    case 5: filter = new Filter<float>(Gauss5x5); 
      filter->set_gauss5x5(sigma); break;
    case 7: filter = new Filter<float>(Gauss7x7); 
      filter->set_gauss7x7(sigma); break;
    }  
  convolve(filter->get_kernel(), filter->dim());  
  if (filter) { delete filter; filter = 0; }
}


void Image::gradientX()
{
  if (filter)
    {
      delete filter; filter = 0;
    }
  filter = new Filter<float>(DX3x3);
  convolve(filter->get_kernel(), filter->dim());
  if (filter) { delete filter; filter = 0; }
}

void Image::gradientY()
{
  if (filter)
    {
      delete filter; filter = 0;
    }
  filter = new Filter<float>(DY3x3);
  convolve(filter->get_kernel(), filter->dim());
  if (filter) { delete filter; filter = 0; }
}

void Image::gradientTheta(Pixel<float> ** &result)
{
  if (filter)
    {
      delete filter; filter = 0;
    }

  // convert image to grayscale first ??
  // TODO: should really move data to a grayscale array
  // no need to store all 3 rgb channels anymore
  Image * gs = new Image(*this);
  gs->convert_gs();

  Pixel<float> ** resultX;
  Pixel<float> ** resultY;

  filter = new Filter<float>(Sobel);

  filter->set_sobelX();
  gs->convolve(resultX, filter->get_kernel(), filter->dim(), 1, 0, 0, 255);

  filter->set_sobelY();
  gs->convolve(resultY, filter->get_kernel(), filter->dim(), 1, 0, 0, 255);

  for (int j=0; j<_size; ++j)
    {
      float r1 = resultX[j]->r; float g1 = resultX[j]->g; float b1 = resultX[j]->b;
      float r2 = resultY[j]->r; float g2 = resultY[j]->g; float b2 = resultY[j]->b;
      float r3 = sqrt(r1*r1 + r2*r2); float g3 = sqrt(g1*g1 + g2*g2); float b3 = sqrt(b1*b1+b2*b2);
      
      clamp(r3,g3,b3);
      gs->set(j,r3,g3,b3);
    }

  result = new Pixel<float>*[_size]; // caller frees this (?)
  double eps = 1e-6;
  for (int j=0; j<_size; ++j)
    {      
      double tr = atan(resultY[j]->r / (resultX[j]->r+eps));
      //double tg = atan(resultY[j]->g / (resultX[j]->g+eps));
      //double tb = atan(resultY[j]->b / (resultX[j]->b+eps));

      // no clamping! nb: gs data only has one unique component
      result[j] = new Pixel<float>(tr, tr, tr); 
    }    

  gs->save("/home/mjg/Desktop/test-gs.jpg",100);
  // first stab at non-maximal supression on edges
  /*
  float r = 1;
  for (int y=0, idx = 0; y < _height; ++y)
    {
      for (int x=0; x<_width; ++x, ++idx)
	{
	  float theta = result[idx]->r; // radians
	  float nx = x + r * cos(theta); float ny = y + r * sin(theta);
	  if (nx >= 0 && nx < _width - 1 && ny >= 0 && ny < _height - 1)
	    {
	      
	    }
	}
    }
  */
  // cleanup
  if (resultX)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete resultX[j]; resultX[j] = 0;
	}
      delete[] resultX; resultX = 0;
    }
  
  if (resultY)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete resultY[j]; resultY[j] = 0;
	}
      delete[] resultY; resultY = 0;
    }
  if (gs) gs->clear();
}

Pixel<float> Image::mean()
{
  Pixel<float> m(0,0,0);
  float rm = 0; float gm = 0; float bm = 0;
  for (int j=0; j<_size; ++j)
    {
      rm += data[j]->r; gm += data[j]->g; bm += data[j]->b;
    }
  m.set(rm / _size, gm / _size, bm / _size);
  return m;
}

Pixel<float> Image::stddev(Pixel<float>& mean)
{
  Pixel<float> s(0,0,0);
  float sr = 0; float sg = 0; float sb = 0;
  for (int j=0; j<_size; ++j)
    {
      sr += (data[j]->r - mean.r) * (data[j]->r - mean.r);
      sg += (data[j]->g - mean.g) * (data[j]->g - mean.g);
      sb += (data[j]->b - mean.b) * (data[j]->b - mean.b);
    }
  s.set(sr/_size, sg/_size, sb/_size); s.sqrt();
  return s;
}

bool Image::mean_filter(int r)
{
  if (r > 7) r = 7;
  else if (r < 0) r = 3;
  else if (r!=3 || r!=5 || r !=7) r = 3;
  
  switch(r)
    {
    case 3: filter = new Filter<float>(Mean3x3); break;
    case 5: filter = new Filter<float>(Mean5x5); break;
    case 7: filter = new Filter<float>(Mean7x7); break;
    }
  convolve(filter->get_kernel(), filter->dim());
  if (filter) { delete filter; filter = 0; }
  return true;
}

bool Image::nonlocal_means(int r, int sz)
{
  if (r > 7) r = 7;
  else if (r < 0) r = 3;
  else if (r!=3 || r!=5 || r !=7) r = 3;

  Pixel<float> img_mean(0,0,0); Pixel<float> sigma(0,0,0);
  img_mean = mean(); sigma = stddev(img_mean);
  float r2 = sigma.r * sigma.r;
  float g2 = sigma.g * sigma.g;
  float b2 = sigma.b * sigma.b;
  // make a local mean image
  Image * local_mean = new Image(*this);
  switch(r)
    {
    case 3: filter = new Filter<float>(Mean3x3); break;
    case 5: filter = new Filter<float>(Mean5x5); break;
    case 7: filter = new Filter<float>(Mean7x7); break;
    }
  local_mean->convolve(filter->get_kernel(), filter->dim());
  
  // non-local means
  for (int y = 0, idx=0; y < _height; ++y)
    {
      for (int x=0; x<_width; ++x, ++idx)
	{
	  Pixel<float> Bp = (*local_mean->pixel(idx));
	  Pixel<float> sum(0,0,0);
	  Pixel<float> val(0,0,0);
	  
	  // patch centered at pixel idx and size
	  // sz is used (instead of entire image)
	  for (int ry=std::max(0, y-sz/2); ry<std::min(y+sz/2, _height); ++ry)
	    {
	      for (int rx=std::max(0, x-sz/2); rx < std::min(x+sz/2, _width); ++rx)
		{
		  int j= ry * _width + rx;
		  Pixel<float> Bq = (*local_mean->pixel(j));
		  float r = exp(-(Bp.r - Bq.r)*(Bp.r - Bq.r)) / r2;
		  float g = exp(-(Bp.g - Bq.g)*(Bp.g - Bq.g)) / g2;
		  float b = exp(-(Bp.b - Bq.b)*(Bp.b - Bq.b)) / b2;
		  sum.r += r; sum.g += g; sum.b += b;
		  val.r += r * Bq.r; val.g += g * Bq.g; val.b += b * Bq.b;
		}
	    }
	  sum.r = (sum.r <= 0 ? 1 : sum.r);
	  sum.g = (sum.g <= 0 ? 1 : sum.g);
	  sum.b = (sum.b <= 0 ? 1 : sum.b);
	  val.r = val.r/sum.r; val.g = val.g / sum.g; val.b = val.b/sum.b;
	  
	  clamp(&val);
      //printf("%f %f %f (%f,%f,%f)\n", val.r, val.g, val.b, 
      //sum.r, sum.g, sum.b);
	  
	  data[idx]->set(val);
	}  
    }

  // cleanup
  if (filter) { delete filter; filter = 0; }  
  if (local_mean) local_mean->clear();
  
  return true;
}

bool Image::unsharp_mask(int r, float s, int C, int t)
{
  // build gaussian blur image
  if (t <= 0) t = 25;
  
  Image * blurred = new Image(*this);
  Image * umask = new Image(*this);
  Image * contrast = new Image(*this);
 
  filter = new Filter<float>();
  filter->set_gauss(r, s);
  blurred->convolve(filter->get_kernel(), filter->dim());
  umask->sub(blurred);
  contrast->adjust_contrast(C); // higher constrast image

  Image * gscale = new Image(*umask);
  gscale->convert_gs();
  float max = -1; float min = 256;
  for (int j=0; j<_size; ++j)
    {
      if (gscale->pixel(j)->r > max) max = gscale->pixel(j)->r;
      if (gscale->pixel(j)->r < min) min = gscale->pixel(j)->r;
    }
  float range = max - min;
  if (range <= 0) range = 1;
  
  for(int j=0; j<_size; ++j)
    {
      Pixel<float> src = *data[j];
      Pixel<float> hc = (*contrast->pixel(j));
      Pixel<float> cd(hc.r-src.r, hc.g-src.g, hc.b-src.b);
      float percent = gscale->pixel(j)->r / range;
      float delta = (0.299 * cd.r + 0.587 * cd.g + 0.114 * cd.b + 0.5) * percent; // gray-value

      if (abs(delta) >= t)
	{
	  src.r += delta; src.g += delta; src.b += delta;
	  clamp(&src);
	  data[j]->set(src);
	}
    }

  // cleanup
  if (filter) { delete filter; filter = 0; }  
  if (blurred) blurred->clear();
  if (umask) umask->clear();
  if (contrast) contrast->clear();
  if (gscale) gscale->clear();
  return true;
}

// TODO: improve ... or remove
bool Image::fft_sharpen(double r0, double n, double A, double B)
{
  // note: 0.25 < A < 0.5 (typical)
  // b > 1
  Image * hp = new Image(*this);
  hp->highpass_filter(30, 10); // butterworth high pass filter
  hp->mul(B);
  hp->adjust_contrast(-255);
  add(hp);
  //mul(0.25);

  /*
  for (int j=0; j<_size; ++j)
    {
      data[j]->set(*hp->pixel(j));
    }
  */
  hp->clear();
  return true;
}

// spatial convolution definitions
bool Image::convolve(Pixel<float> ** &result, float* kernel, int kdim,
		     float scale, float bias,
		     float min, float max)
{
  result = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }
  int r = kdim;
  for (int j = 0, ci = 0; j < _height; ++j)
    {
      for (int i=0; i < _width; ++i, ++ci)
	{
	  float red = 0;
	  float grn = 0;
	  float blu = 0;
	  
	  // convolution over kernel
	  for (int kh = 0, ki = 0; kh < r; ++kh)
	    {
	      for (int kw = 0; kw < r; ++kw, ++ki)		
		{
		  // wrapped central image idx
		  int dx = -r/2 + kw; int dy = -r/2 + kh;
		  int idx  = wrap(j+dy, _height) * _width + wrap(i+dx, _width);		  
		  red += data[idx]->r * kernel[ki];
		  grn += data[idx]->g * kernel[ki];
		  blu += data[idx]->b * kernel[ki];		  
		}
	    }
	  clamp(red, grn, blu, scale, bias, min, max);

	  result[ci]->set(red, grn, blu);

	}
    }
  //if (filter) { delete filter; filter = 0; }
  return result;
}

bool Image::convolve(float * kernel, int kdim,
		     float scale, float bias,
		     float min, float max)
{
  // storage for new image data
  Pixel<float> ** result;
  result = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }
  int r = kdim;

  for (int j = 0, ci = 0; j < _height; ++j)
    {
      for (int i=0; i < _width; ++i, ++ci)
	{
	  float red = 0;
	  float grn = 0;
	  float blu = 0;
	  
	  // convolution over kernel
	  for (int kh = 0, ki = 0; kh < r; ++kh)
	    {
	      for (int kw = 0; kw < r; ++kw, ++ki)		
		{
		  // wrapped central image idx
		  int dx = -r/2 + kw; int dy = -r/2 + kh;
		  int idx  = wrap(j+dy, _height) * _width + wrap(i+dx, _width);		  
		  red += data[idx]->r * kernel[ki];
		  grn += data[idx]->g * kernel[ki];
		  blu += data[idx]->b * kernel[ki];		  
		}
	    }
	  clamp(red, grn, blu, scale, bias, min, max);
	  result[ci]->set(red, grn, blu);
	}
    }

  // overwrite the current image data .. necessary?
  for (int j=0; j<_size; ++j)
    {
      float r = result[j]->r; float g = result[j]->g; float b = result[j]->b;
      data[j]->set(r,g,b);
    }
  
  if (result)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete result[j]; result[j] = 0;
	}
      delete[] result; result = 0;
    }
  //if (filter) { delete filter; filter = 0; }
  return true;
}


/* image operations section */
bool Image::rotate90()
{
  if (!data || width() <= 1 || height() <= 1) return false;

  // temp storage for rated image
  Pixel<float> ** result;
  result = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }
  
  for (int j = _width-1, idx = 0; j >= 0; --j)
    {
      for (int i=0; i < _height; ++i)
	{
	  int isrc = i * _width + j;
	  result[idx++]->set(*data[isrc]);
	}
    }

  // overwrite the current image data .. necessary?
  for (int j=0; j<_size; ++j)
    {
      data[j]->set(*result[j]);
    }
  
  if (result)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete result[j]; result[j] = 0;
	}
      delete[] result; result = 0;
    }
  int w = width();
  _width = height(); 
  _height = w;
  return true;  
}

bool Image::binning(int b)
{
  if (b <= 1) return true;

  // storage for binned image
  Pixel<float> ** result;
  int w = _width / b; 
  int h = _height / b; 
  int bh = (int)floor((float)b/2);
  if (w <= 1 || h <= 1 ) return true;
  int sz = w * h;

  result = new Pixel<float>*[sz];
  for (int j=0; j<sz; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }

  for (int y = bh, idx = 0; y < _height-bh; y += b)
    {
      for (int x = bh; x <_width-bh; x += b, ++idx)
	{	  

	  double pr = 0; double pg = 0; double pb = 0; float count = 0;
	  for (int ry = std::max(0, y - bh); ry < std::min(y + bh, _height); ++ry)
	    {
	      for (int rx = std::max(0, x - bh); rx < std::min(x + bh, _width); ++rx)
		{
		  int i = ry * _width + rx;
		  pr += data[i]->r; pg += data[i]->g; pb += data[i]->b; ++count;
		}
	    }
	  if(count > 0) 
	    { 
	      pr /= count; pg /= count; pb /= count;
	      result[idx]->set(pr, pg, pb);
	    }

	}
    }

  resize(w, h); // updates width/height/and size
  for (int j=0; j<_size; ++j)
    {
      data[j]->set(*result[j]);
    }
  
  if (result)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete result[j]; result[j] = 0;
	}
      delete[] result; result = 0;
    }

  return true;
}

// inplace xform inside current image bounds
bool Image::xform(float tx, float ty, float centerx, float centery, float deg, float scale)
{
  // storage for new image
  Pixel<float> ** result;
  result = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }
  
  double ct = cos(M_PI / 180.0 * deg);
  double st = sin(M_PI / 180.0 * deg);

  // x' = x cos - y sin 
  // y' = x sin + y cos
  if (scale <=0 ) scale = 100.0;
  double sc = 1.0/(scale / 100.0);
  for (int y = 0, idx = 0; y < _height; ++y)
    {
      for (int x = 0; x < _width; ++x, ++idx)
	{
	  float cx = (x - centerx - tx); float cy = (y - centery - ty);
	  float nx = centerx + sc * (cx * ct - cy * st);
	  float ny = centery + sc * (cx * st + cy * ct);
	  
	  // bilinear interpolation
	  if ( nx >= 0 && nx < _width-1 && ny >= 0 && ny < _height-1 )
	    {	      
	      result[idx]->set( *interpolate<bilinear>(nx, ny));
	    }
	}
    }

  // copy and cleanup
  for (int j=0; j<_size; ++j)
    {
      data[j]->set(*result[j]);
    }
  
  if (result)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete result[j]; result[j] = 0;
	}
      delete[] result; result = 0;
    }

  return true;
}

/*note: resize will clear all data, allocate a new array of zeros*/
bool Image::resize(int w, int h, int c)
{
  if (data)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete data[j]; data[j] = 0;
	}
      delete[] data; data = 0;
    }
  init(w, h, c);
  return true;
}

bool Image::on_image(int i)
{
  return (i >= 0 && i < _size);
}

void Image::bounds(float & minr, float & maxr, float & ming, float & maxg, float & minb, float & maxb)
{
  minr = ming = minb = 256;
  maxr = maxg = maxb = 0;
  for (int j=0; j<_size; ++j)
    {
      float r = data[j]->r; float g = data[j]->g; float b= data[j]->b;
      if (r < minr) minr = r; if (g < ming) ming = g; if (b < minb) minb = b;
      if (r > maxr) maxr = r; if (g > maxg) maxg = g; if (b > maxb) maxb = b;
    }
}

void Image::copy_data()
{
  if (data_copy == NULL)
    {
      data_copy = new Pixel<float>*[_size];
      for (int j=0; j <_size; ++j)
	{
	  data_copy[j] = new Pixel<float>(0,0,0);
	  data_copy[j]->set(*data[j]);
	}
    }
}

bool Image::fliph()
{
  if (!data || _width <= 1 || _height <= 1) return false;  
  for (int x = 0, idx = _width-1; x < _width / 2; ++x, --idx)
    {
      for (int y=0 ; y < _height; ++y)
	{
	  int il = y * _width + x;
	  int ir = y * _width + idx;
	  swap(il, ir);
	}
    }
  return true;
}

bool Image::mul(double s)
{  
  for (int j = 0; j<_size; ++j)
    {
      float r = s * pixel(j)->r;
      float g = s * pixel(j)->g;
      float b = s * pixel(j)->b;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
  return true;
}

bool Image::add(double d)
{  
  for (int j = 0; j<_size; ++j)
    {
      float r = d + pixel(j)->r;
      float g = d + pixel(j)->g;
      float b = d + pixel(j)->b;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
  return true;
}

bool Image::add(Image * src)
{
  if (src->size() != _size) return false;
  
  for (int j = 0; j<_size; ++j)
    {
      float r = data[j]->r + src->pixel(j)->r;
      float g = data[j]->g + src->pixel(j)->g;
      float b = data[j]->b + src->pixel(j)->b;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
  return true;
}

bool Image::sub(Image * src)
{
  if (src->size() != _size) return false;
  
  for (int j = 0; j<_size; ++j)
    {
      float r = data[j]->r - src->pixel(j)->r;
      float g = data[j]->g - src->pixel(j)->g;
      float b = data[j]->b - src->pixel(j)->b;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
  return true;
}

bool Image::flipv()
{
  if (!data || _width <= 1 || _height <= 1) return false;  
  for (int y = 0, idx = _height-1; y < _height / 2; ++y, --idx)
    {
      for (int x=0 ; x<_width; ++x)
	{
	  int iup = y * _width + x;
	  int ido = idx * _width + x;
	  swap(iup, ido);
	}
    }
  return true;
}

/* color operations section */
float Image::input_intensity(float I, float G, float max)
{
  return (float) (max * pow(I / max, 1/G)); 
}
float Image::output_intensity(float I, float G, float max)
{
  return (float) ( max * pow( I / max, G ) );
}

bool Image::gamma_correct(float gr, float gg, float gb, float max, float scale, float bias)
{
  for (int j=0; j < _size; ++j)
    {
      
      float r = output_intensity(input_intensity(data[j]->r, gammas.r, max), gr, max);
      float g = output_intensity(input_intensity(data[j]->g, gammas.g, max), gg, max);
      float b = output_intensity(input_intensity(data[j]->b, gammas.b, max), gb, max);
      clamp(r, g, b, scale, bias);
      data[j]->r = r; data[j]->g = g; data[j]->b = b;
    }
  gammas.r = gr; gammas.g = gg; gammas.b = gb;
  return true;
}

bool Image::convert_gs()
{
  for (int j=0; j<_size; ++j)
    {
      float r = data[j]->r; float g = data[j]->g; float b = data[j]->b;
      float nc = (0.299 * r + 0.587 * g + 0.114 * b + 0.5);//sqrt(r*r + g*g + b*b);
      clamp(nc, nc, nc);
      data[j]->set(nc, nc, nc);
    }
  return true;
}

template<Interpolation i>
Pixel<float> * Image::interpolate(float x, float y)
{
  Pixel<float> * result;
  switch (i)
    {
    case nearestneighbor: break;
    case bilinear: 
      {	
	float nx = x - int(x); float ny = y - int(y);
	result = new Pixel<float>(0,0,0);
	result->set((*pixel(x,y))     * (1-nx) * (1-ny) + 
		    (*pixel(x+1,y))   * nx     * (1-ny) + 
		    (*pixel(x,y+1))   * (1-nx) * ny +
		    (*pixel(x+1,y+1)) * nx     * ny);            
	clamp(result);
	break;
      }
    case bicubic: break;
    }
  return result;
}

bool Image::adjust_contrast(float C)
{
  // note: assumes 8bit images!!
  float F = 259.0 * (C + 255.0) / (255.0 * (259.0 - C));
  for (int j=0; j<_size; ++j)
    {
      float nr = data[j]->r * F;
      float ng = data[j]->g * F;
      float nb = data[j]->b * F;
      clamp(nr, ng, nb);
      data[j]->set(nr, ng, nb);
    }
	return true;		   
}

// for now, just the red components are updated
bool Image::update_from_histo(int min, int max)
{
  float sz = 4095;
  float minr = 0; float ming = 0; float minb = 0;
  float maxr = 0; float maxg = 0; float maxb = 0;
  bounds(minr, maxr, ming, maxg, minb, maxb);
  
  float minr2 = (float) (((float) min / sz) * (maxr - minr) + minr);
  float maxr2 = (float) (((float) max / sz) * (maxr - minr) + minr);
  float ming2 = (float) (((float) min / sz) * (maxg - ming) + ming);
  float maxg2 = (float) (((float) max / sz) * (maxg - ming) + ming);
  float minb2 = (float) (((float) min / sz) * (maxb - minb) + minb);
  float maxb2 = (float) (((float) max / sz) * (maxb - minb) + minb);
  float scr = (maxr - minr) / 255.0;
  float scg = (maxg - ming) / 255.0;
  float scb = (maxb - minb) / 255.0;      

  for (int j=0; j<_size; ++j)
    {
      float r = (data[j]->r - minr2) / scr;
      float g = (data[j]->g - ming2) / scg;
      float b = (data[j]->b - minb2) / scb;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
	return true;
}

bool Image::update_from_histo(int min, int max, float  minr, float  maxr, float  ming, float  maxg, float  minb, float  maxb)
{
  copy_data();
  float sz = 4095;
  float minr2 = (float) (((float) min / sz) * (maxr - minr) + minr);
  float maxr2 = (float) (((float) max / sz) * (maxr - minr) + minr);
  float ming2 = (float) (((float) min / sz) * (maxg - ming) + ming);
  float maxg2 = (float) (((float) max / sz) * (maxg - ming) + ming);
  float minb2 = (float) (((float) min / sz) * (maxb - minb) + minb);
  float maxb2 = (float) (((float) max / sz) * (maxb - minb) + minb);
  float scr = (maxr2 - minr2) / 255.0;
  float scg = (maxg2 - ming2) / 255.0;
  float scb = (maxb2 - minb2) / 255.0;      

  for (int j=0; j<_size; ++j)
    {
      float r = (data_copy[j]->r - minr2) / scr;
      float g = (data_copy[j]->g - ming2) / scg;
      float b = (data_copy[j]->b - minb2) / scb;
      clamp(r,g,b);
      data[j]->set(r,g,b);
    }
	return true;
}

/* utilities section */
void Image::clamp(float& r,
		  float& g,
		  float& b,
		  float scale,
		  float bias,
		  float min,
		  float max)
{
  r = r * scale + bias; r = (r < min ? min : r) ; r = (r > max ? max : r);
  g = g * scale + bias; g = (g < min ? min : g) ; g = (g > max ? max : g);
  b = b * scale + bias; b = (b < min ? min : b) ; b = (b > max ? max : b);
}

void Image::clamp(Pixel<float>* p,
		  float scale,
		  float bias,
		  float min,
		  float max)
{
  float r = p->r; float g = p->g; float b = p->b;
  r = r * scale + bias; r = (r < min ? min : r) ; r = (r > max ? max : r);
  g = g * scale + bias; g = (g < min ? min : g) ; g = (g > max ? max : g);
  b = b * scale + bias; b = (b < min ? min : b) ; b = (b > max ? max : b);
  p->r = r; p->g = g; p->b = b;
}

int Image::wrap(int i, int max)
{
  i = i%max;
  return (i < 0 ? i + max : i);
}

Pixel<float> Image::select_median(Pixel<float> ** pixel_list, int sz)
{
  float *reds=new float[sz]; float *grns=new float[sz]; float * blus = new float[sz];
  for (int j=0; j<sz; ++j)
    {
      reds[j] = pixel_list[j]->r; grns[j] = pixel_list[j]->g; blus[j] = pixel_list[j]->b;
    }
  // sort using a lambda expression (ascending vs descending won't matter for median)
  std::sort(reds, reds+sz);
  std::sort(grns, grns+sz);
  std::sort(blus, blus+sz);
  int idx = sz/2;
  Pixel<float> median;
  median.set(reds[idx], grns[idx], blus[idx]);
  {
    delete [] reds; delete[] grns; delete[] blus;
    reds = 0; grns = 0; blus = 0;
  }
  return median;
}

// TODO: update/replace
bool Image::threshold(float val)
{
  median(3);
  median(3);
  median(3);
  sharpen(3);
  convert_gs();
  for (int j=0; j<_size; ++j)
    {
      data[j]->r = (data[j]->r <= val ? 0 : 255);
      data[j]->g = (data[j]->g <= val ? 0 : 255);
      data[j]->b = (data[j]->b <= val ? 0 : 255);
    }
  return true;
}

// note: this function swaps col indices i and i+width/2 ..
// different from swapping first and last element in the row
bool Image::swap_cols2()
{
  int halfx = _width/2;
  for(int y = 0; y < _height; ++y)
    {
      for (int x=0, i1=y*_width, i2=y*_width+halfx; x<halfx; ++x, ++i1, ++i2)
	{
	  swap(i1,i2);
	}
    }
  return true;
}

// note: this function swaps row indices i and i+height/2 ..
// different from swapping first and last element in the col
bool Image::swap_rows2()
{
  int halfy = _height/2;
  for(int y = 0; y < halfy; ++y)
    {
      for (int x=0, i1=y*_width, i2=(halfy + y)*_width; x<_width; ++x, ++i1, ++i2)
	{
	  swap(i1, i2);
	}
    }
  return true;
}

bool Image::swap_rows(double * d, int w, int h)
{
  int halfy = h/2;
  for(int y = 0; y < halfy; ++y)
    {
      for (int x=0, i1=y*w, i2=(halfy + y)*w; x<w; ++x, ++i1, ++i2)
	{
	  double d1 = d[i1]; d[i1] = d[i2]; d[i2] = d1;
	}
    }
  return true;
}

bool Image::swap_cols(double * d, int w, int h)
{
  int halfx = w/2;
  for(int y = 0; y < h; ++y)
    {
      for (int x=0, i1=y*w, i2=y*w+halfx; x<halfx; ++x, ++i1, ++i2)
	{
	  double d1 = d[i1]; d[i1] = d[i2]; d[i2] = d1;
	}
    }
  return true;
}

void Image::swap(int i1, int i2)
{
  Pixel<float> t1 = *data[i1]; data[i1]->set(*data[i2]); data[i2]->set(t1); 
}

bool Image::swap_rows()
{
  int halfx = _width/2;
  int halfy = _height/2;
  for(int y = 0; y < halfy; ++y)
    {
      for (int x = 0, i1 = y*_width, i2 = y*_width + halfx, 
	     i3 = (y+halfy)*_width, i4 = (y+halfy)*_width + halfx; x < halfx; ++x, ++i1, ++i2, ++i3, ++i4)
	{
	  Pixel<float> t1 = *data[i1]; data[i1]->set(*data[i3]); data[i3]->set(t1); 
	  Pixel<float> t2 = *data[i2]; data[i2]->set(*data[i4]); data[i4]->set(t2);
	}      
    }
  return true;
}

bool Image::swap_cols()
{
  int halfx = _width/2;
  int halfy = _height/2;
  for(int y = 0; y < _height; ++y)
    {
      for (int x = 0,
	     i1 = y*_width,  // top left
	     i2 = y*_width + _width - 1,  // top right
	     i3 = (_height-y-1)*_width, // bottom left
	     i4 = (_height-y-1)*_width + _width - 1; // bottom right
	   x < halfx; ++x, ++i1, --i2, ++i3, --i4)
	{
	  Pixel<float> t1 = *data[i1]; data[i1]->set(*data[i2]); data[i2]->set(t1); 
	  Pixel<float> t2 = *data[i3]; data[i3]->set(*data[i4]); data[i4]->set(t2);
	}      
    }
  return true;
}

bool Image::fft(fftw_complex* R, fftw_complex* G, fftw_complex* B)
{
  fftw_plan rplan, gplan, bplan;

  // note: size of output for fourier coefficients in d-dimensions is
  // n1*n2*...*(nd/2+1).  Stored in row-major format.
  int nb = (_width/2+1) * (_height); 

  double * in_r = new double[_size]; std::memset(in_r, 0, _size * sizeof(double)); 
  double * in_g = new double[_size]; std::memset(in_g, 0, _size * sizeof(double)); 
  double * in_b = new double[_size]; std::memset(in_b, 0, _size * sizeof(double)); 

  // copy input data
  for (int j=0; j<_size; ++j) 
    {
      in_r[j] = data[j]->r; in_b[j] = data[j]->b; in_g[j] = data[j]->g;
    }
  rplan = fftw_plan_dft_r2c_2d(_height, _width, in_r, R, FFTW_ESTIMATE);
  gplan = fftw_plan_dft_r2c_2d(_height, _width, in_g, G, FFTW_ESTIMATE);
  bplan = fftw_plan_dft_r2c_2d(_height, _width, in_b, B, FFTW_ESTIMATE);
  
  fftw_execute(rplan);
  fftw_execute(gplan);
  fftw_execute(bplan);

  fftw_destroy_plan(rplan);
  fftw_destroy_plan(gplan);
  fftw_destroy_plan(bplan);
  fftw_cleanup();

  if (in_r) { delete [] in_r; in_r = 0; }
  if (in_g) { delete [] in_g; in_g = 0; }
  if (in_b) { delete [] in_b; in_b = 0; }
  return true;
}

bool Image::ifft(fftw_complex* R, fftw_complex* G, fftw_complex* B)
{
  if (R == 0 || G == 0 || B == 0) return false;
  fftw_plan irplan, igplan, ibplan; 
  double scale = 1.0/double(_size); // normalization factor

  double * out_r = new double[_size]; std::memset(out_r, 0, _size * sizeof(double)); 
  double * out_g = new double[_size]; std::memset(out_g, 0, _size * sizeof(double)); 
  double * out_b = new double[_size]; std::memset(out_b, 0, _size * sizeof(double)); 
  
  irplan = fftw_plan_dft_c2r_2d(_height, _width, R, out_r, FFTW_ESTIMATE);
  igplan = fftw_plan_dft_c2r_2d(_height, _width, G, out_g, FFTW_ESTIMATE);
  ibplan = fftw_plan_dft_c2r_2d(_height, _width, B, out_b, FFTW_ESTIMATE);
  
  fftw_execute(irplan);
  fftw_execute(igplan);
  fftw_execute(ibplan);

  // set image data
  for (int j=0; j<_size; ++j)
    {
      data[j]->r = out_r[j] * scale;
      data[j]->g = out_g[j] * scale;
      data[j]->b = out_b[j] * scale;
    }
  
  fftw_destroy_plan(irplan);
  fftw_destroy_plan(ibplan);
  fftw_destroy_plan(igplan);
  fftw_cleanup();

  if (out_r) { delete [] out_r; out_r = 0; }
  if (out_g) { delete [] out_g; out_g = 0; }
  if (out_b) { delete [] out_b; out_b = 0; } 

  return true;
}

bool Image::lowpass_filter(double r0, double n)
{
  /*butterworth lowpass filter*/
  double * f = new double[_size]; // filter image
  for (int y=0, idx=0; y<_height; ++y)
    {
      for (int x=0; x<_width; ++x, ++idx)
	{	  
	  double xn = double( x - _width/2.0 );
	  double yn = double( y - _height/2.0 );
	  double r2 = sqrt(xn * xn + yn * yn);
	  f[idx] = 1.0 / (1.0 + pow( (r2 / r0), 2 * n)); // low pass butterworth filter
	}
    }

  double * ff = build_fft_filter(f, _width, _height);  

  fftw_complex *R, *G, *B; 
  int nb = (_width/2 + 1) * _height;
  // allocate coefficients for forward fourier transformation
  R = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);
  G = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);
  B = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);

  fft(R, G, B);
  fourier_convolve(R, G, B, ff, _width/2+1, _height);
  if (!ifft(R, G, B)) { printf (" .. ifft failed\n"); }

  if (R) { delete R, R = 0; }
  if (G) { delete G, G = 0; }
  if (B) { delete B, B = 0; }

  return true;
}

bool Image::highpass_filter(double r0, double n)
{
  /*butterworth lowpass filter*/
  double * f = new double[_size]; // filter image
  for (int y=0, idx=0; y<_height; ++y)
    {
      for (int x=0; x<_width; ++x, ++idx)
	{	  
	  double xn = double (x - _width / 2.0);
	  double yn = double (y - _height / 2.0);
	  double r2 = sqrt(xn * xn + yn * yn);
	  if (r2 > 0)
	    f[idx] = 1.0 / (1.0 +  pow( (r0 / r2), 2 * n)); // high pass butterworth filter
	  else f[idx] = 1.0;
	}
    }  
  double * ff = build_fft_filter(f, _width, _height);  

  fftw_complex *R, *G, *B; 
  int nb = (_width/2 + 1) * _height;
  // allocate coefficients for forward fourier transformation
  R = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);
  G = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);
  B = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nb);

  fft(R, G, B);  
  fourier_convolve(R, G, B, ff, _width/2+1, _height);
  if (!ifft(R, G, B)) { printf (" .. ifft failed\n"); }

  if (R) { delete R; R = 0; }
  if (G) { delete G; G = 0; }
  if (B) { delete B; B = 0; }
  if (ff){ delete [] ff; ff = 0; }

  return true;
}

// dbg save fourier mag spectrum
void Image::save_mag_image(char * fname, fftw_complex * R, fftw_complex * G, fftw_complex * B, int w, int h)
{
  Image * mag_img = new Image(w, h);
  int sc = (w/2 + 1) * h;
  for (int y=0; y<h; ++y)
    {
      for (int x=0; x<w/2; ++x)
	{
	  int j = y * (w/2+1) + x;
	  float vr = (float) sqrt(R[j][0]*R[j][0] + R[j][1] * R[j][1])/(sc);
	  float vg = (float) sqrt(G[j][0]*G[j][0] + G[j][1] * G[j][1])/(sc);
	  float vb = (float) sqrt(B[j][0]*B[j][0] + B[j][1] * B[j][1])/(sc);
	  vr = 255*log(1+vr); vg = 255*log(1+vg) ; vb = 255*log(1+vb);
	  clamp(vr, vg, vb);
	  int j1 = y * w + x;
	  mag_img->set(j1,vr,vg,vb);

	  if (x > 0)
	    {
	      int j2 = (h - 1 - y) * w  + (w-1) - (x-1);
	      mag_img->set(j2,vr,vg,vb);
	    }
	}
    }
  mag_img->swap_rows2();
  mag_img->swap_cols2();
  mag_img->save(fname,100);
  delete mag_img; mag_img = 0;
}

// assumes input filter is of size w*h (and radially symmetric for fourier spectrum)
double * Image::build_fft_filter(double * filter, int w, int h)
{
  swap_cols(filter, w, h);
  swap_rows(filter, w, h);
  
  Image * ifilter1 = new Image(w, h);
  /*
  for (int j=0; j<w*h; ++j)
    {      
      ifilter1->set(j,255*(float)filter[j]);
    }
  ifilter1->convert_gs();
  ifilter1->save("/home/mjg/Desktop/dbg-frequency-filter-swapped.jpg",100);
  delete ifilter1; ifilter1 = 0;
  */
  double * ffilter = new double[ h * (w/2+1)];
  for (int y=0, i1 = 0; y<h; ++y)
    {
      for (int x=0; x<w/2+1; ++x, ++i1)
	{
	  int i2 = y * w + x;
	  ffilter[i1] = filter[i2];
	}
    }

  /*
  Image * ifilter2 = new Image(w/2+1, h);
  for (int j=0; j<(w/2+1)*h; ++j)
    {      
      ifilter2->set(j,255*(float)ffilter[j]);
    }
  ifilter2->convert_gs();
  ifilter2->save("/home/mjg/Desktop/dbg-frequency-filter-chopped.jpg",100);
  delete ifilter2; ifilter2 = 0;
  */
  return ffilter;
}

// note: this scales both the real and complex components of the fourier spectrum
void Image::fourier_convolve(fftw_complex *R, fftw_complex *G, fftw_complex*B, double*C, int w, int h)
{
  for (int j=0; j<w*h; ++j)     
    {
      R[j][0] *= C[j];
      R[j][1] *= C[j];
      
      G[j][0] *= C[j];
      G[j][1] *= C[j];
      
      B[j][0] *= C[j];
      B[j][1] *= C[j];
    }
}

int Image::pow2(int i)
{
  // returns the nearest (greatest) power of 2 given integer i
  if ( i <= 0 || !(i & (i-1))) return i;
  int pow = 1;
  while (i > 0) {i >>= 1; pow <<= 1 ;}
  return pow;
}


