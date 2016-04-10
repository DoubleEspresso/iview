#include "image_tiff.h"


Image_TIFF::Image_TIFF() :   
  _width(0), _height(0), _comps(0),
  _size(0), _depth(0), _bytes(0), _order(0),
  gs_data_8(0), gs_data_16(0), gs_data_32(0),
  data_8(0), data_16(0), data_32(0),
  tiff_handle(0)
{
}

Image_TIFF::~Image_TIFF()
{
  clear_gs();
  clear_rgb();
  //if (tiff_handle) { delete tiff_handle; tiff_handle = 0; }
} 

bool Image_TIFF::load_tiff(char * fname)
{
  tiff_handle = TIFFOpen(fname, "r");
  TIFFGetField(tiff_handle, TIFFTAG_IMAGEWIDTH, &_width);
  TIFFGetField(tiff_handle, TIFFTAG_IMAGELENGTH, &_height);
  _size = _width * _height;
  TIFFGetField(tiff_handle, TIFFTAG_BITSPERSAMPLE, &_depth);
  TIFFGetField(tiff_handle, TIFFTAG_SAMPLESPERPIXEL, &_comps);  
  TIFFGetField(tiff_handle, TIFFTAG_FILLORDER, &_order);
  if (_comps <= 0 || _comps > 4) return false;
  if (_comps == 1) // gray scale data
    {
      init_gs();
      read_gs();
      printf("..load tiff ok\n");
    }
  else if (_comps == 3)
    {
      init_rgb();
      read_rgb();
    }
  else
    {
      printf("..tiff found %d-samples per pixel (not supported) abort.\n", _comps);
      clear_gs();
      clear_rgb();
      return false;
    }
  return true;
}

bool Image_TIFF::save_tiff(char * filename)
{
  return true;
}

bool Image_TIFF::read_gs()
{
  if (!tiff_handle) return false;
  void * buff;
  buff = _TIFFmalloc(TIFFScanlineSize(tiff_handle));
  for (int y = 0, idx=0; y < _height; ++y)
    {
      TIFFReadScanline(tiff_handle, buff, y);
      switch (_depth)
	{
	case BIT8:
	  for (int x=0; x<_width; ++x, ++idx)
	    {
	      gs_data_8[idx] = ((uint8*) buff)[x];
	    }
	  break;
	case BIT16:
	  for (int x=0; x<_width; ++x, ++idx)
	    {
	      gs_data_16[idx] = ((uint16*) buff)[x];
	    }
	  break;
	case BIT32:
	  for (int x=0; x<_width; ++x, ++idx)
	    {
	      gs_data_32[idx] = ((uint32*) buff)[x];
	    }
	  break;
	}
    }
  _TIFFfree(buff);
  return true;
}

bool Image_TIFF::read_rgb()
{
  if (!tiff_handle) return false;
  void * buff;
  buff = _TIFFmalloc(TIFFScanlineSize(tiff_handle));
  int config = 0;
  TIFFGetField(tiff_handle, TIFFTAG_PLANARCONFIG, &config);
  /*parse config*/
  if (config == PLANARCONFIG_SEPARATE)
    {
      for (int c =0, idx = 0; c<_comps; ++c)
	{
	  for (int y = 0; y < _height; ++y)
	    {
	      TIFFReadScanline(tiff_handle, buff, y, c);
	      switch (_depth)
		{
		case BIT8:
		  for (int x=0; x<_width; ++x, ++idx)
		    {
		      data_8[idx] = ((uint8*) buff)[x];
		    }
		  break;
		case BIT16:
		  for (int x=0; x<_width; ++x, ++idx)
		    {
		      data_16[idx] = ((uint16*) buff)[x];
		    }
		  break;
		case BIT32:
		  for (int x=0; x<_width; ++x, ++idx)
		    {
		      data_32[idx] = ((uint32*) buff)[x];
		    }
		  break;
		}
	    }
	}
    }
  else
    {
      printf("..invalid config parsed %d not supported (abort)\n", config);
    }
  _TIFFfree(buff);
  return true;
}

bool Image_TIFF::init_gs()
{
  clear_gs();
  switch(_depth)
    {
    case BIT8 :
      _bytes = sizeof(uint8)  * _size;
      gs_data_8 = new uint8[_size];      
      for (int j=0; j<_size; ++j)
	{
	  gs_data_8[j] = 0;
	}
      break;
    case BIT16 :
      _bytes = sizeof(uint16) * _size;
      gs_data_16 = new uint16[_size];
      for (int j=0; j<_size; ++j)
	{
	  gs_data_16[j] = 0;
	}
      break;
    case BIT32 :
      _bytes = sizeof(uint32) * _size;
      gs_data_32 = new uint32[_size];
      for (int j=0; j<_size; ++j)
	{
	  gs_data_32[j] = 0; 
	}
      break;
    }
  return true;
}

bool Image_TIFF::clear_gs()
{
  switch(_depth)
    {
    case BIT8  :
      if (gs_data_8)
	{
	  delete [] gs_data_8; gs_data_8 = 0;
	}
    case BIT16:
      if (gs_data_16)
	{
	  delete [] gs_data_16; gs_data_16 = 0;
	}
    case BIT32:
      if (gs_data_32)
	{
	  delete [] gs_data_32; gs_data_32 = 0;
	}
    }
    _width = 0; _height = 0; _comps = 0; _bytes = 0;
    _depth = 0; _size = 0;
    return true;
}

bool Image_TIFF::init_rgb()
{
  clear_rgb();
  switch(_depth)
    {
    case BIT8  :
      _bytes = sizeof(uint8)  * _size * _comps;
      data_8 = new uint8[_size * _comps];
      for (int j=0; j<_size*_comps; ++j)
	{
	  data_8[j] = 0;
	}
      break;
    case BIT16 :
      _bytes = sizeof(uint16) * _size * _comps;
      data_16 = new uint16[_size*_comps];
      for (int j=0; j<_size*_comps; ++j)
	{
	  data_16[j] = 0;
	}
      break;
    case BIT32 :
      _bytes = sizeof(uint32) * _size *_comps;
      data_32 = new uint32[_size*_comps];
      for (int j=0; j<_size*_comps; ++j)
	{
	  data_32[j] = 0;
	}
      break;
    }
  return true;
}

bool Image_TIFF::clear_rgb()
{
  switch(_depth)
    {
    case BIT8  :
      if (data_8)
	{
	  delete [] data_8; data_8 = 0;
	}
    case BIT16:
      if (data_16)
	{
	  delete [] data_16; data_16 = 0;
	}
    case BIT32:
      if (data_32)
	{
	  delete [] data_32; data_32 = 0;
	}
    }
    _width = 0; _height = 0; _comps = 0; _bytes = 0;
    _depth = 0; _size = 0;
    return true;
}
