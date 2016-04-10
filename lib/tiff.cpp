#include <stdio.h>

#include "img.h"


xTiff::xTiff(std::string fname)
{ 
  try
    {
      name = new char[fname.size()+1];
      memcpy(name,fname.c_str(),fname.size()+1);
      char * ext = extension2(fname);
      if (!strcmp(ext,"tiff") )
	{
	  type = im_TIFF;
	  if (!this->open())
	    Log::write("..[xray] Error : failed to open %s\n",name);
	}
      else if (!strcmp(ext,"jpeg"))
	{
	  Log::write("..[xray] Found .jpeg extension for tiff image object, ignored\n");
	  this->close();
	}
      else 
	{
	  Log::write("..[xray] Invalid extension passed, ignored\n");
	  this->close();
	}
    }
  catch(const std::exception &e)
    {
      Log::write("..[xray] exception : Image::Image() (%s)\n",e.what());
    }
}

bool xTiff::open()
{
  try{
    imgp = TIFFOpen(name, "r");
    TIFFGetField(imgp, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(imgp, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    tdata_t buff = _TIFFmalloc(TIFFScanlineSize(imgp));
    TIFFGetField(imgp, TIFFTAG_BITSPERSAMPLE, &dp);
    switch(dp)
      {
      case 8  : depth = BIT8;  nbytes = sizeof(uint8)  * npixels; break;
      case 16 : depth = BIT16; nbytes = sizeof(uint16) * npixels; break;
      case 32 : depth = BIT32; nbytes = sizeof(uint32) * npixels; break;
      }

    if (depth == BIT8 && (data8 = new uint8[npixels]) != NULL && (ddata = new double[npixels]) != NULL)
      Log::write("..[xray] allocate 8-bit buffer\n");
    else if (depth == BIT16 && (data16 = new uint16[npixels]) != NULL && (ddata = new double[npixels]) != NULL)
      Log::write("..[xray] allocate 16-bit buffer\n");
    else if (depth == BIT32 && (data32 = new uint32[npixels]) != NULL && (ddata = new double[npixels]) != NULL)
      Log::write("..[xray] allocate 32-bit buffer\n");
    else
      {
	Log::write("..[xray] failed to allocate memory for (%s)\n",name);
	close();
	return false;
      }

    for (int j=0; j<h; ++j)
      {	 
	TIFFReadScanline(imgp,buff,j);
	if (depth == BIT8)
	  for(int i=0; i<w;++i)
	    {
	      data8[j*w + i] = ((uint8 *) buff)[i];
	      ddata[j*w + i] = double( data8[j*w+i] );
	    }
	else if (depth == BIT16)
	  for(int i=0; i<w;++i)
	    {
	      data16[j*w + i] = ((uint16 *) buff)[i];
	      ddata[j*w + i] = double( data16[j*w+i] );
	    }
	else if (depth == BIT32)
	  for(int i=0; i<w;++i)
	    {
	      data32[j*w + i] = ((uint32 *) buff)[i];
	      ddata[j*w + i] = double( data32[j*w+i] ); 
	    }
      }
    Log::write("..[xray] opened %s depth(%d), %d(w) x %d(h) = %d pixels, %d bytes\n",
	       name,
	       dp,
	       w,
	       h,
	       npixels,
	       nbytes);    
    is_open = true;
  }
  catch(std::exception &e)
    {
      Log::write("..[xray] Tiff::open() exception %s\n",e.what());
    }
  return true;
}
bool xTiff::save(const char * outf)
{
  try
    {
      TIFF * outi;
      if ((outi = TIFFOpen(outf,"w"))==NULL)
	{
	  Log::write("..[xray] Error : unable to open %s, cannot save\n", 
		     outf);
	  this->close();
	  return false;
	}
      TIFFSetField(outi, TIFFTAG_IMAGEWIDTH, w);
      TIFFSetField(outi, TIFFTAG_IMAGELENGTH, h);
      TIFFSetField(outi, TIFFTAG_BITSPERSAMPLE, dp);
      TIFFSetField(outi, TIFFTAG_SAMPLESPERPIXEL, 1);
      TIFFSetField(outi, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
      //TIFFSetField(outi, TIFFTAG_COMPRESSION, COMPRESSION_LZW); // no compression by default
      TIFFSetField(outi, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);     
      if (dp == 8) TIFFWriteEncodedStrip(outi, 0, data8, npixels * sizeof(uint8));
      else if (dp == 16) TIFFWriteEncodedStrip(outi, 0, data16, npixels * sizeof(uint16));
      else if (dp == 32) TIFFWriteEncodedStrip(outi, 0, data32, npixels * sizeof(uint32));
      TIFFClose(outi);
    }
  catch(std::exception &e)
    {
      Log::write("..[xray] Tiff::save() exception %s\n", e.what());
      return false;
    } 
  return true;
}

bool xTiff::close()
{
  if(imgp)
    TIFFClose(imgp);
  if (data8)
    {
      delete data8;
      data8 = 0;
    }
  if (data16)
    {
      delete data16;
      data16 =0;
    }
  if (data32)
    {
      delete data32;
      data32 =0;
    }
  if (ddata)
    {
      delete ddata;
      ddata = 0;
    }
  return true;
}

xTiff::~xTiff()
{
  if ( close() )
    Log::write("..[xray] close image ok\n");
}

