#include <stdio.h>
#include <ctype.h>

#include "image.h"
#include "stringutils.h"

Image::Image()
{
}

Image::Image(uint w, uint h) : 
  _height(h), _width(w), _comps(0), 
  _size(0), data(0), jpeg_handle(0)
{ 
  if (!jpeg_handle) jpeg_handle = new Image_JPEG();
}

Image::~Image() 
{
  if (jpeg_handle) { delete jpeg_handle; jpeg_handle = 0; }
  if (data) { delete[] data; data = 0; }  
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

  if (!jpeg_handle) jpeg_handle = new Image_JPEG();
  bool ok = jpeg_handle->load_jpeg(filename);
  _width = jpeg_handle->width();
  _height = jpeg_handle->height();
  _comps = jpeg_handle->comps();
  _size = jpeg_handle->size();
  if (data) { delete [] data; data = 0; }
  data = new unsigned char [_size];
  memcpy(data, jpeg_handle->img_data(), _size * sizeof(unsigned char));
  return ok;
}

int Image::comps() { return _comps; }
int Image::width() { return _width; }
int Image::height(){ return _height;}
int Image::size()  { return _size; }


bool Image::save(char * filename, uint quality)
{
  if(!jpeg_handle) return false;
  jpeg_handle->copy_data(data, _width, _height, _comps);  
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


char Image::convolve3(const char* image, float*kernel)
{
  float res = 0;
  float sum = 0;
  for (int i = 0; i < 9; ++i) // all components of the kernel
    {
      res += kernel[i] * image[i];	  
      sum += kernel[i];//(kernel[i] < 0 ? -kernel[i] : kernel[i]);
    }
  return res / sum;
}

// nb. edits data in place!
void Image::sharpen()
{
  float kernel[9] = { 0, -1, 0,
		     -1,  5, -1,
		      0, -1, 0 };
  
  /*
float kernel[9] = { 0, 0, 0,
		    0, 1, 0,
		    0, 0, 0 };
  */
int stride = comps() * width();

  int row_adjust[9] = { -1, -1, -1,
			0, 0, 0,
			1, 1, 1 };
  int col_adjust[9] = { -comps(), 0, comps(),
			-comps(), 0, comps(),
			-comps(), 0, comps() };
  printf("..dbg sharpen start\n");
  for (int j=0; j< height(); ++j) // rows
    {
      //printf("..row(%d)\n",j);
      for (int i=0; i<stride; ++i) // cols
	{
	  int center = j * stride + i;
	  char tmp[9];
	  for (int p = 0; p < 9; ++p)
	    {	      	      
	      int idx = wrap(stride, j, i, row_adjust[p], col_adjust[p]);
	      tmp[p] = data[idx];
	    }
	  //printf("%d->%d\n",data[center], (unsigned char) convolve3(tmp,kernel));
	  data[center] = (unsigned char)convolve3(tmp, kernel);
	}
    }

}

int Image::wrap(int stride, int row, int col, int drow, int dcol)
{
  if (row + drow < 0 || row + drow >= height())
    {
      row = (row + drow < 0 ? height()-1 : 0);
    }
  else row += drow;
  if (col + dcol < 0 || col + dcol >= stride)
    {
      col = (col + dcol < 0 ? stride-1-(2-col) : dcol);
    }
  else col += dcol;

  return stride * row + col;  
}
