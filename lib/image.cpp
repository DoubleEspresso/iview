#include <stdio.h>
#include <ctype.h>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <fstream>
#include <fftw3.h>

#include "image.h"
#include "stringutils.h"


Image::Image() :
  _height(0), _width(0), _comps(0), 
  _size(0), data(0), jpeg_handle(0), filter(0)
{
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
}

Image::Image(uint w, uint h) : 
  _height(h), _width(w), _comps(3), 
  _size(w*h), data(0), jpeg_handle(0), filter(0)
{
  gammas.r = 1; gammas.b = 1; gammas.g = 1;
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
int Image::width() { return _width; }
int Image::height(){ return _height;}
int Image::size()  { return _size; }

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
}


// pixel space convolution definitions
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
  if (filter) { delete filter; filter = 0; }
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
  if (filter) { delete filter; filter = 0; }
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
      float nc = sqrt(r*r + g*g + b*b);
      clamp(nc, nc, nc);
      data[j]->set(nc, nc, nc);
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

bool Image::convolve_fft()
{
  fftw_plan rplan, gplan, bplan, irplan, igplan, ibplan, fplan; 
  fftw_complex *R, *G, *B, *C, *IR, *IG, *IB; // storage for the fourier coefficients
  
  // note: size of output for fourier coefficients in d-dimensions is
  // n1*n2*...*(nd/2+1).  Stored in row-major format.
  int maxd = (_width > _height ? _width : _height);
  maxd = pow2(maxd);
  int nb = (_width/2+1) * (_height); 
  int zpad = _size;//2maxd * maxd; //_size + _size - 1;
  double scale = 1.0/double(_size); // normalization factor
  
  double * in_r = new double[zpad]; std::memset(in_r, 0, zpad * sizeof(double)); 
  double * in_g = new double[zpad]; std::memset(in_g, 0, zpad * sizeof(double)); 
  double * in_b = new double[zpad]; std::memset(in_b, 0, zpad * sizeof(double)); 

  double * out_r = new double[zpad]; std::memset(out_r, 0, zpad * sizeof(double)); 
  double * out_g = new double[zpad]; std::memset(out_g, 0, zpad * sizeof(double)); 
  double * out_b = new double[zpad]; std::memset(out_b, 0, zpad * sizeof(double)); 

  double * f = new double[zpad]; // the filtered image
  
  zpad = nb;
  // allocate coefficients for forward fourier transformation
  R = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);
  G = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);
  B = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);
  C = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);

  // allocate coefficients for backward fourier transformation
  IR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);
  IG = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);
  IB = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * zpad);

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
  
  /* filtering dbg */
  double s = 1;
  double range = 10;
  double norm = 1;
  double freq = 1.25;//2.0/_width; 
  double exps = 1/(2*s*s);
  double nrm = 1.0 / (sqrt(2*M_PI)*s);
  for (int y=0, idx=0; y<_height; ++y)
    {
      for (int x=0; x<_width; ++x, ++idx)
	{	  
	  double xn = x;//2.0*range * double( x - _width/2.0 ) / double(_width);
	  double yn = y;//2.0*range * double( y - _height/2.0 ) / double(_height);
	  //f[idx] = sin( 2.0 * M_PI * freq * x);
	  double r2 = xn * xn + yn * yn;
	  //if (r2 == 0) f[idx] = 1.0;
	  //else f[idx] = sin(freq * M_PI * sqrt(r2)) / (freq * M_PI * sqrt(r2));
	  f[idx] = exp(-r2 *exps );
	  norm += f[idx];
	}
    }
  for (int j=0; j<_size; ++j) f[j] *= 1/norm;
  fplan = fftw_plan_dft_r2c_2d(_height, _width, f, C, FFTW_ESTIMATE);
  fftw_execute(fplan);
  
  /* debug save image filter */
  /*
  Image * ifilter = new Image(_width, _height);
  for (int j=0; j<_size; ++j)
    {      
      ifilter->set(j,(float)255*f[j]);
    }
  ifilter->convert_gs();
  ifilter->save("/home/mjg/Desktop/dbg-gauss-filter.jpg",100);
  delete ifilter; ifilter = 0;
  */

  
  /*debug save fft spectrum/phase data*/
  int w = _width;
  int h = _height;
  Image * fft_image = new Image(w, h);
  //float maxr = 0; float maxg = 0; float maxb = 0;

  /*for (int j=0; j<zpad; ++j)
  {
    float vr = (float) (R[j][0]*R[j][0] + R[j][1] * R[j][1]);
    float vg = (float) (G[j][0]*G[j][0] + G[j][1] * G[j][1]);
    float vb = (float) (B[j][0]*B[j][0] + B[j][1] * B[j][1]);
    if (vr > maxr ) maxr = vr;
    if (vg > maxg ) maxg = vg;
    if (vb > maxb ) maxb = vb;
  }
  */
  for(int y=0, j=0, i1 = _width*_height-1; y < _height; ++y)
    {
      //for (int x=0, j=y*_width, i1 = j + _width-1; x<_width/2; ++x, ++j, --i1)
      for (int x=0; x<_width/2; ++x, ++j, --i1)
	{

	  float vr = (float) (R[j][0]*R[j][0] + R[j][1] * R[j][1])/(zpad);
	  float vg = (float) (G[j][0]*G[j][0] + G[j][1] * G[j][1])/(zpad);
	  float vb = (float) (B[j][0]*B[j][0] + B[j][1] * B[j][1])/(zpad);
	  //clamp(vr, vg, vb);
	  //printf("%f,%f,%f\n", vr, vg, vb);
	  //vr = -log(vr); vg = -log(vg) ; vb = -log(vb);
	  fft_image->set(j, vr, vg, vb);
	  fft_image->set(i1, vr, vg, vb);
	}
    }
  //fft_image->fftswap();
  //fft_image->convert_gs();
  fft_image->save("/home/mjg/Desktop/dbg-fft-spectrum.jpg",100);
  delete fft_image; fft_image = 0;

  for (int j=0; j<zpad; ++j)     
    {
      float rre = R[j][0] * C[j][0] - R[j][1] * C[j][1];
      float rim = R[j][1] * C[j][0] + R[j][0] * C[j][1];
      R[j][0] = rre; R[j][1] = rim;
      
      float gre = G[j][0] * C[j][0] - G[j][1] * C[j][1];
      float gim = G[j][1] * C[j][0] + G[j][0] * C[j][1];
      G[j][0] = gre; G[j][1] = gim;
      
      float bre = B[j][0] * C[j][0] - B[j][1] * C[j][1];
      float bim = B[j][1] * C[j][0] + B[j][0] * C[j][1];
      B[j][0] = bre; B[j][1] = bim;
    }

  /*
  Image * ffilter = new Image(_width/2+1, _height);
  std::ofstream datfile;
  datfile.open("spectrum.txt");
  for (int y=0, j=0, i = nb-1 ; y<_height; ++y)
    {
      for (int x=0; x<_width/2+1; ++x, ++j, --i)
	{
	  float val = (float) (C[j][0]*C[j][0] + C[j][1] * C[j][1]);
	  char line[256];
	  sprintf(line, "%d %d %f\n", x, y, val);
	  if (val > 0.1) datfile << line; 
	  ffilter->set(j, val);
	}
    }
  datfile.close();
  ffilter->convert_gs();
  ffilter->save("/home/mjg/Desktop/dbg-gauss-fft-filter.jpg",100);
  delete ffilter; ffilter = 0;
  */

  irplan = fftw_plan_dft_c2r_2d(_height, _width, R, out_r, FFTW_ESTIMATE);
  igplan = fftw_plan_dft_c2r_2d(_height, _width, G, out_g, FFTW_ESTIMATE);
  ibplan = fftw_plan_dft_c2r_2d(_height, _width, B, out_b, FFTW_ESTIMATE);

  fftw_execute(irplan);
  fftw_execute(igplan);
  fftw_execute(ibplan);
  
  // normalization
  for (int j=0; j<_size; ++j)
    {
      data[j]->r = out_r[j] * 1/_size;
      data[j]->g = out_g[j] * 1/_size;
      data[j]->b = out_b[j] * 1/_size;
    }

  fftw_destroy_plan(rplan);
  fftw_destroy_plan(gplan);
  fftw_destroy_plan(bplan);
  fftw_destroy_plan(fplan);
  fftw_destroy_plan(irplan);
  fftw_destroy_plan(ibplan);
  fftw_destroy_plan(igplan);
  fftw_cleanup();

  if (in_r) { delete [] in_r; in_r = 0; }
  if (in_g) { delete [] in_g; in_g = 0; }
  if (in_b) { delete [] in_b; in_b = 0; }
  if (out_r) { delete [] out_r; out_r = 0; }
  if (out_g) { delete [] out_g; out_g = 0; }
  if (out_b) { delete [] out_b; out_b = 0; } 
  if (f) { delete [] f; f = 0; }

  printf("..done with fft\n");
  return true;
}

bool Image::fftswap()
{
  int halfx = _width/2;
  int halfy = _height/2;
  for(int y = 0; y < halfy; ++y)
    {
      for (int x = 0, i1 = y*_width, i2 = y*_width + halfx, 
	     i3 = (y+halfy)*_width, i4 = (y+halfy)*_width + halfx; x < halfx; ++x, ++i1, ++i2, ++i3, ++i4)
	//i3 = (_height - y - 1 ) * _width + halfx, i4 = (_height - y )* _width - 1; x<halfx; ++x, ++i1, ++i2, --i3, --i4)
	{
	  Pixel<float> t1 = *data[i1]; data[i1]->set(*data[i4]); data[i4]->set(t1); 
	  Pixel<float> t2 = *data[i2]; data[i2]->set(*data[i3]); data[i3]->set(t2);
	}      
    }
  return true;
}

int Image::pow2(int i)
{
  // returns the nearest (greatest) power of 2 given integer i
  if ( i <= 0 || !(i & (i-1))) return i;
  int pow = 1;
  while (i > 0) {i >>= 1; pow <<= 1 ;}
  return pow;
}


