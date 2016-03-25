#include <stdio.h>
#include <ctype.h>
#include <algorithm>

#include "image.h"
#include "stringutils.h"

Image::Image() :
  _height(0), _width(0), _comps(0), 
  _size(0), data(0), jpeg_handle(0), filter(0)
{
}

Image::Image(uint w, uint h) : 
  _height(h), _width(w), _comps(3), 
  _size(0), data(0), jpeg_handle(0), filter(0)
{
  init(w,h,3);
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
}

bool Image::clear()
{
  _size = _width = _height = _comps = 0;
  if (jpeg_handle) { delete jpeg_handle; jpeg_handle = 0; }
  if (data)
    {
      for (int j=0; j<_size; ++j)
	{
	  delete data[j]; data[j] = 0;
	}
      delete[] data; data = 0;
    }
  return true;
}

bool Image::init(int w, int h, int c)
{
  if (_size <= 0 ) return false;
  if (data) clear();
  _width = w; _height = h; _comps = c; _size = w * h;

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
int Image::width() { return _width; }
int Image::height(){ return _height;}
int Image::size()  { return _size; }


bool Image::save(char * filename, uint quality)
{
  if(!jpeg_handle) jpeg_handle = new Image_JPEG();//return false;
  unsigned char * tmp = new unsigned char[_width * _height * _comps];
  for (int j=0, idx=0; j < _width * _height; ++j, idx += 3)
    {
      tmp[idx] = data[j]->r; tmp[idx+1] = data[j]->g; tmp[idx+2] = data[j]->b;
    }
  jpeg_handle->copy_data(tmp, _width, _height, _comps);  
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


void Image::sharpen(int kernel_size)
{
  if (filter)
    {
      delete filter; filter = 0;
    }

  switch(kernel_size)
    {
    case 3: filter = new Filter<float>(Sharpen3x3); break;
    case 5: filter = new Filter<float>(Sharpen5x5); break;
    case 7: filter = new Filter<float>(Sharpen7x7); break;
    }

  int r = kernel_size;
  float * kernel = filter->get_kernel();

  // create storage for new image data
  Pixel<float> ** result;
  result = new Pixel<float>*[_size];
  for (int j=0; j<_size; ++j)
    {
      result[j] = new Pixel<float>(0,0,0);
    }

  // convolve based on kernel_size
  for (int j = 0, ci = 0; j < _height; ++j)
    {
      for (int i=0; i < _width; ++i, ++ci)
	{
	  float red = 0;
	  float grn = 0;
	  float blu = 0;
	  
	  // perform convolution over kernel
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
	  clamp(red, grn, blu, 1, 0);
	  result[ci]->set(red, grn, blu);
	}
    }

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
}

void Image::clamp(float& r, float& g, float& b, float scale, float bias)
{
  r = r * scale + bias; r = (r < 0 ? 0 : r) ; r = (r > 255 ? 255 : r);
  g = g * scale + bias; g = (g < 0 ? 0 : g) ; g = (g > 255 ? 255 : g);
  b = b * scale + bias; b = (b < 0 ? 0 : b) ; b = (b > 255 ? 255 : b);
}

int Image::wrap(int i, int max)
{
  i = i%max;
  return (i < 0 ? i + max : i);
}
