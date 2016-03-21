#include "image_jpeg.h"


Image_JPEG::Image_JPEG()
{
}

bool Image_JPEG::load_jpeg(char * fname)
{
  struct jpeg_decompress_struct decompress_params;

  FILE * infile;
  JSAMPARRAY buffer;
  int stride;

  if ((infile = fopen(fname, "rb")) == NULL)
    {
      return false;
    }

  struct jpeg_err jerr;
  decompress_params.err = jpeg_std_error(&jerr.pub);
  //jerr.pub.error_exit = jpeg_err_exit;

  if (setjmp(jerr.setjmp_buffer))
    {
      jpeg_destroy_decompress(&decompress_params);
      fclose(infile);
      return false;
    }
  
  jpeg_create_decompress(&decompress_params);
  jpeg_stdio_src(&decompress_params, infile);
  jpeg_read_header(&decompress_params, true);
  jpeg_start_decompress(&decompress_params);
  stride = decompress_params.output_width * decompress_params.output_components;
  buffer = (*decompress_params.mem->alloc_sarray)((j_common_ptr)&decompress_params, JPOOL_IMAGE, stride, 1);
  img_width = decompress_params.output_width;
  img_height= decompress_params.output_height;
  img_comps = decompress_params.output_components;
  data = new unsigned char[decompress_params.output_width * decompress_params.output_height * decompress_params.output_components];
  int line_count = 0;
  while (decompress_params.output_scanline < decompress_params.output_height)
    {
      jpeg_read_scanlines(&decompress_params, buffer, 1); // 1 only for gray scale
      for(int i=0; i<stride; ++i)
	{
	  data[line_count * stride + i] = buffer[0][i];
	}
      ++line_count;
    }
  jpeg_finish_decompress(&decompress_params);
  jpeg_destroy_decompress(&decompress_params);
  fclose(infile);
  return true;
}

bool Image_JPEG::flipv()
{
  if (!data || width() <= 1 || height() <= 1) return false;
  int stride = width() * comps();
  
  for (int i = 0, idx = height()-1; i < height() / 2; ++i, --idx)
    {
      for (int j=0 ; j<stride; j += comps())
	{
	  for (int k =0; k<comps(); ++k)
	    { 
	      int iup = i * stride + j + k;
	      int idown = idx * stride + j + k;
	      unsigned char src = data[iup];
	      data[iup] =  data[idown];
	      data[idown] = src;
	    }
	}
    }
  return true;
}

bool Image_JPEG::fliph()
{
  if (!data || width() <= 1 || height() <= 1) return false;
  int stride = width() * comps();
  for (int j = 0, idx = stride; j < stride/2 ; j += comps(), idx -= comps())
    {
      for (int i=0; i < height(); ++i)
	{ 

	  for (int k=0; k<comps(); ++k)
	    {	   
	      int ileft = i * stride + j + k;
	      int iright = i * stride + idx - k;
	      unsigned char src = data[ileft];
	      data[ileft] = data[iright];
	      data[iright] = src;
	    }	  
	}
    } 
  return true;
}

bool Image_JPEG::rotate90()
{
  if (!data || width() <= 1 || height() <= 1) return false;
  int stride = width() * comps();

  unsigned char *  dst = new unsigned char[width() * height() * comps()];  
  for (int j = stride-comps(), idx = 0; j >=0 ; j-= comps())
    {
      for (int i=0; i < height(); ++i)
	{ 

	  for (int k=0; k<comps(); ++k)
	    {
	      int isrc = i * stride + j + k;
	      dst[idx++] = data[isrc];
	    }
	  
	}
    } 
  memcpy(data, dst, size() * sizeof(unsigned char));  

  // try inplace rotate ??

  delete [] dst; dst = 0;
  int w = width();
  img_width = height(); 
  img_height = w;
  stride = width() * comps();

  return true;
}

bool Image_JPEG::save_jpeg(char * fname, uint quality)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_err jerr;
  FILE * outfile;
  JSAMPROW row_pointer[1];
  int row_stride;
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jpeg_create_compress(&cinfo);
  if ( (outfile = fopen(fname, "wb")) == NULL)
    {
      return false;
    }
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = img_width;
  cinfo.image_height= img_height;
  cinfo.input_components = img_comps;
  cinfo.in_color_space = JCS_RGB; // JCS_GRAYSCALE 
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, true);
  jpeg_start_compress(&cinfo, true);
  row_stride = cinfo.image_width * cinfo.input_components; // 3 for RGB images
  
  int line_count = 0;
  unsigned char * width_buffer = new unsigned char[cinfo.image_width * cinfo.input_components];
  while (cinfo.next_scanline < cinfo.image_height)
    {
      for (int j=0; j< cinfo.image_width * cinfo.input_components; ++j) width_buffer[j] = data[line_count * row_stride + j];
      row_pointer[0] = width_buffer;
      //row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
      ++line_count;
    }

  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);

  return true;
}
